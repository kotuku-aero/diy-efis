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
#ifdef X11_SCREEN
#include "x11_screen.h"
#include "../gdi-lib/spatial.h"
#include "linux_hal.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../widgets/layout_window.h"

kotuku::x11_screen_t::x11_screen_t(Display *display, Drawable d, const rect_t &dimensions, size_t bpp)
: screen_metrics_t(dimensions.width(), dimensions.height(), bpp),
  _display(display),
  _drawable(d),
  _origin(dimensions.top_left())
  {
  _gc = XCreateGC(display, d, 0, NULL);
  }

kotuku::x11_screen_t::~x11_screen_t()
  {
  }

// information about the screen
const kotuku::screen_metrics_t *kotuku::x11_screen_t::screen_metrics() const
  {
  return this;
  }

// canvas implementation routines.
kotuku::screen_t *kotuku::x11_screen_t::create_canvas(screen_t *, const extent_t &ex)
  {
  Pixmap pixmap = XCreatePixmap(_display, _drawable, ex.dx, ex.dy, bits_per_pixel);
  return new x11_screen_t(_display, pixmap, rect_t(point_t(0, 0), ex), bits_per_pixel);
  }

kotuku::screen_t *kotuku::x11_screen_t::create_canvas(screen_t *, const bitmap_t &bm)
  {
  Pixmap pixmap = XCreatePixmapFromBitmapData(_display, _drawable,
      reinterpret_cast<char *>(const_cast<color_t *>(bm.pixels)),
      bm.bitmap_width, bm.bitmap_height, color_hollow,
      color_hollow, bm.bpp);

  return new x11_screen_t(_display, pixmap,
      rect_t(0, 0, bm.bitmap_width, bm.bitmap_height), bm.bpp);
  }

kotuku::screen_t *kotuku::x11_screen_t::create_canvas(screen_t *, const rect_t &dimensions)
  {
  Pixmap pixmap = XCreatePixmap(_display, _drawable, dimensions.width(),
      dimensions.height(), bits_per_pixel);
  return new x11_screen_t(_display, pixmap, dimensions, bits_per_pixel);
  }

void kotuku::x11_screen_t::paint()
  {
  // called to render the off-screen bitmap to the display
  XCopyArea(_display, _drawable, _drawable, _gc, 0, 0,
      screen_x, screen_y, _origin.x, _origin.y);
  }

static XPoint to_point(const kotuku::point_t &p)
  {
  XPoint pt;
  pt.x = (short) p.x;
  pt.y = (short) p.y;
  return pt;
  }

void kotuku::x11_screen_t::xset_clip_rect(const rect_t &clip_rect) const
  {
  XRectangle xrect;
  xrect.x = clip_rect.left;
  xrect.y = clip_rect.top;
  xrect.width = clip_rect.width();
  xrect.height = clip_rect.height();

  XSetClipRectangles(_display, _gc, 0, 0, &xrect, 1, Unsorted);
  }

void kotuku::x11_screen_t::xset_pen(const pen_t &pen) const
  {
  xset_color(pen.color);

  XSetLineAttributes(_display, _gc, pen.width,
      pen.style == ps_dash ? LineOnOffDash : LineSolid,
      CapRound, JoinRound);
  }

void kotuku::x11_screen_t::xset_color(color_t color) const
  {
  XSetForeground(_display, _gc, color);
  }

