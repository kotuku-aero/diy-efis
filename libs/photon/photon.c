//#define _DEBUG_POLYGON
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

#include "window.h"

/**
* @struct window_msg_t
*/
typedef struct _window_msg_t {
  canmsg_t msg;
  handle_t hwnd;
  } window_msg_t;

static screen_t *phys_screen;

static void on_timer(const canmsg_t *msg)
  {
  }

static const canmsg_t paint_msg = {
  .flags = id_paint,
  .canas.data_type = CANAS_DATATYPE_NODATA
  };

result_t get_canvas(handle_t hwnd, canvas_t **canvas)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  // proton does not use a separate canvas
  *canvas = wnd->canvas;
  return s_ok;
  }

static bool msg_hook(const canmsg_t *msg, void *parg)
  {
  return succeeded(post_message(0, msg, 0));
  }

// this is defined in window.c
extern result_t on_paint(handle_t hwnd, event_proxy_t *proxy, const canmsg_t *msg);

result_t open_screen(uint16_t orientation, wndproc cb, uint16_t id,
    handle_t *hwnd)
  {
  result_t result;
  canvas_t *canvas;

  // root window creation
  if (failed(result = bsp_canvas_open_framebuffer(&canvas)))
    return result;

  phys_screen = (screen_t *)neutron_malloc(sizeof(screen_t));

  memset(phys_screen, 0, sizeof(screen_t));

  canvas->orientation = orientation;

  phys_screen->wnd.canvas = canvas;
  phys_screen->wnd.parent = 0;
  phys_screen->wnd.next = 0;
  phys_screen->wnd.previous = 0;

  // initialize the font system
  vector_create(sizeof(void *), &phys_screen->fonts);

  deque_create(sizeof(window_msg_t), WINDOW_QUEUE_SIZE, &phys_screen->event_queue);

  phys_screen->wnd.version = sizeof(screen_t);
  phys_screen->wnd.position.left = 0;
  phys_screen->wnd.position.top = 0;
  phys_screen->wnd.position.right = canvas->width;
  phys_screen->wnd.position.bottom = canvas->height;

  phys_screen->wnd.canvas = canvas;
  phys_screen->wnd.window_proc = cb;

  // where to route can messages to
  phys_screen->msg_hook.callback = msg_hook;
  // hook the messages
  subscribe(&phys_screen->msg_hook);

  // attach the default event handlers to the window
  add_event(phys_screen, id_paint, phys_screen, 0, on_paint);

  *hwnd = phys_screen;

  return s_ok;
  }

result_t get_screen(handle_t *screen)
  {
  if(screen == 0)
    return e_bad_parameter;

  if(phys_screen == 0)
    return e_unexpected;

  *screen = phys_screen;
  return s_ok;
  }

result_t create_rect_canvas(const extent_t *size, handle_t *hndl)
  {
  result_t result;
  canvas_t *canvas;
  if (failed(result = bsp_canvas_create_rect(size, &canvas)))
    return result;

  *hndl = canvas;
  return s_ok;
  }

result_t create_bitmap_canvas(const bitmap_t *bitmap, handle_t *hndl)
  {
  return bsp_canvas_create_bitmap(bitmap, (canvas_t **) hndl);
  }

result_t get_wnddata(handle_t hwnd, void **wnd_data)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;


  *wnd_data = wnd->wnd_data;
  return s_ok;
  }

result_t set_wnddata(handle_t hwnd, void *wnd_data)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;


  wnd->wnd_data = wnd_data;
  return s_ok;
  }

result_t get_orientation(handle_t hwnd, uint16_t *orientation)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;


  return wnd->canvas->orientation;
  }

result_t set_orientation(handle_t hwnd, uint16_t orientation)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  if (orientation != 0 && orientation != 90 && orientation != 180
      && orientation != 270)
    return e_bad_parameter;


  wnd->canvas->orientation = orientation;

  return s_ok;
  }

result_t get_message(handle_t hscreen, handle_t *hwnd, canmsg_t *msg)
  {
  result_t result;

  screen_t *screen;
  if (failed(result = as_screen(hscreen, &screen)))
    return result;

  if(hwnd == 0)
    return e_bad_parameter;

  uint16_t queue_count;

  window_msg_t wndmsg;

  if (failed(result = count(screen->event_queue, &queue_count)))
    return result;
  // see if the queue is empty...
  if (queue_count == 0)
    {
    if (succeeded((*screen->wnd.canvas->queue_empty)(hwnd)) &&
     screen->wnd.invalid)
      {
      memcpy(msg, &paint_msg, sizeof(canmsg_t));
      *hwnd = hscreen;
      return s_ok;          // paint message found
      }

    // this means the window is painted, and there are no more messages
    }

  // get the message off the queue
  pop_front(screen->event_queue, &wndmsg, INDEFINITE_WAIT);

  *hwnd = wndmsg.hwnd;
  memcpy(msg, &wndmsg.msg, sizeof(canmsg_t));

  return s_ok;
  }

// send a message to the chain of windows
static void broadcast_message(window_t *wnd, const canmsg_t *msg)
  {
  // send to all
  while (wnd != 0)
    {
    // send to self
    send_message(wnd, msg);
    // recursive send to children
    broadcast_message(wnd->child, msg);
    // skip next
    wnd = wnd->next;
    }
  }

result_t dispatch_message(handle_t hwnd, const canmsg_t *msg)
  {
  // send the message to the wndproc
  if (hwnd == 0)
    {
    if (phys_screen == 0)
      return e_bad_parameter;

    // this is a broadcast message so we send it to
    // the root window.
    broadcast_message(&phys_screen->wnd, msg);

    return s_ok;
    }

  // send the message directly to the hwnd of the
  // message
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  return (*wnd->window_proc)(hwnd, msg);
  }

result_t send_message(handle_t hwnd, const canmsg_t *msg)
  {
  result_t result;

  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  return (*wnd->window_proc)(hwnd, msg);
  }

result_t post_message(handle_t hwnd, const canmsg_t *msg, uint32_t max_wait)
  {
  result_t result;

  if (hwnd != 0 && failed(result = as_window(hwnd, 0)))
    return result;

  // prepare the window message to despatch
  window_msg_t wndmsg;
  wndmsg.hwnd = hwnd;
  memcpy(&wndmsg.msg, msg, sizeof(canmsg_t));

  // if the message is a paint message it is handled very
  // differently as the message is only sent to the window
  // when there are no more messages
  if (get_can_id(msg) == id_paint)
    {
    if (hwnd != 0)
      {
      window_t *wnd = (window_t *) hwnd;

      // flag the canvas is invalid
      wnd->invalid = true;
      }

    // must be room as there are readers waiting on the queue, so
    // we just add a paint message.
    push_back(phys_screen->event_queue, &wndmsg, 0);

    return s_ok;
    }

  // and queue the message
  return push_back(phys_screen->event_queue, &wndmsg, max_wait);
  }

result_t canvas_close(handle_t hc)
  {
  canvas_t *canvas = (canvas_t *) hc;
  return bsp_canvas_close(canvas);
  }

result_t is_invalid(handle_t hwnd)
  {
  result_t result;
  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  return wnd->invalid ? s_ok : s_false;
  }

result_t begin_paint(handle_t hwnd)
  {
  result_t result;
  window_t *wnd;
  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  wnd->invalid = false;

  return (*wnd->canvas->begin_paint)(wnd);
  }

result_t end_paint(handle_t hwnd)
  {
  result_t result;
  window_t *wnd;

  if (failed(result = as_window(hwnd, &wnd)))
    return result;

  wnd->invalid = false;

  return (*wnd->canvas->end_paint)(wnd);
  }

result_t get_canvas_extents(handle_t hwnd, extent_t *extent, uint16_t *bpp)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  if (extent != 0)
    {
    extent->dx = canvas->width;
    extent->dy = canvas->height;
    }

  if (bpp != 0)
    *bpp = canvas->bits_per_pixel;

  return s_ok;
  }

color_t aplha_blend(color_t pixel, color_t back, uint8_t weighting)
  {
  // quick test of saturated color
  if (weighting == 255)
    return pixel;

  if (weighting == 0)
    return back;

  /* alpha blending the source and background colors */
  uint32_t rb = (((pixel & 0x00ff00ff) * weighting)
      + ((back & 0x00ff00ff) * (0xff - weighting))) & 0xff00ff00;
  uint32_t g = (((pixel & 0x0000ff00) * weighting)
      + ((back & 0x0000ff00) * (0xff - weighting))) & 0x00ff0000;

  // return as saturated color as alpha blend already done.
  return 0xff000000 | ((rb | g) >> 8);
  }

typedef int out_code_t;

static const int INSIDE = 0; // 0000
static const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

