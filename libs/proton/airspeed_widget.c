#include "airspeed_widget.h"
#include "converters.h"

static void on_paint_background(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void *wnddata)
  {
  airspeed_widget_t* wnd = (airspeed_widget_t *)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  rect_t rect;
  rectangle(canvas, wnd_rect, 0, color_darkgrey, rect_create(8, 8, ex.dx - 9, ex.dy - 8, &rect));
  }

static void on_paint_foreground(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  airspeed_widget_t* wnd = (airspeed_widget_t*)wnddata;

  // fill the background with hollow
  rectangle(canvas, wnd_rect, color_hollow, color_hollow, wnd_rect);

  extent_t ex;
  rect_extents(wnd_rect, &ex);

  rect_t rect;

  int32_t median = ex.dy >> 1;

  int32_t bar_width = rect_width(wnd_rect);
  // where to draw data
  rect_t roller_box;
  rect_t value_box;


  // the vertical tape displays 28 knots around the current position
  // as there are 240 pixels.  We calc the upper one first
  int32_t top_asi = ((int32_t)wnd->airspeed * wnd->pixels_per_unit) + median - wnd->pixels_per_unit;

  // assign the first line airspeed
  int32_t asi_line =(top_asi / 25) * 25;
  int32_t marker_line;
  // work out how many lines to the next lowest marker
  for(marker_line =(top_asi - asi_line)+ wnd->pixels_per_unit; marker_line < ex.dy; marker_line += 25)
    {
    // draw a line from 10 pixels to 30 pixels then the text.
    // lines at 25 are shorter
    point_t pts[2] =
      {
      { asi_line ==((asi_line / 50) * 50) ? bar_width-20 : bar_width-18, marker_line },
      { bar_width-13, marker_line }
      };

    polyline(canvas, wnd_rect, wnd->pen, 2, pts);

    if(asi_line ==((asi_line / 100) * 100))
      {
      char str[64];
      sprintf(str, "%d",(int)asi_line / wnd->pixels_per_unit);

      uint16_t len = (uint16_t) strlen(str);
      extent_t size;
      text_extent(wnd->font, len, str, &size);
      point_t pt;

      draw_text(canvas, wnd_rect, wnd->font, wnd->text_color, wnd->base.background_color,
                len, str, point_create(bar_width-20 - size.dx, marker_line -(size.dy >> 1), &pt),
                0, 0, 0);
      }

    asi_line -= 25;

    if(asi_line < 0)
      break;
    }

  // finally draw the markers that indicate the v-speeds
  // each knot is 10 pixels in the y direction
  // the scale is 240 pixels high. 

  int32_t vne_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vne) * wnd->pixels_per_unit);
  int32_t vno_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vno) * wnd->pixels_per_unit);
  int32_t va_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->va) * wnd->pixels_per_unit);
  int32_t vfe_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vfe) * wnd->pixels_per_unit);
  int32_t vs0_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vs0) * wnd->pixels_per_unit);      // stall flaps extended
  int32_t vs1_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vs1) * wnd->pixels_per_unit);      // stall flaps up
  int32_t vx_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vx) * wnd->pixels_per_unit);        // best angle of climb
  int32_t vy_pixels = top_asi - (int32_t)(to_display_airspeed->convert_float(wnd->aircraft->vy) * wnd->pixels_per_unit);        // best rate of climb

  int32_t bar0 = bar_width - 12;
  int32_t bar1 = bar_width - 8;
  int32_t bar2 =bar_width - 4;

  // draw vne exceeded
  if(vne_pixels >= 8)
    rectangle(canvas, wnd_rect, 0, color_red, rect_create(bar2, 8, bar2+4, min((int32_t)ex.dy-8, vne_pixels), &rect));

  // draw vne->vno
  if(vno_pixels >= (int32_t)8 && vne_pixels < (int32_t)ex.dy-8)
    rectangle(canvas, wnd_rect, 0, color_yellow,
              rect_create(bar2, max((int32_t)8, vne_pixels), bar2+4,
                        min((int32_t)ex.dy-8, vno_pixels), &rect));

  // draw vno->vs1
  if(vs1_pixels >= 8 && vno_pixels < 232)
    rectangle(canvas, wnd_rect, 0, color_green,
              rect_create(bar2, max((int32_t)8, vno_pixels),
                        bar2+4, min((int32_t)ex.dy-8, vs1_pixels), &rect));

  // draw vfe->vs0
  if(vs0_pixels >= 8 && vfe_pixels < 232)
    rectangle(canvas, wnd_rect, 0, color_white,
              rect_create(bar1, max((int32_t)8, vfe_pixels),
                        bar1+4, min((int32_t)ex.dy-8, vs0_pixels), &rect));

  // draw vy -> vx
  if(vx_pixels >= 8 && vy_pixels < 232)
    rectangle(canvas, wnd_rect, 0, color_blue,
              rect_create(bar0, max((int32_t)8, vy_pixels), bar0 + 4,
                        min((int32_t)ex.dy-8, vx_pixels), &rect));


  // draw the roller over the top of the other info
  rect_create(8, median - 20, bar_width -8, median + 20, &roller_box);
  on_paint_roller_background(canvas, &roller_box, color_black, color_white, false, &value_box);

  roller_box.right -= 20;
  on_display_roller(canvas, &roller_box,
    (uint32_t)(to_display_airspeed->convert_float(wnd->airspeed)), 1,  color_white,
    wnd->large_roller, wnd->small_roller);
  }

static result_t on_indicated_airspeed(handle_t hwnd, airspeed_widget_t *wnd, const canmsg_t *msg)
  {

  return s_ok;
  }

static result_t airspeed_wndproc(handle_t hwnd, const canmsg_t *msg, void *wnddata)
  {
  airspeed_widget_t *wnd = (airspeed_widget_t *)wnddata;

  if (get_can_id(msg) == id_indicated_airspeed)
    {
    bool changed = false;

    // airspeed is in m/s convert to display value (knots, km, m/h etc.)
    float v;
    get_param_float(msg, &v);

    float airspeed = to_display_airspeed->convert_float(v);
    uint16_t value = (uint16_t)(roundf(airspeed));

    changed = wnd->airspeed != value;
    wnd->airspeed = value;

    if (changed)
      invalidate_foreground_rect(hwnd, 0);

    return s_ok;
    }

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_airspeed_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, airspeed_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, airspeed_wndproc, &wnd->base, &hndl)))
    return result;

  wnd->aircraft = aircraft;
  wnd->base.on_paint_background = on_paint_background;
  wnd->base.on_paint_foreground = on_paint_foreground;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }