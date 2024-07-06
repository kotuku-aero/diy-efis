#ifndef __db_geometry_h__
#define __db_geometry_h__

#include <iostream>
#include <fstream>      // filebuf
#include <format>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include <new>

// get the header definitions
// atom db
#include "../../libs/atomdb/db_priv.h"

using namespace std::filesystem;
using namespace std;

namespace kotuku {
  typedef vector<::spatial_point_t> points;
  typedef vector<points> polylines;
  typedef vector<uint32_t> index_t;
  typedef pair<int, int> latlng;
  // for a pointset there is only 1 element.
  typedef map<latlng, polylines> graphics;
  typedef map<latlng, pair<streampos, uint32_t> > tiles;

  struct db_string_t {
    latlng_t position;
    uint32_t string_index;    // index into the string_def_t
    string value;
    };

  typedef vector<db_string_t> strings_t;

  class spatial_entity;
  class spatial_linestring;
  class spatial_point;

  typedef vector<spatial_entity*> entities_t;

  inline uint32_t write_uint32(ofstream& os, uint32_t val)
    {
    os.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
    return sizeof(uint32_t);
    }

  inline uint32_t write_uint16(ofstream& os, uint16_t val)
    {
    os.write(reinterpret_cast<const char*>(&val), sizeof(uint16_t));
    return sizeof(uint16_t);
    }

  inline uint32_t write_int32(ofstream& os, int32_t val)
    {
    os.write(reinterpret_cast<const char*>(&val), sizeof(int32_t));
    return sizeof(int32_t);
    }

  inline uint32_t write_int16(ofstream& os, int16_t val)
    {
    os.write(reinterpret_cast<const char*>(&val), sizeof(int16_t));
    return sizeof(int16_t);
    }

  class geometry_db {
  public:
    geometry_db(uint32_t type, const spatial_rect_t& extents);
    ~geometry_db();

    const path &src_path() const { return _src_path; }
    void src_path(const path &value) { _src_path = value; }

    const ymd_t& effective_from() const { return _effective_from; }
    void effective_from(const ymd_t& value) { _effective_from = value; }

    const ymd_t& effective_to() const { return _effective_to; }
    void effective_to(const ymd_t& value) { _effective_to = value; }

    const string& name() const { return _name; }
    void name(const string& value);

    bool add_spatial_entity(spatial_linestring* value);
    bool add_spatial_entity(spatial_point* value);

    uint16_t min_scale() const { return _min_scale; }
    void min_scale(uint16_t value) { _min_scale = value; }

    uint16_t max_scale() const { return _max_scale; }
    void max_scale(uint16_t value) { _max_scale = value; }

    const spatial_rect_t bounds() const { return _bounds; }
    uint32_t extent_lat() const { return _lat_extent; }
    uint32_t extent_lng() const { return _lng_extent; }

    /**
     * @brief return true if the rectange intersects the database.
     * @param rect rect to test
     * @return
    */
    bool intersects(const spatial_rect_t& rect) const;

    uint32_t find_or_allocate_string(const string& value);

    ofstream& serialize(ofstream& os);

    const entities_t& entities() const { return _entities; }

    entities_t::iterator begin() { return _entities.begin();  }
    entities_t::const_iterator begin() const { return _entities.begin();  }

    entities_t::iterator end() { return _entities.end();  }
    entities_t::const_iterator end() const { return _entities.end();  }
  private:
    geometry_db(const geometry_db&);
    geometry_db& operator =(const geometry_db&);

    uint32_t offset(int lng, int lat) const
      {
      return (uint32_t)(((_lat_extent - (lat + 0 - fixed_to_int(_bounds.bottom))) * _lng_extent) + (lng + (0 - fixed_to_int(_bounds.left))));
      }

    uint32_t _content_type;
    uint16_t _cycle;
    ymd_t _effective_from;          // effective from
    ymd_t _effective_to;            // when expires
    string _name;
    spatial_rect_t _bounds;
    uint16_t _min_scale;
    uint16_t _max_scale;
    uint32_t _lat_extent;         // number of tiles in Y axis
    uint32_t _lng_extent;         // number of tiles in X axis

    path _src_path;

    vector<char> _packed_strings;

    // index of known strings to existing string definitions.
    typedef map<string, uint32_t> string_map_t;
    string_map_t _string_index;

    entities_t _entities;

    // this is always 14000 entries long in all
    // spatial databases.  Most of which may be un-used
    // but quicker processing for a small extra storage overhead

    entities_t _spatial_index[180][360];
    };
  /**
   * @brief This is a builder class, allows for the serialization
   * of a type
  */
  class spatial_entity {
  public:
    spatial_entity_type db_type() const { return _base.type; }
    void add_decoration_to_type(uint32_t decoration) { _base.type |= decoration; }

    // add a string to the list of strings stored
    uint32_t add_string(const string& value, const latlng_t& center);
    // get a string based on the index
    const db_string_t& get_string(uint32_t index) const { return _strings[index]; }
    // number of strings added
    uint32_t num_strings() const { return (uint32_t)_strings.size(); }
    // bounds of this entity
    const spatial_rect_t& bounds() const { return _base.bounding_box; }
    // geographic center point
    const latlng_t center() const { return _base.center_pt; }
    void center(const latlng_t &pt) { _base.center_pt = pt; }

