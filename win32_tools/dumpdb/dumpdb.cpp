#include <iostream>
#include <fstream>
#include "../util/cargs.h"

#include "../../libs/graviton/schema.h"

using namespace std;

static void print_help()
  {
  cerr << "Usage: dumpdb [Options] <input path>" << endl
    << " Options are one of:" << endl
    << "    -r <w,n,e,s> set the spatial rect as west, north, east, south" << endl
    << "    -h      Print help" << endl
    << "example: dumpdb  \\Projects\\map_data\\coastlines.db" << endl;
  }

static bool has_option(cargs_t& args, const char* opt)
  {
  return args.options().find(opt) != args.options().end();
  }

static string format_date(const ymd_t& date)
  {
  string result;

  return result;
  }

static string format_rect(const spatial_rect_t& rect)
  {
  string result;

  return result;
  }

static string format_point(const latlng_t& pt)
  {
  string result;

  return result;
  }

static string format_spatial_id(const spatial_entity_id_t& id)
  {
  string result;

  return result;
  }

static uint32_t read_uint32(ifstream& is, uint32_t& value)
  {
  is.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
  return sizeof(uint32_t);
  }

static uint32_t read_uint16(ifstream& is, uint16_t& value)
  {
  is.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
  return sizeof(uint16_t);
  }

static uint32_t read_int32(ifstream& is, int32_t& value)
  {
  is.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
  return sizeof(int32_t);
  }

static uint32_t read_int16(ifstream& is, int16_t& value)
  {
  is.read(reinterpret_cast<char*>(&value), sizeof(int16_t));
  return sizeof(int16_t);
  }

static void print_airspace(ostream& os, const spatial_entity_hdr_t* entity)
  {
  const airspace_t* airspace = (const airspace_t*)entity;

  string icao_id;
  for (size_t i = 0; i < 6; i++)
    {
    if (isalpha(airspace->base.identifier[i]))
      icao_id += airspace->base.identifier[i];
    else
      break;
    }
  os << "     ICAO       : " << icao_id << endl;
  os << "     CLASS      : ";
  switch (airspace->classification)
    {
    default:
    case   act_unknown:
      os << "Unknown";
      break;
    case act_A:
      os << "A";
      break;
    case act_B:
      os << "B";
      break;
    case act_C:
      os << "C";
      break;
    case act_D:
      os << "D";
      break;
    case act_E:
      os << "E";
      break;
    case act_F:
      os << "F";
      break;
    case act_G:
      os << "G";
      break;
    }

  os << endl;

  os << "     UPPER LIMIT : " << airspace->upper_limit << endl;
  os << "     LOWER LIMIT : " << airspace->lower_limit << endl;

  /*

  uint32_t name_str;      // descriptive name string index
  uint32_t remarks_str;   // remarks string index
  uint32_t remarks2_str;  // second line of remarks string index
  */
  }

