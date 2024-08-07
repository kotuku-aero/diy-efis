/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "../../libs/neutron/bsp.h"

#include <stdint.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../../libs/photon/photon.h"

// data that is based on the actual framebuffer
static int fbfd;          // actual framebuffer

static handle_t worker;

extern void bsp_invalidate_framebuffer();
extern bool bsp_repaint_framebuffer();

// set to the screen framebuffer handle.
// if single bufferred will be /dev/fb0, double bufferred will be /dev/fb1
static int fbfd = -1;

const char *screen_x_s = "screen-x";
const char *screen_y_s = "screen-y";
const char *framebuffer_device_s = "screen";

struct _framebuffer_canvas_t;

typedef uint32_t (*pack_pixel_fn)(struct _framebuffer_canvas_t *canvas, color_t color);
typedef color_t (*unpack_pixel_fn)(struct _framebuffer_canvas_t *canvas, uint32_t pixel);
typedef void (*destroy_fn)(void *);

typedef struct _framebuffer_canvas_t {
  canvas_t canvas;
  uint8_t *buffer;
  bool owns;
  // this is a shift factor
  gdi_dim_t pixel_shift;  // 1 = 16bpp, 2 = 32bpp

  uint32_t red_mask;
  size_t red_shift;
  size_t red_offset;

  uint32_t blue_mask;
  size_t blue_shift;
  size_t blue_offset;

  uint32_t green_mask;
  size_t green_shift;
  size_t green_offset;

  uint32_t alpha_mask;
  size_t alpha_shift;
  size_t alpha_offset;

  pack_pixel_fn pack_pixel;
  unpack_pixel_fn unpack_pixel;
} framebuffer_canvas_t;

static point_t *apply_rotation(framebuffer_canvas_t *canvas, const point_t *src, point_t *dst);
static uint8_t *point_to_address(framebuffer_canvas_t *canvas, const point_t *pt);

static color_t bsp_get_pixel(canvas_t *canvas, const point_t *src);
static void bsp_set_pixel(canvas_t *canvas, const point_t *dest, color_t color);
static void bsp_fast_fill(canvas_t *canvas, const rect_t *dest, color_t fill_color);
static void bsp_fast_line(canvas_t *canvas, const point_t *p1, const point_t *p2, color_t fill_color);
static void bsp_fast_copy(canvas_t *canvas, const point_t *dest, const canvas_t *src_canvas, const rect_t *src);
static void bsp_copy_bitmap(canvas_t *canvas, const point_t *dest, const bitmap_t *src_bitmap, const rect_t *src);
static uint32_t bsp_pack_pixel(struct _framebuffer_canvas_t *canvas, color_t color);
static color_t bsp_unpack_pixel(framebuffer_canvas_t *canvas, uint32_t pixel);

static result_t init_canvas(framebuffer_canvas_t *canvas,
                            gdi_dim_t dx,
                            gdi_dim_t dy)
  {
  uint8_t *buffer = (uint8_t *)malloc(sizeof(color_t) * dx * dy);

  if(buffer == 0)
    return e_not_enough_memory;

  memset(canvas, 0, sizeof(framebuffer_canvas_t));

  canvas->canvas.version = sizeof(framebuffer_canvas_t);
  canvas->canvas.bits_per_pixel = sizeof(color_t);
  canvas->canvas.width = dx;
  canvas->canvas.height = dy;
  canvas->canvas.get_pixel = bsp_get_pixel;
  canvas->canvas.set_pixel = bsp_set_pixel;
  canvas->canvas.fast_fill = bsp_fast_fill;
  canvas->canvas.fast_line = bsp_fast_line;
  canvas->canvas.fast_copy = bsp_fast_copy;
  canvas->canvas.bitmap_copy = bsp_copy_bitmap;
  canvas->canvas.queue_empty = bsp_queue_empty;
  canvas->canvas.begin_paint = bsp_begin_paint;
  canvas->canvas.end_paint = bsp_end_paint;

  // note pack/unpack not used.

  canvas->red_mask = 0x00ff0000;
  canvas->red_shift = 0;
  canvas->red_offset = 16;

  canvas->blue_mask = 0x000000ff;
  canvas->blue_shift = 0;
  canvas->blue_offset = 0;

  canvas->green_mask = 0x0000ff00;
  canvas->green_shift = 0;
  canvas->green_offset = 8;

  canvas->alpha_mask = 0xff000000;
  canvas->alpha_shift = 0;
  canvas->alpha_offset = 24;

  canvas->buffer = buffer;
  canvas->owns = true;
  canvas->pixel_shift = 2;    // 32bpp

  return s_ok;
  }

