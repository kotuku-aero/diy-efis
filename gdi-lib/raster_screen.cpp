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
#include "raster_screen.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <limits>
#include <math.h>

#include "spatial.h"
#include "assert.h"
#include "point.h"

typedef int out_code_t;

static const int INSIDE = 0; // 0000
static const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

namespace kotuku {
// Compute the bit code for a point (x, y) using the clip rectangle
// bounded diagonally by (xmin, ymin), and (xmax, ymax)

// ASSUME THAT xmax, xmin, ymax and ymin are global constants.

out_code_t compute_out_code(const point_t &pt,
    const rect_t &rect)
  {
  out_code_t code;

  code = INSIDE;          // initialised as being inside of clip window

  if(pt.x < rect.left)           // to the left of clip window
    code |= LEFT;
  else if(pt.x > rect.right)      // to the right of clip window
    code |= RIGHT;
  if(pt.y < rect.top)           // below the clip window
    code |= BOTTOM;
  else if(pt.y > rect.bottom)      // above the clip window
    code |= TOP;

  return code;
  }

// clip a line to within the clipping rectangle.  Return true if part of line is within the rectangle
static bool clip_line(point_t &p1, point_t &p2,
    const rect_t &clip_rect)
  {
  // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
  out_code_t outcode0 = compute_out_code(p1, clip_rect);
  out_code_t outcode1 = compute_out_code(p2, clip_rect);

  while(true)
    {
    if(!(outcode0 | outcode1))
      { // Bitwise OR is 0. Trivially accept and get out of loop
      return true;        // line inside rectange
      }
    else if(outcode0 & outcode1)
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
      if(outcodeOut & TOP)
        {           // point is above the clip rectangle
        x = p1.x + (p2.x - p1.x) * (clip_rect.bottom - p1.y) / (p2.y - p1.y);
        y = clip_rect.bottom;
        }
      else if(outcodeOut & BOTTOM)
        { // point is below the clip rectangle
        x = p1.x + (p2.x - p1.x) * (clip_rect.top - p1.y) / (p2.y - p1.y);
        y = clip_rect.top;
        }
      else if(outcodeOut & RIGHT)
        {  // point is to the right of clip rectangle
        y = p1.y + (p2.y - p1.y) * (clip_rect.right - p1.x) / (p2.x - p1.x);
        x = clip_rect.right;
        }
      else if(outcodeOut & LEFT)
        {   // point is to the left of clip rectangle
        y = p1.y + (p2.y - p1.y) * (clip_rect.left - p1.x) / (p2.x - p1.x);
        x = clip_rect.left;
        }

      // Now we move outside point to intersection point to clip
      // and get ready for next pass.
      if(outcodeOut == outcode0)
        {
        p1.x = x;
        p1.y = y;
        outcode0 = compute_out_code(p1, clip_rect);
        }
      else
        {
        p2.x = x;
        p2.y = y;
        outcode1 = compute_out_code(p2, clip_rect);
        }
      }
    }
  }

inline color_t alpha_blend(color_t pixel, color_t back,
    uint8_t weighting)
  {
  if(weighting == 0)
    return back;

  if(weighting == 255)
    return pixel;

  /* alpha blending the source and background colors */
  uint32_t rb = (((pixel & 0x00ff00ff) * weighting)
      + ((back & 0x00ff00ff) * (0xff - weighting))) & 0xff00ff00;
  uint32_t g = (((pixel & 0x0000ff00) * weighting)
      + ((back & 0x0000ff00) * (0xff - weighting))) & 0x00ff0000;
  return (pixel & 0xff000000) | ((rb | g) >> 8);
  }

typedef std::vector<kotuku::point_t> points_t;
typedef std::vector<std::pair<kotuku::point_t, bool> > labeled_points_t;

static bool intersect_line(const point_t &p1, const point_t &p2, // line 1
    const point_t &p3, const point_t &p4,    // line 2
    point_t &ip)
  {
  // if lines do not intersect then exit
  gdi_dim_t d2 = (p1.x - p2.x) * (p3.y - p1.y)
      - (p1.y - p2.y) * (p3.x - p1.x);
  gdi_dim_t d3 = (p1.x - p2.x) * (p4.y - p1.y)
      - (p1.y - p2.y) * (p4.x - p1.x);

  if((d2 < 0 && d3 < 0) || (d2 > 0 && d3 > 0))
    return false;

  d2 = (p3.x - p4.x) * (p1.y - p3.y) - (p3.y - p4.y) * (p1.x - p3.x);
  d3 = (p3.x - p4.x) * (p2.y - p3.y) - (p3.y - p4.y) * (p2.x - p3.x);

  if((d2 < 0 && d3 < 0) || (d2 > 0 && d3 > 0))
    return false;

  gdi_dim_t d = (p1.x - p2.x) * (p3.y - p4.y)
      - (p1.y - p2.y) * (p3.x - p4.x);
  if(d == 0)
    return false;

  ip.x = ((p3.x - p4.x) * ((p1.x * p2.y) - (p1.y * p2.x))
      - (p1.x - p2.x) * ((p3.x * p4.y) - (p3.y * p4.x))) / d;
  ip.y = ((p3.y - p4.y) * ((p1.x * p2.y) - (p1.y * p2.x))
      - (p1.y - p2.y) * ((p3.x * p4.y) - (p3.y * p4.x))) / d;

  return true;
  }

inline gdi_dim_t distance(const point_t &p1,
    const point_t &p2)
  {
  return ((p2.x - p1.x) * (p2.x - p1.x)) + ((p2.y - p1.y) * (p2.y - p1.y));
  }

// is_left(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
inline int is_left(const point_t &p0, const point_t &p1,
    const point_t &p2)
  {
  return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
  }

// point_in_polygon(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only when P is outside)
bool point_in_polygon(const point_t &pt,
    const labeled_points_t &subject)
  {
  int wn = 0;    // the  winding number counter

  // loop through all edges of the polygon
  for(int i = 0; i < subject.size() - 1; i++)
    {   // edge from V[i] to  V[i+1]
    if(subject[i].first.y <= pt.y)
      {          // start y <= P.y
      if(subject[i+1].first.y > pt.y)      // an upward crossing
        if(is_left(subject[i].first, subject[i + 1].first, pt) > 0) // P left of  edge
          ++wn;            // have  a valid up intersect
      }
    else
      {                        // start y > P.y (no test needed)
      if(subject[i + 1].first.y <= pt.y)     // a downward crossing
        if(is_left(subject[i].first, subject[i + 1].first, pt) < 0) // P right of  edge
          --wn;            // have  a valid down intersect
      }
    }

  return wn > 0;
  }

