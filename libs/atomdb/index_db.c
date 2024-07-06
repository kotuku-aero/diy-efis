#include "db_priv.h"
#include <ctype.h>

typedef struct _index_container_t {
  base_t base;

  handle_t stream;					// stream that holds the index
  handle_t db;							// database 
  const index_container_header_t* hdr;

  index_node_t root;         // the root of the tree
  } index_container_t;

static result_t destory_container(handle_t db)
  {
  index_container_t *cont = (index_container_t *)db;

  close_handle(cont->stream);

  return s_ok;
  }

static const typeid_t index_container_type =
  {
  .name = "indexdb",
  .base = &spatial_container_type,
  .etherealize = destory_container
  };


result_t open_spatial_index(handle_t db, handle_t* tree)
  {
  result_t result;
  index_container_t* index;
  if (failed(result = neutron_calloc(1, sizeof(index_container_t), (void**)&index)))
    return result;

  index->base.type = &index_container_type;

  // read the root node from the file.
  uint32_t read = 0;
  if (failed(result = stream_setpos(db, 0, STREAM_SEEK_SET)) ||
    failed(result = stream_read(db, &index->root, sizeof(index_node_t), &read)) ||
    read != sizeof(index_node_t))
    {
    neutron_free(index);
    if (succeeded(result))
      result = e_bad_type;

    return result;
    }

  *tree = index;

  return s_ok;
  }


result_t open_index_container(handle_t atom_db, handle_t stream, const index_container_header_t* hdr, handle_t* out)
  {
  result_t result;
  index_container_t* hndl;

  if (failed(result = neutron_malloc(sizeof(index_container_t), (void**)&hndl)))
    return result;

  // create a handle to the spatial database
  hndl->base.type = &index_container_type;
  hndl->db = atom_db;
  hndl->hdr = hdr;
  hndl->stream = stream;

  *out = hndl;

  return s_ok;
  }

static result_t add_entity_unique(spatial_entity_ids_t* entities, const spatial_entity_id_t* key)
  {
  for (spatial_entity_id_t* it = spatial_entity_ids_begin(entities); it != spatial_entity_ids_end(entities); it++)
    {
    if (it->offset == key->offset)
      return s_ok;
    }

  spatial_entity_ids_push_back(entities, key);

  return s_ok;
  }

static result_t is_entity_contained(index_container_t* cont, const index_key_t* key, uint64_t oid)
  {
  if (key->length == 1 && key->value == oid)
    return s_ok;

  result_t result;
  if (failed(result = stream_setpos(cont->stream, (int64_t)key->value, STREAM_SEEK_SET)))
    return result;    // IO error

  for (uint32_t i = 0; i < key->length; i++)
    {
    uint32_t read = 0;
    uint64_t key_oid;
    if (failed(result = stream_read(cont->stream, &key_oid, sizeof(uint64_t), &read)))
      return result;

    if (read != sizeof(uint64_t))
      return e_no_more_information;

    if (oid == key_oid)
      return s_ok;
    }

  return s_false;
  }

static result_t resolve_entity_id(index_container_t* cont, uint32_t i, const index_key_t* key, spatial_entity_id_t* id)
  {
  return e_not_implemented;
  }

static result_t scan_recursive(index_container_t* cont, spatial_entity_ids_t* filter,
  compare_key_fn* query, void* params,
  const index_node_t* node,
  spatial_entity_ids_t* entities)
  {
  result_t result;
  
  // work from the root node and call the query function
  for (uint32_t i = 0; i < node->num_entries; i++)
    {
    const index_key_t* key = node->keys + i;
    bool in_filter = false;
    if (filter != 0)
      {
      for (spatial_entity_id_t* it = spatial_entity_ids_begin(filter); it != spatial_entity_ids_end(filter); it++)
        {
        result = is_entity_contained(cont, key, it->offset);
        if (result == s_false)
          continue;

        if (failed(result))
          return result;    // IO Error

        in_filter = true;
        break;
        }
      }
    else
      in_filter = true;

    if (!in_filter)
      continue;

    // see if the key matches the filter
    int result = (*query)(cont, key, params);

    if (result == 0)
      {
      // possibly there are many id's
      for (uint32_t i = 0; i < key->length; i++)
        {
        spatial_entity_id_t entity_id;
        if (failed(result = resolve_entity_id(cont, i, key, &entity_id)))
          return result;

        // add to the list of matched entities
        add_entity_unique(entities, &entity_id);
        }
      }
    else if (result < 0)
      {
      // see if the next is a child
      if (key->child != 0)
        {
        index_node_t *child;
        if(failed(result = neutron_malloc(sizeof(index_node_t), (void**)&child)))
          return result;

        uint32_t read = 0;
        // read the child node
        if (failed(result = stream_setpos(cont->stream, key->child, STREAM_SEEK_SET)) ||
          failed(result = stream_read(cont->stream, child, sizeof(index_node_t), &read)))
          {
          neutron_free(child);
          return result;
          }

        if (read != sizeof(index_node_t))
          {
          neutron_free(child);
          return e_unexpected;
          }

        result = scan_recursive(cont, filter, query, params, child, entities);

        neutron_free(child);

        if(failed(result))
          return result;
        }
      }
    else 
      break;    // key is > node so end of scan
    }


  return result;

  }

result_t query_spatial_entities(handle_t db, spatial_entity_ids_t* filter,
  compare_key_fn* query, void* params,
  spatial_entity_ids_t* entities)
  {
  result_t result;
  index_container_t* cont;
  if (failed(result = is_typeof(db, &index_container_type, (void**)&cont)))
    return result;

  return scan_recursive(cont, filter, query, params, &cont->root, entities);
  }
