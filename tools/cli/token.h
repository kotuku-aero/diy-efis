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

