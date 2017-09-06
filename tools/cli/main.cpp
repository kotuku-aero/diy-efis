#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <locale>
#include <regex>

#include <string.h>
#include <sys/stat.h>

#include "cargs.h"
#include "node.h"
#include "token.h"

static void cli_exit(int code)
  {
  exit(code);
  }

static std::string str_tolower(const std::string &str)
  {
  std::locale loc;
  std::string result;
  for(std::string::size_type i = 0; i < str.length(); i++)
    result.push_back(std::tolower(str[i], loc));

  return result;
  }


struct tokenizer_t : std::ctype<char>
  {
  tokenizer_t(char ch)
    : std::ctype<char>(get_table(ch)) {}

  static std::ctype_base::mask const* get_table(char ch)
    {
    typedef std::ctype<char> cctype;
    static const cctype::mask *const_rc = cctype::classic_table();

    static cctype::mask rc[cctype::table_size];
    std::memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

    rc[ch] = std::ctype_base::space;

    return &rc[0];
    }
  };

  static node_t *end_node;
/*
Add one line of CLI to the parse tree.

root     Root node of the parse tree.
line     A line of command from a CLI file.

returns the end of the nodes created
*/
node_t *add_cli(node_t *root, const std::string &filename, const std::string &line)
  {
  nodes_t nodes;
  int flags = 0;
  int num_opt_start = 0;

  strings_t tokens;
  std::stringstream ss(line);
  ss.imbue(std::locale(std::locale(), new tokenizer_t(' ')));
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  tokens.assign(begin, end);

  if(tokens.size() == 0)
    return root;      // blank line

  // Convert tokens to parse tree nodes. '{' and '}' do not produce tree
  // nodes.But they do affect the flags used in some nodes.
  bool start_flag = false;
  for(strings_t::const_iterator it = tokens.begin(); it != tokens.end(); it++)
    {
    // parse each token and look for '{' which identifies it as an optional node
    if(*it == "{")
      {
      // In the last node, its flags field is a reference to
      // 'flags'. So, if we append to it, the last node will get
      // a new flag.
      start_flag = true;
      num_opt_start++;
      continue;
      }
    else if(*it == "}")
      {
      if(nodes.empty())
        throw std::string("Empty nodes inside {}");
      nodes.back()->flags(nodes.back()->flags() | CLI_NODE_FLAGS_OPT_END);
      num_opt_start--;
      continue;
      }

    if(num_opt_start > 0)
      flags = CLI_NODE_FLAGS_OPT_PARTIAL;

    if(start_flag)
      flags |= CLI_NODE_FLAGS_OPT_START;

    token_t tt = token_t(it->c_str());
    nodes.push_back(new node_t(tt.node_type(), tt.variable_type(), tt.get_function_name(), tt.name(), tt.enum_type(), tt.desc(), flags, filename));
    start_flag = false;
    }

  // Insert them into the parse tree
  node_t *cur_node = root;
  int num_braces;
  for(nodes_t::const_iterator it = nodes.begin(); it != nodes.end(); it++)
    {
    // determine if the node has a child with the same name as the current node
    // if so we use that node.
    cur_node = cur_node->add_child(*it);
    }

  // put a marker on the end
  end_node = new node_t("END", "", "", "end", "", "", CLI_NODE_FLAGS_OPT_END, filename);

  cur_node->add_child(end_node);

  return end_node;
  }

enum process_mode {
  pm_compile,
  pm_preprocess,
  pm_mkdep
};

typedef std::vector<std::pair<node_t *, node_t *> > submode_stack_t;
typedef std::vector < std::pair<std::string, int> > label_stack_t;

struct is_label
  {
  const std::string &s;

  is_label(const std::string &_s)
    : s(_s)
    {
    }

  bool operator() (const std::pair<std::string, int> &l)
    { return l.first == s; }
  };

/*
Process one.cli file.This includes handling all preprocessors directive.

@param     filename Name of the.cli file.
@param     root     Root Node object of the parse tree.
@param     mode     "compile", "preprocess" or "mkdep"
@param     labels   A dictionary containing all defined labels used in
preprocessing.
@param     last_cli_root The most recent root node created.
@param     last_cli_end  The most recent end node visited.
@param     submode  depth of a submode

@return    Return the new root node.
*/
static label_stack_t label_stack;
static submode_stack_t submode_stack;
static strings_t deplist;

