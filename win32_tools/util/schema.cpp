#include "schema.h"
#include "string.h"

kotuku::schema::schema(const string &defn)
  {
  fstream is(defn);

  string str;

  while (getline(is, str))
    {
    if (trim(str).empty())
      continue;

    // remove trailing ;
    if (str.back() == ';')
      str = str.substr(0, str.length() - 1);

    int table_name_marker = str.find_first_of(':');
    if (table_name_marker == string::npos)
      throw exception("Definitions must start with a table name followed by a :");

    string table_name = trim_copy(str.substr(0, table_name_marker));
    str = str.substr(table_name_marker + 1);

    vector<string> split_str = split(str, ';');
    columns column_definitions;
    int parsed_value = 0; //Previous parsed value is saved to check if the next one is actually higher (otherwise error)
    for (string column_str : split_str)
      {
      trim(column_str);

      string info;
      int start_pos = column_str.find_first_of('[');
      int end_pos;
      if (start_pos != string::npos)
        {
        end_pos = column_str.find_first_of(']', start_pos);
        if (end_pos != string::npos)
          {
          info = column_str.substr(start_pos + 1, end_pos - start_pos - 1);
          column_str = column_str.substr(0, start_pos);   // the info is always the last part
          }
        }

      start_pos = column_str.find_first_of('(');
      end_pos = column_str.find_first_of(')', start_pos);

      string column_name = trim_copy(column_str.substr(start_pos + 1, end_pos - start_pos - 1));
      if (column_name.empty())
        throw exception("All partition definitions need a column name");

      replace(column_name.begin(), column_name.end(), '/', '_');
      replace(column_name.begin(), column_name.end(), '(', '_');
      replace(column_name.begin(), column_name.end(), ')', '_');

      // after the end-pos is the column size format is <offset>,<length>
      column_str = trim_copy(column_str.substr(end_pos + 1));

      start_pos = column_str.find_first_of(',');
      if (start_pos == string::npos)
        throw exception("The definition must include an <offset>,<length> tuple");

      int offset = stoi(column_str.substr(0, start_pos));
      int length = stoi(column_str.substr(start_pos + 1));

      if (parsed_value > offset)
        throw exception("All column start offsets must be incremental");

      column_definitions.push_back(column_definition(offset, length, column_name, info));
      }

    // fix up duplicate names by adding a suffix
    for (int i = 0; i < column_definitions.size(); i++)
      {
      // if the prior strings has same name then add a (n)
      column_definition definition = column_definitions[i];
      int index_num = 1;
      for (int j = 0; j < column_definitions.size(); j++)
        {
        if (j != i && column_definitions[j].name() == column_definitions[i].name())
          {
          column_definitions[j].name(format("{} {}", column_definitions[j].name(), index_num++));
          }
        }
      }

    _definitions.insert({ table_name, column_definitions });
    }
  }
