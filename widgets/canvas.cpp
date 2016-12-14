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
#include "application.h"
#include "canvas.h"
#include "hal.h"
#include "screen.h"
#include "window.h"

static pen_t white_pen = {
    rgb(255, 255, 255),
    1,
    ps_solid
  };

kotuku::canvas_t::~canvas_t()
  {
  }

kotuku::canvas_t::canvas_t(screen_t *screen,
                   const rect_t &rect)
  {
  _screen = screen;
  _window_rect = rect_t(point_t(0, 0), rect.extents());
  _clipping_rect = _window_rect;
  _pen = &white_pen;
  _font = 0;
  _background_color = color_t(0);
  _text_color = color_t(rgb(255, 255, 255));
  _invalid = false;
  }

kotuku::canvas_t::canvas_t(const rect_t &rect)
  {
  _screen = application_t::hal->screen_create(rect);
  _window_rect = rect_t(point_t(0, 0), rect.extents());
  _clipping_rect = _window_rect;
  _pen = &white_pen;
  _font = 0;
  _background_color = color_t(0);
  _text_color = color_t(rgb(255, 255, 255));
  _invalid = false;
  }

kotuku::canvas_t::canvas_t(const extent_t &e)
  {
  _screen = application_t::hal->screen_create(e);
  _window_rect = rect_t(point_t(0, 0), e);
  _clipping_rect = _window_rect;
  _pen = &white_pen;
  _font = 0;
  _background_color = color_t(0);
  _text_color = color_t(rgb(255, 255, 255));
  _invalid = false;
  }

kotuku::canvas_t::canvas_t(const bitmap_t &b)
  {
  _screen = application_t::hal->screen_create(b);
  _window_rect = rect_t(point_t(0, 0), extent_t(long(b.bitmap_width), long(b.bitmap_height)));
  _clipping_rect = _window_rect;
  _pen = &white_pen;
  _font = 0;
  _background_color = color_t(0);
  _text_color = color_t(rgb(255, 255, 255));
  _invalid = false;
  }

const pen_t *kotuku::canvas_t::pen(const pen_t *p)
  {
  return swap_gdi_object(_pen, p);
  }

const pen_t *kotuku::canvas_t::pen() const
  {
  return _pen;
  }

const font_t *kotuku::canvas_t::font(const font_t *f)
  {
  return swap_gdi_object(_font, f);
  }

const font_t *kotuku::canvas_t::font() const
  {
  return _font;
  }

color_t kotuku::canvas_t::background_color() const
  {
  return _background_color;
  }

color_t kotuku::canvas_t::background_color(color_t c)
  {
  return swap_gdi_object(_background_color, c);
  }

color_t kotuku::canvas_t::text_color() const
  {
  return _text_color;
  }

color_t kotuku::canvas_t::text_color(color_t c)
  {
  return swap_gdi_object(_text_color, c);
  }

kotuku::rect_t kotuku::canvas_t::clipping_rectangle(const rect_t &r)
  {
  rect_t retn = _clipping_rect;
  _clipping_rect = r + _window_rect.top_left();
  return retn - _window_rect.top_left();
  }

kotuku::rect_t kotuku::canvas_t::clipping_rectangle() const
  {
  return _clipping_rect - _window_rect.top_left();
  }

bool kotuku::canvas_t::is_rect_visible(const rect_t &r) const
  {
  return _clipping_rect || rect_t(r + _window_rect.top_left());
  }

void kotuku::canvas_t::polyline(const point_t *points,
                               size_t count)
  {
  if(_window_rect.left != 0 || _window_rect.top != 0)
    {
    std::auto_ptr<point_t> pts(new point_t[count]);

    for(size_t n = 0; n < count; n++)
      {
      pts.get()[n].x = points[n].x + _window_rect.left;
      pts.get()[n].y = points[n].y + _window_rect.top;
      }

    _screen->polyline(this, _clipping_rect, _pen, pts.get(), count);
    }
  else
    _screen->polyline(this, _clipping_rect, _pen, points, count);
  }

void kotuku::canvas_t::fill_rect(const rect_t &r, color_t c)
  {
  _screen->fill_rect(this, _clipping_rect, r + _window_rect.top_left(), c);
  }

void kotuku::canvas_t::ellipse(const rect_t &r)
  {
  _screen->ellipse(this, _clipping_rect,
    _pen,
    _background_color,
    r + _window_rect.top_left());
  }

void kotuku::canvas_t::polygon(const point_t *points,
                       size_t count,
                       bool interior_fill)
  {
  // see if we have to expand the array
  bool expand_array = !(points[0] == points[count-1]);
  bool copy_array = expand_array ||
                    _window_rect.top != 0 || 
                    _window_rect.left != 0;
  if(copy_array)
    {
    std::auto_ptr<point_t> pts(new point_t[count + (expand_array ? 1 : 0)]);

    for(size_t n = 0; n < count; n++)
      {
      pts.get()[n].x = points[n].x + _window_rect.top_left().x;
      pts.get()[n].y = points[n].y + _window_rect.top_left().y;
      }

    if(expand_array)
      {
      pts.get()[count].x = points[0].x + _window_rect.top_left().x;
      pts.get()[count].y = points[0].y + _window_rect.top_left().y;
      count++;
      }

    _screen->polygon(this, _clipping_rect,
      _pen,
      _background_color,
      pts.get(),
      count,
      interior_fill);
    }
  else
    _screen->polygon(this, _clipping_rect,
    _pen,
    _background_color,
    points,
    count,
    interior_fill);
  }

