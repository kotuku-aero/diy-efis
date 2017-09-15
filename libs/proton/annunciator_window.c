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
#include "fonts.h"
#include "pens.h"

static const point_t clock_pt = { 1, 10 };
static const point_t hrs_pt = { 1, 55 };
static const point_t qnh_pt = { 1, 100 };
static const point_t oat_pt = { 1, 145 };
static const point_t cas_pt = { 1, 190 };

static const char *hours_value = "TTS";
static const char *qnh_value = "QNH";

typedef struct _annunciator_window_t {
  size_t version;

  bool draw_border;
  uint32_t hours; // hobbs hours, stored in AHRS as hours * 100
  uint16_t qnh; // qnh, stored in AHRS
  uint16_t clock;
  int16_t oat;
  uint16_t cas;
  } annunciator_window_t;

static result_t widget_wndproc(const window_msg_t *data);
/**
 * Draw an annunciator
 * @param hwnd      Window to draw on
 * @param wnd_rect  rectange of the window
 * @param pt        point in window to draw at
 * @param label     Text label
 * @param value     value to print
 */
static void draw_annunciator(handle_t hwnd, const rect_t *wnd_rect, annunciator_window_t *wnd, const point_t *pt, const char *label, const char *value);

result_t create_annunciator_window(handle_t parent, memid_t key, handle_t *hwnd)
  {
  result_t result;

  // create our window
  if (failed(create_child_widget(parent, key, widget_wndproc, hwnd)))
    return result;

  // create the window data.
  annunciator_window_t *wnd = (annunciator_window_t *)kmalloc(sizeof(annunciator_window_t));
  memset(wnd, 0, sizeof(annunciator_window_t));

  wnd->version = sizeof(annunciator_window_t);


  reg_get_bool(key, "draw-border", &wnd->draw_border);

  // store the parameters for the window
  set_wnddata(*hwnd, wnd);

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);

  return s_ok;
  }

static void update_window(handle_t hwnd, annunciator_window_t *wnd)
  {
  rect_t wnd_rect;
  get_window_rect(hwnd, &wnd_rect);

  extent_t ex;
  rect_extents(&wnd_rect, &ex);

  rect_t rect;
  if (wnd->draw_border)
    round_rect(hwnd, &wnd_rect, &white_pen, color_hollow, &wnd_rect, 12);
  char msg[10];

  sprintf(msg, "%02.2d:%02.2d", wnd->clock / 60, wnd->clock % 60);
  draw_annunciator(hwnd, &wnd_rect, wnd, &clock_pt, "UTC", msg);

  double hrs = ((double)wnd->hours) / 100;

  sprintf(msg, "%06.1f", wnd->hours);
  draw_annunciator(hwnd, &wnd_rect, wnd, &hrs_pt, "Hrs", msg);

  sprintf(msg, "%d", wnd->qnh);
  draw_annunciator(hwnd, &wnd_rect, wnd, &qnh_pt, "QNH", msg);

  sprintf(msg, "%d", wnd->oat);
  draw_annunciator(hwnd, &wnd_rect, wnd, &oat_pt, "OAT", msg);

  sprintf(msg, "%d", wnd->cas);
  draw_annunciator(hwnd, &wnd_rect, wnd, &cas_pt, "CAS", msg);
  }

static result_t widget_wndproc(const window_msg_t *data)
  {
  bool changed = false;
  annunciator_window_t *wnd;
  get_wnddata(data->hwnd, (void **)&wnd);


  switch (data->msg.id)
    {
    case id_def_utc:
    {
    uint8_t v;
    uint16_t minutes;
    get_param_uint8(&data->msg, 0, &v);
    minutes = v * 60;
    get_param_uint8(&data->msg, 1, &v);
    minutes += v;

    changed = wnd->clock != minutes;
    wnd->clock = minutes;
    }
    break;
    case id_qnh:
    {
    uint16_t value;
    get_param_uint16(&data->msg, 0, &value);
    changed = wnd->qnh != value;
    wnd->qnh = value;
    }
    break;
    case id_true_airspeed:
    {
    uint16_t value;
    get_param_uint16(&data->msg, 0, &value);
    changed = wnd->cas != value;
    wnd->cas = value;
    }
    break;
    case id_outside_air_temperature:
    {
    int16_t value;
    get_param_int16(&data->msg, 0, &value);
    changed = wnd->oat != value;
    wnd->oat = value;
    }
    break;
    case id_air_time:
    {
    uint32_t value;
    get_param_uint32(&data->msg, &value);
    changed = wnd->hours != value;
    wnd->hours = value;
    }
    break;
    case id_paint:
      begin_paint(data->hwnd);
      update_window(data->hwnd, wnd);
      end_paint(data->hwnd);
      break;
    default:
      return defwndproc(data);
    }

  if (changed)
    invalidate_rect(data->hwnd, 0);

  return s_ok;
  }

static void draw_annunciator(handle_t hwnd,
  const rect_t *wnd_rect,
  annunciator_window_t *wnd,
  const point_t *pt,
  const char *label,
  const char *value)
  {
  // calculate the size of the label
  rect_t clip_rect = { pt->x, pt->y, pt->x + 75, pt->y + 50 };
  //  font(&arial_9_font);
  size_t text_len = strlen(label);

  rect_t rect;

  extent_t label_size;
  text_extent(hwnd, &arial_9_font, label, text_len, &label_size);

  gdi_dim_t width = rect_width(wnd_rect);
  gdi_dim_t text_start = pt->x + width - (label_size.dx + 3);
  gdi_dim_t right = pt->x + width;

  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(pt->x, pt->y + 1, right, pt->y + 4, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(pt->x, pt->y + 4, pt->x + 3, pt->y + 28, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(right - 3, pt->y + 4, right, pt->y + 45, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(pt->x, pt->y + 28, right, pt->y + 31, &rect));
  rectangle(hwnd, wnd_rect, 0, color_gray, make_rect(text_start - 6, pt->y + 31, right, pt->y + 45, &rect));

  rectangle(hwnd, wnd_rect, 0, color_black, make_rect(pt->x + 3, pt->y + 4, right - 3, pt->y + 28, &rect));

  point_t label_origin = { text_start, pt->y + 30 };

  make_rect(text_start, pt->y + 30,
    text_start + label_size.dx,
    pt->y + 30 + label_size.dy,
    &rect);

  draw_text(hwnd, wnd_rect, &arial_9_font, color_white, color_black,
    label, 0, &label_origin, &rect, 0, 0);

  extent_t text_size;
  text_extent(hwnd, &arial_15_font, value, 0, &text_size);

  // center of the text is 37, 16
  point_t origin = {
    pt->x + 37 - (text_size.dx >> 1),
    pt->y + 16 - (text_size.dy >> 1)
    };

  make_rect_pt(&origin, &text_size, &rect);

  draw_text(hwnd, wnd_rect, &arial_15_font, color_white, color_hollow,
    value, 0, &origin, &rect, 0, 0);
  }