static void polygon_intersect(const kotuku::rect_t &clip_rect,
    const kotuku::point_t *pts, size_t count, points_t &points)
  {
  points.clear();
  labeled_points_t subject_points;

  // create a list of points and mark them as inside or outside the clipping rectangle
  for(size_t pt = 0; pt < count; pt++)
    subject_points.push_back(
        std::make_pair(kotuku::point_t(pts[pt]), (clip_rect && pts[pt])));

  // ensure is a closed polygon

  points_t clip_points;
  clip_points.push_back(clip_rect.top_left());
  clip_points.push_back(clip_rect.top_right());
  clip_points.push_back(clip_rect.bottom_right());
  clip_points.push_back(clip_rect.bottom_left());
  clip_points.push_back(clip_rect.top_left());

  kotuku::point_t ip;
  size_t sp;
  size_t cp;
  int num_intersections = 0;
  int num_inside = 0;

  for(sp = 0; sp < subject_points.size() - 1; sp++)
    {
    kotuku::point_t line_start = subject_points[sp].first;

    std::pair<kotuku::point_t, size_t> intersections[2];
    size_t intersection = 0;

    if(clip_rect && line_start)
      num_inside++;

    // scan the clipping rectangle
    for(cp = 0; cp < clip_points.size() - 1; cp++)
      {
      // an arc on the polygon can intersect at most 2 edges however we need to know the closest
      // to the arc start.
      if(intersect_line(line_start, subject_points[sp + 1].first,
          clip_points[cp], clip_points[cp + 1], ip))
        {
        // when we have added an intersection we will find it again
        if(ip == line_start)
          continue;

        assert(intersection < 2);
        intersections[intersection].first = ip;
        intersections[intersection].second = cp;
        intersection++;
        }
      }

    if(intersection > 1)
      {
      // two intersections found for this line
      // make the first one the furthest distance
      if(distance(line_start, intersections[0].first)
          < distance(line_start, intersections[1].first))
        std::swap(intersections[0], intersections[1]);
      }

    // intersections contains the ordered list of subject points
    while(intersection--)
      {
      ip = intersections[intersection].first;
      cp = intersections[intersection].second;
      // check for case when points are on the clipping area
      if(!(ip == line_start) && !(ip == subject_points[sp + 1].first))
        {
        // we have a clipped line so add it to the result.
        subject_points.insert(subject_points.begin() + sp + 1,
            std::make_pair(ip, true));
        clip_points.insert(clip_points.begin() + cp + 1, ip);
        if(intersection > 0)
          {
          sp++;     // only skip if there are 2 intersections
          if(intersections[0].second > cp)
            intersections[0].second++; // adjust for inserted intersection before edge start
          }
        num_intersections++;
        }
      }
    }

  if(num_intersections == 0)
    {
    if(num_inside > 0)
      points.assign(pts, pts + count);
    else
      {
      // we need to see if the clip rect is inside the subject
      for(cp = 0; cp < clip_points.size() - 1; cp++)
        {
        if(point_in_polygon(clip_points[cp], subject_points))
          {
          points.assign(clip_points.begin(), clip_points.end());
          return;
          }
        }
      }
    return;
    }

  // now we look for the first point on the subject that is inside, last point is first so ignore
  for(sp = 0; sp < subject_points.size() - 1; sp++)
    if(subject_points[sp].second)
      {
      points.push_back(subject_points[sp].first);
      sp++;
      break;
      }

  if(points.empty())
    return;             // no result as all outside

  ip = points[0];       // point just inserted
  // walk the list
  do
    {
    if(subject_points[sp].second)
      {
      // next point is clipped point
      ip = subject_points[sp].first;
      }
    else
      {
      // find the point in the clip_points
      for(cp = 0; cp < clip_points.size() - 1; cp++)
        if(clip_points[cp] == ip)
          break;                  // found last point inserted

      cp++;           // skip next on clipping rectangle
      if(cp == clip_points.size() - 1)
        cp = 0;

      do
        {
        ip = clip_points[cp];

        bool is_clipped = false;
        // see if the next point is on the subject
        for(sp = 0; sp < subject_points.size() - 1; sp++)
          if(subject_points[sp].first == ip)
            {
            is_clipped = true;
            break;
            }

        if(is_clipped)
          break;

        // point is part of the clipping rectangle.
        points.push_back(ip);
        cp++;

        // wrap around to the start till we find an intersection point
        if(cp == clip_points.size() - 1)
          cp = 0;
        }
      while(cp < clip_points.size() - 1);
      }

    points.push_back(ip);
    sp++;
    }
  while(!(ip == points[0]));
  }
};

kotuku::raster_screen_t::~raster_screen_t()
  {
  }

kotuku::raster_screen_t::raster_screen_t(const screen_metrics_t &metrics) :
    screen_metrics_t(metrics)
  {
  screen_x = metrics.screen_x;
  screen_y = metrics.screen_y;
  bits_per_pixel = metrics.bits_per_pixel;
  row_size = metrics.row_size;
  palette = 0;
  }

const kotuku::screen_metrics_t *kotuku::raster_screen_t::screen_metrics() const
  {
  return this;
  }

void kotuku::raster_screen_t::invalidate_rect(const rect_t &rect)
  {
  // do nothing on a framebuffer as it is always redrawn
  }