static const double rotn_table[90][2] =
  {
    {
    0.000000000, 1.000000000
    },
    {
    0.017452406, 0.999847695
    },
    {
    0.034899497, 0.999390827
    },
    {
    0.052335956, 0.998629535
    },
    {
    0.069756474, 0.997564050
    },
    {
    0.087155743, 0.996194698
    },
    {
    0.104528463, 0.994521895
    },
    {
    0.121869343, 0.992546152
    },
    {
    0.139173101, 0.990268069
    },
    {
    0.156434465, 0.987688341
    },
    {
    0.173648178, 0.984807753
    },
    {
    0.190808995, 0.981627183
    },
    {
    0.207911691, 0.978147601
    },
    {
    0.224951054, 0.974370065
    },
    {
    0.241921896, 0.970295726
    },
    {
    0.258819045, 0.965925826
    },
    {
    0.275637356, 0.961261696
    },
    {
    0.292371705, 0.956304756
    },
    {
    0.309016994, 0.951056516
    },
    {
    0.325568154, 0.945518576
    },
    {
    0.342020143, 0.939692621
    },
    {
    0.358367950, 0.933580426
    },
    {
    0.374606593, 0.927183855
    },
    {
    0.390731128, 0.920504853
    },
    {
    0.406736643, 0.913545458
    },
    {
    0.422618262, 0.906307787
    },
    {
    0.438371147, 0.898794046
    },
    {
    0.453990500, 0.891006524
    },
    {
    0.469471563, 0.882947593
    },
    {
    0.484809620, 0.874619707
    },
    {
    0.500000000, 0.866025404
    },
    {
    0.515038075, 0.857167301
    },
    {
    0.529919264, 0.848048096
    },
    {
    0.544639035, 0.838670568
    },
    {
    0.559192903, 0.829037573
    },
    {
    0.573576436, 0.819152044
    },
    {
    0.587785252, 0.809016994
    },
    {
    0.601815023, 0.798635510
    },
    {
    0.615661475, 0.788010754
    },
    {
    0.629320391, 0.777145961
    },
    {
    0.642787610, 0.766044443
    },
    {
    0.656059029, 0.754709580
    },
    {
    0.669130606, 0.743144825
    },
    {
    0.681998360, 0.731353702
    },
    {
    0.694658370, 0.719339800
    },
    {
    0.707106781, 0.707106781
    },
    {
    0.719339800, 0.694658370
    },
    {
    0.731353702, 0.681998360
    },
    {
    0.743144825, 0.669130606
    },
    {
    0.754709580, 0.656059029
    },
    {
    0.766044443, 0.642787610
    },
    {
    0.777145961, 0.629320391
    },
    {
    0.788010754, 0.615661475
    },
    {
    0.798635510, 0.601815023
    },
    {
    0.809016994, 0.587785252
    },
    {
    0.819152044, 0.573576436
    },
    {
    0.829037573, 0.559192903
    },
    {
    0.838670568, 0.544639035
    },
    {
    0.848048096, 0.529919264
    },
    {
    0.857167301, 0.515038075
    },
    {
    0.866025404, 0.500000000
    },
    {
    0.874619707, 0.484809620
    },
    {
    0.882947593, 0.469471563
    },
    {
    0.891006524, 0.453990500
    },
    {
    0.898794046, 0.438371147
    },
    {
    0.906307787, 0.422618262
    },
    {
    0.913545458, 0.406736643
    },
    {
    0.920504853, 0.390731128
    },
    {
    0.927183855, 0.374606593
    },
    {
    0.933580426, 0.358367950
    },
    {
    0.939692621, 0.342020143
    },
    {
    0.945518576, 0.325568154
    },
    {
    0.951056516, 0.309016994
    },
    {
    0.956304756, 0.292371705
    },
    {
    0.961261696, 0.275637356
    },
    {
    0.965925826, 0.258819045
    },
    {
    0.970295726, 0.241921896
    },
    {
    0.974370065, 0.224951054
    },
    {
    0.978147601, 0.207911691
    },
    {
    0.981627183, 0.190808995
    },
    {
    0.984807753, 0.173648178
    },
    {
    0.987688341, 0.156434465
    },
    {
    0.990268069, 0.139173101
    },
    {
    0.992546152, 0.121869343
    },
    {
    0.994521895, 0.104528463
    },
    {
    0.996194698, 0.087155743
    },
    {
    0.997564050, 0.069756474
    },
    {
    0.998629535, 0.052335956
    },
    {
    0.999390827, 0.034899497
    },
    {
    0.999847695, 0.017452406
    },
  };

typedef struct _quadrants_t
  {
    double s0, s1;
    double c0, c1;
  } quadrants_t;

// first 2 are sin conversions
// second are cos conversion
static const quadrants_t quadrants[4] =
  {
    {
    1.0, 0.0, 0.0, 1.0
    },     // 0..89
        {
        0.0, 1.0, -1.0, 0.0
        },     // 90..179
        {
        -1.0, 0.0, 0.0, -1.0
        },     // 180..269
        {
        0.0, -1.0, 1.0, 0.0
        }
  };

float roundf(float value)
  {
  return (float) (value < 0.0 ? ceil(value - 0.5) : floor(value + 0.5));
  }

double round(double value)
  {
  return value < 0.0 ? ceil(value - 0.5) : floor(value + 0.5);
  }

// angle is in degrees, not radians.
const point_t *rotate_point(const point_t *center, point_t *pt, int angle)
  {
  while(angle > 360)
    angle -= 360;

  while(angle < 0)
    angle += 360;

  int la = angle % 90;
  int qd = angle / 90;
  // convert the angle to a usefull form
  double cos_theta = rotn_table[la][0] * quadrants[qd].c0
      + rotn_table[la][1] * quadrants[qd].c1;
  double sin_theta = rotn_table[la][0] * quadrants[qd].s0
      + rotn_table[la][1] * quadrants[qd].s1;

  // calc the transformation
  gdi_dim_t x2 = (gdi_dim_t) (round(
      (pt->x - center->x) * cos_theta - (pt->y - center->y) * sin_theta));
  gdi_dim_t y2 = (gdi_dim_t) (round(
      (pt->x - center->x) * sin_theta + (pt->y - center->y) * cos_theta));

  pt->x = x2 + center->x;
  pt->y = y2 + center->y;
  
  return pt;
  }

out_code_t compute_out_code(const point_t *pt, const rect_t *rect)
  {
  out_code_t code;

  code = INSIDE;          // initialised as being inside of clip window

  if (pt->x < rect->left)           // to the left of clip window
    code |= LEFT;
  else if (pt->x > rect->right)      // to the right of clip window
    code |= RIGHT;
  if (pt->y < rect->top)           // below the clip window
    code |= BOTTOM;
  else if (pt->y > rect->bottom)      // above the clip window
    code |= TOP;

  return code;
  }

// clip a line to within the clipping rectangle.  Return true if part of line is within the rectangle
static bool clip_line(point_t *p1, point_t *p2, const rect_t *clip_rect)
  {
  // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
  out_code_t outcode0 = compute_out_code(p1, clip_rect);
  out_code_t outcode1 = compute_out_code(p2, clip_rect);

  while (true)
    {
    if (!(outcode0 | outcode1))
      { // Bitwise OR is 0. Trivially accept and get out of loop
      return true;        // line inside rectange
      }
    else if (outcode0 & outcode1)
      { // Bitwise AND is not 0. Trivially reject and get out of loop
      return false;       // line outside rectangle
      }
    else
      {
      // failed both tests, so calculate the line segment to clip
      // from an outside point to an intersection with clip edge
      double x, y;

      // At least one endpoint is outside the clip rectangle; pick it.
      out_code_t outcodeOut = outcode0 ? outcode0 : outcode1;

      // Now find the intersection point;
      // use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
      if (outcodeOut & TOP)
        {           // point is above the clip rectangle
        x = p1->x
            + (p2->x - p1->x) * (clip_rect->bottom - p1->y) / (p2->y - p1->y);
        y = clip_rect->bottom;
        }
      else if (outcodeOut & BOTTOM)
        { // point is below the clip rectangle
        x = p1->x
            + (p2->x - p1->x) * (clip_rect->top - p1->y) / (p2->y - p1->y);
        y = clip_rect->top;
        }
      else if (outcodeOut & RIGHT)
        {  // point is to the right of clip rectangle
        y = p1->y
            + (p2->y - p1->y) * (clip_rect->right - p1->x) / (p2->x - p1->x);
        x = clip_rect->right;
        }
      else if (outcodeOut & LEFT)
        {   // point is to the left of clip rectangle
        y = p1->y
            + (p2->y - p1->y) * (clip_rect->left - p1->x) / (p2->x - p1->x);
        x = clip_rect->left;
        }

      // Now we move outside point to intersection point to clip
      // and get ready for next pass.
      if (outcodeOut == outcode0)
        {
        p1->x = (gdi_dim_t)x;
        p1->y = (gdi_dim_t)y;
        outcode0 = compute_out_code(p1, clip_rect);
        }
      else
        {
        p2->x = (gdi_dim_t)x;
        p2->y = (gdi_dim_t)y;
        outcode1 = compute_out_code(p2, clip_rect);
        }
      }
    }
  }

typedef struct _flagged_point_t
  {
    point_t point;
    bool flag;
  } flagged_point_t;

static flagged_point_t *flagged_points_begin(vector_p pts)
  {
  flagged_point_t *buffer;
  vector_begin(pts, (void **) &buffer);
  return buffer;
  }

static uint16_t flagged_points_size(vector_p pts)
  {
  uint16_t size;
  result_t result = vector_count(pts, &size);

  return failed(result) ? 0 : size;
  }

static inline uint16_t points_size(vector_p pts)
  {
  uint16_t size;
  vector_count(pts, &size);

  return size;
  }

static inline point_t *points_begin(vector_p pts)
  {
  point_t *buffer;
  vector_begin(pts, (void **) &buffer);
  return buffer;
  }

static bool intersect_line(const point_t *p1, const point_t *p2, // line 1
    const point_t *p3, const point_t *p4,    // line 2
    point_t *ip)
  {
  // if lines do not intersect then exit
  gdi_dim_t d2 = (p1->x - p2->x) * (p3->y - p1->y)
      - (p1->y - p2->y) * (p3->x - p1->x);
  gdi_dim_t d3 = (p1->x - p2->x) * (p4->y - p1->y)
      - (p1->y - p2->y) * (p4->x - p1->x);

  if ((d2 < 0 && d3 < 0) || (d2 > 0 && d3 > 0))
    return false;

  d2 = (p3->x - p4->x) * (p1->y - p3->y) - (p3->y - p4->y) * (p1->x - p3->x);
  d3 = (p3->x - p4->x) * (p2->y - p3->y) - (p3->y - p4->y) * (p2->x - p3->x);

  if ((d2 < 0 && d3 < 0) || (d2 > 0 && d3 > 0))
    return false;

  gdi_dim_t d = (p1->x - p2->x) * (p3->y - p4->y)
      - (p1->y - p2->y) * (p3->x - p4->x);
  if (d == 0)
    return false;

  ip->x = ((p3->x - p4->x) * ((p1->x * p2->y) - (p1->y * p2->x))
      - (p1->x - p2->x) * ((p3->x * p4->y) - (p3->y * p4->x))) / d;
  ip->y = ((p3->y - p4->y) * ((p1->x * p2->y) - (p1->y * p2->x))
      - (p1->y - p2->y) * ((p3->x * p4->y) - (p3->y * p4->x))) / d;

  return true;
  }

static gdi_dim_t distance(const point_t *p1, const point_t *p2)
  {
  return ((p2->x - p1->x) * (p2->x - p1->x))
      + ((p2->y - p1->y) * (p2->y - p1->y));
  }

// is_left(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
static int is_left(const point_t *p0, const point_t *p1, const point_t *p2)
  {
  return ((p1->x - p0->x) * (p2->y - p0->y) - (p2->x - p0->x) * (p1->y - p0->y));
  }

// point_in_polygon(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only when P is outside)
static bool point_in_polygon(const point_t *pt, handle_t subject)
  {
  int wn = 0;    // the  winding number counter
  int i;

  flagged_point_t *fpt = flagged_points_begin(subject);
  uint16_t fps = flagged_points_size(subject);

  // loop through all edges of the polygon
  for (i = 0; i < fps - 1; i++)
    {   // edge from V[i] to  V[i+1]
    if (fpt[i].point.y <= pt->y)
      {          // start y <= P.y
      if (fpt[i + 1].point.y > pt->y) // an upward crossing
        if (is_left(&fpt[i].point, &fpt[i + 1].point, pt) > 0) // P left of  edge
          ++wn;            // have  a valid up intersect
      }
    else
      {                        // start y > P.y (no test needed)
      if (fpt[i + 1].point.y <= pt->y) // a downward crossing
        if (is_left(&fpt[i].point, &fpt[i + 1].point, pt) < 0) // P right of  edge
          --wn;            // have  a valid down intersect
      }
    }

  return wn > 0;
  }

typedef struct
  {
    point_t pt;
    uint16_t dist;
  } intersection_t;

static result_t polygon_intersect(const rect_t *clip_rect, const point_t *pts, uint16_t count, handle_t points)
  {
  // flagged point
  vector_p subject_points;
  // point
  vector_p clip_points;
  result_t result;

  point_t ip;
  uint16_t sp;
  uint16_t cp;
  int num_intersections = 0;
  int num_inside = 0;

  if(failed(result = vector_clear(points)))
    return result;

  if (failed(result = vector_create(sizeof(point_t), &clip_points)))
    return result;

  if (failed(result = vector_create(sizeof(flagged_point_t), &subject_points)))
    {
    vector_close(clip_points);
    return result;
    }

  flagged_point_t fp1;
  flagged_point_t fp2;
  point_t p1;
  point_t p2;

  // create a list of points and mark them as inside or outside the clipping rectangle
  for(sp = 0; sp < count; sp++)
    {
    fp1.point.x = pts[sp].x;
    fp1.point.y = pts[sp].y;
    fp1.flag = point_in_rect(pts + sp, clip_rect);

	  if(fp1.flag)
		  num_inside++;

    vector_push_back(subject_points, &fp1);
    }

  vector_push_back(clip_points, top_left(clip_rect, &ip));
  vector_push_back(clip_points, top_right(clip_rect, &ip));
  vector_push_back(clip_points, bottom_right(clip_rect, &ip));
  vector_push_back(clip_points, bottom_left(clip_rect, &ip));
  vector_push_back(clip_points, top_left(clip_rect, &ip));

  uint16_t fps = flagged_points_size(subject_points);
  uint16_t cps = points_size(clip_points);

  for (sp = 0; sp < fps - 1; sp++)
    {
    intersection_t intersections[2];
    uint16_t intersection = 0;

    vector_at(subject_points, sp, &fp1);
    vector_at(subject_points, sp + 1, &fp2);
    // scan the clipping rectangle
    for (cp = 0; cp < cps - 1; cp++)
      {
      // an arc on the polygon can intersect at most 2 edges however we need to know the closest
      // to the arc start.
      vector_at(clip_points, cp, &p1);
      vector_at(clip_points, cp + 1, &p2);

      if (intersect_line(&fp1.point, &fp2.point, &p1, &p2, &ip))
        {
        // when we have added an intersection we will find it again
        if (is_equal(&ip, &fp1.point))
          continue;

        if(intersection > 1)
          {
          trace_debug("Intersection range error");
          vector_close(clip_points);
          return e_unexpected;
          }

        copy_point(&ip, &intersections[intersection].pt);
        intersections[intersection].dist = cp;
        intersection++;
        }
      }

    if (intersection > 1)
      {
      // two intersections found for this line
      // make the first one the furthest distance
      if (distance(&fp1.point, &intersections[0].pt) < distance(&fp1.point, &intersections[1].pt))
        {
        uint16_t ts;

        swap_points(&intersections[0].pt, &intersections[1].pt);
        ts = intersections[0].dist;
        intersections[0].dist = intersections[1].dist;
        intersections[1].dist = ts;
        }
      }

    // intersections contains the ordered list of subject points
    while (intersection--)
      {
      copy_point(&intersections[intersection].pt, &ip);
      cp = intersections[intersection].dist;

      // check for case when points are on the clipping area
      if (!is_equal(&ip, &fp1.point) && 
          !is_equal(&ip, &fp2.point))
        {
        // we have a clipped line so add it to the result.
        flagged_point_t fpt;
        fpt.point.x = ip.x;
        fpt.point.y = ip.y;
        fpt.flag = true;

        vector_insert(subject_points, sp + 1, &fpt);
        vector_insert(clip_points, cp + 1, &ip);

        if (intersection > 0)
          {
          sp++;     // only skip if there are 2 intersections
          if (intersections[0].dist > cp)
            intersections[0].dist++; // adjust for inserted intersection before edge start
          }
        num_intersections++;
        }
      }
    }

#ifdef _DEBUG_POLYGON
  {
  uint16_t i;
  uint16_t n;
  trace_debug("There are %d intersections\r\n", num_intersections);
  trace_debug("Clipping points\r\n");
  vector_count(clip_points, &n);
  for(i = 0; i < n; i++)
    {
    point_t pt;
    vector_at(clip_points, i, &pt);
    trace_debug("Clip point %d, x:%d, y:%d\r\n", i, pt.x, pt.y);
    }
  trace_debug("Subject points\r\n");
  vector_count(subject_points, &n);
  for(i = 0; i < n; i++)
    {
    flagged_point_t pt;
    vector_at(subject_points, i, &pt);
    trace_debug("Flagged point %d, x:%d, y:%d, inside:%s\r\n", i, pt.point.x, pt.point.y, pt.flag ? "yes" : "no");
    }
  }
#endif

  if (num_intersections == 0)
    {
	// if no vectors interset the polygon then we are done
    if (num_inside > 0)
      vector_append(points, count, pts);
    else
      {
      // we need to see if the clip rect is inside the subject
      for (cp = 0; cp < points_size(clip_points) - 1; cp++)
        {
        vector_at(clip_points, cp, &p1);
        if (point_in_polygon(&p1, subject_points))
          {
          // copy all of the points
          vector_append(points, points_size(clip_points), points_begin(clip_points));
          break;
          }
        }
      }

    vector_close(clip_points);
    vector_close(subject_points);
    return s_ok;
    }

  // now we look for the first point on the subject that is inside, last point is first so ignore
  fps = flagged_points_size(subject_points);
  for(sp = 0; sp < fps - 1; sp++)
    {
    vector_at(subject_points, sp, &fp1);
    if(fp1.flag)
      {
      vector_push_back(points, &fp1.point);
      sp++;
      break;
      }
    }

  if (succeeded(vector_empty(points)))
    {
    vector_close(clip_points);
    vector_close(subject_points);
    return s_ok;             // no result as all outside
    }

  vector_at(points, 0, &ip);    // point just inserted
  // walk the list
  do
    {
    vector_at(subject_points, sp, &fp1);

    if (fp1.flag)
      {
      // next point is clipped point
      copy_point(&fp1.point, &ip);
      }
    else
      {
      // find the point in the clip_points
      cps = points_size(clip_points);
      for(cp = 0; cp < cps - 1; cp++)
        {
        vector_at(clip_points, cp, &p1);
        if(is_equal(&p1, &ip))
          break;                  // found last point inserted
        }

      cp++;           // skip next on clipping rectangle
      if (cp == cps - 1)
        cp = 0;

      do
        {
        vector_at(clip_points, cp, &ip);

        bool is_clipped = false;
        // see if the next point is on the subject
        fps = flagged_points_size(subject_points);
        for(sp = 0; sp < fps - 1; sp++)
          {
          vector_at(subject_points, sp, &fp1);
          if(is_equal(&fp1.point, &ip))
            {
            is_clipped = true;
            break;
            }
          }

        if (is_clipped)
          break;

        // point is part of the clipping rectangle.
        vector_push_back(points, &ip);
        cp++;

        // wrap around to the start till we find an intersection point
        if (cp == points_size(clip_points) - 1)
          cp = 0;
        }
      while (cp < points_size(clip_points) - 1);
      }

    vector_push_back(points, &ip);
    sp++;
    }
  while (!is_equal(&ip, points_begin(points)));

  // ensure we return a closed polygon
  vector_count(points, &sp);
  vector_at(points, 0, &p1);
  vector_at(points, sp-1, &p2);
  if(!is_equal(&p1, &p2))
    vector_push_back(points, &p1);  // close the polygon

  vector_close(clip_points);
  vector_close(subject_points);
  return s_ok;
  }

result_t polyline_impl(canvas_t *canvas, const rect_t *clip_rect, const pen_t *pen, uint16_t count, const point_t *points)
  {
  gdi_dim_t half_width = pen->width >> 1;
  point_t p1;
  point_t p2;
  uint16_t pt;

  for (pt = 0; (pt + 1) < count; pt++)
    {
    copy_point(&points[pt], &p1);
    copy_point(&points[pt + 1], &p2);

    // clip the line to the clipping area
    if (!clip_line(&p1, &p2, clip_rect))
      continue;                 // line is outside the clipping area

    // ensure the line is always top->bottom
    if (p1.y > p2.y)
      swap_points(&p1, &p2);

    // draw the first pixel
    if (point_in_rect(&p1, clip_rect))
      (*canvas->set_pixel)(canvas, &p1, pen->color);

    gdi_dim_t delta_x = p2.x - p1.x;
    gdi_dim_t delta_y = p2.y - p1.y;

    gdi_dim_t x_incr;
    if (delta_x >= 0)
      x_incr = 1;
    else
      {
      x_incr = -1;
      delta_x = -delta_x;
      }

    // we can optimize the drawing of horizontal and vertical lines
    if (delta_x == 0)
      {
      if (p1.y > p2.y)
        swap_points(&p1, &p2);

      while (p1.y < p2.y)
        {
        gdi_dim_t offset;
        if (point_in_rect(&p1, clip_rect))
          (*canvas->set_pixel)(canvas, &p1, pen->color);

        for (offset = 1; offset <= half_width; offset++)
          {
          point_t p2;
          copy_point(&p1, &p2);
          p2.x += offset;
          if (point_in_rect(&p2, clip_rect))
            (*canvas->set_pixel)(canvas, &p2,
                aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                    255 >> offset));

          p2.x = p1.x;
          p2.x -= offset;
          if (point_in_rect(&p2, clip_rect))
            (*canvas->set_pixel)(canvas, &p2,
                aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                    255 >> offset));
          }
        p1.y++;
        }
      }
    else if (delta_y == 0)
      {
      if (p1.x > p2.x)
        swap_points(&p1, &p2);

      while (p1.x < p2.x)
        {
        gdi_dim_t offset;
        if (point_in_rect(&p1, clip_rect))
          (*canvas->set_pixel)(canvas, &p1, pen->color);

        for (offset = 1; offset <= half_width; offset++)
          {
          point_t p2;

          copy_point(&p1, &p2);
          p2.y += offset;
          if (point_in_rect(&p1, clip_rect))
            (*canvas->set_pixel)(canvas, &p2,
                aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                    255 >> offset));

          p2.y = p1.y;
          p2.y -= offset;
          if (point_in_rect(&p2, clip_rect))
            (*canvas->set_pixel)(canvas, &p2,
                aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                    255 >> offset));
          }
        p1.x++;
        }
      }
    else if (delta_y == delta_x)
      {
      do
        {
        gdi_dim_t offset;
        if (point_in_rect(&p1, clip_rect))
          (*canvas->set_pixel)(canvas, &p1, pen->color);

        p1.x += x_incr;
        p1.y++;

        for (offset = 1; offset <= half_width; offset++)
          {
          point_t p2;
          p2.x = p1.x + offset;
          p2.y = p1.y + offset;

          if (point_in_rect(&p2, clip_rect))
            (*canvas->set_pixel)(canvas, &p2,
                aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                    255 >> offset));

          p2.x = p1.x - offset;
          p2.y = p1.y - offset;

          if (point_in_rect(&p2, clip_rect))
            (*canvas->set_pixel)(canvas, &p2,
                aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                    255 >> offset));
          }
        }
      while (--delta_y > 0);
      }
    else if (delta_y + delta_x != 0)   // more than 1 pixel, or a wide line
      {
      int16_t intensity_shift = 4;
      int16_t weighting_complement_mask = 0xFF;
      int16_t error_adj;                // intensity to weight color by
      int16_t error_acc = 0;
      int16_t weighting;
      int16_t error_acc_temp;
      point_t p_alias;
      //------------------------------------------------------------------------
      // determine independent variable (one that always increments by 1 (or -1) )
      // and initiate appropriate line drawing routine (based on first octant
      // always). the x and y's may be flipped if y is the independent variable.
      //------------------------------------------------------------------------
      if (delta_y > delta_x)
        {
        /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
        error_adj =
            (uint16_t) ((((int32_t) delta_x) << 16) / (int32_t) delta_y);

        while (--delta_y) // process each point in the line one at a time (just use delta_y)
          {
          error_acc_temp = error_acc;  // remember the current accumulated error
          error_acc += error_adj;             // calculate error for next pixel

          if (error_acc <= error_acc_temp)
            p1.x += x_incr;

          p1.y++;                             // increment independent variable

          // if pen width > 1 then we use a modified algorithm
          if (pen->width == 1)
            {
            weighting = error_acc >> intensity_shift;

            if (point_in_rect(&p1, clip_rect))
              (*canvas->set_pixel)(canvas, &p1,
                  aplha_blend((*canvas->get_pixel)(canvas, &p1), pen->color, (uint8_t) weighting)); // plot the pixel

            p_alias.x = p1.x + x_incr;
            p_alias.y = p1.y;

            if (point_in_rect(&p_alias, clip_rect))
              (*canvas->set_pixel)(canvas, &p_alias,
                  aplha_blend((*canvas->get_pixel)(canvas, &p_alias),
                      pen->color, (weighting ^ weighting_complement_mask)));
            }
          else
            {
            gdi_dim_t offset;
            if (point_in_rect(&p1, clip_rect))
              (*canvas->set_pixel)(canvas, &p1, pen->color);

            for (offset = 1; offset <= half_width; offset++)
              {
              point_t p2;
              copy_point(&p1, &p2);
              p2.x += offset;
              if (point_in_rect(&p2, clip_rect))
                (*canvas->set_pixel)(canvas, &p2,
                    aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                        255 >> offset));

              p2.x = p1.x;
              p2.x -= offset;
              if (point_in_rect(&p2, clip_rect))
                (*canvas->set_pixel)(canvas, &p2,
                    aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                        255 >> offset));
              }
            }
          }
        }
      else
        {
        error_adj =
            (uint16_t) ((((int32_t) delta_y) << 16) / (int32_t) delta_x);

        while (--delta_x) // process each point in the line one at a time (just use delta_y)
          {
          error_acc_temp = error_acc;  // remember the current accumulated error
          error_acc += error_adj;             // calculate error for next pixel

          if (error_acc <= error_acc_temp)
            p1.y++;

          p1.x += x_incr;                     // increment independent variable

          if (pen->width == 1)
            {
            weighting = error_acc >> intensity_shift;

            if (point_in_rect(&p1, clip_rect))
              (*canvas->set_pixel)(canvas, &p1,
                  aplha_blend((*canvas->get_pixel)(canvas, &p1), pen->color, (uint8_t) weighting)); // plot the pixel

            p_alias.x = p1.x;
            p_alias.y = p1.y + 1;

            if (point_in_rect(&p_alias, clip_rect))
              (*canvas->set_pixel)(canvas, &p_alias,
                  aplha_blend((*canvas->get_pixel)(canvas, &p_alias),
                      pen->color, (weighting ^ weighting_complement_mask)));
            }
          else
            {
            gdi_dim_t offset;
            if (point_in_rect(&p1, clip_rect))
              (*canvas->set_pixel)(canvas, &p1, pen->color);

            for (offset = 1; offset <= half_width; offset++)
              {
              point_t p2;
              copy_point(&p1, &p2);
              p2.y += offset;
              if (point_in_rect(&p2, clip_rect))
                (*canvas->set_pixel)(canvas, &p2,
                    aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                        255 >> offset));

              p2.y = p1.y;
              p2.y -= offset;

              if (point_in_rect(&p2, clip_rect))
                (*canvas->set_pixel)(canvas, &p2,
                    aplha_blend((*canvas->get_pixel)(canvas, &p2), pen->color,
                        255 >> offset));
              }
            }
          }
        }
      }
    }

  return s_ok;
  }

