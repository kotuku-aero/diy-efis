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
#include "pens.h"

static const rect_t waypoint = { 4, 10, 130, 55 };
static const rect_t waypoint_text = { 7, 15, 126, 50 };

static const rect_t dist_to_waypoint = { 4, 55, 130, 100 };
static const rect_t dist_to_waypoint_text = { 7, 60, 126, 95 };

static const rect_t time_to_waypoint = { 4, 100, 130, 145 };
static const rect_t time_to_waypoint_text = { 7, 105, 126, 140 };

static const rect_t next_waypoint = { 4, 145, 130, 240 };
static const rect_t next_waypoint_clip_area = { 7, 150, 126, 235 };
static const rect_t next_waypoint_text_0 = { 7, 150, 126, 175 };
static const rect_t next_waypoint_text_1 = { 7, 180, 126, 205 };
static const rect_t next_waypoint_text_2 = { 7, 210, 126, 235};

typedef struct _gps_window_t {
  uint16_t version;
  
  extent_t font_cell_size;
  
  const handle_t  *font;

  // current waypoint we are heading to.
  uint16_t dist_to_waypoint;
  int16_t time_to_next;
  // text to display as waypoints
  char waypoints[4][16];
  bool draw_border;
  color_t background_color;

  } gps_window_t;
  
static void draw_background(handle_t hwnd, gps_window_t *wnd, const rect_t *wnd_rect, const rect_t *pt, const char *label);
static void draw_detail(handle_t hwnd, gps_window_t *wnd, 
                        const rect_t *clip_area, const rect_t *text_area, 
                        const char *value, color_t fg);

static result_t widget_wndproc(handle_t hwnd, const canmsg_t *data);

result_t create_gps_window(handle_t parent, memid_t key, handle_t *hwnd)
  {
	result_t result;
	
  // create our window
	if(failed(result = create_child_widget(parent, key, widget_wndproc, hwnd)))
		return result;
	
  // create the window data.
  gps_window_t *wnd = (gps_window_t *)neutron_malloc(sizeof(gps_window_t));
  memset(wnd, 0, sizeof(gps_window_t));
  
  wnd->version = sizeof(gps_window_t);
  
  wnd->time_to_next = -1;
  
  if (failed(lookup_font(key, "font", &wnd->font)))
    {
    // we always have the neo font.
    if (failed(result = open_font("neo", 9, &wnd->font)))
      return result;
    }
  
  text_extent(hwnd, wnd->font, "M", 1, &wnd->font_cell_size);
  wnd->font_cell_size.dx += 2;

  if(failed(lookup_color(key, "background-color", &wnd->background_color)))
    wnd->background_color = color_black;

  reg_get_bool(key, "draw-border", &wnd->draw_border);

  // store the parameters for the window
  set_wnddata(*hwnd, wnd);

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);
	
	return s_ok;
  }

static void update_window(handle_t hwnd, gps_window_t *wnd)
  {
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);
  
  extent_t ex;
  rect_extents(&wnd_rect, &ex);
  
  // fill without a border
  rectangle(hwnd, &wnd_rect, 0, wnd->background_color, &wnd_rect);

  if (wnd->draw_border)
    round_rect(hwnd, &wnd_rect, &gray_pen, color_hollow, &wnd_rect, 12);
  
  draw_background(hwnd, wnd, &wnd_rect, &waypoint, "Waypoint");
  draw_background(hwnd, wnd, &wnd_rect, &dist_to_waypoint, "Distance");
  draw_background(hwnd, wnd, &wnd_rect, &time_to_waypoint, "Time");
  draw_background(hwnd, wnd, &wnd_rect, &next_waypoint, "Next");

  // display the waypoint
  draw_detail(hwnd, wnd, &wnd_rect, &waypoint_text, wnd->waypoints[0], color_magenta);

  // display next waypoint(s)
  draw_detail(hwnd, wnd, &wnd_rect, &next_waypoint_text_0, wnd->waypoints[1], color_green);
  draw_detail(hwnd, wnd, &wnd_rect, &next_waypoint_text_1, wnd->waypoints[2], color_green);
  draw_detail(hwnd, wnd, &wnd_rect, &next_waypoint_text_2, wnd->waypoints[3], color_green);

  // draw the distance
  char buf[64];
  sprintf(buf, "%d",(int)wnd->dist_to_waypoint);

  draw_detail(hwnd, wnd, &wnd_rect, &dist_to_waypoint_text, buf, color_magenta);

  static const char *unknown_time = "--:--";
  static const char *time_fmt = "%02.2d:%02.2d";

  // draw the time to next
  if(wnd->time_to_next == -1)
    strcpy(buf, unknown_time);
  else
    sprintf(buf, time_fmt, wnd->time_to_next/60, wnd->time_to_next % 60);

  draw_detail(hwnd, wnd, &wnd_rect, &time_to_waypoint_text, buf, color_magenta);
  }

