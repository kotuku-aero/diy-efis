#include "../../include/altitude_widget.h"
#include "../../include/converters.h"

typedef struct _vsi_markers {
  const char* text;
  uint16_t length;
  int pos;
  } vsi_markers;

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  altitude_widget_t* wnd = (altitude_widget_t*)wnddata;
  extent_t ex;
  rect_extents(wnd_rect, &ex);

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  rect_t rect;
  gdi_dim_t median_y = ex.dy >> 1;

  // vsi markers
  int i;

  const vsi_markers marks[] = {
    { "3", 1, median_y - 88 },
    { "2", 1, median_y - 68 },
    { "1", 1, median_y - 48 },
    { "1", 1, median_y + 47 },
    { "2", 1, median_y + 67 },
    { "3", 1, median_y + 87 }
    };

  for (i = 0; i < 6; i++)
    {
    extent_t size;
    point_t pt;
    text_extent(wnd->font, marks[i].length, marks[i].text, &size);
    draw_text(canvas, wnd_rect, wnd->font, color_yellow, wnd->base.background_color,
      marks[i].length, marks[i].text,
      point_create(ex.dx - 9 - size.dx, marks[i].pos - (size.dy >> 1), &pt),
      0, 0, 0);
    }

  // draw the assigned altitude rate marked (TODO)

  char str[64];
  rect_t paint_area;

  rect_create(8, 8, ex.dx - 8, ex.dy - 8, &paint_area);

  // the vertical tape displays 250 ft = 20 pixels
  int32_t num_pixels = rect_height(&paint_area) >> 1;
  float num_grads = (float)(num_pixels / 20.0f);
  num_grads *= 250;           // altitude offset

  num_grads += 8;

  int32_t top_altitude = wnd->altitude + (int32_t)num_grads;

  // roundf the height to 10 pixels
  top_altitude = (top_altitude - ((top_altitude / 10) * 10)) > 5
    ? ((top_altitude / 10) + 1) * 10
    : ((top_altitude / 10) * 10);

  // assign the first line altitude
  int32_t line_altitude = (top_altitude / 250) * 250;
  // work out how many lines to the next lowest marker
  int32_t marker_line;
  for (marker_line = ((top_altitude - line_altitude) / 10) + 5;
    marker_line < (ex.dy - 8); marker_line += 20)
    {
    // draw a line from 10 pixels to 20 pixels then the text
    point_t pts[2] =
      {
      { 10, marker_line },
      { 20, marker_line }
      };

    polyline(canvas, &paint_area, wnd->pen, 2, pts);

    if (line_altitude == ((line_altitude / 500) * 500))
      {
      sprintf(str, "%d", (int)line_altitude);

      uint16_t len = (uint16_t)strlen(str);
      extent_t size;
      text_extent(wnd->font, len, str, &size);
      point_t pt;

      draw_text(canvas, &paint_area, wnd->font, wnd->text_color, wnd->base.background_color,
        len, str, point_create(23, marker_line - (size.dy >> 1), &pt),
        0, 0, 0);
      }

    line_altitude -= 250;

    if (line_altitude < 0)
      break;
    }

  median_y = ex.dy >> 1;
  int32_t bar_width = rect_width(wnd_rect);

  rect_t roller_box;
  rect_t value_box;
  rect_create(8, median_y - 20, bar_width - 15, median_y + 20, &roller_box);

  on_paint_roller_background(canvas, &roller_box, color_black, color_white, true, &value_box);

  on_display_roller(canvas, &roller_box, wnd->altitude, 2, wnd->text_color, wnd->large_roller, wnd->small_roller);

  /////////////////////////////////////////////////////////////////////////////
  //
  //	Now display the vertical speed.
  //
  int32_t vs;
  // draw the marker.  There is a non-linear scale
  if (wnd->vertical_speed < 1000 && wnd->vertical_speed > -1000)
    // +/- 48 pixels
    vs = median_y - ((int32_t)((float)(wnd->vertical_speed) * (48.0 / 1000.0)));
  else
    {
    vs = min((int16_t)3000, max((int16_t)-3000, wnd->vertical_speed));

    // make absolute
    vs = abs(vs);

    vs -= 1000;       // makes pixels from 1000 ft mark
    vs /= 50;         // 40 pixels = 2000 ft
    vs += 48;         // add the 1000 ft marks

    if (wnd->vertical_speed < 0)
      vs *= -1;

    vs = median_y - vs;        // add the base marker
    }

  int32_t vs_base = median_y;
  if (vs == median_y)
    {
    vs_base--;
    vs++;
    }

  if (vs_base < vs)
    {
    int32_t tmp = vs;
    vs = vs_base;
    vs_base = tmp;
    }

  rect_t vs_rect;
  rectangle(canvas, wnd_rect, color_hollow, color_white,
    rect_create(ex.dx - 8, vs, ex.dx - 1, vs_base, &vs_rect));

  // draw the assigned altitude at the top of the VSI
  rect_t text_rect;
  rect_create(23, 0, ex.dx - 8, 18, &text_rect);

  rectangle(canvas, wnd_rect, color_white, color_black, &text_rect);

  text_rect.left++;
  text_rect.top++;
  text_rect.right--;
  text_rect.bottom--;

  // roundf the assigned altitude to 10 feet
  vs = (int) wnd->assigned_altitude;
  if (abs((vs / 5) % 5) == 1)
    vs = ((vs / 10) * 10) + (vs < 0 ? -10 : 10);
  else
    vs = (vs / 10) * 10;

  sprintf(str, "%d", vs);

  uint16_t len = (uint16_t)strlen(str);
  extent_t size;
  text_extent(wnd->font, len, str, &size);

  point_t pt;

  draw_text(canvas, &text_rect, wnd->font, color_green, color_black,
    len, str,
    point_create(text_rect.left + (rect_width(&text_rect) >> 1) - (size.dx >> 1),
      text_rect.top + 1, &pt),
    &text_rect, 0, 0);


  // draw the current QNH
  rect_create(23, ex.dy - 19, ex.dx - 8, ex.dy - 1, &text_rect);

  rectangle(canvas, wnd_rect, color_white, color_black, &text_rect);

  text_rect.left++;
  text_rect.top++;
  text_rect.right--;
  text_rect.bottom--;

  if (wnd->qnh_converter != nullptr)
    {
    float qnh = wnd->qnh_converter->convert(wnd->qnh);
    if(wnd->qnh_converter->is_float)
      sprintf(str, wnd->qnh_converter->format, qnh);
    else
      sprintf(str, wnd->qnh_converter->format, (int) qnh);
    }
  else
    sprintf(str, "%d", (int) wnd->qnh);

  len = (uint16_t)strlen(str);
  text_extent(wnd->font, len, str, &size);

  draw_text(canvas, &text_rect, wnd->font, color_green, color_black,
    len, str,
    point_create(text_rect.left + (rect_width(&text_rect) >> 1) - (size.dx >> 1),
      text_rect.top + 1, &pt),
    &text_rect, 0, 0);
  }

