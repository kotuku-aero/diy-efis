#include "arinc_entities.h"
#include "string.h"

namespace kotuku
  {
  static ymd_t parse_date(const string& str)
    {
    // "dd-MMM-yyyyhh:mm:ss"
    ymd_t result;
    result.day = stoi(str.substr(0, 2));
    result.year = stoi(str.substr(7, 4));

    string mon = str.substr(3, 3);
    if(mon == "jan")
      result.month = 1;
    else if(mon == "feb")
      result.month = 2;
    else if (mon == "mar")
      result.month = 3;
    else if (mon == "apr")
      result.month = 4;
    else if (mon == "may")
      result.month = 5;
    else if (mon == "jun")
      result.month = 6;
    else if (mon == "jul")
      result.month = 7;
    else if (mon == "aug")
      result.month = 8;
    else if (mon == "sep")
      result.month = 9;
    else if (mon == "oct")
      result.month = 10;
    else if (mon == "nov")
      result.month = 11;
    else if (mon == "dev")
      result.month = 12;

    return result;
    }

  arinc_entities::arinc_entities(schema& schema, const string& filename)
    {
    _filename = filename;
    ifstream is(filename);

    line* last_primary_line = nullptr;
    string source;
    while (getline(is, source))
      {
      if (source.starts_with("HDR"))
        {
        _headers.push_back(source);
        string hdr_number = source.substr(3, 2);
        if (hdr_number == "01")
          {
          _filename = trim_copy(source.substr(5, 15));
          _version_number = stoi(trim_copy(source.substr(20, 3)));
          _is_production = trim_copy(source.substr(23, 1)) == "P";
          _record_count = stoi(trim_copy(source.substr(28, 7)));
          _cycle_date = trim_copy(source.substr(35, 4));
          _creation_date = parse_date(source.substr(41, 19));
          _data_provider = trim_copy(source.substr(61, 16));
          }
        }
      else if (source.starts_with("S"))
        {
        line *l = new line(schema, source, last_primary_line);

        if(l->type() == primary_with_continuation_following)
          last_primary_line = l;

        entities::iterator it = _entities.find(l->entity_name());

        if(it == _entities.end())
          {
          _entities.insert( { l->entity_name(), vector<line *>() });
          it = _entities.find(l->entity_name());
          }

        it->second.push_back(l);
        }
      }

    }
  }