result_t bsp_framebuffer_create_rect(const extent_t *size, canvas_t **canvas)
  {
  framebuffer_canvas_t *new_canvas = (framebuffer_canvas_t *)malloc(sizeof(framebuffer_canvas_t));

  if(new_canvas == 0)
    return e_not_enough_memory;

  if(init_canvas(new_canvas,
                 size->dx,
                 size->dy)!= s_ok)
    {
    free(new_canvas);
    return e_not_enough_memory;
    }

  *canvas = &new_canvas->canvas;

  return s_ok;
  }

result_t bsp_framebuffer_create_child(canvas_t *parent, const rect_t *rect, canvas_t **canvas)
  {
  if(parent->version != sizeof(framebuffer_canvas_t))
    return e_bad_parameter;

  framebuffer_canvas_t *parent_canvas = (framebuffer_canvas_t *)parent;

  // check the ranges
  if(parent_canvas->canvas.offset.x + rect->right > parent_canvas->canvas.width ||
      parent_canvas->canvas.offset.y + rect->bottom > parent_canvas->canvas.height ||
      parent_canvas->canvas.offset.x + rect->top < 0 ||
      parent_canvas->canvas.offset.y + rect->left < 0)
    return e_bad_parameter;       // rect exceeds bounds

  framebuffer_canvas_t *new_canvas = (framebuffer_canvas_t *)malloc(sizeof(framebuffer_canvas_t));

  if(new_canvas == 0)
    return e_not_enough_memory;

  memcpy(new_canvas, parent_canvas, sizeof(framebuffer_canvas_t));

  new_canvas->canvas.own_buffer = false;           // does not own the buffer
  new_canvas->canvas.offset.x += rect->left;     // offset is relative to the parent, not the canvas!
  new_canvas->canvas.offset.y += rect->top;

  *canvas = &new_canvas->canvas;

  return s_ok;
  }

result_t bsp_framebuffer_create_bitmap(const bitmap_t *bitmap, canvas_t **hndl)
  {
  rect_t rect;

  framebuffer_canvas_t *canvas = (framebuffer_canvas_t *)malloc(sizeof(framebuffer_canvas_t));

  if(canvas == 0)
    return e_not_enough_memory;

  if(init_canvas(canvas,
                 bitmap->bitmap_width,
                 bitmap->bitmap_height) != s_ok)
    {
    free(canvas);
    return e_not_enough_memory;
    }

  *hndl = &canvas->canvas;
  rect.left = 0;
  rect.top = 0;
  rect.bottom = bitmap->bitmap_height;
  rect.right = bitmap->bitmap_width;

  point_t dst_pt;
  dst_pt.x = rect.left;
  dst_pt.y = rect.top;

  bsp_copy_bitmap(&canvas->canvas, &dst_pt, bitmap, &rect);

  return s_ok;
  }

result_t bsp_framebuffer_close(canvas_t *hndl)
  {
  if(hndl->version != sizeof(framebuffer_canvas_t))
    return e_invalid_handle;

  framebuffer_canvas_t *canvas = (framebuffer_canvas_t *)hndl;
  if(canvas->owns)
  free(canvas->buffer);

  free(hndl);

  return s_ok;
  }