result_t altitude_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  altitude_widget_t* wnd = (altitude_widget_t*)wnddata;
  bool changed = false;
  switch (get_can_id(msg))
    {
    case id_baro_corrected_altitude:
      {
      float v;
      get_param_float(msg, &v);

      // Convert to display units (feet)
      int16_t value = (int16_t)wnd->altitude_converter->convert(v);
      changed = wnd->altitude != value;
      wnd->altitude = value;
      }
      break;
    case id_altitude_rate:
      {
      float v;
      get_param_float(msg, &v);

      // alt rate is meters/sec
      // alt rate is ft/min not ft/sec the value passed is m/sec
      int16_t value = (int16_t)roundf(wnd->alt_rate_converter->convert(v));

      changed = wnd->vertical_speed != value;
      wnd->vertical_speed = value;
      }
      break;
    case id_qnh:
      {
      float value;
      get_param_float(msg, &value);
      changed = wnd->qnh != value;
      wnd->qnh = value;
      }
      break;
    case id_ap_altitude:
      {
      float value;
      get_param_float(msg, &value);
      int16_t v = (int16_t)roundf(wnd->altitude_converter->convert(value));

      if(changed = wnd->assigned_altitude != v)
        wnd->assigned_altitude = v;
      }
      break;

    case id_ap_vertical_rate:
      {
      float value;
      get_param_float(msg, &value);
      int16_t v = (int16_t)roundf(wnd->altitude_converter->convert(value));

      if(changed = wnd->assigned_altitude_rate != v)
        wnd->assigned_altitude_rate = v;
      }
      break;
    }

  if (changed)
    invalidate(hwnd);

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_altitude_widget(handle_t parent, uint32_t flags, altitude_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, altitude_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->base.on_paint = on_paint;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }