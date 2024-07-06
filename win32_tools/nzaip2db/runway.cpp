#include "runway.h"
#include "util.h"
#include "aerodrome.h"

#include "../util/string.h"

using namespace kotuku;

runway::runway(geometry_db* db)
  : spatial_polygon(db, SPATIAL_POLYGON | ast_runway)
  {
  }

uint32_t runway::serialize_entity(ofstream& os)
  {
  runway_t entity;
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

spatial_entity* runway::parse(geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  // first find the aerodrome.
  string aerodrome_name = get_string_from_json(properties, "aerodrome");

  entities_t::const_iterator it = std::find_if(spatial_db->entities().begin(), spatial_db->entities().end(),
    [aerodrome_name](const spatial_entity* entity)
    {
    const aerodrome* ad = dynamic_cast<const aerodrome*>(entity);
    if (ad == nullptr)
      return false;

    return ad->designator() == aerodrome_name;
    });

  if (it == spatial_db->entities().end())
    {
    cerr << "Cannot find aerodrome " << aerodrome_name << endl;
    return nullptr;
    }

  aerodrome *ad = const_cast<aerodrome*>(dynamic_cast<const aerodrome*>(*it));

  runway* entity = new runway(spatial_db);

  entity->_designator = get_string_from_json(properties, "designator");
  entity->_surface = get_string_from_json(properties, "surface");
  entity->_length = get_int_from_json(properties, "length");
  entity->_width = get_float_from_json(properties, "width");

  // pre-process the information of the runway
  // the designator is in the form:
  //
  // RWY-02/20 GR
  // RWY-02/20
  // RWY-11/29

  string::size_type end1 = entity->_designator.find_first_of('-');
  string::size_type end2 = entity->_designator.find_first_of('/');
  string::size_type end3 = entity->_designator.find_first_of(' ');

  if (end1 == string::npos)
    {
    cerr << "The designator " << entity->_designator << " for " << aerodrome_name << " is not properly formed" << endl;
    }
  else
    {
    if (end2 == string::npos)
      {
      if(end3 == string::npos)
        // one way strip
        end2 = entity->_designator.length();
      else
        end2 = end3;
      }
    else if (end3 == string::npos)
      end3 = entity->_designator.length();

    entity->end1.designator = entity->_designator.substr(end1 + 1, end2 - end1 - 1);

    if(end2 != entity->_designator.length())
      entity->end2.designator = entity->_designator.substr(end2 + 1, end3 - end2 - 1);
    }

  entity->_aerodrome = ad;
  // save this runway
  entity->_aerodrome->add_runway(entity);

  return entity;
  }

void runway::parse_label(const rapidjson::Value& label)
  {
  if (!label.HasMember("properties"))
    {
    cerr << "Cannot find the properties of a runway label" << endl;
    return;
    }

  const rapidjson::Value& properties = label["properties"];

  string label_designator = get_string_from_json(properties, "designator");

  // remove the GRASS directive
  if(label_designator.ends_with('G'))
    label_designator = label_designator.substr(0, label_designator.length()-1);
  else if(label_designator.ends_with("GR"))
    label_designator = label_designator.substr(0, label_designator.length() - 2);

  trim(label_designator);

  runway_end *end = nullptr;
  if(label_designator == end1.designator)
    end = &end1;
  else if(label_designator == end2.designator)
    end = &end2;
  else
    {
    cerr << "Cannot match the runway end designator " << label_designator << " to the parsed ends" << endl;
    return;
    }

  end->circuit_direction = get_string_from_json(properties, "circuitdirection");
  end->azumith = get_float_from_json(properties, "azumith");

  if (!label.HasMember("geometry"))
    {
    cerr << "Cannot find the runway label position" << endl;
    return;
    }

  const rapidjson::Value& geometry = label["geometry"];
  if (get_string_from_json(geometry, "type") != "Point")
    {
    cerr << "Unexpected type for a label position" << endl;
    return;
    }

  const rapidjson::Value& coordinates = geometry["coordinates"];
  if (!coordinates.IsArray())
    {
    cerr << "Coordinates must be an array" << endl;
    }

  end->position = get_coordinates(coordinates);
  end->position.elevation = (int16_t) get_int_from_json(properties, "elevation");
  }