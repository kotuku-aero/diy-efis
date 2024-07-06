#ifndef __arinc_schema_h__
#define __arinc_schema_h__

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

using namespace std;

namespace kotuku
  {
  class column_definition
    {
    public:

      column_definition(int index, int length, string name, string info)
        {
        }

      string info() const { return _info; }
      int index() const { return _index; }
      int length() const { return _length; }
      string name() const { return _name; }
      void name(const string &value) { _name = value; }

    private:
      string _info;
      int _index;
      int _length;
      string _name;
    };

  typedef vector<column_definition> columns;
  typedef map<string, columns > entity_definition;

  class schema
    {
    public:
      schema(const string &filename);

      const columns* get_entity_schema(const string& entity_name) const
        {
        entity_definition::const_iterator it = _definitions.find(entity_name);
        if(it == _definitions.end())
          return nullptr;

        return &it->second;
        }
    private:
      entity_definition _definitions;
    };
  }

#endif
