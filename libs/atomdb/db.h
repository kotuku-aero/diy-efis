#ifndef __db_h__
#define __db_h__

#include "atomdb_schema.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
  latlng_t top_left;
  latlng_t bottom_left;
  latlng_t top_right;
  latlng_t bottom_right;
  } geo_rhombus_t;

/////////////////////////////////////////////////////////////////////
//
// Database operations
//

/**
 * @brief Open a folder as an atom database
 * @param path  Path to the folder that holds the database files
 * @param num_containers  number of containers in the database
 * @param db    handle to the opened database
 * @return s_ok if opened ok
*/
extern result_t open_atom_db(const char *path, uint32_t *num_containers, handle_t *db);
/**
 * @brief Enumerate the details of an atom database
 * @param db        database handle returned from open_atom_db
 * @param container index number 0..num_containers-1
 * @param num       number of containers found
 * @param hdr       pointer to the container header.  Only valid till close_handle called
 * @return s_ok if the database metadata is available
*/
extern result_t enumerate_containers(handle_t db, size_t *num);
/**
 * @brief Return a handle to a container.
 * @param db        opened database
 * @param container Container to reference
 * @param hndl      opened handle
 * @return s_ok if the container can be opened.
*/
extern result_t open_container(handle_t db, uint32_t container, handle_t *hndl, const db_header_t** hdr);

/////////////////////////////////////////////////////////////////////
//
// Spatial database operations
//

type_vector_t(spatial_entity_id);
// used to pass in types of entities to select 
typedef uint32_t spatial_entity_type;

/**
 * @brief look at the container header and load spatial entities
 * @param db        container to load (must be db_spatial type!)
 * @param hdr       cached header
 * @param bounds    bounding box of entities to select
 * @param types     types of entities to return
 * @param entities  resulting entities
 * @return s_ok if query executed ok
*/
extern result_t select_spatial_entities(handle_t db, const geo_rhombus_t *bounds,
  size_t num_types, const spatial_entity_type* types, spatial_entity_ids_t* entities);
/**
 * @brief Read blob data from a spatial entity.  Must respect that the
 * spatial entity is PACKED so offsets may not be as expected.
 * @param container Database to read from  (must be db_spatial type)
 * @param oid     Object ID as returned from a select
 * @param offset  offset into the spatial entity
 * @param length  bytes to read
 * @param buffer  buffer to read into
 * @param read    bytes read
 * @return s_ok if read ok
*/
extern result_t incarnate_spatial_entity(handle_t db, const spatial_entity_id_t *oid, spatial_entity_hdr_t** out);
extern result_t etherealize_spatial_entity(handle_t db, spatial_entity_hdr_t *entity);


/////////////////////////////////////////////////////////////////////
//
// index container operations
//

/**
 * @brief Comparison function to find a key
 * @param index index being compared
 * @param k1  first key to compare to
 * @param params optional application parameters
 * @return -1 = k1 < value, 0 = k1 == value, 1 = k1 > value
*/
typedef int (*compare_key_fn)(handle_t index, const index_key_t* k1, void* params);

/**
 * @brief look at the container header and load spatial entities
 * @param db        database to load (must be index type, and returned from open_index_db)
 * @param filter    previous result set (from select_spatial_entities, or query_spatial_entities)
 * @param query     query to execute on the indexes
 * @param params    parameters to query
 * @param entities  resulting entities
 * @return s_ok if query executed ok
*/
extern result_t query_spatial_entities(handle_t db, 
  spatial_entity_ids_t* filter,
  compare_key_fn* query, void *params,
  spatial_entity_ids_t* entities);

/////////////////////////////////////////////////////////////////////
//
// Terrain container operations
//

/**
 * @brief A tile is a 120 x 120 pixel area
 * for 30 arc seconds this is 1 degree, for
 * 15 arc seconds this is 0.5 degree and for
 * 7.5 arc seconds it is 0.25 degrees
*/
typedef pixel_t tile_t[120][120];

/**
 * @brief Read a tile from a terrain database
 * @param db    Database to read from (must be db_terrain type)
 * @param lng   longitude of the tile (x)
 * @param lat   lattitude of the tile (y)
 * @param out   where to store the tile
 * @return s_ok if the tile is available and read from the store
*/
extern result_t tile_at(handle_t db, int32_t lng, int32_t lat, tile_t *out);

/////////////////////////////////////////////////////////////////////
//
// Entity data operations
//
/**
 * @brief Resolve a spatial string
 * @param container  Database whose string table to use
 * @param ref     Reference to the string in the database
 * @param str     allocated temporary c string with trailing 0
 * @return s_ok if string allocated
 * @remark the caller must call neutron_free to release the
 * allocated string
*/
extern result_t resolve_string(handle_t container, const spatial_string_t* ref, char** str);
/**
 * @brief Load a pre-processed pointset into a polyline container
 * @param container Database to load from
 * @param ref       Spatial entity details
 * @param out       container to fill
 * @return s_ok if a valid reference
*/
extern result_t load_metafile(handle_t container, const spatial_entity_id_t*ref, polylines_t *out);

#if defined(__cplusplus)
}
#endif

#endif

