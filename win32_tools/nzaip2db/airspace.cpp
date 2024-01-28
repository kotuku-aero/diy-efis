#include "airspace.h"
#include "../clipper2/clipper.h"
#include "util.h"

using namespace kotuku;

airspace::airspace(geometry_db* db, e_entity_type type)
  : spatial_polyline(db, SPATIAL_POLYLINE | type)
  {
  }

uint32_t airspace::serialize_entity(ofstream& os)
  {
  // encode the class into the high 24bits
  add_decoration_to_type(_airspace_class << 24);

  // write the icao_id_t
  airspace_t entity;
  memset(&entity, 0, sizeof(entity));
  size_t i = 0;
  for (; i < _designator.size() && i < 6; i++)
    entity.base.identifier[i] = _designator[i];

  while (i++ < 6)
    entity.base.identifier[i] = ' ';


  // calculate the center point of the entity
  // used to print labels on the map
  // the entity can have multiple paths, not sure about this
  // for now just use the first
  if (!_paths.empty())
    {
    double area = 0;
    double x = 0;
    double y = 0;

    points::size_type n = _paths[0].size();

    for (points::size_type i = 0; i < n; i++)
      {
      spatial_point_t p1 = _paths[0][i];
      spatial_point_t p2 = _paths[0][(i+1) % n];

      double x1 = fixed_to_float(p1.latlng.lng);
      double y1 = fixed_to_float(p1.latlng.lat);
      double x2 = fixed_to_float(p2.latlng.lng);
      double y2 = fixed_to_float(p2.latlng.lat);

      double partial = (y2 * x1) - (y1 * x2);

      area += partial;

      double x3 = (x1 + x2) * partial;
      double y3 = (y1 + y2) * partial;
      x += x3;
      y += y3;
      }

    area *= 0.5f;

    x = x /(area * 6.0f);
    y = y /(area * 6.0f);

    latlng_t pt;
    pt.lng = float_to_fixed(x);
    pt.lat = float_to_fixed(y);

    center(pt);
    entity.area = float_to_fixed(area);
    }

  entity.base.name.offset = db()->find_or_allocate_string(_name);
  entity.base.name.length = _name.length();
  entity.base.effective_date = _effective_date;
  entity.base.base.center_pt = center();

  entity.classification = _airspace_class;
  entity.airspace_active.offset = db()->find_or_allocate_string(_active_times);
  entity.airspace_active.length = _active_times.length();

  entity.upper_limit = _upper_limit;
  entity.lower_limit = _lower_limit;
  entity.transponder_mandatory = _transponder_mandatory;

  if (!_note.empty())
    {
    entity.notes.offset = db()->find_or_allocate_string(_note);
    entity.notes.length = _note.length();
    }

  if (!_csabbrev.empty())
    {
    entity.csabbrev.offset = db()->find_or_allocate_string(_csabbrev);
    entity.csabbrev.length = _csabbrev.length();
    }

  if (!_frequency.empty())
    {
    entity.frequency.offset = db()->find_or_allocate_string(_frequency);
    entity.frequency.length = _frequency.length();
    }

  // only stream the actual data
  uint32_t len = sizeof(entity) - sizeof(spatial_entity_hdr_t);
  os.write(reinterpret_cast<char*>(&entity) + sizeof(spatial_entity_hdr_t), len);
  // return only the extra size.
  return len;
  }


spatial_entity* airspace::parse(geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  e_entity_type type;

  string str = properties["type"].GetString();

  if (str == "GAA" || str == "GAA_PART")
    type = ast_general_aviation;
  else if (str == "MBZ" || str == "MBZ_PART")
    type = ast_mandatory_broadcast_zone;
  else if (str == "CFZ")
    type = ast_common_frequency_zone;
  else if (str == "CTR" || str == "CTR_SECTOR")
    type = ast_ctr;
  else if (str == "CTA")
    type = ast_cta;
  else if (str == "D")
    type = ast_danger_area;
  else if (str == "LFZ")
    type = ast_low_flying_zone;
  else if (str == "R")
    type = ast_restricted_area;
  else if (str == "MOA")
    type = ast_millitary_zone;
  else if (str == "T")
    type = ast_transit_lane;
  else if (str == "VHZ")
    type = ast_volcanic_hazard;
  else if (str == "FISCOM")
    type = ast_fiscom;
  else if (str == "FIR")
    type = ast_fir;

  airspace* entity = new airspace(spatial_db, type);

  entity->_name = get_string_from_json(properties, "name");
  entity->_note = get_string_from_json(properties, "note");
  entity->_airspace_class = get_class_from_json(properties);
  entity->_designator = get_string_from_json(properties, "desiglabel");
  entity->_lower_limit = get_int_from_json(properties, "lowerfilter");
  entity->_upper_limit = get_int_from_json(properties, "upperfilter");
  entity->_active_times = get_string_from_json(properties, "workinghours");
  entity->_effective_date = get_ymd_from_json(properties, "effectivedate");
  entity->_csabbrev = get_string_from_json(properties, "csabbrev");
  entity->_frequency = get_string_from_json(properties, "frequency");

  return entity;
  }
