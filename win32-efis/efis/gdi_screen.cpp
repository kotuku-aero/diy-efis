#include "gdi_screen.h"
#include "windows_screen.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Screen Functions
kotuku::gdi_screen_t::gdi_screen_t(HDC dc, size_t x, size_t y, size_t bpp)
: screen_metrics_t(x, y, bpp)
  {
  _dc = dc;
  }

kotuku::gdi_screen_t::~gdi_screen_t()
  {
  }

// information about the screen
const kotuku::screen_metrics_t *kotuku::gdi_screen_t::screen_metrics() const
  {
  return this;
  }

inline COLORREF to_color(color_t c)
  {
  return ((c & 0xff)<< 16) | (c & 0xff00) | ((c & 0xff0000) >> 16) | (c & 0xff000000);
  }

inline color_t from_color(COLORREF c)
  {
  return ((c & 0xff)<< 16) | (c & 0xff00) | ((c & 0xff00) >> 16) | (c & 0xff000000);
  }

class win_rect_t : public RECT
  {
  public:
    win_rect_t(const kotuku::rect_t &r)
      {
      left = r.left;
      right = r.right;
      top = r.top;
      bottom = r.bottom;
      }
  };

static HPEN as_pen(const pen_t *p)
  {
  return CreatePen(p->style, (int) p->width, to_color(p->color));
  }

inline HBRUSH create_brush(color_t color)
  {
  if(color == color_hollow)
    return (HBRUSH) GetStockObject(NULL_BRUSH);

  return  CreateSolidBrush(to_color(color));
  }

namespace kotuku {
class mem_screen_t : public gdi_screen_t {
public:
  mem_screen_t(HDC dc, const extent_t &ex, size_t bpp);
  ~mem_screen_t();
  };
	}

kotuku::mem_screen_t::mem_screen_t(HDC dc, const extent_t &ex, size_t bpp)
: gdi_screen_t(dc, ex.dx, ex.dy, bpp)
  {

  }

kotuku::mem_screen_t::~mem_screen_t()
  {
  ReleaseDC(NULL, DC());
  }

// canvas implementation routines.
kotuku::screen_t *kotuku::gdi_screen_t::create_canvas(screen_t *parent, const extent_t &ex)
  {
  HDC newDC = CreateCompatibleDC(_dc);
  HBITMAP hBitmap = CreateCompatibleBitmap(_dc, ex.dx, ex.dy);
  SelectObject(newDC, hBitmap);

  size_t bpp = (size_t) GetDeviceCaps(newDC, BITSPIXEL);

  return new mem_screen_t(newDC, ex, bpp);
  }

kotuku::screen_t *kotuku::gdi_screen_t::create_canvas(screen_t *c, const rect_t &rect)
  {
  windows_screen_t *parent_wnd = as_screen_handle(c);
  HWND hwnd = CreateWindowExW(0, child_window_class, screen_name, WS_CHILD | WS_VISIBLE,
    rect.left, rect.top, int(rect.width()), int(rect.height()), parent_wnd->handle(), NULL, NULL, NULL);

  size_t bpp = (size_t) GetDeviceCaps(GetDC(hwnd), BITSPIXEL);

  windows_screen_t *screen = new windows_screen_t(hwnd, rect.width(), rect.height(), parent_wnd->bits_per_pixel);
  SetWindowLongW(hwnd, GWL_USERDATA, LONG(screen));

  return screen;
  }

kotuku::screen_t *kotuku::gdi_screen_t::create_canvas(screen_t *parent, const bitmap_t &bm)
  {
  HDC newDC = CreateCompatibleDC(_dc);

  BITMAPINFO bmi;
  memset(&bmi, 0, sizeof(BITMAPINFO));
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biBitCount = bm.bpp;
  bmi.bmiHeader.biWidth = bm.bitmap_width;
  bmi.bmiHeader.biHeight = -((int)bm.bitmap_height);
  bmi.bmiHeader.biCompression = BI_RGB;

  VOID *pixels;

  HBITMAP hBitmap = CreateDIBSection(GetDC(0), &bmi, DIB_RGB_COLORS, &pixels, NULL, 0);

  memcpy(pixels, bm.pixels, bm.bitmap_width * bm.bitmap_height * sizeof(color_t));

  HGDIOBJ hObj = SelectObject(newDC, hBitmap);
  DeleteObject(hObj);

  size_t bpp = (size_t) GetDeviceCaps(newDC, BITSPIXEL);

  return new mem_screen_t(newDC, extent_t(bm.bitmap_width, bm.bitmap_height), bpp);
  }

int kotuku::gdi_screen_t::_metric = 0;

// drawing functions.
// most are the same as the canvas functions but they
// have the clipping and drawing tools added
void kotuku::gdi_screen_t::polyline(const rect_t &clip_rect,
                            const pen_t *p,
                            const point_t *points,
                            size_t count)
  {
  gdi_opn();

  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  HPEN oldPen = (HPEN) SelectObject(_dc, as_pen(p));

  std::vector<POINT> win_pts;

  for(size_t i = 0; i < count; i++)
    {
    POINT pt = { points[i].x, points[i].y };
    win_pts.push_back(pt);
    }

  Polyline(_dc, win_pts.begin().operator ->(), (int) count);

  DeleteObject(SelectObject(_dc, oldPen));
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::fill_rect(const rect_t &clip_rect,
                             const rect_t &r,
                             color_t c)
  {
  gdi_opn();
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  HBRUSH brush = create_brush(c);

  win_rect_t rect(r);

  FillRect(_dc, &rect, brush);

  DeleteObject(brush);
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::ellipse(const rect_t &clip_rect,
                           const pen_t *p,
                           color_t c,
                           const rect_t &r)
  {
  gdi_opn();
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  HPEN oldPen = (HPEN) SelectObject(_dc, as_pen(p));

  HBRUSH oldBrush = (HBRUSH) SelectObject(_dc, create_brush(c));

  Ellipse(_dc, r.left, r.top, r.right, r.bottom);

  DeleteObject(SelectObject(_dc, oldBrush));
  DeleteObject(SelectObject(_dc, oldPen));
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::polygon(const rect_t &clip_rect,
                           const pen_t *p,
                           color_t c,
                           const point_t *points,
                           size_t count,
                           bool interior_fill)
  {
  gdi_opn();
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  HPEN oldPen = (HPEN) SelectObject(_dc, as_pen(p));
  HBRUSH oldBrush = (HBRUSH) SelectObject(_dc, create_brush(c));

  std::vector<POINT> win_pts;

  for(size_t i = 0; i < count; i++)
    {
    POINT pt = { points[i].x, points[i].y };
    win_pts.push_back(pt);
    }

  Polygon(_dc, win_pts.begin().operator ->(), (int) count);

  DeleteObject(SelectObject(_dc, oldBrush));
  DeleteObject(SelectObject(_dc, oldPen));
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::rectangle(const rect_t &clip_rect,
                             const pen_t *pen,
                             color_t c,
                             const rect_t &r)
  {
  gdi_opn();
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  HPEN oldPen = (HPEN) SelectObject(_dc, as_pen(pen));
  HBRUSH oldBrush = (HBRUSH) SelectObject(_dc, create_brush(c));

  Rectangle(_dc, r.left, r.top, r.right, r.bottom);

  DeleteObject(SelectObject(_dc, oldBrush));
  DeleteObject(SelectObject(_dc, oldPen));
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::round_rect(const rect_t &clip_rect,
                              const pen_t *pen,
                              color_t c,
                              const rect_t &r,
                              const extent_t &e)
  {
  gdi_opn();
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  HPEN oldPen = (HPEN) SelectObject(_dc, as_pen(pen));
  HBRUSH oldBrush = (HBRUSH) SelectObject(_dc, create_brush(c));

  RoundRect(_dc, r.left, r.top, r.right, r.bottom, e.dx, e.dy);

  DeleteObject(SelectObject(_dc, oldBrush));
  DeleteObject(SelectObject(_dc, oldPen));
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::bit_blt(const rect_t &clip_rect,
                           const rect_t &dest_rect,
                           const screen_t *src_screen,
                           const rect_t &src_clip_rect,
                           const point_t &src_pt,
                           raster_operation operation)
  {
  gdi_opn();
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  const gdi_screen_t *src_gdi = reinterpret_cast<const gdi_screen_t *>(src_screen);

  BitBlt(_dc, dest_rect.left, dest_rect.top, dest_rect.width(), dest_rect.height(),
    src_gdi->DC(), src_pt.x, src_pt.y, (DWORD) operation);
  DeleteObject(clipRgn);
  }

void kotuku::gdi_screen_t::mask_blt(const rect_t &clip_rect,
                            const rect_t &dest_rect,
                            const screen_t *src_screen,
                            const rect_t &src_clip_rect,
                            const point_t &src_point,
                            const bitmap_t &mask_bitmap,
                            const point_t &mask_point,
                            raster_operation operation)
  {
  gdi_opn();
  }

void kotuku::gdi_screen_t::rotate_blt(const rect_t &clip_rect,
                              const point_t &dest_center,
                              const screen_t *src,
                              const rect_t &src_clip_rect,
                              const point_t &src_point,
                              size_t radius,
                              double angle,
                              raster_operation operation)
  {
  gdi_opn();
  // we perform this operation only on the circle bounded however for
  // speed we perform this as a retangle
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

      color_t cr = src->get_pixel(src_clip_rect, point_t(x, y));

      if(cr != color_t(-1) && (operation != rop_srcpaint || cr != 0))
        {
        point_t pt(x, y);

        rotate_point(src_point, pt, angle);

        yp = pt.y - src_point.y;
        xp = pt.x - src_point.x; 

        set_pixel(clip_rect, point_t(dest_center.x + xp, dest_center.y + yp), cr);
        }
      }
  }

color_t kotuku::gdi_screen_t::get_pixel(const rect_t &clip_rect, const point_t &p) const
  {
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  COLORREF c = GetPixel(_dc, p.x, p.y);
  DeleteObject(clipRgn);

  return from_color(c);
  }

color_t kotuku::gdi_screen_t::set_pixel(const rect_t &clip_rect,
                                const point_t &p,
                                color_t c)
  {
  HRGN clipRgn = CreateRectRgn(clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom);
  SelectClipRgn(_dc, clipRgn);

  color_t oldColor = from_color(SetPixel(_dc, p.x, p.y, to_color(c)));
  DeleteObject(clipRgn);

  return oldColor;
  }

inline double adjust_radians_to_windows(double angle)
  {
  angle -= rad_90;

  return angle;
  }

/*
Draws an arc. The arc is drawn along the perimeter of a circle with the
given radius and center. The length of the arc is defined by the given
start and sweep angles.

_AngleArc moves the current position to the ending point of the arc.
The arc is drawn by constructing an imaginary circle with the specified
radius around the specified center point. The starting point of the arc is
determined by measuring counterclockwise from the x-axis of the circle by
the number of degrees in the start angle. The ending point is similarly
located by measuring counterclockwise from the starting point by the
number of degrees in the sweep angle.

If the sweep angle is greater than 360 degrees the arc is swept multiple
times. The figure is not filled.
*/
void kotuku::gdi_screen_t::angle_arc(const rect_t &clip_rect,
                             const pen_t *pen,
                             const point_t &p,
                             gdi_dim_t radius,
                             double start_angle,
                             double end_angle)
  {
  assert_valid(this);
  gdi_opn();

  start_angle = adjust_radians_to_windows(start_angle);
  end_angle = adjust_radians_to_windows(end_angle);

  // normalise the drawing numbers
  if(start_angle > end_angle)
    std::swap(start_angle, end_angle);

  point_t pt(radius, 0);

  gdi_dim_t decision_over2 = 1 - pt.x;

  gdi_dim_t offset = pen->width >> 1;

  extent_t rect_offset(0-offset, 0-offset);
  extent_t wide_pen(pen->width, pen->width);
  double rad_45 = degrees_to_radians(45);

  while(pt.y <= pt.x)
    {
    double angle = atan2((double)pt.y, (double)pt.x);

    if(pen->width == 1)
      {
      // forward angles first
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.x + p.x, pt.y + p.y), pen->color);  // octant 1

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.y + p.x, pt.x + p.y), pen->color);  // octant 3

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.x + p.x, -pt.y + p.y), pen->color);   // octant 5
  
      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.y + p.x, -pt.x + p.y), pen->color);    // octant 7

      angle -= rad_270;
      angle *= -1;
      angle += rad_90;

      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.y + p.x, pt.x + p.y), pen->color); // octant 2

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.x + p.x, pt.y + p.y), pen->color);  // octant 4

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.y + p.x, -pt.x + p.y), pen->color);   // octant 6

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.x + p.x, -pt.y + p.y), pen->color);  // octant 8
      }
    else
      {
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(pt.x + p.x, pt.y + p.y), wide_pen) + rect_offset);  // octant 1

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(-pt.y + p.x, pt.x + p.y), wide_pen) + rect_offset); // octant 3

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)  
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(-pt.x + p.x, -pt.y + p.y), wide_pen) + rect_offset);  // octant 5

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(pt.y + p.x, -pt.x + p.y), wide_pen) + rect_offset);   // octant 7

      angle -= rad_270;
      angle *= -1;
      angle += rad_90;

      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(pt.y + p.x, pt.x + p.y), wide_pen) + rect_offset);  // octant 2

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(-pt.x + p.x, pt.y + p.y), wide_pen) + rect_offset); // octant 4

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(-pt.y + p.x, -pt.x + p.y), wide_pen) + rect_offset);  // octant 6

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color, rect_t(point_t(pt.x + p.x, -pt.y + p.y), wide_pen) + rect_offset);   // octant 8
      }

    pt.y++;
    if(decision_over2 <= 0)
      decision_over2 += 2 * pt.y + 1; // Change in decision criterion for y -> y+1
    else
      {
      pt.x--;
      decision_over2 += 2 * (pt.y - pt.x) + 1;   // Change for y -> y+1, x -> x-1
      }
    }
  }