void kotuku::x11_screen_t::xset_raster_operation(raster_operation rop) const
  {
  XGCValues values;
  switch (rop)
    {
    case rop_r2_black:   //  0
    case rop_r2_notmergepen:// dpon
    case rop_r2_masknotpen:// dpna
    case rop_r2_notcopypen:// pn
    case rop_r2_maskpennot:// pdna
    case rop_r2_not:// dn
    case rop_r2_xorpen:// dpx
    case rop_r2_notmaskpen:// dpan
    case rop_r2_maskpen:// dpa
    case rop_r2_notxorpen:// dpxn
    case rop_r2_nop:// d
    case rop_r2_mergenotpen:// dpno
    case rop_r2_copypen:// p
    case rop_r2_mergepennot:// pdno
    case rop_r2_mergepen:// dpo
    case rop_r2_last:
    case rop_srccopy:// dest = source
    values.function = GXcopy;
    break;
    case rop_whiteness:// dest = white
    values.function = GXset;
    break;
    case rop_srcpaint:// dest = source or dest
    values.function = GXor;
    break;
    case rop_srcand:// dest = source and dest
    values.function = GXand;
    break;
    case rop_srcinvert:// dest = source xor dest
    values.function = GXxor;
    break;
    case rop_srcerase:// dest = source and (not dest )
    values.function = GXandReverse;
    break;
    case rop_notsrccopy:// dest = (not source)
    values.function = GXcopyInverted;
    break;
    case rop_notsrcerase:// dest = (not src) and (not dest)
    values.function = GXnor;
    break;
    case rop_mergecopy:// dest = (source and pattern)
    values.function = GXand;
    break;
    case rop_mergepaint:// dest = (not source) or dest
    values.function = GXorInverted;
    break;
    case rop_patcopy:// dest = pattern
    values.function = GXcopy;
    break;
    case rop_patpaint:// dest = dpsnoo
    values.function = GXcopy;
    break;
    case rop_patinvert:// dest = pattern xor dest
    values.function = GXcopy;
    break;
    case rop_dstinvert:// dest = (not dest)
    values.function = GXinvert;
    break;
    case rop_blackness:// dest = black
    values.function = GXclear;
    break;
    }

  XChangeGC(_display, _gc, GCFunction, &values);
  }

// drawing functions.
// most are the same as the canvas functions but they
// have the clipping and drawing tools added
void kotuku::x11_screen_t::polyline(canvas_t *canvas, const rect_t &clip_rect, const pen_t *p,
    const point_t *points, size_t count)
  {
  xset_clip_rect(clip_rect);
  xset_pen(*p);

  XPoint *xpoints = (XPoint *) alloca(sizeof(XPoint) * count);

  for(size_t i = 0; i < count; i++)
    {
    xpoints[i].x = (short) points[i].x;
    xpoints[i].y = (short) points[i].y;
    }

  XDrawLines(_display, _drawable, _gc, xpoints, count, CoordModeOrigin);
  }

void kotuku::x11_screen_t::fill_rect(canvas_t *canvas, const rect_t &clip_rect, const rect_t &r,
    color_t c)
  {
  xset_clip_rect(clip_rect);
  xset_color(c);

  XFillRectangle(_display, _drawable, _gc, r.left, r.top, r.width(),
      r.height());
  }

void kotuku::x11_screen_t::ellipse(canvas_t *canvas, const rect_t &clip_rect, const pen_t *p,
    color_t c, const rect_t &r)
  {
  // draw filled arc first
  xset_clip_rect(clip_rect);
  xset_color(c);

  XFillArc(_display, _drawable, _gc, r.left, r.top, r.width(), r.height(), 0,
      360 * 64);

  // now the border
  if(p != 0)
    {
    xset_pen(*p);
    XDrawArc(_display, _drawable, _gc, r.left, r.top, r.width(), r.height(), 0,
        360 * 64);
    }
  }

void kotuku::x11_screen_t::polygon(canvas_t *canvas, const rect_t &clip_rect, const pen_t *p,
    color_t c, const point_t *points, size_t count,
    bool interior_fill)
  {
  xset_clip_rect(clip_rect);

  XPoint *xpoints = (XPoint *) alloca(sizeof(XPoint) * count);

  for(size_t i = 0; i < count; i++)
    {
    xpoints[i].x = (short) points[i].x;
    xpoints[i].y = (short) points[i].y;
    }

  xset_color(c);
  XFillPolygon(_display, _drawable, _gc, xpoints, count, Convex,
      CoordModeOrigin);

  xset_pen(*p);
  XDrawLines(_display, _drawable, _gc, xpoints, count, CoordModeOrigin);
  }

