#include "db_priv.h"

typedef struct _terrain_container_t {
  base_t base;
  handle_t db;              // atom database
  handle_t stream;          // storage stream
  const terrain_container_header_t *hdr; // cached header

  } terrain_container_t;


static result_t destroy_db(handle_t handle)
  {
  terrain_container_t* db = (terrain_container_t*)handle;

  close_handle(db->stream);

  return s_ok;
  }

static typeid_t terrain_type = 
  {
  .name = "terrain",
  .base = &spatial_container_type,
  .etherealize = destroy_db
  };

result_t tile_at(handle_t db, int32_t lat, int32_t lng, tile_t* tile)
  {
  result_t result;

  terrain_container_t *terrain_db;
  if(failed(result = is_typeof(db, &terrain_type, (void **)&terrain_db)))
    return result;

  // the request needs to be loaded.  first determine if the file contains the block
  // lng is -180 to 180  (W-->R)
  // lat is 90 to -90    (N-->S)

  if (lng < -180 || lng > 179 || lat > 90 || lat < -90)
    return e_bad_parameter;

  lng += 180;     // make 0..359
  lat += 90;      // make 0-179

  lat = 180 - lat;  // index is first row = 90 degrees north


  uint32_t index_block_offset = (lng * terrain_db->hdr->tiles_lat) + lat;
  index_block_offset <<= 2;

  uint32_t hdr_size = sizeof(terrain_container_header_t);
  index_block_offset += hdr_size;

  // this handles 2 possible situations.  Firstly the block cache is un-used
  // in which case a tile is allocated and the cache filled.
  // otherwise the tile is replaced and the cache filled.
  uint32_t tile_offset = 0;
  if (failed(result = stream_setpos(terrain_db->stream, index_block_offset, STREAM_SEEK_SET)) ||
    failed(result = stream_read(terrain_db->stream, &tile_offset, sizeof(uint32_t), 0)))
    return result;

  if (tile_offset == 0)
    return e_not_found;

  // calc index of block target.
  if (failed(result = stream_setpos(terrain_db->stream, tile_offset, STREAM_SEEK_SET)))
    return result;

  return  stream_read(terrain_db->stream, tile, sizeof(tile_t), 0);
  }

result_t open_terrain_container(handle_t atom_db, handle_t stream, const terrain_container_header_t* hdr, handle_t* out)
  {
  result_t result;
  terrain_container_t *hndl;

  if(failed(result = neutron_malloc(sizeof(terrain_container_t), (void**)&hndl)))
    return result;

  hndl->base.type = &terrain_type;
  hndl->db = atom_db;
  hndl->hdr = hdr;
  hndl->stream = stream;

  *out = hndl;

  return s_ok;
  }