void kotuku::raster_screen_t::polyline(const rect_t &clip_rect,
    const pen_t *pen, const point_t *points, size_t count)
  {
  gdi_dim_t half_width = pen->width >> 1;

  for(size_t pt = 0; (pt + 1) < count; pt++)
    {
    point_t p1 = points[pt];
    point_t p2 = points[pt + 1];

    // clip the line to the clipping area
    if(!clip_line(p1, p2, clip_rect))
      continue;                 // line is outside the clipping area

    // ensure the line is always top->bottom
    if(p1.y > p2.y)
      std::swap(p1, p2);

    // draw the first pixel
    set_pixel(point_to_address(p1), pen->color);

    gdi_dim_t delta_x = p2.x - p1.x;
    gdi_dim_t delta_y = p2.y - p1.y;

    gdi_dim_t x_incr;
    if(delta_x >= 0)
      x_incr = 1;
    else
      {
      x_incr = -1;
      delta_x = -delta_x;
      }

    // we can optimize the drawing of horizontal and vertical lines
    if(delta_x == 0)
      {
      if(p1.y > p2.y)
        std::swap(p1, p2);

      while(p1.y < p2.y)
        {
        set_pixel(point_to_address(p1), pen->color);

        for(gdi_dim_t offset = 1; offset <= half_width; offset++)
          {
          uint8_t *alpha_pt;
          alpha_pt = point_to_address(p1 + extent_t(offset, 0));
          set_pixel(alpha_pt,
              alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));

          alpha_pt = point_to_address(p1 + extent_t(-offset, 0));
          set_pixel(alpha_pt,
              alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));
          }
        p1.y++;
        }
      }
    else if(delta_y == 0)
      {
      if(p1.x > p2.x)
        std::swap(p1, p2);

      while(p1.x < p2.x)
        {
        set_pixel(point_to_address(p1), pen->color);

        for(gdi_dim_t offset = 1; offset <= half_width; offset++)
          {
          uint8_t *alpha_pt;
          alpha_pt = point_to_address(p1 + extent_t(0, offset));
          set_pixel(alpha_pt,
              alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));

          alpha_pt = point_to_address(p1 + extent_t(0, -offset));
          set_pixel(alpha_pt,
              alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));
          }
        p1.x++;
        }
      }
    else if(delta_y == delta_x)
      {
      do
        {
        set_pixel(point_to_address(p1), pen->color);
        p1.x += x_incr;
        p1.y++;

        for(gdi_dim_t offset = 1; offset <= half_width; offset++)
          {
          uint8_t *alpha_pt;
          alpha_pt = point_to_address(p1 + extent_t(offset, offset));
          set_pixel(alpha_pt,
              alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));

          alpha_pt = point_to_address(p1 + extent_t(-offset, -offset));
          set_pixel(alpha_pt,
              alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));
          }
        }
      while(--delta_y > 0);
      }
    else if(delta_y + delta_x != 0)   // more than 1 pixel, or a wide line
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
      if(delta_y > delta_x)
        {
        /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
        error_adj =
            (uint16_t) ((((int32_t) delta_x) << 16) / (int32_t) delta_y);

        while(--delta_y) // process each point in the line one at a time (just use delta_y)
          {
          error_acc_temp = error_acc;  // remember the current accumulated error
          error_acc += error_adj;             // calculate error for next pixel

          if(error_acc <= error_acc_temp)
            p1.x += x_incr;

          p1.y++;										          // increment independent variable

          // if pen width > 1 then we use a modified algorithm
          if(pen->width == 1)
            {
            weighting = error_acc >> intensity_shift;

            uint8_t *alpha_pt = point_to_address(p1);

            set_pixel(alpha_pt,
                alpha_blend(get_pixel(alpha_pt), pen->color, weighting));	// plot the pixel

            p_alias.x = p1.x + x_incr;
            p_alias.y = p1.y;

            alpha_pt = point_to_address(p_alias);
            set_pixel(alpha_pt,
                alpha_blend(get_pixel(alpha_pt), pen->color,
                    (weighting ^ weighting_complement_mask)));
            }
          else
            {
            uint8_t *alpha_pt = point_to_address(p1);
            set_pixel(alpha_pt, pen->color);

            for(gdi_dim_t offset = 1; offset <= half_width; offset++)
              {
              alpha_pt = point_to_address(p1 + extent_t(offset, 0));
              set_pixel(alpha_pt,
                  alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));

              alpha_pt = point_to_address(p1 + extent_t(-offset, 0));
              set_pixel(alpha_pt,
                  alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));
              }
            }
          }
        }
      else
        {
        error_adj =
            (uint16_t) ((((int32_t) delta_y) << 16) / (int32_t) delta_x);

        while(--delta_x)// process each point in the line one at a time (just use delta_y)
          {
          error_acc_temp = error_acc;  // remember the current accumulated error
          error_acc += error_adj;             // calculate error for next pixel

          if(error_acc <= error_acc_temp)
            p1.y++;

          p1.x += x_incr;										  // increment independent variable

          if(pen->width == 1)
            {
            weighting = error_acc >> intensity_shift;

            uint8_t *alpha_pt = point_to_address(p1);
            set_pixel(alpha_pt,
                alpha_blend(get_pixel(alpha_pt), pen->color, weighting));	// plot the pixel

            p_alias.x = p1.x;
            p_alias.y = p1.y + 1;

            alpha_pt = point_to_address(p_alias);
            set_pixel(alpha_pt,
                alpha_blend(get_pixel(alpha_pt), pen->color,
                    (weighting ^ weighting_complement_mask)));
            }
          else
            {
            uint8_t *alpha_pt = point_to_address(p1);
            *alpha_pt = pen->color;

            for(gdi_dim_t offset = 1; offset <= half_width; offset++)
              {
              alpha_pt = point_to_address(p1 + extent_t(0, offset));
              set_pixel(alpha_pt,
                  alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));

              alpha_pt = point_to_address(p1 + extent_t(0, -offset));
              set_pixel(alpha_pt,
                  alpha_blend(get_pixel(alpha_pt), pen->color, 255 >> offset));
              }
            }
          }
        }
      }
    }
  }

void kotuku::raster_screen_t::fill_rect(const rect_t &clip_rect,
    const rect_t &rect, color_t color)
  {
// calculate how large the actual rectangle is
  rect_t bounds = rect;
  bounds &= clip_rect;
  extent_t size = bounds.extents();

// if the extents are 0 then done
  if(size.dx == 0 || size.dy == 0)
    return;

  // we try to fill using the fastest mechanism possible.
  // for this we use an optimised draw routine
  point_t row_start = bounds.top_left();
  for(gdi_dim_t row = 0; row < size.dy; row++)
    {
    fast_fill(point_to_address(row_start), size.dx, color);
    row_start.y++;
    }
  }

