#include "db_geometry.h"

#include <cassert>

kotuku::geometry_db::geometry_db(uint32_t type, const spatial_rect_t& extents)
  {
  _content_type = type;
  _bounds = extents;

  // allocate the spatial index reference based on the extents

  _lng_extent = fixed_to_int(sub_fixed(extents.right, extents.left));
  _lat_extent = fixed_to_int(sub_fixed(extents.top, extents.bottom));
  }

kotuku::geometry_db::~geometry_db()
  {
  for (size_t i = 0; i < _entities.size(); i++)
    delete _entities[i];
  }

bool kotuku::geometry_db::intersects(const spatial_rect_t& rect) const
  {
  return rect.right >= _bounds.left &&
    rect.bottom <= _bounds.top &&
    rect.left <= _bounds.right &&
    rect.top >= _bounds.bottom;
  }

void kotuku::geometry_db::name(const string& value)
  {
  if (value.length() > 31)
    throw string("Name can only be 31 characters");

  _name = value;
  }

//#ifdef _DEBUG
//static int num_poly = 0;
//#endif

static inline int correct_lng(int lng)
  {
  if(lng > 179)
    lng -= 360;
  else if(lng < -180)
    lng += 360;

  return lng;
  }

bool kotuku::geometry_db::add_spatial_entity(spatial_point* value)
  {
  int lat_index = 90 - fixed_to_int(value->center().lat);      // lattude is 90...-90
  // but index stored as north-up
  int lng_index = correct_lng(fixed_to_int(value->center().lng)) + 180;     // longitude is -180...180

  entities_t& list = _spatial_index[lat_index][lng_index];

  // cout << "Add entity to cell " << lat << ", " << lng << endl;
  if (find(list.begin(), list.end(), value) == list.end())
    list.push_back(value);

  _entities.push_back(value);

  return true;
  }

bool kotuku::geometry_db::add_spatial_entity(spatial_linestring* value)
  {
  if(value == nullptr)
    return false;
  // only add if the entity intersects the map.
  bool has_polygon = false;

//#ifdef _DEBUG
//  if (num_poly > 5)
//    return false;
//#endif

  // TODO: ask the entity to clip to the bounds so that
  // only the graphics needed are stored.
  if (intersects(value->bounds()))
    {
    int northing = fixed_to_int(value->bounds().top);
    int southing = fixed_to_int(value->bounds().bottom)-1;
    int westing = fixed_to_int(value->bounds().left);
    int easting = fixed_to_int(value->bounds().right)+1;


    // cout << "Entity " << westing << "W, " << northing << "N, " << easting << "E, " << southing << "S" <<endl;
    // determine which entities this belongs to
    for (int lat = northing; lat > southing; lat--)
      {
      for (int lng = westing; lng < easting; lng++)
        {
#ifdef _DEBUG
//        cout << "Process " << lat << ", " << lng << endl;
//        if (num_poly > 5)
//          break;
#endif
        // TODO: what about strings?

        // Build the metafile that will be used
        // and if false then no metafile generated so skip the entity
        if (!value->process_pointset(lat, lng))
          continue;

        int lat_index = 90 - lat;      // lattude is 90...-90
        // but index stored as north-up
        int lng_index = correct_lng(lng) + 180;     // longitude is -180...180

        entities_t& list = _spatial_index[lat_index][lng_index];

        // cout << "Add entity to cell " << lat << ", " << lng << endl;
        if (find(list.begin(), list.end(), value) == list.end())
          list.push_back(value);
        has_polygon = true;

//#ifdef _DEBUG
//        ++num_poly;
//#endif
        }

//#ifdef _DEBUG
//      if (++num_poly > 5)
//        break;
//#endif
      }
    // if the entity has at least 1 generated polygon then add
    // it to the cache of entities to be stored
    if (has_polygon)
      _entities.push_back(value);
    }

  return has_polygon;
  }

uint32_t kotuku::geometry_db::find_or_allocate_string(const string& value)
  {
  if (value.length() == 0)
    return 0;

  // NOT correct...
  string_map_t::const_iterator it = _string_index.find(value);

  if (it != _string_index.end())
    return it->second;             // existing string

  uint32_t result = (uint32_t)_packed_strings.size();

  _string_index.insert({ value, result });

  // add the definition to the packed strings at the end of the header
  for (size_t i = 0; i <= value.length(); i++)
    _packed_strings.push_back(value.c_str()[i]);

  return result;
  }

