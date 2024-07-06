#ifndef __db_priv_h__
#define __db_priv_h__

#include "db.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern const typeid_t spatial_container_type;
/**
 * @brief Open a container by its name (case insensitive)
 * @param db    atom database handle
 * @param name  name of the container
 * @param type  type to check against
 * @param hndl  stream handle
 * @param hdr   header read from container
 * @return s_ok if the container exists
*/
extern result_t find_container(handle_t db, const char *name, handle_t* hndl, const db_header_t** hdr);

/**
 * @brief Open a container for a terrain container
 * @param atom_db Database being opened
 * @param hdr     Header read from the DB
 * @param hndl    Resulting container handle
 * @return s_ok if created ok
*/
extern result_t open_terrain_container(handle_t atom_db, handle_t stream, const terrain_container_header_t*hdr, handle_t *hndl);
/**
 * @brief Open a container for a geospatial container
 * @param atom_db Database being opened
 * @param hdr     Header read from the DB
 * @param hndl    Resulting container handle
 * @return s_ok if created ok
*/
extern result_t open_geospatial_container(handle_t atom_db, handle_t stream, const geospatial_container_header_t* hdr, handle_t* hndl);
/**
 * @brief Open a container for an index container
 * @param atom_db Database being opened
 * @param hdr     Header read from the DB
 * @param hndl    Resulting container handle
 * @return s_ok if created ok
*/
extern result_t open_index_container(handle_t atom_db, handle_t stream, const index_container_header_t* hdr, handle_t* hndl);

#if defined(__cplusplus)
  }
#endif

#endif
