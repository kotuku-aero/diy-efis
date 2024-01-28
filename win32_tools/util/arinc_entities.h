#ifndef __arinc_entities_h__
#define __arinc_entities_h__

#include "line.h"

#include "../../libs/graviton/schema.h"

using namespace std;

namespace kotuku
  {
  typedef map<string, vector<line *> > entities;

  class arinc_entities
    {
    public:
      arinc_entities(schema &schema, const string &filename);

      const string& filename() const { return _filename; }
      int version_number() const { return _version_number; }
      bool is_production() const { return _is_production; }
      int record_count() const { return _record_count; }
      const string &cycle_date() const { return _cycle_date; }
      ymd_t creation_date() const { return _creation_date; }
      const string &data_provider() const { return _data_provider; }
      const vector<string> &headers() const { return _headers; }

    private:
      string _filename;
      int _version_number;
      bool _is_production;
      int _record_count;
      string _cycle_date;
      ymd_t _creation_date;
      string _data_provider;
      vector<string> _headers;

      entities _entities;
    };
  }
#endif
