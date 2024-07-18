#include "ap_widget.h"

static void draw_detail(handle_t canvas, ap_widget_t* wnd,
  const rect_t* clip_area, const rect_t* text_area,
  const char* value, color_t fg);

static const char *wpt_label = "WPT";
static const char *brg_label = "BRG";
static const char *dst_label = "DST";
static const char *ete_label = "ETE";
static const char *alt_label = "ALT";
static const char *vs_label = "VS";

static const char *ap_modes[] = 
  {
  "OFF",
  "HDG+ALT",
  "CRS+ALT",
  "HDG",
  "CRS",
  "ALT",
  "180L",
  "180R"
  };

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  ap_widget_t *wnd = (ap_widget_t *)wnddata;
  extent_t ex;

  if (wnd->background_canvas == nullptr)
    {
    rect_extents(wnd_rect, &ex);
    canvas_create(&ex, &wnd->background_canvas);

    on_paint_widget_background(wnd->background_canvas, wnd_rect, msg, wnddata);

    rect_t button_rect;
    rect_create_ex(wnd->gutter, wnd->gutter, rect_width(wnd_rect) - (wnd->gutter << 1), wnd->button_height, &button_rect);
    // mode button
    rectangle(wnd->background_canvas, wnd_rect, color_blue, color_lightblue, &button_rect);

    point_t pt;
    rect_top_left(&wnd->wpt_label, &pt);
    draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, 0, wpt_label, &pt, &wnd->wpt_label, 0, 0);

    rect_top_left(&wnd->brg_label, &pt);
    draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, 0, brg_label, &pt, &wnd->dst_label, 0, 0);

    rect_top_left(&wnd->dst_label, &pt);
    draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, 0, dst_label, &pt, &wnd->dst_label, 0, 0);

    rect_top_left(&wnd->ete_label, &pt);
    draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, 0, ete_label, &pt, &wnd->ete_label, 0, 0);

    rect_top_left(&wnd->alt_label, &pt);
    draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, 0, alt_label, &pt, &wnd->alt_label, 0, 0);

    rect_top_left(&wnd->vs_label, &pt);
    draw_text(wnd->background_canvas, wnd_rect, wnd->base.name_font, color_white, color_hollow, 0, vs_label, &pt, &wnd->vs_label, 0, 0);

    }

  point_t pt = { 0, 0 };
  bit_blt(canvas, wnd_rect, wnd_rect, wnd->background_canvas, wnd_rect, &pt, src_copy);

  // draw the mode text
  const char *ap_mode = ap_modes[wnd->mode];

  text_extent(wnd->base.name_font, 0, ap_mode, &ex);

  rect_top_left(&wnd->mode_button, &pt);

  int32_t txt_top = rect_height(&wnd->mode_button);

  txt_top >>= 1;      // center pt
  txt_top -= ex.dy >> 1;

  pt.y += txt_top;

  int32_t txt_right = rect_width(&wnd->mode_button);

  txt_right >>= 1;    // center
  txt_right -= ex.dx >> 1;

  pt.x += txt_right;

  draw_text(canvas, wnd_rect, wnd->base.name_font, color_black, color_purple, 0, ap_mode, &pt, &wnd->mode_button, 0, 0);

  // display the waypoint
  draw_detail(canvas, wnd, wnd_rect, &wnd->wpt_value, wnd->waypoint, wnd->active_color);

  // display the desired track angle
  char buf[64];
  sprintf(buf, "%d", (int)wnd->desired_track_angle);
  draw_detail(canvas, wnd, wnd_rect, &wnd->brg_value, buf, wnd->info_color);

  // draw the distance
  sprintf(buf, "%d", (int)wnd->dist_to_waypoint);
  draw_detail(canvas, wnd, wnd_rect, &wnd->dst_value, buf, wnd->info_color);

  static const char* unknown_time = "--:--";
  static const char* time_fmt = "%02.2d:%02.2d";

  // draw the time to next
  if (wnd->time_to_next == -1)
    strcpy(buf, unknown_time);
  else
    sprintf(buf, time_fmt, wnd->time_to_next / 60, wnd->time_to_next % 60);

  draw_detail(canvas, wnd, wnd_rect, &wnd->ete_value, buf, wnd->info_color);

  bool alt_mode = 
    wnd->mode == ap_hdg_alt ||
    wnd->mode == ap_crs_alt ||
    wnd->mode == ap_alt ||
    wnd->mode == ap_180_left ||
    wnd->mode == ap_180_right;

  sprintf(buf, "%d", (int)wnd->altitude_set);
  
  draw_detail(canvas, wnd, wnd_rect, &wnd->alt_value, buf, alt_mode ? wnd->active_color : wnd->standby_color);

  sprintf(buf, "%d", (int)wnd->vertical_speed);
  draw_detail(canvas, wnd, wnd_rect, &wnd->vs_value, buf, alt_mode ? wnd->active_color : wnd->standby_color);
  }

