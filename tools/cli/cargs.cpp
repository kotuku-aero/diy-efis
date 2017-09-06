#include "cargs.h"

#include <cstdlib>
#include <string.h>
#include <vector>
#include <map>
#include <string>

void cargs_t::parse(int argc, const char* argv[])
  {
  std::string val;
  std::string to_insert;
  size_t pos = to_insert.npos;
  for(int i = 1; i < argc; i++)
    {
    val.clear();
    switch(argv[i][0])
      {
      case '-':
        if(argv[i][1] == '-')
          {
          // Argument of long form (--example).
          to_insert = &argv[i][2];
          }
        else if(strlen(argv[i]) > 2)
          {
          // Multiple arguments of short form (-abc).
          for(size_t j = 1; j < strlen(argv[i]); j++)
            {
            to_insert = argv[i][j];
            _map.insert(std::make_pair(to_insert, val));
            }
          break;
          }
        else
          {
          // Single argument of short form (-a).
          to_insert = argv[i][1];
          }
        // Get value of arg if provided.
        if((pos = to_insert.find("=")) != to_insert.npos)
          {
          val = to_insert.substr(pos + 1);
          to_insert.erase(pos);
          }
        else if(i + 1 < argc)
          {
          if(argv[i+1][0] != '-')
            val = argv[i + 1];
          }
        _map.erase(to_insert);
        _map.insert(make_pair(to_insert, val));
        // If value is found, increment argument index.
        if(pos == to_insert.npos &&
          !val.empty())
          i++;
        break;
      default:
        _vec.push_back(argv[i]);
        break;
      }
    }
  }
