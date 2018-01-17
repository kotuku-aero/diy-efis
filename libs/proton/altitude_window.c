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
#include "../photon/widget.h"
#include "pens.h"
#include "spatial.h"

typedef struct _vsi_markers {
	const char *text;
	uint16_t length;
	int pos;
} vsi_markers;

typedef struct _altitude_window_t {
  uint16_t version;
  int16_t altitude;
  int16_t vertical_speed;

  uint16_t qnh;
  color_t background_color;
  color_t text_color;
  pen_t pen;
  handle_t  font;      // arial 9
  handle_t  small_roller;  // neo 12
  handle_t  large_roller;  // neo 15
} altitude_window_t;
 
static result_t on_paint(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  begin_paint(hwnd);

  altitude_window_t *wnd = (altitude_window_t *)proxy->parg;
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);
  
  extent_t ex;
  rect_extents(&wnd_rect, &ex);
  
  rect_t rect;
  gdi_dim_t median_y = ex.dy >> 1;

  rectangle(hwnd, &wnd_rect, 0, wnd->background_color, make_rect(8, 8, ex.dx-8, ex.dy-8, &rect));
	
  int i;

	const vsi_markers marks[] = {
		{ "3", 1, median_y-88 },
		{ "2", 1, median_y-68 },
		{ "1", 1, median_y-48 },
		{ "1", 1, median_y+47 },
		{ "2", 1, median_y+67 },
		{ "3", 1, median_y+87 }
		};

	for(i = 0; i < 6; i++)
		{
		extent_t size;
    point_t pt;
    text_extent(hwnd, wnd->font, marks[i].text, marks[i].length, &size);
		draw_text(hwnd, &wnd_rect, wnd->font, color_yellow, wnd->background_color,
             marks[i].text, marks[i].length,
             make_point(ex.dx - 9 - size.dx, marks[i].pos -(size.dy >> 1), &pt),
             0, 0, 0);
		}
  
  char str[64];
  rect_t paint_area;
  
  make_rect(8, 8, ex.dx -8, ex.dy-8, &paint_area);

	// the vertical tape displays 250 ft = 20 pixels
	gdi_dim_t num_pixels = rect_height(&paint_area) >> 1;
	float num_grads = num_pixels / 20.0;
	num_grads *= 250;           // altitude offset

	num_grads += 8;

	gdi_dim_t top_altitude = wnd->altitude + (gdi_dim_t)num_grads;

	// roundf the height to 10 pixels
	top_altitude =(top_altitude -((top_altitude / 10) * 10)) > 5
	    ? ((top_altitude / 10) + 1) * 10
			: ((top_altitude / 10) * 10);

	// assign the first line altitude
	gdi_dim_t line_altitude =(top_altitude / 250) * 250;
	// work out how many lines to the next lowest marker
	gdi_dim_t marker_line;
	for(marker_line =((top_altitude - line_altitude) / 10)+ 10;
			marker_line < (ex.dy - 8); marker_line += 20)
		{
		// draw a line from 10 pixels to 20 pixels then the text
		point_t pts[2] =
			{
			{ 10, marker_line },
			{ 20, marker_line }
			};

		polyline(hwnd, &paint_area, &wnd->pen, 2, pts);

		if(line_altitude ==((line_altitude / 500) * 500))
			{
			sprintf(str, "%d",(int)line_altitude);

			uint16_t len = strlen(str);
			extent_t size;
      text_extent(hwnd, wnd->font, str, len, &size);
      point_t pt;
      
			draw_text(hwnd, &paint_area, wnd->font, wnd->text_color, wnd->background_color,
               str, len, make_point(23, marker_line -(size.dy >> 1), &pt),
               0, 0, 0);
			}

		line_altitude -= 250;

		if(line_altitude < 0)
			break;
		}

	median_y = ex.dy >> 1;

	const point_t roller[8] =
		{
		{ 23,  median_y },
		{ 35,  median_y+12 },
		{ 35,  median_y+20 },
		{ ex.dx-8,   median_y+20 },
		{ ex.dx-8,   median_y-20 },
		{ 35,  median_y-20 },
		{ 35,  median_y-12 },
		{ 23,  median_y }
		};

	polygon(hwnd, &wnd_rect, &white_pen, color_black, 8, roller);

	rect_t text_rect;
  (36, median_y-19, 88, median_y+19);
	display_roller(hwnd, make_rect(36, median_y-19, ex.dx-8, median_y+19, &text_rect),
                 wnd->altitude, 2, color_black, wnd->text_color, wnd->large_roller, wnd->small_roller);

	/////////////////////////////////////////////////////////////////////////////
	//
	//	Now display the vertical speed.
	//
	gdi_dim_t vs;
	// draw the marker.  There is a non-linear scale
	if(wnd->vertical_speed < 1000 && wnd->vertical_speed > -1000)
		// +/- 48 pixels
		vs = median_y - ((gdi_dim_t)((double)(wnd->vertical_speed) *(48.0 / 1000.0)));
	else
		{
    vs = min((int16_t)3000, max((int16_t)-3000, wnd->vertical_speed));

    // make absolute
    vs = abs(vs);

    vs -= 1000;       // makes pixels from 1000 ft mark
    vs /= 50;         // 40 pixels = 2000 ft
    vs += 48;         // add the 1000 ft marks

    if(wnd->vertical_speed < 0)
      vs *= -1;

    vs = median_y - vs;        // add the base marker
		}

  gdi_dim_t vs_base = median_y;
  if(vs == median_y)
    {
    vs_base--;
    vs++;
    }

  if(vs_base < vs)
    {
    gdi_dim_t tmp = vs;
    vs = vs_base;
    vs_base = tmp;
    }

	rect_t vs_rect;
  rectangle(hwnd, &wnd_rect, 0, color_white,
            make_rect(ex.dx-8, vs, ex.dx-1, vs_base, &vs_rect));

  // draw the text at the top of the VSI
  rect_t vsi_rect;
  make_rect(23, 0, ex.dx-8, 18, &vsi_rect);

  rectangle(hwnd, &wnd_rect, &white_pen, color_black, &vsi_rect);

  vsi_rect.left++;
  vsi_rect.top++;
  vsi_rect.right--;
  vsi_rect.bottom--;

  // roundf the vs to 10 feet
  vs = wnd->vertical_speed;
  if(abs((vs / 5)%5) == 1)
    vs = ((vs /10) * 10)+(vs < 0 ? -10 : 10);
  else
    vs = (vs/10) * 10;

	sprintf(str, "%d", vs);

  uint16_t len = strlen(str);
	extent_t size;
  text_extent(hwnd, wnd->font, str, len, &size);
  point_t pt;

	draw_text(hwnd, &vsi_rect, wnd->font, color_green, color_black,
           str, len,
           make_point(vsi_rect.left + (rect_width(&vsi_rect)>> 1) - (size.dx >> 1),
               vsi_rect.top+1, &pt),
           &vsi_rect, 0, 0);

	// draw the current QNH
  make_rect(23, ex.dy - 19, ex.dx-8, ex.dy-1, &vsi_rect);

  rectangle(hwnd, &wnd_rect, &white_pen, color_black, &vsi_rect);

  vsi_rect.left++;
  vsi_rect.top++;
  vsi_rect.right--;
  vsi_rect.bottom--;


  sprintf(str, "%d", wnd->qnh);

  len = strlen(str);
  text_extent(hwnd, wnd->font, str, len, &size);

  draw_text(hwnd, &vsi_rect, wnd->font, color_green, color_black,
           str, len,
           make_point(vsi_rect.left + (rect_width(&vsi_rect)>> 1) - (size.dx >> 1),
               vsi_rect.top+1, &pt),
           &vsi_rect, 0, 0);

  end_paint(hwnd);
	}

