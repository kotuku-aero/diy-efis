#ifndef __aerodrome_h__
#define __aerodrome_h__

#include "runway.h"

namespace kotuku {

  typedef vector<runway *> runways;

  class aerodrome : public spatial_point {
  public:
    aerodrome(geometry_db* db);

    uint32_t serialize_entity(ofstream& os) override;

    static spatial_entity* parse(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties);

    void add_runway(runway *rwy) { _runways.push_back(rwy); }

    const string &designator() const { return _designator; }

    runways::iterator rwy_begin() { return _runways.begin(); }
    runways::iterator rwy_end() { return _runways.end(); }
  private:
    string _designator;
    string _flightrule;

    int _azumith;
    int _length;       // SHOULD BE RUNWAYS
    int _elevation;
    int _magvar;

    ymd_t _effective_date;

    runways _runways;
    };

  };

#endif