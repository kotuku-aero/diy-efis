#ifndef __util_h__
#define __util_h__

#include <string>
#include "../util/rapidjson/rapidjson.h"
#include "../util/rapidjson/document.h"
#include "../util/rapidjson/istreamwrapper.h"
// get the header definitions
#include "../../libs/atomdb/db.h"
#include "../geometry_db/db_geometry.h"
#include "../../libs/graviton/schema.h"

namespace kotuku {

  inline std::string get_string_from_json(const rapidjson::Value& src, const char* prop)
    {
    if (src.HasMember(prop))
      {
      const rapidjson::Value& value = src[prop];

      if(value.IsString())
        return value.GetString();
      }

    return "";
    }

  inline int get_int_from_json(const rapidjson::Value& src, const char* prop)
    {
    if (!src.HasMember(prop))
      return 0;

    const rapidjson::Value& value = src[prop];
    if (value.IsNull())
      return 0;

    if(value.IsInt())
      return value.GetInt();

    if (value.IsString())
      {
      string str = value.GetString();
      
      return stoi(str);
      }

    return 0;
    }

  inline float get_float_from_json(const rapidjson::Value& src, const char* prop)
    {
    if(!src.HasMember(prop))
      return 0;

    const rapidjson::Value& value = src[prop];
    if(value.IsNull())
      return 0;

    return value.GetFloat();
    }

  extern ymd_t get_ymd_from_json(const rapidjson::Value& src, const char* prop);
  extern e_controlled_airspace_class get_class_from_json(const rapidjson::Value& src);
  extern spatial_point_t get_coordinates(const rapidjson::Value& src);
  }

#endif