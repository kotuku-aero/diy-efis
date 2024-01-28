#include "util.h"


ymd_t kotuku::get_ymd_from_json(const rapidjson::Value& src, const char* prop)
  {
  if (!src.HasMember(prop))
    return ymd_t{ .year = 0, .month = 0, .day = 0 };

  string str = src[prop].GetString();

  ymd_t result;

  // 2023-10-05
  result.year = stoi(str.substr(0, 4));
  result.month = stoi(str.substr(5, 2));
  result.day = stoi(str.substr(8, 2));

  return result;
  }

e_controlled_airspace_class kotuku::get_class_from_json(const rapidjson::Value& src)
  {
  if (!src.HasMember("class"))
    return act_unknown;

  string str = src["class"].GetString();

  if (str == "A")
    return act_A;

  if (str == "B")
    return act_B;

  if (str == "C")
    return act_C;

  if (str == "D")
    return act_D;

  if (str == "E")
    return act_E;

  if (str == "F")
    return act_F;

  if (str == "G")
    return act_G;

  cerr << "Unknown airspace class " << str << endl;
  return act_unknown;
  }

spatial_point_t kotuku::get_coordinates(const rapidjson::Value& src)
  {
  spatial_point_t pt;

  // is in form:
  /*
        "geometry": {
        "type": "Point",
        "coordinates": [
          174.17860009,
          -39.000444393
        ]
      }
*/
  pt.latlng.lng = float_to_fixed(src[0].GetFloat());
  pt.latlng.lat = float_to_fixed(src[1].GetFloat());

  if(src.GetArray().Size() == 3)
    pt.elevation = (int16_t) src[2].GetInt();
  else
    pt.elevation = 0;

  return pt;
  }