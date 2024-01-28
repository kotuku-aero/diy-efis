#define handle_t win_handle_t
#include <Windows.h>
#include <ddraw.h>
#include <stdio.h>
#undef handle_t

#include "../../libs/neutron/neutron.h"
#include "../../libs/photon/photon_priv.h"

#define BITS_PER_PIXEL 32
#define PIXELS_PER_LONG 1

handle_t h_worker;

extern screen_t* _screen;


static framebuffer_t* direct_draw_fb = 0;

static framebuffer_t* background_fb = 0;
static framebuffer_t* foreground_fb = 0;
static framebuffer_t* overlay_fb = 0;

static handle_t draw_canvas = 0;

typedef struct _krypton_framebuffer_t krypton_framebuffer_t;

typedef uint32_t(*pack_pixel_fn)(krypton_framebuffer_t* fb, color_t color);
typedef color_t(*unpack_pixel_fn)(krypton_framebuffer_t* fb, uint32_t pixel);
typedef void (*destroy_fn)(void*);

extern const typeid_t framebuffer_type;
// defined in canvas.c so embedded and emulator both work the same

typedef struct _krypton_framebuffer_t {
  framebuffer_t base;

  union {
    framebuffer_t* parent_fb; // if this is != 0 then the canvas does not own the buffer
    uint8_t* buffer;          // the pixel buffer (only valid if this is the owner of the buffer
    } fb;

  bool release_buffer;

  uint16_t orientation;     // 0, 90, 180, 270
  uint16_t bits_per_pixel;  // number of bits in a pixel
  // this is a shift factor
  int32_t pixel_shift;  // 1 = 16bpp, 2 = 32bpp

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
  } krypton_framebuffer_t;

/**
 * @brief      return the pixel buffer that has the canvas draws on
 * @param fb   framebuffer
 * @return pointer to the buffer
*/
static inline uint8_t* get_framebuffer(krypton_framebuffer_t* fb)
  {
  while (!fb->base.is_surface)
    fb = (krypton_framebuffer_t*)fb->fb.parent_fb;

  return fb->fb.buffer;
  }
/**
 * @brief Get the framebuffer that holds the pixel buffer
 * @param fb framebuffer to query
 * @return root buffer
*/
static inline krypton_framebuffer_t* root_framebuffer(krypton_framebuffer_t* fb)
  {
  while (!fb->base.is_surface)
    fb = (krypton_framebuffer_t*)fb->fb.parent_fb;

  return fb;
  }
/**
 * @brief Ensure the rectange is clipped to the actual framebuffer rectangle
 * @param fb    framebuffer
 * @param rect  rectangle to clip
 * @return true if the rectangle is within the framebuffer
*/
static bool clip_to_framebuffer(krypton_framebuffer_t* fb, rect_t* rect)
  {
  while (!fb->base.is_surface)
    fb = (krypton_framebuffer_t*)fb->fb.parent_fb;

  if (rect->top < fb->base.position.top)
    rect->top = fb->base.position.top;

  if (rect->right > fb->base.position.right)
    rect->right = fb->base.position.right;

  if (rect->left < fb->base.position.left)
    rect->left = fb->base.position.left;

  if (rect->bottom > fb->base.position.bottom)
    rect->bottom = fb->base.position.bottom;

  return rect->left < fb->base.position.right &&
    rect->top < fb->base.position.bottom &&
    rect->right > fb->base.position.left &&
    rect->bottom > fb->base.position.top;
  }
/**
 * @brief Convert a window point to a screen point
 * @param fb    Framebuffer to refer to
 * @param src   source point
 * @param dst   destination
 * @return destination
*/
static inline const point_t* point_to_surface(krypton_framebuffer_t* fb, const point_t* src, point_t* dst)
  {
  dst->x = src->x + fb->base.position.left;
  dst->y = src->y + fb->base.position.top;

  return dst;
  }
/**
 * @brief Convert a window position to an absolute position
 * @param fb    framebuffer
 * @param src   source rectangle
 * @param dst   calculated rectangle
 * @return dst
*/
static inline const rect_t* to_framebuffer_position(krypton_framebuffer_t* fb, const rect_t* src, rect_t* dst)
  {
  rect_copy(src, dst);
  dst->left += fb->base.position.left;
  dst->top += fb->base.position.top;
  dst->right += fb->base.position.left;
  dst->bottom += fb->base.position.top;

  return dst;
  }

static point_t* apply_rotation(krypton_framebuffer_t* fb, const point_t* src, point_t* dst);
static uint8_t* point_to_address(krypton_framebuffer_t* fb, const point_t* pt);

static result_t bsp_get_pixel(framebuffer_t* fb, const point_t* src, color_t* pix);
static result_t bsp_set_pixel(framebuffer_t* fb, const point_t* dest, color_t color, color_t* pix);
static result_t bsp_fast_fill(framebuffer_t* fb, const rect_t* dest, color_t fill_color);
static result_t bsp_fast_line(framebuffer_t* fb, const point_t* p1, const point_t* p2, color_t fill_color);
static result_t bsp_fast_copy(framebuffer_t* fb, const point_t* dest, const framebuffer_t* src_canvas, const rect_t* src, raster_operation op);
static result_t bsp_copy_bitmap(framebuffer_t* fb, const point_t* dest, const bitmap_t* src_bitmap, const rect_t* src);

