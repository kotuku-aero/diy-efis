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
#ifndef __x11_screen_h__
#define __x11_screen_h__

#ifdef X11_SCREEN

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../gdi-lib/screen.h"

namespace kotuku
  {
  class x11_screen_t: public screen_t, protected screen_metrics_t
    {
  public:
    x11_screen_t(Display *display, Drawable d, const rect_t &dimensions,
        size_t bpp);
    virtual ~x11_screen_t();

    // information about the screen
    virtual const screen_metrics_t *screen_metrics() const;

    // canvas implementation routines.
    virtual screen_t *create_canvas(screen_t *, const extent_t &);
    virtual screen_t *create_canvas(screen_t *, const bitmap_t &);
    virtual screen_t *create_canvas(screen_t *, const rect_t &);

    // drawing functions.
    // most are the same as the canvas functions but they
    // have the clipping and drawing tools added
    virtual void polyline(canvas_t *canvas, const rect_t &clip_rect, const pen_t *,
        const point_t *points, size_t count);
    virtual void fill_rect(canvas_t *canvas, const rect_t &clip_rect, const rect_t &, color_t);
    virtual void ellipse(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t,
        const rect_t &);
    virtual void polygon(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t,
        const point_t *, size_t count, bool interior_fill);
    virtual void rectangle(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t,
        const rect_t &);
    virtual void round_rect(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t,
        const rect_t &, const extent_t &);
    virtual void pattern_blt(canvas_t *canvas, const rect_t &clip_rect, const bitmap_t *,
        const rect_t &, raster_operation mode);
    virtual void bit_blt(canvas_t *canvas, const rect_t &clip_rect, const rect_t &dest_rect,
        const screen_t *src_screen, const rect_t &src_clip_rect,
        const point_t &src_pt, raster_operation operation);
    virtual void mask_blt(canvas_t *canvas, const rect_t &clip_rect, const rect_t &dest_rect,
        const screen_t *src_screen, const rect_t &src_clip_rect,
        const point_t &src_point, const bitmap_t &mask_bitmap,
        const point_t &mask_point, raster_operation operation);
    virtual void rotate_blt(canvas_t *canvas, const rect_t &clip_rect, const point_t &dest_center,
        const screen_t *src, const rect_t &src_clip_rect,
        const point_t &src_point, size_t radius, double angle,
        raster_operation operation);
    virtual color_t get_pixel(const canvas_t *canvas, const rect_t &clip_rect, const point_t &) const;
    virtual color_t set_pixel(canvas_t *canvas, const rect_t &clip_rect, const point_t &,
        color_t c);
    virtual void angle_arc(canvas_t *canvas, const rect_t &clip_rect, const pen_t *,
        const point_t &, gdi_dim_t radius, double start, double end);
    virtual void pie(canvas_t *canvas, const rect_t &clip_rect, const pen_t *, color_t,
        const point_t &, double start, double end, gdi_dim_t radii, gdi_dim_t inner);
    virtual void draw_text(canvas_t *canvas, const rect_t &clip_rect, const font_t *, color_t fg,
        color_t bg, const char *str, size_t count, const point_t &src_pt,
        const rect_t &txt_clip_rect, text_flags format, size_t *char_widths);
    virtual extent_t text_extent(const canvas_t *canvas, const font_t *, const char *str,
        size_t count) const;
    virtual void scroll(canvas_t *canvas, const rect_t &clip_rect, const extent_t &offsets,
        const rect_t &area_to_scroll, const rect_t &clipping_rectangle,
        rect_t *rect_update);
    virtual void background_mode(canvas_t *canvas, int);
    virtual void invalidate_rect(canvas_t *canvas, const rect_t &rect);
    /**
     * This will copy the screen to the display.
     */
    virtual void paint();

    Display *display();
  protected:
    void xset_clip_rect(const rect_t &) const;
    void xset_pen(const pen_t &) const;
    void xset_color(color_t color) const;
    void xset_raster_operation(raster_operation rop) const;
  private:
    GC _gc;
    Display *_display;
    Drawable _drawable;

    point_t _origin;
    // angles are in degrees
    void angle_arc(canvas_t *canvas, const rect_t &clip_rect, const pen_t *,
        const point_t &, gdi_dim_t radius, int start, int end);

    };

  inline Display *x11_screen_t::display()
    {
    return _display;
    }

  }

#endif
#endif
