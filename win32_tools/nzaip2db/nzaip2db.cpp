#include <iostream>
#include <fstream>      // filebuf
#include <format>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include "util.h"
#include "../util/cargs.h"
#include "../util/rapidjson/rapidjson.h"
#include "../util/rapidjson/document.h"
#include "../util/rapidjson/istreamwrapper.h"
#include <cassert>
#include <algorithm>

#include "util.h"
#include "airspace.h"
#include "runway.h"
#include "aerodrome.h"

using namespace std::filesystem;
using namespace std;

static void print_help()
  {
  cerr << "Usage: nzaip2db [Options] <output path>" << endl
    << " Options are one of:" << endl
    << "    -h      Print help" << endl
    << "    -s <path>  Set airspace directory <path> (required)" << endl
    << "    -c <cycle> Set the cycle to <cycle>" << endl
    << "    -r <w,n,e,s> set the spatial rect as west, north, east, south" << endl
    << "    -n <name>  set the name" << endl
    << "    -z <min>,<max> set the min-max scale for the geography (required)"
    << "    -m <filename>  create a metadata file that exports the shape metadata" << endl
    << "example: nzaip2db -s /Projects/NZDATA -c 2301 -n \"NZANR\" /Projects/map-data/nz.db" << endl;
  }

static void add_path(kotuku::spatial_entity* entity, const rapidjson::Value& path,
  fixed_t& westing, fixed_t& northing, fixed_t& easting, fixed_t& southing)
  {
  kotuku::points line;
  if (path.IsArray())
    {
    for (rapidjson::SizeType ptind = 0; ptind < path.Size(); ptind++)
      {
      const rapidjson::Value& jpt = path[ptind];
      if (jpt.IsArray() && jpt.Size() == 2)
        {
        spatial_point_t pt = kotuku::get_coordinates(jpt);

        if (easting < pt.latlng.lng)
          easting = pt.latlng.lng;

        if (westing > pt.latlng.lng)
          westing = pt.latlng.lng;

        if (northing < pt.latlng.lat)
          northing = pt.latlng.lat;

        if (southing > pt.latlng.lat)
          southing = pt.latlng.lat;

        line.push_back(pt);
        }
      }
    }

  entity->add_path(line);
  }

static bool has_option(cargs_t& args, const char* opt)
  {
  return args.options().find(opt) != args.options().end();
  }

static pair<string, string> get_tuple(const string& line)
  {
  size_t pos = line.find_first_of('=');

  string name = line.substr(0, pos);
  string value = line.substr(pos + 1);

  return make_pair(name, value);
  }

typedef kotuku::spatial_entity* (*parse_fn)(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties);


static kotuku::spatial_entity* parse_navaid(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  return nullptr;
  }

static kotuku::spatial_entity* parse_hazard(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  return nullptr;
  }

static kotuku::spatial_entity* parse_obstacle(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  return nullptr;
  }

static kotuku::spatial_entity* parse_vrp(kotuku::geometry_db* spatial_db, const rapidjson::Value& properties)
  {
  return nullptr;
  }

static kotuku::spatial_entity* parse_start_extension(kotuku::geometry_db* spatial_db, const rapidjson::Value& label)
  {
  return nullptr;
  }


static kotuku::spatial_entity* parse_designator_label(kotuku::geometry_db* spatial_db, const rapidjson::Value& label)
  {
  // this is used to parse a runway decoration
  if (!label.HasMember("properties"))
    {
    cerr << "Cannot find the properties of a runway label" << endl;
    return nullptr;
    }

  const rapidjson::Value& properties = label["properties"];

  string aerodrome_designator = kotuku::get_string_from_json(properties, "aerodrome");
  string runway_designator = kotuku::get_string_from_json(properties, "runway");

  kotuku::entities_t::iterator ad_it = std::find_if(spatial_db->begin(), spatial_db->end(),
    [aerodrome_designator, runway_designator](kotuku::spatial_entity* it)
    {
    uint32_t entity_type = it->db_type() & PAYLOAD_TYPE_MASK;
    if (entity_type == ast_aerodrome)
      {
      kotuku::aerodrome* ad = dynamic_cast<kotuku::aerodrome*>(it);

      if (ad == 0)
        return false;

      if (ad->designator() != aerodrome_designator)
        return false;

      if (std::find_if(ad->rwy_begin(), ad->rwy_end(),
        [runway_designator](kotuku::runway* rwy)
        {
        return rwy->designator() == runway_designator;
        }) != ad->rwy_end())
        return true;
      }

    return false;
    });

  // if the matching runway not found then exit
  if (ad_it == spatial_db->end())
    return nullptr;

  kotuku::aerodrome* ad = dynamic_cast<kotuku::aerodrome*>(*ad_it);
  if (ad == nullptr)
    return nullptr;

  kotuku::runways::iterator rwy_it = std::find_if(ad->rwy_begin(), ad->rwy_end(),
    [runway_designator](kotuku::runway* rwy)
    {
    return rwy->designator() == runway_designator;
    });

  kotuku::runway* rwy = dynamic_cast<kotuku::runway*>(*rwy_it);
  if (rwy == nullptr)
    return nullptr;

  rwy->parse_label(label);

  return nullptr;     // already processed geometry
  }

