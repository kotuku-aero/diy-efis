#include "token.h"

token_t::token_t(const char *str)
  {
  if (valid_keyword(str))
    {
    _node_type = "KEYWORD";
    _name = str;
    return;
    }

  // This token must be a parameter of some kind.Parse the type.
  std::smatch result;
  std::string token(str);

  std::smatch::const_iterator it;
  if (std::regex_search(token, result, std::regex("^\<(STRING):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "STRING";
    _variable_type = "const char *";
    _get_function_name = "cli_get_string";
    }
  else if (std::regex_search(token, result, std::regex("^\<(ENUM):([a-zA-Z][a-zA-Z0-9_]*):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    it++;
    _enum_type = *it;
    _node_type = "ENUM";
    _variable_type = "uint16_t";
    _get_function_name = "cli_get_enum";
    }
  else if (std::regex_search(token, result, std::regex("^\<(PATH):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "PATH";
    _variable_type = "const char *";
    _get_function_name = "cli_get_path";
    }
  else if (std::regex_search(token, result, std::regex("^\<(UINT16):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "UINT16";
    _variable_type = "uint16_t";
    _get_function_name = "cli_get_uint16";
    }
  else if (std::regex_search(token, result, std::regex("^\<(UINT32):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "UINT32";
    _variable_type = "uint32_t";
    _get_function_name = "cli_get_uint32";
    }
  else if (std::regex_search(token, result, std::regex("^\<(INT16):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "INT16";
    _variable_type = "int16_t";
    _get_function_name = "cli_get_int16";
    }
  else if (std::regex_search(token, result, std::regex("^\<(INT32):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "INT32";
    _variable_type = "int32_t";
    _get_function_name = "cli_get_int32";
    }
  else if (std::regex_search(token, result, std::regex("^\<(FLOAT):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "FLOAT";
    _variable_type = "float";
    _get_function_name = "cli_get_float";
    }
  else if (std::regex_search(token, result, std::regex("^\<(BOOL):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _enum_type = "booleans";
    _node_type = "ENUM";
    _variable_type = "bool";
    _get_function_name = "cli_get_bool";
    }
  else if (std::regex_search(token, result, std::regex("^\<(SCRIPT):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "STRING";
    _variable_type = "const char *";
    _get_function_name = "cli_get_script";
    }
  else if (std::regex_search(token, result, std::regex("^\<(XYZ):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "XYZ";
    _variable_type = "xyz_t";
    _get_function_name = "cli_get_xyz";
    }
  else if (std::regex_search(token, result, std::regex("^\<(MATRIX):([a-zA-Z][a-zA-Z0-9_]*)(:?(.*))\>$")))
    {
    it = result.begin();
    it++;
    _node_type = "MATRIX";
    _variable_type = "matrix_t";
    _get_function_name = "cli_get_matrix";
    }
  else
    throw std::string("Invalid token");

  it++;
  // extract the name of the parameter
  _name = *it++;
  // and the optional description
  _desc = *it++;
  }

bool token_t::valid_keyword(const char *str)
  {
  std::string pat = "^[a-zA-Z0-9_-]+$";
  std::regex reg(pat);

  return std::regex_search(str, reg);
  }

