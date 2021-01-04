/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "registry.h"
#include "stream.h"
#include <string.h>
#include <stdlib.h>

typedef struct _regstream_handle_t
  {
  stream_handle_t stream;
  field_stream_t field;     // descriptor in the registry
  uint32_t offset;          // current offset in the stream.
  memid_t cluster[16];      // cached cluster.
  memid_t cluster_id;       // if of cached cluster
  } regstream_handle_t;

static result_t check_handle(handle_t hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *) hndl;
  if (stream->version != sizeof (regstream_handle_t))
    return e_bad_parameter;

  return s_ok;
  }

static result_t regstream_eof(stream_handle_t *stream);
static result_t regstream_read(stream_handle_t *stream, void *buffer, uint16_t size, uint16_t *read);
static result_t regstream_write(stream_handle_t *stream, const void *buffer, uint16_t size);
static result_t regstream_getpos(stream_handle_t *stream, uint32_t *pos);
static result_t regstream_setpos(stream_handle_t *stream, uint32_t pos);
static result_t regstream_length(stream_handle_t *stream, uint32_t *length);
static result_t regstream_truncate(stream_handle_t *stream, uint32_t length);
static result_t regstream_close(stream_handle_t *stream);
static result_t regstream_delete(stream_handle_t *stream);
static result_t regstream_path(stream_handle_t *hndl, bool full_path, uint16_t len, char *path);

static void init_stream(regstream_handle_t *stream)
  {
  memset(stream, 0, sizeof (regstream_handle_t));

  // set up the callbacks
  stream->stream.version = sizeof(regstream_handle_t);
  stream->stream.stream_eof = regstream_eof;
  stream->stream.stream_getpos = regstream_getpos;
  stream->stream.stream_length = regstream_length;
  stream->stream.stream_read = regstream_read;
  stream->stream.stream_setpos = regstream_setpos;
  stream->stream.stream_truncate = regstream_truncate;
  stream->stream.stream_write = regstream_write;
  stream->stream.stream_delete = regstream_delete;
  stream->stream.stream_path = regstream_path;
  stream->stream.stream_close = regstream_close;
  }

static result_t create_handle(memid_t parent, const char *name, bool create, stream_p *stream)
  {
  result_t result;
  uint8_t datatype = 0;
  uint16_t length;
  memid_t stream_memid;
  
  enter_registry();

  result = reg_get_value(parent, name, &datatype, &length, &stream_memid, 0, 0);

  regstream_handle_t *new_stream = 0;

  if (failed(result))
    {
    if (!create)
      return exit_registry(e_path_not_found);

    // create the stream
    new_stream = (regstream_handle_t *) neutron_malloc(sizeof (regstream_handle_t));
    init_stream(new_stream);

    // and create the key.
    if(failed(result = reg_set_value(parent, name, field_stream,
                                     sizeof(field_stream_t) - sizeof(field_definition_t),
                                     &new_stream->field.length, &stream_memid)))
      return exit_registry(result);

    // update the header for debugging only!
    new_stream->field.hdr.length = sizeof(field_stream_t);
    new_stream->field.hdr.data_type = field_stream;
    new_stream->field.hdr.parent = parent;
    strncpy(new_stream->field.hdr.name, name, sizeof(new_stream->field.hdr.name));
    new_stream->field.hdr.memid = stream_memid;
    }
  else
    {
    // can't create if it exists
    if (create)
      return exit_registry(e_exists);

    if (datatype != field_stream)
      return exit_registry(e_wrong_type);

    if (length != sizeof (field_stream_t))
      return exit_registry(e_wrong_type);

    // create a stream
    new_stream = (regstream_handle_t *) neutron_malloc(sizeof (regstream_handle_t));
    init_stream(new_stream);
    // read the values  (TODO: check this!!!!)
    if (failed(result = reg_get_value(parent, name, &datatype, &length, &stream_memid, &length, &new_stream->field)))
      {
      neutron_free(new_stream);
      return exit_registry(result);
      }
    }

  *stream = (stream_p)new_stream;
  return exit_registry(s_ok);
  }

result_t reg_stream_open(memid_t parent, const char *path, stream_p *stream)
  {
  return create_handle(parent, path, false, stream);
  }

result_t reg_stream_create(memid_t parent, const char *path, stream_p *stream)
  {
  return create_handle(parent, path, true, stream);
  }