result_t polyline(handle_t hwnd, const rect_t *clip_rect, const pen_t *pen, uint16_t count, const point_t *points)
  {
  result_t result;
  canvas_t *canvas;
  if (failed(result = get_canvas(hwnd, &canvas)))
    return result;

  return polyline_impl(canvas, clip_rect, pen, count, points);
  }

static result_t fill_rect(canvas_t *canvas, const rect_t *clip_rect, const rect_t *rect,
    color_t color)
  {
  rect_t bounds;
  extent_t size;

  copy_rect(rect, &bounds);
  intersect_rect(clip_rect, &bounds);
  rect_extents(&bounds, &size);

// if the extents are 0 then done
  if (size.dx == 0 || size.dy == 0)
    return s_ok;

  // as the bsp to fill the rectangle as quickly as possible
  (*canvas->fast_fill)(canvas, &bounds, color);

  return s_ok;
  }

result_t ellipse(handle_t hwnd, const rect_t *clip_rect, const pen_t *pen,
    color_t fill, const rect_t *area)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  gdi_dim_t w = rect_width(area) >> 1;
  gdi_dim_t h = rect_height(area) >> 1;

  point_t c =
    {
    area->left + w, area->top + h
    };

// these are used to trace and draw the lines
  point_t q1;
  point_t q2;
  point_t q3;
  point_t q4;

// this holds the fill line
  point_t l1;
  point_t l2;

// we now use the width\height to calculate the ellipse
  gdi_dim_t a = w;
  gdi_dim_t b = h;
  gdi_dim_t x = 0;
  gdi_dim_t y = b;
  gdi_dim_t a_sqr = a * a;
  gdi_dim_t b_sqr = b * b;
  gdi_dim_t a22 = a_sqr + a_sqr;
  gdi_dim_t b22 = b_sqr + b_sqr;
  gdi_dim_t a42 = a22 + a22;
  gdi_dim_t b42 = b22 + b22;
  gdi_dim_t x_slope = b42;            // x_slope = (4 * b^^2) * (x * 1)
  gdi_dim_t y_slope = b42 * (y - 1);  // y_slope = (4 * a^^2) * (y - 1)
  gdi_dim_t mida = a_sqr >> 1;        // a^^2 / 2
  gdi_dim_t midb = b_sqr >> 1;        // b^^2 / 2
  gdi_dim_t d = a_sqr - (y_slope >> 1) - mida;  // subtract a^^2 / 2 to optimize

  bool draw_pen = pen->color != color_hollow;
  bool draw_fill = fill != color_hollow;

  q1.x = c.x;
  q1.y = area->top + (h << 1);
  q2.x = c.x;
  q2.y = area->top + (h << 1);
  q3.x = c.x;
  q3.y = area->top;
  q4.x = c.x;
  q4.y = area->top;

// region 1
  while (d <= y_slope)
    {
    // draw the ellipse point
    if (draw_pen && point_in_rect(&q1, clip_rect))
      (*canvas->set_pixel)(canvas, &q1, pen->color);

    if (!is_equal(&q1, &q2))
      {
      if (draw_pen && point_in_rect(&q2, clip_rect))
        (*canvas->set_pixel)(canvas, &q2, pen->color);

      if (draw_fill)
        {
        l1 = q1;
        l2 = q2;
        // fill the ellipse
        if (fill != ((l2.x - l1.x) > 2))
          {
          l1.x++;
          if (clip_line(&l1, &l2, clip_rect))
            (*canvas->fast_line)(canvas, &l1, &l2, fill);
          }
        }
      }

    if (!is_equal(&q1, &q3) && draw_pen && point_in_rect(&q3, clip_rect))
      (*canvas->set_pixel)(canvas, &q3, pen->color);

    if (!is_equal(&q2, &q4) && !is_equal(&q3, &q4))
      {
      if (draw_pen && point_in_rect(&q4, clip_rect))
        (*canvas->set_pixel)(canvas, &q4, pen->color);

      l1 = q3;
      l2 = q4;
      // fill the ellipse
      if (draw_fill && (l2.x - l1.x) > 2)
        {
        l1.x++;
        if (clip_line(&l1, &l2, clip_rect))
          (*canvas->fast_line)(canvas, &l1, &l2, fill);
        }
      }

    if (d > 0)
      {
      d -= y_slope;
      y--;
      q1.y--;
      q2.y--;
      q3.y++;
      q4.y++;
      y_slope -= a42;
      }

    d += b22 + x_slope;
    x++;
    q1.x--;
    q2.x++;
    q3.x--;
    q4.x++;
    x_slope += b42;
    }

  d -= ((x_slope + y_slope) >> 1) + (b_sqr - a_sqr) + (mida - midb);
// optimized region change using x_slope, y_slope
// region 2
  while (y >= 0)
    {
    // draw the ellipse point
    if (draw_pen && point_in_rect(&q1, clip_rect))
      (*canvas->set_pixel)(canvas, &q1, pen->color);

    if (!is_equal(&q1, &q2))
      {
      if (draw_pen && point_in_rect(&q2, clip_rect))
        (*canvas->set_pixel)(canvas, &q2, pen->color);

      if (draw_fill)
        {
        l1 = q1;
        l2 = q2;
        // fill the ellipse
        if (fill != ((l2.x - l1.x) > 2))
          {
          l1.x++;
          if (clip_line(&l1, &l2, clip_rect))
            (*canvas->fast_line)(canvas, &l1, &l2, fill);
          }
        }
      }

    if (!is_equal(&q1, &q3) && draw_pen && point_in_rect(&q3, clip_rect))
      (*canvas->set_pixel)(canvas, &q3, pen->color);

    if (!is_equal(&q2, &q4) && !is_equal(&q3, &q4))
      {
      if (draw_pen && point_in_rect(&q4, clip_rect))
        (*canvas->set_pixel)(canvas, &q4, pen->color);

      copy_point(&q3, &l1);
      copy_point(&q4, &l2);

      // fill the ellipse
      if (draw_fill && (l2.x - l1.x) > 2)
        {
        l1.x++;
        if (clip_line(&l1, &l2, clip_rect))
          (*canvas->fast_line)(canvas, &l1, &l2, fill);
        }
      }

    if (d <= 0)
      {
      d += x_slope;
      x++;
      q1.x--;
      q2.x++;
      q3.x--;
      q4.x++;
      x_slope += b42;
      }

    d += a22 - y_slope;
    y--;
    q1.y--;
    q2.y--;
    q3.y++;
    q4.y++;
    y_slope -= a42;
    }

  return s_ok;
  }

typedef struct _edge_t
  {
  point_t p1;
  point_t p2;

  gdi_dim_t fn;
  gdi_dim_t mn;
  gdi_dim_t d;
  } edge_t;

static void edge_swap(edge_t *left, edge_t *right)
  {
  edge_t tmp;

  memcpy(&tmp, left, sizeof(edge_t));
  memcpy(left, right, sizeof(edge_t));
  memcpy(right, &tmp, sizeof(edge_t));
  }

static int edge_cmp(const edge_t *lp, const edge_t *rp)
  {
  /* if the minimum y values are different, sort on minimum y */
  if (lp->p1.y != rp->p1.y)
    return lp->p1.y < rp->p1.y ? -1 : 1;

  /* if the current x values are different, sort on current x */
  if (lp->p1.x != rp->p1.x)
    return lp->p1.x < rp->p1.x ? -1 : 1;

  /* otherwise they are equal */
  return 0;
  }

static result_t add_edge(handle_t edge_table, edge_t *edge)
  {
  // a horizontal edge is ignored as this is handled by the end
  // and start of the previous edges
  if(edge->p1.y == edge->p2.y)
    return s_ok;

  if(edge->p1.y > edge->p2.y)
    swap_points(&edge->p1, &edge->p2);

  edge->mn = edge->p2.x - edge->p1.x;
  edge->d = edge->p2.y - edge->p1.y;
  edge->fn = edge->mn >> 1;

  return vector_push_back(edge_table, edge);
  }