ofstream& kotuku::geometry_db::serialize(ofstream& os)
  {
  // calculate the size of the spatial entity lists
  uint32_t num_index_cells = _lng_extent * _lat_extent;
  uint32_t num_index_list_cells = 0;
  uint32_t num_entities = 0;

  // in the class
// write the geography_container_t header

  cout << "Write database header " << endl;

  geospatial_container_header_t hdr;
  memset(&hdr, 0, sizeof(geospatial_container_header_t));

  hdr.base.magic = DB_MAGIC;
  hdr.base.length = sizeof(geospatial_container_header_t);
  hdr.base.db_type = db_spatial;
  hdr.base.content_type = _content_type;
  hdr.base.effective_from = _effective_from;
  hdr.base.effective_to = _effective_to;
  strncpy(hdr.base.name, _name.c_str(), 31);
  hdr.base.bounds = _bounds;
  hdr.tiles_lat = _lat_extent;
  hdr.tiles_lng = _lng_extent;
  hdr.base.max_scale = _max_scale;
  hdr.base.min_scale = _min_scale;
  hdr.tiles_lng = _lng_extent;
  hdr.tiles_lat = _lat_extent;
  hdr.num_index_lists = num_index_list_cells;
  hdr.num_entities = (uint32_t)_entities.size();

  os.write(reinterpret_cast<const char*>(&hdr), sizeof(geospatial_container_header_t));

  // write the spatial index next.  Initially fill with 0
  streampos index_pos = os.tellp();
  uint32_t file_pos =  ((uint32_t)index_pos) + (num_index_cells << 3);    // incremented as each entity stored

  // step 2: write the spatial index, and where there is a
  // list of entities, store the offset and length of the
  // entity pointer list.
  for (size_t lat = 0; lat < _lat_extent; lat++)
    for (size_t lng = 0; lng < _lng_extent; lng++)
      {
      // walk the spatial index.
      // if index == 0 (no entities) then emit a 0,0
      // otherwise if the count of entities == 0 then emit
      // a 0,0 as this is post-processed
      // otherise calc an offset position to the start of the list
      const entities_t& entities = _spatial_index[lat][lng];
      // offset to the lists
      // store the index, then number of cells
      write_uint32(os, (uint32_t)(entities.size() > 0 ? ((uint32_t)file_pos) : 0));
      write_uint32(os, (uint32_t)entities.size());

      // and one more list  
      // these are stored as a spatial ID so the code can get
      // the type early, rather than having to load the entity
      file_pos += ((streampos)entities.size() * sizeof(spatial_entity_id_t));
      }

  streampos index_list_pos = os.tellp();

  // Step 3: write the index lists as 0's first to reserve space in the file.
  for (size_t lat = 0; lat < _lat_extent; lat++)
    for (size_t lng = 0; lng < _lng_extent; lng++)
      {
      const entities_t& entities = _spatial_index[lat][lng];

      for (size_t i = 0; i < entities.size(); i++)
        {
        // write 5 uint32's
        write_uint32(os, 0);
        write_uint32(os, 0);
        write_uint32(os, 0);
        write_uint32(os, 0);
        write_uint32(os, 0);
        }
      }

  // the file should now be where the entities are to be stored

  cout << "Store entities" << endl;

  int num_processed = 0;

  // Step 4: write the entity serializations to the file.
  for (entities_t::const_iterator it = _entities.begin(); it != _entities.end(); it++)
    {
    // write to the stream
    (*it)->serialize(os);

    num_processed++;
    // cout << "Stored " << num_processed++ << "\r";
    }

  // this is end of the file
  streampos file_size = os.tellp();

  //  go back to the spatial index
  os.seekp(index_list_pos);

  // we are back to the lists 
  cout << "Write spatial index lists" << endl;

  uint32_t num_lists = 0;
  // Step 5: write the index lists that were reserved in step 3
  // this is a list of index offsets to a pyhysical entity
  for (int lat = 0; lat < _lat_extent; lat++)
    {
    for (int lng = 0; lng < _lng_extent; lng++)
      {
      const entities_t& entities = _spatial_index[lat][lng];

      // write all of the entity file offsets from the list.
      for (entities_t::const_iterator it = entities.begin(); it != entities.end(); it++)
        {
        const spatial_entity* entity = *it;

        // these are stored in the same order
        //spatial_entity_id_t id;
        //id.type = entity->db_type();
        //id.offset = (uint32_t)entity->file_offset();
        //id.length = entity->entity_size();
        //id.graphics_offset = (uint32_t)entity->graphics_list(90 - lat, lng -180);
        //id.graphics_length = entity->graphics_list_size(90 - lat, lng -180);
        uint32_t graphics_length = entity->graphics_list_size(90 - lat, lng - 180);
        write_uint32(os, entity->db_type());
        write_uint32(os, (uint32_t)entity->file_offset());
        write_uint32(os, entity->entity_size());
        write_uint32(os, (uint32_t)entity->graphics_list(90 - lat, lng - 180));
        write_uint32(os, graphics_length);

        num_lists ++;

        // os.write(reinterpret_cast<char *>(&id), sizeof(spatial_entity_id_t));
        }
      }
    }

  // go to end of file
  os.seekp(file_size);

  // Step 1: write the strings
  if (_packed_strings.size() > 0)
    {
    hdr.base.string_table = (uint32_t)file_size;
    hdr.base.string_table_length = (uint32_t)_packed_strings.size();

    os.write(_packed_strings.data(), _packed_strings.size());
    }

  file_size = os.tellp();
  os.seekp(0);
  // and re-write the header
  os.write(reinterpret_cast<const char*>(&hdr), sizeof(geospatial_container_header_t));

  os.seekp(file_size);      // back to eof
  os.flush();
  os.close();


  cout << "Stored " << num_processed << " entities " << endl;
  cout << "File size " << file_size << endl;
  cout << "Contains " << num_lists << " graphics paths" << endl;
  cout << "Strings " << hdr.base.string_table_length << endl;

  return os;
  }

