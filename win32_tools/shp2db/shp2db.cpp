#include "../util/cargs.h"
#include <cassert>
#include <iostream>

#include "../geometry_db/db_geometry.h"
#include "shapefil.h"
// schema types
#include "../../libs/graviton/schema.h"

static void print_help()
  {
  cerr << "Usage: shp2db [Options] <output path>" << endl
    << " Options are one of:" << endl
    << "    -h      Print help" << endl
    << "    -s <path>  set source path to <path> (as many as required)" << endl
    << "    -t <type>  set the container type (required)" << endl
    << "    -r <w,n,e,s> set the spatial rect as west, north, east, south" << endl
    << "    -n <name>  set the name" << endl
    << "    -z <min>,<max> set the min-max scale for the geography (required)"
    << "    -m <filename>  create a metadata file that exports the shape metadata" << endl
    << "example: shp2db -s \\Projects\\contours\\land.shp -t coastlines -r W180,N90,E180,S60 -n \"Coastlines 7.5 arcsec\" -c 2301  \\Projects\\map-data\\coastlines.db" << endl;
  }

//-s \Projects\contours\land.shp -t coastlines -r W180,N90,E180,S60 -n "Coastlines 7.5 arcsec" -c 2301  \Projects\map-data\coastlines.db

static bool has_option(cargs_t& args, const char* opt)
  {
  return args.options().find(opt) != args.options().end();
  }

struct export_type_t {
  const char* name;
  uint32_t type;
  };

static const export_type_t export_options[] =
  {
  { "contours", ct_contours },            // contours
  { "coastlines", ct_coastlines },        // coastlines
  { "landareas", ct_landarea },            // land area fill
  { "cities", ct_cities },                // cities and towns
  { "surface_water", ct_surface_water },  // lakes and rivers
  { "transport", ct_transport },          // roads and rail
  { "obstacles", ct_obstacles},           // power lines, aerial obstructions
  { 0, db_null },
  };

