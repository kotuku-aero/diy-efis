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
#ifndef __raster_screen_h__
#define __raster_screen_h__

#include "color.h"
#include "extent.h"
#include "point.h"
#include "rect.h"
#include "bitmap.h"
#include "pen.h"
#include "font.h"
#include "thread.h"
#include "screen.h"

namespace kotuku
{
///////////////////////////////////////////////////////////////////////////////
//
//  Raster screen is defined that allows for writing on a frame buffer
//
// The frame buffer uses 32 bits/pixel (true color) only

// the only routines that actually write are:
//
//  get_pixel
//  set_pixel
//  fast_fill
//  fast_copy
//  point_to_address

class raster_screen_t : public screen_t, protected screen_metrics_t
  {
public:
  // create a screen given a specific set of metrics
  // Note that the bits_per_pixel is ignored and is forced to 5
  raster_screen_t(const screen_metrics_t &);
  ~raster_screen_t();

  // screen_t implementation
  virtual const screen_metrics_t *screen_metrics() const;
  virtual void polyline(const rect_t &clip_rect, const pen_t *, const point_t *points, size_t count);
  virtual void fill_rect(const rect_t &clip_rect, const rect_t &, color_t);
  virtual void ellipse(const rect_t &clip_rect, const pen_t *, color_t, const rect_t &);
  virtual void polygon(const rect_t &clip_rect, const pen_t *, color_t, const point_t *, size_t count, bool interior_fill);
  virtual void rectangle(const rect_t &clip_rect, const pen_t *, color_t, const rect_t &);
  virtual void round_rect(const rect_t &clip_rect, const pen_t *, color_t, const rect_t &, const extent_t &);
  virtual void pattern_blt(const rect_t &clip_rect, const bitmap_t *, const rect_t &, raster_operation mode);
  virtual void bit_blt(const rect_t &clip_rect, const rect_t &dest_rect, const screen_t *src_screen, const rect_t &src_clip_rect,
                       const point_t &src_pt, raster_operation operation);
  virtual void rotate_blt(const rect_t &clip_rect, const point_t &dest_center,
                          const screen_t *src, const rect_t &src_clip_rect,
                          const point_t &src_point, size_t radius, double angle,
                          raster_operation operation);
  virtual void mask_blt(const rect_t &clip_rect, const rect_t &dest_rect,
                        const screen_t *src_screen, const rect_t &src_clip_rect,
                        const point_t &src_point, const bitmap_t &mask_bitmap,
                        const point_t &mask_point, raster_operation operation);
  virtual color_t get_pixel(const rect_t &clip_rect, const point_t &) const;
  virtual color_t set_pixel(const rect_t &clip_rect, const point_t &,
                            color_t c);
  virtual void angle_arc(const rect_t &clip_rect, const pen_t *,
                         const point_t &, gdi_dim_t radius, double start, double end);
  virtual void pie(const rect_t &clip_rect, const pen_t *, color_t,
                   const point_t &, double start, double end, gdi_dim_t radii, gdi_dim_t inner);
  virtual void draw_text(const rect_t &clip_rect, const font_t *, color_t fg,
                         color_t bg, const char *str, size_t count, const point_t &src_pt,
                         const rect_t &txt_clip_rect, text_flags format, size_t *char_widths);
  virtual extent_t text_extent(const font_t *, const char *str,
                               size_t count) const;
  virtual void scroll(const rect_t &clip_rect, const extent_t &offsets,
                      const rect_t &area_to_scroll, const rect_t &clipping_rectangle,
                      rect_t *rect_update);
  virtual void background_mode(int m);
  virtual void invalidate_rect(const rect_t &);

  virtual void display_mode(int);
  virtual int display_mode() const;
protected:

  virtual gdi_dim_t pixel_increment() const;
  virtual color_t get_pixel(const uint8_t *src) const;
  virtual void set_pixel(uint8_t *dest, color_t color) const;
  /**
   * Fast fill a region
   * @param ptr         Pointer into the screen buffer
   * @param words       Number of words to fill
   * @param fill_color  Fill color
   * @param rop         Operation
   *
   * @remarks This routine assumes a landscape fill.  (_display_mode == 0 | 180)
   * So if the display mode is 90 or 3 then things get performed vertically
   */
  virtual void fast_fill(uint8_t *dest, size_t words, color_t fill_color, raster_operation = rop_srccopy);
  /**
   * Perform a fast copy from source to destination
   * @param dest        Destination in pixel buffer (0,0)
   * @param src_screen  Screen to copy from
   * @param src         Bitmap to copy from (always lanscape orientation
   * @param words       Number of pixels to copy
   * @param rop         copy mode
   */
  virtual void fast_copy(uint8_t *dest, const raster_screen_t *src_screen, const uint8_t *src, size_t words, raster_operation rop = rop_srccopy);
  /**
   * Execute a raster operation on the two colors
   * @param src   color to merge
   * @param dst   color to merge into
   * @param rop   operation type
   * @return      merged color
   */
  virtual color_t execute_rop(color_t src, color_t dst, raster_operation rop);
  /**
   * Convert a screen point to an address.
   * @param pt    Point to convert
   * @return      Address of point in buffer
   */
  virtual uint8_t *point_to_address(const point_t &pt) = 0;
  /**
   * Convert a screen point to an address.
   * @param pt    Point to convert
   * @return      Address of point in buffer
   *
   * @remarks The point is not corrected for screen rotation.  A call
   * must be made to apply_rotation if the display device supports rotation
   */
  virtual const uint8_t *point_to_address(const point_t &pt) const = 0;

  /**
   * Rotate a point based on the screen rotation mode.
   * @param src     Point to be rotated
   * @param dst     Resulting rotated point
   * @return  reference to the destination point
   *
   * @remarks The point is not corrected for screen rotation.  A call
   * must be made to apply_rotation if the display device supports rotation
   */
  virtual point_t &apply_rotation(const point_t &src, point_t &dst) const
    {
    dst = src;
    return dst;
    }

private:
  raster_screen_t();
  raster_screen_t &operator=(const raster_screen_t &);
  critical_section_t _cs;
  int _background_mode;
  };
  }

#endif
