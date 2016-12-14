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
#include "widget.h"
#include "gauge_window.h"

#include "spatial.h"

static const char *gauge_style_values[] =
  {
      "gs_pointer",
      "gs_sweep",
      "gs_bar",
      "gs_point",
      "gs_pointer_minmax",
      "gs_point_minmax",
      "bgs_point",
      "bgs_pointer_minmax",
      "bgs_pointer_max",
      "bgs_pointer_min"
  };

static result_t split_param(const std::string &str,
    std::vector<std::string> &params)
  {
  params.clear();
  // we split the params using a ','
  std::string::size_type beg = 0;
  std::string::size_type end = str.find(',');
  while(end != std::string::npos)
    {
    params.push_back(str.substr(beg, end - beg));
    beg = end + 1;

    end = str.find(',', beg);
    }

  if(beg > 0)
    params.push_back(str.substr(beg));
  else if(beg == 0)
    params.push_back(str);

  return s_ok;
  }

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

kotuku::gauge_window_t::gauge_window_t(widget_t &parent, const char *section)
: widget_t(parent, section),
  _background_canvas(window_rect().extents())
  {
  char temp_name[32];
  int value;

  if(failed(application_t::instance->hal()->get_config_value(section, "scale", _scale)))
    _scale = 1.0;

  if(failed(application_t::instance->hal()->get_config_value(section, "offset", _offset)))
    _offset = 0.0;

  value = lookup_enum_setting(section, "style", gauge_style_values,
      bgs_pointer_min + 1);
  if(value > -1)
    _style = (gauge_style) value;
  else
    _style = gs_pointer;

  if(failed(application_t::instance->hal()->get_config_value(section, "reset-id", value)))
    value = 0;

  _reset_label = value;

  if(failed(application_t::instance->hal()->get_config_value(section, "reset-value", _reset_value)))
    _reset_value = 0.0;

  _font = lookup_font(section, "font");
  if(_font == 0)
    _font = &arial_9_font;

  if(failed(application_t::instance->hal()->get_config_value(section, "arc-begin", _arc_begin)))
    _arc_begin = 120;

  if(failed(application_t::instance->hal()->get_config_value(section, "arc-range", _arc_range)))
    _arc_range = 270;

  if(failed(application_t::instance->hal()->get_config_value(section, "can-id", value)))
    {
    // could be can value 0..3
    for(int id = 0; id < 4; id++)
      {
      snprintf(temp_name, 32, "can-id-%d", id);

      if(failed(application_t::instance->hal()->get_config_value(section, temp_name, value)))
        break;

      _labels.push_back((uint16_t) value);
      _values.push_back(_reset_value);
      _min_values.push_back(_reset_value);
      _max_values.push_back(_reset_value);
      }
    }
  else
    {
    _labels.push_back((uint16_t) value);
    _values.push_back(_reset_value);
    _min_values.push_back(_reset_value);
    _max_values.push_back(_reset_value);
    }

  if(failed(application_t::instance->hal()->get_config_value(section, "center-x", value)))
    _center.x = window_rect().width() >> 1;
  else
    _center.x = (gdi_dim_t)value;

  if(failed(application_t::instance->hal()->get_config_value(section, "center-y", value)))
    _center.y = window_rect().height() >> 1;
  else
    _center.y = (gdi_dim_t)value;

  if(failed(application_t::instance->hal()->get_config_value(section, "radii", value)))
    value = (window_rect().width() >> 1) - 5;

  _gauge_radii = (gdi_dim_t)value;

  std::vector<std::string> params;
  std::string value_str;
  // only support 20 ticks/steps
  for(int i = 0; i < 20; i++)
    {
    snprintf(temp_name, 32, "step-%d", i);

    if(failed(application_t::instance->hal()->get_config_value(section, temp_name, value_str)))
      break;

    // the step is a series of settings in the form:
    // step-0=15, color_orange, 5, ps_solid , color_light_blue
    // param1 -> step level
    // param2 -> pen color
    // param3 -> pen width
    // param4 -> pen style
    // param5 -> pointer color
    if(failed(split_param(value_str, params)))
      {
      trace_warning("Parameter %s=%s is not propertly formed\n", temp_name,
          value_str.c_str());
      continue;
      }

    if(params.size() != 5)
      {
      trace_warning("Parameter %s=%s is does not have 5 fields\n", temp_name,
          value_str.c_str());
      continue;
      }

    step new_step;
    new_step.value = atoi(params[0].c_str());
    new_step.pen.color = lookup_color(params[1].c_str());
    new_step.pen.width = (size_t) atoi(params[2].c_str());
    new_step.pen.style = lookup_pen_style(params[3].c_str());
    new_step.gauge_color = lookup_color(params[4].c_str());

    _steps.push_back(new_step);
    }

  // only support 20 ticks/steps
  for(int i = 0; i < 20; i++)
    {
    snprintf(temp_name, 32, "tick-%d", i);

    if(failed(application_t::instance->hal()->get_config_value(section, temp_name, value_str)))
      break;

    // the step is a series of settings in the form:
    // tick-0=650, 650
    // param1 -> tick point
    // param2 -> tick label
    if(failed(split_param(value_str, params)))
      {
      trace_warning("Parameter %s=%s is not propertly formed\n", temp_name,
          value_str.c_str());
      continue;
      }

    if(params.size() != 1 && params.size() != 2)
      {
      trace_warning("Parameter %s=%s is does not have 1 or 2 fields\n",
          temp_name, value_str.c_str());
      continue;
      }

    tick_mark new_tick;
    new_tick.value = atoi(params[0].c_str());
    if(params.size() > 1)
      new_tick.text = params[1];

    _ticks.push_back(new_tick);
    }

  std::string color;
  if(failed(application_t::instance->hal()->get_config_value(section, "background-color", color)))
    _background_color = color_black;
  else
    _background_color = lookup_color(color.c_str());

  rect_t r(point_t(0, 0), _background_canvas.window_rect().extents());

  _background_canvas.fill_rect(r, _background_color);
  _background_canvas.background_color(_background_color);
  _background_canvas.pen(&light_gray_pen);

  if(draw_border())
    _background_canvas.round_rect(r, extent_t(12, 12));

  if(is_bar_style())
    {
    // we support up to 4 ID's.

    // each bar is drawn with the following dimensions:
    //
    //  *****************
    //                5 pixels
    //        ********
    //        *      *
    //        *      *
    //   +    *      *
    //   |\   *      *
    //   | \  *      *
    //   |  * *      *
    //   | /  *      *
    //   |/   *      *
    //   +    *      *
    //        ********
    //                5 pixels
    //  *****************
    //  ^ ^  ^    ^    ^
    //  | |  |    |    +- 2 Pixels
    //  | |  |    +------ 6 Pixels
    //  | |  +----------- 1 Pixel
    //  | +-------------- 5 Pixels
    //  +---------------- 1 Pixel
    //
    //
    rect_t graph(r.top_left(), extent_t(14, r.height()));

    for(size_t i = 0; i < _labels.size(); i++)
      {
      draw_bar(_background_canvas, graph);
      graph.left += 14;
      graph.right += 14;
      }

    _background_canvas.font(&arial_9_font);
    _background_canvas.background_color(color_black);
    _background_canvas.text_color(color_white);

    extent_t sz = _background_canvas.text_extent(name().c_str(), name().length());
    point_t pt(r.bottom_right());

    pt.x -= 4;
    pt.x -= sz.cx;
    pt.y -= sz.cy;

    _background_canvas.draw_text(name().c_str(), name().length(), pt);

    long height = graph.height() - 12;

    long range = _steps.rbegin()->value - _steps.begin()->value;
    double pixels_per_unit = ((double) height) / ((double) range);

    for(size_t i = 0; i < _ticks.size(); i++)
      {
      if(_ticks[i].text.empty())
        continue;

      long value = _ticks[i].value;

      extent_t sz = _background_canvas.text_extent(_ticks[i].text.c_str(),
          _ticks[i].text.length());

      double relative_value = value - _steps.begin()->value;
      double pixels = relative_value * pixels_per_unit;
      point_t pt(graph.left + 2,
          (graph.bottom - gdi_dim_t(pixels)) - 7 - (sz.cy >> 1));

      _background_canvas.draw_text(_ticks[i].text.c_str(),
          _ticks[i].text.length(), pt);
      }
    }
  else
    {
    if(!name().empty())
      {
      _background_canvas.font(&arial_9_font);
      _background_canvas.background_color(color_black);
      _background_canvas.text_color(color_white);

      extent_t sz = _background_canvas.text_extent(name().c_str(),
          name().length());
      point_t pt(r.bottom_right());

      pt.x -= 4;
      pt.x -= sz.cx;
      pt.y -= sz.cy;

      _background_canvas.draw_text(name().c_str(), name().length(), pt);
      }

    // we now draw an arc.  The range is the last value in the arc
    // minus the first value.  Note that the values can be -
    // the range of the gauge is gauge_y - 360 degrees
    long min_range = _steps.begin()->value;
    long max_range = _steps.rbegin()->value;

    long steps_range = max_range - min_range;

    double degrees_per_unit = arc_range() / double(steps_range);

    double arc_start = arc_begin();
    const pen_t *old_pen = _background_canvas.pen();

    size_t i;

    font(_font);
    background_color(color_black);
    text_color(color_white);

    for(i = 1; i < _steps.size(); i++)
      {
      // the arc angle is the step end
      double arc_end = double(_steps[i].value - min_range) * degrees_per_unit;
      arc_end += arc_begin();

      // The outline pen is 3 pixels wide
      _background_canvas.pen(&_steps[i].pen);

      // draw arc
      _background_canvas.angle_arc(center(), gauge_radii(),
          degrees_to_radians(arc_start), degrees_to_radians(arc_end));

      arc_start = arc_end;
      }

    // we now draw the tick marks.  They are a line from the point
    // 53 - 58(5 pixels).

    // we create a white pen for this
    _background_canvas.pen(&white_pen_2);
    rect_t clip_rect = clipping_rectangle();

    for(i = 0; i < _ticks.size(); i++)
      {
      arc_start = double(_ticks[i].value - min_range) * degrees_per_unit;
      arc_start += arc_begin();

      point_t pts[2];

      pts[0] = center();
      pts[0].y -= gauge_radii() + 5;

      rotate_point(center(), pts[0], degrees_to_radians(arc_start));

      pts[1] = center();
      pts[1].y -= gauge_radii() + 1;

      rotate_point(center(), pts[1], degrees_to_radians(arc_start));

      _background_canvas.polyline(pts, 2);

      size_t len = _ticks[i].text.length();
      if(len > 0)
        {
        // we use the imageDC for this
        fill_rect(r, color_black);

        // write the text at the point
        extent_t size = text_extent(_ticks[i].text.c_str(), len);
        rect_t text_rect(
            point_t(center().x - (size.cx >> 1),
                center().y - (gauge_radii() + 5 + size.cy)), size);

        text_rect &= clip_rect;
        clipping_rectangle(text_rect);

        draw_text(_ticks[i].text.c_str(), len, text_rect.top_left());

        // we now rotate and merge the text
        _background_canvas.rotate_blt(center(), *this, center(), center().y,
            degrees_to_radians(arc_start));
        }
      }

    clipping_rectangle(clip_rect);
    }

  if(_reset_label != 0)
    subscribe(_reset_label);

  for(std::vector<uint16_t>::const_iterator it = _labels.begin();
      it != _labels.end(); it++)
    subscribe(*it);
  }

