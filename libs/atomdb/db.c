#include "db_priv.h"
#include "../neutron/type_vector.h"
#include "../neutron/vector.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif

// this is used to alias the actual Database header.
typedef struct PACKED _container_hdr_t {
  db_header_t hdr;
  uint8_t reserved[32];
  } container_hdr_t;

// This contains the header and actual database that
// is opened
typedef struct _container_handle_t {
  container_hdr_t hdr;
  handle_t container;         // initially 0, but will be cached
  } container_handle_t;

typedef container_handle_t *container_handle_p;

vector_t(container_handle_p);

typedef struct _atom_db_t {
 base_t base;
 container_handle_ps_t containers;
 } atom_db_t;

static result_t destroy_atom_db(handle_t hndl)
  {
  atom_db_t *db = (atom_db_t *)hndl;

  //for (container_handle_p *it = container_handle_ps_begin(&db->containers); it != container_handle_ps_end(&db->containers); it++)
  //  close_handle((*it)->container);

  return s_ok;
  }

static const typeid_t atom_db_type =
  {
  .name = "atom_db",
  .etherealize = destroy_atom_db
  };

result_t open_atom_db(const char* path, uint32_t* num_containers, handle_t* out)
  {
  // open an atom database and load all of the headers
  // all spatial databases have a common 128 byte header
  result_t result;
  handle_t dirp;
  char* fname = 0;
  if (failed(result = open_directory(path, &dirp)))
    return result;

  atom_db_t* db;
  if (failed(result = neutron_calloc(1, sizeof(atom_db_t), (void**)&db)))
    {
    close_handle(dirp);
    return result;
    }

  db->base.type = &atom_db_type;

  // allocate a temp string
  if (failed(result = neutron_calloc(_MAX_DIR, 1, (void**)&fname)))
    goto exit;

  result = s_ok;
  stat_t entry;
  while (result == s_ok)
    {
    if (failed(result = read_directory(dirp, &entry)))
      {
      if(result == e_no_more_information)
        result = s_ok;

      goto exit;
      }

    // ignore special files
    if (strcmp(entry.name, ".") == 0 ||
      strcmp(entry.name, "..") == 0)
      continue;

    if (entry.type == FILE_TYPE_FILE)
      {
      // filename must end in .db
      size_t len = strlen(entry.name);

      if (len < 3 ||
        entry.name[len - 3] != '.' ||
        entry.name[len - 2] != 'd' ||
        entry.name[len - 1] != 'b')
        continue;

      // combine the paths
      strncpy(fname, path, _MAX_DIR - 1);
      size_t end = strlen(fname);
      fname[end++] = '/';
      fname[end] = 0;
      strncpy(fname + end, entry.name, _MAX_DIR - end - 1);

      handle_t stream;
      container_handle_t *handle;
      // try to open the file as a database
      if(failed(result = stream_open(fname, STREAM_O_RD, &stream)))
        goto exit;

      if (failed(result = neutron_calloc(1, sizeof(container_handle_t), (void **) &handle)))
        {
        close_handle(stream);
        goto exit;
        }

      uint32_t read;
      if (failed(result = stream_read(stream, &handle->hdr, sizeof(container_hdr_t), &read)) ||
        read != sizeof(container_hdr_t))
        {
        neutron_free(handle);
        close_handle(stream);
        goto exit;
        }

      // at present the version (length) is 128
      if (handle->hdr.hdr.magic != DB_MAGIC ||
        handle->hdr.hdr.length < sizeof(container_hdr_t))
        {
        neutron_free(handle);
        close_handle(db);
        result = e_bad_type;
        goto exit;
        }

      // load the container
      switch(handle->hdr.hdr.db_type)
        {
        case db_terrain:
          result = open_terrain_container(db, stream, (const terrain_container_header_t *) & handle->hdr, &handle->container);
          container_handle_ps_push_back(&db->containers, handle);   
          break;
        case db_spatial :
          result = open_geospatial_container(db, stream, (const geospatial_container_header_t*)&handle->hdr, &handle->container);
          container_handle_ps_push_back(&db->containers, handle);   
          break;
        case db_index :
          result = open_index_container(db, stream, (const index_container_header_t *) & handle->hdr, &handle->container);
          container_handle_ps_push_back(&db->containers, handle);
          break;
        default:
          result = e_bad_type;
          goto exit;
        }
      }
    }
 

exit:
  if (failed(result))
    close_handle(db);
  else
    {
    *num_containers = container_handle_ps_count(&db->containers);
    *out = db;
    }

  close_handle(dirp);
  neutron_free(fname);

  return result;
  }

result_t open_container(handle_t hndl, uint32_t container, handle_t* out, const db_header_t** hdr)
  {
  result_t result;
  atom_db_t* db;
  if (failed(result = is_typeof(hndl, &atom_db_type, (void**)&db)))
    return result;

  if(container >= container_handle_ps_count(&db->containers))
    return e_bad_parameter;

  container_handle_p *ch = container_handle_ps_begin(&db->containers) + container;
  *out = (*ch)->container;
  *hdr = &(*ch)->hdr.hdr;

  return s_ok;
  }

result_t find_container(handle_t hndl, const char* name, handle_t* out, const db_header_t** hdr)
  {
  result_t result;
  atom_db_t* db;
  if (failed(result = is_typeof(hndl, &atom_db_type, (void**)&db)))
    return result;

  for (container_handle_p* it = container_handle_ps_begin(&db->containers); it != container_handle_ps_end(&db->containers); it++)
    {
    if (strncasecmp((*it)->hdr.hdr.name,  name, sizeof((*it)->hdr.hdr.name)) == 0)
      {
      *out = (*it)->container;
      *hdr = &(*it)->hdr.hdr;

      return s_ok;
      }
    }

  return e_not_found;
  }

result_t enumerate_containers(handle_t hndl, size_t* num)
  {
  result_t result;
  atom_db_t *db;
  if(failed(result = is_typeof(hndl, &atom_db_type, (void **)&db)))
    return result;

  *num = container_handle_ps_count(&db->containers);

  return s_ok;
  }
