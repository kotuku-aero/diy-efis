#ifndef __line_h__
#define __line_h__

#include <vector>
#include <string>
#include <map>

#include "schema.h"

using namespace std;

namespace kotuku
  {
  enum record_type
    {
    primary_without_continuation_following,
    primary_with_continuation_following,
    continuation
    };

  class line
    {
    public:
      line(schema &s, string str, line *parent = nullptr);

      record_type type() const { return _type; }
      
      const string& operator [](const string& key) const 
      {
      values::const_iterator it = find_if(_values.begin(), _values.end(),
        [key] (const std::pair<const column_definition *, string> &l) -> bool
        {
        return l.first->name() == key;
        });
      if(it == _values.end())
        throw exception(format("{} not found", key).c_str());

      return it->second;
      }

      const string &entity_name() const { return _entity_name; }
      const string &source() const { return _source; }

      void add_child(line *child) { _children.push_back(child); }
    private:
      line *_parent;
      vector<line *> _children;
      record_type _type;
      const columns *_columns;
      string _entity_name;
      string _source;
      typedef map<const column_definition *, string> values;
      values _values;
    };
  }
#endif
