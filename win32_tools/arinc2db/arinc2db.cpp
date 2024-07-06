#include <iostream>
#include <fstream>      // filebuf
#include <format>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include "../util/cargs.h"

#include "../../libs/atomdb/db_priv.h"


#include "../util/arinc_entities.h"

#ifdef assert
#undef asset
#endif

#include <cassert>

using namespace std::filesystem;
using namespace std;
using namespace kotuku;

static void print_help()
  {
  cerr << "Usage: arinc2db [Options] <output path>" << endl
    << " Options are one of:" << endl
    << "    -h      Print help" << endl
    << "    -a <path>  ARINC 424 file (required)" << endl
    << "    -s <path>  Schema definition (required)" << endl
    << "    -d <name>  Database name to export(required) " << endl
    << "    -m <path>  Set the path of the metadata file" << endl
    << "example: arinc2db -c 2306 -s localhost -u export:password -d CFIPS_2304 -m C:/Projects/tmp/map-data.csv C:/Projects/map-data/navdata .db" << endl;
  }

static bool has_option(cargs_t& args, const char* opt)
  {
  return args.options().find(opt) != args.options().end();
  }

int main(int argc, const char** argv)
  {
  cargs_t args;

  args.parse(argc, argv);

  if (args.arguments().size() < 1)
    {
    print_help();
    exit(-1);
    }

  if (has_option(args, "h") ||
    !has_option(args, "s") ||
    !has_option(args, "a") ||
    !has_option(args, "d") ||
    args.arguments().empty())
    {
    print_help();
    exit(-1);
    }

  string schema_name = args.options()["s"];
  string arinc_424 = args.options()["a"];
  schema sch(schema_name);

  arinc_entities entities(sch, arinc_424);

  cout << "Filename  " << entities.filename() << endl;
  cout << "Cycle     " << entities.cycle_date() << endl;
  cout << "Provider  " << entities.data_provider() << endl;
  cout << "Loaded " << entities.record_count() << endl;

  // work over each entity type and add to the spatial database
  //
  // this is the mapping
  //
  // canfly             ARINC types     spatial type
  // airspace_t     


  // output the metadatafile
  if (has_option(args, "m"))
    {
    ofstream os(args.options()["m"], ios_base::trunc | ios_base::out);

    os << "entity-id,field-name,value" << endl;

    // dump each entity attributes.

    os.flush();
    os.close();
    }

  cout << "Container created" << endl;

  return 0;
  }