struct entity_map {
  const char* type;
  parse_fn parse;
  };

static const entity_map parsers[] = {
  "GAA", kotuku::airspace::parse,
  "GAA_PART", kotuku::airspace::parse,
  "MBZ", kotuku::airspace::parse,
  "MBZ_PART", kotuku::airspace::parse,
  "CFZ", kotuku::airspace::parse,
  "CTR", kotuku::airspace::parse,
  "CTR_SECTOR", kotuku::airspace::parse,
  "CTA", kotuku::airspace::parse,
  "D", kotuku::airspace::parse,
  "LFZ", kotuku::airspace::parse,
  "R", kotuku::airspace::parse,
  "MOA", kotuku::airspace::parse,
  "T", kotuku::airspace::parse,
  "VHZ", kotuku::airspace::parse,
  "FISCOM", kotuku::airspace::parse,
  "runway", kotuku::runway::parse,
  "designatorlabel", parse_designator_label,
  "starterextn", parse_start_extension,
  "AD", kotuku::aerodrome::parse,
  "VOR_DME", parse_navaid,
  "NDB", parse_navaid,
  "NDB_DME", parse_navaid,
  "FIR", kotuku::airspace::parse,
  "KITE_FLYING", parse_hazard,
  "HANG GLIDING", parse_hazard,
  "MODEL ACFT", parse_hazard,
  "FLIGHT TRAINING", parse_hazard,
  "GLIDER ACTIVITY", parse_hazard,
  "SENSITIVE FAUNA", parse_hazard,
  "OTHER:MICROWAVE_TOWER", parse_obstacle,
  "ANTENNA", parse_obstacle,
  "TOWER", parse_obstacle,
  "WINDMILL", parse_obstacle,
  "VRP", parse_vrp,
  0, 0
  };

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
    !has_option(args, "z") ||
    !has_option(args, "r") ||
    !has_option(args, "s") ||
    args.arguments().empty())
    {
    print_help();
    exit(-1);
    }

  string source_path = "";
  if (has_option(args, "s"))
    source_path = args.options()["s"];

  string output_path = args.arguments()[0];

  // the source path is a directory with a collection of .air files
  // that are combined into an airspace database

  spatial_rect_t rect = { 0, 0, 0, 0 };
  string rect_str = args.options()["r"];

  // split the string into , sep values

  int field = -1;
  // must be 1
  size_t start = 0;
  size_t term = rect_str.find(',');
  while (term == rect_str.npos || term < rect_str.length())
    {
    if (term == rect_str.npos)
      term = rect_str.length();

    if (term > start)
      {
      string str = rect_str.substr(start, term - start);
      float deg;
      switch (tolower(str[0]))
        {
        case 's':
        case 'w':
          deg = strtof(str.c_str() + 1, 0) * -1.0f;
          break;
        case 'n':
        case 'e':
          deg = strtof(str.c_str() + 1, 0);
          break;
        }

      switch (++field)
        {
        case 0:
          rect.left = float_to_fixed(deg);
          break;
        case 1:
          rect.top = float_to_fixed(deg);
          break;
        case 2:
          rect.right = float_to_fixed(deg);
          break;
        case 3:
          rect.bottom = float_to_fixed(deg);
          break;
        default:
        {
        cerr << "Too many arguments to the rect" << endl;
        return -1;
        }
        }
      }

    start = term + 1;

    if (term < rect_str.length())
      term = rect_str.find(',', start);
    }

  if (rect.right < rect.left ||
    rect.top < rect.bottom)
    {
    cerr << "The rectangle must have N > S and E > W" << endl;
    return -1;
    }

  // write the database header
  kotuku::geometry_db* spatial_db = new kotuku::geometry_db(ct_airspace, rect);

  if (has_option(args, "n"))
    spatial_db->name(args.options()["n"]);

  string zoom = args.options()["z"];
  term = zoom.find(',');

  if (term == string::npos)
    term = zoom.length();

  int min_zoom = stoi(zoom.substr(0, term));
  int max_zoom = 25000;

  if (term != zoom.length())
    max_zoom = stoi(zoom.substr(term + 1));

  spatial_db->min_scale((uint16_t)min_zoom);
  spatial_db->max_scale((uint16_t)max_zoom);

  // read them all.
  path json_path = path(source_path);

  spatial_db->src_path(json_path);

  vector<kotuku::airspace> db;

  for (auto const& dir_entry : filesystem::directory_iterator{ json_path })
    {
    string filename = dir_entry.path().generic_string();

    if (filename.ends_with(".json"))
      {
      // we have a .json character file.
      cout << "Process " << filename << endl;

      ifstream is(filename, ios_base::in);
      rapidjson::BasicIStreamWrapper stream(is);

      rapidjson::Document jsonDoc;
      // parse the document
      jsonDoc.ParseStream(stream);
      // the json document 
      const rapidjson::Value& features = jsonDoc["features"];

      if (features.IsArray())
        {
        for (rapidjson::SizeType ind = 0; ind < features.Size(); ind++)
          {
          const rapidjson::Value& feature = features[ind];
          kotuku::spatial_entity* entity = nullptr;

          string str;

          if (feature.HasMember("properties"))
            {
            const rapidjson::Value& properties = feature["properties"];

            str = properties["type"].GetString();

            for (const entity_map* map = parsers; map->type != 0; map++)
              {
              if (str == map->type)
                {
                // special case for a label type, which parses the geometry directly
                // TODO: make this a param of the array
                entity = (*map->parse)(spatial_db, str == "designatorlabel" ? feature : properties);
                break;
                }
              }

            if (entity == 0)
              continue;           // don't parse geometry
            }

          fixed_t easting = float_to_fixed(-180);
          fixed_t westing = float_to_fixed(180);
          fixed_t northing = float_to_fixed(-90);
          fixed_t southing = float_to_fixed(90);

          // work over the geometry
          if (entity != nullptr && feature.HasMember("geometry"))
            {
            const rapidjson::Value& geometry = feature["geometry"];
            if (geometry.HasMember("coordinates"))
              {
              const rapidjson::Value& coordinates = geometry["coordinates"];
              if (coordinates.IsArray())
                {
                for (rapidjson::SizeType pind = 0; pind < coordinates.Size(); pind++)
                  {
                  const rapidjson::Value& path = coordinates[pind];

                  kotuku::points line;
                  string entity_type = kotuku::get_string_from_json(geometry, "type");

                  if (entity_type == "Polygon" || entity_type == "LineString")
                    add_path(entity, path, westing, northing, easting, southing);
                  else if (entity_type == "MultiPolygon" || entity_type == "MultiLineString")
                    {
                    for (rapidjson::SizeType ptind = 0; ptind < path.Size(); ptind++)
                      {
                      const rapidjson::Value& poly = path[ptind];
                      add_path(entity, poly, westing, northing, easting, southing);
                      }
                    }
                  else if (entity_type == "Point")
                    {
                    spatial_point_t pt = kotuku::get_coordinates(coordinates);

                    entity->center(pt.latlng);
                    }
                  else
                    {
                    cerr << "Unknown geometry type " << entity_type << endl;
                    }
                  }
                }
              }
            }

//#ifdef _DEBUG
//          cout << "Entity bounds "
//            << fixed_to_float(westing) << ", "
//            << fixed_to_float(northing) << ", "
//            << fixed_to_float(easting) << ", "
//            << fixed_to_float(southing) << endl;
//#endif
          // not correct for a point entity!
          entity->set_bounds(westing, northing, easting, southing);

          switch (entity->db_type() & SPATIAL_TYPE_MASK)
            {
            case SPATIAL_POINT:
              spatial_db->add_spatial_entity(dynamic_cast<kotuku::spatial_point*>(entity));
              break;
            case SPATIAL_POLYGON:
            case SPATIAL_POLYLINE:
              spatial_db->add_spatial_entity(dynamic_cast<kotuku::spatial_linestring*>(entity));
              break;
            }
          }
        }
      }
    }

  // write the airspaces to a database.

  ofstream* metadata_file = 0;
  if (has_option(args, "m"))
    {
    metadata_file = new ofstream(args.options()["m"]);

    *metadata_file << "entity_id,field-name,field-value" << endl;
    }

  std::cout << spatial_db->entities().size() << " Entites read from file and indexed " << endl;

  std::cout << "Create database " << output_path << endl;
  // write the database
  ofstream os(output_path, ios_base::binary | ios_base::trunc | ios_base::out);
  spatial_db->serialize(os);

  os.flush();
  os.close();

  if (metadata_file != 0)
    {
    const kotuku::entities_t& entities = spatial_db->entities();

    for (size_t i = 0; i < entities.size(); i++)
      {
      const kotuku::spatial_entity* entity = entities[i];

      for (size_t attr = 0; attr < entity->num_attributes(); attr++)
        {
        const pair<string, string>& nv_pair = entity->get_attribute(attr);

        *metadata_file << entity->file_offset() << ", \"" << nv_pair.first << "\", \"" << nv_pair.second << "\"" << endl;
        }
      }

    metadata_file->flush();
    metadata_file->close();

    delete metadata_file;
    }

  return 0;
  }