void kotuku::x11_screen_t::rectangle(canvas_t *canvas, const rect_t &clip_rect, const pen_t *pen,
    color_t c, const rect_t &r)
  {
  // draw filled arc first
  xset_clip_rect(clip_rect);
  xset_color(c);

  int major = r.width() >> 1;
  int minor = r.height() >> 1;

  XFillRectangle(_display, _drawable, _gc, r.left, r.top, r.width(),
      r.height());

  if(pen != 0)
    {
    XPoint points[5] =
      {
      to_point(r.top_left()), to_point(r.top_right()), to_point(
          r.bottom_right()), to_point(r.bottom_left()), to_point(r.top_left())
      };

    xset_pen(*pen);
    XDrawLines(_display, _drawable, _gc, points, 5, CoordModeOrigin);
    }
  }

void kotuku::x11_screen_t::round_rect(canvas_t *canvas, const rect_t &clip_rect, const pen_t *pen,
    color_t c, const rect_t &r, const extent_t &e)
  {
  // draw filled arc first
  xset_clip_rect(clip_rect);
  xset_color(c);

  xset_pen(*pen);
  if(e.dx == e.dy)
    {
    angle_arc(canvas, clip_rect, pen, r.top_left()+ e, e.dx, 270, 360);
    angle_arc(canvas, clip_rect, pen, point_t(r.top_right().x- e.dx -1, r.top_right().y + e.dx), e.dx, 0, 90);
    angle_arc(canvas, clip_rect, pen, point_t(r.bottom_left().x + e.dx, r.bottom_left().y - e.dx -1), e.dx, 180, 270);
    angle_arc(canvas, clip_rect, pen, point_t(r.bottom_right().x-e.dx -1, r.bottom_right().y - e.dx -1), e.dx, 90, 180);
    }
  else
    {
    // use a clipped ellipse
    }

  point_t line_seg[2];

  // left line
  line_seg[0].x = r.left; line_seg[0].y = r.top + e.dy;
  line_seg[1].x = r.left; line_seg[1].y = r.bottom - e.dy -1;
  polyline(canvas, clip_rect, pen, line_seg, 2);

  // top line
  line_seg[0].x = r.left + e.dx; line_seg[0].y = r.top;
  line_seg[1].x = r.right - e.dx -1; line_seg[1].y = r.top;
  polyline(canvas, clip_rect, pen, line_seg, 2);

  // bottom line
  line_seg[0].x = r.left + e.dx; line_seg[0].y = r.bottom -1;
  line_seg[1].x = r.right - e.dx -1; line_seg[1].y = r.bottom -1;
  polyline(canvas, clip_rect, pen, line_seg, 2);

  // right line
  line_seg[0].x = r.right-1; line_seg[0].y = r.top + e.dy;
  line_seg[1].x = r.right-1; line_seg[1].y = r.bottom - e.dy -1;
  polyline(canvas, clip_rect, pen, line_seg, 2);
  }

void kotuku::x11_screen_t::pattern_blt(canvas_t *canvas, const rect_t &clip_rect,
    const bitmap_t *, const rect_t &, raster_operation mode)
  {

  }

void kotuku::x11_screen_t::bit_blt(canvas_t *canvas, const rect_t &clip_rect,
    const rect_t &dest_rect, const screen_t *src_screen,
    const rect_t &src_clip_rect, const point_t &src_pt,
    raster_operation operation)
  {
  xset_clip_rect(clip_rect);
  xset_raster_operation(operation);

  const x11_screen_t *x11_src = dynamic_cast<const x11_screen_t *>(src_screen);

  XCopyArea(_display, x11_src->_drawable, _drawable, _gc, src_pt.x, src_pt.y,
      dest_rect.width(), dest_rect.height(), dest_rect.left, dest_rect.top);

  xset_raster_operation(rop_srccopy);
  }

