#include "photon_priv.h"


result_t bit_blt(handle_t hndl, const rect_t* clip_rect,
  const rect_t* dest_rect, handle_t src, const rect_t* src_clip_rect,
  const point_t* src_pt, raster_operation operation)
  {
  if (clip_rect == 0 || dest_rect == 0 || src_clip_rect == 0 || src_pt == 0)
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  canvas_t* src_canvas;
  if (failed(result = is_typeof(src, &canvas_type, (void**)&src_canvas)))
    return result;

  // if the src point is outside the source then done
  if (!rect_contains(src_clip_rect, src_pt))
    return e_bad_parameter;

  // if the destination is to the left of teeh clip-rect then
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

  rect_t destination;
  rect_create(dst_left, dst_top, dst_right, dst_bottom, &destination);
  extent_t dst_size;
  rect_extents(&destination, &dst_size);

  if (dst_size.dx == 0 || dst_size.dy == 0)
    return s_ok;

  rect_t source;

  rect_create(
    src_pt->x + src_left_offset,
    src_pt->y + src_top_offset,
    src_pt->x + rect_width(dest_rect) + src_right_offset,
    src_pt->y + rect_height(dest_rect) + src_bottom_offset,
    &source);

  rect_intersect(clip_rect, &source, &source);

  extent_t src_size;
  rect_extents(&source, &src_size);

  if (src_size.dx == 0 || src_size.dy == 0)
    return e_bad_parameter;

  point_t dst_pt;
  rect_top_left(&destination, &dst_pt);

  return (*canvas->fb->fast_copy)(canvas->fb, &dst_pt, src_canvas->fb, &source, operation);
  }