result_t polypolygon_impl(canvas_t *canvas, const rect_t *clip_rect, color_t fill, uint16_t count, const uint16_t *lengths, const point_t *pts)
  {
  result_t result;
  const point_t *cp = pts;

  if(fill != color_hollow)
    {
    vector_p edge_table;

    // clip the polygon(s) and ensure the polygons are closed.
    if(failed(result = vector_create(sizeof(edge_t), &edge_table)))
      return result;

    // this is the clipped contour
    vector_p clipped_contour;
    if(failed(result = vector_create(sizeof(point_t), &clipped_contour)))
      {
      vector_close(edge_table);
      return result;
      }

    uint16_t contour_num;
    uint16_t clipped_contour_len;
    for(contour_num = 0; contour_num < count; contour_num++)
      {
      // firstly we clip the contour into a clipped form.
      if(failed(vector_clear(clipped_contour)) ||
         failed(result = polygon_intersect(clip_rect, cp, lengths[contour_num], clipped_contour)))
        {
        vector_close(edge_table);
        vector_close(clipped_contour);
        return result;
        }

      // move the source pointer
      cp += lengths[contour_num];

      if(failed(result = vector_count(clipped_contour, &clipped_contour_len)))
        {
        vector_close(clipped_contour);
        vector_close(edge_table);
        return result;
        }
#ifdef _DEBUG_POLYGON
{
point_t ip;
uint16_t i;
trace_debug("Clipped Polygon\r\n");
for(i = 0; i < clipped_contour_len; i++)
  {
  vector_at(clipped_contour, i, &ip);
  trace_debug("Point %d, x:%d, y:%d\r\n", i, ip.x, ip.y);
  }
}
#endif


      // if the polygon does not have enough points then fail.  Could be outside the
      // clipping area completely.
      if(clipped_contour_len > 2)
        {
        // get the first point, will check to make sure this edge is closed.
        edge_t edge;
        point_t first_point;
        point_t pt;

        bool last_edge = false;

        if(failed(result = vector_at(clipped_contour, 0, &first_point)))
          {
          vector_close(clipped_contour);
          vector_close(edge_table);
          return result;
          }
        edge.p1.x = first_point.x;
        edge.p1.y = first_point.y;

        uint16_t offs;
        for(offs = 1; offs < clipped_contour_len; offs++)
          {
          if(failed(result = vector_at(clipped_contour, offs, &pt)))
            {
            vector_close(clipped_contour);
            vector_close(edge_table);
            return result;
            }

          if(last_edge)
            {
            copy_point(&pt, &first_point);
            copy_point(&pt, &edge.p1);
            last_edge = false;
            }
          else
            {
            // form the end of the edge
            copy_point(&pt, &edge.p2);

            if(failed(result = add_edge(edge_table, &edge)))
              {
              vector_close(clipped_contour);
              vector_close(edge_table);
              return result;
              }

            if(is_equal(&pt, &first_point))
              last_edge = true;
            else
              copy_point(&pt, &edge.p1);
            }
          }
        }
      }

    // we are done with the contour so close it
    vector_close(clipped_contour);
    clipped_contour = 0;

    edge_t edge1;
    edge_t edge2;
    uint16_t num_edges;
    uint16_t i;

#ifdef _DEBUG_POLYGON
memory_check();
vector_count(edge_table, &num_edges);
trace_debug("Unsorted edges\r\n");
for(i = 0; i < num_edges; i++)
  {
  vector_at(edge_table, i, &edge1);
  trace_debug("%d,%d,%d,%d\r\n", edge1.p1.x, edge1.p1.y, edge1.p2.x, edge1.p2.y);
  }
#endif

    // now we sort the edges so
    if(failed(result = vector_sort(edge_table, edge_cmp, edge_swap)))
      {
      vector_close(edge_table);
      return result;
      }

    /* start with the lowest y in the table */
    vector_at(edge_table, 0, &edge1);
    gdi_dim_t y = edge1.p1.y;

    if(failed(result = vector_count(edge_table, &num_edges)))
      {
      vector_close(edge_table);
      return result;
      }

#ifdef _DEBUG_POLYGON
    memory_check();
    trace_debug("Sorted edges\r\n");
    for(i = 0; i < num_edges; i ++)
      {
      vector_at(edge_table, i, &edge1);
      trace_debug("%d,%d,%d,%d\r\n", edge1.p1.x, edge1.p1.y, edge1.p2.x, edge1.p2.y);
      }
#endif

    do
      {
      /* using odd parity, render alternating line segments */
      for(i = 1; i < num_edges; i += 2)
        {
        if(failed(result = vector_at(edge_table, i - 1, &edge1)) ||
          failed(result = vector_at(edge_table, i, &edge2)))
          {
          vector_close(edge_table);
          return result;
          }

        if(edge1.p1.y != y)
          break;              // not an active edge so we are done.

        // fill the horizontal line
        if(edge2.p1.x > edge1.p1.x)
          (*canvas->fast_line)(canvas, &edge1.p1, &edge2.p1, fill);
        else
          (*canvas->fast_line)(canvas, &edge2.p1, &edge1.p1, fill);
        }

      /* prepare for the next scan line */
      if(++y > clip_rect->bottom)
        {
        vector_close(edge_table);
        return e_unexpected;
        }

      /* remove inactive edges from the active edge table */
      /* or update the current x position of active edges */
      for(i = 0; i < num_edges; i++)
        {
        vector_at(edge_table, i, &edge1);
        if(edge1.p2.y == y)
          {
          // this is the end of an edge
          vector_erase(edge_table, i);
          num_edges--;
          i--;
          }
        else
          {
          if(edge1.p1.y != y - 1)
            break;
          else
            {
            edge1.fn += edge1.mn;
            if(edge1.fn < 0)
              {
              edge1.p1.x += edge1.fn / edge1.d - 1;
              edge1.fn %= edge1.d;
              edge1.fn += edge1.d;
              }

            if(edge1.fn >= edge1.d)
              {
              edge1.p1.x += edge1.fn / edge1.d;
              edge1.fn %= edge1.d;
              }

            edge1.p1.y = y;

            if(failed(result = vector_set(edge_table, i, &edge1)))
              {
              vector_close(edge_table);
              return e_unexpected;
              }
            }
          }
        }
      /* keep doing this while there are any edges left */
      } while(num_edges > 0);

    /* all done, free the edge tables */
    vector_close(edge_table);
    }

  return s_ok;
  }

result_t polypolygon(handle_t hwnd, const rect_t *clip_rect, const pen_t *outline, color_t fill, uint16_t count, const uint16_t *lengths, const point_t *pts)
  {
  result_t result;
  // if the interior color is hollow then just a line draw
  if (fill == color_hollow)
    {
    const point_t *cp = pts;
    uint16_t contour_num;
    for(contour_num = 0; contour_num < count; contour_num++)
      {
      if(failed(result = polyline(hwnd, clip_rect, outline, lengths[contour_num], cp)))
        return result;

     cp += lengths[contour_num];
     }

    return s_ok;
    }

  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  if (failed(result = polypolygon_impl(canvas, clip_rect, fill, count, lengths, pts)))
    return result;

  // now outline the polygon using the pen
  if (outline != 0 && outline->color != color_hollow)
    {
    uint16_t poly;
    for (poly = 0; poly < count; poly++)
      {
      uint16_t num_pts = lengths[poly];
      if (failed(result = polyline_impl(canvas, clip_rect, outline, num_pts, pts)))
        return result;

      pts += num_pts;
      }
    }

  return s_ok;
  }

result_t polygon(handle_t canvas, const rect_t *clip_rect, const pen_t *outline,
  color_t fill, uint16_t count, const point_t *pts)
  {
  return polypolygon(canvas, clip_rect, outline, fill, 1, &count, pts);
  }

result_t rectangle(handle_t hwnd, const rect_t *clip_rect, const pen_t *pen,
    color_t color, const rect_t *rect)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  gdi_dim_t rect_offet = (pen == 0 ? 0 : 1);
  rect_t rect_to_fill =
    {
        rect->left + rect_offet,
        rect->top + rect_offet,
        rect->right - rect_offet,
        rect->bottom - rect_offet
    };

  // the we fill the inside
  if ((result = fill_rect(canvas, clip_rect, &rect_to_fill, color)) != s_ok)
    return result;

  if (pen != 0)
    {
    // draw the outline
    point_t pts[5] =
      {
        {
        rect->left, rect->top
        },
        {
        rect->left, rect->bottom
        },
        {
        rect->right, rect->bottom
        },
        {
        rect->right, rect->top
        },
        {
        rect->left, rect->top
        }
      };

    return polyline(hwnd, clip_rect, pen, 5, pts);
    }

  return s_ok;
  }

