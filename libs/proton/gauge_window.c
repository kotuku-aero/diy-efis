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
#include "spatial.h"
#include "pens.h"

typedef enum 
  {
  gs_pointer,         // a simple line
  gs_sweep,           // a pie sweep
  gs_bar,             // line sweep
  gs_point,           // simple-pointer
  gs_pointer_minmax,  // a pointer with min-max markers
  gs_point_minmax,    // simple-pointer with min-max markers
  gs_hbar,            // small horizonatal bar
  // bar graph styles
  bgs_point, // simple-pointer
  bgs_pointer_minmax, // a pointer with min-max markers
  bgs_pointer_max, // simple-pointer with max markers
  bgs_pointer_min,
  bgs_small,            // vertical bars with tick marks
  } gauge_style;

typedef struct 
  {
  int16_t value;
  pen_t pen;
  color_t gauge_color;
  } step_t;

typedef struct 
  {
  int16_t value;
  char text[REG_STRING_MAX+1];
  } tick_mark_t;

typedef struct _gauge_window_t {
  uint16_t version;
  
  char name[REG_STRING_MAX +1];
  color_t name_color;
  handle_t  name_font;
  point_t name_pt;
  bool draw_name;
  
  color_t background_color;
  pen_t border_pen;
  bool draw_border;
  handle_t  font;

  
  uint16_t num_values;
  // this is updated on each window message
  float values[4];
  float min_values[4];
  float max_values[4];
  uint16_t labels[4];

  uint16_t reset_label;
  uint16_t label;

  point_t center;
  gdi_dim_t gauge_radii;

  uint16_t arc_begin;
  uint16_t arc_range;
  float reset_value;

  gauge_style style;
  uint16_t width;          // pointer or sweep width

  bool draw_value;         // draw the value
  handle_t value_font;       // what font to draw a value in
  rect_t value_rect;

  vector_p steps;
  vector_p ticks;

  float scale;
  float offset;

  } gauge_window_t;

static const char *gauge_style_values[] =
  {
      "gs_pointer",
      "gs_sweep",
      "gs_bar",
      "gs_point",
      "gs_pointer_minmax",
      "gs_point_minmax",
      "gs_hbar",
      "bgs_point",
      "bgs_pointer_minmax",
      "bgs_pointer_max",
      "bgs_pointer_min",
      "bgs_small"
  };

#define num_elements(p) (sizeof(p) / sizeof(p[0]))
static bool is_bar_style(gauge_window_t *wnd);

static int calculate_rotation(gauge_window_t *wnd, float value);
static const pen_t *calculate_pen(gauge_window_t *wnd, uint16_t width, float value, pen_t *pen);
static color_t calculate_color(gauge_window_t *wnd, float value);

static void update_dial_gauge(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect);
static void update_bar_gauge(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect);
static void draw_pointer(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect, const pen_t *pen, int rotation);
static void draw_sweep(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect, const pen_t *pen, color_t fill, int rotation);
static void draw_bar(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect, const pen_t *pen, int rotation);
static void draw_point(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect, const pen_t *pen, int rotation);
static void draw_graph_value(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect, const pen_t *pen, size_t index, gdi_dim_t offset);
static void draw_bar_graph(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect, const rect_t *rect);

static result_t on_paint(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  begin_paint(hwnd);

  gauge_window_t *wnd = (gauge_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);
  
  extent_t ex;
  rect_extents(&wnd_rect, &ex);
  
  // fill without a border
  rectangle(hwnd, &wnd_rect, 0, wnd->background_color, &wnd_rect);

  if (wnd->draw_border)
    round_rect(hwnd, &wnd_rect, &wnd->border_pen, color_hollow, &wnd_rect, 12);

  if(is_bar_style(wnd))
    update_bar_gauge(hwnd, wnd, &wnd_rect);
  else
    update_dial_gauge(hwnd, wnd, &wnd_rect);

  end_paint(hwnd);
  return s_ok;
  }

