#ifndef __cargs_h__
#define __cargs_h__

#include <map>
#include <vector>
#include <string>


class cargs_t {
public:
  cargs_t() {}
  ~cargs_t() {}

  typedef std::map<std::string, std::string> arg_map_t;
  typedef std::vector<std::string> arg_vec_t;

  // Parses command line arguments.
  // |argc| number of arguments
  // |argv| argument vector
  void parse(int argc, const char** argv);

  // return the options
  arg_map_t &options() { return _map; }
  const arg_map_t &options() const { return _map; }

  // return the arguments
  arg_vec_t &arguments() { return _vec; }
  const arg_vec_t &arguments() const { return _vec; }
private:
  arg_map_t _map;
  arg_vec_t _vec;
  };

#endif

