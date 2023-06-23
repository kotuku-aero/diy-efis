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
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <locale>
#include <regex>

#include "node.h"

static strings_t enums_generated;

static std::string replace_str(const std::string &str, const char *str1, const char *str2)
  {
  std::string result;
  std::string::size_type offs = 0;
  std::string::size_type pos;
  std::string::size_type len = strlen(str1);

  while ((pos = str.find(str1, offs)) != std::string::npos)
    {
    result = str.substr(offs, (pos)-offs);
    result += str2;
    offs = pos + len;
    }

  if (offs < str.length())
    result += str.substr(offs);

  return result;
  }

node_t::node_t(const std::string &node_type,
    const std::string &variable_type,
    const std::string &get_function_name,
    const std::string &name,
    const std::string &enum_type,
    const std::string &desc,
    int flags,
  const std::string &filename)
    {
    _get_function_name = get_function_name;
    _node_type = node_type;
    _variable_type = variable_type;
    _name = name;
    _enum_type = enum_type;
    _desc = desc;
    _generated_enum_ref = false;
    _flags = flags;
    _parent = 0;
    _next = 0;
    _filename = filename;
    }

node_t * node_t::add_child(node_t *child)
  {
  for (nodes_t::iterator it = _children.begin(); it != _children.end(); it++)
    {
    if (child->_node_type == (*it)->_node_type && child->_name == (*it)->_name)
      return (*it);
    }
  // Fill out some information that are tree structure dependent.
  // These information are actually embedded in the tree already
  // However, we compute them and cache them to reduce the
  // processing time later.This approach increases the storage
  // requirement but decreases the processing time.
  child->_parent = this;

  if (!_children.empty())
    _children.back()->_next = child;

  // Insert the node into the children list
  _children.push_back(child);

  return child;
  }

bool node_t::is_param() const
  {
  // Is this node a parameter node.
  return ("ROOT" != _node_type) &&
    ("END" != _node_type) &&
    ("KEYWORD" != _node_type);
  }

bool node_t::is_keyword() const
  {
  return "KEYWORD" == _node_type;
  }

bool node_t::is_optional() const
  {
  // Is this node an optional keyword / parameter.
  return (_flags & CLI_NODE_FLAGS_OPT_PARTIAL) != 0;
  }

bool node_t::has_generated_enum_ref() const
  {
  if (_generated_enum_ref)
    return true;

  for (strings_t::const_iterator it = enums_generated.begin(); it != enums_generated.end(); it++)
    {
    if (*it == enum_type())
      {
      const_cast<node_t *>(this)->_generated_enum_ref = true;
      return true;
      }
    }

  // flag we are about to send it
  enums_generated.push_back(enum_type());

  return false;
  }

const std::string &node_t::path()
  {
  if (node_type() != "ROOT" &&
    _path.empty())
    {
    strings_t paths;
    for (const node_t *node = this; node != 0; node = node->_parent)
      {
      if (node->node_type() != "ROOT" && node->node_type() != "END")
        paths.push_back(replace_str(node->_name, "-", "_"));
      }

    for (strings_t::reverse_iterator it = paths.rbegin(); it != paths.rend(); it++)
      {
      if (!_path.empty())
        _path += "_";
      _path += *it;
      }
    }

  return _path;
  }

/*
##
# \brief     Walk the tree
#
# \param     fn     Function to be called per node.
# \param     mode   'pre-order', 'func', 'post-order'.
# \param     cookie An opaque object to be passed into 'fn'.
#
# \return    Return the number of callback made.
*/
int node_t::walk(walk_fn fn, void *cookie)
  {
  int count = 0;
  for (nodes_t::iterator it = _children.begin(); it != _children.end(); it++)
    count += (*it)->walk(fn, cookie);

  (*fn)(this, cookie);
  count++;

  return count;
  }

std::string node_t::c_struct_fwd()
  {
  std::stringstream msg;

  if (path().empty())
    return msg.str();

  if (_parent != 0)
    {
    msg << "static cli_node_t " << "node_" << path();

    if (node_type() == "END")
      msg << "_end";

    msg << ";" << std::endl;
    }

  return msg.str();
  }
