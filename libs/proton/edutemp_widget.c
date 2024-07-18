#include "edutemp_widget.h"
#include "converters.h"

/**
 * @brief Calculate the height of a display bar
 * @param value value to scale
 * @param calc_top calulated top of bar
 * @param bar_top maximum bar line
 * @param bar_bottom minimum bar line
 * @param max_value maximum value to display
 * @param min_value minimum value to display
 * @return line that is the height of the graph
*/
static int32_t calculate_height(uint16_t value, int32_t calc_top, int32_t bar_top,
  int32_t bar_bottom, uint16_t max_value, uint16_t min_value)
  {
  if (value <= min_value)
    return bar_bottom - 1;

  // calculate the distance
  uint16_t dist = value - min_value;
  uint16_t bar_height = bar_bottom - calc_top;
  float pct = ((float)dist) / ((float)(max_value - min_value));
  pct *= bar_height;

  dist = (uint16_t)pct;
  dist = bar_bottom - dist;

  if (dist < bar_top)
    dist = bar_top;

  return dist;
  }

static void draw_bar(handle_t canvas, const rect_t* wnd_rect, color_t outline, color_t fill, const rect_t* rect)
  {
  point_t pts[2];
  pts[0].x = rect->left;
  pts[1].x = rect->right;

  for (int32_t row = rect->bottom; row >= rect->top; row -= 2)
    {
    pts[0].y = row;
    pts[1].y = row;

    polyline(canvas, wnd_rect, fill, 2, pts);
    }
  }
/**
 * @brief draw a sensor as failed.  Draws a red cross
 * @param canvas window
 * @param clip_rect draw clipping
 * @param line_color color to draw
 * @param rect rect that bounds the fail
*/
static void draw_failed_sensor(handle_t canvas, const rect_t* clip_rect, color_t line_color, const rect_t* rect)
  {
  point_t pts[2];
  pts[0].x = rect->left; pts[0].y = rect->top;
  pts[1].x = rect->right; pts[1].y = rect->bottom;
  polyline(canvas, clip_rect, line_color, 2, pts);

  pts[0].x = rect->right;
  pts[1].x = rect->left;
  polyline(canvas, clip_rect, line_color, 2, pts);
  }