bool kotuku::gauge_window_t::ev_msg(const msg_t &data)
  {
  bool changed = false;

  if(data.message_id() == _reset_label)
    {
    for(size_t i = 0; i < 4; i++)
      {
      _min_values[i] = reset_value();
      _max_values[i] = _min_values[i];
      }
    changed = true;
    }
  else
    for(size_t i = 0; i < _labels.size(); i++)
      {
      if(data.message_id() == _labels[i])
        {
        double value = data.value<double>();
        value *= _scale;
        value += _offset;

        changed |= assign_msg(value, _values[i]);
        _min_values[i] = min(_min_values[i], _values[i]);
        _max_values[i] = max(_max_values[i], _values[i]);
        break;
        }
      }

  if(changed)
    invalidate();

  return true;
  }

bool kotuku::gauge_window_t::is_bar_style() const
  {
  return _style >= bgs_point;
  }

void kotuku::gauge_window_t::update_window()
  {
  rect_t window_size(0, 0, window_rect().width(), window_rect().height());

  clipping_rectangle(window_size);
  bit_blt(window_size, _background_canvas, point_t(0, 0));
 
  if(is_bar_style())
    {
    rect_t window_size(0, 0, window_rect().width(), window_rect().height());

    gdi_dim_t offset = 1;
    for(size_t i = 0; i < _labels.size(); i++)
      {
      draw_point(&lightblue_pen, i, offset);
      offset += 14;
      }
    }
  else
    {
    // we now calculate the range of the segments.  There are 8 segments
    // around a 240 degree arc.  We draw the nearest based on the range
    // of the gauge.

    switch (style())
      {
    case gs_pointer_minmax:
      draw_point(calculate_pen1(_min_values[0]),
          calculate_rotation(_min_values[0]));
      draw_point(calculate_pen1(_max_values[0]),
          calculate_rotation(_max_values[0]));
    case gs_pointer:
      draw_pointer(calculate_pen3(_values[0]), calculate_rotation(_values[0]));
      break;
    case gs_sweep:
      draw_sweep(calculate_pen1(_values[0]), calculate_color(_values[0]),
          calculate_rotation(_values[0]));
      break;
    case gs_bar:
      draw_bar(calculate_pen5(_values[0]), calculate_rotation(_values[0]));
      break;
    case gs_point_minmax:
      draw_point(calculate_pen1(_min_values[0]),
          calculate_rotation(_min_values[0]));
      draw_point(calculate_pen1(_max_values[0]),
          calculate_rotation(_max_values[0]));
    case gs_point:
      draw_point(calculate_pen1(_values[0]), calculate_rotation(_values[0]));
      break;
      }

    // draw the value of the gauge as a text string
    if(_font != 0)
      {
      font(_font);
      text_color(color_lightblue);
      background_color(color_black);

      char str[32];
      sprintf(str, "%d", (int) _values[0]);

      size_t len = strlen(str);
      extent_t size = text_extent(str, len);

      pen(&gray_pen);
      background_color(color_black);

      // draw a rectangle around the text
      rect_t rect_text;

      rect_text.left = center().x + 2;
      rect_text.top = center().y + 6;
      rect_text.bottom = rect_text.top + size.cy + 4;
      rect_text.right = window_size.width() - 1;

      rectangle(rect_text);

      point_t pt(rect_text.right - (size.cx + 2), rect_text.top + 2);

      draw_text(str, len, point_t(pt.x, pt.y), rect_text, eto_clipped);
      }
    }
  }