void kotuku::x11_screen_t::mask_blt(canvas_t *canvas, const rect_t &clip_rect,
    const rect_t &dest_rect, const screen_t *src_screen,
    const rect_t &src_clip_rect, const point_t &src_point,
    const bitmap_t &mask_bitmap, const point_t &mask_point,
    raster_operation operation)
  {

  }

void kotuku::x11_screen_t::rotate_blt(canvas_t *canvas, const rect_t &clip_rect,
    const point_t &dest_center, const screen_t *src,
    const rect_t &src_clip_rect, const point_t &src_point, size_t radius,
    double angle, raster_operation operation)
  {
  x11_screen_t *src_screen = (x11_screen_t *) src;
  XImage *p_image = XGetImage(_display, src_screen->_drawable, 0, 0,
      src_screen->screen_x, src_screen->screen_y, AllPlanes, XYPixmap);

  if(p_image == 0)
  return;

  // we perform this operation only on the circle bounded however for
  // speed we perform this as a rectangle
  long r2 = long(radius) * long(radius);

  // assign the start and end to the rectangle bounds relative to the origin
  long init_x;
  long y;
  long right;
  long bottom;

  if(src_point.x - src_clip_rect.left < long(radius))
  init_x = src_clip_rect.left;
  else
  init_x = src_point.x - long(radius);

  if(src_clip_rect.right - src_point.y < long(radius))
  right = src_clip_rect.right;
  else
  right = src_point.x + long(radius);

  if(src_point.y - src_clip_rect.top < long(radius))
  y = src_clip_rect.top;
  else
  y = src_point.y - long(radius);

  if(src_clip_rect.bottom - src_point.y < long(radius))
  bottom = src_clip_rect.bottom;
  else
  bottom = src_point.y + long(radius);

  for(; y < bottom; y++)
  for(long x = init_x; x < right; x++)
    {
    int yp = y - src_point.y;
    int xp = x - src_point.x;
    // simple pythagorus theorum to check the inclusion
    if(((yp * yp) + (xp * xp)) > r2)
    continue;

    color_t cr = XGetPixel(p_image, x, y);

    if(cr != color_t(-1) && (operation != rop_srcpaint || cr != 0))
      {
      point_t pt(x, y);

      rotate_point(src_point, pt, angle);

      yp = pt.y - src_point.y;
      xp = pt.x - src_point.x;

      set_pixel(canvas, clip_rect, point_t(dest_center.x + xp, dest_center.y + yp),
          cr);
      }
    }

  XDestroyImage(p_image);
  }

void kotuku::x11_screen_t::invalidate_rect(canvas_t *canvas, const rect_t &)
  {
  // no real concept of an invalid rectangle.  We just update the window

  window_t *the_window = canvas->as_window();
  if(the_window != 0)
    the_window->paint(true);
  }

inline void set_pixel(Display *display, Drawable drawable, GC gc,
    const kotuku::point_t &p, color_t c)
  {
  XSetForeground(display, gc, c);
  XDrawPoint(display, drawable, gc, p.x, p.y);
  }

inline color_t get_pixel(Display *display, Drawable drawable, GC gc,
    const kotuku::point_t &p)
  {
  XImage *p_image = XGetImage(display, drawable, p.x, p.y, 1, 1, AllPlanes,
      XYPixmap);
  color_t c = XGetPixel(p_image, 0, 0);
  XDestroyImage(p_image);
  return c;
  }

color_t kotuku::x11_screen_t::get_pixel(const canvas_t *canvas, const rect_t &clip_rect,
    const point_t &p) const
  {
  xset_clip_rect(clip_rect);

  return ::get_pixel(_display, _drawable, _gc, p);
  }

color_t kotuku::x11_screen_t::set_pixel(canvas_t *canvas, const rect_t &clip_rect,
    const point_t &p, color_t c)
  {
  xset_clip_rect(clip_rect);

  //color_t old_color = get_pixel(clip_rect, p);_drawable,
  ::set_pixel(_display, _drawable, _gc, p, c);

  return c;
  }

