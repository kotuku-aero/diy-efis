#include "line.h"

namespace kotuku {
  static record_type GetRecordType(char continuationNo)
    {

    switch (continuationNo)
      {
      case '0':
        return primary_without_continuation_following; //No continuation records following
      case '1':
        return primary_with_continuation_following; //Several continuation records following
      default:
        return continuation; //Number of the continuation record
      }
    }


  line::line(schema& schema, string source, line* parent)
    {
    _source = source;
    int layout_first_index = 4;
    int layoutSecondIndex = source[5] != ' ' ? 5 : 12;   // Layout ending symbol index in the source line
    _entity_name += source[layout_first_index];

    if (source[layoutSecondIndex] != ' ')
      _entity_name += source[layoutSecondIndex];

    _columns = schema.get_entity_schema(_entity_name);
    if (_columns != nullptr) // Checking if such layout exists in the partition map
      {
      columns::const_iterator continuationIndexLocation = _columns->end();
      columns::const_iterator continuationIndexTypeLocation = _columns->end();

      for (columns::const_iterator col_it = _columns->begin(); col_it != _columns->end(); col_it++)
        {
        if (col_it->info() == "cnum")
          continuationIndexLocation = col_it;
        else if (col_it->info() == "cnum_type")
          continuationIndexTypeLocation = col_it;

        if (continuationIndexLocation != _columns->end() &&
          continuationIndexTypeLocation != _columns->end())
          break;
        }

      char continuationType;

      if (continuationIndexTypeLocation != _columns->end())
        {
        _type = GetRecordType(source[continuationIndexTypeLocation->index()]);
        continuationType = source[continuationIndexTypeLocation->index() + 1];
        }
      else
        if (continuationIndexLocation != _columns->end())
          {
          _type = GetRecordType(source[continuationIndexLocation->index()]);
          continuationType = 'A';
          }
        else
          {
          _type = primary_without_continuation_following;
          continuationType = '\0';
          }

      if (_type == continuation)
        {
        if (parent != nullptr)
          parent->add_child(this);

        string continuation_name = format("{}_{}", _entity_name, continuationType);
        const columns* continuation_type = schema.get_entity_schema(continuation_name);

        if (continuation_type != nullptr)
          _entity_name = continuation_name;
        else
          throw exception(format("Cannot locate schema definition for {}", continuation_name).c_str());
        }

      _columns = schema.get_entity_schema(_entity_name);

      if (_columns == nullptr)
        throw exception(format("Cannot locate schema definition for {}", _entity_name).c_str());

      for (columns::const_iterator it = _columns->begin(); it != _columns->end(); it++)
        {
        string value = source.substr(it->index(), it->length());
        const column_definition* col = it.operator->();

        _values.insert({ col, value });
        }
      }
    }

  }