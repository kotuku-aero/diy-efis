/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
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