static result_t ap_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  ap_widget_t* wnd = (ap_widget_t*)wnddata;

  bool changed = false;
  uint16_t id = get_can_id(msg);

 
  switch (id)
    {
    case id_desired_track_angle:
      {
      uint16_t v;
      get_param_uint16(msg, &v);

      changed = wnd->desired_track_angle != v;
      }
      break;
    }

  if (changed)
    invalidate(hwnd);

  return widget_wndproc(hwnd, msg, wnddata);
  }

void draw_detail(handle_t canvas,
  ap_widget_t* wnd,
  const rect_t* wnd_rect,
  const rect_t* rect,
  const char* txt,
  color_t fg)
  {
  if (txt == 0)
    return;

  // center the string
  point_t pt;
  rect_top_left(rect, &pt);

  int32_t cell_width = wnd->font_cell_size.dx;

  size_t len = strlen(txt);

  pt.x += rect_width(rect) >> 1;
  pt.x -= (int32_t)((cell_width * len) >> 1);

  pt.y += rect_height(rect) >> 1;
  pt.y -= wnd->font_cell_size.dy >> 1;

  size_t c;
  for (c = 0; c < len; c++)
    {
    extent_t cell_size;
    text_extent(wnd->base.name_font, 1, txt + c, &cell_size);

    point_t cp;
    point_copy(&pt, &cp);

    cp.x += cell_width >> 1;
    cp.x -= cell_size.dx >> 1;

    draw_text(canvas, wnd_rect, wnd->base.name_font, fg, wnd->base.background_color, 1, txt + c,
      &cp, rect, eto_clipped, 0);

    pt.x += cell_width;
    }
  }

result_t on_create_autopilot_widget(handle_t hwnd, widget_t *widget)
  {
  result_t result = s_ok;

  return result;
  }

result_t create_autopilot_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, ap_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, ap_wndproc, &wnd->base, &hndl)))
    return result;

  // calculate the rectangles
  rect_t wnd_rect;
  window_rect(hndl, &wnd_rect);

  int32_t border = wnd->gutter << 1;

  wnd->mode_button.left = wnd_rect.left + border;
  wnd->mode_button.top = wnd_rect.top + border;
  wnd->mode_button.right = wnd_rect.right - border;
  wnd->mode_button.bottom = wnd->mode_button.top + wnd->button_height;

  // calculate the max txt width
  extent_t label_txt_ex;
  text_extent(wnd->base.name_font, 0, wpt_label, &label_txt_ex);

  // get the label width

  point_t label_pt = { wnd->gutter, wnd->mode_button.bottom + wnd->gutter };
  point_t value_pt = { wnd->gutter, label_pt.y + wnd->height + wnd->gutter };

  extent_t label_ex = { wnd_rect.right - border, wnd->height + wnd->gutter };
  extent_t value_ex = { wnd_rect.right - border, wnd->height + wnd->gutter };

  int32_t y_incr = (wnd->height + wnd->gutter) << 1;

  // create the waypoint details
  rect_create_ex(label_pt.x, label_pt.y, label_ex.dx, label_ex.dy, &wnd->wpt_label);
  rect_create_ex(value_pt.x, value_pt.y, value_ex.dx, value_ex.dy, &wnd->wpt_value);
  label_pt.y += y_incr;
  value_pt.y += y_incr;

  // create the bearing label
  rect_create_ex(label_pt.x, label_pt.y, label_ex.dx, label_ex.dy, &wnd->brg_label);
  rect_create_ex(value_pt.x, value_pt.y, value_ex.dx, value_ex.dy, &wnd->brg_value);
  label_pt.y += y_incr;
  value_pt.y += y_incr;

  // create the distance details
  rect_create_ex(label_pt.x, label_pt.y, label_ex.dx, label_ex.dy, &wnd->dst_label);
  rect_create_ex(value_pt.x, value_pt.y, value_ex.dx, value_ex.dy, &wnd->dst_value);
  label_pt.y += y_incr;
  value_pt.y += y_incr;

  // create the time details
  rect_create_ex(label_pt.x, label_pt.y, label_ex.dx, label_ex.dy, &wnd->ete_label);
  rect_create_ex(value_pt.x, value_pt.y, value_ex.dx, value_ex.dy, &wnd->ete_value);
  label_pt.y += y_incr;
  value_pt.y += y_incr;

  // create the altitude details
  rect_create_ex(label_pt.x, label_pt.y, label_ex.dx, label_ex.dy, &wnd->alt_label);
  rect_create_ex(value_pt.x, value_pt.y, value_ex.dx, value_ex.dy, &wnd->alt_value);
  label_pt.y += y_incr;
  value_pt.y += y_incr;

  // create the vertical speed
  rect_create_ex(label_pt.x, label_pt.y, label_ex.dx, label_ex.dy, &wnd->vs_label);
  rect_create_ex(value_pt.x, value_pt.y, value_ex.dx, value_ex.dy, &wnd->vs_value);

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }