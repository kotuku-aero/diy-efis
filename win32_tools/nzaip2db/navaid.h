#ifndef __navaid_h__
#define __navaid_h__

#include <string>
#include <map>

// get the header definitions
#include "../../libs/atomdb/db.h"
#include "../geometry_db/db_geometry.h"
#include "../../libs/graviton/schema.h"

namespace kotuku {
  class navaid : public spatial_point {
  public:
    navaid(geometry_db* db);

    /*
        "type": "NDB",
        "designator": "FA",
        "name": "FALEOLO",
        "frequency": "270",
        "frequencyuom": "KHZ",
        "dmefrequency": null,
        "dmefrequencyuom": null,
        "aerodrome": null,
        "morse": "..-.  .-"
    */


    };

  }

#endif