static result_t on_baro_corrected_altitude(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  altitude_window_t *wnd = (altitude_window_t *)proxy->parg;
  bool changed = false;
  
  float v;
  get_param_float(msg, &v);
  int16_t value = (int16_t)roundf(v);
  changed = wnd->altitude != value;
  wnd->altitude = value;

	if(changed)
	  invalidate_rect(hwnd, 0);

	return s_ok;
	}

static result_t on_altitude_rate(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  altitude_window_t *wnd = (altitude_window_t *)proxy->parg;
  bool changed = false;

  float v;
  get_param_float(msg, &v);
  int16_t value = (int16_t)roundf(v);
  changed = wnd->vertical_speed != value;
  wnd->vertical_speed = value;

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

static result_t on_qnh(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg)
  {
  altitude_window_t *wnd = (altitude_window_t *)proxy->parg;
  bool changed = false;

  uint16_t value;
  get_param_uint16(msg, 0, &value);
  changed = wnd->vertical_speed != value;
  wnd->qnh = value;

  if (changed)
    invalidate_rect(hwnd, 0);

  return s_ok;
  }

result_t create_altitude_window(handle_t parent, memid_t key, handle_t *hwnd)
  {
  result_t result;

  // create our window
  if (failed(result = create_child_widget(parent, key, defwndproc, hwnd)))
    return result;

  // create the window data.
  altitude_window_t *wnd = (altitude_window_t *)neutron_malloc(sizeof(altitude_window_t));
  memset(wnd, 0, sizeof(altitude_window_t));

  wnd->version = sizeof(altitude_window_t);

  if (failed(lookup_font(key, "font", &wnd->font)))
    {
    // we always have the neo font.
    if (failed(result = open_font("neo", 9, &wnd->font)))
      return result;
    }

  if (failed(lookup_font(key, "large-font", &wnd->large_roller)))
    {
    // we always have the neo font.
    if (failed(result = open_font("neo", 12, &wnd->large_roller)))
      return result;
    }

  if (failed(lookup_font(key, "small-font", &wnd->small_roller)))
    {
    // we always have the neo font.
    if (failed(result = open_font("neo", 9, &wnd->small_roller)))
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

  wnd->qnh = 1013;

  // store the parameters for the window
  set_wnddata(*hwnd, wnd);

  add_event(*hwnd, id_paint, wnd, 0, on_paint);
  add_event(*hwnd, id_baro_corrected_altitude, wnd, 0, on_baro_corrected_altitude);
  add_event(*hwnd, id_altitude_rate, wnd, 0, on_altitude_rate);
  add_event(*hwnd, id_qnh, wnd, 0, on_qnh);

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);

  return s_ok;
  }