static const char* cyl_numbers[] = { "1", "2", "3", "4", "5", "6" };

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  edu_temp_widget_t* wnd = (edu_temp_widget_t*)wnddata;
  extent_t ex;
  rect_extents(wnd_rect, &ex);

  if (wnd->background_canvas == nullptr)
    {
    canvas_create(&ex, &wnd->background_canvas);
    on_paint_widget_background(wnd->background_canvas, wnd_rect, msg, wnddata);

    edu_temp_widget_t* wnd = (edu_temp_widget_t*)wnddata;
    extent_t ex;
    rect_extents(wnd_rect, &ex);

    point_t pt_text = { 2, wnd->cht_line };
    draw_text(wnd->background_canvas, wnd_rect, wnd->font, wnd->cht_color, color_hollow, 3, "CHT",
      &pt_text, 0, eto_none, 0);

    pt_text.x = wnd->right_gutter;
    pt_text.y = wnd->egt_line;

    draw_text(wnd->background_canvas, wnd_rect, wnd->font, wnd->egt_color, color_hollow, 3, "EGT",
      &pt_text, 0, eto_none, 0);

    extent_t text_ex;
    char val[16];
    point_t text_pt;

    // draw the CHT redline
    // TODO: this should be in a static background that is only updated if
    // settings change....

    sprintf(val, "%d", to_display_temperature->convert_uint16(wnd->cht_red_line));

    uint16_t len = (uint16_t)strlen(val);

    text_extent(wnd->font, len, val, &text_ex);

    text_pt.x = wnd->left_gutter - text_ex.dx;
    text_pt.y = wnd->cht_red_line_pos - (text_ex.dy >> 1);

    draw_text(wnd->background_canvas, wnd_rect, wnd->font, color_red, color_hollow, len, val, &text_pt, wnd_rect, eto_none, 0);

    // draw the EGT redline
    sprintf(val, "%d", to_display_temperature->convert_uint16(wnd->egt_red_line));

    len = (uint16_t)strlen(val);

    text_extent(wnd->font, len, val, &text_ex);

    text_pt.x = wnd_rect->right - (text_ex.dx + 1);
    text_pt.y = wnd->bar_top - (text_ex.dy >> 1);

    draw_text(wnd->background_canvas, wnd_rect, wnd->font, color_red, color_hollow, len, val, &text_pt, wnd_rect, eto_none, 0);

    // draw the CHT min
    sprintf(val, "%d", to_display_temperature->convert_uint16(wnd->cht_min));

    len = (uint16_t)strlen(val);

    text_extent(wnd->font, len, val, &text_ex);

    text_pt.x = wnd->left_gutter - text_ex.dx;
    text_pt.y = wnd->bar_bottom - (text_ex.dy >> 1);

    draw_text(wnd->background_canvas, wnd_rect, wnd->font, color_white, color_hollow, len, val, &text_pt, wnd_rect, eto_none, 0);

    // draw the egt_min
    sprintf(val, "%d", to_display_temperature->convert_uint16(wnd->egt_min));

    len = (uint16_t)strlen(val);

    text_extent(wnd->font, len, val, &text_ex);

    text_pt.x = wnd->right_gutter;
    text_pt.y = wnd->bar_bottom - (text_ex.dy >> 1);

    draw_text(wnd->background_canvas, wnd_rect, wnd->font, color_white, color_hollow, len, val, &text_pt, wnd_rect, eto_none, 0);

    int32_t bar_center = wnd->cylinder_draw_width >> 1;
    int32_t pos = wnd->left_gutter + bar_center;
    point_t pts[2];

    // draw the vertical separators between the cylinders
    for (uint16_t i = 0; i < wnd->num_cylinders; i++)
      {
      pts[0].x = pos; pts[0].y = wnd->bar_top;
      pts[1].x = pos; pts[1].y = wnd->bar_bottom + 1;

      polyline(wnd->background_canvas, wnd_rect, color_white, 2, pts);

      pts[0].x = pos - bar_center + 1; pts[0].y = wnd->cht_red_line_pos;
      pts[1].x = pos - 1; pts[1].y = wnd->cht_red_line_pos;
      polyline(wnd->background_canvas, wnd_rect, color_red, 2, pts);

      // draw the cylinder number
      text_extent(wnd->font, 1, cyl_numbers[i], &text_ex);

      text_pt.x = pos - (text_ex.dx >> 1); text_pt.y = wnd->bar_bottom + 1;
      draw_text(wnd->background_canvas, wnd_rect, wnd->font, color_white, color_hollow, 1, cyl_numbers[i], &text_pt, wnd_rect, eto_none, 0);


      pos += wnd->cylinder_draw_width;
      }
    }

  point_t pt = { 0, 0 };
  bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_canvas, wnd_rect, &pt, src_copy);

  int32_t bar_center = wnd->cylinder_draw_width >> 1;
  int32_t pos = wnd->left_gutter + bar_center;

  uint16_t egt_max = wnd->egt_min;
  uint16_t max_cyl = 7;
  for (uint16_t i = 0; i < wnd->num_cylinders; i++)
    {
    if (wnd->egt[i].value > egt_max)
      {
      max_cyl = i;
      egt_max = wnd->egt[i].value;
      }
    }

  extent_t text_ex;
  char val[16];
  point_t text_pt;
  uint16_t len;

  for (uint16_t i = 0; i < wnd->num_cylinders; i++)
    {
    if (i == max_cyl)
      {
      text_extent(wnd->font, 1, cyl_numbers[i], &text_ex);

      text_pt.x = pos - (text_ex.dx >> 1); text_pt.y = wnd->bar_bottom + 1;

      rect_t egt_max_rect;
      rect_create(text_pt.x - 2, text_pt.y, text_pt.x + text_ex.dx, wnd_rect->bottom - 1, &egt_max_rect);
      rectangle(canvas, wnd_rect, color_white, color_hollow, &egt_max_rect);
      draw_text(canvas, wnd_rect, wnd->font, color_white, color_hollow, 1, cyl_numbers[i], &text_pt, wnd_rect, eto_none, 0);

      break;
      }

    pos += wnd->cylinder_draw_width;
    }

  // Draw the thermocouple values

  // EGT values
  pos = wnd->left_gutter + 2;
  int32_t graph_width = bar_center - 4;
  int32_t center_offset = graph_width >> 1;

  rect_t graph_rect;
  // draw the CHT
  for (uint16_t i = 0; i < wnd->num_cylinders; i++)
    {
    const edu_bar_t* bar = wnd->cht + i;
    if (bar->sensor_fail)
      {
      rect_create(pos + 3, wnd->bar_top, pos + graph_width, wnd->bar_bottom, &graph_rect);
      draw_failed_sensor(canvas, wnd_rect, color_red, &graph_rect);
      }
    else
      {
      uint16_t cht = to_display_temperature->convert_uint16(bar->value);
      sprintf(val, "%d", cht);
      len = (uint16_t)strlen(val);

      text_extent(wnd->font, len, val, &text_ex);

      text_pt.x = pos + bar_center - 2 - (text_ex.dx >> 1);
      text_pt.y = wnd->cht_line;

      draw_text(canvas, wnd_rect, wnd->font,
        (bar->is_alarm) ? bar->alarm_color : bar->default_color,
        color_hollow, len, val, &text_pt, wnd_rect, eto_none, 0);

      int32_t top_pix = calculate_height(bar->value, wnd->cht_red_line_pos, wnd->bar_top, wnd->bar_bottom,
        wnd->cht_red_line, wnd->cht_min);

      rect_create(pos + 3, top_pix, pos + graph_width, wnd->bar_bottom, &graph_rect);

      draw_bar(canvas, wnd_rect, color_hollow, (bar->value >= wnd->cht_red_line) ? color_red : bar->default_color, &graph_rect);
      }

    pos += wnd->cylinder_draw_width;
    }

  pos = wnd->left_gutter + bar_center + 4;

  for (uint16_t i = 0; i < wnd->num_cylinders; i++)
    {
    const edu_bar_t* bar = wnd->egt + i;
    if (bar->sensor_fail)
      {
      rect_create(pos + graph_width, wnd->bar_top, pos - 2, wnd->bar_bottom, &graph_rect);
      draw_failed_sensor(canvas, wnd_rect, color_red, &graph_rect);
      }
    else
      {
      uint16_t egt = to_display_temperature->convert_uint16(bar->value);
      sprintf(val, "%d", egt);
      len = (uint16_t)strlen(val);

      text_extent(wnd->font, len, val, &text_ex);

      text_pt.x = pos - 4 - (text_ex.dx >> 1);
      text_pt.y = wnd->egt_line;
      draw_text(canvas, wnd_rect, wnd->font, bar->default_color, color_hollow, len, val, &text_pt, wnd_rect, eto_none, 0);

      int32_t top_pix = calculate_height(bar->value, wnd->bar_top, wnd->bar_top, wnd->bar_bottom,
        wnd->egt_red_line, wnd->egt_min);

      rect_create(pos + graph_width, top_pix, pos - 2, wnd->bar_bottom, &graph_rect);

      draw_bar(canvas, wnd_rect, color_hollow, (bar->value >= wnd->egt_red_line) ? color_red : bar->default_color, &graph_rect);
      }

    pos += wnd->cylinder_draw_width;
    }
  }