void kotuku::gdi_screen_t::pie(const rect_t &clip_rect,
                       const pen_t *pen,
                       color_t c,
                       const point_t &p,
                       double start,
                       double end,
                       gdi_dim_t radii,
                       gdi_dim_t inner)
  {
  gdi_opn();
  if(end < start)
    std::swap(start, end);

  // special case for start == end
  if(start == end)
    {
    point_t pts[2] = {
      point_t(p.x + inner, p.y),
      point_t(p.x + radii, p.y)
      };

    rotate_point(p, pts[0], start);
    rotate_point(p, pts[1], start);

    polyline(clip_rect, pen, pts, 2);
    return;
    }
  // we draw a polygon using the current pen
  // and brush. We first need to calculate the outer
  // point count
  double outer_incr = atan2(1.0, radii);
  double delta = end - start;

  delta = (delta < 0) ? -delta : delta;

  long segs = long(delta / outer_incr)+1;

  double inner_incr;
  
  // see if we have an inner radii
  if(inner > 0)
    {
    inner_incr = atan2(1.0, inner);
    segs += long(delta / inner_incr)+1;
    }
  else
    segs++;
  
  // We allocate 1 more point so that the polyline will fill the area.
  std::auto_ptr<point_t> pts(new point_t[segs +1]);
  
  int pt = 1;
  pts.get()[0] = p;
  double theta;
  for(theta = start; theta <= end; theta += outer_incr)
    {
    pts.get()[pt].x = p.x + radii; pts.get()[pt].y = p.y;
    rotate_point(p, pts.get()[pt], theta);
    pt++;
    }
  
  if(inner > 0)
    {
    for(theta = end; theta >= start; theta -= inner_incr)
      {
      pts.get()[pt].x = p.x + inner; pts.get()[pt].y = p.y;
      rotate_point(p, pts.get()[pt], theta);
      pt++;
      }
    }
  else
    pts.get()[pt] = p;

  polygon(clip_rect, pen, c, pts.get(), segs+1, false);
  }