#define PtInside 0
#define PtLeft  1
#define PtRight 2
#define PtBottom 4
#define PtTop 8

// this is for spatial coordinates
static int calculate_out_code(fixed_t x, fixed_t y, const spatial_rect_t& r)
  {
  int code = PtInside;

  if (x < r.left)
    code |= PtLeft;

  if (x > r.right)
    code |= PtRight;

  if (y > r.top)
    code |= PtTop;

  if (y < r.bottom)
    code |= PtBottom;

  return code;
  }

static bool calculate_intersection(const spatial_rect_t& r, const spatial_point_t& p1, const spatial_point_t& p2, int out_code, spatial_point_t& result)
  {
  fixed_t dx = sub_fixed(p2.latlng.lng, p1.latlng.lng);
  fixed_t dy = sub_fixed(p2.latlng.lat, p1.latlng.lat);

  fixed_t slopeY = div_fixed(dx, dy); // slope to use for possibly-vertical lines
  fixed_t slopeX = div_fixed(dy, dx); // slope to use for possibly-horizontal lines

  /*

    if (clipTo.HasFlag(OutCode.Top)) {
        return new PointF(
            p1.X + slopeY * (r.Top - p1.Y),
            r.Top
            );
    }
  */
  if (out_code & PtTop)
    {
    result.latlng.lng = add_fixed(p1.latlng.lng, mul_fixed(slopeY, sub_fixed(r.top, p1.latlng.lat)));
    result.latlng.lat = r.top;

    return true;
    }
/*
    if (clipTo.HasFlag(OutCode.Bottom)) {
        return new PointF(
            p1.X + slopeY * (r.Bottom - p1.Y),
            r.Bottom
            );
    }
*/
  if (out_code & PtBottom)
    {
    result.latlng.lng = add_fixed(p1.latlng.lng, mul_fixed(slopeY, sub_fixed(r.bottom, p1.latlng.lat)));
    result.latlng.lat = r.bottom;

    return true;
    }
/*
    if (clipTo.HasFlag(OutCode.Right)) {
        return new PointF(
            r.Right,
            p1.Y + slopeX * (r.Right - p1.X)
            );
    }
*/
  if (out_code & PtRight)
    {
    result.latlng.lng = r.right;
    result.latlng.lat = add_fixed(p1.latlng.lat, mul_fixed(slopeX, sub_fixed(r.right, p1.latlng.lng)));

    return true;
    }
    /*
    if (clipTo.HasFlag(OutCode.Left)) {
        return new PointF(
            r.Left,
            p1.Y + slopeX * (r.Left - p1.X)
            );
    */
  if (out_code & PtLeft)
    {
    result.latlng.lng = r.left;
    result.latlng.lat = add_fixed(p1.latlng.lat, mul_fixed(slopeX, sub_fixed(r.left, p1.latlng.lng)));

    return true;
    }

  return false;
  }

/**
 * @brief Clip to a rectangle
 * @param rect Clipping bounds
 * @param p1    First point
 * @param p2    Second point
 * @return true if p1->p2 are points to add to the clipped area
*/
bool clip_to(const spatial_rect_t& rect, spatial_point_t& p1, spatial_point_t& p2)
  {
  // classify the endpoints of the line
  int outCodeP1 = calculate_out_code(p1.latlng.lng, p1.latlng.lat, rect);
  int outCodeP2 = calculate_out_code(p2.latlng.lng, p2.latlng.lat, rect);

  while (true)
    { // should only iterate twice, at most
      // Case 1:
      // both endpoints are within the clipping region
    if ((outCodeP1 | outCodeP2) == PtInside)
      return true;

    // Case 2:
    // both endpoints share an excluded region, impossible for a line between them to be within the clipping region
    if ((outCodeP1 & outCodeP2) != 0)
      return false;

    // Case 3:
    // The endpoints are in different regions, and the segment is partially within the clipping rectangle

    // Select one of the endpoints outside the clipping rectangle
    int outCode = outCodeP1 != PtInside ? outCodeP1 : outCodeP2;

    spatial_point_t p;
    p.elevation = 0;

    // calculate the intersection of the line with the clipping rectangle
    if (!calculate_intersection(rect, p1, p2, outCode, p))
      return false;

    // update the point after clipping and recalculate outcode
    if (outCode == outCodeP1)
      {
      p1 = p;
      outCodeP1 = calculate_out_code(p1.latlng.lng, p1.latlng.lat, rect);
      }
    else
      {
      p2 = p;
      outCodeP2 = calculate_out_code(p2.latlng.lng, p2.latlng.lat, rect);
      }
    }

  return false;
  }