static result_t regstream_eof(stream_handle_t *hndl)
  {
  regstream_handle_t *stream = (regstream_handle_t *)hndl;

   return stream->offset >= stream->field.length ? s_ok : s_false;
  }

/**
 * Ensure that the stream is expanded to at least the size requested
 * @param stream      Stream to expand
 * @param size        size requested
 * @return s_ok if the stream is expaneded as requested
 */
static result_t ensure_stream(regstream_handle_t *stream, uint32_t size)
  {
  result_t result;
  // this will allocate blocks if needed
  if (stream->field.length > size)
    return s_ok;

  uint32_t alloc_size = ((size - 1) | (SECTOR_SIZE - 1)) + 1;
  
  if(stream->field.extent <= CLUSTER_SIZE &&
     alloc_size <= CLUSTER_SIZE)
    {
    // stream is a level0 stream so we just allocated memid's till the cluster is full
    uint32_t i;

    for (i = 0; i < SECTORS_PER_CLUSTER && alloc_size > 0; i++, alloc_size -= SECTOR_SIZE)
      {
      if (stream->field.sectors[i] == 0)
        {
        if (failed(result = allocate_memid(SECTOR_SIZE, &stream->field.sectors[i])))
          return result;

        stream->field.extent += SECTOR_SIZE;
        }
      }

    // we set the values
    return reg_set_value(stream->field.hdr.parent,
      stream->field.hdr.name,
      field_stream,
      sizeof(regstream_handle_t) - sizeof(field_definition_t),
      &stream->field.length, 0);
    }

  memid_t memid;

  // size must be > CLUSTER_SIZE
  if (stream->field.extent <= CLUSTER_SIZE)
    {
    // convert to a 2 level stream
    if (failed(result = allocate_memid(SECTOR_SIZE, &memid)))
      return result;

    if (failed(result = reg_write_bytes(memid << BLOCK_SHIFT, 32, &stream->field.sectors)))
      return result;

    // cache it
    memcpy(stream->cluster, stream->field.sectors, 32);
    stream->cluster_id = memid;

    // assign the sector
    stream->field.sectors[0] = memid;
    // fill all with 00
    memset(&stream->field.sectors[1], 0, 30);

    // write the updated sector allocation next...
    // just so we keep the system correct if the next step fails
    if (failed(result = reg_set_value(stream->field.hdr.parent,
      stream->field.hdr.name,
      field_stream,
      sizeof(regstream_handle_t) - sizeof(field_definition_t),
      &stream->field.length, 0)))
      return result;
    }

  // allocate sectors till the file is at least the correct size...
  uint32_t fp = ((stream->field.length-1) | (SECTOR_SIZE-1))+1;

  for (; fp < alloc_size; fp += SECTOR_SIZE)
    {
    // which cluster?
    uint32_t sector_num = fp >> SECTOR_SHIFT;
    uint32_t cluster_num = sector_num >> CLUSTER_SHIFT;
    sector_num &= CLUSTER_MASK;

    if (stream->field.sectors[cluster_num] == 0)
      {
      // new cluster, allocate it
      if (failed(result = allocate_memid(32,  &memid)))
        return result;

      memset(stream->cluster, 0, 32);
      stream->field.sectors[cluster_num] = memid;

      // write the updated sector allocation next...
      // just so we keep the system correct if the next step fails
      if (failed(result = reg_set_value(stream->field.hdr.parent,
        stream->field.hdr.name, field_stream, sizeof(regstream_handle_t) - sizeof(field_definition_t), &stream->field.length, 0)))
        return result;

      // save the cluster which is 0 bytes
      if (failed(result = reg_write_bytes(memid << BLOCK_SHIFT, 32, stream->cluster)))
        return result;

      stream->cluster_id = memid;
      }
    else if (stream->cluster_id != stream->field.sectors[cluster_num])
      {
      // loaded cluster is not the one we want.
      if (failed(result = reg_read_bytes(stream->field.sectors[cluster_num] << BLOCK_SHIFT, 32, stream->cluster)))
        return result;

      stream->cluster_id = stream->field.sectors[cluster_num];
      }

    /* Check to see if the cluster sector is not allocated.  If so we
     * allocate the cluster.
    */
    if (stream->cluster[sector_num] == 0)
      {
      // allocate a sector.
      if (failed(result = allocate_memid(SECTOR_SIZE, &memid)))
        return result;

      // mask out the offset
      stream->cluster[sector_num] = memid;              // store data pointer

      // save the cluster which is 0 bytes
      if (failed(result = reg_write_bytes(stream->cluster_id << BLOCK_SHIFT, 32, stream->cluster)))
        return result;

      // a new sector is allocated (must be at end of stream)
      stream->field.extent += SECTOR_SIZE;
      }

    // we set the values
    if (failed(result = reg_set_value(stream->field.hdr.parent,
      stream->field.hdr.name,
      field_stream,
      sizeof(regstream_handle_t) - sizeof(field_definition_t),
      &stream->field.length, 0)))
      return result;
    }

  return s_ok;
  }

