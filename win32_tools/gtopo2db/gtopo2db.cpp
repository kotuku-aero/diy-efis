#include <iostream>
#include <fstream>      // filebuf
#include <format>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include "../util/cargs.h"
#include <cassert>

// get the header definitions
#include "../../libs/atomdb/db.h"

using namespace std::filesystem;
using namespace std;

static void print_help()
  {
  cerr << "Usage: gtopo2db [Options] <output path>" << endl
    << " Options are one of:" << endl
    << "    -h      Print help" << endl
    << "    -s <path>  Set DEM directory <path> (required)" << endl
    << "    -a <num>   Set the hillshade azumith to <num> degrees (rec. 315 (NW))" << endl
    << "    -z <num>   Set the hillshade zenith to <num> degrees (rec. 45)" << endl
    << "    -n <name>  set the name" << endl
    << " NOTE: both -a and -z must be provided to hillshade the output pixmaps" << endl
    << "example: gtopo2db -s /Projects/terrain -c 2301 -n \"All World\" - a 315 - z 45 /Projects/map-data/terrain.db" << endl;
  }

static bool has_option(cargs_t& args, const char* opt)
  {
  return args.options().find(opt) != args.options().end();
  }

static const char* gtopo_names[] =
  {
  "gt30e020n90",
  "gt30e020n40",
  "gt30e020s10",

  "gt30e060n90",
  "gt30e060n40",
  "gt30e060s10",

  "gt30e100n90",
  "gt30e100n40",
  "gt30e100s10",

  "gt30e140n90",
  "gt30e140n40",
  "gt30e140s10",

  "gt30w180n90",
  "gt30w180n40",
  "gt30w180s10",

  "gt30w140n90",
  "gt30w140n40",
  "gt30w140s10",

  "gt30w100n90",
  "gt30w100n40",
  "gt30w100s10",

  "gt30w060n90",
  "gt30w060n40",
  "gt30w060s10",

  "gt30w020n90",
  "gt30w020n40",
  "gt30w020s10",


  //"gt30w180s60",
  //"gt30w120s60",
  //"gt30w060s60",
  //"gt30w000s60",
  //"gt30e060s60",
  //"gt30e120s60",
  0
  };

static const int16_t no_data = -9999;
//
// values are stored in 3 bytes, with the following format:
//
// TODO: pack this to 3 bytes

// shade is an alpha of the color from 0-255
//
// These are calculated at compile time from the surounding tile
// pixels
// These values can then be used to render hilside shading.
// The preferred manual style can be simulated in analytical computations by ignoring 
// slope information and basing the shading on aspect only. Various GIS applications
// sometimes combine aspect-based shading and diffuse reflection. Hereafter an 
// interactive variant is presented.
//
// Aspect-based shading is calculated according to a modified cosine 
// shading equation (Moellering and Kimerling, 1990):
//
// gray value=cos(<angle>)+12
// 
// Where <angle> is the angle between aspect and the azimuth of the light direction
//
// to calculate the slope of a pixel, the surrounding 9 pixels are examined
// to approximate the slope.
//

// tile built here
typedef pixel_t tile_t[120][120];

// cache of tiles that are defines
static tile_t* tiles[360][180];

typedef double shade_matrix_t[3][3];

static string make_tile_name(int lng, int lat)
  {
  bool easting = lng >= 0;
  bool southing = lat < 0;

  if (!easting)
    lng = 0 - lng;

  if (southing)
    lat = 0 - lat;

  return format("{}{:03d}{}{:02d}", easting ? 'E' : 'W', lng, southing ? 'S' : 'N', lat);
  }


inline tile_t* tile_at(int lng, int lat)
  {
  lng += 180;       // make 0-359
  lat += 90;        // make 0-180 (0 is south)

  if (lat >= 180)
    return 0;

  //assert(lng >= 0 && lng < 360);
  //assert(lat >= 0);

  return tiles[lng][lat];
  }

inline void store_tile(int lng, int lat, tile_t* tile)
  {
  lng += 180;       // make 0-359
  lat += 90;        // make 0-180 (0 is south)

  //assert(lng >= 0 && lng < 360);
  //assert(lat >= 0 && lat < 180);

  tiles[lng][lat] = tile;
  }

inline double get_gtopo30_elevation(tile_t* tile, int x, int y)
  {
  if ((*tile)[y][x].elevation == no_data)
    return 0;

  return (*tile)[y][x].elevation;
  }

static double get_gtopo30_elevation(int lng, int lat, int x, int y)
  {
  while (lng < -180)
    lng += 360;

  while (lng >= 180)
    lng -= 180;

  if (lat >= 90 || lat <= -60)
    return 0;

  // each tile is 120 pixels or 0.5 degrees
  tile_t* tile = tile_at(lng, lat);

  if (tile == 0)
    return 0;

  return get_gtopo30_elevation(tile, x, y);
  }

#define DB_VERSION 0x0101

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
    args.arguments().empty())
    {
    print_help();
    exit(-1);
    }

  string source_base_path = "";
  if (has_option(args, "s"))
    source_base_path = args.options()["s"];

  string output_path = args.arguments()[0];

  // TODO: these should be parameters
  int16_t db_lat = 90;
  int16_t db_lng = -180;
  uint16_t degrees_lat = 150;
  uint16_t degrees_lng = 360;

  // the source path is a directory with sub-directories for
  // each gtopo30 tile set
  // iterate over the directories and files.
  // the DEM files are divided into pixel tiles that are 120x120 pixels
  // each or 1degree by 1 degree each.
  //
  // the tiles are written to the output directory in the format
  // of <N/S><nn><E/W>nnn
  //
  // where N/S is latitude and E/W is longitude

  int num_tiles = 0;

  for (const char** tilename = gtopo_names; *tilename != 0; tilename++)
    {
    string dem_name = format("{}.dem", *tilename);
    string dem_dir = format("{}_dem", *tilename);

    path dem_path = path(source_base_path) / dem_dir / dem_name;

    // get the base lat/lng from the tile name
    //
    // name is gt30<E/W>NNN<N/S>NN
    string gtoponame(*tilename);

    bool westing = gtoponame[4] == 'w';
    bool southing = gtoponame[8] == 's';

    string lngstr = gtoponame.substr(5, 3);
    string latstr = gtoponame.substr(9, 2);

    int lngbase = stoi(lngstr);

    if (westing)
      lngbase = 0 - lngbase;        // convert to -180..180 format

    int latbase = stoi(latstr);

    if (southing)
      latbase = 0 - latbase;          // make south

    // open the DEM file, and split the row into an array of 128x128 tiles
    ifstream is(dem_path, ios_base::in | ios_base::binary);

    if (!is.good())
      {
      cerr << "Cannot open the DEM " << dem_path << endl;
      exit(-1);
      }

    tile_t* imported_tiles[40];

    cout << "Process " << gtoponame << endl;

    // each tile is processed as a 1 degree tile
    // there are 6000 rows, with 120 pixels per 1 degree tile
    // means there are 50 tiles veriticaly
    for (int tile_y = 0; tile_y < 50; tile_y++)
      {
      // allocate memory for a line of tiles.
      // 14400 is 120x120 pixels
      // there are 4800 elevations in a dem row
      // which is 40 x 120 pixel tiles
      for (int i = 0; i < 40; i++)
        imported_tiles[i] = (tile_t*) new pixel_t[14400];

      for (int tile_row = 0; tile_row < 120; tile_row++)
        {
        for (int tile_x = 0; tile_x < 40; tile_x++)
          {
          for (int tile_column = 0; tile_column < 120; tile_column++)
            {
            // get the cell from the input file (will be next int16)
            uint16_t elev;
            is.read(reinterpret_cast<char*>(&elev), sizeof(uint16_t));

            // swap bytes from big-endian
            int16_t elev_little_endian = (int16_t)((elev >> 8) | (elev << 8));

            (*imported_tiles[tile_x])[tile_row][tile_column].elevation = elev_little_endian;
            (*imported_tiles[tile_x])[tile_row][tile_column].shade = 0xFF;       // no shading yet
            }
          }
        }

      // there have been 40 1 degree tiles created
      // work over them an export them
      // first determine if the tile is empty
      for (int tile_x = 0; tile_x < 40; tile_x++)
        {
        bool is_empty = true;
        for (int tile_row = 0; tile_row < 120; tile_row++)
          {
          for (int tile_column = 0; tile_column < 120; tile_column++)
            {
            if ((*imported_tiles[tile_x])[tile_row][tile_column].elevation != no_data)
              {
              is_empty = false;
              break;
              }
            }

          if (!is_empty)
            break;
          }

        if (is_empty)
          delete imported_tiles[tile_x];
        else
          {
          store_tile(lngbase + tile_x, latbase - tile_y, imported_tiles[tile_x]);

          num_tiles++;
          }
        }
      }
    }

  cout << "Created " << num_tiles << endl;

  // perform shading on the generated tiles as a post-process operation
  if (has_option(args, "a") && has_option(args, "z"))
    {
    cout << "Post process dataset for shading" << endl;
    double zenith = stod(args.options()["z"]);
    cout << "Zenith is assigned value of " << zenith << endl;
    double azumith = stod(args.options()["a"]);
    cout << "Azumith is assigned value of " << azumith << endl;

    // convert to radians
    const double azumith_rad = degrees_to_radians(azumith);
    const double zenith_rad = degrees_to_radians(zenith);

    // work over the cached tiles that were created and assign the
    // shading values to each
    shade_matrix_t sample;
    int num_shader = 1;

    for (int lng = -180; lng < 180; lng++)
      {
      for (int lat = 90; lat > -60; lat--)
        {
        tile_t* tile = tile_at(lng, lat);
        if (tile != 0)
          {
          for (int y = 0; y < 120; y++)
            {
            for (int x = 0; x < 120; x++)
              {
              // calculate tile to the left
              int left_pix = x - 1;
              if (left_pix < 0)
                left_pix = 119;

              int left_lng = lng - 1;
              if (left_lng < -180)
                left_lng = 179;

              // calculate tile to the right
              int right_pix = x + 1;
              if (right_pix > 119)
                right_pix = 0;

              int right_lng = lng + 1;
              if (right_lng >= 180)
                right_lng = -180;

              // calculate row above
              int above_pix = y - 1;
              int above_lat = lat;
              if (above_pix < 0)
                {
                above_lat++;
                above_pix = 119;
                }

              // calculate row below
              int below_pix = y + 1;
              int below_lat = lat;
              if (below_pix >= 120)
                {
                below_lat--;
                below_pix = 0;
                }

              // special case for lat == 90
              sample[0][0] = get_gtopo30_elevation(left_lng, above_lat, left_pix, above_pix);
              sample[0][1] = get_gtopo30_elevation(lng, above_lat, x, above_pix);
              sample[0][2] = get_gtopo30_elevation(right_lng, above_lat, right_pix, above_pix);

              sample[1][0] = get_gtopo30_elevation(left_lng, lat, x, y);
              // get our sample to be shaded
              sample[1][1] = get_gtopo30_elevation(tile, x, y);
              sample[1][2] = get_gtopo30_elevation(right_lng, lat, right_pix, y);

              sample[2][0] = get_gtopo30_elevation(left_lng, below_lat, left_pix, below_pix);
              sample[2][1] = get_gtopo30_elevation(lng, below_lat, x, below_pix);
              sample[2][2] = get_gtopo30_elevation(right_lng, below_lat, right_pix, below_pix);

              // calculate the slope and aspect of the pixel matrix
              // in relation to a NED frame

              /*
              * based on a matrix
              *
              *   [Zi-1,j+1][Zi,j+1][Zi+1,j+1]
              *   [Zi-1,j  ][Zi,j  ][Zi+1,j  ]
              *   [Zi-1,j-1][Zi,j-1][Zi+1,j-1]
              *
              If average slope is requested, east west gradients are calculated as follows

              dEW= [(Zi+1,j+1 + 2Zi+1,j + Zi+1,j-1) - (Zi-1,j+1 + 2Zi-1,j + Zi-1,j-1)]/8dX

              The north-south gradient is calculated by

              dNS = [(Zi+1,j+1 + 2Zi,j+1 + Zi-1,j+1) - (Zi+1,j-1 + 2Zi,j-1 + Zi-1,j-1)]/8dy

              Where
              dx = the east-west distance across the cell (cell width)
              dy = the north-south distance across the cell (cell height)

              Percent slope is calculated by:

              Slope% = 100 * [(dEW)^2*(dNS)^2]^1/2

              Degrees slope is calculated by:

              SlopeDegrees = ArcTangent[(dEW)^2 +(dNS)^2]^1/2
              */

              double dew = ((sample[0][2] + 2 * sample[1][2] + sample[2][2]) - (sample[0][0] + 2 * sample[1][0] + sample[2][0])) / 8;
              double dns = (sample[0][2] + 2 * sample[0][1] + sample[0][0]) - (sample[2][2] + 2 * sample[2][1] + sample[2][0]) / 8;

              double slope = 100 * pow(((dew * dew) + (dns * dns)), 0.5);
              if (slope > 0)
                {
                double slope_angle = atan(pow(((dew * dew) + (dns * dns)), 0.5));

                /*
                The above equation is adjusted to reflect aspect in degrees in a range from 0 to 360.
                Often -1 is used to represent a cell with no slope (skyward aspect)
                and the values from 0 to 360 represent azimuths in clockwise degrees from north.
                North is 0, East is 90 degrees, South is 180 degrees etc.
                Optionally, the result is then divided by 22.5 and converted
                to an integer to derive a set of generalized solar azimuths .
                */
                double aspect = atan(dew / dns);      // aspect in radians

                /*
                Hillshade = 255.0 * ((cos(Zenith_rad) * cos(Slope_rad)) +
                 (sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad)))

                 */
                double hillshade = (cos(zenith_rad) * cos(slope_angle)) +
                  (sin(zenith_rad) * sin(slope_angle) * cos(azumith_rad - aspect));

                hillshade *= 255;

                uint8_t alpha = (uint8_t)hillshade;

                (*tile)[y][x].shade = alpha;
                }
              }
            }

           cout << "Shaded " << num_shader++ << "\r";
          }
        }
      }

    }

#ifdef GENFILES
  for (int lng = -180; lng < 180; lng++)
    {
    for (int lat = 90; lat > -60; lat--)
      {
      tile_t* tile = tile_at(lng, lat);
      if (tile != 0)
        {
        string tile_name = make_tile_name(lng, lat);
        cout << "Write tile " << tile_name << endl;

        path tile_path = path(output_path) / tile_name;

        fstream os(tile_path, ios_base::binary | ios_base::trunc | ios_base::out);

        os.write(reinterpret_cast<const char*>(tile), sizeof(pixel_t) * 120 * 120);

        os.flush();
        os.close();
        }
      }
    }
#else
  cout << "Write terrain database to " << output_path << endl;

  // this holds the list of tiles to be written when the header is done
  vector<tile_t*> contents;

  const uint32_t tile_size = sizeof(pixel_t) * 120 * 120;

  uint32_t no_tile = 0;

  size_t hdr_size = sizeof(terrain_container_header_t) + (sizeof(uint32_t) * degrees_lat * degrees_lng);

  terrain_container_header_t*hdr = (terrain_container_header_t*) malloc(hdr_size);
  memset(hdr, 0, hdr_size);

  /*
  int16_t db_lat = 90;
  int16_t db_lng = -180;
  uint16_t degrees_lat = 150;
  uint16_t degrees_lng = 360;
*/
  hdr->base.magic = DB_MAGIC;
  hdr->base.length = sizeof(terrain_container_header_t);

  if(has_option(args, "n"))
    strncpy(hdr->base.name, args.options()["n"].c_str(), 31);

  hdr->base.db_type = db_terrain;
  hdr->base.content_type = 0;

  hdr->base.bounds.left = int_to_fixed(db_lng);
  hdr->base.bounds.top = int_to_fixed(db_lat);
  hdr->base.bounds.right = int_to_fixed(db_lng + degrees_lng);
  hdr->base.bounds.bottom = int_to_fixed(db_lat - degrees_lat);
  hdr->tiles_lat = degrees_lat;
  hdr->tiles_lng = degrees_lng;

  hdr->base.min_scale = 0;        // always visible
  hdr->base.max_scale = 100000;    // max 100km visibility

  hdr->resolution = make_fixed(0, 0, 30);     // 30 arc-seconds
  hdr->tile_modulo = 120;         // 120 pixels/tile
  hdr->tiles_lng = degrees_lng;
  hdr->tiles_lat = degrees_lat;

  uint32_t file_ptr = hdr_size;          // offset to first tile block

  uint32_t *tile_indexes = ((uint32_t*)(((uint8_t *)hdr) + sizeof(terrain_container_header_t)));

  fstream os(output_path, ios_base::binary | ios_base::trunc | ios_base::out);
  // output a matrix of pointers in [lng][lat] format
  for (int lng = db_lng; lng < (db_lng + degrees_lng); lng++)
    {
    for (int lat = db_lat; lat > (db_lat - degrees_lat); lat--)
      {
      tile_t* tile = tile_at(lng, lat);

      if (tile != 0)
        {
        contents.push_back(tile);

        int lng_offset = lng - db_lng;
        int lat_offset = db_lat - lat;

        int tile_offset = lng_offset * degrees_lat;
        tile_offset += lat_offset;

        // hdr->tiles[lng][lat] = file_ptr;
        tile_indexes[tile_offset] = file_ptr;

        // cout << "Write tile " << lat << ": " << lng << " at file offset " << file_ptr << endl;

        file_ptr += tile_size;
        }
      }
    }

    os.write(reinterpret_cast<char *>(hdr), hdr_size);

    // now write each of the 56k tiles
    for(size_t i = 0; i < contents.size(); i++)
      os.write(reinterpret_cast<const char *>(contents[i]), tile_size);

    os.flush();
    os.close();
#endif

  return 0;
  }