static point_t *apply_rotation(framebuffer_canvas_t *canvas, const point_t *src, point_t *dst)
  {
  gdi_dim_t temp;
  switch(canvas->canvas.orientation)
    {
    case 0 :
      copy_point(src, dst);
      break;
    case 90 :
      temp = src->y;
      dst->y = canvas->canvas.width - src->x -1;
      dst->x = temp;
      break;
    case 180 :
      temp = canvas->canvas.height - src->y -1;
      dst->y = canvas->canvas.width - src->x -1;
      dst->x = temp;
      break;
    case 270 :
      temp = canvas->canvas.width - src->y -1;
      dst->y = src->x;
      dst->x = temp;
      break;
    }

  return dst;
  }

/**
 * Return a color that blends the foreground and background
 * @param pixel Foreground color
 * @param back  Background color
 * @return blended color
 */
static color_t alpha_blend(color_t pixel, color_t back)
  {
  uint8_t weighting = alpha(pixel);

  // quick test of saturated color
  if(weighting == 255)
    return pixel;

  if(weighting == 0)
    return back;

  /* alpha blending the source and background colors */
  uint32_t rb = (((pixel & 0x00ff00ff) * weighting)
      + ((back & 0x00ff00ff) * (0xff - weighting))) & 0xff00ff00;
  uint32_t g = (((pixel & 0x0000ff00) * weighting)
      + ((back & 0x0000ff00) * (0xff - weighting))) & 0x00ff0000;

  return (pixel & 0xff000000) | ((rb | g) >> 8);
  }
/**
 * Calculate the address in a framebuffer for a point
 * @param canvas  Framebuffer that is to be de-referenced
 * @param pt      Point to locate
 * @return  Offset into the framebuffer of the pixel
 */
static uint8_t *point_to_address(framebuffer_canvas_t *canvas, const point_t *_pt)
  {
  point_t pt;
  // add the offset if the canvas has an offset
  pt.x = _pt->x + canvas->canvas.offset.x;
  pt.y = _pt->y + canvas->canvas.offset.y;

  // first rotate the point to ensure the offsets are correct
  point_t dest;

  apply_rotation(canvas, &pt, &dest);

  gdi_dim_t pixel_offset = 0;

  switch(canvas->canvas.orientation)
    {
  case 0 :
    pixel_offset = ((dest.y * canvas->canvas.width) + dest.x);
    break;
  case 90 :
    pixel_offset = (dest.y * canvas->canvas.height)  + dest.x;
    break;
  case 180 :
    pixel_offset = ((dest.y * canvas->canvas.width) - dest.x);
    break;
  case 270 :
    pixel_offset = (dest.y * -canvas->canvas.height)  + dest.x;
    break;
    }

  return canvas->buffer + (pixel_offset << canvas->pixel_shift);
  }

/**
 * Return a pixel packed into an opaque type
 * @param canvas    Framebuffer details
 * @param color     color to pack
 * @return packed pixel
 */
static uint32_t bsp_pack_pixel(framebuffer_canvas_t *canvas, color_t color)
  {
  uint32_t pixel = 0;
  pixel = ((red(color) >> canvas->red_shift)& canvas->red_mask) << canvas->red_offset;
  pixel |= ((blue(color) >> canvas->blue_shift) & canvas->blue_mask) << canvas->blue_offset;
  pixel |= ((green(color) >> canvas->green_shift) & canvas->green_mask) << canvas->green_offset;
  pixel |= ((alpha(color) >> canvas->alpha_shift) & canvas->alpha_mask) << canvas->alpha_offset;

  return pixel;
  }

/**
 * Unpack a packed pixel
 * @param canvas  Framebuffer details
 * @param pixel   pixel to unpack
 * @return color of pixel
 */
static color_t bsp_unpack_pixel(framebuffer_canvas_t *canvas, uint32_t pixel)
  {
  color_t red = ((pixel >> canvas->red_offset) & canvas->red_mask) << canvas->red_shift;
  color_t blue = ((pixel >> canvas->blue_offset) & canvas->blue_mask) << canvas->blue_shift;
  color_t green = ((pixel >> canvas->green_offset) & canvas->green_mask) << canvas->green_shift;
  color_t alpha = ((pixel >> canvas->alpha_offset) & canvas->alpha_mask) << canvas->alpha_shift;

  // see if a saturated color
  if(canvas->alpha_mask == 0)
    alpha = 0xff;

  return rgba(alpha, red, green, blue);
  }

