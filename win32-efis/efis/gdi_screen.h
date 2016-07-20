#ifndef __gdi_screen_h__
#define __gdi_screen_h__

#include "win32_hal.h"

namespace kotuku {
  class gdi_screen_t : public screen_t, protected screen_metrics_t {
  public:
    gdi_screen_t(HDC dc, size_t x, size_t y, size_t bpp);
    virtual ~gdi_screen_t();

    // information about the screen
    virtual const screen_metrics_t *screen_metrics() const;

    // canvas implementation routines.
    virtual screen_t *create_canvas(screen_t *, const rect_t &);
    virtual screen_t *create_canvas(screen_t *, const extent_t &);
    virtual screen_t *create_canvas(screen_t *, const bitmap_t &);

    // drawing functions.
    // most are the same as the canvas functions but they
    // have the clipping and drawing tools added
    virtual void polyline(const rect_t &clip_rect, const pen_t *, const point_t *points, size_t count);
    virtual void fill_rect(const rect_t &clip_rect, const rect_t &, color_t);
    virtual void ellipse(const rect_t &clip_rect, const pen_t *, color_t, const rect_t &);
    virtual void polygon(const rect_t &clip_rect, const pen_t *, color_t, const point_t *, size_t count, bool interior_fill);
    virtual void rectangle(const rect_t &clip_rect, const pen_t *, color_t, const rect_t &);
    virtual void round_rect(const rect_t &clip_rect, const pen_t *, color_t, const rect_t &, const extent_t &);
    virtual void bit_blt(const rect_t &clip_rect, const rect_t &dest_rect, const screen_t *src_screen, const rect_t &src_clip_rect, const point_t &src_pt, raster_operation operation);
    virtual void mask_blt(const rect_t &clip_rect, const rect_t &dest_rect, const screen_t *src_screen, const rect_t &src_clip_rect, const point_t &src_point, const bitmap_t &mask_bitmap, const point_t &mask_point, raster_operation operation);
    virtual void rotate_blt(const rect_t &clip_rect, const point_t &dest_center, const screen_t *src, const rect_t &src_clip_rect, const point_t &src_point, size_t radius, double angle, raster_operation operation);
    virtual color_t get_pixel(const rect_t &clip_rect, const point_t &) const;
    virtual color_t set_pixel(const rect_t &clip_rect, const point_t &, color_t c);
    virtual void angle_arc(const rect_t &clip_rect, const pen_t *, const point_t &, gdi_dim_t radius, double start, double end);
    virtual void pie(const rect_t &clip_rect, const pen_t *, color_t, const point_t &, double start, double end, gdi_dim_t radii, gdi_dim_t inner);
    virtual void draw_text(const rect_t &clip_rect, const font_t *, color_t fg, color_t bg, const char *str, size_t count, const point_t &src_pt, const rect_t &txt_clip_rect, text_flags format, size_t *char_widths);
    virtual extent_t text_extent(const font_t *, const char *str, size_t count) const;
    virtual void scroll(const rect_t &clip_rect, const extent_t &offsets, const rect_t &area_to_scroll, const rect_t &clipping_rectangle, rect_t *rect_update);
    virtual void background_mode(int m);
    virtual void invalidate_rect(const rect_t &);
    virtual void update_window();
    int z_order() const;
    void z_order(int);

    static int gdi_metric() { return _metric; }
    static void gdi_metric(int value) { _metric = value; }
    void gdi_opn() { _metric++; }
  protected:
    HDC DC() const { return _dc; }
    static int _metric;
  private:
    HDC _dc;
  };
}
#endif