int main(int argc, const char** argv)
  {
  cargs_t args;

  args.parse(argc, argv);

  if (args.arguments().size() < 1 ||
    has_option(args, "h"))
    {
    print_help();
    exit(-1);
    }

  bool filter_to_rect = false;
  spatial_rect_t rect = { 0, 0, 0, 0 };
  if (has_option(args, "r"))
    {
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

    filter_to_rect = true;
    }

  db_header_t hdr;

  ifstream is(args.arguments()[0], ios_base::binary | ios_base::in);

  is.read(reinterpret_cast<char*>(&hdr), sizeof(db_header_t));

  if (hdr.magic != DB_MAGIC)
    {
    cerr << "Database header signature not found" << endl;
    return -1;
    }

  cout << "hdr.length         = " << hdr.length << endl;
  cout << "hdr.type           = " << hdr.db_type << endl;
  cout << "hdr.content_type   = " << hdr.content_type << endl;
  cout << "hdr.effective_from = " << format_date(hdr.effective_from) << endl;
  cout << "hdr.effective_to   = " << format_date(hdr.effective_to) << endl;
  cout << "hdr.name           = " << hdr.name << endl;
  cout << "hdr.bounds         = " << format_rect(hdr.bounds) << endl;
  cout << "hdr.string_table   = " << hdr.string_table << endl;
  cout << "hdr.num_strings    = " << hdr.string_table_length << endl;
  cout << "hdr.min_scale      = " << hdr.min_scale << endl;
  cout << "hdr.max_scale      = " << hdr.max_scale << endl;

  if (hdr.db_type == db_spatial)
    {
    cout << endl;
    cout << "db_type == db_spatial" << endl;

    is.seekg(0, ios_base::beg);

    geospatial_container_header_t spatial_hdr;
    is.read(reinterpret_cast<char*>(&spatial_hdr), sizeof(geospatial_container_header_t));

    cout << "spatial.tiles_lng       = " << spatial_hdr.tiles_lng << endl;
    cout << "spatial.tiles_lat       = " << spatial_hdr.tiles_lat << endl;
    cout << "spatial.num_index_lists = " << spatial_hdr.num_index_lists << endl;
    cout << "spatial.num_entities    = " << spatial_hdr.num_entities << endl;

    cout << endl << "dump of spatial_entity_id tile map" << endl;
    if (filter_to_rect)
      {
      cout << "-- filtered to" << endl;
      cout << "   top    = " << fixed_to_int(rect.top) << endl;
      cout << "   left   = " << fixed_to_int(rect.left) << endl;
      cout << "   bottom = " << fixed_to_int(rect.bottom) << endl;
      cout << "   right  = " << fixed_to_int(rect.right) << endl << endl;
      }
    spatial_offset_t id;

    for (int lat = 0; lat < spatial_hdr.tiles_lat; lat++)
      {
      for (int lng = 0; lng < spatial_hdr.tiles_lng; lng++)
        {
        is.read(reinterpret_cast<char*>(&id), sizeof(spatial_offset_t));
        if (id.offset == 0 || id.length == 0)
          continue;       // no cell

        int lat_deg = 90 - lat;
        int lng_deg = lng - 180;

        if (filter_to_rect &&
          (lat_deg > fixed_to_int(rect.top) ||
            lat_deg <= fixed_to_int(rect.bottom) ||
            lng_deg < fixed_to_int(rect.left) ||
            lng_deg >= fixed_to_int(rect.right)))
          continue;

        streampos pos = is.tellg();
        cout << "Lat: " << lat_deg << ", Lng: " << lng_deg << endl;
        cout << "   Offset = " << id.offset << ", Length = " << id.length << endl;

        is.seekg(id.offset, ios_base::beg);
        spatial_entity_id_t entity_id;
        for (uint32_t i = 0; i < id.length; i++)
          {
          is.read(reinterpret_cast<char*>(&entity_id), sizeof(spatial_entity_id_t));

          streampos nxt_id_pos = is.tellg();
          cout << "   entity_id_t - " << i + 1 << endl;
          cout << "     entity.type     = " << entity_id.type << endl;
          cout << "     entity.offset   = " << entity_id.offset << endl;
          cout << "     entity.length   = " << entity_id.length << endl;
          cout << "     graphics.offset = " << entity_id.graphics_offset << endl;
          cout << "     graphics.length = " << entity_id.graphics_length << endl;

          is.seekg(entity_id.offset, ios_base::beg);

          spatial_entity_hdr_t* entity = (spatial_entity_hdr_t*) new uint8_t[entity_id.length];
          is.read(reinterpret_cast<char*>(entity), entity_id.length);

          cout << "   entity_t -" << endl;
          cout << "     size           = " << entity->size << endl;
          cout << "     type           = " << entity->type << endl;
          cout << "     bounding_box   = " << format_rect(entity->bounding_box) << endl;
          cout << "     center_pt      = " << format_point(entity->center_pt) << endl;
          cout << "     entity_length  = " << entity->entity_length << endl;
          cout << "     strings_length = " << entity->strings_length << endl;
          cout << "     paths_length   = " << entity->paths_length << endl;

          cout << "     graphics encoded as ";
          switch (entity->type & SPATIAL_TYPE_MASK)
            {
            case SPATIAL_DEM:
              cout << "DEM";
              break;
            case SPATIAL_POINT:
              cout << "POINT";
              break;
            case SPATIAL_POLYLINE:
              cout << "POLYLINE";
              break;
            case SPATIAL_POLYGON:
              cout << "POLYGON";
              break;
            default:
              cout << "Unknown encoding " << (entity->type & SPATIAL_TYPE_MASK);
              break;
            }

          cout << endl;
          cout << "   payload is a ";

          switch (entity->type & PAYLOAD_TYPE_MASK)
            {
            case ast_danger_area:
              cout << "ast_danger_area" << endl;
              print_airspace(cout, entity);
              break;
            case ast_restricted_area:
              cout << "ast_restricted_area" << endl;
              print_airspace(cout, entity);
              break;
            case ast_general_aviation:
              cout << "ast_general_aviation" << endl;
              print_airspace(cout, entity);
              break;
            case ast_ctr:
              cout << "ast_ctr" << endl;
              print_airspace(cout, entity);
              break;
            case ast_cta:
              cout << "ast_cta" << endl;
              print_airspace(cout, entity);
              break;
            case ast_millitary_zone:
              cout << "ast_millitary_zone" << endl;
              print_airspace(cout, entity);
              break;
            case ast_mandatory_broadcast_zone:
              cout << "ast_mandatory_broadcast_zone" << endl;
              print_airspace(cout, entity);
              break;
            case ast_common_frequency_zone:
              cout << "ast_common_frequency_zone" << endl;
              print_airspace(cout, entity);
              break;
            case ast_volcanic_hazard:
              cout << "ast_volcanic_hazard" << endl;
              print_airspace(cout, entity);
              break;
            case ast_aerodrome:
              cout << "ast_aerodrome" << endl;
              print_airspace(cout, entity);
              break;
            case ast_heliport:
              cout << "ast_heliport" << endl;
              print_airspace(cout, entity);
              break;
            case ast_visual_reporting_point:
              cout << "ast_visual_reporting_point" << endl;
              print_airspace(cout, entity);
              break;
            default:
              break;
            }

          delete entity;

          if (entity_id.graphics_offset > 0)
            {
            cout << "  entity has cached graphics paths" << endl;

            uint32_t graphics_count = 0;
            is.seekg(entity_id.graphics_offset, ios_base::beg);
            uint32_t num_lines;

            graphics_count += read_uint32(is, num_lines);

            cout << "        there are " << num_lines << " polylines" << endl;

            for (uint32_t line = 0; line < num_lines; line++)
              {
              uint32_t num_points;
              graphics_count += read_uint32(is, num_points);

              cout << "          line " << line << " has " << num_points << " points, shown as (lng, lat)" << endl;
              spatial_point_t pt;
              for (uint32_t ptidx = 0; ptidx < num_points; ptidx++)
                {
                graphics_count += read_int32(is, (int32_t&)pt.latlng.lat);
                graphics_count += read_int32(is, (int32_t&)pt.latlng.lng);
                graphics_count += read_int16(is, (int16_t&)pt.elevation);

                cout << "            " << fixed_to_float(pt.latlng.lat) << ", " << fixed_to_float(pt.latlng.lng) << "" << endl;
                }

              cout << endl;
              }


            if (graphics_count != entity_id.graphics_length)
              cerr << "Error *** The header says graphics length is " << entity_id.graphics_length << " but is " << graphics_count << endl;
            }

          // go back to index list
          is.seekg(nxt_id_pos, ios_base::beg);
          }

        is.seekg(pos, ios_base::beg);
        }
      }
    }

  return 0;
  }