result_t widget_wndproc(handle_t hwnd, const canmsg_t *msg)
  {
  bool changed = false;
  gps_window_t *wnd;
  get_wnddata(hwnd, (void **)&wnd);
  
  switch(msg->id)
    {
    case id_paint :
      begin_paint(hwnd);
      update_window(hwnd, wnd);
      end_paint(hwnd);
      break;
//    case 	id_new_route :
      // read the route
  //    {
  //    aio_application_t *aio_app = reinterpret_cast<aio_application_t *>(application_t::instance);

  //    const base_gps_device_t *gps = aio_app->gps_device();

  //    size_t waypoint = 0;
  //    size_t destination_waypoint = gps->destination_waypoint();
  //    size_t waypoint_count = gps->waypoint_count() - destination_waypoint;

  //    for(waypoint = 0; waypoint < waypoint_count && waypoint < 4; waypoint++)
  //      {
  //      strncpy(&(_waypoints[waypoint][0]), gps->get_waypoint_name(waypoint + destination_waypoint), 15);
  //      _waypoints[waypoint][15] = 0;
  //      }

  //    for(; waypoint < 4; waypoint++)
  //      _waypoints[waypoint][0] = 0;

  //    changed = true;
  //    }
  //    break;
  //  case id_distance_to_destination :
  //    changed = assign_msg<int16_t>(msg.msg_data_16(), _dist_to_waypoint);
  //    break;
  //  case id_time_to_destination :
  //    changed = assign_msg<int16_t>(msg.msg_data_16(), _time_to_next);
  //    break;
    default :
      return defwndproc(hwnd, msg);
    }

  if(changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

void draw_background(handle_t hwnd,
                     gps_window_t *wnd,
                     const rect_t *wnd_rect, 
                     const rect_t *text_area,
                     const char *label)
{
   
  size_t text_len = strlen(label);
  extent_t label_size;
  text_extent(hwnd, wnd->font, label, text_len, &label_size);

  int width = rect_width(text_area);
  int text_start = text_area->left + 3;
  int text_end = text_start + label_size.dx;
  int right = text_area->left + width;
  
  rect_t rect;

  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(text_area->left, text_area->top+1, right, text_area->top + 4, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(text_area->left, text_area->top+4, text_area->left + 3, text_area->bottom - 17, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(right -3, text_area->top+4, right, text_area->bottom -14, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(text_area->left, text_area->bottom - 17, right, text_area->bottom - 14, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(text_area->left, text_area->bottom - 14, text_end + 3, text_area->bottom, &rect));

  point_t text_origin = { text_start, text_area->bottom - 15 };

  draw_text(hwnd, wnd_rect, wnd->font, color_white, color_hollow, label, text_len,
            &text_origin, make_rect(text_origin.x, text_origin.y,
                                   text_origin.x + label_size.dx,
                                   text_origin.y + label_size.dy, &rect ), 0, 0);
}

void draw_detail(handle_t hwnd,
                 gps_window_t *wnd,
                 const rect_t *wnd_rect,
                 const rect_t *rect,
                 const char *txt,
                 color_t fg)
  {
  rectangle(hwnd, wnd_rect, 0, wnd->background_color, rect);

  if(txt == 0)
    return;

  // center the string
  point_t pt;
  top_left(rect, &pt);

  gdi_dim_t cell_width = wnd->font_cell_size.dx;

  size_t len = strlen(txt);

  pt.x += rect_width(rect) >> 1;
  pt.x -= (cell_width * len) >> 1;

  pt.y += rect_height(rect) >> 1;
  pt.y -= wnd->font_cell_size.dy >> 1;

  size_t c;
  for(c = 0; c < len; c++)
    {
    extent_t cell_size;
    text_extent(wnd, wnd->font, txt + c, 1, &cell_size);

    point_t cp;
    copy_point(&pt, &cp);
    
    cp.x += cell_width >> 1;
    cp.x -= cell_size.dx >> 1;

    draw_text(hwnd, wnd_rect, wnd->font, fg, wnd->background_color, txt + c,
              1, &cp, rect, eto_clipped, 0);

    pt.x += cell_width;
    }
  }