result_t round_rect(handle_t hwnd, const rect_t *clip_rect, const pen_t *pen,
    color_t fill, const rect_t *rect, gdi_dim_t dim)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  // a round rect is a series of lines and arcs.
  rect_t tmp;
  // all lines are drawn on the last pixel
  rect_t draw_rect;

  copy_rect(rect, &draw_rect);
  draw_rect.right--;
  draw_rect.bottom--;

  // fill
  if (fill != color_hollow)
    {

    // top bar
    tmp.top = draw_rect.top + 1;
    tmp.left = draw_rect.left + dim;
    tmp.bottom = draw_rect.top + dim + 1;
    tmp.right = draw_rect.right - dim - 1;
    if ((result = fill_rect(canvas, clip_rect, &tmp, fill)) != s_ok)
      return result;

    tmp.top = draw_rect.bottom - dim;
    tmp.left = draw_rect.left + dim + 1;
    tmp.right = draw_rect.right + dim;
    tmp.bottom = draw_rect.bottom - dim - 1;
    if ((result = fill_rect(canvas, clip_rect, &tmp, fill)) != s_ok)
      return result;

    tmp.top = draw_rect.top + dim + 1;
    tmp.left = draw_rect.left + 1;
    tmp.right = draw_rect.right - 1;
    tmp.bottom = draw_rect.bottom - dim - 1;
    if ((result = fill_rect(canvas, clip_rect, &tmp, fill)) != s_ok)
      return result;
    }
  // lines
  point_t pts[2];

  pts[0].x = draw_rect.left + dim;
  pts[0].y = draw_rect.top;

  pts[1].x = draw_rect.right - dim;
  pts[1].y = draw_rect.top;

  if ((result = polyline(hwnd, clip_rect, pen, 2, pts)) != s_ok)   // top
    return result;

  pts[0].x = draw_rect.right;
  pts[0].y = draw_rect.top + dim;

  pts[1].x = draw_rect.right;
  pts[1].y = draw_rect.bottom - dim;

  if ((result = polyline(hwnd, clip_rect, pen, 2, pts)) != s_ok)   // right
    return result;

  pts[0].x = draw_rect.left;
  pts[0].y = draw_rect.top + dim;

  pts[1].x = draw_rect.left;
  pts[1].y = draw_rect.bottom - dim;

  if ((result = polyline(hwnd, clip_rect, pen, 2, pts)) != s_ok)   // left
    return result;

  pts[0].x = draw_rect.left + dim;
  pts[0].y = draw_rect.bottom;

  pts[1].x = draw_rect.right - dim;
  pts[1].y = draw_rect.bottom;

  if ((result = polyline(hwnd, clip_rect, pen, 2, pts)) != s_ok)   // bottom
    return result;

  // Arcs

  // top left
  // clip the elipse
  tmp.left = draw_rect.left;
  tmp.top = draw_rect.top;
  tmp.right = draw_rect.left + dim + 1;
  tmp.bottom = draw_rect.top + dim + 1;

  intersect_rect(clip_rect, &tmp);

  rect_t ellrect;
  ellrect.left = draw_rect.left;
  ellrect.top = draw_rect.top;
  ellrect.right = draw_rect.left + (dim << 1);
  ellrect.bottom = draw_rect.top + (dim << 1);

  if ((result = ellipse(hwnd, &tmp, pen, fill, &ellrect)) != s_ok)
    return result;

  // bottom left
  tmp.left = draw_rect.left;
  tmp.top = draw_rect.bottom - dim - 1;
  tmp.right = draw_rect.left + dim + 1;
  tmp.bottom = draw_rect.bottom;

  intersect_rect(clip_rect, &tmp);

  ellrect.left = draw_rect.left;
  ellrect.top = draw_rect.bottom - (dim << 1);
  ellrect.right = draw_rect.left + (dim << 1);
  ellrect.bottom = draw_rect.bottom;

  if ((result = ellipse(hwnd, &tmp, pen, fill, &ellrect)) != s_ok)
    return result;

  // top right
  tmp.left = draw_rect.right - dim - 1;
  tmp.top = draw_rect.top;
  tmp.right = draw_rect.right;
  tmp.bottom = draw_rect.top + dim + 1;

  intersect_rect(clip_rect, &tmp);

  ellrect.left = draw_rect.right - (dim << 1);
  ellrect.top = draw_rect.top;
  ellrect.right = draw_rect.right;
  ellrect.bottom = draw_rect.top + (dim << 1);

  if ((result = ellipse(hwnd, &tmp, pen, fill, &ellrect)) != s_ok)
    return result;

  // bottom right
  tmp.left = draw_rect.right - dim - 1;
  tmp.top = draw_rect.bottom - dim - 1;
  tmp.right = draw_rect.right;
  tmp.bottom = draw_rect.bottom;

  intersect_rect(clip_rect, &tmp);

  ellrect.left = draw_rect.right - (dim << 1);
  ellrect.top = draw_rect.bottom - (dim << 1);
  ellrect.right = draw_rect.right;
  ellrect.bottom = draw_rect.bottom;

  return ellipse(hwnd, &tmp, pen, fill, &ellrect);
  }

result_t bit_blt(handle_t hwnd, const rect_t *clip_rect,
    const rect_t *dest_rect, handle_t src_hwnd, const rect_t *src_clip_rect,
    const point_t *src_pt)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  canvas_t *src_canvas;
  if(failed(result = get_canvas(src_hwnd, &src_canvas)))
    return result;

  // if the src point is outside the source then done
  if (!point_in_rect(src_pt, src_clip_rect))
    return s_ok;

// if the destination is to the left of teh clip-rect then
// adjust both it and the src-rect

// determine how much to shift the left by
  gdi_dim_t dst_left =
      dest_rect->left > clip_rect->left ? dest_rect->left : clip_rect->left;
  gdi_dim_t src_left_offset =
      dst_left > dest_rect->left ? dst_left - dest_rect->left : 0;

  gdi_dim_t dst_top =
      dest_rect->top > clip_rect->top ? dest_rect->top : clip_rect->top;
  gdi_dim_t src_top_offset =
      dst_top > dest_rect->top ? dst_top - dest_rect->top : 0;

  gdi_dim_t dst_right =
      dest_rect->right < clip_rect->right ? dest_rect->right : clip_rect->right;
  gdi_dim_t src_right_offset =
      dest_rect->right < dst_right ? dst_right - dest_rect->right : 0;

  gdi_dim_t dst_bottom =
      dest_rect->bottom < clip_rect->bottom ?
          dest_rect->bottom : clip_rect->bottom;
  gdi_dim_t src_bottom_offset =
      dest_rect->bottom > dst_bottom ? dst_bottom - dest_rect->bottom : 0;

  rect_t destination =
    {
    dst_left, dst_top, dst_right, dst_bottom
    };

  extent_t dst_size =
    {
    rect_width(&destination), rect_height(&destination)
    };

  if (dst_size.dx == 0 || dst_size.dy == 0)
    return s_ok;

  rect_t source =
    {
    src_pt->x + src_left_offset, src_pt->y + src_top_offset, src_pt->x
        + rect_width(dest_rect) + src_right_offset, src_pt->y
        + rect_height(dest_rect) + src_bottom_offset
    };

  intersect_rect(src_clip_rect, &source);

  extent_t src_size =
    {
    rect_width(&source), rect_height(&source)
    };

  if (src_size.dx == 0 || src_size.dy == 0)
    return s_ok;

  point_t dst_pt =
    {
    destination.left, destination.top
    };

  (*canvas->fast_copy)(canvas, &dst_pt, src_canvas, &source);

  return s_ok;
  }

result_t get_pixel(handle_t hwnd, const rect_t *clip_rect, const point_t *pt,
    color_t *pixel)
  {
  if (!point_in_rect(pt, clip_rect))
    {
    *pixel = color_hollow;
    return s_false;
    }
  
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  *pixel = (*canvas->get_pixel)(canvas, pt);
  return s_ok;
  }

result_t set_pixel(handle_t hwnd, const rect_t *clip_rect, const point_t *pt,
    color_t c, color_t *pixel)
  {
  if (!point_in_rect(pt, clip_rect))
    {
    if(pixel != 0)
      *pixel = color_hollow;
    
    return s_false;
    }
  
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  if (pixel != 0)
    *pixel = (*canvas->get_pixel)(canvas, pt);
  (*canvas->set_pixel)(canvas, pt, c);

  return s_ok;
  }

static const uint32_t arctans[45] =
  {
      0,
      17450,
      34878,
      52264,
      69587,
      86824,
      103956,
      120961,
      137819,
      154508,
      171010,
      187303,
      203368,
      219186,
      234736,
      250000,
      264960,
      279596,
      293893,
      307831,
      321394,
      334565,
      347329,
      359670,
      371572,
      383022,
      394005,
      404508,
      414519,
      424024,
      433013,
      441474,
      449397,
      456773,
      463592,
      469846,
      475528,
      480631,
      485148,
      489074,
      492404,
      495134,
      497261,
      498782,
      499695
  };

static int lookup_atan2(double scale, gdi_dim_t y, gdi_dim_t x)
  {
  uint32_t prod = (uint32_t) ((y * scale) * (x * scale));
  int s;
  // find the offset
  if (prod < arctans[5])
    s = 0;
  else if (prod < arctans[10])
    s = 5;
  else if (prod < arctans[15])
    s = 10;
  else if (prod < arctans[20])
    s = 15;
  else if (prod < arctans[25])
    s = 20;
  else if (prod < arctans[30])
    s = 25;
  else if (prod < arctans[35])
    s = 30;
  else if (prod < arctans[40])
    s = 35;
  else
    s = 40;

  if (arctans[s + 1] > prod)
    return s;

  if (arctans[s + 2] > prod)
    return s + 1;

  if (arctans[s + 3] > prod)
    return s + 2;

  if (arctans[s + 4] > prod)
    return s + 3;

  return s + 4;
  }

static inline int fix_angle(int angle)
  {
  while (angle < 0)
    angle += 360;

  while (angle > 360)
    angle -= 360;

  return angle;
  }

