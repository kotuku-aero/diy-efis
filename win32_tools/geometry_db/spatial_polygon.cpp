#include "db_geometry.h"
#include "../clipper2/clipper.h"

kotuku::spatial_polygon::spatial_polygon(geometry_db* db, uint32_t type)
  : spatial_linestring(db, type)
  {
  }

bool kotuku::spatial_polygon::include_in_index(int lat, int lng) const
  {
  // need to use clipperlib to see if the rect is contained within
  // the polygon

  return true;
  }

bool kotuku::spatial_polygon::include_in_database(const spatial_rect_t& rect) const
  {
  return intersects(rect, bounds());
  }

bool kotuku::spatial_polygon::process_pointset(int lat, int lng)
  {
  // create a set of points
  spatial_rect_t rect;
  rect.left = int_to_fixed(lng < 0 ? lng - 1 : lng);
  rect.top = int_to_fixed(lat);
  rect.right = int_to_fixed(lng < 0 ? lng : lng + 1);
  rect.bottom = int_to_fixed(lat-1);

//#ifdef _DEBUG
//  cout << "Clip polygon to " <<
//    fixed_to_int(rect.left) << ", " <<
//    fixed_to_int(rect.top) << ", " <<
//    fixed_to_int(rect.right) << ", " <<
//    fixed_to_int(rect.bottom) << endl;
//#endif

  Clipper2Lib::Path64 clip_path;

  clip_path.push_back(Clipper2Lib::Point64(rect.left, rect.top, 0));
  clip_path.push_back(Clipper2Lib::Point64(rect.right, rect.top, 0));
  clip_path.push_back(Clipper2Lib::Point64(rect.right, rect.bottom, 0));
  clip_path.push_back(Clipper2Lib::Point64(rect.left, rect.bottom, 0));
  clip_path.push_back(Clipper2Lib::Point64(rect.left, rect.top, 0));

  Clipper2Lib::Paths64 clipperPath;
  clipperPath.push_back(clip_path);

  bool has_polyline = false;
  for (polylines::const_iterator it = _paths.begin(); it != _paths.end(); it++)
    {
    Clipper2Lib::Path64 path;
    Clipper2Lib::Paths64 subjectPath;
    path.clear();

    for (int i = 0; i < it->size(); i++)
      {
//#ifdef _DEBUG
//      cout << "Add point " << fixed_to_float((*it)[i].latlng.lng) << ", " << fixed_to_float((*it)[i].latlng.lat) << endl;
//#endif
      path.push_back(Clipper2Lib::Point64((*it)[i].latlng.lng, (*it)[i].latlng.lat, (*it)[i].elevation));
      }

    subjectPath.push_back(path);

    Clipper2Lib::Paths64 result = Clipper2Lib::Intersect(subjectPath, clipperPath, Clipper2Lib::FillRule::EvenOdd);

    // TODO: handle holes!

    // save the result;
    polylines lines;
    points line;
    for (Clipper2Lib::Paths64::const_iterator it = result.begin(); it != result.end(); it++)
      {
      line.clear();
      for (Clipper2Lib::Path64::const_iterator pts = it->begin(); pts != it->end(); pts++)
        {
        spatial_point_t pt;
        pt.latlng.lat = (fixed_t)pts->y;
        pt.latlng.lng = (fixed_t)pts->x;
        pt.elevation = (int16_t)pts->z;

        line.push_back(pt);
        }

      // close the line
      if (line[0] != line[line.size() - 1])
        line.push_back(line[0]);

      cout << "Entity creates polygon with " << line.size() << " vertex" << endl;

      lines.push_back(line);
      }

    if (lines.size() > 0)
      {
      _graphics.insert({ make_pair(lat, lng), lines });

      has_polyline = true;
      }
    }


  return has_polyline;
  }