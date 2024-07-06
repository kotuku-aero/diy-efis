#include "photon_priv.h"

result_t rectangle(handle_t hndl, const rect_t* clip_rect, color_t pen, color_t color, const rect_t* rect)
  {
  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  //trace_info("rectangle( %d, %d, %d, %d )\n", rect->left, rect->top, rect->right, rect->bottom);

  rect_t rect_to_fill;
  rect_intersect(clip_rect, rect, &rect_to_fill);
  // the we fill the inside
  (*canvas->fb->fast_fill)(canvas->fb, &rect_to_fill, color);

  if (pen != color_hollow)
    {
    // draw the outline
    point_t pts[5];

    rect_top_left(rect, pts + 0);
    rect_bottom_left(rect, pts + 1);
    rect_bottom_right(rect, pts + 2);
    rect_top_right(rect, pts + 3);
    rect_top_left(rect, pts + 4);

    polyline(hndl, clip_rect, pen, 5, pts);
    }

  return s_ok;
  }