/**
 * Internal routine to return a pixel
 * @param canvas  Canvas that references the pixel
 * @param src     Address in the framebuffer
 * @return Color reference of pixel.
 */
static color_t _get_pixel(framebuffer_canvas_t *canvas, const uint8_t *src)
	{
  uint32_t pixel = 0;
  if(canvas->pixel_shift == 1)
    pixel = *((const uint16_t *)src);
  else
    pixel = *((const uint32_t *)src);

  if(canvas->unpack_pixel != 0)
    return (canvas->unpack_pixel)(canvas, pixel);

  return (color_t) pixel;
	}

/**
 * Internal function to set a pixel.  Does not alpha blend.
 * @param canvas  Canvas referenced
 * @param dest    Pixel to set
 * @param color   Color to set
 */
static void _set_pixel(framebuffer_canvas_t *canvas, uint8_t *dest, color_t color)
  {
  gdi_dim_t bytes_per_pixel = 1 << canvas->pixel_shift;
  uint32_t pixel;
  if(canvas->pack_pixel != 0)
    pixel = (*canvas->pack_pixel)(canvas, color);
  else
    pixel = color;

  while(bytes_per_pixel--)
    {
    *dest++ = (uint8_t) pixel;
    pixel >>= 8;
    }
  }

static color_t bsp_get_pixel(canvas_t *hndl, const point_t *src)
  {
  if(hndl->version != sizeof(framebuffer_canvas_t))
    return color_hollow;

  framebuffer_canvas_t *canvas = (framebuffer_canvas_t *)hndl;

  // clip to canvas...
  if(src->x < 0 ||
      src->y < 0 ||
      src->x >= canvas->canvas.width ||
      src->y >= canvas->canvas.height)
    return color_hollow;

  return _get_pixel(canvas, point_to_address(canvas, src));
  }

void bsp_set_pixel(canvas_t *hndl,
                          const point_t *dest,
                          color_t color)
  {
  if(hndl->version != sizeof(framebuffer_canvas_t))
    return;

  framebuffer_canvas_t *canvas = (framebuffer_canvas_t *)hndl;

  // clip to canvas...
  if(dest->x < 0 ||
     dest->y < 0 ||
     dest->x >= canvas->canvas.width ||
     dest->y >= canvas->canvas.height)
    return;

  // alpha blend the color if it is not opaque
  if(alpha(color) != 255)
    color = alpha_blend(color, bsp_get_pixel(hndl, dest));

	_set_pixel(canvas, point_to_address(canvas, dest), color);
  }

void bsp_fast_fill(canvas_t *hndl,
                   const rect_t *dest,
                   color_t fill_color)
  {
  if(hndl->version != sizeof(framebuffer_canvas_t))
    return;

  point_t p1;
  top_left(dest, &p1);
  point_t p2;
  top_right(dest, &p2);

  while(p1.y < dest->bottom)
    {
    bsp_fast_line(hndl, &p1, &p2, fill_color);
    p1.y++;
    p2.y++;
    }
  }

