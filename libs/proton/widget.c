#include "widget.h"
#include "../photon/photon_priv.h"
#include <stdio.h>

void on_paint_widget_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  widget_t* widget = (widget_t*)wnddata;

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  // fill without a border
  if (widget->is_alarm)
    {
    // handle the alarm of the widget by drawing a box
    rect_t alarm_rect =
      {
      .left = wnd_rect->left + (widget->style & BORDER_LEFT ? 1 : 0),
      .top = wnd_rect->top + (widget->style & BORDER_TOP ? 1 : 0),
      .right = wnd_rect->right - (widget->style & BORDER_RIGHT ? 2 : 1),
      .bottom = wnd_rect->bottom - (widget->style & BORDER_BOTTOM ? 2 : 1)
      };

    rectangle(canvas, wnd_rect, widget->alarm_color, color_hollow, &alarm_rect);
    }
  else if (widget->background_color != color_hollow && widget->style & DRAW_BACKGROUND)
    {
    // todo make the +/- 8 a setting
    rect_t rect;
    if (widget->background_gutter != 0)
      {
      // left gutter
      rectangle(canvas, wnd_rect, color_hollow, widget->gutter_color, rect_create_ex(0, 0, widget->background_gutter, ex.dy, &rect));
      // right gutter
      rectangle(canvas, wnd_rect, color_hollow, widget->gutter_color, rect_create_ex(ex.dx - widget->background_gutter, 0, widget->background_gutter, ex.dy, &rect));
      // top gutter
      rectangle(canvas, wnd_rect, color_hollow, widget->gutter_color, rect_create_ex(widget->background_gutter, 0, ex.dx - (widget->background_gutter << 1), widget->background_gutter, &rect));
      // bottom gutter
      rectangle(canvas, wnd_rect, color_hollow, widget->gutter_color, rect_create_ex(widget->background_gutter, ex.dy - widget->background_gutter, ex.dx - (widget->background_gutter << 1), widget->background_gutter, &rect));
      }

    rectangle(canvas, wnd_rect, 0, widget->background_color, rect_create(widget->background_gutter, widget->background_gutter, ex.dx - widget->background_gutter, ex.dy - widget->background_gutter, &rect));

    }

  point_t pts[2];

  if (widget->style & BORDER_BOTTOM)
    {
    pts[0].x = wnd_rect->left;
    pts[0].y = wnd_rect->bottom - 1;

    pts[1].x = wnd_rect->right + 1;
    pts[1].y = pts[0].y;

    polyline(canvas, wnd_rect, widget->border_color, 2, pts);
    }

  if (widget->style & BORDER_TOP)
    {
    pts[0].x = wnd_rect->left;
    pts[0].y = wnd_rect->top;

    pts[1].x = wnd_rect->right + 1;
    pts[1].y = pts[0].y;

    polyline(canvas, wnd_rect, widget->border_color, 2, pts);
    }

  if (widget->style & BORDER_LEFT)
    {
    pts[0].x = wnd_rect->left;
    pts[0].y = wnd_rect->top;

    pts[1].x = pts[0].x;
    pts[1].y = wnd_rect->bottom;

    polyline(canvas, wnd_rect, widget->border_color, 2, pts);
    }

  if (widget->style & BORDER_RIGHT)
    {
    pts[0].x = wnd_rect->right - 1;
    pts[0].y = wnd_rect->top;

    pts[1].x = pts[0].x;
    pts[1].y = wnd_rect->bottom;

    polyline(canvas, wnd_rect, widget->border_color, 2, pts);
    }
  }

result_t widget_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t id = get_can_id(msg);
  widget_t* widget = (widget_t*)wnddata;

  bool changed = false;
  bool flag;

  if (widget->sensor_id == id)
    {
    if (succeeded(get_param_bool(msg, &flag)))
      {
      changed = widget->sensor_failed = !flag;
      widget->sensor_failed = !flag;
      }

    // reset the status counter
    widget->sensor_timer = widget->status_timeout;
    }
  else if (id == id_timer && widget->sensor_id != 0)
    {
    // timer is a 1 second interval, so subtract 1000
    if (widget->sensor_timer > 0)
      {
      if (widget->sensor_timer >= 1000)
        widget->sensor_timer -= 1000;
      else
        widget->sensor_timer = 0;

      if (widget->sensor_timer == 0)
        {
        changed = true;
        widget->sensor_failed = true;
        }
      }
    }
  else if (widget->alarm_id == id)
    {
    if (succeeded(get_param_bool(msg, &flag)))
      {
      changed = widget->is_alarm != (flag != 0);
      widget->is_alarm = (flag != 0);
      }
    }
  else if (id == id_paint)
    on_paint_widget(hwnd, msg, wnddata);

  if (changed)
    invalidate(hwnd);

  return defwndproc(hwnd, msg, wnddata);
  }

result_t create_widget(handle_t parent, uint16_t id, wndproc_fn cb, widget_t* widget, handle_t* hwnd)
  {
  result_t result;

  if (failed(result = window_create(parent, &widget->rect, cb, widget, id, hwnd)))
    return result;

  invalidate(*hwnd);

  if (failed(result = set_z_order(*hwnd, widget->z_order)))
    return result;

  if (widget->on_create != 0)
    result = (*widget->on_create)(*hwnd, widget);
  else
    result = s_ok;

  return result;
  }

