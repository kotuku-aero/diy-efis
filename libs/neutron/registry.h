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
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __registry_h__
#define	__registry_h__

#ifdef	__cplusplus
extern "C"
  {
#endif
  
#include "neutron.h"
  // a memid descrbes a block which id 32 bytes
#define BLOCK_SHIFT 5
#define BLOCK_SIZE (1 << BLOCK_SHIFT)
#define BLOCK_MASK (BLOCK_SIZE -1)
#define BLOCKS_PER_SECTOR 4
#define SECTOR_SIZE 128
#define SECTOR_MASK (SECTOR_SIZE-1)
#define SECTOR_SHIFT 7
#define SECTORS_PER_CLUSTER 16
#define CLUSTER_MASK (SECTORS_PER_CLUSTER-1)
#define CLUSTER_SHIFT 4
#define CLUSTER_SIZE (SECTOR_SIZE * SECTORS_PER_CLUSTER)
  // this field is 28 bytes long
typedef struct _can_field_definition_t {
  // Length MUST be the first 2 bytes
	uint16_t length;                // sizeof(field_type_x)
	memid_t memid;              		// memory ID used to access this parameter
	memid_t parent;									// index of the parent key (if > 0)
	memid_t next;										// next sibling
	memid_t previous;								// previous sibling
	char name[REG_NAME_MAX+1];     	// Name of the parameter 0 terminated
	uint8_t data_type;              // type of data stored or published
	} field_definition_t;
  
typedef struct _field_int16_t {
	field_definition_t hdr;
	int16_t value;
	} field_int16_t;
	
typedef struct _field_uint16_t {
	field_definition_t hdr;
	uint16_t value;
	} field_uint16_t;
	
typedef struct _field_int32_t {
	field_definition_t hdr;
	uint32_t value;
	} field_int32_t;
	
typedef struct _field_uint32_t {
	field_definition_t hdr;
	uint32_t value;
	} field_uint32_t;
	
typedef struct _field_float_t {
	field_definition_t hdr;
	float value;
	} field_float_t;
	
typedef struct _field_xyz_t {
	field_definition_t hdr;
	xyz_t value;
	} field_xyz_t;
	
typedef struct _field_matrix_t {
	field_definition_t hdr;
	matrix_t value;
	} field_matrix_t;
	
typedef struct _field_qtn_t {
	field_definition_t hdr;
	qtn_t value;
	} field_qtn_t;
	
typedef struct _field_lla_t {
	field_definition_t hdr;
	lla_t value;
	} field_lla_t;
	
// A string occupier 2 blocks.  Hdr is 2 blocks and max string setting
// is 36 bytes
typedef struct _field_string_t {
	field_definition_t hdr;
  char value[36];
  } field_string_t;

  // this MUST be 32 bytes
typedef struct _field_key_t {
  field_definition_t hdr;
  uint16_t first_child;
  uint16_t last_child;
  } field_key_t;

typedef struct _field_bool_t {
  field_definition_t hdr;
  bool value;
  } field_bool_t;

// total size is 64 bytes
typedef struct _field_stream_t {
  field_definition_t hdr;
  // length of the stream.  Will be 0..65535
  uint16_t length;
  // extent of the file.  Can be > length but never less
  uint16_t extent;
  memid_t sectors[SECTORS_PER_CLUSTER];
  } field_stream_t;

typedef union {
  field_key_t key_f;
  field_int16_t int16_f;
  field_uint16_t uint16_f;
  field_int32_t int32_f;
  field_uint32_t uint32_f;
  field_float_t float_f;
  field_xyz_t xyz_f;
  field_matrix_t matrix_f;
  field_string_t string_f;
  field_bool_t bool_f;
  field_qtn_t qtn_f;
  field_lla_t lla_f;
  field_stream_t stream_f;
  } can_field_t;

  /**
   * Allocate a block of storage.
   * @param length          memory to allocate
   * @param stream_storage  true if allocation is from end of store
   * @param memid           memid to store
   * @return s_ok if block is allocated
   */
extern result_t allocate_memid(uint16_t length, memid_t *memid);
/**
 * Release a memid back to the pool
 * @param memid       memid to release
 * @param memid_size  Size of the allocated block
 * @return s_ok if freed ok.
 */
extern result_t release_memid(memid_t memid, uint16_t memid_size);
/**
 * Find the ID of a parameter
 * @param parent        parent node to search within, 0 for the root node
 * @param name          name of the parameter to find
 * @param datatype      optional datatype of the found node
 * @param length        optional length of the parameter to be returned
 * @param memid         found identifier
 * @return s_ok if the parameter was found
 */
extern result_t reg_get_value(memid_t parent,
                               const char *name,
                               uint8_t *datatype,
                               uint16_t *length,
                               memid_t *memid,
                               uint16_t *data_length,
                               void *data);
/**
 * Define a parameter to be stored in the system.
 * @param parent    parent to define under
 * @param name      Name header to be filled in
 * @param datatype  type of data to define.
 * @param length    Length of parameter to be stored
 * @param p_data    Pointer to data to initialize definition with
 * @return s_ok if created ok
 */
extern result_t reg_set_value(memid_t parent,
                              const char *name,
                              field_datatype datatype,
                              uint16_t length,
                              const void *p_data,
                              memid_t *memid);
/**
 * read a block of bytes from the persistent device
 * @param byte_offset       offset into the block device
 * @param bytes_to_read     number of bytes to read
 * @param buffer            buffer to read into
 * @return s_ok if block read ok
 */
extern result_t reg_read_bytes(uint32_t byte_offset,
                               uint16_t bytes_to_read,
                               void *buffer);
/**
 * Write a block of memory to the persistent device
 * @param byte_offset       offset into the block device
 * @param bytes_to_write    Number of bytes to write
 * @param buffer            buffer to write
 * @return s_ok if written ok
 */
extern result_t reg_write_bytes(uint32_t byte_offset,
                                uint16_t bytes_to_write,
                                const void *buffer);

/**
 */
extern void enter_registry();

/**
 */
extern result_t exit_registry(result_t exit_code);

extern result_t reg_query_child_impl(memid_t parent, const char *name, memid_t *key, field_datatype *type, uint16_t *len);
extern result_t reg_query_memid_impl(memid_t memid, field_datatype *type, char *name, uint16_t *length, memid_t *parent);
extern result_t reg_delete_value_impl(memid_t memid, const char *name);

#ifdef	__cplusplus
  }
#endif

#endif	/* REGISTRY_H */