int main(int argc, const char** argv)
  {
  cargs_t args;

  args.parse(argc, argv);

  if (args.arguments().empty() ||
    has_option(args, "h") ||
    !has_option(args, "z") ||
    !has_option(args, "s") ||
    !has_option(args, "r") ||
    !has_option(args, "t"))
    {
    print_help();
    exit(-1);
    }

  vector<string> source_files;
  if (has_option(args, "s"))
    {
    string source_filenames = args.options()["s"];

    size_t start_char = 0;
    for (size_t end_delim = source_filenames.find(';');
      end_delim == string::npos || end_delim < source_filenames.length();
      end_delim = source_filenames.find(';', start_char))
      {
      if (start_char > source_filenames.length())
        break;

      // special case for 1 string
      if (end_delim == string::npos)
        end_delim = source_filenames.length();

      if (start_char >= end_delim)
        break;

      string filename = source_filenames.substr(start_char, end_delim - start_char);

      if (filename.length() > 0)
        source_files.push_back(filename);

      start_char = end_delim + 1;
      }
    }

  ofstream* metadata_file = 0;
  if (has_option(args, "m"))
    {
    metadata_file = new ofstream(args.options()["m"]);

    *metadata_file << "entity_id,field-name,field-value" << endl;
    }

  string output_path = args.arguments()[0];

  // decode the resulting type
  string db_type_str = args.options()["t"];

  uint32_t type = db_null;
  for (const export_type_t* export_str = export_options; export_str->name != 0; export_str++)
    {
    if (db_type_str == export_str->name)
      {
      type = export_str->type;
      break;
      }
    }

  if (type == db_null)
    {
    cerr << "The database type " << db_type_str << " is not supported" << endl;
    exit(-1);
    }

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


  string zoom = args.options()["z"];
  term = zoom.find(',');

  if(term == string::npos)
    term = zoom.length();

  int min_zoom = stoi(zoom.substr(0, term));
  int max_zoom = 25000;

  if(term != zoom.length())
    max_zoom = stoi(zoom.substr(term+1));

  // create the new database
  kotuku::geometry_db* db = new kotuku::geometry_db(type, rect);

  if (has_option(args, "n"))
    db->name(args.options()["n"]);

  db->min_scale((uint16_t) min_zoom);
  db->max_scale((uint16_t) max_zoom);

  for (vector<string>::const_iterator it = source_files.begin(); it != source_files.end(); it++)
    {
    std::cout << "Open shapefile " << *it << endl;
    // read and convert the shapefile to db types.
    SHPHandle shpfile = SHPOpen(it->c_str(), "rb");

    DBFHandle dbffile = 0;
    vector<string> fieldnames;
    if (metadata_file != 0)
      {
      // open the file with the name of the file with the type replaced
      // by .dbf
      size_t index = it->find_last_of('.');
      if (index != string::npos)
        {
        string dbf = it->substr(0, index);
        dbf.append(".dbf");

        dbffile = DBFOpen(dbf.c_str(), "rb");

        if (dbffile == 0)
          cerr << "Cannot open the metadata file for shape file " << *it << endl;
        else
          {
          int field_count = DBFGetFieldCount(dbffile);
          for (int i = 0; i < field_count; i++)
            {
            char field_name[12];
            if (DBFGetFieldInfo(dbffile, i, field_name, 0, 0) != FTInvalid)
              fieldnames.push_back(field_name);
            }
          }
        }
      }

    int num_entities;
    int shp_type;

    SHPGetInfo(shpfile, &num_entities, &shp_type, 0, 0);

    std::cout << "There are " << num_entities << " entities" << endl;

    for (int i = 0; i < num_entities; i++)
      {
      std::cout << "Process entity " << i+1 << endl;

      SHPObject* shpObject = SHPReadObject(shpfile, i);

      uint32_t db_type;
      switch (shp_type)
        {
        case SHPT_POLYLINE:
        case SHPT_POLYLINEZ:
        case SHPT_POLYLINEM:
          db_type = SPATIAL_POLYLINE;
          break;
        case SHPT_POLYGON:
        case SHPT_POLYGONZ:
        case SHPT_POLYGONM:
          // if we are filling a coastine then this is a 
          // polyline so the area is outlined
          if(type == ct_coastlines)
            db_type = SPATIAL_POLYLINE;
          else
            db_type = SPATIAL_POLYGON;
          break;
        case SHPT_POINT:
        case SHPT_MULTIPOINT:
        case SHPT_POINTZ:
        case SHPT_MULTIPOINTZ:
        case SHPT_POINTM:
        case SHPT_MULTIPOINTM:
        case SHPT_MULTIPATCH:
        case SHPT_NULL:
        default:
          db_type = 0;
          break;
        }

      if (db_type != 0)
        {
        kotuku::spatial_linestring* entity = 0;
        int part_end = shpObject->nVertices;

        switch (db_type)
          {
          case SPATIAL_POLYGON:
            entity = new kotuku::spatial_polygon(db);
            break;
          case SPATIAL_POLYLINE:
            entity = new kotuku::spatial_polyline(db);
            break;
            //case SPATIAL_POINT:
            //  entity = new kotuku::spatial_point(db);
            //  break;
          }

        fixed_t easting = int_to_fixed(- 180);
        fixed_t westing = int_to_fixed(180);
        fixed_t northing = int_to_fixed(-90);
        fixed_t southing = int_to_fixed(90);

        for (int path = 0; path < shpObject->nParts; path++)
          {
          int part_start = shpObject->panPartStart[path];

          // TODO: not sure.
          int part_type = shpObject->panPartType[path];

          if (path == shpObject->nParts - 1)
            part_end = shpObject->nVertices;
          else
            part_end = shpObject->panPartStart[path + 1];

          kotuku::points line;
          while (part_start < part_end)
            {
            ::spatial_point_t pt;
            pt.latlng.lat = float_to_fixed((float)shpObject->padfY[part_start]);
            pt.latlng.lng = float_to_fixed((float)shpObject->padfX[part_start]);

#ifdef _DEBUG
            // cout << "Shapefile object path " << path << " point " << shpObject->padfX[part_start] << ", " << shpObject->padfY[part_start] << endl;
#endif

            int lat = fixed_to_int(pt.latlng.lat);
            int lng = fixed_to_int(pt.latlng.lng);

            if (easting < pt.latlng.lng)
              easting = pt.latlng.lng;

            if (westing > pt.latlng.lng)
              westing = pt.latlng.lng;

            if (northing < pt.latlng.lat)
              northing = pt.latlng.lat;

            if (southing > pt.latlng.lat)
              southing = pt.latlng.lat;

            if (shpObject->nSHPType == SHPT_POLYGONZ)
              pt.elevation = (int16_t)round(shpObject->padfZ[part_start]);
            else if (part_type == SHPT_POLYGONM)
              pt.elevation = (int16_t)round(shpObject->padfM[part_start]);
            else
              pt.elevation = 0;

            line.push_back(pt);

            part_start++;
            }

          entity->add_path(line);
          }


        //std::cout << "Entity bounds " 
        //  << fixed_to_float(westing) << ", " 
        //  << fixed_to_float(northing) << ", "
        //  << fixed_to_float(easting) << ", "
        //  << fixed_to_float(southing) << endl;

        entity->set_bounds(westing, northing, easting, southing);

        // cout << "Add new entity" << endl;
        if (!db->add_spatial_entity(entity))
          delete entity;

        if (dbffile != 0)
          {
          // export the attributes
          int field_index = 0;
          for (vector<string>::const_iterator it = fieldnames.begin(); it != fieldnames.end(); it++, field_index++)
            {
            const char* value = DBFReadStringAttribute(dbffile, i, field_index);
            // store the attributes
            if (value != 0 && strlen(value) > 0)
              entity->add_attribute(*it, value);
            }
          }
        }

      SHPDestroyObject(shpObject);
      //#ifdef _DEBUG
      //      break;
      //#endif
      }
    //#ifdef _DEBUG
    //    break;
    //#endif
    }

  std::cout << db->entities().size() << " Entites read from file and indexed " << endl;

  std::cout << "Create database " << output_path << endl;
  // write the database
  ofstream os(output_path, ios_base::binary | ios_base::trunc | ios_base::out);
  db->serialize(os);

  os.flush();
  os.close();

  if (metadata_file != 0)
    {
    const kotuku::entities_t& entities = db->entities();

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

