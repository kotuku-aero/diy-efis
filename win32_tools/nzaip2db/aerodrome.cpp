#include "aerodrome.h"

#include "util.h"

using namespace kotuku;

aerodrome::aerodrome(geometry_db* db)
  : spatial_point(db, SPATIAL_POINT | ast_aerodrome)
  {
  }

uint32_t aerodrome::serialize_entity(ofstream& os)
  {
  airspace_t entity;
  memset(&entity, 0, sizeof(entity));
  size_t i = 0;
  for (; i < _designator.size() && i < 6; i++)
    entity.base.identifier[i] = _designator[i];

  while (i++ < 6)
    entity.base.identifier[i] = ' ';


  // only stream the actual data
  uint32_t len = sizeof(entity) - sizeof(spatial_entity_hdr_t);
  os.write(reinterpret_cast<char*>(&entity) + sizeof(spatial_entity_hdr_t), len);
  // return only the extra size.
  return len;
  }

spatial_entity* aerodrome::parse(geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  aerodrome* ad = new aerodrome(spatial_db);

  ad->_designator = get_string_from_json(properties, "designator");
  ad->_flightrule = get_string_from_json(properties, "flightrule");
  ad->_azumith = get_int_from_json(properties, "azumith");
  ad->_length = get_int_from_json(properties, "length");
  ad->_elevation = get_int_from_json(properties, "elevation");
  ad->_magvar = get_int_from_json(properties, "magvar");
  ad->_effective_date = get_ymd_from_json(properties, "effectivedate");

  return ad;
  }