void kotuku::gauge_window_t::draw_pointer(const pen_t *outline_pen,
    double rotation)
  {
  // draw the marker line
  point_t pts[2] =
    {
    point_t(center().x, center().y - 5), point_t(center().x,
        center().y - gauge_radii() + 5)
    };

  rotate_point(center(), pts[0], rotation);
  rotate_point(center(), pts[1], rotation);
  pen(outline_pen);
  polyline(pts, 2);
  }

void kotuku::gauge_window_t::draw_sweep(const pen_t *outline_pen,
    color_t fill_color, double rotation)
  {
  pen(outline_pen);
  background_color(fill_color);
  pie(center(), degrees_to_radians(arc_begin()), rotation,
      center().y - gauge_radii() + 5, 5);
  }

void kotuku::gauge_window_t::draw_bar(const pen_t *outline_pen, double rotation)
  {
  pen(outline_pen);
  angle_arc(center(), gauge_radii() - 6, degrees_to_radians(arc_begin()),
      rotation);
  }

void kotuku::gauge_window_t::draw_point(const pen_t *outline_pen,
    double rotation)
  {
  point_t pts[4] =
    {
    point_t(center().x, center().y - gauge_radii() + 5), point_t(center().x - 6,
        center().y - gauge_radii() + 11), point_t(center().x + 6,
        center().y - gauge_radii() + 11), point_t(center().x,
        center().y - gauge_radii() + 5),
    };
  rotate_point(center(), pts[0], rotation);
  rotate_point(center(), pts[1], rotation);
  rotate_point(center(), pts[2], rotation);
  pts[3] = pts[0];
  pen(outline_pen);
  background_color(outline_pen->color);
  polygon(pts, 4, true);
  }