void kotuku::raster_screen_t::ellipse(const rect_t &clip_rect,
    const pen_t *pen, color_t fill, const rect_t &points)
  {
  gdi_dim_t w = points.width() >> 1;
  gdi_dim_t h = points.height() >> 1;

  point_t c(points.left + w, points.top + h);

// these are used to trace and draw the lines
  point_t q1(c.x, points.top + (h << 1));
  point_t q2(c.x, points.top + (h << 1));
  point_t q3(c.x, points.top);
  point_t q4(c.x, points.top);

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
  gdi_dim_t x_slope = b42;						// x_slope = (4 * b^^2) * (x * 1)
  gdi_dim_t y_slope = b42 * (y - 1);	// y_slope = (4 * a^^2) * (y - 1)
  gdi_dim_t mida = a_sqr >> 1;				// a^^2 / 2
  gdi_dim_t midb = b_sqr >> 1;				// b^^2 / 2
  gdi_dim_t d = a_sqr - (y_slope >> 1) - mida;	// subtract a^^2 / 2 to optimize

  bool draw_pen = pen->color != color_hollow;
  bool draw_fill = fill != color_hollow;

// region 1
  while(d <= y_slope)
    {
    // draw the ellipse point
    if(draw_pen && (clip_rect && q1))
      set_pixel(point_to_address(q1), pen->color);

    if(!(q1 == q2))
      {
      if(draw_pen && (clip_rect && q2))
        set_pixel(point_to_address(q2), pen->color);

      if(draw_fill)
        {
        l1 = q1;
        l2 = q2;
        // fill the ellipse
        if(fill != (l2.x - l1.x) > 2)
          {
          l1.x++;
          if(clip_line(l1, l2, clip_rect))
            fast_fill(point_to_address(l1), l2.x - l1.x, fill);
          }
        }
      }

    if(!(q1 == q3) && draw_pen && (clip_rect && q3))
      set_pixel(point_to_address(q3), pen->color);

    if(!(q2 == q4) && !(q3 == q4))
      {
      if(draw_pen && (clip_rect && q4))
        set_pixel(point_to_address(q4), pen->color);

      l1 = q3;
      l2 = q4;
      // fill the ellipse
      if(draw_fill && (l2.x - l1.x) > 2)
        {
        l1.x++;
        if(clip_line(l1, l2, clip_rect))
          fast_fill(point_to_address(l1), l2.x - l1.x, fill);
        }
      }

    if(d > 0)
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
  while(y >= 0)
    {
    // draw the ellipse point
    if(draw_pen && (clip_rect && q1))
      set_pixel(point_to_address(q1), pen->color);

    if(!(q1 == q2))
      {
      if(draw_pen && (clip_rect && q2))
        set_pixel(point_to_address(q2), pen->color);

      if(draw_fill)
        {
        l1 = q1;
        l2 = q2;
        // fill the ellipse
        if(fill != (l2.x - l1.x) > 2)
          {
          l1.x++;
          if(clip_line(l1, l2, clip_rect))
            fast_fill(point_to_address(l1), l2.x - l1.x, fill);
          }
        }
      }

    if(!(q1 == q3) && draw_pen && (clip_rect && q3))
      set_pixel(point_to_address(q3), pen->color);

    if(!(q2 == q4) && !(q3 == q4))
      {
      if(draw_pen && (clip_rect && q4))
        set_pixel(point_to_address(q4), pen->color);

      l1 = q3;
      l2 = q4;
      // fill the ellipse
      if(draw_fill && (l2.x - l1.x) > 2)
        {
        l1.x++;
        if(clip_line(l1, l2, clip_rect))
          fast_fill(point_to_address(l1), l2.x - l1.x, fill);
        }
      }

    if(d <= 0)
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
  }

inline double round(double d)
  {
  return floor(d + 0.5);
  }

struct edge_t
  {
  gdi_dim_t x1, y1, x2, y2;
  gdi_dim_t cx, fn, mn, d;
  };

inline bool edge_cmp(const edge_t &lp, const edge_t &rp)
  {
  /* if the minimum y values are different, sort on minimum y */
  if(lp.y1 != rp.y1)
    return lp.y1 < rp.y1;

  /* if the current x values are different, sort on current x */
  if(lp.cx != rp.cx)
    return lp.cx < rp.cx;

  /* otherwise they are equal */
  return false;
  }

void kotuku::raster_screen_t::polygon(const rect_t &clip_rect,
    const pen_t *outline, color_t fill, const point_t *pts,
    size_t count, bool interior_fill)
  {
// if the interior color is hollow then just a line draw
  if(fill == color_hollow)
    {
    polyline(clip_rect, outline, pts, count);
    return;
    }

  pen_t fill_pen =
    {
    fill, 1, ps_solid
    };

// clip the polygon
  points_t points;
  polygon_intersect(clip_rect, pts, count, points);

  // ignore small polygons
  if(points.size() < 3)
    return;

  edge_t *get; /* global edge table */
  int nge = 0; /* num global edges */
  int cge = 0; /* cur global edge */

  edge_t *aet; /* active edge table */
  int nae = 0; /* num active edges */

  int i, y;

  get = new edge_t[points.size()];
  aet = new edge_t[points.size()];

  /* setup the global edge table */
  for(i = 0; i < count; ++i)
    {
    get[nge].x1 = points[i].x;
    get[nge].y1 = points[i].y;
    get[nge].x2 = points[(i + 1) % points.size()].x;
    get[nge].y2 = points[(i + 1) % points.size()].y;

    if(get[nge].y1 != get[nge].y2)
      {
      if(get[nge].y1 > get[nge].y2)
        {
        std::swap(get[nge].x1, get[nge].x2);
        std::swap(get[nge].y1, get[nge].y2);
        }
      get[nge].cx = get[nge].x1;
      get[nge].mn = get[nge].x2 - get[nge].x1;
      get[nge].d = get[nge].y2 - get[nge].y1;
      get[nge].fn = get[nge].mn / 2;
      ++nge;
      }
    }

  std::sort(get, get + nge, edge_cmp);

  /* start with the lowest y in the table */
  y = get[0].y1;

  do
    {

    /* add edges to the active table from the global table */
    while((nge > 0) && (get[cge].y1 == y))
      {
      aet[nae] = get[cge++];
      --nge;
      aet[nae++].y1 = 0;
      }

    std::sort(aet, aet + nae, edge_cmp);

    /* using odd parity, render alternating line segments */
    for(i = 1; i < nae; i += 2)
      {
      int l = (int) aet[i - 1].cx;
      int r = (int) aet[i].cx;

      if(r > l)
        fast_fill(point_to_address(point_t(l, y)), r - l, fill); // draw line between l and r and not between l and (r-1)
      }

    /* prepare for the next scan line */
    ++y;

    /* remove inactive edges from the active edge table */
    /* or update the current x position of active edges */
    for(i = 0; i < nae; ++i)
      {
      if(aet[i].y2 == y)
        aet[i--] = aet[--nae];
      else
        {
        aet[i].fn += aet[i].mn;
        if(aet[i].fn < 0)
          {
          aet[i].cx += aet[i].fn / aet[i].d - 1;
          aet[i].fn %= aet[i].d;
          aet[i].fn += aet[i].d;
          }

        if(aet[i].fn >= aet[i].d)
          {
          aet[i].cx += aet[i].fn / aet[i].d;
          aet[i].fn %= aet[i].d;
          }
        }
      }
    /* keep doing this while there are any edges left */
    }
  while((nae > 0) || (nge > 0));

  /* all done, free the edge tables */
  delete[] get;
  delete[] aet;

  // now outline the polygon using the pen
  if(outline != 0 && outline->color != color_hollow)
    polyline(clip_rect, outline, pts, count);
//tracer.measure("Polygon outline drawn");
  }

void kotuku::raster_screen_t::rectangle(const rect_t &clip_rect,
    const pen_t *outline, color_t fill, const rect_t &rect)
  {
// the we fill the inside
  fill_rect(clip_rect,
      rect_t(rect.left + 1, rect.top - 1, rect.right - 1,
          rect.bottom + 1), fill);
// draw the outline
  point_t pts[5] =
    {
        rect.top_left(),
        rect.bottom_left(),
        rect.bottom_right(),
        rect.top_right(),
        rect.top_left()
    };

  polyline(clip_rect, outline, pts, 5);
  }

void kotuku::raster_screen_t::round_rect(const rect_t &clip_rect,
    const pen_t *pen, color_t fill, const rect_t &rect,
    const extent_t &dim)
  {
  // a round rect is a series of lines and arcs.
  rect_t tmp;

  // all lines are drawn on the last pixel
  rect_t draw_rect = rect;
  draw_rect.right--;
  draw_rect.bottom--;

  // fill
  if(fill != color_hollow)
    {

    // top bar
    tmp.top = draw_rect.top + 1;
    tmp.left = draw_rect.left + dim.dx;
    tmp.bottom = draw_rect.top + dim.dy + 1;
    tmp.right = draw_rect.right - dim.dx - 1;
    fill_rect(clip_rect, tmp, fill);

    tmp.top = draw_rect.bottom - dim.dy;
    tmp.left = draw_rect.left + dim.dx + 1;
    tmp.right = draw_rect.right + dim.dy;
    tmp.bottom = draw_rect.bottom - dim.dy - 1;
    fill_rect(clip_rect, tmp, fill);

    tmp.top = draw_rect.top + dim.dy + 1;
    tmp.left = draw_rect.left + 1;
    tmp.right = draw_rect.right - 1;
    tmp.bottom = draw_rect.bottom - dim.dy - 1;
    fill_rect(clip_rect, tmp, fill);
    }
  // lines
  point_t pts[2];

  pts[0] = draw_rect.top_left();
  pts[1] = draw_rect.top_right();
  pts[0].x += dim.dx;
  pts[1].x -= dim.dx;
  polyline(clip_rect, pen, pts, 2);   // top

  pts[0] = draw_rect.top_right();
  pts[1] = draw_rect.bottom_right();
  pts[0].y += dim.dy;
  pts[1].y -= dim.dy;
  polyline(clip_rect, pen, pts, 2);   // right

  pts[0] = draw_rect.top_left();
  pts[1] = draw_rect.bottom_left();
  pts[0].y += dim.dy;
  pts[1].y -= dim.dy;
  polyline(clip_rect, pen, pts, 2);   // left

  pts[0] = draw_rect.bottom_left();
  pts[1] = draw_rect.bottom_right();
  pts[0].x += dim.dx;
  pts[1].x -= dim.dx;
  polyline(clip_rect, pen, pts, 2);   // bottom

  // Arcs

  // top left
  // clip the elipse
  tmp.left = draw_rect.left;
  tmp.top = draw_rect.top;
  tmp.right = draw_rect.left + dim.dx + 1;
  tmp.bottom = draw_rect.top + dim.dy + 1;

  tmp &= clip_rect;

  rect_t ellrect;
  ellrect.left = draw_rect.left;
  ellrect.top = draw_rect.top;
  ellrect.right = draw_rect.left + (dim.dx << 1);
  ellrect.bottom = draw_rect.top + (dim.dy << 1);

  ellipse(tmp, pen, fill, ellrect);

  // bottom left
  tmp.left = draw_rect.left;
  tmp.top = draw_rect.bottom - dim.dy - 1;
  tmp.right = draw_rect.left + dim.dx + 1;
  tmp.bottom = draw_rect.bottom;

  tmp &= clip_rect;

  ellrect.left = draw_rect.left;
  ellrect.top = draw_rect.bottom - (dim.dy << 1);
  ellrect.right = draw_rect.left + (dim.dx << 1);
  ellrect.bottom = draw_rect.bottom;

  ellipse(tmp, pen, fill, ellrect);

  // top right
  tmp.left = draw_rect.right - dim.dx - 1;
  tmp.top = draw_rect.top;
  tmp.right = draw_rect.right;
  tmp.bottom = draw_rect.top + dim.dy + 1;

  tmp &= clip_rect;

  ellrect.left = draw_rect.right - (dim.dx << 1);
  ellrect.top = draw_rect.top;
  ellrect.right = draw_rect.right;
  ellrect.bottom = draw_rect.top + (dim.dy << 1);

  ellipse(tmp, pen, fill, ellrect);

  // bottom right
  tmp.left = draw_rect.right - dim.dx - 1;
  tmp.top = draw_rect.bottom - dim.dy - 1;
  tmp.right = draw_rect.right;
  tmp.bottom = draw_rect.bottom;

  tmp &= clip_rect;

  ellrect.left = draw_rect.right - (dim.dx << 1);
  ellrect.top = draw_rect.bottom - (dim.dy << 1);
  ellrect.right = draw_rect.right;
  ellrect.bottom = draw_rect.bottom;

  ellipse(tmp, pen, fill, ellrect);
  }

void kotuku::raster_screen_t::pattern_blt(const rect_t &clip_rect,
    const bitmap_t *src, const rect_t &dest, raster_operation mode)
  {
  // copy the bitmap to the screen
  rect_t src_bitmap(0, 0, src->bitmap_width, src->bitmap_height);

// clip the bitmap to the destination width
  src_bitmap.right = std::min(src_bitmap.width(), dest.width());
  src_bitmap.bottom = std::min(src_bitmap.height(), dest.height());

// copy the lines from src-> dest
  for(int row = 0; row < src_bitmap.bottom; row++)
    {
    fast_copy(point_to_address(point_t(dest.left, row + dest.top)), 0,
        reinterpret_cast<const uint8_t *>(src->pixels
            + (row * src->bitmap_width)), dest.width(), mode);
    }
  }

void kotuku::raster_screen_t::bit_blt(const rect_t &clip_rect,
    const rect_t &dest_rect, const screen_t *src_screen,
    const rect_t &src_clip_rect, const point_t &src_pt,
    raster_operation operation)
  {
// we copy 32 bit pixels using the fast copy

// if the src point is outside the source then done
  if(!(src_clip_rect && src_pt))
    return;

// if the destination is to the left of teh clip-rect then
// adjust both it and the src-rect

// determine how much to shift the left by
  gdi_dim_t dst_left = std::max(dest_rect.left, clip_rect.left);
  gdi_dim_t src_left_offset =
      dst_left > dest_rect.left ? dst_left - dest_rect.left : 0;

  gdi_dim_t dst_top = std::max(dest_rect.top, clip_rect.top);
  gdi_dim_t src_top_offset =
      dst_top > dest_rect.top ? dst_top - dest_rect.top : 0;

  gdi_dim_t dst_right = std::min(dest_rect.right, clip_rect.right);
  gdi_dim_t src_right_offset =
      dest_rect.right < dst_right ? dst_right - dest_rect.right : 0;

  gdi_dim_t dst_bottom = std::min(dest_rect.bottom, clip_rect.bottom);
  gdi_dim_t src_bottom_offset =
      dest_rect.bottom > dst_bottom ? dst_bottom - dest_rect.bottom : 0;

  rect_t destination(dst_left, dst_top, dst_right, dst_bottom);

  extent_t dst_size = destination.extents();
  if(dst_size.dx == 0 || dst_size.dy == 0)
    return;

  rect_t source(src_pt, dest_rect.extents());

  source.left += src_left_offset;
  source.top += src_top_offset;
  source.right += src_right_offset;
  source.bottom += src_bottom_offset;

  source &= src_clip_rect;

  extent_t src_size = source.extents();

  if(src_size.dx == 0 || src_size.dy == 0)
    return;

// calculate the smallest size we can copy
  extent_t copy_area(std::min(src_size.dx, dst_size.dx),
      std::min(src_size.dy, dst_size.dy));

// we do not support mixed raster screens with some other form
// of screen.  This will bail if the destination is a different type
// should be a dynamic_cast
  const raster_screen_t *src =
      reinterpret_cast<const raster_screen_t *>(src_screen);

  for(gdi_dim_t row = 0; row < copy_area.dy; row++)
    {
    point_t src_pt(source.left, source.top + row);
    //reinterpret_cast<const raster_screen_t *>(src)->apply_rotation(src_pt, src_pt);
    const uint8_t *src_ptr = src->point_to_address(src_pt);
    point_t dst_pt(destination.left, destination.top + row);
    //apply_rotation(dst_pt, dst_pt);
    uint8_t *dst_ptr = point_to_address(dst_pt);

    fast_copy(dst_ptr, reinterpret_cast<const raster_screen_t *>(src_screen),
        src_ptr, copy_area.dx, operation);
    }

// we must now fill the left, top, right and bottom areas with blackness
  }

void kotuku::raster_screen_t::mask_blt(const rect_t &clip_rect,
    const rect_t &dest_rect, const screen_t *src_screen,
    const rect_t &src_clip_rect, const point_t &src_point,
    const bitmap_t &mask_bitmap, const point_t &mask_point,
    raster_operation operation)
  {
  }

color_t kotuku::raster_screen_t::get_pixel(const rect_t &clip_rect, const point_t &pt) const
  {
  if(!(clip_rect && pt))
    return color_black;

  point_t src_pt;
  apply_rotation(pt, src_pt);

  return get_pixel(point_to_address(src_pt));
  }

color_t kotuku::raster_screen_t::set_pixel(
    const rect_t &clip_rect, const point_t &pt,
    color_t c)
  {
  if(!(clip_rect && pt))
    return color_black;

  point_t dst_pt;
  apply_rotation(pt, dst_pt);

  uint8_t *ptr = point_to_address(dst_pt);
  color_t old_color = get_pixel(ptr);
  set_pixel(ptr, c);

  return old_color;
  }

inline double adjust_radians_to_windows(double angle)
  {
  angle -= rad_90;

  return angle;
  }

/*
 Draws an arc. The arc is drawn along the perimeter of a circle with the
 given radius and center. The length of the arc is defined by the given
 start and sweep angles.

 _AngleArc moves the current position to the ending point of the arc.
 The arc is drawn by constructing an imaginary circle with the specified
 radius around the specified center point. The starting point of the arc is
 determined by measuring counterclockwise from the x-axis of the circle by
 the number of degrees in the start angle. The ending point is similarly
 located by measuring counterclockwise from the starting point by the
 number of degrees in the sweep angle.

 If the sweep angle is greater than 360 degrees the arc is swept multiple
 times. The figure is not filled.
 */
void kotuku::raster_screen_t::angle_arc(const rect_t &clip_rect,
    const pen_t *pen, const point_t &p, gdi_dim_t radius,
    double start_angle, double end_angle)
  {
  start_angle = adjust_radians_to_windows(start_angle);
  end_angle = adjust_radians_to_windows(end_angle);

  // normalise the drawing numbers
  if(start_angle > end_angle)
    std::swap(start_angle, end_angle);

  point_t pt(radius, 0);

  gdi_dim_t decision_over2 = 1 - pt.x;

  gdi_dim_t offset = pen->width >> 1;

  extent_t rect_offset(0 - offset, 0 - offset);
  extent_t wide_pen(pen->width, pen->width);
  double rad_45 = degrees_to_radians(45);

  while(pt.y <= pt.x)
    {
    double angle = atan2((double) pt.y, (double) pt.x);

    if(pen->width == 1)
      {
      // forward angles first
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.x + p.x, pt.y + p.y), pen->color); // octant 1

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.y + p.x, pt.x + p.y), pen->color); // octant 3

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.x + p.x, -pt.y + p.y), pen->color); // octant 5

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.y + p.x, -pt.x + p.y), pen->color); // octant 7

      angle -= rad_270;
      angle *= -1;
      angle += rad_90;

      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.y + p.x, pt.x + p.y), pen->color); // octant 2

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.x + p.x, pt.y + p.y), pen->color); // octant 4

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(-pt.y + p.x, -pt.x + p.y), pen->color); // octant 6

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        set_pixel(clip_rect, point_t(pt.x + p.x, -pt.y + p.y), pen->color); // octant 8
      }
    else
      {
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(pt.x + p.x, pt.y + p.y), wide_pen) + rect_offset); // octant 1

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(-pt.y + p.x, pt.x + p.y), wide_pen) + rect_offset); // octant 3

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(-pt.x + p.x, -pt.y + p.y), wide_pen) + rect_offset); // octant 5

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(pt.y + p.x, -pt.x + p.y), wide_pen) + rect_offset); // octant 7

      angle -= rad_270;
      angle *= -1;
      angle += rad_90;

      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(pt.y + p.x, pt.x + p.y), wide_pen) + rect_offset); // octant 2

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(-pt.x + p.x, pt.y + p.y), wide_pen) + rect_offset); // octant 4

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(-pt.y + p.x, -pt.x + p.y), wide_pen) + rect_offset); // octant 6

      angle += rad_90;
      if(angle >= start_angle && angle <= end_angle)
        ellipse(clip_rect, pen, pen->color,
            rect_t(point_t(pt.x + p.x, -pt.y + p.y), wide_pen) + rect_offset); // octant 8
      }

    pt.y++;
    if(decision_over2 <= 0)
      decision_over2 += 2 * pt.y + 1; // Change in decision criterion for y -> y+1
    else
      {
      pt.x--;
      decision_over2 += 2 * (pt.y - pt.x) + 1;  // Change for y -> y+1, x -> x-1
      }
    }
  }

