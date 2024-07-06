#ifndef __runway_h__
#define __runway_h__

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
  class aerodrome;

  /**
   * @brief There are allways 2 runways created for an aerodrome.
  */
  class runway : public spatial_polygon {
  public:
    runway(geometry_db* db);

    uint32_t serialize_entity(ofstream& os) override;

    static spatial_entity* parse(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties);

    void parse_label(const rapidjson::Value& label);

    const string &designator() const { return _designator; }
  private:
    aerodrome *_aerodrome;
    string _designator;
    string _surface;
    int _length;
    float _width;
    float _azumith;
    int _elevation;

    struct runway_end
      {
      string designator;
      spatial_point_t position;
      float azumith;
      string circuit_direction;
      };

    runway_end end1;
    runway_end end2;
    };
  }
#endif

