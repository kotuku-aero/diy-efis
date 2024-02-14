This project encapsulates the client side tools library to create a mapping database.  The mapping database is an optimized structure for use on small systems.

The database contains a 128 byte header that describes the contents of the database.  The header exists at the project location:

libs\atomdb\atomdb_schema.h

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

This library has routines to write this header and subsequent headers.  Depending on the type of database there will be a different header.  The database types are:

- Terrain - stores pixel maps 120x120 pixels that are dependent on the resolution.  For 30 arc seconds data from GTOPO30 this is 1 degree, for 15 arc seconds this is 0.5 degrees and for 
7.5 arc seconds this is 0.25 degrees.  The terrain data is stored with calculated hillshading using the tool gtopo2db
- Spatial - this is the most common, but also smallest database type.  It stores vector information for contours, airspaces, routes etc.  Created by a variety of tools.
- Index - After the header and spatial index the system stores strings.  String are stored in 
utf-8 and the system will re-use strings where possible.

## Strings

To save space strings are stored in the spatial database in packed form.  The code in this
library will coalesce strings and re-use string fragments.  Any strings stored in spatial
entities are referred to using a string reference:


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

A Spatial index can then index a field in a spatial database such as a waypoint name using the string reference only.