static result_t on_reset_label(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  gauge_window_t *wnd = (gauge_window_t *)proxy->parg;

  size_t i;
  for (i = 0; i < 4; i++)
    {
    wnd->min_values[i] = wnd->reset_value;
    wnd->max_values[i] = wnd->min_values[i];
    }
  changed = true;

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_value_label(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  bool changed = false;
  gauge_window_t *wnd = (gauge_window_t *)proxy->parg;

  size_t i;
  for (i = 0; i < wnd->num_values; i++)
    {
    if (msg->id == wnd->labels[i])
      {
      float float_value;
      int16_t short_value;

      switch (msg->canas.data_type)
        {
        case CANAS_DATATYPE_FLOAT :
          get_param_float(msg, &float_value);
          break;
        case CANAS_DATATYPE_SHORT :
          get_param_int16(msg, 0, &short_value);
          float_value = short_value;
          break;
        }
      float_value *= wnd->scale;
      float_value += wnd->offset;

      changed = wnd->values[i] != float_value;
      wnd->values[i] = float_value;

      wnd->min_values[i] = min(wnd->min_values[i], wnd->values[i]);
      wnd->max_values[i] = max(wnd->max_values[i], wnd->values[i]);

      break;
      }
    }


  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

void update_bar_gauge(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect)
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
  rect_t graph =
    { wnd_rect->left, wnd_rect->top, wnd_rect->left + 14, rect_height(wnd_rect) };

  size_t i;
  for(i = 0; i < wnd->num_values; i++)
    {
    draw_bar_graph(hwnd, wnd, wnd_rect, &graph);
    graph.left += 14;
    graph.right += 14;
    }

  if(wnd->draw_name)
    {
    extent_t sz;
    text_extent(hwnd, wnd->font, wnd->name, 0, &sz);
    point_t pt;
    bottom_right(wnd_rect, &pt);

    pt.x -= 4;
    pt.x -= sz.dx;
    pt.y -= sz.dy;

    draw_text(hwnd, wnd_rect, wnd->font,
              wnd->name_color, wnd->background_color,
              wnd->name, 0, &pt, 0, 0, 0);
    }

  gdi_dim_t height = rect_height(&graph) - 12;

  if(wnd->ticks != 0)
    {
    uint16_t num_ticks;

    vector_count(wnd->ticks, &num_ticks);

    tick_mark_t first_tick;
    tick_mark_t last_tick;

    vector_at(wnd->ticks, 0, &first_tick);
    vector_at(wnd->ticks, num_ticks-1, &last_tick);

    int16_t range = last_tick.value - first_tick.value;
    float pixels_per_unit = ((float) height) / ((float) range);
    size_t i;
    for (i = 0; i < num_ticks; i++)
      {
      vector_at(wnd->ticks, i, &last_tick);
      if(last_tick.text[0] == 0)
        continue;

      long value = last_tick.value;

      extent_t sz;
      text_extent(hwnd, wnd->font, last_tick.text, 0, &sz);

      float relative_value = value - first_tick.value;
      float pixels = relative_value * pixels_per_unit;
      point_t pt =
        { graph.left + 2,
          (graph.bottom - pixels) - 7 - (sz.dy >> 1)
        };

      draw_text(hwnd, wnd_rect, wnd->font,
                wnd->name_color, wnd->background_color,
                last_tick.text, 0, &pt, 0, 0, 0);
      }
    }
  
  gdi_dim_t offset = 1;

  for (i = 0; i < wnd->num_values; i++)
    {
    draw_graph_value(hwnd, wnd, wnd_rect, &lightblue_pen, i, offset);
    offset += 14;
    }
  }

void update_dial_gauge(handle_t hwnd, gauge_window_t *wnd, const rect_t *wnd_rect)
  {
  if (wnd->draw_name)
    {
//    font(name_font);
//    background_color(color_black);
//    text_color(color_white);
    point_t name_pt;
    copy_point(&wnd->name_pt, &name_pt);

    extent_t sz;
    text_extent(hwnd, wnd->name_font, wnd->name, 0, &sz);

    name_pt.x -= sz.dx >> 1;
    name_pt.y -= sz.dy >>1;

    draw_text(hwnd, wnd_rect, wnd->name_font,
              wnd->name_color, wnd->background_color,
              wnd->name, 0, &name_pt, 0, 0, 0);
    }

  int16_t min_range = 0;
  int16_t max_range = 0;
  float degrees_per_unit = 1.0;
  
    // we now draw an arc.  The range is the last value in the arc
    // minus the first value.  Note that the values can be -
    // the range of the gauge is gauge_y - 360 degrees
  if(wnd->steps != 0)
    {
    uint16_t num_steps;
    vector_count(wnd->steps, &num_steps);
    
    step_t first_step;
    step_t last_step;
    
    vector_at(wnd->steps, 0, &first_step);
    vector_at(wnd->steps, num_steps-1, &last_step);
    
    min_range = first_step.value;
    max_range = last_step.value;

    int16_t steps_range = max_range - min_range;

    degrees_per_unit = ((float) wnd->arc_range) / steps_range;

    float arc_start = wnd->arc_begin;
    uint16_t i;

//    font(_font);
//    background_color(color_black);
//    text_color(color_white);
//
    for (i = 1; i < num_steps; i++)
      {
      vector_at(wnd->steps, i, &last_step);
      // the arc angle is the step end
      float arc_end = (last_step.value - min_range) * degrees_per_unit;
      arc_end += wnd->arc_begin;

      // the arc starts a point(0, -1) which is 0 degrees
      // so ranges from 0..270 are 90->360 and needs to
      // be split to draw 270..360
      // draw arc
      int arc_angles[3] = { (int)arc_start+90, (int)arc_end+90, -1 };

      if(arc_angles[0] > 360 )
        {
        arc_angles[0] -= 360;
        arc_angles[1] -= 360;
        }
      else if(arc_angles[1] > 360)
        {
        arc_angles[2] = arc_angles[1] - 360;
        arc_angles[1] = 360;
        }

      arc(hwnd, wnd_rect, &last_step.pen, &wnd->center,
          wnd->gauge_radii, arc_angles[0], arc_angles[1]);

      if(arc_angles[2] >= 0)
        arc(hwnd, wnd_rect, &last_step.pen, &wnd->center,
            wnd->gauge_radii, 0, arc_angles[2]);

      arc_start = arc_end;
      }
    }

  if(wnd->ticks != 0)
    {
    uint16_t num_ticks;
    vector_count(wnd->ticks, &num_ticks);
    uint16_t i;
    // we now draw the tick marks.  They are a line from the point 5 pixels long.

    // we create a white pen for this
    //pen(&white_pen_2);

    gdi_dim_t line_start = wnd->gauge_radii;
    line_start += wnd->width >> 1;              // add the line half width

    gdi_dim_t line_end = line_start - wnd->width - 5;
    float arc_start;

    for (i = 0; i < num_ticks; i++)
      {
      tick_mark_t tick;
      vector_at(wnd->ticks, i, &tick);
      
      arc_start = (tick.value - min_range) * degrees_per_unit;
      arc_start += wnd->arc_begin;
      // make it point to correct spot....
      arc_start += 180;

      point_t pts[2];

      copy_point(&wnd->center, &pts[0]);
      pts[0].y -= line_start;

      rotate_point(&wnd->center, &pts[0], arc_start);

      copy_point(&wnd->center, &pts[1]);
      pts[1].y -= line_end;

      rotate_point(&wnd->center, &pts[1], arc_start);

      polyline(hwnd, wnd_rect, &white_pen, 2, pts);

      if (tick.text[0] != 0 && wnd->font != 0)
        {
        // write the text at the point
        extent_t size;
        text_extent(hwnd, wnd->font, tick.text, 0, &size);

        // the text is below the tick marks
        point_t top_left = {
          wnd->center.x,
          wnd->center.y - line_end + (size.dy >> 1)
          };
        
        rotate_point(&wnd->center, &top_left, arc_start);

        top_left.x -= (size.dx >> 1);
        top_left.y -= (size.dy >> 1);

        draw_text(hwnd, wnd_rect, wnd->font, color_white, wnd->background_color,
                  tick.text, 0, &top_left, 0, 0, 0);
        }
      }
    }

    // we now calculate the range of the segments.  There are 8 segments
    // around a 240 degree arc.  We draw the nearest based on the range
    // of the gauge.
    pen_t pointer_pen;

    switch (wnd->style)
      {
    case gs_pointer_minmax:
      draw_point(hwnd, wnd, wnd_rect,
                 calculate_pen(wnd, 1, wnd->min_values[0], &pointer_pen), 
                 calculate_rotation(wnd, wnd->min_values[0]));
      draw_point(hwnd, wnd, wnd_rect,
                 calculate_pen(wnd, 1, wnd->max_values[0], &pointer_pen),
                 calculate_rotation(wnd, wnd->max_values[0]));
      break;
    case gs_pointer:
      draw_pointer(hwnd, wnd, wnd_rect,
                   calculate_pen(wnd, wnd->width, wnd->values[0], &pointer_pen),
                   calculate_rotation(wnd, wnd->values[0]));
      break;
    case gs_sweep:
      draw_sweep(hwnd, wnd, wnd_rect,
                 calculate_pen(wnd, wnd->width, wnd->values[0], &pointer_pen),
                 calculate_color(wnd, wnd->values[0]), calculate_rotation(wnd, wnd->values[0]));
      break;
    case gs_bar:
      draw_bar(hwnd, wnd, wnd_rect,
               calculate_pen(wnd, wnd->width, wnd->values[0], &pointer_pen),
               calculate_rotation(wnd, wnd->values[0]));
      break;
    case gs_point_minmax:
      draw_point(hwnd, wnd, wnd_rect,
                 calculate_pen(wnd, 1, wnd->min_values[0], &pointer_pen),
                 calculate_rotation(wnd, wnd->min_values[0]));
      draw_point(hwnd, wnd, wnd_rect,
                 calculate_pen(wnd, 1, wnd->max_values[0], &pointer_pen),
                 calculate_rotation(wnd, wnd->max_values[0]));
    case gs_point:
      draw_point(hwnd, wnd, wnd_rect,
                 calculate_pen(wnd, 1, wnd->values[0], &pointer_pen),
                 calculate_rotation(wnd, wnd->values[0]));
      break;
      }
    // draw the value of the gauge as a text string
    if (wnd->draw_value && wnd->value_font != 0)
      {
//      font(_value_font);
//      text_color(color_lightblue);
//      background_color(color_black);

      char str[32];
      sprintf(str, "%d", (int) wnd->values[0]);

      size_t len = strlen(str);
      extent_t size;
      text_extent(hwnd, wnd->value_font, str, len, &size);

//      pen(&gray_pen);
//      background_color(color_black);

      // draw a rectangle around the text
      rectangle(hwnd, wnd_rect, &gray_pen, color_black, &wnd->value_rect);

      point_t pt = {
        wnd->value_rect.right - (size.dx + 2),
        wnd->value_rect.top + 2 };

      draw_text(hwnd, wnd_rect, wnd->value_font, color_lightblue, color_black,
                str, len, &pt, &wnd->value_rect, eto_clipped, 0);
      }
  }

bool is_bar_style(gauge_window_t *wnd)
  {
  return wnd->style >= bgs_point;
  }

static void draw_pointer(handle_t hwnd,
                         gauge_window_t *wnd,
                         const rect_t *wnd_rect, 
                         const pen_t *outline_pen,
                         int rotation)
  {
  // draw the marker line
  point_t pts[2] =
    {
    { wnd->center.x, wnd->center.y - 5 },
    { wnd->center.x, wnd->center.y - wnd->gauge_radii + 5}
    };

  rotate_point(&wnd->center, &pts[0], rotation+180);
  rotate_point(&wnd->center, &pts[1], rotation+180);
  
  polyline(hwnd, wnd_rect, outline_pen, 2, pts);
  }

static void draw_sweep(handle_t hwnd,
                       gauge_window_t *wnd,
                       const rect_t *wnd_rect,
                       const pen_t *outline_pen,
                       color_t fill_color,
                       int rotation)
  {
  pie(hwnd,
      wnd_rect,
      outline_pen,
      fill_color,
      &wnd->center,
      wnd->arc_begin+90,
      rotation+90,
      wnd->center.y - wnd->gauge_radii + 5,
      5);
  }

static void draw_bar(handle_t hwnd,
                     gauge_window_t *wnd,
                     const rect_t *wnd_rect,
                     const pen_t *outline_pen,
                     int rotation)
  {
  int arc_angles[3] = { wnd->arc_begin + 90, rotation + 90, -1 };

  if(arc_angles[0] > 360)
    {
    arc_angles[0] -= 360;
    arc_angles[1] -= 360;
    }
  else if(arc_angles[1] > 360)
    {
    arc_angles[2] = arc_angles[1] - 360;
    arc_angles[1] = 360;
    }

  gdi_dim_t radii = wnd->gauge_radii - (wnd->width >> 1);
  radii -= outline_pen->width >> 1;

  arc(hwnd, wnd_rect, outline_pen, &wnd->center, radii, arc_angles[0], arc_angles[1]);

  if(arc_angles[2] > 0)
    arc(hwnd, wnd_rect, outline_pen, &wnd->center, radii, 0, arc_angles[2]);
  }

static void draw_point(handle_t hwnd,
                       gauge_window_t *wnd,
                       const rect_t *wnd_rect,
                       const pen_t *outline_pen,
                       int rotation)
  {
  point_t pts[4] =
    {
    { wnd->center.x, wnd->center.y - wnd->gauge_radii + 5 },
    { wnd->center.x - 6, wnd->center.y - wnd->gauge_radii + 11 },
    { wnd->center.x + 6, wnd->center.y - wnd->gauge_radii + 11 },
    { wnd->center.x, wnd->center.y - wnd->gauge_radii + 5 }
    };

  float rotn = degrees_to_radians(rotation+90);
  rotate_point(&wnd->center, &pts[0], rotn);
  rotate_point(&wnd->center, &pts[1], rotn);
  rotate_point(&wnd->center, &pts[2], rotn);
  copy_point(&pts[0], &pts[3]);

  polygon(hwnd, wnd_rect, outline_pen, outline_pen->color, 4, pts);
  }

static int calculate_rotation(gauge_window_t *wnd, float value)
  {
  step_t first_step;
  step_t last_step;
  
  if(wnd->steps == 0)
    return 0;
  
  uint16_t count;
  vector_count(wnd->steps, &count);
  
  vector_at(wnd->steps, 0, &first_step);
  vector_at(wnd->steps, count-1, &last_step);
  
  float min_range = first_step.value;
  float max_range = last_step.value;

  // get the percent that the gauge is displaying
  float percent = (max(min(value, max_range), min_range) - min_range) / (max_range - min_range);

  float rotation;
  rotation = wnd->arc_begin + (wnd->arc_range * percent);

  return (int)rotation;
  }

static const pen_t *calculate_pen(gauge_window_t *wnd,
                                    uint16_t width,
                                    float value,
                                    pen_t *pen)
  {
  pen->style = ps_solid;
  pen->color = color_hollow;
  pen->width = width;
  
  step_t step;
  uint16_t count;
  
  if(wnd->steps == 0)
    return pen;
  
  vector_count(wnd->steps, &count);
  if(count == 0)
    return pen;
 
  size_t i;
  // step 0 is only used to set the minimum value for the gauge
  // so it is ignored
  for (i = 1; i < count; i++)
    {
    // get the next one, and see if we are done.
    vector_at(wnd->steps, i, &step);
    // assume this is our color.
    pen->color = step.gauge_color;
    if(value <= step.value)
      break;    
    }

  return pen;
  }

static color_t calculate_color(gauge_window_t *wnd, float value)
  {
  color_t fill_color = color_lightblue;
  step_t step;
  uint16_t count;
  
  if(wnd->steps == 0)
    return fill_color;
  
  vector_count(wnd->steps, &count);
  if(count == 0)
    return fill_color;
  
  vector_at(wnd->steps, 0, &step);

  size_t i;
  for (i = 0; i < count; i++)
    {
    fill_color = step.gauge_color;
    
    vector_at(wnd->steps, i, &step);
    if(value >= step.value)
      break;    
    }

  return fill_color;
  }

static void draw_bar_graph(handle_t hwnd,
                     gauge_window_t *wnd, 
                     const rect_t *wnd_rect,
                     const rect_t *rect)
  {
  uint16_t count;
   
  vector_count(wnd->steps, &count);
  
  if (count < 2)
    return;

  // calculate the height of the graph.
  long height = rect_height(rect) - 12;
  
  step_t first_step;
  step_t last_step;
  
  vector_at(wnd->steps, 0, &first_step);
  vector_at(wnd->steps, count-1, &last_step);

  long range = last_step.value - first_step.value;
  
  float pixels_per_unit = ((float) height) / ((float) range);

  // so we now have the increment, just start at the bottom
  rect_t drawing_rect = {
    rect->left + 8,
    rect->top + 3,
    rect->right - 2,
    rect->bottom - 7
    };

  size_t i;
  for (i = 1; i < count; i++)
    {
    vector_at(wnd->steps, i, &last_step);
    
    float relative_value = last_step.value - first_step.value;
    memcpy(&first_step, &last_step, sizeof(step_t));
    
    float pixels = relative_value * pixels_per_unit;
    drawing_rect.top = drawing_rect.bottom - (gdi_dim_t) (pixels);
    
    rectangle(hwnd, wnd_rect, 0, last_step.pen.color, &drawing_rect);
    drawing_rect.bottom = drawing_rect.top;
    }
  }

static void draw_graph_value(handle_t hwnd,
                       gauge_window_t *wnd,
                       const rect_t *wnd_rect,
                       const pen_t *outline_pen,
                       size_t index, 
                       gdi_dim_t offset)
  {
  // calculate the height of the graph.
  gdi_dim_t height = rect_height(wnd_rect) - 12;
  uint16_t count;
   
  vector_count(wnd->steps, &count);
  
  if (count < 2)
    return;
  
  step_t first_step;
  step_t last_step;
  
  vector_at(wnd->steps, 0, &first_step);
  vector_at(wnd->steps, count-1, &last_step);

  gdi_dim_t range = last_step.value - first_step.value;
  float pixels_per_unit = ((float) height) / ((float) range);

  float min_range = first_step.value;

  float value = wnd->values[index];
  value = max(value, min_range);
  value = min(value, min_range + range);

  float relative_value = value - min_range;
  gdi_dim_t position = height - (relative_value * pixels_per_unit) - 8;

  position = max(position, 5);

  point_t pts[4] =
    {
      { offset, position + 5 },
      { offset + 5, position },
      { offset, position - 5 },
      { offset, position + 5 }
    };

  polygon(hwnd, wnd_rect, outline_pen, outline_pen->color, 4, pts);

  if (wnd->style == bgs_pointer_minmax || wnd->style == bgs_pointer_max)
    {
    relative_value = wnd->max_values[index] - min_range;
    if (relative_value >= 0.0)
      {
      position = height - (relative_value * pixels_per_unit) - 8;
      position = max(position, 5);

      pts[0].x = offset; pts[0].y = position + 5;
      pts[1].x = offset + 5; pts[1].y = position;
      pts[2].x = offset; pts[2].y = position - 5;
      pts[3].x = offset; pts[3].y = position + 5;

      polygon(hwnd, wnd_rect, outline_pen, color_hollow, 4, pts);
      }
    }

  if (wnd->style == bgs_pointer_minmax || wnd->style == bgs_pointer_min)
    {
    relative_value = wnd->min_values[index] - min_range;
    if (relative_value >= 0.0)
      {
      position = height - (relative_value * pixels_per_unit) - 8;
      position = max(position, 5);

      pts[0].x = offset; pts[0].y = position + 5;
      pts[1].x = offset + 5; pts[1].y = position;
      pts[2].x = offset; pts[2].y = position - 5;
      pts[3].x = offset; pts[3].y =  position + 5;

      polygon(hwnd, wnd_rect, outline_pen, color_hollow, 4, pts);
      }
    }
  }

result_t create_gauge_window(handle_t parent, memid_t key, handle_t *hwnd)
  {
  result_t result;
  int16_t int_value;
  uint16_t uint_value;

  // create our window
  if (failed(result = create_child_widget(parent, key, defwndproc, hwnd)))
    return result;

  // create the window data.
  gauge_window_t *wnd = (gauge_window_t *)neutron_malloc(sizeof(gauge_window_t));
  memset(wnd, 0, sizeof(gauge_window_t));

  wnd->version = sizeof(gauge_window_t);

  rect_t rect_wnd;

  get_window_rect(*hwnd, &rect_wnd);

  uint16_t len = REG_STRING_MAX + 1;
  if (failed(reg_get_float(key, "scale", &wnd->scale)))
    wnd->scale = 1.0;

  if (failed(reg_get_float(key, "offset", &wnd->offset)))
    wnd->offset = 0.0;

  if (failed(lookup_enum(key, "style", gauge_style_values, num_elements(gauge_style_values), (int *)&wnd->style)))
    wnd->style = gs_pointer;

  reg_get_uint16(key, "reset-id", &wnd->reset_label);
  reg_get_float(key, "reset-value", &wnd->reset_value);

  if (failed(lookup_font(key, "font", &wnd->font)))
    {
    // we always have the neo font.
    if (failed(result = open_font("neo", 9, &wnd->font)))
      return result;
    }

  if (failed(reg_get_int16(key, "center-x", &int_value)))
    wnd->center.x = rect_width(&rect_wnd) >> 1;
  else
    wnd->center.x = (gdi_dim_t)int_value;

  if (failed(reg_get_int16(key, "center-y", &int_value)))
    wnd->center.y = rect_height(&rect_wnd) >> 1;
  else
    wnd->center.y = (gdi_dim_t)int_value;

  // get the details for the name
  if (failed(reg_get_bool(key, "draw-name", &wnd->draw_name)))
    wnd->draw_name = true;

  if (wnd->draw_name)
    {
    reg_get_string(key, "name", wnd->name, &len);

    if (failed(lookup_color(key, "name-color", &wnd->name_color)))
      wnd->name_color = color_white;

    if (failed(lookup_font(key, "name-font", &wnd->name_font)))
      {
      // we always have the neo font.
      if (failed(result = open_font("neo", 9, &wnd->name_font)))
        return result;
      }

    if (failed(reg_get_int16(key, "name-x", &int_value)))
      wnd->name_pt.x = wnd->center.x;
    else
      wnd->name_pt.x = (gdi_dim_t)int_value;

    if (failed(reg_get_int16(key, "name-y", &int_value)))
      wnd->name_pt.y = wnd->center.y;
    else
      wnd->name_pt.y = (gdi_dim_t)int_value;
    }

  if (failed(reg_get_uint16(key, "arc-begin", &wnd->arc_begin)))
    wnd->arc_begin = 120;

  if (failed(reg_get_uint16(key, "arc-range", &wnd->arc_range)))
    wnd->arc_range = 270;

  if (failed(reg_get_uint16(key, "width", &wnd->width)))
    wnd->width = 7;

  if (failed(reg_get_bool(key, "draw-value", &wnd->draw_value)))
    wnd->draw_value = true;

  if (wnd->draw_value)
    {
    if (failed(lookup_font(key, "value-font", &wnd->value_font)))
      {
      // we always have the neo font.
      if (failed(result = open_font("neo", 9, &wnd->value_font)))
        return result;
      }

    // default values
    wnd->value_rect.bottom = rect_wnd.bottom;
    wnd->value_rect.left = wnd->center.x + 2;
    wnd->value_rect.top = wnd->value_rect.bottom - 25;
    wnd->value_rect.right = rect_wnd.right;

    if (succeeded(reg_get_int16(key, "value-x", &int_value)))
      wnd->value_rect.left = (gdi_dim_t)int_value;

    if (succeeded(reg_get_uint16(key, "value-y", &uint_value)))
      wnd->value_rect.top = (gdi_dim_t)uint_value;

    if (succeeded(reg_get_uint16(key, "value-w", &uint_value)))
      wnd->value_rect.right = (gdi_dim_t)uint_value + wnd->value_rect.left;

    if (succeeded(reg_get_uint16(key, "value-h", &uint_value)))
      wnd->value_rect.bottom = (gdi_dim_t)uint_value + wnd->value_rect.top;
    }

  char temp_name[REG_NAME_MAX + 1];
  if (failed(reg_get_uint16(key, "can-id", &uint_value)))
    {
    // could be can value 0..3
    for (wnd->num_values = 0; wnd->num_values < 4; wnd->num_values++)
      {
      snprintf(temp_name, sizeof(temp_name), "can-id-%d", wnd->num_values);

      if (failed(reg_get_uint16(key, temp_name, &uint_value)))
        break;

      wnd->labels[wnd->num_values] = uint_value;
      wnd->values[wnd->num_values] = wnd->reset_value;
      wnd->min_values[wnd->num_values] = wnd->reset_value;
      wnd->max_values[wnd->num_values] = wnd->reset_value;
      }
    }
  else
    {
    wnd->labels[0] = uint_value;
    wnd->values[0] = wnd->reset_value;
    wnd->min_values[0] = wnd->reset_value;
    wnd->max_values[0] = wnd->reset_value;
    wnd->num_values = 1;
    }

  if (failed(reg_get_uint16(key, "radii", &uint_value)))
    uint_value = (rect_width(&rect_wnd) >> 1) - 5;

  wnd->gauge_radii = (gdi_dim_t)uint_value;

  // open a step key
  memid_t step_key;

  if (succeeded(reg_open_key(key, "step", &step_key)))
    {
    vector_create(sizeof(step_t), &wnd->steps);

    int i;
    // only support 99 ticks/steps
    for (i = 0; i < 99; i++)
      {
      snprintf(temp_name, 32, "%d", i);

      memid_t child_key;

      if (succeeded(reg_open_key(step_key, temp_name, &child_key)))
        {
        step_t new_step;
        memset(&new_step, 0, sizeof(new_step));

        reg_get_int16(child_key, "value", &new_step.value);
        lookup_color(child_key, "color", &new_step.gauge_color);

        memid_t pen_key;
        if (failed(reg_open_key(child_key, "pen", &pen_key)) ||
          failed(lookup_pen(pen_key, &new_step.pen)))
          {
          new_step.pen.color = color_lightblue;
          new_step.pen.width = 5;
          new_step.pen.style = ps_solid;
          }

        vector_push_back(wnd->steps, &new_step);
        }
      }
    }

  if (succeeded(reg_open_key(key, "tick", &step_key)))
    {
    vector_create(sizeof(tick_mark_t), &wnd->ticks);

    int i;
    for (i = 0; i < 99; i++)
      {
      snprintf(temp_name, 32, "%d", i);

      memid_t child_key;

      if (succeeded(reg_open_key(step_key, temp_name, &child_key)))
        {

        // the step is a series of settings in the form:
        // tick-0=650, 650
        // param1 -> tick point
        // param2 -> tick label
        tick_mark_t new_tick;
        memset(&new_tick, 0, sizeof(tick_mark_t));

        if (failed(reg_get_int16(child_key, "value", &new_tick.value)))
          new_tick.value = 0;

        reg_get_string(child_key, "text", new_tick.text, 0);

        vector_push_back(wnd->ticks, &new_tick);
        }
      }
    }

  if (failed(lookup_color(key, "background-color", &wnd->background_color)))
    wnd->background_color = color_black;

  reg_get_bool(key, "draw-border", &wnd->draw_border);

  memid_t pen_key;
  if(failed(reg_open_key(key, "border-pen", &pen_key)) ||
     failed(lookup_pen(pen_key, &wnd->border_pen)))
    memcpy(&wnd->border_pen, &gray_pen, sizeof(pen_t));

  // store the parameters for the window
  set_wnddata(*hwnd, wnd);

  // add all of the properties now
  add_event(*hwnd, id_paint, wnd, 0, on_paint);

  if(wnd->reset_label != 0)
    add_event(*hwnd, wnd->reset_label, wnd, 0, on_reset_label);

  uint16_t i;
  for (i = 0; i < wnd->num_values; i++)
    {
    add_event(*hwnd, wnd->labels[i], wnd, 0, on_value_label);
    }      

  invalidate_rect(*hwnd, &rect_wnd);

  return s_ok;
  }