void kotuku::raster_screen_t::pie(const rect_t &clip_rect,
    const pen_t *pen, color_t c, const point_t &p, double start,
    double end, gdi_dim_t radii, gdi_dim_t inner)
  {
  if(end < start)
    std::swap(start, end);

// special case for start == end
  if(start == end)
    {
    point_t pts[2] =
      {
      point_t(p.x + inner, p.y), point_t(p.x + radii, p.y)
      };

    rotate_point(p, pts[0], start);
    rotate_point(p, pts[1], start);

    polyline(clip_rect, pen, pts, 2);
    return;
    }
// we draw a polygon using the current pen
// and brush.  We first need to calculate the outer
// point count
  double outer_incr = atan2(1.0, radii);
  double delta = end - start;

  delta = (delta < 0) ? -delta : delta;

  gdi_dim_t segs = gdi_dim_t(delta / outer_incr) + 1;

  double inner_incr;

// see if we have an inner radii
  if(inner > 0)
    {
    inner_incr = atan2(1.0, inner);
    segs += gdi_dim_t(delta / inner_incr) + 1;
    }
  else
    segs++;

// We allocate 1 more point so that the polyline will fill the area.
  std::auto_ptr<point_t> pts(new point_t[segs + 1]);

  int pt = 1;
  pts.get()[0] = p;
  double theta;
  for(theta = start; theta <= end; theta += outer_incr)
    {
    pts.get()[pt].x = p.x + radii;
    pts.get()[pt].y = p.y;
    rotate_point(p, pts.get()[pt], theta);
    pt++;
    }

  if(inner > 0)
    {
    for(theta = end; theta >= start; theta -= inner_incr)
      {
      pts.get()[pt].x = p.x + inner;
      pts.get()[pt].y = p.y;
      rotate_point(p, pts.get()[pt], theta);
      pt++;
      }
    }
  else
    pts.get()[pt] = p;

  polygon(clip_rect, pen, c, pts.get(), segs + 1, false);
  }