node_t *process_cli_file(const std::string &filename,
                         const char *include_paths,
                         node_t *root,
                         process_mode mode,
                         node_t *last_cli_root = 0,
                         node_t *last_cli_end = 0,
                         int submode = 0)
  {
  int num_disable = 0;
  std::string line;
  std::smatch result;

  std::fstream fin(filename, std::ios_base::in);

  if(!fin.good())
    {
    std::cerr << "Failed to open " << filename;
    cli_exit(-1);
    }

  std::string path;
  std::string fname;
  std::string ext;
  std::string::size_type s_index;
  if((s_index = filename.find_last_of('/')) != std::string::npos)
    {
    path = filename.substr(0, s_index);
    fname = filename.substr(s_index+1, filename.length() - s_index);
    }
  else
    {
    path = ".";
    fname = filename;
    }

  if((s_index = fname.find_last_of('.')) == std::string::npos)
    ext = "";
  else
    {
    ext = fname.substr(s_index, fname.length() - s_index);
    fname = fname.substr(s_index);
    }

  // we have to set the paths relative to the src filename if we are using
  // a #include and it refers to a relative path

  int line_num = 0;
  while(!fin.eof())
    {
    std::getline(fin, line);

    line_num++;

    size_t pos = 0;
    while(!line.empty() && isspace(line[pos]))
      pos++;

    if(pos > 0)
      line = line.substr(pos);

    if(line.empty())
      continue;


    // Process the file line by line.The orderof processing
    // These different directives is extremely important.And the
    // order below is not arbitrary.
    //
    // We must process #endif not matter what.Otherwise, once
    // we start a #ifdef block that is omitted, we'll never be able
    // to terminate it.Then, we omit every other type of line as long
    // as there is at least one disable #ifdef left.Afterward, we
    // check for illegal directives.A normal command line is handled
    // last.Illegal tokens are checked inside add_cli().
    if(std::regex_search(line, result, std::regex("^#endif")))
      {
      if(label_stack.empty())
        {
        std::cerr << filename << ":" << line_num << ": Unmatched #ifdef/#ifndef";
        cli_exit(-1);
        }

      num_disable -= label_stack.back().second;
      label_stack.pop_back();

      // Skip the rest of processing because some #ifdef / #ifndef is
    // keeping this line from being processed.
      if(num_disable > 0)
        continue;

      // Check for illegal preprocessor directives
      if(std::regex_search(line, result, std::regex("^#")) &&
        !std::regex_search(line, result, std::regex("^#ifdef(\S*//.*)*")) &&
        !std::regex_search(line, result, std::regex("^#submode(\S*//.*)*")) &&
        !std::regex_search(line, result, std::regex("^#endsubmode(\S*//.*)*")) &&
        !std::regex_search(line, result, std::regex("^#include(\S*//.*)*")))
        {
        std::cerr << filename << ":" << line_num << ": Unknown preprocessor directive";
        cli_exit(-1);
        }
      }
    // comment
    else if(std::regex_search(line, result, std::regex("^\s*//\s*(.*)")))
      {
      continue;
      }
    // #ifdef
    else if(std::regex_search(line, result, std::regex("^#ifdef (.+)")))
      {
      std::string label = *result.begin();

      label_stack_t::iterator it = std::find_if(label_stack.begin(), label_stack.end(), is_label(label));
      if(it == label_stack.end())
        {
        num_disable = num_disable + 1;
        label_stack.push_back(label_stack_t::value_type(label, 1));
        }
      continue;
      }
    // #ifndef
    else if(std::regex_search(line, result, std::regex("^#ifndef (.+)")))
      {
      std::string label = *result.begin();

      label_stack_t::iterator it = std::find_if(label_stack.begin(), label_stack.end(), is_label(label));
      if(it != label_stack.end())
        {
        num_disable = num_disable + 1;
        label_stack.push_back(label_stack_t::value_type(label, 1));
        }
      continue;
      }
    // #include
    else if(std::regex_search(line, result, std::regex("^#include \"(.+)\"")))
      {
      std::smatch::const_iterator it = result.begin();

      it++;

      std::string inc_filename = *it;

      // if not an absolute path then we prepend the rel path
      if(inc_filename[0] != '/')
        {
        inc_filename = path;
        inc_filename += "/";
        inc_filename += *it;
        }


      // check for file.
      struct stat fileInfo;
      if(stat(inc_filename.c_str(), &fileInfo) != 0)
        {
        std::cerr << filename << ":" << line_num << ": File " << inc_filename << "does not exist";
        cli_exit(-1);
        }

      switch(mode)
        {
        case pm_preprocess:
        case pm_compile:
          {
          process_cli_file(inc_filename.c_str(), include_paths, root, mode, last_cli_root, last_cli_end, submode);
          break;
          }
        case pm_mkdep:
          deplist.push_back(inc_filename);
          break;
        }

      continue;
      }
    // #submode
    else if(std::regex_search(line, result, std::regex("^#submode (.+)")))
      {
      submode_stack.push_back(submode_stack_t::value_type(last_cli_root, last_cli_end));
      std::smatch::const_iterator it = result.begin();
      it++;

      std::string desc = "Root of submode ";
      desc += *it;

      std::string name = "_";
      name += *it;

      last_cli_root = new node_t("ROOT", "", "", name, "", desc, 0, filename);
      submode++;
      last_cli_end->add_child(last_cli_root);
      continue;
      }
    // #endsubmode
    else if(std::regex_search(line, result, std::regex("^#endsubmode")))
      {
      if(submode == 0)
        {
        std::cerr << filename << ":" << line_num << ": #endsubmode without a #submode";
        cli_exit(-1);
        }

      submode--;
      last_cli_root = submode_stack.back().first;
      last_cli_end = submode_stack.back().second;
      submode_stack.pop_back();
      continue;
      }

    // must be an empty line or a command
    if(mode == pm_compile)
      {
      try
        {
        last_cli_end = add_cli(last_cli_root, filename, line);
        }
      catch(std::string &msg)
        {
        std::cerr << filename << ":" << line_num << msg;
        cli_exit(-1);
        }
      }
    else if(mode == pm_preprocess)
      {
      //std::cout << line;
      }
    else if(mode == pm_mkdep)
      {
      std::cout << filename << ":";
      for(strings_t::const_iterator it = deplist.begin(); it != deplist.end(); it++)
        std::cout << " " << *it;

      std::cout << std::endl;
      }
    }

  return root;
  }

