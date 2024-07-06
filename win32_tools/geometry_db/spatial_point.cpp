#include "db_geometry.h"

kotuku::spatial_point::spatial_point(geometry_db* db, uint32_t type)
  : spatial_entity(db, type)
  {
  }

bool kotuku::spatial_point::include_in_index(int lat, int lng) const
  {
  return fixed_to_int(center().lat) == lat && fixed_to_int(center().lng) == lng;
  }

bool kotuku::spatial_point::include_in_database(const spatial_rect_t& rect) const
  {
  return point_in_rect(rect, center());
  }

bool kotuku::spatial_point::process_pointset(int lat, int lng)
  {
  // no points located
  return false;
  }
