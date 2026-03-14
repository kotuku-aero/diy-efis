#include <stdio.h>

#include "../../include/map_widget.h"

typedef struct _map_entity_t map_entity_t;

static void on_paint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* _msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  // based on the current scale, draw the range circle that is scale / 4
  extent_t ex;
  rect_extents(wnd_rect, &ex);

  // draw the map decoratins
  on_paint_widget_background(canvas, wnd_rect, _msg, wnddata);

  // see if borders
  rect_t map_rect;
  rect_copy(wnd_rect, &map_rect);

  uint32_t style = wnd->base.style;
  
  if (style & BORDER_LEFT)
    map_rect.left++;

  if (style & BORDER_RIGHT)
    map_rect.right--;

  if (style & BORDER_TOP)
    map_rect.top++;

  if (style & BORDER_BOTTOM)
    map_rect.bottom--;

  map_render_canvas(wnd->map, &map_rect, canvas);

  // create the corners of the display, it could be rotated
  // in respect to the screen
  // TODO: cache these when the position/rotation changes
  // in gtopo30 1 pixel = 1km
  int32_t pixel_dist = ex.dx >> 1;


  // ellipse(canvas, wnd_rect, color_white, color_hollow, &range_rect);

  }

static result_t map_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  map_widget_t* wnd = (map_widget_t*)wnddata;

  bool changed = false;
  switch (get_can_id(msg))
    {
    case id_position_lat:
      get_param_float(msg, &wnd->gps_position.lat);
      break;
    case id_position_lng:
      get_param_float(msg, &wnd->gps_position.lng);
      break;
    case id_gps_height:
      get_param_float(msg, &wnd->gps_position.alt);
      break;
    case id_true_track:
      // we don't use magnetic
      break;
    case id_gps_groundspeed:
      break;
    case id_gps_valid:
      // save the value
      changed = succeeded(map_update_position(wnd->map, &wnd->gps_position, wnd->heading, wnd->track));
      break;
    case id_magnetic_heading:
      {
      int16_t direction;
      get_param_int16(msg, &direction);

      while (direction < 0)
        direction += 360;
      while (direction > 359)
        direction -= 360;

      changed = wnd->track != direction;
      wnd->track = direction;
      }
      break;
    case id_selected_course:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->course != value;
      wnd->course = value;
      }
      break;
    case id_track:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->track != value;
      wnd->track = value;
      }
      break;
    case id_heading:
      {
      int16_t value;
      get_param_int16(msg, &value);
      changed = wnd->heading != value;
      wnd->heading = value;
      }
      break;
    case id_touch_zoom:
      {
      // TODO: pass message as binary
      // withthe zoom center 
      int16_t value;
      get_param_int16(msg, &value);

      map_zoom(wnd->map, value);

      changed = true;
      }
      break;
    case id_touch_pan:
      {
      uint32_t value;
      get_param_uint32(msg, &value);

      extent_t ex;
      ex.dx = (gdi_dim_t)(((int32_t)value) >> 16);
      ex.dy = (gdi_dim_t)((int16_t)(value & 0xffff));

      map_pan(wnd->map, &ex);

      changed = true;
      }
      break;
    }

  if (changed)
    invalidate(hwnd);

  // pass to default
  return widget_wndproc(hwnd, msg, wnddata);
  }

result_t create_map_widget(handle_t parent, uint32_t flags, map_widget_t* wnd, handle_t* out)
  {
  result_t result;
  if (failed(result = create_widget(parent, flags, map_wndproc, &wnd->base, &wnd->hwnd)))
    return result;

  extent_t background_ex = { rect_width(&wnd->base.rect), rect_height(&wnd->base.rect) };

  // TODO: should be smarter than this.
  if (failed(result = map_create_canvas(wnd->hwnd, &background_ex, &wnd->day_theme, &wnd->map)))
    return result;

  wnd->base.on_paint = on_paint;

  // TODO: this should be from a setting
  wnd->map_mode = mdm_track;

  if (out != 0)
    *out = wnd->hwnd;

  return s_ok;

  }