void bsp_fast_line(canvas_t *hndl,
                          const point_t *p1,
                          const point_t *p2,
                          color_t fill_color)
  {
  int i;
  // 0 .. alpha blend
  // 1 .. saturated fill
  // -1 .. blackness
  int fill_mode = 0;
  uint8_t alpha_fill = alpha(fill_color);

  if(hndl->version != sizeof(framebuffer_canvas_t))
    return;

  if(p1->y != p2->y)
    return;

  framebuffer_canvas_t *canvas = (framebuffer_canvas_t *)hndl;

  if(p1->y < 0 || p1->y >= canvas->canvas.height)
    return;

  if(alpha_fill == 0)
    return;

  point_t _p1;
  point_t _p2;

  copy_point(p1, &_p1);
  copy_point(p2, &_p2);

  if(_p1.x > _p2.x)
    swap_points(&_p1, &_p2);

  if(_p1.x < 0)
    _p1.x = 0;

  if(_p1.x > canvas->canvas.width)
    _p1.x = canvas->canvas.width;

  if(_p2.x > canvas->canvas.width)
    _p2.x = canvas->canvas.width;

  p1 = &_p1;
  p2 = &_p2;

  gdi_dim_t pixels = p2->x - p1->x;
  if(pixels <= 0)
    return;

  // special case for blackness
  if(fill_color == color_black)
    fill_mode = -1;
  else
    // see if the mode is saturated
    fill_mode = alpha(fill_color) == 255 ? 1 : 0;

  gdi_dim_t bytes_per_pixel = 1 << canvas->pixel_shift;


  uint32_t packed_color = 0;

  // if we have a saturated fill then we can just pack the color in
  if(fill_mode != 0)
    {
    if(canvas->pack_pixel != 0)
      packed_color = (*canvas->pack_pixel)(canvas, fill_color);
    else
      packed_color = fill_color;
    }
  gdi_dim_t pixel_incr;

  switch(canvas->canvas.orientation)
    {
  case 0:
    pixel_incr = bytes_per_pixel;
    break;
  case 90:
    pixel_incr = -(canvas->canvas.height << canvas->pixel_shift);
    break;
  case 180:
    pixel_incr = -bytes_per_pixel;
    break;
  case 270:
    pixel_incr = (canvas->canvas.height << canvas->pixel_shift);
    break;
    }

  // get our starting pixel address
  point_t src_pix;
  copy_point(p1, &src_pix);

  uint8_t *dest = point_to_address(canvas, &src_pix);

  while(pixels--)
    {
    uint32_t pix = packed_color;
    if(fill_mode == 0)
      {
      color_t alpha_color = alpha_blend(fill_color, _get_pixel(canvas, dest));
      // the pixel to store is the alpha blended color
      if(canvas->pack_pixel != 0)
        pix = (*canvas->pack_pixel)(canvas, alpha_color);
      else
        pix = alpha_color;
      }

    for(i = 0; i < bytes_per_pixel; i++)
      {
      dest[i] = (uint8_t) pix;
      pix >>= 8;
      }

    dest += pixel_incr;
    // and assume a greater address.
    src_pix.x++;
    }
  }

void bsp_fast_copy(canvas_t *hndl,
                   const point_t *dest_pt,
                   const canvas_t *src_hndl,
                   const rect_t *src_rect)
  {
  if(hndl->version != sizeof(framebuffer_canvas_t) ||
      src_hndl->version != sizeof(framebuffer_canvas_t))
    return;

  framebuffer_canvas_t *dst_canvas = (framebuffer_canvas_t *)hndl;
  gdi_dim_t dst_incr;

  switch(dst_canvas->canvas.orientation)
    {
  case 0:
    dst_incr = 1 << dst_canvas->pixel_shift;
    break;
  case 90:
    dst_incr = -(dst_canvas->canvas.height << dst_canvas->pixel_shift);
    break;
  case 180:
    dst_incr = -(1 << dst_canvas->pixel_shift);
    break;
  case 270:
    dst_incr = (dst_canvas->canvas.height << dst_canvas->pixel_shift);
    break;
    }

  framebuffer_canvas_t *src_canvas = (framebuffer_canvas_t *)src_hndl;
  gdi_dim_t src_incr;

  switch(src_canvas->canvas.orientation)
    {
  case 0:
    src_incr = 1 << src_canvas->pixel_shift;
    break;
  case 90:
    src_incr = -(src_canvas->canvas.height << src_canvas->pixel_shift);
    break;
  case 180:
    src_incr = -(1 << src_canvas->pixel_shift);
    break;
  case 270:
    src_incr = (src_canvas->canvas.height << src_canvas->pixel_shift);
    break;
    }

  gdi_dim_t x_pixels = rect_width(src_rect);
  gdi_dim_t y_pixels = rect_height(src_rect);
  point_t src_pt;
  point_t dst_pt;

  // get our point correct
  top_left(src_rect, &src_pt);
  copy_point(dest_pt, &dst_pt);

  while(y_pixels--)
    {
    gdi_dim_t x;
    uint8_t *src_line = point_to_address(src_canvas, &src_pt);
    uint8_t *dst_line = point_to_address(dst_canvas, &dst_pt);

    for(x = 0; x < x_pixels; x++)
      {
      _set_pixel(dst_canvas,
                 dst_line,
                 alpha_blend(_get_pixel(src_canvas, src_line),
                             _get_pixel(dst_canvas, dst_line)));

      // add the byte offset for the pixel
      src_line += src_incr;
      dst_line += dst_incr;
      }

    src_pt.y++;
    dst_pt.y++;
    }
  }