result_t on_paint_widget(handle_t hwnd, const canmsg_t* msg, widget_t* widget)
  {
  result_t result;
  handle_t canvas;

  if (failed(result = begin_paint(hwnd, &canvas)))
    return result;

  rect_t wnd_rect;
  window_rect(hwnd, &wnd_rect);

  if (widget->on_paint != nullptr)
    (widget->on_paint)(canvas, &wnd_rect, msg, widget);

  // handle the sensor failed case
  if (widget->sensor_failed)
    {
    point_t pts[2];

    rect_top_left(&wnd_rect, pts);
    rect_bottom_right(&wnd_rect, pts + 1);
    polyline(canvas, &wnd_rect, widget->alarm_color, 2, pts);

    rect_top_right(&wnd_rect, pts);
    rect_bottom_left(&wnd_rect, pts + 1);
    polyline(canvas, &wnd_rect, widget->alarm_color, 2, pts);
    }

  return end_paint(hwnd);
  }

result_t on_paint_roller_background(handle_t canvas,
  const rect_t* bounds,
  color_t bg_color,
  color_t border_color,
  bool left_ptr,
  rect_t* value_box)
  {
  int32_t median = rect_height(bounds) >> 1;
  median += bounds->top;

  int32_t bar_width = rect_width(bounds);

  if (left_ptr)
    {
    point_t roller[8] =
      {
          { bounds->left,  median },
          { bounds->left + 7,  median + 7 },
          { bounds->left + 7,  median + 20 },
          { bounds->left + bar_width,  median + 20 },
          { bounds->left + bar_width,  median - 20 },
          { bounds->left + 7,  median - 20 },
          { bounds->left + 7,  median - 7 },
          { bounds->left,  median }
      };

    polygon(canvas, bounds, border_color, bg_color, 8, roller);

    if (value_box != 0)
      {
      value_box->left = bounds->left + 21;
      value_box->right = bounds->left + bar_width - 1;
      value_box->top = median - 19;
      value_box->bottom = median + 19;
      }
    }
  else
    {
    point_t roller[8] =
      {
          { bounds->left + bar_width,  median },
          { bounds->left + bar_width - 7,  median + 7 },
          { bounds->left + bar_width - 7,  median + 20 },
          { bounds->left,  median + 20 },
          { bounds->left,  median - 20 },
          { bounds->left + bar_width - 7,  median - 20 },
          { bounds->left + bar_width - 7,  median - 7 },
          { bounds->left + bar_width,  median }
      };

    polygon(canvas, bounds, border_color, bg_color, 8, roller);

    if (value_box != 0)
      {
      value_box->left = bounds->left + 1;
      value_box->right = bounds->left + bar_width - 21;
      value_box->top = median - 19;
      value_box->bottom = median + 19;
      }
    }

  return s_ok;
  }

result_t on_display_roller(handle_t canvas,
  const rect_t* bounds,
  uint32_t value,
  int digits,
  color_t fg_color,
  const font_t* large_font,
  const font_t* small_font)
  {
  // we need to work out the size of the roller digits first
//  const handle_t  *old_font = cv.font(&arial_12_font);

  extent_t size_medium;
  text_extent(small_font, 2, "00", &size_medium);

  point_t pt = {
    bounds->right - (digits == 1 ? size_medium.dx >>= 1 : size_medium.dx),
    bounds->top
    };

  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= size_medium.dy >> 1;

  if (digits == 1)
    value *= 10;

  // calc the interval / pixel ratio
  pt.y += (int32_t)((value % 10) * (size_medium.dy / 10.0));
  int32_t minor = (value / 10) * 10;

  int32_t large_value = minor / 100;
  minor %= 100;

  while (pt.y > bounds->top)
    {
    pt.y -= size_medium.dy;
    minor += 10;
    }

  char str[64];

  while (pt.y <= bounds->bottom)
    {
    // draw the text + digits first
    minor %= 100;
    if (minor < 0)
      minor += 100;

    if (minor >= 0)
      {
      if (digits == 1)
        sprintf(str, "%d", (int)minor / 10);
      else
        sprintf(str, "%02.2d", (int)minor);

      draw_text(canvas, bounds, small_font, fg_color, color_hollow,
        2, str, &pt, bounds, eto_clipped, 0);
      }

    minor -= 10;
    pt.y += size_medium.dy;
    }

  // now the larger value

  sprintf(str, "%d", (int)large_value);
  uint16_t len = (uint16_t)strlen(str);

  // calc the size
  //cv.font(&arial_15_font);
  extent_t large_size;
  text_extent(large_font, len, str, &large_size);

  pt.x -= large_size.dx;
  pt.y = bounds->top;
  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= large_size.dy >> 1;

  draw_text(canvas, bounds, large_font, fg_color, color_hollow,
    len, str, &pt, bounds, eto_clipped, 0);

  return s_ok;
  }



result_t get_background_color(handle_t hwnd, color_t* color)
  {
  if (color == 0)
    return e_bad_pointer;

  widget_t* widget;
  result_t result;
  if (failed(result = get_wnddata(hwnd, (void**)&widget)))
    return result;

  *color = widget->background_color;

  return s_ok;
  }

result_t set_background_color(handle_t hwnd, color_t color)
  {
  result_t result;
  widget_t* widget;
  if (failed(result = get_wnddata(hwnd, (void**)&widget)))
    return result;

  widget->background_color = color;

  return s_ok;
  }