void kotuku::raster_screen_t::draw_text(const rect_t &clip_rect,
    const font_t *font, color_t fg, color_t bg, const char *str,
    size_t count, const point_t &pt,
    const rect_t &txt_clip_rect, text_flags format, size_t *char_widths)
  {
  // create a clipping rectangle as needed
  rect_t txt_rect(clip_rect);
  point_t pt_pos = pt;

  if(format & eto_clipped)
    txt_rect &= txt_clip_rect;

  // a font is a monochrome bitmap.
  for(size_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];

    if(c > font->last_char || c < font->first_char)
      c = font->default_char;

    c -= font->first_char;

    size_t cell_width = font->char_table[c << 1];
    // see if the user wants the widths returned
    if(char_widths != 0)
      char_widths[ch] = cell_width;

    size_t offset = font->char_table[(c << 1) + 1];
    const uint8_t *mask = font->bitmap_pointer + offset;

    size_t columns = (((cell_width - 1) | 7) + 1) >> 3;
    for(size_t col = 0; col < columns; col++)
      {
      for(size_t row = 0; row < font->bitmap_height; row++)
        {
        size_t bit = 0;

        for(size_t pel = (col << 3); bit < 8 && pel < cell_width; pel++, bit++)
          {
          point_t pos(gdi_dim_t(pt_pos.x + pel),
              gdi_dim_t(pt_pos.y + row));

          bool is_fg = (*mask & (0x80 >> bit)) != 0;

          if(clip_rect && pos)
            {
            if(is_fg)
              set_pixel(point_to_address(pos), fg);
            else if(format & eto_opaque)
              set_pixel(point_to_address(pos), bg);
            }
          }

        // increment past the current row
        mask++;
        }
      }

    pt_pos.x += gdi_dim_t(cell_width);
    }
  }

