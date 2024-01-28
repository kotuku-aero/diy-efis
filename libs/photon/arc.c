#include "photon_priv.h"

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

static int lookup_atan2(float scale, gdi_dim_t y, gdi_dim_t x)
  {
  uint32_t prod = (uint32_t)((y * scale) * (x * scale));
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

result_t arc(handle_t hndl, const rect_t* clip_rect, color_t pen,
  const point_t* p, gdi_dim_t radius, int16_t start_angle, int16_t end_angle)
  {
  if (clip_rect == 0 || p == 0)
    return e_bad_parameter;

  result_t result;
  canvas_t* canvas;
  if (failed(result = is_typeof(hndl, &canvas_type, (void**)&canvas)))
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
    radius,
    0
    };

  gdi_dim_t decision_over2 = 1 - radius;

  // our angles are 0..44 but our radius varies.  We need to calculate from
  // a given x and y coordinate the approximate angle.
  float scale = 1000.0f / radius;

  point_t dp;

  while (pt.y <= pt.x)
    {
    int angle = lookup_atan2(scale, pt.y, pt.x);

    // float angle = atan2((float) pt->y, (float) pt->x);
    // forward angles first
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(pt.x + p->x, pt.y + p->y, &dp), pen, 0); // octant 1

    angle += 90;
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(-pt.y + p->x, pt.x + p->y, &dp), pen, 0); // octant 3

    angle += 90;
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(-pt.x + p->x, -pt.y + p->y, &dp), pen, 0); // octant 5

    angle += 90;
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(pt.y + p->x, -pt.x + p->y, &dp), pen, 0); // octant 7

    angle -= 270;
    angle *= -1;
    angle += 90;

    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(pt.y + p->x, pt.x + p->y, &dp), pen, 0); // octant 2

    angle += 90;
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(-pt.x + p->x, pt.y + p->y, &dp), pen, 0); // octant 4

    angle += 90;
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(-pt.y + p->x, -pt.x + p->y, &dp), pen, 0); // octant 6

    angle += 90;
    if (angle >= start_angle && angle <= end_angle)
      (*canvas->fb->set_pixel)(canvas->fb, point_create(pt.x + p->x, -pt.y + p->y, &dp), pen, 0); // octant 8

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

result_t arc_3pt(handle_t canvas, const rect_t* clip_rect, color_t pen, const point_t* center, const point_t* start, const point_t* end)
  {
  return e_not_implemented;
  }