/*
Generate the C structure name.

@return  Return a string that contains the C structure for the node.
*/
std::string node_t::c_struct()
  {
  std::stringstream msg;
  std::string this_kw;

  if (_parent != 0 && path().empty())
    return msg.str();

  if (_parent != 0)
    msg << "static ";

  msg << "cli_node_t ";
  if (_parent == 0)
    msg << _filename << "_cli_root";
  else
    msg << "node_" << path();

  if (node_type() == "END")
    msg << "_end";

  msg << " = {" << std::endl;

  // type
  msg << "  CLI_NODE_" << _node_type << "," << std::endl;

  // flags
  msg << "  ";
  if (_flags == 0)
    msg << "0";
  else
    {
    bool first = true;

    if (_flags & CLI_NODE_FLAGS_OPT_START)
      {
      if (!first)
        msg << " | ";

      msg << "CLI_NODE_FLAGS_OPT_START";
      first = false;
      }

    if (_flags & CLI_NODE_FLAGS_OPT_END)
      {
      if (!first)
        msg << " | ";

      msg << "CLI_NODE_FLAGS_OPT_END";
      first = false;
      }

    if (_flags & CLI_NODE_FLAGS_OPT_PARTIAL)
      {
      if (!first)
        msg << " | ";

      msg << "CLI_NODE_FLAGS_OPT_PARTIAL";
      first = false;
      }
    }
  msg << "," << std::endl;

  // param
  if (_node_type == "ROOT")
    msg << "  0," << std::endl;
  else if (_node_type == "END")    // and end node is a pointer to a function
    msg << "  cli_" << path() << "," << std::endl;
  else if (_node_type == "KEYWORD")
    msg << "  \"" << name() << "\"," << std::endl;
  else if (_node_type == "ENUM")
    msg << "  &" << enum_type() << "," << std::endl;
  else
    msg << "  \"" << _node_type << ":" << name() << "\"," << std::endl;

  // desc
  if (_desc.empty())
    msg << "  0";
  else
    msg << "  \"" << _desc << "\"";

  msg << "," << std::endl;

  // sibling
  if (_next != 0)
    msg << "  &node_" << _next->path() << "," << std::endl;
  else
    msg << "  0," << std::endl;

  // children
  if (!_children.empty())
    {
    node_t *child = _children[0];
    // if submode.
    if (child->node_type() == "ROOT")
      {
      if (!child->_children.empty())
        child = child->_children[0];
      else
        child = 0;
      }

    if (child != 0)
      {
      msg << "  &node_" << child->path();

      if (_children[0]->node_type() == "END")
        msg << "_end";

      msg << std::endl;
      }
    else
      msg << "  0" << std::endl;
    }
  else
    msg << "  0" << std::endl;

  msg << "  };" << std::endl << std::endl;

  return msg.str();
  }

/*
Return a list of Node objects that forms a path from root to this node.

@return  A list of Node objects that forms a path from root(first elemnt) to this node(last element).
*/
nodes_t node_t::walk_up_to_root() const
  {
  nodes_t result;
  if (_node_type != "END")
    return result;

  const node_t *cur_node = this;

  while (cur_node->_parent != 0 && cur_node->node_type() != "ROOT")
    {
    result.push_back(const_cast<node_t *>(cur_node));
    cur_node = cur_node->_parent;
    }

  // the nodes are in reverse order, so reverse them
  nodes_t r_nodes;
  for (nodes_t::reverse_iterator it = result.rbegin(); it != result.rend(); it++)
    r_nodes.push_back(*it);

  return r_nodes;
  }

bool node_t::is_ref_type(node_t *node)
  {
  const std::string &value_type = node->variable_type();

  if (value_type == "const char *")
    return false;

  // optional params always a reference
  if ((node->flags() & CLI_NODE_FLAGS_OPT_PARTIAL) != 0)
    return true;

  // structures by reference
  bool is_ref = value_type == "xyz_t" ||
    value_type == "lla_t" ||
    value_type == "qtn_t" ||
    value_type == "matrix_t";

  return is_ref;
  }

/*
Generate the action function prototype.

@return  The C action function prototype for a command.
*/
std::string node_t::action_fn()
  {
  std::stringstream msg;
  std::string tmp;
  std::string _path = path();
  if (!_path.empty())
    {

    // Build a list of parse nodes that forms the path from the root.
    //  to this end node
    nodes_t nodes = walk_up_to_root();

    msg << "extern result_t " << _path << "_action(cli_t *context";

    // Declare the variable list
    for (nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++)
      {
      if ((*it)->is_param())
        {
        std::string _variable_type = (*it)->variable_type();

        msg << ", " << _variable_type <<
          (is_ref_type(*it) ? " *" : " ") << (*it)->path() << "_";
        }
      }

    msg << ");" << std::endl;
    }

  return msg.str();
  }

