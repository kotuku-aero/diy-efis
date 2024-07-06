#include "photon_priv.h"

result_t round_rect(handle_t hndl, const rect_t* clip_rect, color_t pen,
  color_t fill, const rect_t* rect, gdi_dim_t dim)
  {
  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  // all lines are drawn on the last pixel
  rect_t draw_rect = {
    rect->left,
    rect->top,
    rect->right - 1,
    rect->bottom - 1
    };

  rect_t rect_fill;
  // fill
  if (fill != color_hollow)
    {
    // top fill
    (*canvas->fb->fast_fill)(canvas->fb,
      rect_create(draw_rect.left + dim,
        draw_rect.top + 1,
        draw_rect.right - dim - 1,
        draw_rect.top + dim + 1,
        &rect_fill), fill);

    // bottom fill
    (*canvas->fb->fast_fill)(canvas->fb,
      rect_create(draw_rect.left + dim + 1,
        draw_rect.bottom - dim,
        draw_rect.right - dim - 1,
        draw_rect.bottom - 1,
        &rect_fill), fill);

    // center fill
    (*canvas->fb->fast_fill)(canvas->fb,
      rect_create(draw_rect.left + 1,
        draw_rect.top + dim + 1,
        draw_rect.right - 1,
        draw_rect.bottom - dim - 1,
        &rect_fill), fill);
    }

  if (pen != color_hollow)
    {
    // lines
    point_t pt;

    points_clear(&canvas->path);
    points_push_back(&canvas->path, point_create(draw_rect.left + dim, draw_rect.top, &pt));
    points_push_back(&canvas->path, point_create(draw_rect.right - dim, draw_rect.top, &pt));
    polyline_impl(canvas, clip_rect, pen);

    points_clear(&canvas->path);
    points_push_back(&canvas->path, point_create(draw_rect.right, draw_rect.top + dim, &pt));
    points_push_back(&canvas->path, point_create(draw_rect.right, draw_rect.bottom - dim, &pt));
    polyline_impl(canvas, clip_rect, pen);

    points_clear(&canvas->path);
    points_push_back(&canvas->path, point_create(draw_rect.left, draw_rect.top + dim, &pt));
    points_push_back(&canvas->path, point_create(draw_rect.left, draw_rect.bottom - dim, &pt));
    polyline_impl(canvas, clip_rect, pen);

    points_clear(&canvas->path);
    points_push_back(&canvas->path, point_create(draw_rect.left + dim, draw_rect.bottom, &pt));
    points_push_back(&canvas->path, point_create(draw_rect.right - dim, draw_rect.bottom, &pt));
    polyline_impl(canvas, clip_rect, pen);
    }

  // Arcs
  // a round rect is a series of lines and arcs.

  // top left
  ellipse_impl(canvas, clip_rect, pen, fill,
    rect_create(draw_rect.left,
      draw_rect.top,
      draw_rect.left + (dim << 1),
      draw_rect.top + (dim << 1), &rect_fill));

  // bottom left
  ellipse_impl(canvas, clip_rect, pen, fill,
    rect_create(draw_rect.left,
      draw_rect.bottom - (dim << 1) - 1,
      draw_rect.left + (dim << 1),
      draw_rect.bottom, &rect_fill));

  // top_right
  ellipse_impl(canvas, clip_rect, pen, fill,
    rect_create(draw_rect.right - (dim << 1) - 1,
      draw_rect.top,
      draw_rect.right,
      draw_rect.top + (dim << 1), &rect_fill));

  // bottom right
  ellipse_impl(canvas, clip_rect, pen, fill,
    rect_create(draw_rect.right - (dim << 1) - 1,
      draw_rect.bottom - (dim << 1) - 1,
      draw_rect.right,
      draw_rect.bottom, &rect_fill));

  return s_ok;
  }