result_t arc(handle_t hwnd, const rect_t *clip_rect, const pen_t *pen,
    const point_t *p, gdi_dim_t radius, int start_angle, int end_angle)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;


  // the arc routine expects 0 degrees is point(0,1) while the routine
  start_angle = fix_angle(start_angle);
  end_angle = fix_angle(end_angle);

  // normalise the drawing numbers
  if (start_angle > end_angle)
    {
    int tmp = start_angle;
    start_angle = end_angle;
    end_angle = tmp;
    }

  point_t pt =
    {
    radius, 0
    };

  gdi_dim_t decision_over2 = 1 - pt.x;

  gdi_dim_t offset = pen->width >> 1;

  extent_t rect_offset =
    {
    0 - offset, 0 - offset
    };
  extent_t wide_pen =
    {
    pen->width, pen->width
    };

  // our angles are 0..44 but our radius varies.  We need to calculate from
  // a given x and y coordinate the approximate angle.
  double scale = 1000.0f / radius;

  point_t dp;
  rect_t dr;

  while (pt.y <= pt.x)
    {
    int angle = lookup_atan2(scale, pt.y, pt.x);

    // double angle = atan2((double) pt.y, (double) pt.x);

    if (pen->width == 1)
      {
      // forward angles first
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect, make_point(pt.x + p->x, pt.y + p->y, &dp),
            pen->color, 0); // octant 1

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect, make_point(-pt.y + p->x, pt.x + p->y, &dp),
            pen->color, 0); // octant 3

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect,
            make_point(-pt.x + p->x, -pt.y + p->y, &dp), pen->color, 0); // octant 5

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect, make_point(pt.y + p->x, -pt.x + p->y, &dp),
            pen->color, 0); // octant 7

      angle -= 270;
      angle *= -1;
      angle += 90;

      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect, make_point(pt.y + p->x, pt.x + p->y, &dp),
            pen->color, 0); // octant 2

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect, make_point(-pt.x + p->x, pt.y + p->y, &dp),
            pen->color, 0); // octant 4

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect,
            make_point(-pt.y + p->x, -pt.x + p->y, &dp), pen->color, 0); // octant 6

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        set_pixel(canvas, clip_rect, make_point(pt.x + p->x, -pt.y + p->y, &dp),
            pen->color, 0); // octant 8
      }
    else
      {
      // result_t ellipse(handle_t hndl, const rect_t *clip_rect, const pen_t *pen, color_t fill, const rect_t *area)

      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(pt.x + p->x, pt.y + p->y, 
                          pt.x + p->x + wide_pen.dx,
                          pt.y + p->y + wide_pen.dy, &dr))); // octant 1

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(-pt.y + p->x, pt.x + p->y,
                          -pt.y + p->x + wide_pen.dx,
                          pt.x + p->y + wide_pen.dy, &dr))); // octant 3

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(-pt.x + p->x, -pt.y + p->y, 
                          -pt.x + p->x + wide_pen.dx,
                          -pt.y + p->y + wide_pen.dy, &dr))); // octant 5

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(pt.y + p->x, -pt.x + p->y,
                          pt.y + p->x + wide_pen.dx,
                          -pt.x + p->y + wide_pen.dy, &dr))); // octant 7

      angle -= 270;
      angle *= -1;
      angle += 90;

      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(pt.y + p->x, pt.x + p->y, 
                          pt.y + p->x + wide_pen.dx,
                          pt.x + p->y + wide_pen.dy, &dr))); // octant 2

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(-pt.x + p->x, pt.y + p->y,
                          -pt.x + p->x + wide_pen.dx,
                          pt.y + p->y + wide_pen.dy, &dr))); // octant 4

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(-pt.y + p->x, -pt.x + p->y,
                          -pt.y + p->x + wide_pen.dx,
                          -pt.x + p->y + wide_pen.dy, &dr))); // octant 6

      angle += 90;
      if (angle >= start_angle && angle <= end_angle)
        ellipse(hwnd, clip_rect, pen, pen->color,
            offset_rect(&rect_offset,
                make_rect(pt.x + p->x, -pt.y + p->y,
                          pt.x + p->x + wide_pen.dx,
                          -pt.y + p->y + wide_pen.dy, &dr))); // octant 8
      }

    pt.y++;
    if (decision_over2 <= 0)
      decision_over2 += 2 * pt.y + 1; // Change in decision criterion for y -> y+1
    else
      {
      pt.x--;
      decision_over2 += 2 * (pt.y - pt.x) + 1;  // Change for y -> y+1, x -> x-1
      }
    }

  return s_ok;
  }

result_t pie(handle_t hwnd, const rect_t *clip_rect, const pen_t *pen,
    color_t c, const point_t *p, int start, int end, gdi_dim_t radii,
    gdi_dim_t inner)
  {
  result_t result;
  canvas_t *canvas;
  if(failed(result = get_canvas(hwnd, &canvas)))
    return result;

  if (end < start)
    {
    int t = start;
    start = end;
    end = t;
    }

// special case for start == end
  if (start == end)
    {
    point_t pts[2] =
      {
        {
        p->x + inner, p->y
        },
        {
        p->x + radii, p->y
        }
      };

    rotate_point(p, &pts[0], start);
    rotate_point(p, &pts[1], start);

    return polyline(canvas, clip_rect, pen, 2, pts);
    }
// we draw a polygon using the current pen
// and brush.  We first need to calculate the outer
// point count

  double outer_incr = atan2(1.0, radii);
  double delta = end - start;

  delta = (delta < 0) ? -delta : delta;

  gdi_dim_t segs = (gdi_dim_t) (delta / outer_incr) + 1;

  double inner_incr;

// see if we have an inner radii
  if (inner > 0)
    {
    inner_incr = atan2(1.0, inner);
    segs += (gdi_dim_t) (delta / inner_incr) + 1;
    }
  else
    segs++;

// We allocate 1 more point so that the polyline will fill the area.
  point_t *pts = neutron_malloc(sizeof(point_t) * (segs + 1));

  int pt = 1;
  copy_point(p, pts);
  int theta;
  for (theta = start; theta <= end; theta += (int) floor(outer_incr))
    {
    pts[pt].x = p->x + radii;
    pts[pt].y = p->y;
    rotate_point(p, pts + pt, theta);
    pt++;
    }

  if (inner > 0)
    {
    for (theta = end; theta >= start; theta -= (int) floor(inner_incr))
      {
      pts[pt].x = p->x + inner;
      pts[pt].y = p->y;
      rotate_point(p, pts + pt, theta);
      pt++;
      }
    }
  else
    copy_point(p, pts + pt);

  polygon(canvas, clip_rect, pen, c, segs + 1, pts);

  neutron_free(pts);

  return s_ok;
  }
/////////////////////////////////////////////////////////////////////////
//
// This is the double buffered framebuffer draw code.

typedef enum
  {
  fbds_idle,
  fbds_in_paint,
  fbds_painting,
  fbds_painted,
  fbds_in_sync,
  fbds_in_sync_need_paint
  } framebuffer_draw_state;

static framebuffer_draw_state state = fbds_idle;

static int paint_depth = 0;

// these are the fbds state machine transitions
// a bsp_sync is detected
result_t bsp_sync()
  {
  if(state == fbds_painted && paint_depth == 0)
    {
    state = fbds_in_sync;     // we are syncing the framebuffer now
    bsp_sync_framebuffer();
    }

  return s_ok;
  }

result_t bsp_queue_empty(handle_t hwnd)       // the queue is empty
  {
  result_t result = s_false;
  enter_critical();
  switch(state)
    {
    case fbds_idle :
      state = fbds_in_paint;        // the GDI is painting so don't update video buffer
      result = s_ok;                  // send a paint message
      break;
    case fbds_in_paint :
      state = fbds_painting;
      result = s_ok;
      break;
    case fbds_painting :
      break;
    case fbds_painted :
      state = fbds_in_paint;
      result = s_ok;
      break;
    case fbds_in_sync :
      state = fbds_in_sync_need_paint;
      break;
    case fbds_in_sync_need_paint :
      state = fbds_in_paint;
      exit_critical();
      // we post a paint message to the root window
      return post_message(0, &paint_msg, INDEFINITE_WAIT);
    }

  exit_critical();

  return result;
  }

result_t bsp_begin_paint(handle_t hwnd)       // the application is starting to paint
  {
  paint_depth++;
  if(state == fbds_in_sync_need_paint)
    post_message(0, &paint_msg, INDEFINITE_WAIT);

  state = fbds_painting;

  return s_ok;
  }

result_t bsp_end_paint(handle_t hwnd)         // the application has finished painting
  {
  if(--paint_depth == 0)
    state = fbds_painted;          // wait for vsync

  return s_ok;
  }

result_t bsp_sync_done()
  {
  switch(state)
    {
    case fbds_idle :
      break;
    case fbds_in_paint :
      break;
    case fbds_painting :
      break;
    case fbds_painted :
      break;
    case fbds_in_sync :
      state = fbds_idle;
      break;
    case fbds_in_sync_need_paint :
      post_message(0, &paint_msg, INDEFINITE_WAIT);
      state = fbds_in_paint;
      break;
    }

  return s_ok;
  }