extern bool clip_line(point_t* p1, point_t* p2, const rect_t* clip_rect);
/**
 * @brief create a framebuffer that the gdi owns
 * @param fb    framebuffer to init
 * @param dx    dimensions to allocate x
 * @param dy    dimensions to allocate y
 * @return s_ok if created ok
*/
static result_t init_framebuffer(krypton_framebuffer_t* fb,
  int32_t dx,
  int32_t dy)
  {
  result_t result;
  uint8_t* buffer;

  if (failed(result = neutron_malloc(sizeof(color_t) * dx * dy, (void**)&buffer)))
    return result;

  memset(fb, 0, sizeof(krypton_framebuffer_t));

  fb->bits_per_pixel = sizeof(color_t) << 3;
  fb->base.position.left = 0;
  fb->base.position.top = 0;
  fb->base.position.right = dx;
  fb->base.position.bottom = dy;
  fb->base.get_pixel = bsp_get_pixel;
  fb->base.set_pixel = bsp_set_pixel;
  fb->base.fast_fill = bsp_fast_fill;
  fb->base.fast_line = bsp_fast_line;
  fb->base.fast_copy = bsp_fast_copy;
  fb->base.queue_empty = window_queue_empty;
  fb->base.begin_paint = window_begin_paint;
  fb->base.end_paint = window_end_paint;

  // note pack/unpack not used.

  fb->red_mask = 0x00ff0000;
  fb->red_shift = 0;
  fb->red_offset = 16;

  fb->blue_mask = 0x000000ff;
  fb->blue_shift = 0;
  fb->blue_offset = 0;

  fb->green_mask = 0x0000ff00;
  fb->green_shift = 0;
  fb->green_offset = 8;

  fb->alpha_mask = 0xff000000;
  fb->alpha_shift = 0;
  fb->alpha_offset = 24;
  fb->pixel_shift = 2;    // 32bpp

  fb->fb.buffer = buffer;
  fb->base.is_surface = true;

  return s_ok;
  }

result_t bsp_framebuffer_create_rect(const extent_t* size, framebuffer_t** fb)
  {
  result_t result;
  krypton_framebuffer_t* _fb;

  if (failed(result = neutron_malloc(sizeof(krypton_framebuffer_t), (void**)&_fb)))
    return result;

  if (init_framebuffer(_fb, size->dx, size->dy) != s_ok)
    {
    neutron_free(_fb);
    return e_not_enough_memory;
    }

  *fb = &_fb->base;

  return s_ok;
  }

/**
 * @brief The emulator packs the pixel into windows format
 * @param fb framebuffer
 * @param color color in RGBA
 * @return packed to ARGB
*/
static color_t pack_pixel(struct _krypton_framebuffer_t* fb, color_t color)
  {
  return (alpha(color) << 24) | (red(color) << 16) | (green(color) << 8) | blue(color);
  }
