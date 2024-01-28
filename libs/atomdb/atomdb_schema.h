#ifndef __atomdb_schema_h__
#define __atomdb_schema_h__

#include "spatial.h"
#include "../neutron/vector.h"

// this file serves to define all of the common on-disk
// structure that is used for the atom_db.
//

#ifdef __cplusplus
extern "C" {
#endif

  /* This file describes the on-disk format of all spatial objects.
  * The definition of the spatial types is in spatial.h
  */
  typedef enum {
    db_null,                      // not a valid database type
    db_terrain,                   // a terrain database
    db_spatial,                   // some sort of goepspatial features
    db_index,                     // indexed to strings in another database
    } spatial_db_format;

  typedef struct PACKED _ymd_t {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    } ymd_t;

  // is KOTU
#define DB_MAGIC 0x54554b4f
/**
 * @brief All database files start with this header (96 bytes long)
*/
  typedef struct PACKED _db_header_t {
    uint32_t magic;               // header magic           4
    uint32_t length;              // size of the header     4
    spatial_db_format db_type;    // storage type of the database 4
    uint32_t content_type;        // what is stored in this database 4
    ymd_t effective_from;         // effective from        4
    ymd_t effective_to;           // when expires          4
    char name[32];                // database name          32
    spatial_rect_t bounds;        // area described by this database  16
    uint32_t string_table;        // offset to the strings        4
    uint32_t string_table_length; // length of the string table   4
    // the following are used to select display objects
    //
    uint16_t min_scale;           // min mtrs * 100 to display 2
    uint16_t max_scale;           // max mtrs * 100 to display (0 = no limit) 2
    uint8_t reserved[12];
    } db_header_t;

  /**
   * @brief This is referred to a string
   * in the database.  These are stored as packed
   * strings, and the function resolve_string() must be called
   * to resolve it.
   * Allows for overlapped string usage
   */
  typedef struct PACKED _spatial_string_t {
    uint32_t offset;      // offset into the database string table
    uint32_t length;      // length of the string in the packed table
    } spatial_string_t;

  /**
   * @brief An index that is stored on a node
  */
  typedef struct PACKED _index_key_t {
    uint32_t offset;            // offset into the database string table
    uint32_t length;            // if length == 1 then value -> entity otherwise a list of entities (4)
    uint32_t value;             // index of the entity (4)
    uint32_t child;             // if this node < key and next node > key then child node used
    } index_key_t;

  /**
   * @brief An index node stored in the index database (1024 byte page)
  */
  typedef struct PACKED _index_node_t index_node_t;
  typedef struct PACKED _index_node_t {
    uint32_t page_index;         // node page index
    uint32_t num_entries;       // current capacity
    uint32_t parent;            // index of the parent node, 0 = root node
    uint8_t reserved[16];       // reserved space to make 32 bytes
    uint32_t child;             // this is the child to the left of keys[0]
                                // in a leaf node this will always be 0
    index_key_t keys[31];       // index entries, each is 32 bytes
    } index_node_t;

  typedef struct PACKED _spatial_entity_id_t {
    uint32_t type;                    // spatial_entity_type type of entity
     // these 2 describe the header for the entity that is stored.
    // for basic types that is a spatial_entity_t but for
    // ARINC and other types is the details
    uint32_t offset;
    uint32_t length;
    // The cell index stores a list of entities that
    // are contained in the cell, this list points
    // to a pre-processed clipped list of graphics
    // that can be loaded and passed to the gdi (after rotation and scaling)
    // so that the memory constraints are not all used by
    // clipping useless data
    uint32_t graphics_offset;
    uint32_t graphics_length;
  } spatial_entity_id_t;

// These are the pre-defined entity types
#define SPATIAL_DEM         0x00
#define SPATIAL_POINT       0x01
#define SPATIAL_POLYLINE    0x02
#define SPATIAL_POLYGON     0x03

#define SPATIAL_TYPE_MASK   0x0F

// there are 4096 possible types (or bitmask)
#define PAYLOAD_TYPE_MASK   0x0000FFF0
#define PAYLOAD_TYPE_SHIFT  4
// there are 16 possible flags
#define PAYLOAD_FLAGS_MASK  0xFFFF0000
#define PAYLOAD_FLAGS_SHIFT 16
// payload can be selected
#define PAYLOAD_SELECTABLE  0x00010000

  /**
   * @brief Base of all stored spatial entities.
  */
  typedef struct PACKED _spatial_entity_hdr_t {
    uint32_t size;                  // is sizeof(spatial_entity_t) + entity_length + strings_length +  paths_length
    uint32_t type;                  // type of entity that follows (not defined here)
    spatial_rect_t bounding_box;    // box containing all points
    latlng_t center_pt;             // approximate center of points
    uint32_t entity_length;         // size of fixed entity data
    uint32_t strings_length;        // length of strings stored after the entity
    uint32_t paths_length;          // length of all paths stored after entity
    } spatial_entity_hdr_t;

  // after this header, there is stored the entity, strings, then paths

  /**
   * @brief These are stored in a terrain database.
  */
  typedef struct PACKED _pixel_t {
    int16_t elevation;      // elevation of the pixel
    uint8_t shade;          // shade to tint
  } pixel_t;
  /***************************************************************
   * @brief a database of terrain tiles
  */
  typedef struct PACKED _terrain_container_header_t {
    db_header_t base;                         // 96
    // resolution of the database.
    // the smaller the number, the greater the resolution
    //
    // Will be one of 30 arc-seconds, 15 arc-seconds or 7.5 arc-seconds
    // all systems come with a world-wide 30 arc-second database
    fixed_t resolution;     // used to add to pixel to index a tile 100
    int32_t tile_modulo;          // used to quickly select a tile 104
    // will be int(pos * tile_modulo)
    // 30 arc-seconds - 1 degree so 1 
    // 15 arc-seconds - 0.5 degrees so 2
    // 7.5 arc-seconds - 0.25 degrees so 4
  // pre-calculated so no abiguity
    uint32_t tiles_lng;           // number of longitude tiles 108
    // is calculated from bounds abs(easting - westing)
    uint32_t tiles_lat;           // number of latitude tiles 112
    // is calculated from bounds abs(norting - southing)
    uint8_t reserved[16];
    } terrain_container_header_t;
  // after the terrain_db_header is an array whose bounds are:
  //
  // uint32_t tile_offset[tiles_lat][tiles_lng];
  //
  // the offset is the absolute position of the tile in the file
  // if 0 then no tile (not landmass)
  //
  // after the variable length tile_offset array is a sequence of
  // 120x120 pixel tiles

  typedef struct PACKED _spatial_offset_t {
    uint32_t offset;        // offset of start of list in file
    uint32_t length;        // number of spatial_entity_id_t
    } spatial_offset_t;

  /*****************************************************
   * @brief data is stored as spatial entities, this is the header
  */
  typedef struct PACKED _geospatial_db_header_t {
    db_header_t base;           // 96

    // all spatial geography is based on a 1 degree index.
    // When written each polygon is indexed and stored in a tile
    // map
    // pre-calculated so no ambiguity
    uint32_t tiles_lng;           // number of longitude tiles  100
    // is calculated from bounds abs(easting - westing)         
    uint32_t tiles_lat;           // number of latitude tiles   104
    // is calculated from bounds abs(norting - southing)
    uint32_t num_index_lists;     // number of lists that are stored  112
    uint32_t num_entities;        // number of serialized entities  116
    uint8_t reserved[16];        // guid of a resources database that holds the strings
    // char strings[num_strings][]
    // spatial_offset_t spatial_index[tiles_lat][tiles_lng]
    // spatial_entity_id_t cell_lists[num_index_lists];
    // spatial_entity_t entities[n]     // variable length
    } geospatial_container_header_t;

  /*****************************************************
   * @brief Data is a B+Tree index
  */
  typedef enum _index_data_type {
    idt_string = 1,         // variable length case-insensitive string
    idt_int32 = 2,          // signed 32-bit integer
    idt_uint32 = 3,         // unsigned 32 bit integer
    idt_fixed = 4,          // fixed point 16.16
    idt_float = 5,          // 32bit floating point
    } index_data_type;

  typedef struct PACKED _index_db_header_t {
    db_header_t base;           // 96

    uint32_t root_offset;       // 4
    uint8_t data_type;          // one of the index_data_type constants
    uint8_t reserved[27];

    } index_container_header_t;
  // following this is the root node of the B+Tree index

#ifdef __cplusplus
}
#endif

#endif