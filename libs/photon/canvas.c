#include <math.h>
#include "photon_priv.h"

// for qsort
#include <stdlib.h>
#include <math.h>

const matrix_t identity_matrix;

const xyz_t* matrix_dot(const matrix_t* m, const xyz_t* v, xyz_t* r)
  {

  return r;
  }

const matrix_t* matrix_add(const matrix_t* m, const matrix_t* v, matrix_t* r)
  {

  return r;
  }

const matrix_t* matrix_subtract(const matrix_t* m, const matrix_t* v, matrix_t* r)
  {

  return r;
  }

const matrix_t identity_matrix = {
    {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 }
    }
  };

const matrix_t* matrix_multiply(const matrix_t* a, const matrix_t* b, matrix_t* mat)
  {
  int x;
  int y;
  int w;
  float op[3];

  for (x = 0; x < 3; x++)
    {
    for (y = 0; y < 3; y++)
      {
      for (w = 0; w < 3; w++)
        op[w] = a->v[x][w] * b->v[w][y];

      mat->v[x][y] = 0;
      mat->v[x][y] = op[0] + op[1] + op[2];
      }
    }

  return mat;
  }


const matrix_t* matrix_divide(const matrix_t* m, const matrix_t* v, matrix_t* r)
  {

  return r;
  }

const matrix_t* matrix_transpose(const matrix_t* m, matrix_t* r)
  {

  return r;
  }

const matrix_t* matrix_exponetiation(const matrix_t* m, matrix_t* r)
  {

  return r;
  }

const matrix_t* matrix_copy(const matrix_t* m, matrix_t* r)
  {

  return r;
  }

#define _DEBUG_POLYGON

const typeid_t canvas_type =
  {
  .name = "canvas"
  };

#ifndef _WIN32
float roundf(float value)
  {
  return (float)(value < 0.0 ? ceil(value - 0.5) : floor(value + 0.5));
  }
/*
float round(float value)
  {
  return value < 0.0 ? ceil(value - 0.5) : floor(value + 0.5);
  }
 * */
#endif

result_t get_pixel(handle_t hndl, const rect_t* clip_rect, const point_t* pt, color_t* pix)
  {
  if (clip_rect == 0 || pt == 0 || pix == 0)
    return e_bad_pointer;

  if (!rect_contains(clip_rect, pt))
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  return (*canvas->fb->get_pixel)(canvas->fb, pt, pix);
  }

result_t set_pixel(handle_t hndl, const rect_t* clip_rect, const point_t* pt, color_t c, color_t* pix)
  {
  if (c == color_hollow)
    {
    if (pix == 0)
      return s_ok;

    return get_pixel(hndl, clip_rect, pt, pix);
    }

  if (clip_rect == 0 || pt == 0)
    return e_bad_pointer;

  if (!rect_contains(clip_rect, pt))
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  return (*canvas->fb->set_pixel)(canvas->fb, pt, c, pix);
  }

result_t canvas_create(const extent_t* size, handle_t* hndl)
  {
  if (size == 0 || hndl == 0)
    return e_bad_parameter;

  framebuffer_t* fb;
  result_t result;

  if (failed(result = bsp_framebuffer_create_rect(size, &fb)))
    return result;

  return create_canvas_from_framebuffer(fb, hndl);
  }

result_t create_canvas_from_framebuffer(framebuffer_t* fb, handle_t* hndl)
  {
  result_t result;
  canvas_t* canvas;

  if (failed(result = neutron_calloc(1, sizeof(canvas_t), (void**)&canvas)))
    return result;


  canvas->base.type = &canvas_type;
  canvas->fb = fb;

  *hndl = (handle_t)canvas;

  return s_ok;
  }

result_t canvas_create_bitmap(const bitmap_t* bitmap, handle_t* hndl)
  {
  if (bitmap == 0 || hndl == 0)
    return e_bad_parameter;

  framebuffer_t* fb;
  result_t result;

  if (failed(result = bsp_framebuffer_create_bitmap(bitmap, &fb)))
    return result;

  canvas_t* canvas;
  if (failed(result = neutron_calloc(1, sizeof(canvas_t), (void**)&canvas)))
    return result;

  canvas->base.type = &canvas_type;
  canvas->fb = fb;

  *hndl = (handle_t)canvas;

  return s_ok;
  }

result_t close_canvas(handle_t hndl)
  {
  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  if (failed(result = bsp_framebuffer_close(canvas->fb)))
    return result;

  // so that if the handle is re-used it is not seen as valid
  canvas->base.type = 0;
  canvas->fb = 0;

  points_close(&canvas->solution);
  points_close(&canvas->path);
  points_close(&canvas->clip_points);
  edges_close(&canvas->edges);

  neutron_free(canvas);

  return s_ok;
  }

result_t canvas_extents(handle_t hndl, extent_t* ex)
  {
  if (ex == 0)
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
    return result;

  rect_extents(&canvas->fb->position, ex);
  return s_ok;
  }