kotuku::extent_t kotuku::raster_screen_t::text_extent(const font_t *font,
    const char *str, size_t count) const
  {
  extent_t ex(0, gdi_dim_t(font->bitmap_height));

  for(size_t ch = 0; ch < count; ch++)
    {
    // get the character to use.  If the character is outside the
    // map then we use the default char
    char c = str[ch];
    if(c == 0)
      break;							// end of the string

    if(c > font->last_char || c < font->first_char)
      c = font->default_char;

    c -= font->first_char;

    ex.dx += font->char_table[c << 1];
    }

  return ex;
  }

void kotuku::raster_screen_t::scroll(const rect_t &clip_rect,
    const extent_t &offsets, const rect_t &area_to_scroll,
    const rect_t &clipping_rectangle, rect_t *rect_update)
  {
  }

void kotuku::raster_screen_t::rotate_blt(const rect_t &clip_rect,
    const point_t &dest_center, const screen_t *src,
    const rect_t &src_clip_rect, const point_t &src_point,
    size_t radius, double angle, raster_operation operation)
  {
  // we perform this operation only on the circle bounded however for
  // speed we perform this as a retangle
  gdi_dim_t r2 = gdi_dim_t(radius) * gdi_dim_t(radius);

  // assign the start and end to the rectangle bounds relative to the origin
  gdi_dim_t init_x;
  gdi_dim_t y;
  gdi_dim_t right;
  gdi_dim_t bottom;

  if(src_point.x - src_clip_rect.left < gdi_dim_t(radius))
    init_x = src_clip_rect.left;
  else
    init_x = src_point.x - gdi_dim_t(radius);

  if(src_clip_rect.right - src_point.y < gdi_dim_t(radius))
    right = src_clip_rect.right;
  else
    right = src_point.x + gdi_dim_t(radius);

  if(src_point.y - src_clip_rect.top < gdi_dim_t(radius))
    y = src_clip_rect.top;
  else
    y = src_point.y - gdi_dim_t(radius);

  if(src_clip_rect.bottom - src_point.y < gdi_dim_t(radius))
    bottom = src_clip_rect.bottom;
  else
    bottom = src_point.y + gdi_dim_t(radius);

  for(; y < bottom; y++)
    for(gdi_dim_t x = init_x; x < right; x++)
      {
      int yp = y - src_point.y;
      int xp = x - src_point.x;
      // simple pythagorus theorum to check the inclusion
      if(((yp * yp) + (xp * xp)) > r2)
        continue;

      color_t cr =
          reinterpret_cast<const raster_screen_t *>(src)->get_pixel(
              reinterpret_cast<const raster_screen_t *>(src)->point_to_address(
                  point_t(x, y)));

      if(cr != color_t(-1) && (operation != rop_srcpaint || cr != 0))
        {
        point_t pt(x, y);

        rotate_point(src_point, pt, angle);

        yp = pt.y - src_point.y;
        xp = pt.x - src_point.x;

        set_pixel(
            point_to_address(
                point_t(dest_center.x + xp, dest_center.y + yp)), cr);
        }
      }
  }