/**
 * @brief Unpack a pixel from windows to canfly format
 * @param fb    framebuffer
 * @param color color to unpack
 * @return
*/
static color_t unpack_pixel(struct _krypton_framebuffer_t* fb, color_t color)
  {
  return rgba((color >> 24) & 0xff, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
  }

result_t bsp_set_primary(framebuffer_t* _fb, uint8_t* buffer)
  {
  krypton_framebuffer_t* fb = (krypton_framebuffer_t*)_fb;

  if (fb->release_buffer)
    neutron_free(fb->fb.buffer);   // release the old canvas

  // set pixel mappers
  fb->pack_pixel = pack_pixel;
  fb->unpack_pixel = unpack_pixel;

  fb->fb.buffer = buffer;
  fb->release_buffer = false;             // hardware issued canvas

  return s_ok;
  }
/**
 * @brief create a child window canvas
 * @param parent  parent of the canvas
 * @param rect    relative to the parent
 * @param _fb     resulting canvas
 * @return
*/
result_t bsp_framebuffer_create_child(framebuffer_t* parent, const rect_t* rect, framebuffer_t** _fb)
  {
  result_t result;
  krypton_framebuffer_t* parent_fb = (krypton_framebuffer_t*)parent;

  // check the ranges.  The window must be completely within its parent.
  if (rect->left < 0 || rect->left > rect_width(&parent_fb->base.position) ||
    rect->top < 0 || rect->top > rect_height(&parent_fb->base.position) ||
    rect->right < rect->left || rect->right > rect_width(&parent_fb->base.position) ||
    rect->bottom > rect_height(&parent_fb->base.position))
    return e_bad_parameter;       // rect exceeds bounds

  krypton_framebuffer_t* fb;
  if (failed(result = neutron_malloc(sizeof(krypton_framebuffer_t), (void**)&fb)))
    return result;

  memcpy(fb, parent_fb, sizeof(krypton_framebuffer_t));

  to_framebuffer_position(parent_fb, rect, &fb->base.position);      // store the rectange as relative to the canvas

  fb->base.is_surface = false;           // does not own the buffer
  fb->fb.parent_fb = &parent_fb->base;

  //trace_info("Create child { %d, %d, %d, %d }\n", fb->base.position.left,
  //  fb->base.position.top, fb->base.position.right, fb->base.position.bottom);

  *_fb = &fb->base;

  return s_ok;
  }

result_t bsp_framebuffer_create_bitmap(const bitmap_t* bitmap, framebuffer_t** hndl)
  {
  rect_t rect;
  result_t result;
  krypton_framebuffer_t* fb;
  if (failed(result = neutron_malloc(sizeof(krypton_framebuffer_t), (void**)&fb)))
    return result;

  if (fb == 0)
    return e_not_enough_memory;

  // create an owned bitmap.
  if (init_framebuffer(fb, bitmap->bitmap_width, bitmap->bitmap_height) != s_ok)
    {
    neutron_free(fb);
    return e_not_enough_memory;
    }

  *hndl = &fb->base;
  rect.left = 0;
  rect.top = 0;
  rect.bottom = bitmap->bitmap_height;
  rect.right = bitmap->bitmap_width;

  point_t dst_pt;
  dst_pt.x = rect.left;
  dst_pt.y = rect.top;

  bsp_copy_bitmap(&fb->base, &dst_pt, bitmap, &rect);

  return s_ok;
  }

result_t bsp_framebuffer_close(framebuffer_t* hndl)
  {
  krypton_framebuffer_t* fb = (krypton_framebuffer_t*)hndl;

  if (fb->release_buffer)
    neutron_free(fb->fb.buffer);

  neutron_free(hndl);

  return s_ok;
  }

static point_t* apply_rotation(krypton_framebuffer_t* fb, const point_t* src, point_t* dst)
  {
  int32_t temp;
  switch (fb->orientation)
    {
    case 0:
      point_copy(src, dst);
      break;
    case 90:
      temp = src->y;
      dst->y = rect_width(&root_framebuffer(fb)->base.position) - src->x - 1;
      dst->x = temp;
      break;
    case 180:
      temp = rect_height(&root_framebuffer(fb)->base.position) - src->y - 1;
      dst->y = rect_width(&root_framebuffer(fb)->base.position) - src->x - 1;
      dst->x = temp;
      break;
    case 270:
      temp = rect_width(&root_framebuffer(fb)->base.position) - src->y - 1;
      dst->y = src->x;
      dst->x = temp;
      break;
    }

  return dst;
  }

/**
 * Calculate the address in a fb_buffer for a point
 * @param canvas  Framebuffer that is to be de-referenced
 * @param pt      Point to locate
 * @return  Offset into the fb_buffer of the pixel
 */
static uint8_t* point_to_address(krypton_framebuffer_t* fb, const point_t* _pt)
  {
  point_t pt;
  point_to_surface(fb, _pt, &pt);

  // first rotate the point to ensure the offsets are correct
  point_t dest;

  apply_rotation(fb, &pt, &dest);

  int32_t pixel_offset = 0;

  switch (fb->orientation)
    {
    case 0:
      pixel_offset = (((int32_t)(dest.y * root_framebuffer(fb)->base.position.right)) + ((int32_t)dest.x));
      break;
    case 90:
      pixel_offset = ((int32_t)(dest.y * root_framebuffer(fb)->base.position.bottom)) + ((int32_t)dest.x);
      break;
    case 180:
      pixel_offset = (((int32_t)(dest.y * root_framebuffer(fb)->base.position.right)) - ((int32_t)dest.x));
      break;
    case 270:
      pixel_offset = (((int32_t)(dest.y * 0 - root_framebuffer(fb)->base.position.bottom)) + ((int32_t)dest.x));
      break;
    }

  return get_framebuffer(fb) + (pixel_offset << fb->pixel_shift);
  }


static color_t alpha_blend(color_t pixel, color_t back)
  {
  uint16_t alpha_pixel = alpha(pixel);
  // quick test of saturated color
  if (alpha_pixel == 255)
    return pixel;

  if (alpha_pixel == 0)
    return back;

  // move RGB to RGB as low bytes
  // loses the aplha color but makes space for the multiply
  pixel >>= 8;
  back >>= 8;

  // make 1..256
  uint16_t pixel_saturation = alpha_pixel + 1;
  uint16_t back_saturation = 256 - pixel_saturation;

  /* alpha blending the source and background colors */
  uint32_t rb = (((pixel & 0x00ff00ff) * pixel_saturation) + ((back & 0x00ff00ff) * back_saturation)) & 0xff00ff00;
  uint32_t g = (((pixel & 0x0000ff00) * pixel_saturation) + ((back & 0x0000ff00) * back_saturation)) & 0x00ff0000;

  // return as saturated color as alpha blend already done.
  return 0x000000ff | (rb | g);
  }


/**
 * Internal routine to return a pixel
 * @param canvas  Canvas that references the pixel
 * @param src     Address in the fb_buffer
 * @return Color reference of pixel.
 */
static color_t _get_pixel(krypton_framebuffer_t* fb, const uint8_t* src)
  {
  uint32_t pixel = *((const uint32_t*)src);

  if (fb->unpack_pixel != 0)
    return fb->unpack_pixel(fb, pixel);

  return (color_t)pixel;
  }

/**
 * Internal function to set a pixel.  Does not alpha blend.
 * @param canvas  Canvas referenced
 * @param dest    Pixel to set
 * @param color   Color to set
 */
static void _set_pixel(krypton_framebuffer_t* fb, uint8_t* dest, color_t color)
  {
  if (fb->pack_pixel != 0)
    color = fb->pack_pixel(fb, color);

  //color_t back_color = *((uint32_t*)dest);

  //*((uint32_t*)dest) = alpha_blend(color, back_color);
  *((uint32_t*)dest) = color;
  }

/**
 * Internal function to set a pixel.
 * @param canvas  Canvas referenced
 * @param dest    Pixel to set
 * @param color   Color to set
 */
static void _set_pixel_alpha(krypton_framebuffer_t* fb, uint8_t* dest, color_t color)
  {
  if (fb->pack_pixel != 0)
    color = fb->pack_pixel(fb, color);

  color_t back_color = *((uint32_t*)dest);

  *((uint32_t*)dest) = alpha_blend(color, back_color);
  }


static result_t bsp_get_pixel(framebuffer_t* hndl, const point_t* src, color_t* pix)
  {
  if (src == 0 || pix == 0)
    return e_bad_pointer;

  krypton_framebuffer_t* fb = (krypton_framebuffer_t*)hndl;

  // clip to canvas...
  if (src->x < 0 ||
    src->y < 0 ||
    src->x >= rect_width(&fb->base.position) ||
    src->y >= rect_height(&fb->base.position))
    return e_bad_parameter;

  *pix = _get_pixel(fb, point_to_address(fb, src));

  return s_ok;
  }

result_t bsp_set_pixel(framebuffer_t* hndl,
  const point_t* dest,
  color_t color,
  color_t* pix)
  {
  krypton_framebuffer_t* fb = (krypton_framebuffer_t*)hndl;

  // clip to canvas...
  if (dest->x < 0 ||
    dest->y < 0 ||
    dest->x >= rect_width(&fb->base.position) ||
    dest->y >= rect_height(&fb->base.position))
    return e_bad_parameter;

  _set_pixel(fb, point_to_address(fb, dest), color);
  return s_ok;
  }

static result_t bsp_fast_fill(framebuffer_t* hndl, const rect_t* dest, color_t fill_color)
  {
  point_t p1;
  rect_top_left(dest, &p1);
  point_t p2;
  rect_top_right(dest, &p2);

  krypton_framebuffer_t* fb = (krypton_framebuffer_t*)hndl;


  while (p1.y < dest->bottom)
    {
    point_t pix = { p1.x, p1.y };

    for (; pix.x < p2.x; pix.x++)
      _set_pixel(fb, point_to_address(fb, &pix), fill_color);

    p1.y++;
    p2.y++;
    }

  return s_ok;
  }

result_t bsp_fast_line(framebuffer_t* _fb, const point_t* _p1, const point_t* _p2, color_t fill_color)
  {
  int fill_mode = 0;
  //trace_info("fast_line( (%d, %d), (%d, %d) )\n", _p1->x, _p1->y, _p2->x, _p2->y);

  krypton_framebuffer_t* fb = (krypton_framebuffer_t*)_fb;

  rect_t clip_rect = { 0, 0, rect_width(&fb->base.position), rect_height(&fb->base.position) };

  point_t p1;
  point_copy(_p1, &p1);
  point_t p2;
  point_copy(_p2, &p2);

  //trace_info("fast_line_s( (%d, %d), (%d, %d )\n", p1.x, p1.y, p2.x, p2.y);


  // clip the line to the clipping area
  if (!clip_line(&p1, &p2, &clip_rect))
    {
    //trace_info("Clipped\n");
    return s_ok;                 // line is outside the clipping area
    }

  // ensure the line is always top->bottom
  if (p1.y > p2.y)
    point_swap(&p1, &p2);

  int32_t delta_x = p2.x - p1.x;
  int32_t delta_y = p2.y - p1.y;

  // draw the first pixel
  if (rect_contains(&clip_rect, &p1))
    {
    color_t back_color;
    (*fb->base.get_pixel)(&fb->base, &p1, &back_color);

    (*fb->base.set_pixel)(&fb->base, &p1, alpha_blend(fill_color, back_color), 0);
    }

  int32_t x_incr;
  if (delta_x >= 0)
    x_incr = 1;
  else
    {
    x_incr = -1;
    delta_x = -delta_x;
    }

  // we can optimize the drawing of horizontal and vertical lines
  if (delta_x == 0)
    {
    // ensure the line is always top->bottom
    if (p1.y > p2.y)
      {
      point_swap(&p1, &p2);

      // however this causes a draw issue as the last point is not drawn!
      p1.y++;
      p2.y++;
      }

    while (p1.y < p2.y)
      {
      if (rect_contains(&clip_rect, &p1))
        {
        color_t back_color = (*fb->base.get_pixel)(&fb->base, &p1, 0);

        if (rect_contains(&clip_rect, &p1))
          (*fb->base.set_pixel)(&fb->base, &p1,
            alpha_blend(fill_color, back_color), 0);
        }

      p1.y++;
      }
    }
  else if (delta_y == 0)
    {
    if (p1.x > p2.x)
      {
      point_swap(&p1, &p2);
      p1.x++;
      p2.x++;
      }

    while (p1.x < p2.x)
      {
      if (rect_contains(&clip_rect, &p1))
        {
        color_t back_color = (*fb->base.get_pixel)(&fb->base, &p1, 0);

        if (rect_contains(&clip_rect, &p1))
          (*fb->base.set_pixel)(&fb->base, &p1,
            alpha_blend(fill_color, back_color), 0);
        }

      p1.x++;
      }
    }
  else if (delta_y == delta_x)
    {
    // ensure the line is always top->bottom
    if (p1.y > p2.y)
      point_swap(&p1, &p2);

    do
      {
      if (rect_contains(&clip_rect, &p1))
        {
        color_t back_color = (*fb->base.get_pixel)(&fb->base, &p1, 0);

        if (rect_contains(&clip_rect, &p1))
          (*fb->base.set_pixel)(&fb->base, &p1,
            alpha_blend(fill_color, back_color), 0);
        }

      p1.x += x_incr;
      p1.y++;

      } while (--delta_y > 0);
    }
  else if (delta_y + delta_x != 0)   // more than 1 pixel, or a wide line
    {

    int16_t intensity_shift = 4;
    int16_t weighting_complement_mask = 0xFF;
    int16_t error_adj;                // intensity to weight color by
    int16_t error_acc = 0;
    int16_t weighting;
    int16_t error_acc_temp;
    point_t p_alias;
    //------------------------------------------------------------------------
    // determine independent variable (one that always increments by 1 (or -1) )
    // and initiate appropriate line drawing routine (based on first octant
    // always). the x and y's may be flipped if y is the independent variable.
    //------------------------------------------------------------------------
    if (delta_y > delta_x)
      {
      /* Y-major line; calculate 16-bit fixed-point fractional part of a
        pixel that X advances each time Y advances 1 pixel, truncating the
        result so that we won't overrun the endpoint along the X axis */
      error_adj =
        (uint16_t)((((int32_t)delta_x) << 16) / (int32_t)delta_y);

      while (--delta_y) // process each point in the line one at a time (just use delta_y)
        {
        error_acc_temp = error_acc;  // remember the current accumulated error
        error_acc += error_adj;             // calculate error for next pixel

        if (error_acc <= error_acc_temp)
          p1.x += x_incr;

        p1.y++;                             // increment independent variable

        // if pen width > 1 then we use a modified algorithm
        weighting = error_acc >> intensity_shift;

        if (rect_contains(&clip_rect, &p1))
          (*fb->base.set_pixel)(&fb->base, &p1,
            alpha_blend(rgb_alpha(fill_color, weighting), (*fb->base.get_pixel)(&fb->base, &p1, 0)), 0); // plot the pixel

        p_alias.x = p1.x + x_incr;
        p_alias.y = p1.y;

        if (rect_contains(&clip_rect, &p_alias))
          (*fb->base.set_pixel)(&fb->base, &p_alias,
            alpha_blend(rgb_alpha(fill_color, (weighting ^ weighting_complement_mask)),
              (*fb->base.get_pixel)(&fb->base, &p_alias, 0)), 0);
        }
      }
    else
      {
      error_adj =
        (uint16_t)((((int32_t)delta_y) << 16) / (int32_t)delta_x);

      while (--delta_x) // process each point in the line one at a time (just use delta_y)
        {
        error_acc_temp = error_acc;  // remember the current accumulated error
        error_acc += error_adj;             // calculate error for next pixel

        if (error_acc <= error_acc_temp)
          p1.y++;

        p1.x += x_incr;                     // increment independent variable

        weighting = error_acc >> intensity_shift;

        if (rect_contains(&clip_rect, &p1))
          (*fb->base.set_pixel)(&fb->base, &p1,
            alpha_blend(rgb_alpha(fill_color, weighting), (*fb->base.get_pixel)(&fb->base, &p1, 0)), 0); // plot the pixel

        p_alias.x = p1.x;
        p_alias.y = p1.y + 1;

        if (rect_contains(&clip_rect, &p_alias))
          (*fb->base.set_pixel)(&fb->base, &p_alias,
            alpha_blend(rgb_alpha(fill_color, (weighting ^ weighting_complement_mask)),
              (*fb->base.get_pixel)(&fb->base, &p_alias, 0)), 0);
        }
      }
    }

  return s_ok;
  }

result_t bsp_fast_copy(framebuffer_t* hndl,
  const point_t* dest_pt,
  const framebuffer_t* src_hndl,
  const rect_t* src_rect,
  raster_operation operation)
  {
  krypton_framebuffer_t* dst_canvas = (krypton_framebuffer_t*)hndl;
  int32_t dst_incr;

  switch (dst_canvas->orientation)
    {
    case 0:
      dst_incr = 1 << dst_canvas->pixel_shift;
      break;
    case 90:
      dst_incr = -(root_framebuffer(dst_canvas)->base.position.right << dst_canvas->pixel_shift);
      break;
    case 180:
      dst_incr = -(1 << dst_canvas->pixel_shift);
      break;
    case 270:
      dst_incr = (root_framebuffer(dst_canvas)->base.position.right << dst_canvas->pixel_shift);
      break;
    }

  krypton_framebuffer_t* src_canvas = (krypton_framebuffer_t*)src_hndl;
  int32_t src_incr;

  switch (src_canvas->orientation)
    {
    case 0:
      src_incr = 1 << src_canvas->pixel_shift;
      break;
    case 90:
      src_incr = -(root_framebuffer(src_canvas)->base.position.right << src_canvas->pixel_shift);
      break;
    case 180:
      src_incr = -(1 << src_canvas->pixel_shift);
      break;
    case 270:
      src_incr = (root_framebuffer(src_canvas)->base.position.right << src_canvas->pixel_shift);
      break;
    }

  int32_t x_pixels = rect_width(src_rect);
  int32_t y_pixels = rect_height(src_rect);
  point_t src_pt;
  point_t dst_pt;

  // get our point correct
  rect_top_left(src_rect, &src_pt);
  point_copy(dest_pt, &dst_pt);

  while (y_pixels--)
    {
    int32_t x;
    uint8_t* src_line = point_to_address(src_canvas, &src_pt);
    uint8_t* dst_line = point_to_address(dst_canvas, &dst_pt);

    for (x = 0; x < x_pixels; x++)
      {
      if (operation == src_alpha_blend)
        _set_pixel_alpha(dst_canvas, dst_line, _get_pixel(src_canvas, src_line));
      else
        _set_pixel(dst_canvas, dst_line, _get_pixel(src_canvas, src_line));

      // add the byte offset for the pixel
      src_line += src_incr;
      dst_line += dst_incr;
      }

    src_pt.y++;
    dst_pt.y++;
    }

  return s_ok;
  }

result_t bsp_copy_bitmap(framebuffer_t* hndl,
  const point_t* dest_pt,
  const bitmap_t* src_bitmap,
  const rect_t* src_rect)
  {
  krypton_framebuffer_t* dst_canvas = (krypton_framebuffer_t*)hndl;
  int32_t dst_incr;

  switch (dst_canvas->orientation)
    {
    case 0:
      dst_incr = 1 << dst_canvas->pixel_shift;
      break;
    case 90:
      dst_incr = -(root_framebuffer(dst_canvas)->base.position.right << dst_canvas->pixel_shift);
      break;
    case 180:
      dst_incr = -(1 << dst_canvas->pixel_shift);
      break;
    case 270:
      dst_incr = (root_framebuffer(dst_canvas)->base.position.right << dst_canvas->pixel_shift);
      break;
    }

  int32_t width = rect_width(src_rect);
  int32_t height = rect_height(src_rect);
  point_t src_pt;
  point_t dst_pt;

  int32_t src_y_offset = 0;

  rect_top_left(src_rect, &src_pt);
  point_copy(dest_pt, &dst_pt);

  while (height--)
    {
    int32_t x;
    uint8_t* dst_line = point_to_address(dst_canvas, &dst_pt);
    for (x = 0; x < width; x++)
      {
      // set the destination but alpha blend the bitmap color.
      _set_pixel(dst_canvas, dst_line, src_bitmap->pixels[x + src_y_offset]);

      // add another pixel to the destination offset
      dst_line += dst_incr;
      }

    dst_pt.y++;
    src_pt.y++;

    src_y_offset += src_bitmap->bitmap_width;
    }

  return s_ok;
  }

result_t bsp_open_layer(uint16_t orientation, layer_type lt, framebuffer_t** _fb)
  {
  switch (lt)
    {
    case lt_background:
      *_fb = background_fb;
      break;
    case lt_foreground:
      *_fb = foreground_fb;
      break;
    case lt_overlay:
      *_fb = overlay_fb;
      break;
    }

  if (*_fb != 0)
    return s_ok;

  return e_unexpected;
  }

result_t bsp_create_framebuffer(uint16_t x, uint16_t y, layer_type lt, framebuffer_t** _fb)
  {
  result_t result;

  krypton_framebuffer_t* fbc;
  if (failed(result = neutron_malloc(sizeof(krypton_framebuffer_t), (void**)&fbc)))
    return result;

  memset(fbc, 0, sizeof(krypton_framebuffer_t));
  fbc->base.type = lt;
  fbc->bits_per_pixel = 32;
  fbc->base.get_pixel = bsp_get_pixel;
  fbc->base.set_pixel = bsp_set_pixel;
  fbc->base.fast_fill = bsp_fast_fill;
  fbc->base.fast_line = bsp_fast_line;
  fbc->base.fast_copy = bsp_fast_copy;
  fbc->base.queue_empty = window_queue_empty;
  fbc->base.begin_paint = window_begin_paint;
  fbc->base.end_paint = window_end_paint;
  fbc->base.is_surface = true;

  rect_create(0, 0, x, y, &fbc->base.position);

  fbc->red_mask = 0xff000000;
  fbc->red_shift = 0;
  fbc->red_offset = 24;

  fbc->blue_mask = 0x0000ff00;
  fbc->blue_shift = 0;
  fbc->blue_offset = 8;

  fbc->green_mask = 0x00ff0000;
  fbc->green_shift = 0;
  fbc->green_offset = 16;

  fbc->alpha_mask = 0x000000ff;
  fbc->alpha_shift = 0;
  fbc->alpha_offset = 00;

  size_t buffer_size = (x * y) * sizeof(color_t);
  // allocate memory
  if (failed(result = neutron_malloc(buffer_size, (void**)&fbc->fb.buffer)))
    return result;

  // set to a hollow screen
  memset(fbc->fb.buffer, 0, buffer_size);

  fbc->release_buffer = true;

  fbc->pixel_shift = 2;

  // save the root canvas
  *_fb = &fbc->base;

  return s_ok;
  }

result_t bsp_set_primary(framebuffer_t* fb, uint8_t* buffer);
result_t bsp_create_framebuffer(uint16_t x, uint16_t y, layer_type lt, framebuffer_t** fb);

static HWND window;
static bool repaint = false;
static uint16_t dimx;
static uint16_t dimy;

static LPDIRECTDRAW direct_draw = NULL;
static LPDIRECTDRAWSURFACE primary_surface = NULL;
static LPDIRECTDRAWSURFACE background_surface = NULL;
static LPDIRECTDRAWCLIPPER clipper;    // Clipper for windowed mode
static bool primary = true;
static const char* className = "msh-class";

static handle_t vsync_semaphore;
static handle_t gdi_semaphore;

static void set_background()
  {
  DDSURFACEDESC2 surface_desc;

  memset(&surface_desc, 0, sizeof(DDSURFACEDESC2));
  surface_desc.dwSize = sizeof(DDSURFACEDESC2);
  IDirectDrawSurface_Lock(background_surface, NULL, (LPDDSURFACEDESC)&surface_desc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
  bsp_set_primary(direct_draw_fb, (uint8_t*)surface_desc.lpSurface);
  }


static void alpha_blt(framebuffer_t* hndl,
  const point_t* dest_pt,
  const framebuffer_t* src_hndl,
  const rect_t* src_rect)
  {
  krypton_framebuffer_t* dst_canvas = (krypton_framebuffer_t*)hndl;
  int32_t dst_incr;

  switch (dst_canvas->orientation)
    {
    case 0:
      dst_incr = 1 << dst_canvas->pixel_shift;
      break;
    case 90:
      dst_incr = -(root_framebuffer(dst_canvas)->base.position.right << dst_canvas->pixel_shift);
      break;
    case 180:
      dst_incr = -(1 << dst_canvas->pixel_shift);
      break;
    case 270:
      dst_incr = (root_framebuffer(dst_canvas)->base.position.right << dst_canvas->pixel_shift);
      break;
    }

  krypton_framebuffer_t* src_canvas = (krypton_framebuffer_t*)src_hndl;
  int32_t src_incr;

  switch (src_canvas->orientation)
    {
    case 0:
      src_incr = 1 << src_canvas->pixel_shift;
      break;
    case 90:
      src_incr = -(root_framebuffer(src_canvas)->base.position.right << src_canvas->pixel_shift);
      break;
    case 180:
      src_incr = -(1 << src_canvas->pixel_shift);
      break;
    case 270:
      src_incr = (root_framebuffer(src_canvas)->base.position.right << src_canvas->pixel_shift);
      break;
    }

  int32_t x_pixels = rect_width(src_rect);
  int32_t y_pixels = rect_height(src_rect);
  point_t src_pt;
  point_t dst_pt;

  // get our point correct
  rect_top_left(src_rect, &src_pt);
  point_copy(dest_pt, &dst_pt);

  while (y_pixels--)
    {
    int32_t x;
    uint8_t* src_line = point_to_address(src_canvas, &src_pt);
    uint8_t* dst_line = point_to_address(dst_canvas, &dst_pt);

    for (x = 0; x < x_pixels; x++)
      {
      color_t pix = _get_pixel(src_canvas, src_line);
      _set_pixel(dst_canvas, dst_line,
        alpha_blend(pix, _get_pixel(dst_canvas, dst_line)));

      // add the byte offset for the pixel
      src_line += src_incr;
      dst_line += dst_incr;
      }

    src_pt.y++;
    dst_pt.y++;
    }
  }


static void vsync_worker(void* arg)
  {
  // create direct draw
  HRESULT hr = DirectDrawCreate(NULL, &direct_draw, NULL);

  hr = IDirectDraw_SetCooperativeLevel(direct_draw, window, DDSCL_NORMAL);

  DDSURFACEDESC ddsd; // A structure to describe the surfaces we want
  // Clear all members of the structure to 0
  memset(&ddsd, 0, sizeof(ddsd));
  // The first parameter of the structure must contain the size of the structure
  ddsd.dwSize = sizeof(ddsd);

  // The dwFlags paramater tell DirectDraw which DDSURFACEDESC
    // fields will contain valid values
  ddsd.dwFlags = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

  hr = IDirectDraw_CreateSurface(direct_draw, &ddsd, &primary_surface, NULL);

  //-- Create the back buffer

  ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
  // Make our off-screen surface 320x240
  ddsd.dwWidth = dimx;
  ddsd.dwHeight = dimy;
  // Create an offscreen surface
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

  hr = IDirectDraw_CreateSurface(direct_draw, &ddsd, &background_surface, NULL);

  DDPIXELFORMAT ddpf;
  memset(&ddpf, 0, sizeof(DDPIXELFORMAT));
  ddpf.dwSize = sizeof(DDPIXELFORMAT);

  hr = IDirectDrawSurface_GetPixelFormat(background_surface, &ddpf);

  // Create the clipper using the DirectDraw object
  IDirectDraw_CreateClipper(direct_draw, 0, &clipper, NULL);

  // Assign your window's HWND to the clipper
  hr = IDirectDrawClipper_SetHWnd(clipper, 0, window);

  // Attach the clipper to the primary surface
  hr = IDirectDrawSurface_SetClipper(primary_surface, clipper);

  set_background();

  semaphore_signal(gdi_semaphore);

  int32_t idle_timer = 5;
  canmsg_t msg;
  set_can_id(&msg, id_timer);
  set_can_len(&msg, 0);

  while (true)
    {
    semaphore_wait(vsync_semaphore, 200);

    if (_screen != 0)
      {
      canvas_t* canvas;
      if (succeeded(is_typeof(_screen->base.background_canvas, &canvas_type, (void**)&canvas)))
        window_sync(canvas->fb);

      if (succeeded(is_typeof(_screen->base.foreground_canvas, &canvas_type, (void**)&canvas)))
        window_sync(canvas->fb);

      if (succeeded(is_typeof(_screen->base.overlay_canvas, &canvas_type, (void**)&canvas)))
        window_sync(canvas->fb);
      }

    if (--idle_timer <= 0)
      {
      post_message((handle_t)_screen, &msg, 0);
      idle_timer = 5;
      }
    }
  }

static point_t mouse_down_pt;
static bool mouse_down;

static LRESULT CALLBACK _wndproc(HWND   hwnd, UINT   uMsg, WPARAM wParam, LPARAM lParam)
  {
  if (uMsg == WM_MOUSEWHEEL)
    {
    int16_t scroll = HIWORD(wParam);
    scroll /= 24;
    scroll *= -1;
    canmsg_t msg;
    create_can_msg_int16(&msg, id_touch_zoom, scroll);
    post_message((handle_t)_screen, &msg, 0);
    return 0;
    }
  else if (uMsg == WM_LBUTTONDOWN)
    {
    if (!mouse_down)
      {
      mouse_down = true;
      mouse_down_pt.x = LOWORD(lParam);
      mouse_down_pt.y = HIWORD(lParam);
      }

    return 0;
    }
  else if (uMsg == WM_LBUTTONUP)
    {
    if (mouse_down)
      {
      mouse_down = false;
      point_t up_pt;
      up_pt.x = LOWORD(lParam);
      up_pt.y = HIWORD(lParam);

      int16_t dx = 0-(int16_t)(up_pt.x - mouse_down_pt.x);
      int16_t dy = 0-(int16_t)(up_pt.y - mouse_down_pt.y);
      uint32_t ex;
      ex = (((uint32_t)dx) << 16) | (((uint32_t)dy) & 0xFFFF);

      canmsg_t msg;
      create_can_msg_uint32(&msg, id_touch_pan, ex);
      post_message((handle_t)_screen, &msg, 0);
      return 0;
      }
    return 0;
    }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

static void window_worker(void* argv)
  {
  WNDCLASS wndclass;
  memset(&wndclass, 0, sizeof(WNDCLASS));
  wndclass.lpfnWndProc = _wndproc;
  wndclass.lpszClassName = className;

  RegisterClass(&wndclass);

  char name[80];
  sprintf(name, "MSH: %d * %d", dimx, dimy);

  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = dimx;
  rect.bottom = dimy;

  AdjustWindowRect(&rect, WS_CAPTION | WS_THICKFRAME | WS_VISIBLE, FALSE);

  rect.bottom += 0 - rect.top;
  rect.right += 0 - rect.left;
  rect.left = 0;
  rect.top = 0;

  window = CreateWindowEx(0, className, name, WS_CAPTION | WS_THICKFRAME | WS_VISIBLE,
    rect.left, rect.top, rect.right, rect.bottom,
    NULL, NULL, NULL, NULL);

  ShowWindow(window, SW_SHOW);

  task_create("vsync", 0, vsync_worker, 0, HIGH_PRIORITY, &h_worker);

  MSG msg;
  while (true)
    {
    while (GetMessage(&msg, window, 0, 0))
      DispatchMessage(&msg);
    }
  }

result_t start_fb(uint16_t x, uint16_t y, uint8_t* buffer)
  {
  result_t result;

  semaphore_create(&vsync_semaphore);
  semaphore_create(&gdi_semaphore);

  dimx = x;
  dimy = y;

  if (failed(result = bsp_create_framebuffer(x, y, lt_background, &background_fb)))
    return result;

  if (failed(result = bsp_create_framebuffer(x, y, lt_foreground, &foreground_fb)))
    return result;

  if (failed(result = bsp_create_framebuffer(x, y, lt_overlay, &overlay_fb)))
    return result;

  // create the pseudo canvas
  krypton_framebuffer_t* fbc;
  if (failed(result = neutron_malloc(sizeof(krypton_framebuffer_t), (void**)&fbc)))
    return result;

  memset(fbc, 0, sizeof(krypton_framebuffer_t));
  fbc->base.type = 0;
  fbc->bits_per_pixel = 32;
  fbc->base.get_pixel = bsp_get_pixel;
  fbc->base.set_pixel = bsp_set_pixel;
  fbc->base.fast_fill = bsp_fast_fill;
  fbc->base.fast_line = bsp_fast_line;
  fbc->base.fast_copy = bsp_fast_copy;
  fbc->base.queue_empty = window_queue_empty;
  fbc->base.begin_paint = window_begin_paint;
  fbc->base.end_paint = window_end_paint;
  fbc->base.is_surface = true;

  rect_create(0, 0, x, y, &fbc->base.position);

  fbc->red_mask = 0xff000000;
  fbc->red_shift = 0;
  fbc->red_offset = 24;

  fbc->blue_mask = 0x0000ff00;
  fbc->blue_shift = 0;
  fbc->blue_offset = 8;

  fbc->green_mask = 0x00ff0000;
  fbc->green_shift = 0;
  fbc->green_offset = 16;

  fbc->alpha_mask = 0x000000ff;
  fbc->alpha_shift = 0;
  fbc->alpha_offset = 00;

  // allocate memory
  fbc->fb.buffer = 0;
  fbc->release_buffer = false;

  fbc->pixel_shift = 2;

  direct_draw_fb = &fbc->base;

  create_canvas_from_framebuffer(direct_draw_fb, &draw_canvas);

  task_create("wnd", 0, window_worker, 0, NORMAL_PRIORITY, &h_worker);

  // wait till the worker has started
  return semaphore_wait(gdi_semaphore, INDEFINITE_WAIT);
  }

result_t bsp_sync_framebuffer(framebuffer_t* fb)
  {
  // fill the canvas black
  rectangle(draw_canvas, &_screen->base.position, color_hollow, color_black, &_screen->base.position);

  // bitblt the canvas to simulate overlays
  point_t pt = { 0, 0 };

  alpha_blt(direct_draw_fb, &pt, background_fb, &_screen->base.position);
  alpha_blt(direct_draw_fb, &pt, foreground_fb, &_screen->base.position);
  alpha_blt(direct_draw_fb, &pt, overlay_fb, &_screen->base.position);

  // Check the primary surface
  if (primary_surface != NULL)
    {
    if (IDirectDrawSurface_IsLost(primary_surface) == DDERR_SURFACELOST)
      IDirectDrawSurface_Restore(primary_surface);
    }

  if (background_surface)
    {
    if (IDirectDrawSurface_IsLost(background_surface) == DDERR_SURFACELOST)
      IDirectDrawSurface_Restore(background_surface);
    }

  RECT    rcSrc;  // source blit rectangle
  RECT    rcDest; // destination blit rectangle
  POINT   p;
  HRESULT hr;

  // find out where on the primary surface our window lives
  p.x = 0;
  p.y = 0;

  ClientToScreen(window, &p);
  GetClientRect(window, &rcDest);

  OffsetRect(&rcDest, p.x, p.y);
  SetRect(&rcSrc, 0, 0, dimx, dimy);

  // release the background so we can use it
  IDirectDrawSurface_Unlock(background_surface, NULL);
  // hardware copy it
  hr = IDirectDrawSurface_Blt(primary_surface, &rcDest, background_surface, &rcSrc, DDBLT_WAIT, NULL);

  set_background();

  return window_sync_done(fb);
  }