    const geometry_db* db() const { return _db; }
    geometry_db* db() { return _db; }

    // return number of bytes written
    virtual uint32_t serialize(ofstream& os);
    /**
     * @brief Determine if the actual graphics of the entity overlap the rectange
     * @param rect rectangle to test
     * @return true if any of the graphics is contained in the rectangle
    */
    virtual bool include_in_database(const spatial_rect_t& rect) const = 0;

    void add_attribute(const string& name, const string& value) { _attributes.push_back({ name, value }); }
    size_t num_attributes() const { return _attributes.size(); }

    const pair<string, string>& get_attribute(size_t index) const { return _attributes[index]; }

    void set_bounds(fixed_t _west, fixed_t _north, fixed_t _east, fixed_t _south)
      {
      _base.bounding_box.left = _west;
      _base.bounding_box.top = _north;
      _base.bounding_box.right = _east;
      _base.bounding_box.bottom = _south;
      }

    void add_path(const points &line) { _paths.push_back(line); }

    streampos file_offset() const { return _file_offset; }
    virtual uint32_t entity_size() const { return _base.entity_length; }

    virtual bool process_pointset(int lat, int lng) = 0;

    virtual streampos graphics_list(int lat, int lng) const;
    virtual uint32_t graphics_list_size(int lat, int lng) const;

    virtual ~spatial_entity() {}
  protected:
    spatial_entity(geometry_db* db, spatial_entity_type type);
    uint32_t serialize_point(ofstream& os, const ::spatial_point_t& pt) const
      {
      uint32_t len = 0;
      len += write_uint32(os, pt.latlng.lat);
      len += write_uint32(os, pt.latlng.lng);
      len += write_int16(os, pt.elevation);

      return len;
      }

    // serialize the entity payload that is stored after the header
    virtual uint32_t serialize_entity(ofstream &os) { return 0; }
    // serialize the strings stored after the entity
    uint32_t serialize_strings(ofstream& os)const;

    polylines _paths;
    graphics _graphics;
    // filled in when the entity has serialized itself, along with
    // the grpahics paths.
    tiles _tiles;
  private:
    spatial_entity_hdr_t _base;
    strings_t _strings;
    geometry_db* _db;
    vector<pair<string, string> > _attributes;
    streampos _file_offset;
    };

  inline bool operator ==(const pair<int, int>& p1, const pair<int, int>& p2)
    {
    return p1.first == p2.first && p1.second == p2.second;
    }

  inline bool operator <(const pair<int, int>& p1, const pair<int, int>& p2)
    {
    return p1.first < p2.first ||
      (p1.first == p2.first && p1.second < p2.second);
    }

  class spatial_point : public spatial_entity {
  public:
    spatial_point(geometry_db* db, uint32_t type = SPATIAL_POINT);

    bool include_in_database(const spatial_rect_t& rect) const;
    bool include_in_index(int lat, int lng) const;
    bool process_pointset(int lat, int lng);
    };

  class spatial_linestring : public spatial_entity {
  public:
    spatial_linestring(geometry_db* db, uint32_t type) : spatial_entity(db, type) {}
    };

  class spatial_polyline : public spatial_linestring {
  public:
    spatial_polyline(geometry_db* db, uint32_t type = SPATIAL_POLYLINE);

    bool include_in_database(const spatial_rect_t& rect) const;
    bool include_in_index(int lat, int lng) const;
    bool process_pointset(int lat, int lng);
    };

  class spatial_polygon : public spatial_linestring {
  public:
    spatial_polygon(geometry_db* db, uint32_t type = SPATIAL_POLYGON);

    bool include_in_database(const spatial_rect_t& rect) const;
    bool include_in_index(int lat, int lng) const;
    bool process_pointset(int lat, int lng);
    };
  }

inline bool operator ==(const spatial_point_t& p1, const spatial_point_t& p2)
  {
  return p1.latlng.lng == p2.latlng.lng && p1.latlng.lat == p2.latlng.lat;
  }

inline bool operator !=(const spatial_point_t& p1, const spatial_point_t& p2)
  {
  return p1.latlng.lng != p2.latlng.lng || p1.latlng.lat != p2.latlng.lat;
  }

inline bool point_in_rect(const spatial_rect_t& rect, const latlng_t& pt)
  {
  return pt.lat > rect.bottom &&
    pt.lat <= rect.top &&
    pt.lng >= rect.left &&
    pt.lng < rect.right;
  }

inline bool intersects(const spatial_rect_t& r1, const spatial_rect_t& r2)
  {
  return r1.left <= r2.right &&
    r1.right >= r2.left &&
    r1.top >= r2.bottom &&
    r1.bottom <= r2.top;
  }

static inline int clip_lat(int lat)
  {
  if (lat > 90)
    return 90;

  if (lat < -89)
    return -89;

  return lat;
  }

static inline int clip_lng(int lng)
  {
  if (lng > 180)
    lng = 180;

  if (lng < -179)
    lng = -179;

  return lng;
  }

extern bool clip_to(const spatial_rect_t& rect,
                    spatial_point_t& p1,
                    spatial_point_t& p2);

#endif
