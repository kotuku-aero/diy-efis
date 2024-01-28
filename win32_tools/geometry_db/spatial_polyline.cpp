#include "db_geometry.h"

kotuku::spatial_polyline::spatial_polyline(geometry_db* db, uint32_t type)
  : spatial_linestring(db, type)
  {
  }

bool kotuku::spatial_polyline::include_in_index(int lat, int lng) const
  {
  // need to do a line intersects rect to work this out

  return true;
  }


bool kotuku::spatial_polyline::include_in_database(const spatial_rect_t& rect) const
  {
  return intersects(rect, bounds());
  }

bool kotuku::spatial_polyline::process_pointset(int lat, int lng)
  {
  // create a rectangle
  spatial_rect_t rect;
  rect.left = int_to_fixed(lng);
  rect.top = int_to_fixed(lat);
  rect.right = int_to_fixed(lng + 1);
  rect.bottom = int_to_fixed(lat - 1);

  polylines lines;
  points line;
  bool has_polyline = false;
  for (polylines::const_iterator it = _paths.begin(); it != _paths.end(); it++)
    {
    for (size_t i = 0; i < it->size() - 1; i++)
      {
      spatial_point_t pt1 = (*it)[i];
      spatial_point_t pt2 = (*it)[i + 1];

      if (clip_to(rect, pt1, pt2))
        {
        if(line.size() == 0 ||
          line.rbegin()->latlng.lat != pt1.latlng.lat ||
          line.rbegin()->latlng.lng != pt1.latlng.lng)
          line.push_back(pt1);    // enter polygon

        line.push_back(pt2);

        // if pt2 is outside rect then end line
        if (!point_in_rect(rect, (*it)[i + 1].latlng))
          {
          lines.push_back(line);
          line.clear();
          }
        }
      else if (!line.empty())
        {
        // exit rect
        lines.push_back(line);
        line.clear();
        }
      }

    if (!line.empty())
      {
      lines.push_back(line);
      // cout << "Line complete, end of polyline" << endl;
      }
    }

  // possibly the polylines could have duplicate points,
  // clean them up
  for (polylines::iterator it = lines.begin(); it != lines.end(); it++)
    {
    points cleaned_line;
    for (size_t i = 0; i < it->size() - 1; i++)
      {
      spatial_point_t p1 = (*it)[i];
      spatial_point_t p2 = (*it)[i + 1];

      cleaned_line.push_back(p1);
      if (p2.latlng.lat == p1.latlng.lat && p2.latlng.lng == p1.latlng.lng)
        i++;          // skip next point
      else if(i == it->size()-2)
        cleaned_line.push_back(p2);
      }

    if (cleaned_line.size() > 1)
      *it = cleaned_line;
    }

  // at this point the polylines contain all lines
  // that intersect or are contained in the tile.
  if (lines.size() > 0)
    {
    _graphics.insert({ make_pair(lat, lng), lines });

    return true;
    }

  // no points located
  return false;
  }