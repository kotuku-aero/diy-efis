#include "photon_priv.h"

result_t ellipse_impl(canvas_t* canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area)
  {
  gdi_dim_t w = rect_width(area);
  w >>= 1;

  gdi_dim_t h = rect_height(area);
  h >>= 1;

  point_t c = {
    .x = area->left + w,
    .y = area->top + h
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
  int32_t a = w;
  int32_t b = h;
  int32_t x = 0;
  int32_t y = b;
  int32_t a_sqr = a * a;
  int32_t b_sqr = b * b;
  int32_t a22 = a_sqr + a_sqr;
  int32_t b22 = b_sqr + b_sqr;
  int32_t a42 = a22 + a22;
  int32_t b42 = b22 + b22;
  int32_t x_slope = b42;            // x_slope = (4 * b^^2) * (x * 1)
  int32_t y_slope = b42 * (y - 1);  // y_slope = (4 * a^^2) * (y - 1)
  int32_t mida = a_sqr >> 1;        // a^^2 / 2
  int32_t midb = b_sqr >> 1;        // b^^2 / 2
  int32_t d = a_sqr - (y_slope >> 1) - mida;  // subtract a^^2 / 2 to optimize

  bool draw_pen = pen != color_hollow;
  bool draw_fill = fill != color_hollow;

  q1.x = (c.x);
  q1.y = (area->top + (h << 1));
  q2.x = (c.x);
  q2.y = (area->top + (h << 1));
  q3.x = (c.x);
  q3.y = (area->top);
  q4.x = (c.x);
  q4.y = (area->top);

  // region 1
  while (d <= y_slope)
    {
    // draw the ellipse point
    if (draw_pen && rect_contains(clip_rect, &q1))
      (*canvas->fb->set_pixel)(canvas->fb, &q1, pen, 0);

    if (!point_equals(&q1, &q2))
      {
      if (draw_pen && rect_contains(clip_rect, &q2))
        (*canvas->fb->set_pixel)(canvas->fb, &q2, pen, 0);

      if (draw_fill)
        {
        l1 = q1;
        l2 = q2;
        // fill the ellipse
        if (fill != ((l2.x - l1.x) > 2))
          {
          l1.x++;
          if (clip_line(&l1, &l2, clip_rect))
            (*canvas->fb->fast_line)(canvas->fb, &l1, &l2, fill);
          }
        }
      }

    if (!point_equals(&q1, &q3) &&
      draw_pen &&
      rect_contains(clip_rect, &q3))
      (*canvas->fb->set_pixel)(canvas->fb, &q3, pen, 0);

    if (!point_equals(&q2, &q4) && !point_equals(&q3, &q4))
      {
      if (draw_pen && rect_contains(clip_rect, &q4))
        (*canvas->fb->set_pixel)(canvas->fb, &q4, pen, 0);

      l1 = q3;
      l2 = q4;
      // fill the ellipse
      if (draw_fill && (l2.x - l1.x) > 2)
        {
        l1.x++;
        if (clip_line(&l1, &l2, clip_rect))
          (*canvas->fb->fast_line)(canvas->fb, &l1, &l2, fill);
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
    if (draw_pen && rect_contains(clip_rect, &q1))
      (*canvas->fb->set_pixel)(canvas->fb, &q1, pen, 0);

    if (!point_equals(&q1, &q2))
      {
      if (draw_pen && rect_contains(clip_rect, &q2))
        (*canvas->fb->set_pixel)(canvas->fb, &q2, pen, 0);

      if (draw_fill)
        {
        l1 = q1;
        l2 = q2;
        // fill the ellipse
        if (fill != ((l2.x - l1.x) > 2))
          {
          l1.x++;

          if (clip_line(&l1, &l2, clip_rect))
            (*canvas->fb->fast_line)(canvas->fb, &l1, &l2, fill);
          }
        }
      }

    if (!point_equals(&q1, &q2) && draw_pen && rect_contains(clip_rect, &q3))
      (*canvas->fb->set_pixel)(canvas->fb, &q3, pen, 0);

    if (!point_equals(&q2, &q4) && !point_equals(&q3, &q4))
      {
      if (draw_pen && rect_contains(clip_rect, &q4))
        (*canvas->fb->set_pixel)(canvas->fb, &q4, pen, 0);

      l1 = q3;
      l2 = q4;

      // fill the ellipse
      if (draw_fill && (l2.x - l1.x) > 2)
        {
        l1.x++;

        if (clip_line(&l1, &l2, clip_rect))
          (*canvas->fb->fast_line)(canvas->fb, &l1, &l2, fill);
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


  result_t ellipse(handle_t hndl, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area)
    {
    if (clip_rect == 0 || area == 0)
      return e_bad_pointer;

    canvas_t* canvas;
    result_t result;
    if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
      return result;

    return ellipse_impl(canvas, clip_rect, pen, fill, area);
    }
