#ifndef __airspace_h__
#define __airspace_h__

#include <string>
#include <map>

// get the header definitions
#include "../../libs/atomdb/db.h"
#include "../geometry_db/db_geometry.h"
#include "../../libs/graviton/schema.h"
#include "../util/rapidjson/rapidjson.h"
#include "../util/rapidjson/document.h"
#include "../util/rapidjson/istreamwrapper.h"

using namespace std;

namespace kotuku {
  class airspace : public spatial_polyline {
    public:
      airspace(geometry_db* db, e_entity_type type);

      static spatial_entity *parse(geometry_db* spatial_db, const rapidjson::Value& properties);

      uint32_t serialize_entity(ofstream& os) override;
      //bool process_pointset(int lat, int lng) override;

      const string &designator() const { return _designator; }
    private:
      string _designator;
      string _name;
      string _note;
      string _activity;
      uint32_t _entity_type;
      ymd_t _effective_date;
      bool _transponder_mandatory;
      e_controlled_airspace_class _airspace_class;
      string _active_times;

      // special types
      string _csabbrev;
      string _frequency;

      // are in ft
      int _lower_limit;
      int _upper_limit;
    };

  };

#endif