void kotuku::raster_screen_t::fast_fill(uint8_t *dest, size_t words,
    color_t fill_color, raster_operation rop)
  {
  gdi_dim_t ptr_incr;

  switch(display_mode())
    {
  case 0:
    ptr_incr = pixel_increment();
    break;
  case 90:
    ptr_incr = -(screen_y * pixel_increment());
    break;
  case 180:
    ptr_incr = -pixel_increment();
    break;
  case 270:
    ptr_incr = (screen_y * pixel_increment());
    break;
    }

  while(words--)
    {
    set_pixel(dest, execute_rop(fill_color, get_pixel(dest), rop));
    dest += ptr_incr;
    }
  }

void kotuku::raster_screen_t::fast_copy(uint8_t *dest,
    const raster_screen_t *src_screen, const uint8_t *src, size_t words,
    raster_operation rop)
  {
  gdi_dim_t ptr_incr;

  switch(display_mode())
    {
  case 0:
    ptr_incr = pixel_increment();
    break;
  case 90:
    ptr_incr = -(screen_y * pixel_increment());
    break;
  case 180:
    ptr_incr = -pixel_increment();
    break;
  case 270:
    ptr_incr = (screen_y * pixel_increment());
    break;
    }

  gdi_dim_t src_increment =
      src_screen == 0 ? sizeof(color_t) : src_screen->pixel_increment();
  while(words--)
    {
    color_t pixel;
    if(src_screen == 0)
      pixel = *reinterpret_cast<const color_t *>(src); // special case for copy from bitmaps
    else
      pixel = src_screen->get_pixel(src);

    src += src_increment;
    set_pixel(dest, execute_rop(pixel, get_pixel(dest), rop));
    dest += ptr_incr;
    }
  }

gdi_dim_t kotuku::raster_screen_t::pixel_increment() const
  {
  return sizeof(color_t);
  }

color_t kotuku::raster_screen_t::get_pixel(const uint8_t *src) const
  {
  return *reinterpret_cast<const color_t *>(src);
  }

void kotuku::raster_screen_t::set_pixel(uint8_t *dest, color_t color) const
  {
  *reinterpret_cast<color_t *>(dest) = color;
  }

color_t kotuku::raster_screen_t::execute_rop(color_t src,
    color_t dst, raster_operation rop)
  {
  switch(rop)
    {
  case color_hollow:
    return dst;
  case rop_r2_masknotpen:			// dpna
    return dst & ~src;
  case rop_r2_maskpennot:			// pdna
    return ~(dst & src);
  case rop_r2_notxorpen:				// dpxn
    return ~(dst ^ src);
  case rop_r2_nop:							// d
    return dst;
  case rop_r2_mergepennot:			// pdno
    return ~(dst | src);
  case rop_patcopy:						// dest = pattern
  case rop_srccopy:						// dest = source
  case rop_r2_copypen:					// p
  case rop_r2_last:
    return src;
  case rop_patpaint:						// dest = dpsnoo
  case rop_srcpaint:						// dest = source or dest
  case rop_r2_notmergepen:			// dpon
  case rop_r2_mergepen:				// dpo
    return src | dst;
  case rop_srcand:							// dest = source and dest
  case rop_r2_maskpen:					// dpa
    return src & dst;
  case rop_patinvert:					// dest = pattern xor dest
  case rop_srcinvert:					// dest = source xor dest
  case rop_r2_xorpen:					// dpx
    return src ^ dst;
  case rop_r2_notmaskpen:			// dpan
  case rop_srcerase:						// dest = source and (not dest )
    return src & ~dst;
  case rop_notsrccopy:					// dest = (not source)
  case rop_r2_notcopypen:			// pn
    return ~src;
  case rop_notsrcerase:				// dest = (not src) and (not dest)
    return ~src & ~dst;
  case rop_mergecopy:					// dest = (source and pattern)
    return src & dst;
  case rop_r2_mergenotpen:			// dpno
  case rop_mergepaint:					// dest = (not source) or dest
    return ~src | dst;

  case rop_dstinvert:					// dest = (not dest)
  case rop_r2_not:							// dn
    return ~dst;
  case rop_r2_black:						//  0
  case rop_blackness:					// dest = black
    return 0;
  case rop_whiteness:					// dest = white
    return 0x00FFFFFF;
    }
  return 0;
  }

void kotuku::raster_screen_t::background_mode(int m)
  {
  _background_mode = m;
  }

int kotuku::raster_screen_t::display_mode() const
  {
  return 0;
  }

void kotuku::raster_screen_t::display_mode(int value)
  {
  }