static void print_help()
  {
  std::cerr << "Usage: cli [Options] <filename>" << std::endl;
  std::cerr << " Options are one of:" << std::endl;
  std::cerr << "    -h      Print help" << std::endl;
  std::cerr << "    -i <path>  Set include path to <path>" << std::endl;
  std::cerr << "    -o <path>  Set the output path to <path>" << std::endl;
  std::cerr << "    -f <filename>  Set the output base filename to <filename> defaults to input filename" << std::endl;
  std::cerr << "    -l <path>  Set the generated library header path to <path>";
  std::cerr << "    -s      Generate implementation files as <filename>_impl.c" << std::endl;
  std::cerr << "example: cli -o . -l ../../../libs/muon ahrs.cli" << std::endl;
  }

static bool has_option(cargs_t &args, const char *opt)
  {
  return args.options().find(opt) != args.options().end();
  }

static void print_action(node_t *node, void *cookie)
  {
  std::ofstream *hdr = (std::ofstream *)cookie;

  if(((node->flags() & CLI_NODE_FLAGS_OPT_END) != 0) &&
     ((node->flags() & CLI_NODE_FLAGS_OPT_PARTIAL) == 0) )
    {
    std::string msg = node->action_fn();
    if(!msg.empty())
      *hdr << msg << std::endl;
    }

  if (node->node_type() == "ENUM" && !node->has_generated_enum_ref())
    {
    std::string msg = node->enum_ref();
    if (!msg.empty())
      *hdr << msg << std::endl;
    }
  }

static void print_glue(node_t *node, void *cookie)
  {
  std::ofstream *hdr = (std::ofstream *)cookie;

  if(((node->flags() & CLI_NODE_FLAGS_OPT_END) != 0) &&
    ((node->flags() & CLI_NODE_FLAGS_OPT_PARTIAL) == 0))
    {
    std::string msg = node->glue_fn();
    if(!msg.empty())
      *hdr << msg << std::endl;
    }
  }

static void print_struct_fwd(node_t *node, void *cookie)
  {
  std::ofstream *hdr = (std::ofstream *)cookie;

  std::string msg = node->c_struct_fwd();
  if(!msg.empty())
    *hdr << msg << std::endl;
  }

static void print_struct(node_t *node, void *cookie)
  {
  std::ofstream *hdr = (std::ofstream *)cookie;

  std::string msg = node->c_struct();
  if(!msg.empty())
    *hdr << msg << std::endl;
  }

