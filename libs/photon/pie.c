#include "photon_priv.h"


result_t pie(handle_t hndl, const rect_t* clip_rect, color_t outline_pen,
  color_t fill_color, const point_t* p, int16_t start, int16_t end, gdi_dim_t radii,
  gdi_dim_t inner)
  {
  if (clip_rect == 0 || p == 0)
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
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
    point_t pt;
    points_clear(&canvas->path);

    point_create(p->x + inner, p->y, &pt);
    points_push_back(&canvas->path, _rotate_point(p, start, &pt));

    point_create(p->x + radii, p->y, &pt);
    points_push_back(&canvas->path, _rotate_point(p, start, &pt));

    polyline_impl(canvas, clip_rect, outline_pen);
    }

  // we draw a polygon using the current pen
  // and brush.  We first need to calculate the outer
  // point count
  gdi_dim_t delta = end - start;

  delta = (delta < 0) ? -delta : delta;

  points_clear(&canvas->path);
  if (inner == 0)
    points_push_back(&canvas->path, p);

  point_t last_point;
  point_t rp;
  point_copy(p, &last_point);

  int theta;
  for (theta = start; theta <= end; theta++)
    {
    rp.x = p->x + radii;
    rp.y = p->y;

    _rotate_point(p, theta, &rp);

    if (!point_equals(&rp, &last_point))
      {
      points_push_back(&canvas->path, &rp);

      point_copy(&rp, &last_point);
      }
    }

  if (inner > 0)
    {
    for (theta = end; theta >= start; theta--)
      {
      rp.x = p->x + inner;
      rp.y = p->y;

      _rotate_point(p, theta, &rp);

      if (!point_equals(&rp, &last_point))
        {
        points_push_back(&canvas->path, &rp);

        point_copy(&rp, &last_point);
        }
      }

    // close the polygon
    points_push_back(&canvas->path, pt_at(&canvas->path, 0));
    }
  else
    points_push_back(&canvas->path, p);

  result = polygon_impl(canvas, clip_rect, outline_pen, fill_color);

  return result;
  }
