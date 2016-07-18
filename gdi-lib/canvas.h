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
#ifndef __canvas_h__
#define __canvas_h__

#include "stddef.h"
#include "color.h"
#include "font.h"
#include "bitmap.h"
#include "pen.h"
#include "rect.h"
#include <math.h>
#include <memory>

namespace kotuku {
class screen_t;
class window_t;

//  This is a canvas class. It is an encapsulation of a series of
// screen primitives along with a drawing region. All drawing will
// be clipped to the region selected
// NOTE: a brush is an 8x8 pixel bitmap

class canvas_t
  {
public:
  virtual ~canvas_t();

  // child window creation
  explicit canvas_t(window_t *owner, canvas_t &, const rect_t &);
  // compatible canvas for bitblt sources
  explicit canvas_t(canvas_t &, const extent_t &);
  // create a canvas that is based on a bitmap. Will convert to
  // the correct color map
  explicit canvas_t(canvas_t &, const bitmap_t &);

  // select a pen into this context. The default pen is
  // a 1 pixel black pen
  const pen_t *pen(const pen_t *);
  const pen_t *pen() const;
  // select a bitmap_t font to draw with
  const font_t *font(const font_t *);
  const font_t *font() const;
  // select the clipping rectangle to be used. Will
  // return the old clipping rectangle.
  rect_t clipping_rectangle(const rect_t &);
  rect_t clipping_rectangle() const;

  // Drawing-attribute Functions
  color_t background_color() const;
  color_t background_color(color_t);

  enum e_background_mode
    {
    opaque = 2,
    transparent = 1
      };

  e_background_mode background_mode() const;
  e_background_mode background_mode(e_background_mode);

  // This method determines whether any part of the specified rectangle
  // lies within the clipping region of the display window_class
  bool is_rect_visible(const rect_t &) const;
  // Line-Output Functions
  // Draws a set of line segments connecting the points specified by
  // points. The lines are drawn from the first point_t through
  // subsequent points using the current pen.
  void polyline(const point_t *points, size_t count);
  // Simple Drawing Functions
  //Call this member function to fill a given rectangle using the
  // specified brush. The function fills the complete rectangle, including
  // the left and top borders, but it does not fill the right and
  // bottom borders.
  void fill_rect(const rect_t &, color_t);
  // Draws an ellipse. The center of the ellipse is the center of the
  // bounding rectangle specified by the rect_t. The ellipse is drawn with the
  // current pen, and its interior is filled with the current brush.
  // The figure drawn by this function extends up to, but does not include,
  // the right and bottom coordinates. This means that the height of the
  // figure is y2 � y1 and the width of the figure is x2 � x1.
  // If either the width or the height of the bounding rectangle is 0,
  // no ellipse is drawn.
  void ellipse(const rect_t &);
  // Draws a polygon consisting of two or more points (vertices) connected
  // by lines, using the current pen. The system closes the polygon
  // automatically, if necessary, by drawing a line from the last vertex
  // to the first.
  void polygon(const point_t *, size_t count, bool interior_fill = false);
  // Draws a rectangle using the current pen. The interior of the rectangle
  // is filled using the current brush.
  // The rectangle extends up to, but does not include, the right and bottom
  // coordinates. This means that the height of the rectangle is y2 � y1 and
  // the width of the rectangle is x2 � x1. Both the width and the height of
  // a rectangle must be greater than 2 units and less than 32,767 units.
  void rectangle(const rect_t &);
  // Draws a rectangle with rounded corners using the current pen. The
  // interior of the rectangle is filled using the current brush.
  // The figure this function draws extends up to but does not include the
  // right and bottom coordinates. This means that the height of the figure
  // is y2 � y1 and the width of the figure is x2 � x1. Both the height
  // and the width of the bounding rectangle must be greater than 2 units
  // and less than 32,767 units.
  void round_rect(const rect_t &, const extent_t &);
  // This method copies a bitmap_t from the source device window_class to the
  // current device window_class.
  void bit_blt(const rect_t &dest_rect, const canvas_t &src_canvas,
               const point_t &src_pt, raster_operation operation);
  // This method combines the color_t data for the source and destination
  // bitmaps using the specified mask and raster operation
  void mask_blt(const rect_t &dest_rect, const canvas_t &src_canvas,
                const point_t &src_point, const bitmap_t &mask_bitmap,
                const point_t &mask_point, raster_operation operation);
  // this method will rotate a region and copy it
  void rotate_blt(const point_t &dest_center, const canvas_t &src_canvas,
                  const point_t &src_point, size_t radius, double angle,
                  raster_operation operation);
  // Retrieves the RGB color_t value of the pixel at the point_t specified by x
  // and y. The point_t must be in the clipping region. If the point_t is not
  // in the clipping region, the function has no effect and returns �1.
  color_t get_pixel(const point_t &) const;
  // Sets the pixel at the point_t specified to the closest approximation
  // of the color_t specified by crColor. The point_t must be in the clipping
  // region. If the point_t is not in the clipping region, the function
  // does nothing.
  // Returns an RGB value for the color_t that the point_t is actually painted.
  // This value can be different from that specified by crColor if an
  // approximation of that color_t is used. If the function fails (if the
  // point_t is outside the clipping region), the return value is �1.
  color_t set_pixel(const point_t &, color_t c);