/**
 * Store a stream back to the registry
 * @param stream    stream to save
 * @return s_ok if stored ok
 */
static result_t save_stream(regstream_handle_t *stream)
  {
  // TODO:
  return s_ok;
  }

/**
 * Calculate the offset and maximum transfer size based on a file descriptor
 * assumes the file is extended
 * @param stream    The stream
 * @param offset    byte offset
 * @param memid     resulting device address
 * @param max_xfer  maximum bytes that can be trasferred in this sector.
 * if the next cluster is co-incident then this can be > SECTOR size
 * @return
 */
static result_t calculate_memid(regstream_handle_t *stream, uint32_t offset, memid_t *memid, uint16_t *max_xfer)
  {
  result_t result;
  if (memid == 0 || max_xfer == 0)
    return e_bad_parameter;

  /* calculate the sector that holds the offset required */
  uint32_t sector = offset >> SECTOR_SHIFT;

  *max_xfer = 0;

  // see if this is a level0 stream
  if (stream->field.extent <= CLUSTER_SIZE)
    {
    // pick up the memid
    *memid = stream->field.sectors[sector];

    // see if the sector is co-incident
    if (*memid == 0)
      *max_xfer = 0;
    else
      {
      *max_xfer = SECTOR_SIZE - (offset & (SECTOR_MASK));
      memid_t id = *memid;
      for (sector++, id += BLOCKS_PER_SECTOR; sector < SECTORS_PER_CLUSTER; sector++, id += BLOCKS_PER_SECTOR)
        {
        if (stream->field.sectors[sector] == 0)
          break;
        if (stream->field.sectors[sector] != id)
          break;
        *max_xfer += SECTOR_SIZE;
        }
      }
    }
  else
    {
    // calculate whicj cluster to read
    uint32_t cluster = sector >> CLUSTER_SHIFT;
    memid_t cluster_id = stream->field.sectors[cluster];
    if (stream->cluster_id != cluster_id)
      {
      // read the cluster into the cache
      if (failed(result = reg_read_bytes(cluster_id << BLOCK_SHIFT, 32, stream->cluster)))
        return result;

      stream->cluster_id = cluster_id;
      }

    // mask out the cluster number
    sector &= CLUSTER_MASK;
    *memid = stream->cluster[sector];
    // see if the sector is co-incident
    if (*memid == 0)
      *max_xfer = 0;
    else
      {
      *max_xfer = SECTOR_SIZE - (offset & (SECTOR_MASK));
      memid_t id = *memid;
      for (sector++, id += BLOCKS_PER_SECTOR; sector < SECTORS_PER_CLUSTER; sector++, id += BLOCKS_PER_SECTOR)
        {
        if (stream->cluster[sector] == 0)
          break;
        if (stream->cluster[sector] != id)
          break;
        *max_xfer += SECTOR_SIZE;
        }
      }
    }

  return s_ok;
  }

static result_t regstream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  memid_t read_memid;
  uint16_t bytes_available;
  uint16_t read_offset = stream->offset & SECTOR_MASK;
  uint16_t bytes_read = 0;
  uint16_t bytes_to_read = 0;

  if (read != 0)
    *read = 0;
  
  enter_registry();

  while(size > 0)
    {
    // check if at or past end of file.
    if (stream->offset >= stream->field.length)
      return e_no_more_information;

    /* The first time through this will read from the offset to the end
     * of the cluster.  Then read in multiples of a cluster (2048) with
     * the last read being a runt read
      */

    // find which memid contains the information
    if (failed(result = calculate_memid(stream, stream->offset, &read_memid, &bytes_available)))
      {
      if (result == e_no_more_information)
        {
        if (bytes_read > 0)
          return exit_registry(s_ok);
        }

      return exit_registry(result);
      }

    if (bytes_available == 0)
      return exit_registry(s_ok);

    bytes_to_read = bytes_available;

    if(bytes_to_read > size)
      bytes_to_read = size;

    if(failed(result = reg_read_bytes((read_memid << BLOCK_SHIFT) + read_offset, bytes_to_read, buffer)))
      return exit_registry(result);

    read_offset = 0;
    buffer = ((uint8_t *)buffer) + bytes_to_read;
    stream->offset += bytes_to_read;
    bytes_read += bytes_to_read;

    if (read != 0)
      *read += bytes_to_read;

    size -= bytes_to_read;
    }

  return exit_registry(s_ok);
  }

static result_t regstream_write(stream_handle_t *hndl, const void *buffer, uint16_t size)
  {
  result_t result;
  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  memid_t memid;
  uint16_t byte_offset;
  uint16_t max_write;
  uint16_t bytes_to_write;

  enter_registry();
  // seek to the address requested and allocate clusters to make sure it fits.
  if(failed(result = ensure_stream(stream, stream->offset + size)))
    return exit_registry(result);

  /*  First write will write to the end of the cluster
   * then writes up to a cluster then a runt write at the end
  */

  while(size > 0)
    {
    if(failed(result = calculate_memid(stream, stream->offset, &memid, &max_write)))
      return exit_registry(result);

    byte_offset = stream->offset & SECTOR_MASK;

    if (max_write > size)
      bytes_to_write = size;
    else
      bytes_to_write = max_write;

    if(failed(result = reg_write_bytes((memid << BLOCK_SHIFT) + byte_offset, bytes_to_write, buffer)))
      return exit_registry(result);

    buffer = ((const uint8_t *)buffer) + bytes_to_write;
    byte_offset += bytes_to_write;
    stream->offset += bytes_to_write;
    size -= bytes_to_write;

    if (stream->field.length < stream->offset)
      stream->field.length = stream->offset;
    }

  return exit_registry(reg_set_value(stream->field.hdr.parent, stream->field.hdr.name, field_stream,
    sizeof(regstream_handle_t) - sizeof(field_definition_t), &stream->field.length, 0));
  }

static result_t regstream_getpos(stream_handle_t *hndl, uint32_t *pos)
  {
  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  *pos = stream->offset;
  return s_ok;
  }

static result_t regstream_setpos(stream_handle_t *hndl, uint32_t pos)
  {
  result_t result;

  if (failed(result = check_handle(hndl)))
    return result;

  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  if(pos > stream->field.length)
    pos = stream->field.length;

  stream->offset = pos;
  return s_ok;
  }

static result_t regstream_length(stream_handle_t *hndl, uint32_t *length)
  {
  result_t result;

  if (failed(result = check_handle(hndl)))
    return result;

  regstream_handle_t *stream = (regstream_handle_t *)hndl;
  *length = (uint32_t) stream->field.length;

  return s_ok;
  }

