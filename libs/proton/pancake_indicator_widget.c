#include "pancake_indicator_widget.h"


static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  pancake_widget_t* wnd = (pancake_widget_t*)wnddata;
  extent_t ex;

  if (wnd->background_canvas == nullptr)
    {
    rect_extents(wnd_rect, &ex);

    canvas_create(&ex, &wnd->background_canvas);
    // fill the background with the color
    on_paint_widget_background(wnd->background_canvas, wnd_rect, msg, wnddata);

    // draw the name if enabled
    if ((wnd->base.style & DRAW_NAME) != 0)
      draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, wnd->base.name_color, wnd->base.background_color, 0,
        wnd->base.name, &wnd->base.name_pt, wnd_rect, 0, 0);

    }

  point_t pt = { 0, 0 };
  bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_canvas, wnd_rect, &pt, src_copy);

  // calculate what indicator cell to draw

  // calc the scale
  int range = wnd->max_value - wnd->min_value;
  int value = wnd->value - wnd->min_value;

  value *= 100;

  // adjust to pct (0-100)
  value /= range;

  int cell = value / 12;

  rect_extents(&wnd->indicator_rect, &ex);

  point_t draw_pt = {
    .x = wnd->indicator_rect.left + (ex.dx >> 1),
    .y = wnd->indicator_rect.top + (ex.dy >> 1)
    };

  extent_t delta;
  if (wnd->horizontal)
    {

    // adjust the x start point
    int32_t offset = wnd->bar_height >> 1;
    // subtract 4 bars to left
    offset += (wnd->bar_height + wnd->bar_gutter) << 2;

    draw_pt.x -= offset;
    draw_pt.y -= (wnd->bar_width >> 1);

    delta.dx = wnd->bar_height + wnd->bar_gutter;
    delta.dy = 0;
    ex.dx = wnd->bar_height;
    ex.dy = wnd->bar_width;
    }
  else
    {

    // adjust the x start point
    int32_t offset = wnd->bar_height >> 1;
    // subtract 4 bars to left
    offset += (wnd->bar_height + wnd->bar_gutter) << 2;

    draw_pt.y -= offset;
    draw_pt.x -= (wnd->bar_width >> 1);

    delta.dx = 0;
    delta.dy = wnd->bar_height + wnd->bar_gutter;
    ex.dx = wnd->bar_width;
    ex.dy = wnd->bar_height;
    }

  rect_t indicator_rect;
  // todo: most can be precalculated as a background

  for (int i = 0; i < 9; i++)
    {
    rectangle(canvas, wnd_rect, wnd->outline_color,
      (i == cell) ? wnd->indicator_color : wnd->base.background_color,
      rect_create_ex(draw_pt.x, draw_pt.y, ex.dx, ex.dy, &indicator_rect));

    draw_pt.x += delta.dx;
    draw_pt.y += delta.dy;
    }
  }

static result_t pancake_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  pancake_widget_t* wnd = (pancake_widget_t*)wnddata;

  uint16_t id = get_can_id(msg);

  if (wnd->can_id == id)
    {
    // if there is a converter use that
    int16_t value;

    // only process if it can be a int16
    if (failed(get_param_int16(msg, &value)))
      return s_ok;

    if (wnd->converter != 0)
      value = (*wnd->converter->convert_int16)(value);

    if (value > wnd->max_value)
      value = wnd->max_value;
    else if (value < wnd->min_value)
      value = wnd->min_value;

    if (wnd->value != value)
      {
      wnd->value = value;
      invalidate(hwnd);
      }
    }

  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_pancake_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, pancake_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, pancake_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }
