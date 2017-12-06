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
#include "../photon/window.h"
#include "spatial.h"
#include "pens.h"

#include <math.h>

typedef struct _airspeed_window_t {
  size_t version;
  
  uint16_t vs0;
  uint16_t vs1;
  uint16_t vfe;
  uint16_t vno;
  uint16_t vne;
  uint16_t va;
  uint16_t vx;
  uint16_t vy;
  
  float scale;
  float offset;
	
  // this is updated on each window message
  long airspeed;
  
  color_t background_color;
  color_t text_color;
  pen_t pen;
  handle_t  font;
	bool draw_border;
  handle_t large_roller;
  handle_t small_roller;
} airspeed_window_t;

static result_t get_vs0(handle_t hwnd, const char *property_name, void *parg, variant_t *value)
  {
  airspeed_window_t *wnd = (airspeed_window_t *)parg;
  value->dt = field_uint16;
  value->v_uint16 = wnd->vs0;

  return s_ok;
  }

static result_t set_vs0(handle_t hwnd, const char *property_name, void *parg, const variant_t *value)
  {
  airspeed_window_t *wnd = (airspeed_window_t *)parg;
  if (value->dt != field_uint16)
    return e_bad_type;

  bool changed = wnd->vs0 != value->v_uint16;
  if (changed)
    {
    wnd->vs0 = value->v_uint16;
    invalidate_rect(hwnd, 0);
    }

  return s_ok;
  }

static result_t on_paint(handle_t hwnd, void *parg, const char *func, const canmsg_t *msg)
  {
  begin_paint(hwnd);

  airspeed_window_t *wnd = (airspeed_window_t *) parg;

  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);
  
  extent_t ex;
  rect_extents(&wnd_rect, &ex);
  
  rect_t rect;

  rectangle(hwnd, &wnd_rect, 0, color_hollow,  make_rect(0, 0, ex.dx, 8, &rect));
  rectangle(hwnd, &wnd_rect, 0, color_hollow,  make_rect(0, 8, 8, ex.dy, &rect));
  rectangle(hwnd, &wnd_rect, 0, color_hollow,  make_rect(0, ex.dy-8, ex.dx, ex.dy, &rect));
  rectangle(hwnd, &wnd_rect, 0, color_hollow,  make_rect(ex.dx-9, 8, ex.dx, ex.dy-8, &rect));

  rectangle(hwnd, &wnd_rect, 0, wnd->background_color,  make_rect(8, 8, ex.dx-9, ex.dy-8, &rect));

  gdi_dim_t median = ex.dy >> 1;

  // the vertical tape displays 28 knots around the current position
  // as there are 240 pixels.  We calc the upper one first
  gdi_dim_t top_asi = ((gdi_dim_t)wnd->airspeed * 10) + median -10;

  // assign the first line airspeed
  gdi_dim_t asi_line =(top_asi / 25) * 25;
  gdi_dim_t marker_line;
  // work out how many lines to the next lowest marker
  for(marker_line =(top_asi - asi_line)+ 10; marker_line < ex.dy; marker_line += 25)
    {
    // draw a line from 10 pixels to 30 pixels then the text.
    // lines at 25 are shorter
    point_t pts[2] = {
      { asi_line ==((asi_line / 50) * 50) ? 50 : 55, marker_line },
      { 65, marker_line }
      };

    polyline(hwnd, &wnd_rect, &wnd->pen, pts, 2);

    if(asi_line ==((asi_line / 100) * 100))
      {
      char str[64];
      sprintf(str, "%d",(int)asi_line / 10);

      uint16_t len = strlen(str);
      extent_t size;
      text_extent(hwnd, wnd->font, str, len, &size);
      point_t pt;

      draw_text(hwnd, &wnd_rect, wnd->font, wnd->text_color, wnd->background_color,
                str, len, make_point(47 - size.dx, marker_line -(size.dy >> 1), &pt),
                0, 0, 0);
      }

    asi_line -= 25;

    if(asi_line < 0)
      break;
    }

  point_t roller[8] = 
    {
    { 47,  median },
    { 40,  median+7 },
    { 40,  median+20 },
    { 0,  median+20 },
    { 0,  median-20 },
    { 40,  median-20 },
    { 40,  median-7 },
    { 47,  median }
    };

  polygon(hwnd, &wnd_rect, &white_pen, color_black, roller, 8);

  // now we draw the roller
  display_roller(hwnd, make_rect(1, median-19, 39, median+19, &rect), 
                 wnd->airspeed, 1, color_black, color_white,
                 wnd->large_roller, wnd->small_roller);

  // finally draw the markers that indicate the v-speeds
  // each knot is 10 pixels in the y direction
  // the scale is 240 pixels high. 

  gdi_dim_t vne_pixels = top_asi - wnd->vne + 10;
  gdi_dim_t vno_pixels = top_asi - wnd->vno + 10;
  gdi_dim_t va_pixels = top_asi - wnd->va + 10;
  gdi_dim_t vfe_pixels = top_asi - wnd->vfe + 10;
  gdi_dim_t vs0_pixels = top_asi - wnd->vs0 + 10;      // stall flaps extended
  gdi_dim_t vs1_pixels = top_asi - wnd->vs1 + 10;      // stall flaps up
  gdi_dim_t vx_pixels = top_asi - wnd->vx + 10;        // best angle of climb
  gdi_dim_t vy_pixels = top_asi - wnd->vy + 10;        // best rate of climb

  // draw vne exceeded
  if(vne_pixels >= 8)
    rectangle(hwnd, &wnd_rect, 0, color_red, make_rect(75, 8, 79, min((gdi_dim_t)ex.dy-8, vne_pixels), &rect));

  // draw vne->vno
  if(vno_pixels >= (gdi_dim_t)8 && vne_pixels < (gdi_dim_t)ex.dy-8)
    rectangle(hwnd, &wnd_rect, 0, color_yellow,
              make_rect(75, max((gdi_dim_t)8, vne_pixels), 79,
                        min((gdi_dim_t)ex.dy-8, vno_pixels), &rect));

  // draw vno->vs1
  if(vs1_pixels >= 8 && vno_pixels < 232)
    rectangle(hwnd, &wnd_rect, 0, color_green,
              make_rect(75, max((gdi_dim_t)8, vno_pixels),
                        79, min((gdi_dim_t)ex.dy-8, vs1_pixels), &rect));

  // draw vfe->vs0
  if(vs0_pixels >= 8 && vfe_pixels < 232)
    rectangle(hwnd, &wnd_rect, 0, color_white,
              make_rect(71, max((gdi_dim_t)8, vfe_pixels),
                        75, min((gdi_dim_t)ex.dy-8, vs0_pixels), &rect));

  // draw vy -> vx
  if(vx_pixels >= 8 && vy_pixels < 232)
    rectangle(hwnd, &wnd_rect, 0, color_blue,
              make_rect(67, max((gdi_dim_t)8, vy_pixels), 71,
                        min((gdi_dim_t)ex.dy-8, vx_pixels), &rect));

  end_paint(hwnd);
  return s_ok;
  }

static result_t on_indicated_airspeed(handle_t hwnd, void *parg, const char *func, const canmsg_t *msg)
  {
  bool changed = false;

  airspeed_window_t *wnd = (airspeed_window_t *)parg;
  
  // airspeed is in m/s convert to display value (knots, km, m/h etc.)
  float v;
  get_param_float(msg, &v);

  float airspeed = v * wnd->scale;
  airspeed += wnd->offset;
      
  long value = (long)(roundf(airspeed));
      
  changed = wnd->airspeed != value;
  wnd->airspeed = value;

  if(changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

result_t create_airspeed_window(handle_t parent, memid_t key, handle_t *hwnd)
  {
  result_t result;

  // create our window
  if (failed(result = create_child_widget(parent, key, defwndproc, hwnd)))
    return result;

  // create the window data.
  airspeed_window_t *wnd = (airspeed_window_t *)neutron_malloc(sizeof(airspeed_window_t));
  memset(wnd, 0, sizeof(airspeed_window_t));

  wnd->version = sizeof(airspeed_window_t);

  reg_get_uint16(key, "vs0", &wnd->vs0);
  reg_get_uint16(key, "vs1", &wnd->vs1);
  reg_get_uint16(key, "vfe", &wnd->vfe);
  reg_get_uint16(key, "vno", &wnd->vno);
  reg_get_uint16(key, "vne", &wnd->vne);
  reg_get_uint16(key, "va", &wnd->va);
  reg_get_uint16(key, "vx", &wnd->vx);
  reg_get_uint16(key, "vy", &wnd->vy);

  // this conversion factor is for knots
  if (failed(reg_get_float(key, "scale", &wnd->scale)))
    wnd->scale = 1 / 0.5144444445610519f;

  reg_get_float(key, "offset", &wnd->offset);

  if (failed(lookup_font(key, "font", &wnd->font)))
    {
    // we always have the neo font.
    if (failed(result = create_font("neo", 9, 0, &wnd->font)))
      return result;
    }

  if (failed(lookup_font(key, "large-font", &wnd->large_roller)))
    {
    // we always have the neo font.
    if (failed(result = create_font("neo", 15, 0, &wnd->font)))
      return result;
    }

  if (failed(lookup_font(key, "small-font", &wnd->small_roller)))
    {
    // we always have the neo font.
    if (failed(result = create_font("neo", 12, 0, &wnd->font)))
      return result;
    }

  if (failed(lookup_color(key, "back-color", &wnd->background_color)))
    wnd->background_color = color_black;

  if (failed(lookup_color(key, "text-color", &wnd->text_color)))
    wnd->text_color = color_white;

  memid_t pen_key;
  if (failed(reg_open_key(key, "pen", &pen_key)) ||
    failed(lookup_pen(key, &wnd->pen)))
    {
    wnd->pen.color = color_white;
    wnd->pen.width = 1;
    wnd->pen.style = ps_solid;
    }

  // store the parameters for the window
  set_wnddata(*hwnd, wnd);

  add_property(*hwnd, "vs0", wnd, get_vs0, set_vs0, field_uint16, 0);

  add_event(*hwnd, id_paint, wnd, 0, on_paint);
  add_event(*hwnd, id_indicated_airspeed, wnd, 0, on_indicated_airspeed);

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);

  return s_ok;
  }