void kotuku::gdi_screen_t::draw_text(const rect_t &clip_rect,
                             const font_t *font,
                             color_t fg,
                             color_t bg,
                             const char *str,
                             size_t count,
                             const point_t &src_pt,
                             const rect_t &txt_clip_rect,
                             text_flags format,
                             size_t *char_widths)
  {
  gdi_opn();

  
  // create a clipping rectangle as needed
  rect_t txt_rect(clip_rect);
  point_t pt_pos = src_pt;

  if(format & eto_clipped)
    txt_rect &= txt_clip_rect;

  COLORREF fgc = to_color(fg);
  COLORREF bgc = to_color(bg);

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

    size_t columns = (((cell_width -1) | 7) +1) >> 3;
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
              set_pixel(clip_rect, pos, fgc);
            else if(format & eto_opaque)
              set_pixel(clip_rect, pos, bgc);
            }
          }

        // increment past the current row
        mask++;
        }
      }

    pt_pos.x += long(cell_width);
    }
  }

kotuku::extent_t kotuku::gdi_screen_t::text_extent(const font_t *font,
                                   const char *str,
                                   size_t count) const
  {
  extent_t ex(0, long(font->bitmap_height));

  for(size_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];
    if(c == 0)
      break ;							// end of the std::string

    if(c > font->last_char || c < font->first_char)
      c = font->default_char;

    c -= font->first_char;

    ex.dx += font->char_table[c << 1];
    }

  return ex;
  }

void kotuku::gdi_screen_t::scroll(const rect_t &clip_rect,
                          const extent_t &offsets,
                          const rect_t &area_to_scroll,
                          const rect_t &clipping_rectangle,
                          rect_t *rect_update)
  {

  }

void kotuku::gdi_screen_t::background_mode(int m)
  {
  ::SetBkMode(_dc, m);
  }

void kotuku::gdi_screen_t::invalidate_rect(const rect_t &rect)
  {

  }

void kotuku::gdi_screen_t::update_window()
  {

  }

int kotuku::gdi_screen_t::z_order() const
  {
  return 0;
  }

void kotuku::gdi_screen_t::z_order(int order)
  {
  }
