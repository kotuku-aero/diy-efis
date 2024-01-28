#include "photon_priv.h"


result_t polyline_impl(canvas_t* canvas, const rect_t* clip_rect, color_t pen)
  {
  const point_t *pts = points_begin(&canvas->path);
  // call the canvas->fb to draw each line.
  for (uint16_t pt = 0; pt < points_count(&canvas->path) - 1; pt++)
    {
    point_t p1;
    point_t p2;

    point_copy(pts + pt, &p1);
    point_copy(pts + (pt + 1), &p2);

    if(clip_line(&p1, &p2, clip_rect))
      (*canvas->fb->fast_line)(canvas->fb, &p1, &p2, pen);
    }

  return s_ok;
  }


result_t polyline(handle_t hndl, const rect_t* clip_rect, color_t pen, uint32_t len, const point_t* pts)
  {
  if (len < 2 || pts == 0 || clip_rect == 0)
    return e_bad_parameter;

  if (pen == color_hollow)
    return s_ok;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  points_clear(&canvas->path);
  while(len--)
    points_push_back(&canvas->path, pts++);

  return polyline_impl(canvas, clip_rect, pen);
  }
