#include "photon_priv.h"

// #define DEBUG_CLIPPER

/*
// Returns x-value of point of intersection of two
// lines
int x_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (x3-x4) -
              (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// Returns y-value of point of intersection of
// two lines
int y_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (y3-y4) -
              (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}*/

static bool intersect_line(const point_t* p1, const point_t* p2, // line 1
  const point_t* p3, const point_t* p4,    // line 2
  point_t* ip)
  {
  int32_t x1 = p1->x;
  int32_t y1 = p1->y;
  int32_t x2 = p2->x;
  int32_t y2 = p2->y;
  int32_t x3 = p3->x;
  int32_t y3 = p3->y;
  int32_t x4 = p4->x;
  int32_t y4 = p4->y;

  int32_t nx = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
  int32_t dx = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  ip->x = nx / dx;

  int32_t ny = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
  int32_t dy = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  ip->y = ny / dy;

  return true;
  }

static void clip_to_line(canvas_t* canvas, const point_t *clip1, const point_t *clip2)
  {
#ifdef DEBUG_CLIPPER
  trace_debug("Clip to line p1.x = %d, p1.y = %d, p2.x = %d, p2.y = %d\n", 
    clip1->x, clip1->y, clip2->x, clip2->y);
#endif

  points_clear(&canvas->solution);

  int len = points_count(&canvas->path);

  for (int i = 0; i < len; i++)
    {
    point_t *pts = points_begin(&canvas->path);

    // forms a line
    point_t *p1 = pts + i;

    point_t *p2;

    if(i == len-1)
      {
      if(point_equals(pts + i, pts))
        {
#ifdef DEBUG_CLIPPER
        trace_debug("Last point already closed polygon, so skip\n");
#endif

        break;      // closed polygon
        }

      // last point p2 = start
      p2 = pts;
      }
    else
      p2 = pts + i + 1;

    point_t ip;

#ifdef DEBUG_CLIPPER
  trace_debug(" Evaluate edge p1.x = %d, p1.y = %d, p2.x = %d, p2.y = %d result = ",
    p1->x, p1->y, p2->x, p2->y);

  if(p1->x > 16384 || p1->x < -16384)
    debug_break();
#endif

    // calculate position of p1
    int32_t pos1 = (clip2->x - clip1->x) * (p1->y - clip1->y) - (clip2->y - clip1->y) * (p1->x - clip1->x);
    // calculate position of p2
    int32_t pos2 = (clip2->x - clip1->x) * (p2->y - clip1->y) - (clip2->y - clip1->y) * (p2->x - clip1->x);

    if(pos1 < 0 && pos2 < 0)
      {
#ifdef DEBUG_CLIPPER
      trace_debug(" case 1, both inside\n");
#endif
      // case 1 both points inside
      points_push_back(&canvas->solution, p2);
      }
    else if (pos1 >= 0 && pos2 < 0)
      {
#ifdef DEBUG_CLIPPER
      trace_debug(" case 1, first point outside\n");
#endif
      // Case 2: When only first point is outside
      // Point of intersection with edge and the second point is added
      intersect_line(clip1, clip2, p1, p2, &ip);

#ifdef DEBUG_CLIPPER
      trace_debug("    Intersection x = %d, y = %d\n", ip.x, ip.y);
#endif

      points_push_back(&canvas->solution, &ip);
      points_push_back(&canvas->solution, p2);
      }
    else if(pos1 < 0 && pos2 >= 0)
      {
#ifdef DEBUG_CLIPPER
      trace_debug(" case 3, second point outside\n");
#endif
      // Case 3: When only second point is outside
      intersect_line(clip1, clip2, p1, p2, &ip);

#ifdef DEBUG_CLIPPER
      trace_debug("    Intersection x = %d, y = %d\n", ip.x, ip.y);
#endif

      points_push_back(&canvas->solution, &ip);
      }
    else
      {
#ifdef DEBUG_CLIPPER
      trace_debug(" case 3, both points outside\n");
#endif
      // Case 4: When both points are outside
      }
    }

#ifdef DEBUG_CLIPPER
    trace_debug("Resulting polygon - %d points\n", points_count(&canvas->solution));
    for (int i = 0; i < points_count(&canvas->solution); i++)
      {
      point_t *pt = points_begin(&canvas->solution) + i;
      trace_debug("  point %d  x = %d, y = %d\n", i, pt->x, pt->y);
      }
#endif
    // copy solution to path
    points_clear(&canvas->path);
    points_append(&canvas->path, points_count(&canvas->solution), points_begin(&canvas->solution));
  }

/**
 * @brief clip the points to the clipping rectangle
 * @param clip_rect   rectangle to clip to
 * @param pts points to be clipped
 * @return
*/
result_t clip_path(canvas_t* canvas, const rect_t* clip_rect)
  {
  points_clear(&canvas->clip_points);
  points_clear(&canvas->solution);

  point_t p1;
  point_t p2;

  clip_to_line(canvas, rect_top_left(clip_rect, &p1), rect_bottom_left(clip_rect, &p2));
  clip_to_line(canvas, rect_bottom_left(clip_rect, &p1), rect_bottom_right(clip_rect, &p2));
  clip_to_line(canvas, rect_bottom_right(clip_rect, &p1), rect_top_right(clip_rect, &p2));
  clip_to_line(canvas, rect_top_right(clip_rect, &p1), rect_top_left(clip_rect, &p2));

#ifdef DEBUG_CLIPPER
  trace_debug("Close open polygon\n");
#endif

  // result is an open polygon so close it
  points_push_back(&canvas->solution, points_begin(&canvas->solution));

  return s_ok;
  }

