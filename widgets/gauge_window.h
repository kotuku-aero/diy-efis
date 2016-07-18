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
#ifndef __gauge_window_h__
#define __gauge_window_h__

#include "../gdi-lib/application.h"
#include "../gdi-lib/window.h"
#include "../gdi-lib/bitmap.h"
#include "../gdi-lib/can_aerospace.h"
#include "fonts.h"
#include "widget.h"
#include "pens.h"

namespace kotuku {
enum gauge_style
  {
  gs_pointer, // a simple line
  gs_sweep, // a pie sweep
  gs_bar, // line sweep
  gs_point, // simple-pointer
  gs_pointer_minmax, // a pointer with min-max markers
  gs_point_minmax, // simple-pointer with min-max markers
  // bar graph styles
  bgs_point, // simple-pointer
  bgs_pointer_minmax, // a pointer with min-max markers
  bgs_pointer_max, // simple-pointer with max markers
  bgs_pointer_min
  };

struct step
  {
  gdi_dim_t value;
  pen_t pen;
  color_t gauge_color;
  };

struct tick_mark
  {
  gdi_dim_t value;
  std::string text;
  };

class gauge_window_t : public widget_t
  {
public:
  gauge_window_t(widget_t &parent, const char *section);
protected:
  virtual void update_window();

  virtual gauge_style style() const;
  virtual const point_t &center() const;
  virtual int gauge_radii() const;
  virtual const font_t *gauge_font() const;
  virtual double arc_begin() const;
  virtual double arc_range() const;
  virtual double reset_value() const;

  canvas_t _background_canvas;
private:
  virtual bool ev_msg(const msg_t &);

  color_t _background_color;

  const font_t *_font;
  // this is updated on each window message
  std::vector<double> _values;
  std::vector<double> _min_values;
  std::vector<double> _max_values;
  std::vector<uint16_t> _labels;

  uint16_t _reset_label;
  uint16_t _label;

  point_t _center;
  gdi_dim_t _gauge_radii;

  double _arc_begin;
  double _arc_range;
  double _reset_value;

  gauge_style _style;

  std::vector<step> _steps;
  std::vector<tick_mark> _ticks;

  double _scale;
  double _offset;

  bool is_bar_style() const;
  void draw_pointer(const pen_t *pen, double rotation);
  void draw_sweep(const pen_t *pen, color_t fill, double rotation);
  void draw_bar(const pen_t *pen, double rotation);
  void draw_point(const pen_t *pen, double rotation);
  double calculate_rotation(double value);
  const pen_t *calculate_pen1(double value);
  const pen_t *calculate_pen3(double value);
  const pen_t *calculate_pen5(double value);
  color_t calculate_color(double value);

  void draw_point(const pen_t *pen, size_t index, gdi_dim_t offset);
  void draw_bar(canvas_t &, const rect_t &rect);
  };
  };

#endif