void bsp_copy_bitmap(canvas_t *hndl,
                     const point_t *dest_pt,
                     const bitmap_t *src_bitmap,
                     const rect_t *src_rect)
  {
  if(hndl->version != sizeof(framebuffer_canvas_t))
    return;

  framebuffer_canvas_t *dst_canvas = (framebuffer_canvas_t *)hndl;
  gdi_dim_t dst_incr;

  switch(dst_canvas->canvas.orientation)
    {
  case 0:
    dst_incr = 1 << dst_canvas->pixel_shift;
    break;
  case 90:
    dst_incr = -(dst_canvas->canvas.height << dst_canvas->pixel_shift);
    break;
  case 180:
    dst_incr = -(1 << dst_canvas->pixel_shift);
    break;
  case 270:
    dst_incr = (dst_canvas->canvas.height << dst_canvas->pixel_shift);
    break;
    }

  gdi_dim_t width = rect_width(src_rect);
  gdi_dim_t height = rect_height(src_rect);
  point_t src_pt;
  point_t dst_pt;

  gdi_dim_t src_y_offset = 0;

  top_left(src_rect, &src_pt);
  copy_point(dest_pt, &dst_pt);

  while(height--)
    {
    gdi_dim_t x;
    uint8_t *dst_line = point_to_address(dst_canvas, &dst_pt);
    for(x = 0; x < width; x++)
      {
      // set the destination but alpha blend the bitmap color.
      _set_pixel(dst_canvas, dst_line,
          alpha_blend(src_bitmap->pixels[src_pt.x + src_y_offset],
                      _get_pixel(dst_canvas, dst_line)));

      // add another pixel to the destination offset
      dst_line += dst_incr;
      }

    dst_pt.y++;
    src_pt.y++;
    }
  }

static bool is_emulator = false;

static void fb_run(void *parg)
  {
  handle_t semp;
  if(is_emulator)
    semaphore_create(&semp);
  while(true)
    {
    if(is_emulator)
      wait(semp, 30);       // 30hz update
    else
      {
      // we assume the device is 0
      int arg = 0;
      ioctl(fbfd, FBIO_WAITFORVSYNC, &arg);
      }

    bsp_sync();
    }
  }

// this is the actual memory mapped framebuffer
static uint8_t *framebuffer;
static uint8_t *draw_buffer;
static uint32_t fb_length;