  // Draws an arc. The arc is drawn along the perimeter of a circle with the
  // given radius and center. The length of the arc is defined by the given
  // start and sweep angles.
  //
  // The arc is drawn by constructing an imaginary circle with the specified
  // radius around the specified center point_t. The starting point_t of the arc is
  // determined by measuring counterclockwise from the x-axis of the circle by
  // the number of degrees in the start angle. The ending point_t is similarly
  // located by measuring counterclockwise from the starting point_t by the
  // number of degrees in the sweep angle.
  //
  // If the sweep angle is greater than 360 degrees the arc is swept multiple
  // times. This function draws lines by using the current pen.
  // The figure is not filled.
  void angle_arc(const point_t &pt, gdi_dim_t radius, double start, double end);
  // draw a pie
  void pie(const point_t &pt, double start, double end, gdi_dim_t radii,
           gdi_dim_t inner);
  // Text Functions
  color_t text_color() const;
  color_t text_color(color_t);

  void draw_text(const char *str, size_t count, const point_t &pt,
                 text_flags format = 0, size_t *char_widths = 0);
  void draw_text(const char *str, size_t count, const point_t &pt,
                 const rect_t &clip_rect, text_flags format = 0,
                 size_t *char_widths = 0);

  // Call this member function to compute the width and height of a line of text
  // using the current font to determine the dimensions.
  // The current clipping region does not affect the width and height returned
  // by get_text_extent.
  // Since some devices do not place characters in regular cell arrays (that is,
  // they carry out kerning), the sum of the extents of the characters in a
  // string may not be equal to the extent_t of the string
  extent_t text_extent(const char *str, size_t count) const;
  // Scrolls a rectangle of bits horizontally and vertically.
  void scroll(const extent_t &offsets, const rect_t &area_to_scroll,
              const rect_t &clipping_rectangle, rect_t *rect_update = 0);
  // return the window rectangle associated with a canvas
  const rect_t &window_rect() const;
  // change the window rectangle assigned to the canvas
  void window_rect(const rect_t &);

  // request a repaint of the current window
  virtual void invalidate();

  virtual bool is_invalidated() const
    {
    return _invalid;
    }

  virtual void is_updated()
    {
    _invalid = false;
    }
  canvas_t(canvas_t &, const rect_t &);
  /**
   * Construct canvas given a screen buffer and screen rectangle
   * @param screen  Screen to draw on
   * @param rect    Area canvas defined for
   * 
   * A canvas partitions the screen into drawing areas.  all coordinates
   * on the canvas are offset by the origin of the rectangle and drawing
   * is clipped to that rectange
   */
  canvas_t(screen_t *screen, const rect_t &rect);

  /**
   * Obtain the underlying drawing surface
   * @return screen the canvas is defined for.
   */
  screen_t *screen() const
    {
    return _screen;
    }
private:

  template<typename _Ty>
  inline _Ty swap_gdi_object(_Ty &g1, _Ty g2)
    {
    _Ty r = g1;
    g1 = g2;
    return r;
    }

  rect_t _window_rect;
  rect_t _clipping_rect;
  screen_t *_screen;

  // the following are always realized to the device context
  // pixel mode.
  const pen_t *_pen;
  const font_t *_font;
  color_t _background_color;
  color_t _text_color;
  e_background_mode _background_mode;
  // set true when an invalidate call is made.
  bool _invalid;
  };
  };
  
#endif