static result_t regstream_truncate(stream_handle_t *hndl, uint32_t length)
  {
  result_t result;

  if (failed(result = check_handle(hndl)))
    return result;
  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  if(length >= stream->field.length)
    return e_bad_parameter;

  uint32_t size_required = ((length - 1) | (SECTOR_SIZE - 1)) + 1;
  uint32_t stream_size = stream->field.extent;    // make sure this points at the last cluster
  uint32_t sector = stream_size >> SECTOR_SHIFT;

  enter_registry();
  // process till done
  while (size_required < stream_size)
    {
    if (stream->field.extent <= CLUSTER_SIZE)
      {
      // small file.  Can only get smaller.
      sector = (stream_size-1) >> SECTOR_SHIFT;
      if (failed(result = release_memid(stream->field.sectors[sector], SECTOR_SIZE)))
        return exit_registry(result);


      stream_size -= SECTOR_SIZE;

      stream->field.sectors[sector] = 0;
      stream->field.extent -= SECTOR_SIZE;
      }
    else
      {
      // determine the cluster
      uint32_t cluster = (stream_size - 1) >> (CLUSTER_SHIFT + SECTOR_SHIFT);
      memid_t cluster_id = stream->field.sectors[cluster];

      if (cluster_id != 0)
        {
        if (cluster_id != stream->cluster_id)
          {
          // cache the cluster
          if (failed(result = reg_read_bytes(cluster_id << BLOCK_SHIFT, 32, stream->cluster)))
            return exit_registry(result);

          stream->cluster_id = cluster_id;
          }

        // mask the cluster number
        cluster = (stream_size - 1) >> SECTOR_SHIFT;
        cluster &= CLUSTER_MASK;
        memid_t release_id = stream->cluster[cluster];
        if (release_id != 0)
          {
          if (failed(result = release_memid(release_id, SECTOR_SIZE)))
            return exit_registry(result);

          stream->cluster[cluster] = 0;
          // save the cluster
          if (failed(result = reg_write_bytes(cluster_id << BLOCK_SHIFT, 32, stream->cluster)))
            return exit_registry(result);

          // see if they are all free
          if (cluster == 0)
            {
            if (failed(result = release_memid(cluster_id, SECTOR_SIZE)))
              return exit_registry(result);

            stream->cluster_id = 0;

            // restore cluster
            cluster = stream_size >> (CLUSTER_SHIFT + SECTOR_SHIFT);
            stream->field.sectors[cluster] = 0;
            }
          }
        }

      stream_size -= SECTOR_SIZE;
      stream->field.extent -= SECTOR_SIZE;

      if (stream->field.extent <= CLUSTER_SIZE)
        {
        // convert to a single extent record
        memid_t release_id = stream->field.sectors[0];
        if (release_id != stream->cluster_id)
          {
          // cache the cluster
          if (failed(result = reg_read_bytes(release_id << BLOCK_SHIFT, 32, stream->cluster)))
            return exit_registry(result);

          stream->cluster_id = release_id;
          }

        // copy the stream over
        memcpy(stream->field.sectors, stream->cluster, 32);

        if (failed(result = release_memid(release_id, SECTOR_SIZE)))
          return exit_registry(result);

        memset(stream->cluster, 0, 32);
        stream->cluster_id = 0;
        }
      }

    if (failed(result = reg_set_value(stream->field.hdr.parent, stream->field.hdr.name, field_stream,
      sizeof(regstream_handle_t) - sizeof(field_definition_t), &stream->field.length, 0)))
      return exit_registry(result);
    }

  stream->field.length = length;

  // save the new field descriptor
  return exit_registry(reg_set_value(stream->field.hdr.parent, stream->field.hdr.name, field_stream,
    sizeof(regstream_handle_t) - sizeof(field_definition_t), &stream->field.length, 0));
  }

static result_t regstream_close(stream_handle_t *hndl)
  {
  // we only have to release the memory as the state is persisted
  neutron_free(hndl);

  return s_ok;
  }

static result_t regstream_delete(stream_handle_t *hndl)
  {
  result_t result;

  if (failed(result = check_handle(hndl)))
    return result;
  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  if(failed(result = regstream_truncate(hndl, 0)))
    return result;

  enter_registry();
  // free the registry value
  if(failed(result = reg_delete_value_impl(stream->field.hdr.parent, stream->field.hdr.name)))
    return exit_registry(result);

  neutron_free(stream);

  return exit_registry(s_ok);
  }

static result_t get_full_path(memid_t key, uint16_t len, char *path)
  {
  char name[REG_NAME_MAX+1];

  result_t result;
  memid_t parent;
  
  enter_registry();
  
  if (failed(result = reg_query_memid_impl(key, 0, name, 0, &parent)))
    return exit_registry(result);
  
  exit_registry(result);

  name[REG_NAME_MAX] = 0;

  if (parent != 0)
    {
    if (failed(result = get_full_path(parent, len, path)))
      return result;
    }
  else
    path[0] = 0;

  size_t path_length = strlen(path);

  if (path_length > 0 &&
    (path_length > 1 || strcmp(path, "/") != 0))
    strncat(path, "/", len);

  strncat(path, name, len);

  return s_ok;
  }

static result_t regstream_path(stream_handle_t *hndl, bool full_path, uint16_t len, char *path)
  {
  result_t result;

  if (failed(result = check_handle(hndl)))
    return result;

  if (path == 0)
    return e_bad_parameter;

  regstream_handle_t *stream = (regstream_handle_t *)hndl;

  memid_t parent = stream->field.hdr.parent;

  if (!full_path || parent == 0)
    strncpy(path, stream->field.hdr.name, len);
  else
    {
    if (failed(result = get_full_path(parent, len, path)))
      return result;
    
    strncat(path, "/", len);
    strncat(path, stream->field.hdr.name, len);
    }

  return s_ok;
  }