static void print_impl(node_t *node, void *cookie)
  {
  std::ofstream *hdr = (std::ofstream *)cookie;


  if(((node->flags() & CLI_NODE_FLAGS_OPT_END) != 0) &&
    ((node->flags() & CLI_NODE_FLAGS_OPT_PARTIAL) == 0))
    {
    std::string msg = node->c_impl();
    if(!msg.empty())
      *hdr << msg << std::endl << std::endl;
    }
  }

int main(int argc, const char **argv)
  {
  cargs_t args;

  args.parse(argc, argv);

  if(args.arguments().size() < 1)
    {
    print_help();
    cli_exit(-1);
    }

  if(has_option(args, "h"))
    {
    print_help();
    cli_exit(-1);
    }

  std::string include_path = "";
  if(has_option(args, "i"))
    include_path = args.options()["i"];

  std::string src_filename(args.arguments()[0]);

  std::string s_dirname;
  std::string s_filename;
  std::string s_ext;
  std::string::size_type s_index;
  for (std::string::size_type pos = src_filename.find_first_of('\\');
    pos != std::string::npos; pos = src_filename.find_first_of('\\'))
    src_filename.replace(pos, 1, 1, '/');

  if((s_index = src_filename.find_last_of('/')) != std::string::npos)
    {
    s_dirname = src_filename.substr(0, s_index);
    s_filename = src_filename.substr(s_index+1, src_filename.length() - s_index);
    }
  else
    {
    s_dirname = ".";
    s_filename = src_filename;
    }

  if((s_index = s_filename.find_last_of('.')) == std::string::npos)
    s_ext = "";
  else
    {
    s_ext = s_filename.substr(s_index, s_filename.length() - s_index);
    s_filename = s_filename.substr(0, s_index);
    }

  node_t *root = new node_t("ROOT", "", "", "", "", "Root node of the parser tree", 0, s_filename);

  process_cli_file(src_filename.c_str(), include_path.c_str(), root, pm_compile, root);

  // We work over the parser file now
  std::string library_path = "<muon.h>";
  if(has_option(args, "l"))
    {
    library_path = "\"";
    library_path += args.options()["l"];
    library_path += "/muon.h\"";
    }

  std::string output_path;

  if(has_option(args, "o"))
    output_path = args.options()["o"];
  else
    output_path = s_dirname;

  std::string hdr_fname;
  std::string src_fname;
  std::string impl_fname;

  std::string guard_fname = s_filename;
  guard_fname += "_cli";

  std::string filename;
  if(has_option(args, "f"))
    filename = args.options()["f"];
  else
    {
    filename = output_path;
    filename += "/";
    filename += guard_fname;
    }

  hdr_fname = filename;
  hdr_fname += ".h";

  src_fname = filename;
  src_fname += ".c";

  bool needs_impl = has_option(args, "s");

  if(needs_impl)
    {
    impl_fname = filename;
    impl_fname += "_impl";
    impl_fname += ".c";
    }

  std::ofstream hdr(hdr_fname);

  hdr << "#ifndef __" << guard_fname << "_h__" << std::endl;
  hdr << "#define __" << guard_fname << "_h__" << std::endl;
  hdr << "/* This is an auto-generated file by cli */" << std::endl;
  hdr << "#include " << library_path << std::endl;
  hdr << std::endl;
  hdr << "#ifdef __cplusplus" << std::endl;
  hdr << "extern \"C\" {" << std::endl;
  hdr << "#endif" << std::endl << std::endl;
  hdr << "extern cli_node_t " << filename << "_cli_root;" << std::endl << std::endl;
  root->walk(print_action, &hdr);
  hdr << "#ifdef __cplusplus" << std::endl;
  hdr << "}" << std::endl;
  hdr << "#endif" << std::endl;
  hdr << "#endif" << std::endl;

  hdr.flush();
  hdr.close();

  std::ofstream src(src_fname);

  src << "#include \"" << guard_fname << ".h\""<< std::endl;
  src << "/* This file is generated by cli */" << std::endl << std::endl;
  root->walk(print_glue, &src);
  root->walk(print_struct_fwd, &src);
  root->walk(print_struct, &src);

  src.flush();
  src.close();

  if(needs_impl)
    {
    std::ofstream impl(impl_fname);

    impl << "#include \"" << hdr_fname << "\"" << std::endl;
    impl << "/* This file is generated by cli */" << std::endl << std::endl;

    root->walk(print_impl, &impl);

    impl.flush();
    impl.close();
    }

  return 0;
  }