double kotuku::gauge_window_t::calculate_rotation(double value)
  {
  double min_range = _steps.begin()->value;
  double max_range = _steps.rbegin()->value;

  // get the percent that the gauge is displaying
  double percent = double(
      max(min(value, max_range), min_range) - min_range)
      / (max_range - min_range);

  double rotation;
  rotation = arc_begin() + (arc_range() * percent);
  rotation = degrees_to_radians(rotation);

  return rotation;
  }

const pen_t *kotuku::gauge_window_t::calculate_pen1(double value)
  {
  const pen_t *pen1 = &lightblue_pen;

  for(size_t i = 0; i < _steps.size(); i++)
    {
    if(i == _steps.size() - 1
        || value >= _steps[i].value && value < _steps[i + 1].value)
      {
      switch (_steps[i].gauge_color)
        {
      case color_white:
        pen1 = &white_pen;
        break;
      case color_red:
        pen1 = &red_pen;
        break;
      case color_lightblue:
        pen1 = &lightblue_pen;
        break;
      case color_orange:
        pen1 = &orange_pen;
        break;
      case color_green:
        pen1 = &green_pen;
        break;
      default:
        break;
        }
      break;
      }
    }

  return pen1;
  }

const pen_t *kotuku::gauge_window_t::calculate_pen3(double value)
  {
  const pen_t *pen3 = &lightblue_pen_3;

  for(size_t i = 0; i < _steps.size(); i++)
    {
    if(i == _steps.size() - 1
        || value >= _steps[i].value && value < _steps[i + 1].value)
      {
      switch (_steps[i].gauge_color)
        {
      case color_white:
        pen3 = &white_pen_3;
        break;
      case color_red:
        pen3 = &red_pen_3;
        break;
      case color_lightblue:
        pen3 = &lightblue_pen_3;
        break;
      case color_orange:
        pen3 = &orange_pen_3;
        break;
      case color_green:
        pen3 = &green_pen_3;
        break;
      default:
        break;
        }
      break;
      }
    }

  return pen3;
  }