result_t bsp_canvas_open_framebuffer(canvas_t **canvas)
  {
  result_t result;
  uint16_t x;
  uint16_t y;
  uint16_t length = REG_STRING_MAX + 1;
  char device[REG_STRING_MAX + 1];
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;


  memid_t key;
  if(failed(result = reg_open_key(0, "electron", &key)))
    return result;

  if(failed(result = reg_get_uint16(key, screen_x_s, &x)))
    x = 320;

  if(failed(result = reg_get_uint16(key, screen_y_s, &y)))
    y = 240;

  if(failed(result = reg_get_string(key, framebuffer_device_s, device, &length)))
    strcpy(device, "/tmp/fb0");

  // try to open the actual framebuffer.
  // If this is a single-bufferred screen then it will usually be /dev/fb0
  // if double buffered then it will

  // determine if we are double-buffered
  // open the framebuffer device
  fbfd = open(device, O_RDWR);
  int pixel_increment = 4;

  if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) != -1)
    {
    vinfo.grayscale = 0;
    ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);

    //Get fixed screen information
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);

    pixel_increment = vinfo.bits_per_pixel >> 3;
    }
  else
    {
    is_emulator = true;
    memset(&vinfo, 0, sizeof(vinfo));
    memset(&finfo, 0, sizeof(finfo));

    // handle operation on the emulation platform
    vinfo.bits_per_pixel = 32;
    vinfo.red.length = 8;
    vinfo.red.offset = 16;

    vinfo.green.length = 8;
    vinfo.green.offset = 8;

    vinfo.blue.length = 8;
    vinfo.blue.offset = 0;

    vinfo.transp.length = 0;
    vinfo.transp.offset = 0;

    vinfo.xres = x;
    vinfo.yres = y;

    pixel_increment = vinfo.bits_per_pixel >> 3;
    }

  // calc bytes to store the values
  fb_length = vinfo.xres * vinfo.yres * pixel_increment;

  // map the device to memory
  framebuffer = (uint8_t *) mmap(0, fb_length, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

  draw_buffer = (uint8_t *)malloc(fb_length);

  memset(draw_buffer, 0, fb_length);

  // create the thread first
  if(failed(result = task_create("FB", 4096, fb_run, 0, NORMAL_PRIORITY, &worker)) ||
      failed(result = task_resume(worker)))
    return result;

  framebuffer_canvas_t *fbc = (framebuffer_canvas_t *) malloc(sizeof(framebuffer_canvas_t));

  memset(fbc, 0, sizeof(framebuffer_canvas_t));
  fbc->canvas.version = sizeof(framebuffer_canvas_t);
  fbc->canvas.bits_per_pixel = vinfo.bits_per_pixel;
  fbc->canvas.width = vinfo.xres;
  fbc->canvas.height = vinfo.yres;
  fbc->canvas.get_pixel = bsp_get_pixel;
  fbc->canvas.set_pixel = bsp_set_pixel;
  fbc->canvas.fast_fill = bsp_fast_fill;
  fbc->canvas.fast_line = bsp_fast_line;
  fbc->canvas.fast_copy = bsp_fast_copy;
  fbc->canvas.bitmap_copy = bsp_copy_bitmap;
  fbc->canvas.queue_empty = bsp_queue_empty;
  fbc->canvas.begin_paint = bsp_begin_paint;
  fbc->canvas.end_paint = bsp_end_paint;
  fbc->canvas.own_buffer = true;

  fbc->pack_pixel = bsp_pack_pixel;
  fbc->unpack_pixel = bsp_unpack_pixel;

  fbc->red_mask = (1 << vinfo.red.length)-1;
  fbc->blue_mask = (1 << vinfo.blue.length)-1;
  fbc->green_mask = (1 << vinfo.green.length)-1;
  fbc->alpha_mask = (1 << vinfo.transp.length)-1;

  fbc->red_shift = 8 - vinfo.red.length;
  fbc->blue_shift = 8 - vinfo.blue.length;
  fbc->green_shift = 8 - vinfo.green.length;
  fbc->alpha_shift = 8 - vinfo.transp.length;

  fbc->red_offset = vinfo.red.offset;
  fbc->green_offset = vinfo.green.offset;
  fbc->blue_offset = vinfo.blue.offset;
  fbc->alpha_offset = vinfo.transp.offset;

  fbc->buffer = draw_buffer;
  fbc->owns = false;
  fbc->pixel_shift = fbc->canvas.bits_per_pixel == 16 ? 1 : 2;

  // save the root canvas
  *canvas = &fbc->canvas;

  return s_ok;
  }

result_t bsp_sync_framebuffer()
  {
  memcpy(framebuffer, draw_buffer, fb_length);

  bsp_sync_done();
  }