void kotuku::canvas_t::rectangle(const rect_t &r)
  {
  _screen->rectangle(this, _clipping_rect,
    _pen,
    _background_color,
    r + _window_rect.top_left());
  }

void kotuku::canvas_t::round_rect(const rect_t &r,
                                 const extent_t &e)
  {
  _screen->round_rect(this, _clipping_rect,
    _pen,
    _background_color,
    r + _window_rect.top_left(),
    e);
  }

void kotuku::canvas_t::bit_blt(const rect_t &dest_rect,
                              const canvas_t &src_canvas,
                              const point_t &src_pt)
  {
  _screen->bit_blt(this, _clipping_rect,
    dest_rect + _window_rect.top_left(),
    src_canvas._screen,
    src_canvas._clipping_rect,
    point_t(src_pt.x + src_canvas._window_rect.top_left().x,
    src_pt.y + src_canvas._window_rect.top_left().y));
  }

void kotuku::canvas_t::rotate_blt(const point_t &dest_center,
                                 const canvas_t &src_canvas,
                                 const point_t &src_point,
                                 size_t radius,
                                 double angle)
  {
  _screen->rotate_blt(this, _clipping_rect,
    point_t(dest_center.x + _window_rect.top_left().x,
    dest_center.y + _window_rect.top_left().y),
    src_canvas._screen,
    src_canvas._clipping_rect,
    point_t(src_point.x + src_canvas._window_rect.top_left().x,
    src_point.y + src_canvas._window_rect.top_left().y),
    radius,
    angle);
  }

color_t kotuku::canvas_t::get_pixel(const point_t &p) const
  {
  return _screen->get_pixel(this, _clipping_rect,
    point_t(p.x + _window_rect.top_left().x,
    p.y + _window_rect.top_left().y));
  }

color_t kotuku::canvas_t::set_pixel(const point_t &p,
                                   color_t c)
  {
  return _screen->set_pixel(this, _clipping_rect,
    point_t(p.x + _window_rect.top_left().x,
    p.y + _window_rect.top_left().y),
    c);
  }

void kotuku::canvas_t::angle_arc(const point_t &pt,
                                gdi_dim_t radius,
                                double start,
                                double end)
  {
  _screen->angle_arc(this, _clipping_rect, _pen,
    point_t(pt.x + _window_rect.top_left().x,
    pt.y + _window_rect.top_left().y), radius, start, end);
  }

void kotuku::canvas_t::pie(const point_t &pt,
                          double start,
                          double end,
                          gdi_dim_t radii,
                          gdi_dim_t inner)
  {
  _screen->pie(this, _clipping_rect,
    _pen,
    _background_color,
    point_t(pt.x + _window_rect.top_left().x,
    pt.y + _window_rect.top_left().y), start, end, radii, inner);
  }

void kotuku::canvas_t::draw_text(const char *str,
                                size_t count,
                                const point_t &pt,
                                text_flags format,
                                size_t *char_widths)
  {
 if(str == 0)
  return;

 if(count == -1)
  count = strlen(str);

  _screen->draw_text(this, _clipping_rect,
    _font, _text_color,
    _background_color,
    str, count,
    point_t(pt.x + _window_rect.left, pt.y + _window_rect.top),
    _clipping_rect,
    format, char_widths);
  }

void kotuku::canvas_t::draw_text(const char *str,
                                size_t count,
                                const point_t &pt,
                                const rect_t &clip_rect,
                                text_flags format,
                                size_t *char_widths)
  {
  _screen->draw_text(this, _clipping_rect,
    _font,
    _text_color,
    _background_color,
    str, count,
    point_t(pt.x + _window_rect.left, pt.y + _window_rect.top),
    clip_rect + _window_rect.top_left(),
    format, char_widths);
  }

kotuku::extent_t kotuku::canvas_t::text_extent(const char *str, size_t count) const
  {
  return _screen->text_extent(this, _font, str, count);
  }

const kotuku::rect_t &kotuku::canvas_t::window_rect() const
  {
  return _window_rect;
  }

void kotuku::canvas_t::window_rect(const rect_t &r)
  {
  _clipping_rect = rect_t(
    point_t(_clipping_rect.left - _window_rect.left,
    _clipping_rect.top - _window_rect.top),
    _clipping_rect.extents());
  _window_rect = r;
  _clipping_rect.left += r.left;
  _clipping_rect.right += r.left;
  _clipping_rect.top += r.top;
  _clipping_rect.bottom += r.top;
  }


void kotuku::canvas_t::invalidate()
  {
  _invalid = true;

  _screen->invalidate_rect(this, _window_rect);
  }

kotuku::window_t *kotuku::canvas_t::as_window()
  {
  return 0;
  }

const kotuku::window_t *kotuku::canvas_t::as_window() const
  {
  return 0;
  }