const pen_t *kotuku::gauge_window_t::calculate_pen5(double value)
  {
  const pen_t *pen5 = &lightblue_pen_5;

  for(size_t i = 0; i < _steps.size(); i++)
    {
    if(i == _steps.size() - 1
        || value >= _steps[i].value && value < _steps[i + 1].value)
      {
      switch (_steps[i].gauge_color)
        {
      case color_white:
        pen5 = &white_pen_5;
        break;
      case color_red:
        pen5 = &red_pen_5;
        break;
      case color_lightblue:
        pen5 = &lightblue_pen_5;
        break;
      case color_orange:
        pen5 = &orange_pen_5;
        break;
      case color_green:
        pen5 = &green_pen_5;
        break;
      default:
        break;
        }
      break;
      }
    }

  return pen5;
  }

color_t kotuku::gauge_window_t::calculate_color(double value)
  {
  color_t fill_color = color_lightblue;

  for(size_t i = 0; i < _steps.size(); i++)
    {
    if(i == _steps.size() - 1
        || value >= _steps[i].value && value < _steps[i + 1].value)
      {
      switch (_steps[i].gauge_color)
        {
      case color_white:
        fill_color = color_white;
        break;
      case color_red:
        fill_color = color_red;
        break;
      case color_lightblue:
        fill_color = color_lightblue;
        break;
      case color_orange:
        fill_color = color_orange;
        break;
      default:
        break;
        }
      break;
      }
    }

  return fill_color;
  }