std::string node_t::enum_ref()
  {
  std::stringstream msg;

  if (!_generated_enum_ref && node_type() == "ENUM")
    {
    msg << "extern const enum_t "
      << enum_type()
      << "[];"
      << std::endl;

    _generated_enum_ref = true;
    }

  return msg.str();
  }

/*
Generate the action function implementation stub.

@return  The C action function prototype for a command.
*/
std::string node_t::c_impl()
  {
  std::stringstream msg;
  std::string tmp;

  std::string _path = path();

  if (!_path.empty())
    {
    // Build a list of parse nodes that forms the path from the root.
    //  to this end node
    nodes_t nodes = walk_up_to_root();

    msg << "result_t " << _path << "_action(cli_t *context";

    // Declare the variable list
    for (nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++)
      {
      if ((*it)->is_param())
        {
        std::string _variable_type = (*it)->variable_type();
        msg << ", " << _variable_type <<
          (is_ref_type(*it) ? "*" : " ") << "" << (*it)->path() << "_";
        }
      }

    msg << ")" << std::endl;
    msg << "  {" << std::endl;
    msg << "  return e_not_implemented;" << std::endl;
    msg << "  }" << std::endl << std::endl;
    }

  return msg.str();
  }

/*
Generate the glue funtion.

@return  The C glue function for a command.
*/
std::string node_t::glue_fn()
  {
  std::locale loc;
  std::stringstream msg;
  std::string skip;
  /*
  Build a list of parse nodes that forms the path from the root.
  to this end node
  */
  nodes_t nodes = walk_up_to_root();
  msg << "static result_t cli_" << path() << " (cli_t *parser)" << std::endl
    << "  {" << std::endl;

  msg << "  result_t result;" << std::endl;

  bool has_params = false;
  // Declare the variable list
  for (nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++)
    {
    if (!(*it)->is_param())
      continue;
    skip = "\n";

    msg << "  " << (*it)->_variable_type << " " << (*it)->path() << "_";
    if ((*it)->_variable_type == "const char *")
      msg << " = 0";

    msg << ";" << std::endl;

    has_params = true;
    }

  if (has_params)
    msg << std::endl;

  int k = 0;
  // extract the parameters
  for (nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++, k++)
    {
    if (!(*it)->is_param())
      continue;

    msg << "  if(";
    if ((*it)->is_optional())
      {
      msg << "parser->tokens[" << k << "].token_length > 0 &&" << std::endl;
      msg << "    ";
      }
    std::string _variable_type = (*it)->variable_type();
    msg << "failed(result = " << (*it)->get_function_name()
      << "(&parser->tokens["
      << k << "], ";

    if ((*it)->node_type() == "ENUM")
      {
      msg << (*it)->enum_type() << ", ";
      }

    msg << "&" << (*it)->path() << "_)))";

    msg << std::endl;
    msg << "    return result;" << std::endl << std::endl;
    }

  // Call the user - provided action function
  msg << "  result = " << path() << "_action(parser";
  k = 0;
  for (nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++, k++)
    {
    std::string _variable_type = (*it)->variable_type();
    if ((*it)->is_param())
      {
      msg << ", ";
      if ((*it)->is_optional())
        {
        msg << "((parser->tokens[" << k << "].token_length > 0) ? ";
        }
      msg << (is_ref_type(*it) ? "&" : "") << (*it)->path() << "_";
      if ((*it)->is_optional())
        {
        msg << " : 0)";
        }
      }
    }

  msg << ");" << std::endl << std::endl;
  /*
  for(nodes_t::iterator it = nodes.begin(); it != nodes.end(); it++, k++)
  {
  std::string _variable_type = (*it)->variable_type();
  if((*it)->is_param() && _variable_type == "const char *")
  {
  msg << "  neutron_free(" << (*it)->path() << "_);" << std::endl;
  }
  }
  */
  msg << std::endl;
  msg << "  return result;" << std::endl;
  msg << "  }" << std::endl << std::endl;

  return msg.str();
  }
