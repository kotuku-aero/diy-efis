#include "viewport.h"

void rotate_points(const spatial_point_t* center, int16_t angle, spatial_points_t* pts)
  {
  for (spatial_point_t* pt = spatial_points_begin(pts); pt != spatial_points_end(pts); pt++)
    // rotate the polygon in-place
    rotate_spatial_point(center, angle, pt);
  }

void rotate_contours(const spatial_point_t* center, int16_t angle, polylines_t* contours)
  {
  // very expensive....
  for (polyline_t* it = polylines_begin(contours); it != polylines_end(contours); it++)
    rotate_points(center, angle, it);
  }

void to_screen_coordinates(viewport_params_t* params, const latlng_t* top_left, const latlng_t* pt, point_t* gdi_pt)
  {
  // todo: use a rotation matrix
  // use a spherical projection
  gdi_pt->x = fixed_to_int(mul_fixed(params->geo_scale_x, sub_fixed(pt->lng, top_left->lng)));
  gdi_pt->y = fixed_to_int(mul_fixed(params->geo_scale_y, sub_fixed(top_left->lat, pt->lat)));
  }