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
#ifndef __token_h__
#define __token_h__

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <locale>
#include <regex>

// Token class. This class represents a token in a CLI command.
class token_t
  {
  public:
    token_t(const char *str);

    bool valid_keyword(const char *str);

    const std::string &node_type() const { return _node_type; }
    const std::string &get_function_name() const { return _get_function_name; }
    const std::string &variable_type() const { return _variable_type; }
    const std::string &name() const { return _name; }
    const std::string &desc() const { return _desc; }
    const std::string &enum_type() const { return _enum_type; }
  private:
    std::string _node_type;
    std::string _get_function_name;
    std::string _variable_type;
    std::string _name;
    std::string _desc;
    std::string _enum_type;
  };
#endif

