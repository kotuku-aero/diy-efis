#include "db_geometry.h"
#include <algorithm>
#include <utility>

kotuku::spatial_entity::spatial_entity(geometry_db* db, spatial_entity_type type)
  : _db(db)
  {
  _base.type = type;
  }

uint32_t kotuku::spatial_entity::serialize(ofstream& os)
  {
  _file_offset = os.tellp();
  
  // emit the base data
  _base.size = sizeof(spatial_entity_hdr_t);

  // these were written above
  os.write(reinterpret_cast<const char *>(&_base), sizeof(spatial_entity_hdr_t));

  _base.entity_length = serialize_entity(os) + sizeof(spatial_entity_hdr_t);
  _base.size += _base.entity_length;

  // write the rest of the data that forms the type
  _base.strings_length = serialize_strings(os);       // send the indexed strings
  _base.size += _base.strings_length;

  _base.paths_length = 0;
  // to speed up rendering the graphics what follows is a list of
  // pointsets
  for (graphics::const_iterator it = _graphics.begin(); it != _graphics.end(); it++)
    {
    streampos graphics_pos = os.tellp();      // where this is
    uint32_t graphics_len = 0;

    // write the number of points/ line / polygon points
    graphics_len += write_uint32(os, it->second.size());

    for (polylines::const_iterator poly_it = it->second.begin(); poly_it != it->second.end(); poly_it++)
      {
      graphics_len += write_uint32(os, poly_it->size());    // number of spatial points in polyline
      for (points::const_iterator pt_it = poly_it->begin(); pt_it != poly_it->end(); pt_it++)
        {
        graphics_len += write_uint32(os, pt_it->latlng.lat);
        graphics_len += write_uint32(os, pt_it->latlng.lng);
        graphics_len += write_uint16(os, pt_it->elevation);
        }
      }

    _base.paths_length += graphics_len;         // entity a bit bigger now

    // and store the latlng info for the tile.
    _tiles.insert( { it->first, { graphics_pos, graphics_len } } );
    }

  _base.size += _base.paths_length;

  streampos pos = os.tellp();
  // seek back to start
  os.seekp(_file_offset);
  // rewrite the header as all of the lengths are updated
  os.write(reinterpret_cast<const char*>(&_base), sizeof(spatial_entity_hdr_t));
  os.seekp(pos);            // and restore the file pointer

  return _base.size;
  }

uint32_t kotuku::spatial_entity::add_string(const string& value, const latlng_t& center)
  {
  return 0;
  }

uint32_t kotuku::spatial_entity::serialize_strings(ofstream& os)const
  {
  uint32_t strings_size = 0;
  for (strings_t::const_iterator it = _strings.begin(); it != _strings.end(); it++)
    {
    // write in serialization order
    os << (fixed_t)it->position.lng;
    os << (fixed_t)it->position.lat;

    // stored as length of c_str() then offset
    os << it->string_index;
    os << (uint32_t)it->value.length();
    }

  // 4 bytes each
  return (uint32_t)( _strings.size() * sizeof(spatial_string_t));
  }

streampos kotuku::spatial_entity::graphics_list(int lat, int lng) const
  {
  latlng key = make_pair(lat, lng);

  tiles::const_iterator it = _tiles.find(key);

  if(it != _tiles.end())
    return it->second.first;

  return 0;
  }

uint32_t kotuku::spatial_entity::graphics_list_size(int lat, int lng) const
  {
  latlng key = make_pair(lat, lng);

  tiles::const_iterator it = _tiles.find(key);

  if (it != _tiles.end())
    return it->second.second;

  return 0;
  }