kotuku::gauge_style kotuku::gauge_window_t::style() const
  {
  return _style;
  }

const kotuku::point_t &kotuku::gauge_window_t::center() const
  {
  return _center;
  }

const font_t *kotuku::gauge_window_t::gauge_font() const
  {
  return _font;
  }

double kotuku::gauge_window_t::arc_begin() const
  {
  return _arc_begin;
  }

double kotuku::gauge_window_t::arc_range() const
  {
  return _arc_range;
  }

int kotuku::gauge_window_t::gauge_radii() const
  {
  return _gauge_radii;
  }

double kotuku::gauge_window_t::reset_value() const
  {
  return _reset_value;
  }

void kotuku::gauge_window_t::draw_bar(canvas_t &canvas, const rect_t &rect)
  {
  if(_steps.size() < 2)
    return;

  // calculate the height of the graph.
  long height = rect.height() - 12;

  long range = _steps.rbegin()->value - _steps.begin()->value;
  double pixels_per_unit = ((double) height) / ((double) range);

  // so we now have the increment, just start at the bottom
  rect_t drawing_rect(rect.left + 8, rect.top + 3, rect.right - 2,
      rect.bottom - 7);

  for(size_t i = 1; i < _steps.size(); i++)
    {
    double relative_value = _steps[i].value - _steps[i - 1].value;
    double pixels = relative_value * pixels_per_unit;
    drawing_rect.top = drawing_rect.bottom - (gdi_dim_t) (pixels);
    canvas.fill_rect(drawing_rect, _steps[i].pen.color);
    drawing_rect.bottom = drawing_rect.top;
    }
  }

void kotuku::gauge_window_t::draw_point(const pen_t *outline_pen, size_t index, gdi_dim_t offset)
  {
  // calculate the height of the graph.
  gdi_dim_t height = window_rect().height() - 12;

  gdi_dim_t range = _steps.rbegin()->value - _steps.begin()->value;
  double pixels_per_unit = ((double) height) / ((double) range);

  double min_range = _steps.begin()->value;

  double value = _values[index];
  value = max(value, min_range);
  value = min(value, min_range + double(range));

  double relative_value = double(value - min_range);
  gdi_dim_t position = window_rect().height()
      - (gdi_dim_t) (relative_value * pixels_per_unit) - 8;

  position = max(position, (gdi_dim_t)5);

  point_t pts[4] =
    {
    point_t(offset, position + 5),
    point_t(offset + 5, position),
    point_t(offset, position - 5),
    point_t(offset, position + 5)
    };

  pen(outline_pen);
  background_color(outline_pen->color);
  polygon(pts, 4, true);

  background_color (color_hollow);

  if(_style == bgs_pointer_minmax || _style == bgs_pointer_max)
    {
    relative_value = double(_max_values[index] - min_range);
    if(relative_value >= 0.0)
      {
      position = window_rect().height()
          - (gdi_dim_t) (relative_value * pixels_per_unit) - 8;
      position = max(position, (gdi_dim_t)5);

      pts[0] = point_t(offset, position + 5);
      pts[1] = point_t(offset + 5, position);
      pts[2] = point_t(offset, position - 5);
      pts[3] = point_t(offset, position + 5);

      polygon(pts, 4, true);
      }
    }

  if(_style == bgs_pointer_minmax || _style == bgs_pointer_min)
    {
    relative_value = double(_min_values[index] - min_range);
    if(relative_value >= 0.0)
      {
      position = window_rect().height()
          - (gdi_dim_t) (relative_value * pixels_per_unit) - 8;
      position = max(position, (gdi_dim_t)5);

      pts[0] = point_t(offset, position + 5);
      pts[1] = point_t(offset + 5, position);
      pts[2] = point_t(offset, position - 5);
      pts[3] = point_t(offset, position + 5);

      polygon(pts, 4, true);
      }
    }
  }