void kotuku::x11_screen_t::angle_arc(canvas_t *canvas, const rect_t &clip_rect, const pen_t *pen,
    const point_t &p, gdi_dim_t radius, double start, double end)
  {
  angle_arc(canvas, clip_rect, pen, p, radius, (int)radians_to_degrees(start), (int)radians_to_degrees(end));
  }

void kotuku::x11_screen_t::angle_arc(canvas_t *canvas, const rect_t &clip_rect, const pen_t *pen,
    const point_t &p, gdi_dim_t radius, gdi_dim_t start_degrees, gdi_dim_t end_degrees)
  {
  xset_clip_rect(clip_rect);

  int sweep = (end_degrees - start_degrees) * 64;

  start_degrees *= 64;

  start_degrees -= 90 * 64;
  while(start_degrees > (359 * 64))
  start_degrees -= (360 * 64);

  xset_pen(*pen);
  long diameter = radius << 1;
  XDrawArc(_display, _drawable, _gc, p.x - radius, p.y - radius, diameter,
      diameter, -start_degrees, -sweep);
  }

void kotuku::x11_screen_t::pie(canvas_t *canvas, const rect_t &clip_rect, const pen_t *pen,
    color_t c, const point_t &p, double start, double end, gdi_dim_t radii,
    gdi_dim_t inner)
  {
  }

void kotuku::x11_screen_t::draw_text(canvas_t *canvas, const rect_t &clip_rect,
    const font_t *font, color_t fg, color_t bg, const char *str, size_t count,
    const point_t &src_pt, const rect_t &txt_clip_rect, text_flags format,
    size_t *char_widths)
  {
  // create a clipping rectangle as needed
  rect_t txt_rect(clip_rect);
  point_t pt_pos = src_pt;

  if(format & eto_clipped)
  txt_rect &= txt_clip_rect;

  // a font is a monochrome bitmap.
  for(size_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];

    if(c > font->last_char || c < font->first_char)
    c = font->default_char;

    c -= font->first_char;

    size_t cell_width = font->char_table[c << 1];
    // see if the user wants the widths returned
    if(char_widths != 0)
    char_widths[ch] = cell_width;

    size_t offset = font->char_table[(c << 1) + 1];
    const unsigned char *mask = font->bitmap_pointer + offset;

    size_t columns = (((cell_width - 1) | 7) + 1) >> 3;
    for(size_t col = 0; col < columns; col++)
      {
      for(size_t row = 0; row < font->bitmap_height; row++)
        {
        size_t bit = 0;

        for(size_t pel = (col << 3); bit < 8 && pel < cell_width; pel++, bit++)
          {
          point_t pos(long(pt_pos.x + pel), long(pt_pos.y + row));

          bool is_fg = (*mask & (0x80 >> bit)) != 0;

          if(clip_rect && pos)
            {
            if(is_fg)
            ::set_pixel(_display, _drawable, _gc, pos, fg);
            else if(format & eto_opaque)
            ::set_pixel(_display, _drawable, _gc, pos, bg);
            }
          }

        // increment past the current row
        mask++;
        }
      }

    pt_pos.x += long(cell_width);
    }
  }

kotuku::extent_t kotuku::x11_screen_t::text_extent(const canvas_t *canvas, const font_t *font,
    const char *str, size_t count) const
  {
  extent_t ex(0, long(font->bitmap_height));

  for(size_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];
    if(c == 0)
    break;// end of the std::string

    if(c > font->last_char || c < font->first_char)
    c = font->default_char;

    c -= font->first_char;

    ex.dx += font->char_table[c << 1];
    }

  return ex;
  }

void kotuku::x11_screen_t::scroll(canvas_t *canvas, const rect_t &clip_rect,
    const extent_t &offsets, const rect_t &area_to_scroll,
    const rect_t &clipping_rectangle, rect_t *rect_update)
  {

  }

void kotuku::x11_screen_t::background_mode(canvas_t *canvas, int)
  {

  }
#endif
