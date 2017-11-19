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
#ifndef __node_h__
#define __node_h__

#include <vector>
#include <map>
#include <string>

class node_t;

typedef std::vector<node_t *> nodes_t;
typedef std::vector<std::string> strings_t;

#define CLI_NODE_FLAGS_OPT_START    0x0002
#define CLI_NODE_FLAGS_OPT_END      0x0004
#define CLI_NODE_FLAGS_OPT_PARTIAL  0x0008

class node_t {
public:
  node_t(const std::string &node_type,
    const std::string &variable_type,
    const std::string &get_function_name,
    const std::string &name,
    const std::string &enum_type,
    const std::string &desc,
    int flags,
    const std::string &filename);

  node_t * add_child(node_t *child);

  bool is_param() const;
  bool is_keyword() const;
  bool is_optional() const;
  const std::string &node_type() const { return _node_type; }
  bool has_generated_enum_ref() const;
  const std::string &name() const { return _name; }

  const std::string &path();
  const std::string &variable_type() const { return _variable_type; }
  const std::string &get_function_name() const { return _get_function_name; }
  int flags() const { return _flags; }
  void flags(int value) { _flags = value; }
  const std::string &enum_type() const { return _enum_type; }
  typedef void(*walk_fn)(node_t *node, void *cookie);
  int  walk(walk_fn fn, void *cookie);
  std::string c_struct_fwd();
  std::string c_struct();
  nodes_t walk_up_to_root() const;
  bool is_ref_type(node_t *node);
  std::string action_fn();
  std::string enum_ref();
  std::string c_impl();
  std::string glue_fn();
  node_t *parent() { return _parent; }
private:
  std::string _node_type;
  std::string _variable_type;
  std::string _get_function_name;
  std::string _name;
  std::string _desc;
  std::string _enum_type;
  int _flags;
  nodes_t _children;
  strings_t _list_kw;
  node_t *_parent;
  node_t *_next;
  std::string _path;
  bool _generated_enum_ref;
  std::string _filename;
  };


#endif
