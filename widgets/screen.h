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
#ifndef __vScreen_h__
#define __vScreen_h__

#include "color.h"
#include "extent.h"
#include "point.h"
#include "rect.h"
#include "bitmap.h"
#include "pen.h"
#include "font.h"
#include "thread.h"
#include "canvas.h"

namespace kotuku {
class canvas_t;
class window_t;

///////////////////////////////////////////////////////////////////////////////
//
// This encapsulates an interface to a physical drawing surface.
// The GDI uses a canvas as a wrapper around this interface.  It allows
// a memory mapped canvas as used by a bitmap and a window to use the same
// operations.

struct screen_metrics_t
  {
  size_t screen_x; // number of pixels in the x direction
  size_t screen_y; // number of pixels in the y direction
  size_t row_size; // number of pixels for a row.
  size_t bits_per_pixel; // number of bits in a pixel
  color_t *palette; // optional palette to use for color lookups

  screen_metrics_t(size_t x, size_t y, size_t bpp, color_t *p = 0, size_t rs = 0)
  : screen_x(x),
    screen_y(y),
    bits_per_pixel(bpp),
    row_size(rs == 0 ? x : rs),
    palette(p)
    {
    }

  screen_metrics_t(const screen_metrics_t &other)
  : screen_x(other.screen_x),
  screen_y(other.screen_y),
  bits_per_pixel(other.bits_per_pixel),
  row_size(other.row_size),
  palette(other.palette)
    {
    }

  screen_metrics_t &operator=(const screen_metrics_t &other)
    {
    screen_x = other.screen_x;
    screen_y = other.screen_y;
    bits_per_pixel = other.bits_per_pixel;
    row_size = other.row_size;
    palette = other.palette;

    return *this;
    }

private:
  screen_metrics_t();
  };

class screen_t
  {
public:

  virtual ~screen_t()
    {
    }

  // information about the screen
  virtual const screen_metrics_t *screen_metrics() const = 0;

  // canvas implementation routines.
  virtual screen_t *create_canvas(const extent_t &) = 0;
  virtual screen_t *create_canvas(const bitmap_t &) = 0;

  // drawing functions.
  // most are the same as the canvas functions but they
  // have the clipping and drawing tools added
  virtual void polyline(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, const point_t *points, size_t count) = 0;
  virtual void fill_rect(canvas_t *canvas, const rect_t &clip_rect, const rect_t &, color_t) = 0;
  virtual void ellipse(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t, const rect_t &) = 0;
  virtual void polygon(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t, const point_t *, size_t count, bool interior_fill) = 0;
  virtual void rectangle(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t, const rect_t &) = 0;
  virtual void round_rect(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t, const rect_t &, const extent_t &) = 0;
  virtual void bit_blt(canvas_t *canvas, const rect_t &clip_rect, const rect_t &dest_rect, const screen_t *src_screen, const rect_t &src_clip_rect, const point_t &src_pt) = 0;
  virtual void rotate_blt(canvas_t *canvas, const rect_t &clip_rect, const point_t &dest_center, const screen_t *src, const rect_t &src_clip_rect, const point_t &src_point, size_t radius, double angle) = 0;
  virtual color_t get_pixel(const canvas_t *canvas, const rect_t &clip_rect, const point_t &) const = 0;
  virtual color_t set_pixel(canvas_t *canvas, const rect_t &clip_rect, const point_t &, color_t c) = 0;
  virtual void angle_arc(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, const point_t &, gdi_dim_t radius, double start, double end) = 0;
  virtual void pie(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t, const point_t &, double start, double end, gdi_dim_t radii, gdi_dim_t inner) = 0;
  virtual void draw_text(canvas_t *canvas, const rect_t &clip_rect, const font_t *, color_t fg, color_t bg, const char *str, size_t count, const point_t &src_pt, const rect_t &txt_clip_rect, text_flags format, size_t *char_widths) = 0;
  virtual extent_t text_extent(const canvas_t *canvas, const font_t *, const char *str, size_t count) const = 0;
  virtual void invalidate_rect(canvas_t *canvas, const rect_t &rect) = 0;
  };
  };
  
#endif