result_t edutemp_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  edu_temp_widget_t* wnd = (edu_temp_widget_t*)wnddata;

  uint16_t can_id = get_can_id(msg);
  bool changed = false;
  uint16_t value;
  // check all of the values
  for (uint16_t i = 0; i < wnd->num_cylinders; i++)
    {
    if (can_id == wnd->cht[i].can_id)
      {
      if (succeeded(get_param_int16(msg, (int16_t*)&value)))
        {
        changed = wnd->cht[i].value != value;
        if (changed)
          {
          wnd->cht[i].value = value;
          if (value > wnd->cht[i].max_value)
            wnd->cht[i].max_value = value;
          }
        }
      break;
      }
    else if (can_id == wnd->egt[i].can_id)
      {
      if (succeeded(get_param_int16(msg, (int16_t*)&value)))
        {
        changed = wnd->egt[i].value != value;
        if (changed)
          {
          wnd->egt[i].value = value;
          if (value > wnd->egt[i].max_value)
            wnd->egt[i].max_value = value;
          }
        }
      break;
      }
    else if (can_id == wnd->cht[i].alarm_id)
      {
      if (succeeded(get_param_int16(msg, (int16_t*)&value)))
        {
        bool is_alarm = value != 0;
        changed = wnd->cht[i].is_alarm != is_alarm;
        if (changed)
          wnd->cht[i].is_alarm = is_alarm;
        }
      break;
      }
    else if (can_id == wnd->egt[i].alarm_id)
      {
      if (succeeded(get_param_int16(msg, (int16_t*)&value)))
        {
        bool is_alarm = value != 0;
        changed = wnd->egt[i].is_alarm != is_alarm;
        if (changed)
          wnd->egt[i].is_alarm = is_alarm;
        }
      break;
      }
    else if (can_id == wnd->cht[i].sensor_id)
      {
      if (succeeded(get_param_int16(msg, (int16_t*)&value)))
        {
        bool is_failed = value == 0;
        changed = wnd->cht[i].sensor_fail != is_failed;
        if (changed)
          wnd->cht[i].sensor_fail = is_failed;
        }
      break;
      }
    else if (can_id == wnd->egt[i].sensor_id)
      {
      if (succeeded(get_param_int16(msg, (int16_t*)&value)))
        {
        bool is_failed = value == 0;
        changed = wnd->egt[i].sensor_fail != is_failed;
        if (changed)
          wnd->egt[i].sensor_fail = is_failed;
        }
      break;
      }
    }

  if (changed)
    invalidate(hwnd);

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_edutemps_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, edu_temp_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, edutemp_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }