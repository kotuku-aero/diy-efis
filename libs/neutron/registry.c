#include "neutron.h"
#include "registry.h"

// a page is 64k bytes
#define PAGE_MASK (0x07FF)

static uint16_t num_bitmap_bytes;
static uint16_t page_size;

// semaphore used to ensure only 1 thread is using the registry
// at a time.
static handle_t mutex;

void enter_registry()
  {
  semaphore_wait(mutex, INDEFINITE_WAIT);
  }

result_t exit_registry(result_t exit_code)
  {
  semaphore_signal(mutex);
  
  return exit_code;
  }

#define REG_BLOCK_SIZE 128

// this is the header written to the start of the eeprom
typedef struct _eeprom_root_t {
  field_key_t hdr;
  uint8_t bitmap[];                // bitmap blocks
  } eeprom_root_t;

static eeprom_root_t *root;        // this is dynamically allocated
static uint32_t max_offset = 0;

static inline uint32_t get_block_offset(memid_t memid)
  {
  return ((uint32_t)memid) << BLOCK_SHIFT;
  }

/* CACHE read/write helpers
*/

typedef struct _cache_buffer_t cache_buffer_t;

typedef struct _cache_buffer_t
  {
  cache_buffer_t *next;
  cache_buffer_t *previous;
  uint32_t offset;
  uint8_t buffer[REG_BLOCK_SIZE];
  } cache_buffer_t;

static uint32_t reg_address_start;
static uint32_t reg_address_end;

#define NUM_CACHE_BLOCKS 64

static cache_buffer_t cache[NUM_CACHE_BLOCKS];
cache_buffer_t *head = 0;
cache_buffer_t *tail = 0;

#ifdef _WIN32
uint32_t cache_hit = 0;
uint32_t cache_read = 0;
uint32_t cache_miss = 0;
#endif

static void link_as_head(cache_buffer_t *buffer)
  {
  if (head == buffer)
    return;

  buffer->next = head;
  head->previous = buffer;
  buffer->previous = 0;
  head = buffer;
  }

static void link_as_tail(cache_buffer_t *buffer)
  {
  if (tail == buffer)
    return;

  if (head == 0)
    head = buffer;

  if (tail != 0)
    tail->next = buffer;

  buffer->previous = tail;
  buffer->next = 0;
  tail = buffer;
  }

static void unlink(cache_buffer_t *link)
  {
  if (link == head)
    {
    head = link->next;
    head->previous = 0;     // new head
    }
  else if (link == tail)
    {
    tail = tail->previous;
    tail->next = 0;         // new tail
    }
  else
    {
    if (link->previous != 0)
      link->previous->next = link->next;

    if (link->next != 0)
      link->next->previous = link->previous;
    }

  link->previous = 0;
  link->next = 0;
  }

static result_t cache_init(uint32_t max_read_address, uint32_t min_read_address)
  {
  reg_address_end = max_read_address;
  reg_address_start = min_read_address;

  memset(cache, 0, sizeof(cache));

  // create a linked-list in reverse order for the cache blocks
  for (uint32_t i = 0; i < NUM_CACHE_BLOCKS; i++)
    {
    cache_buffer_t *link = &cache[i];
    link_as_tail(link);
    }

  return s_ok;
  }

static result_t cache_reg_read_block(uint32_t offset, uint16_t bytes_to_read, void *dest)
  {
  result_t result = s_ok;
  // if reading bitmap just skip as this is startup
  if(offset < reg_address_start)
    return bsp_reg_read_block(offset, bytes_to_read, dest);

  // fill the cache where possible.  First get the first block
  uint32_t sector = offset & ~BLOCK_MASK;
  // initial setting
  uint32_t read_start = offset & BLOCK_MASK;

  uint8_t *u8_dest = (uint8_t *)dest;

  do
    {
    cache_buffer_t *found_sector = 0;
    cache_buffer_t *cache_buffer = head;

    // scan the cache (most frequent at head)
    while (cache_buffer != 0)
      {
      if (cache_buffer->offset == sector)
        {
        if (cache_buffer != head)
          {
          // found a block
          unlink(cache_buffer);
          link_as_head(cache_buffer);
          }
        found_sector = cache_buffer;
#ifdef _WIN32
        cache_hit++;
#endif
        break;
        }

      cache_buffer = cache_buffer->next;
      }

    if (found_sector == 0)
      {
#ifdef _WIN32
      cache_read++;
#endif

      // read the next block and cache it
      found_sector = tail;      // get last block
      unlink(found_sector);
      link_as_head(found_sector);

#ifdef _WIN32
      if (found_sector->offset != 0)
        cache_miss++;
#endif

      found_sector->offset = sector;

      if (failed(result = bsp_reg_read_block(sector, SECTOR_SIZE, found_sector->buffer)))
        {
        found_sector->offset = 0;
        return result;
        }
      }

    // we have a block, copy it
    uint16_t num_bytes = SECTOR_SIZE - read_start;
    if (num_bytes > bytes_to_read)
      num_bytes = bytes_to_read;

    memcpy(u8_dest, &found_sector->buffer[read_start], num_bytes);
    u8_dest += num_bytes;
    bytes_to_read -= num_bytes;
    read_start = 0;
    sector++;

    } while (bytes_to_read > 0);
    
  return result;
  }

static result_t cache_reg_write_block(uint32_t offset, uint16_t bytes_to_write, const void *src)
  {
  if (offset < reg_address_start)
    return bsp_reg_write_block(offset, bytes_to_write, src);

  result_t result = s_ok;

  // fill the cache where possible.  First get the first block
  uint32_t sector = offset & ~BLOCK_MASK;
  // initial setting
  uint32_t write_start = offset & BLOCK_MASK;

  const uint8_t *u8_src = (const uint8_t *)src;

  do
    {
    cache_buffer_t *found_sector = 0;
    cache_buffer_t *cache_buffer = head;

    // scan the cache (most frequent at head)
    while (cache_buffer != 0)
      {
      if (cache_buffer->offset == sector)
        {
        if (cache_buffer != head)
          {
          // found a block
          unlink(cache_buffer);
          link_as_head(cache_buffer);
          }

        found_sector = cache_buffer;

        break;
        }

      cache_buffer = cache_buffer->next;
      if (cache_buffer == 0)
        break;
      }

    // we have a block, copy it
    uint16_t num_bytes = SECTOR_SIZE - write_start;
    if (num_bytes > bytes_to_write)
      num_bytes = bytes_to_write;

    // update lookaside cache.
    if (found_sector != 0)
      memcpy(&found_sector->buffer[write_start], u8_src, num_bytes);

    if (failed(result = bsp_reg_write_block(sector + write_start, num_bytes, u8_src)))
      return result;

    u8_src += num_bytes;
    bytes_to_write -= num_bytes;
    write_start = 0;
    sector++;

    } while (bytes_to_write > 0);

    return result;
  }

result_t reg_read_bytes(uint32_t byte_offset,
                        uint16_t bytes_to_read,
                        void *buffer)
  {
  result_t result;
  
  if(byte_offset >= max_offset ||
     (byte_offset + bytes_to_read) > max_offset)
    return e_bad_parameter;

  // calculate the first block to read.
  uint16_t read_size = REG_BLOCK_SIZE - (byte_offset & 0x7f);
 
  while(bytes_to_read > 0)
    {
    // determine how many blocks to write
    bool last_read = false;

    // see if this is the last write
    if(bytes_to_read <= read_size)
      {
      read_size = bytes_to_read;
      last_read = true;
      }

    // last block is on a REG_BLOCK_SIZE byte page
    if(failed(result = cache_reg_read_block(byte_offset, read_size, buffer)))
      return result;

    if(last_read)
      return result;

    buffer = ((uint8_t *)buffer) + read_size;
    byte_offset += read_size;
    bytes_to_read -= read_size;

    // assume we write a full sector.
    read_size = SECTOR_SIZE;
    }

  return s_ok;
  }

result_t reg_write_bytes(uint32_t byte_offset,
                         uint16_t bytes_to_write,
                         const void *buffer)
  {
  result_t result;
  
  if(byte_offset >= max_offset ||
     (byte_offset + bytes_to_write) > max_offset)
    return e_bad_parameter;

  // calculate the first block to write.
  uint16_t write_size = SECTOR_SIZE - (byte_offset & (SECTOR_SIZE-1));
  if(write_size > bytes_to_write)
    write_size = bytes_to_write;

  while(bytes_to_write > 0)
    {
    // determine how many blocks to write
    bool last_write = false;

    // see if this is the last write
    if(bytes_to_write <= write_size)
      {
      write_size = bytes_to_write;
      last_write = true;
      }

    // last block is on a REG_BLOCK_SIZE byte page
    if(failed(result = cache_reg_write_block(byte_offset, write_size, buffer)))
      return result;

    if(last_write)
      return result;

    buffer = ((uint8_t *)buffer) + write_size;
    byte_offset += write_size;
    bytes_to_write -= write_size;

    // assume we write a full sector.
    write_size = SECTOR_SIZE;
    }

  return s_ok;
  }

result_t bsp_reg_init(bool factory_reset, uint16_t num_blocks, uint16_t _page_size)
  {
  result_t result;

  // create the semaphore that stops access to the registry
  // by more than 1 thread
  semaphore_create(&mutex);
  semaphore_signal(mutex);
  
  max_offset = get_block_offset(num_blocks);

  num_bitmap_bytes = num_blocks >> 3;  // this is the number of bytes that make a bitmap
  page_size = _page_size;

  uint16_t num_reserved = num_bitmap_bytes >> BLOCK_SHIFT;    // blocks in the bitmap, excludes the root block

  if (failed(result = cache_init(num_blocks << BLOCKS_PER_SECTOR_SHIFT, (num_reserved +1) << BLOCK_SHIFT)))
    return result;
  
  // read the root block.
  if(failed(result = neutron_malloc(num_bitmap_bytes + sizeof(eeprom_root_t), (void **)&root)))
    return result;

  // read the blocks from the eeprom
  // Check for a valid eeprom
  if(factory_reset ||
     failed(reg_read_bytes(0, sizeof(eeprom_root_t), root)) ||    // initially just read the root block
     root->hdr.hdr.length != num_bitmap_bytes + sizeof(eeprom_root_t))
    factory_reset = true;     // something wrong with eeprom
  else
    {
    // not a factory reset, so read the block and the bitmap
    if(failed(result = reg_read_bytes(0, num_bitmap_bytes + sizeof(eeprom_root_t), root)))
      return result;
    
    // see if there is a init_db key, in that case same as a factory_reset
    bool init_db;
    if(succeeded(reg_get_bool(0, "init-db", &init_db)) &&
       init_db == true)
      factory_reset = true;     // this wastes the reg read above but not very often
    }
  
  if(factory_reset)
    {
    // clear out root blocks.
    memset(root, 0, num_bitmap_bytes + sizeof(eeprom_root_t));

    memset(root->bitmap, 0xFF, num_reserved >> 3);
    root->bitmap[num_reserved >> 3] = 0x01;         // allocate the block that is reserved for the header.
   
    root->hdr.hdr.data_type = field_key;
    root->hdr.hdr.length = num_bitmap_bytes + sizeof(eeprom_root_t);
    root->hdr.hdr.memid = 0;
    root->hdr.hdr.name[0] = '/';
    if(failed(result = reg_write_bytes(0, num_bitmap_bytes+ sizeof(eeprom_root_t), root)))
      return result;

    return e_not_initialized;
    }

  return s_ok;
  }

static void allocate_blocks(uint16_t first_block,
														uint16_t num_blocks)
	{
	// point to the first byte to allocate
  uint8_t *base = root->bitmap;
  base += first_block >> 3;

	// calculate the number of bits to set in the first byte
  // this calculates the position of the first but to set
	uint16_t num_bits = 8 - (first_block & 7);

	if(num_bits > num_blocks)
		num_bits = num_blocks;

	if(num_bits < 8)
		{
		num_blocks -= num_bits;

		// create the initial mask
		unsigned char bits = (unsigned char)((1 << num_bits) -1);
		bits <<= first_block & 7;
		*base |= bits;
		base++;
		}

	// fill an even number of bytes next
	uint16_t num_bytes = num_blocks >> 3;
	if(num_bytes > 0)
		{
		memset(base, 0xff, num_bytes);
		base += num_bytes;
		num_blocks -= num_bytes << 3;
		}

	// now the last block
	if(num_blocks > 0)
		{
		unsigned char bits = (unsigned char)((1 << num_blocks) -1);
		*base |= bits;
		}
	}

static void release_block(uint16_t first_block,
													uint16_t num_blocks)
	{
	// point to the first byte to allocate
  uint8_t *base = root->bitmap;
	base += first_block >> 3;

  uint16_t num_bits = 8 - (first_block & 7);
	if(num_bits > num_blocks)
		num_bits = num_blocks;

	if(num_bits < 8)
		{
		num_blocks -= num_bits;

		// create the initial mask
		unsigned long bits = (1 << num_bits)-1;
		bits <<= first_block & 7;

		*base &= ~(unsigned char)(bits);
		base++;
		}

	// fill an even number of bytes next
  uint16_t num_bytes = num_blocks >> 3;
	if(num_bytes > 0)
		{
		memset(base, 0x00, num_bytes);
		base += num_bytes;
		num_blocks -= num_bytes << 3;
		}

	// now the last block
	if(num_blocks > 0)
		{
		unsigned char bits = ~(unsigned char)((1 << num_blocks) -1);
		*base &= bits;
		}
	}

static unsigned char block_status(uint16_t block,
																	unsigned char mask)
	{
	return (unsigned char)(root->bitmap[block >> 3] & (mask << (block & 7)));
	}

// this is the allocator for the system.
// it is a best-fit allocator to reduce the chance of
// memory fragmentation.
static memid_t allocate_block(uint16_t number_of_blocks)
	{
  // how many blocks available
  uint32_t num_blocks = num_bitmap_bytes << 3;

	// point to the first available block
  uint16_t block = (num_bitmap_bytes >> BLOCK_SHIFT) +1;

  uint16_t best_fit = 0;

	while(block < num_blocks && (block + number_of_blocks) < num_blocks)
		{
		// skip allocated blocks
		if((block & 7) == 0 && root->bitmap[block >> 3] == 0xff)
			{
			block += 8;
			continue;
			}

		// count how many free bits in this block.
		// if the block is exact then we exit
		uint16_t block_offset = 0;
		uint16_t num_blocks_available = 0;
		for(;(block + block_offset) < num_blocks &&
				 block_status(block + block_offset, 1) == 0;
				block_offset++)
			{
			num_blocks_available++;
      if(num_blocks_available >= number_of_blocks)
        {
        best_fit = block;
        break;
        }
			}

		if(best_fit != 0)
      break;

		block += block_offset == 0 ? 1 : block_offset;
		}

	// if we have found a block that is a good fit then
	// malloc ot
	if(best_fit != 0)
		allocate_blocks(best_fit, number_of_blocks);

	return best_fit;
	}

/**
 * Save a block that has been allocated
 * @param allocated_block   memid of the block
 * @param alloc_size        length of the block in bytes
 * @return s_ok if saved ok.
 */
static result_t flush_memid_bitblock(memid_t allocated_block, uint16_t alloc_size)
  {
  uint16_t start_block;        // first block to save
  uint16_t end_block;         // last block to save
  uint16_t save_size;         // number of bytes to save

  // we need to work out the byte position of the allocated block in the
  // bitmap.  A block is 32 bytes
  start_block = allocated_block;
  // add in extra blocks we are allocating
  // alloc_size is always from the start of a block so make it the number of
  // blocks
  save_size = ((alloc_size -1)| BLOCK_MASK)+1 ;
  save_size >>= BLOCK_SHIFT;

  end_block = start_block + save_size;

  // we now have 2 start/end points make them bytes rather than bits
  start_block >>= 3;
  end_block >>= 3;

  end_block++;
  save_size = end_block - start_block;          // bytes to save

  // add reserved space to the block as block 0 is the root block.
  return reg_write_bytes(start_block + sizeof(eeprom_root_t), save_size, root->bitmap + start_block);
  }

result_t allocate_memid(uint16_t length, memid_t *memid)
  {
  // round up the size
  uint16_t alloc_size = ((length -1)|BLOCK_MASK)+1;
  memid_t allocated_block = allocate_block(alloc_size >> BLOCK_SHIFT);

  if(allocated_block == 0)
    return e_no_space;

  // a memid is the block allocated offset / 2
  *memid = allocated_block;

  // save the allocated pointers
  return flush_memid_bitblock(allocated_block, alloc_size);
  }

result_t release_memid(memid_t memid, uint16_t memid_size)
  {
  release_block(memid, (((memid_size-1)|BLOCK_MASK)+1) >> BLOCK_SHIFT);

  return flush_memid_bitblock(memid, memid_size);
  }

static result_t reg_open_first_child(memid_t parent,
                                     memid_t *memid)
  {
  result_t result;
  // load the parent memid
  field_key_t parent_key;
  if(failed(result = reg_read_bytes(get_block_offset(parent), 
                                    sizeof(field_key_t), &parent_key)))
    return result;

  if(parent_key.hdr.data_type != field_key)
    return e_invalid_operation;

  // search through the child records
  *memid = parent_key.first_child;            // get the first child
  return s_ok;
  }

static result_t is_valid_name(const char *name)
  {
  if(name == 0)
    return e_bad_parameter;

  uint16_t len = 0;
  while(*name != 0)
    {
    char ch = *name++;

    len++;
    if(len >= REG_NAME_MAX)
      return e_bad_parameter;

    if((ch >= 'a' && ch <= 'z') ||
       (ch >= 'A' && ch <= 'Z') ||
       (ch >= '0' && ch <= '9'))
      continue;

    if(len > 1)
      {
      if(ch == '.' ||
         ch == '_' ||
         ch == '-')
        continue;
      }

    return e_bad_parameter;
    }

  return s_ok;
  }

result_t reg_query_child_impl(memid_t parent, const char *name, memid_t *key, field_datatype *type, uint16_t *len)
  {
  memid_t memid;
  result_t result;
  // this is so we can read a string header.
  field_definition_t defn;

  if(name == 0 ||
     key == 0 ||
     strlen(name) > REG_NAME_MAX)
    return e_bad_parameter;

  if(failed(result = reg_open_first_child(parent, &memid)))
    return result;

  while(memid != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(memid), sizeof(field_definition_t), &defn)))
      return result;

    if(strcmp(name, defn.name)== 0)
      break;

    // skip to next child
    memid = defn.next;
    }
  

  if(memid == 0)
    return e_path_not_found;

  if(type != 0)
    *type = defn.data_type;

  if(len != 0)
    *len = defn.length;

  *key = memid;
  return s_ok;
  }

result_t reg_query_memid_impl(memid_t memid, field_datatype *type, char *name, uint16_t *length, memid_t *parent)
  {
  result_t result;

  field_definition_t defn;
  if(failed(result = reg_read_bytes(get_block_offset(memid), sizeof(field_definition_t), &defn)))
    return result;

  if(type != 0)
    *type = defn.data_type;

  if(name != 0)
    strncpy(name, defn.name, REG_NAME_MAX);

  if(length != 0)
    *length = defn.length;

  if(parent != 0)
    *parent = defn.parent;

  return s_ok;
  }

result_t reg_create_key_impl(memid_t parent, const char *name, memid_t *key)
  {
  memid_t memid;
  result_t result;
  // this is so we can read a string header.
  field_key_t defn;
  
  if(key == 0 ||
     strlen(name) > REG_NAME_MAX ||
     failed(is_valid_name(name)))
    return e_bad_parameter;

  if(failed(result = reg_open_first_child(parent, &memid)))
    return result;

  while(memid != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(memid), sizeof(field_key_t), &defn)))
      return result;

    if(strcmp(name, defn.hdr.name)== 0)
      break;

    // skip to next child
    memid = defn.hdr.next;
    }

  if(memid == 0)
    {
    // not found so create it.
    memset(&defn, 0, sizeof(field_key_t));
    defn.hdr.data_type = field_key;
    defn.hdr.length = sizeof(field_key_t);
    defn.hdr.parent = parent;
    if(failed(result = allocate_memid(sizeof(field_key_t), &defn.hdr.memid)))
      return result;
    
    strncpy(defn.hdr.name, name, sizeof(defn.hdr.name));

    // write the key
    *key = defn.hdr.memid;

    field_key_t parent_key;
    // append to the end of the parent.
    reg_read_bytes(get_block_offset(parent), sizeof(field_key_t), &parent_key);

    if(parent_key.first_child == 0)
      {
      parent_key.first_child = defn.hdr.memid;
      parent_key.last_child = defn.hdr.memid;
      }
    else
      {
      // read last block
      field_definition_t last_child;
      if(failed(result =reg_read_bytes(get_block_offset(parent_key.last_child), sizeof(field_definition_t), &last_child)))
        return exit_registry(result);

      parent_key.last_child = defn.hdr.memid;
      last_child.next = defn.hdr.memid;
      defn.hdr.previous = last_child.memid;

      if(failed(result = reg_write_bytes(get_block_offset(last_child.memid), sizeof(field_definition_t), &last_child)))
        return result;
      }

    if(failed(result = reg_write_bytes(get_block_offset(parent_key.hdr.memid), sizeof(field_key_t), &parent_key)))
      return result;

    if(failed(result = reg_write_bytes(get_block_offset(defn.hdr.memid), sizeof(field_key_t), &defn)))
      return result;

    return s_ok;
    }

  if(defn.hdr.data_type != field_key)
    return e_invalid_operation;

  *key = memid;

  return s_ok;
  }


result_t reg_create_key(memid_t parent, const char *name, memid_t *key)
  {
  enter_registry();

  return exit_registry(reg_create_key_impl(parent, name, key));
  }

result_t reg_query_child(memid_t parent, const char *name, memid_t *key, field_datatype *type, uint16_t *len)
  {
  enter_registry();
  return exit_registry(reg_query_child_impl(parent, name, key, type, len));
  }

result_t reg_query_memid(memid_t memid, field_datatype *type, char *name, uint16_t *length, memid_t *parent)
  {
  enter_registry();
  return exit_registry(reg_query_memid_impl(memid, type, name, length, parent));
  }


result_t reg_open_key(memid_t parent,
                        const char *name,
                        memid_t *key)
  {
  result_t result;
  field_datatype type;
  uint16_t len;

  if(name == 0 ||
      strlen(name) > REG_NAME_MAX)
    return e_bad_parameter;

  enter_registry();
  if(failed(result = reg_query_child_impl(parent, name, key, &type, &len)))
    return exit_registry(result);

  exit_registry(result);
  
  if(type != field_key)
    return e_invalid_operation;

  return s_ok;
  }

result_t reg_enum_key(memid_t parent,
                      field_datatype *type,
                      uint16_t *length,
                      void *data,
                      uint16_t len,
                      char *name,
                      memid_t *child)
  {
  result_t result;
  if(type == 0 ||
     child == 0 ||
     (name != 0 && len < REG_NAME_MAX))
    return e_bad_parameter;

  enter_registry();
  // if non 0 then a filter is applied
  field_datatype filter = *type;
  bool read_next = true;
  if(*child == 0)         // case for first child
    {
    if(failed(result = reg_open_first_child(parent, child)))
      return exit_registry(result);

    read_next = false;
    }

  if(*child == 0)
    return exit_registry(e_not_found);       // no child is found

  can_field_t defn;
  if(failed(result = reg_read_bytes(get_block_offset(*child), sizeof(field_definition_t), &defn)))
    return exit_registry(result);

  if(!read_next && filter != field_none && defn.key_f.hdr.data_type != filter)
    read_next = true;       // not the type we want.

  while(read_next)
    {
    if(defn.key_f.hdr.next == 0)
      return exit_registry(e_no_more_information);

    if(failed(result = reg_read_bytes(get_block_offset(defn.key_f.hdr.next), sizeof(field_definition_t), &defn)))
      return exit_registry(result);

    if(filter == field_none || defn.key_f.hdr.data_type == filter)
      break;        // found a matching key
    }

  *child = defn.key_f.hdr.memid;
  *type = defn.key_f.hdr.data_type;

  if(name != 0)
    {
    // copy the name string into the buffer.
    memcpy(name, defn.key_f.hdr.name, REG_NAME_MAX);

    if(len >= (REG_NAME_MAX+1))
      name[REG_NAME_MAX] = 0;
    }

  if(length != 0)
    {
    if(data == 0)
      *length = defn.key_f.hdr.length;
    else
      {
      uint16_t data_length = defn.key_f.hdr.length - sizeof(field_definition_t);
      if(*length < data_length)
        return exit_registry(e_buffer_too_small);

      // read the extra bytes
      if(failed(result = reg_read_bytes(get_block_offset(*child), defn.key_f.hdr.length, &defn)))
        return exit_registry(result);

      memcpy(data, ((uint8_t *)&defn)+sizeof(field_definition_t), data_length);
      *length = data_length;
      }
    }

  return exit_registry(s_ok);
  }

result_t reg_set_value(memid_t parent,
                       const char *name,
                       field_datatype datatype,
                       uint16_t length,
                       const void *data,
                       memid_t *child)
  {
  result_t result;
  field_datatype type;
  memid_t memid;
  uint16_t len;
  uint16_t defn_len;
  can_field_t defn;

  if(data == 0 ||
     strlen(name) > REG_NAME_MAX ||
     failed(is_valid_name(name)))
    return e_bad_parameter;
  
  switch(datatype)
    {
    default :
      return e_bad_parameter;
    case field_uint8 :
      len = sizeof(field_uint8_t);
      break;
    case field_int8 :
      len = sizeof(field_int8_t);
      break;
    case field_bool :
      len = sizeof(field_bool_t);
      break;
    case field_int16 :
      len = sizeof(field_int16_t);
      break;
    case field_uint16 :
      len = sizeof(field_uint16_t);
      break;
    case field_int32 :
      len = sizeof(field_int32_t);
      break;
    case field_uint32 :
      len = sizeof(field_uint32_t);
      break;
    case field_float :
      len = sizeof(field_float_t);
      break;
    case field_xyz :
      len = sizeof(field_xyz_t);
      break;
    case field_matrix :
      len = sizeof(field_matrix_t);
      break;
    case field_string :
      len = sizeof(field_string_t);
      break;
    case field_qtn :
      len = sizeof(field_qtn_t);
      break;
    case field_lla :
      len = sizeof(field_lla_t);
      break;
    case field_stream:
      len = sizeof(field_stream_t);
      break;
    }

  if(failed(result = reg_query_child_impl(parent, name, &memid, &type, &defn_len)) &&
     result != e_path_not_found)
    return result;

  if(result == e_path_not_found)
    {
    memset(&defn, 0, len);
    defn.key_f.hdr.data_type = datatype;
    defn.key_f.hdr.length = len;
    defn.key_f.hdr.parent = parent;
    strncpy(defn.key_f.hdr.name, name, sizeof(defn.key_f.hdr.name));

    // figure out the links now.
    if(failed(result = allocate_memid(len, &defn.key_f.hdr.memid)))
      return result;

    field_key_t key;
    if(failed(result = reg_read_bytes(get_block_offset(parent), sizeof(field_key_t), &key)))
      return result;

    memid_t previous = key.last_child;
    if(key.first_child == 0)
      // this is the first value assigned
      key.first_child = defn.key_f.hdr.memid;

    key.last_child = defn.key_f.hdr.memid;

    // write the parent key field with our new last child
    if(failed(result = reg_write_bytes(get_block_offset(parent), sizeof(field_key_t), &key)))
      return result;

    field_definition_t child;

    if(previous != 0)
      {
      // link this into the linked-list of the last child
      if(failed(result == reg_read_bytes(get_block_offset(previous), sizeof(field_definition_t), &child)))
        return result;

      child.next = defn.key_f.hdr.memid;
      defn.key_f.hdr.previous = child.memid;

      if(failed(result = reg_write_bytes(get_block_offset(previous), sizeof(field_definition_t), &child)))
        return result;
      }
    }
  else
    {
    if(datatype != type)
      return e_invalid_operation;             // can't change a type

    // read the value
    if(failed(result = reg_read_bytes(get_block_offset(memid), len, &defn)))
      return result;
    }

  // now copy the value over
  switch(datatype)
    {
    default :
      return e_bad_parameter;
    case field_uint8 :
      defn.uint8_f.value = *((const uint8_t *)data);
      break;
    case field_int8 :
      defn.int8_f.value = *((const int8_t *)data);
      break;
    case field_bool :
      defn.bool_f.value = *((const bool *)data);
      break;
    case field_int16 :
      defn.int16_f.value = *((const int16_t *)data);
      break;
    case field_uint16 :
      defn.uint16_f.value = *((const uint16_t *)data);
      break;
    case field_int32 :
      defn.int32_f.value = *((const int32_t *)data);
      break;
    case field_uint32 :
      defn.uint32_f.value = *((const uint32_t *)data);
      break;
    case field_float :
      defn.float_f.value = *((const float*)data);
      break;
    case field_xyz :
      memcpy(&defn.xyz_f.value, data, sizeof(xyz_t));
      break;
    case field_matrix :
      memcpy(&defn.matrix_f.value, data, sizeof(matrix_t));
      break;
    case field_qtn :
      memcpy(&defn.qtn_f.value, data, sizeof(qtn_t));
      break;
    case field_lla :
      memcpy(&defn.lla_f.value, data, sizeof(lla_t));
      break;
    case field_string :
      memset(defn.string_f.value, 0, sizeof(defn.string_f.value));
      strncpy(defn.string_f.value, (const char *) data, sizeof(defn.string_f.value));
      break;
    case field_stream :
      memcpy(&defn.stream_f.length, data, sizeof(field_stream_t) - sizeof(field_definition_t));
      break;
    }

  if(child != 0)
    *child = defn.key_f.hdr.memid;

  return reg_write_bytes(get_block_offset(defn.key_f.hdr.memid), len, &defn);
  }

result_t reg_get_value(memid_t parent,
                        const char *name,
                        uint8_t *datatype,
                        uint16_t *field_length,
                        memid_t *field_memid,
                        uint16_t *data_length,    // in/out
                        void *data)
  {
  memid_t memid;
  uint16_t length;
  result_t result;
  // this is so we can read a string header.
  field_definition_t defn;

  if(name == 0)
    return e_bad_parameter;
  
  if(failed(result = reg_open_first_child(parent, &memid)))
    return result;

  while(memid != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(memid), sizeof(field_definition_t), &defn)))
      return result;

    if(strcmp(name, defn.name)== 0)
      break;

    // skip to next child
    memid = defn.next;
    }

  if(memid == 0)
    return e_path_not_found;

  if(field_memid != 0)
    *field_memid = memid;

  // if found copy values if ok
  if(datatype != 0)
    {
    if(*datatype != 0 &&
       *datatype != (defn.data_type & 0x7f))
      return exit_registry(e_bad_parameter);

    if(*datatype == 0)
      *datatype = (defn.data_type & 0x7f);
    }

  length = defn.length;
  if(field_length != 0)
    *field_length = length;

  // see if we copy the data over
  if(data_length != 0)
    {
    if(data == 0)
      *data_length = defn.length;
    else
      {
      // just read the data.
      if(*data_length < defn.length)
        return e_buffer_too_small;

      if(failed(result = reg_read_bytes(get_block_offset(memid), defn.length, data)))
        return result;
      }
    }
  return s_ok;
  }

result_t reg_rename_value(memid_t parent, const char *name, const char *new_name)
  {
  return e_not_implemented;
  }

result_t reg_rename_key(memid_t key, const char *new_name)
  {
  return e_not_implemented;
  }

result_t reg_delete_key_impl(memid_t memid)
  {
  result_t result;
  field_key_t key;
  memid_t next_child;
  field_definition_t child;

  if(failed(result = reg_read_bytes(get_block_offset(memid), sizeof(field_key_t), &key)))
    return result;

  if(key.hdr.data_type != field_key)
    return e_bad_parameter;

  child.memid = 0;

  if(failed(result == reg_open_first_child(memid, &next_child)))
    return result;

  // release the children
  while(next_child != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(next_child), sizeof(field_definition_t), &child)))
      return result;

    next_child = child.next;

    if(child.data_type == field_key)
      {
      // recurse over keys....
      if(failed(result = reg_delete_key_impl(child.memid)))
        return result;
      }
    else if(failed(result = release_memid(child.memid, child.length)))
      return result;
    }

  // release the next/previous
  if(key.hdr.previous != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(key.hdr.previous), sizeof(field_definition_t), &child)))
      return result;

    child.next = key.hdr.next;
    if(failed(result = reg_write_bytes(get_block_offset(child.memid), sizeof(field_definition_t), &child)))
      return result;

    // unlink the parent from the child (only if first/last child)
    field_key_t parent;
    if(failed(result = reg_read_bytes(get_block_offset(key.hdr.parent), sizeof(field_key_t), &parent)))
      return result;

    if(parent.first_child == key.hdr.memid)
      parent.first_child = key.hdr.next;

    if(parent.last_child == key.hdr.memid)
      parent.last_child = key.hdr.previous;

    if(failed(result = reg_write_bytes(get_block_offset(key.hdr.parent), sizeof(field_key_t), &parent)))
      return result;
    }

  if(key.hdr.next != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(key.hdr.next), sizeof(field_definition_t), &child)))
      return result;

    child.previous = key.hdr.previous;
    if(failed(result = reg_write_bytes(get_block_offset(child.memid), sizeof(field_definition_t), &child)))
      return result;
    }

  memid = key.hdr.memid;

  uint16_t length = key.hdr.length;
  memset(&key, 0, sizeof(field_key_t));

  if(failed(result = reg_write_bytes(get_block_offset(memid), sizeof(field_key_t), &key)))
    return result;

  // now release the key
  return release_memid(memid, length);
  }

result_t reg_delete_key(memid_t memid)
  {
  enter_registry();
  return exit_registry(reg_delete_key_impl(memid));
  }

result_t reg_delete_value_impl(memid_t memid, const char *name)
  {
  result_t result;
  field_key_t key;
  memid_t next_child;
  field_definition_t child;

  if(failed(result = reg_read_bytes(get_block_offset(memid), sizeof(field_key_t), &key)))
    return result;

  if(key.hdr.data_type != field_key)
    return e_bad_parameter;

  child.memid = 0;

  if(failed(result == reg_open_first_child(memid, &next_child)))
    return result;

  bool found = false;
  while(next_child != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(next_child), sizeof(field_definition_t), &child)))
      return result;

    next_child = child.next;

    if(child.data_type != field_key &&
      child.data_type != field_none &&
       strncmp(child.name, name, sizeof(child.name))== 0)
      {
      found = true;
      break;
      }
    }

  if(!found || child.memid == 0)
    return e_path_not_found;

  field_definition_t link_child;
  // unlink the key
  if(child.previous != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(child.previous), sizeof(field_definition_t), &link_child)))
      return result;

    link_child.next = child.next;
    if(failed(result = reg_write_bytes(get_block_offset(child.previous), sizeof(field_definition_t), &link_child)))
      return result;
    }

  if(child.next != 0)
    {
    if(failed(result = reg_read_bytes(get_block_offset(child.next), sizeof(field_definition_t), &link_child)))
      return result;

    link_child.previous = child.previous;
    if(failed(result = reg_write_bytes(get_block_offset(child.memid), sizeof(field_definition_t), &link_child)))
      return result;
    }

  // update our parent and remove our references
  if (failed(result = reg_read_bytes(get_block_offset(child.parent), sizeof(field_key_t), &key)))
    return result;
  
  bool update = false;
  if (key.first_child == child.memid)
    {
    key.first_child = child.next;
    update = true;
    }

  if (key.last_child == child.memid)
    {
    key.last_child = child.previous;
    update = true;
    }

  if (update && failed(result = reg_write_bytes(get_block_offset(child.parent), sizeof(field_key_t), &key)))
    return result;  

  memid = child.memid;
  uint16_t length = child.length;
  memset(&child, 0, sizeof(field_definition_t));

  // write nulled out data
  if (failed(result = reg_write_bytes(get_block_offset(memid), sizeof(field_definition_t), &child)))
    return result;


  // now release the key
  return release_memid(memid, length);
  }

result_t reg_delete_value(memid_t memid, const char *name)
  {
  enter_registry();
  return exit_registry(reg_delete_value_impl(memid, name));
  }

result_t reg_get_int8(memid_t parent, const char *name, int8_t *value)
  {
  result_t result;
  if (value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_int8;
  uint16_t buffer_len = sizeof(field_int8_t);

  field_int8_t field;

  enter_registry();
  if (failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);

  *value = field.value;
  return s_ok;
  }

result_t reg_set_int8(memid_t parent, const char *name, int8_t value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_int8, sizeof(int8_t), &value, 0));
  }

result_t reg_get_uint8(memid_t parent, const char *name, uint8_t *value)
  {
  result_t result;
  if (value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_uint8;
  uint16_t buffer_len = sizeof(field_uint8_t);

  field_uint8_t field;

  enter_registry();
  if (failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);

  *value = field.value;
  return s_ok;
  }

result_t reg_set_uint8(memid_t parent, const char *name, uint8_t value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_uint8, sizeof(uint8_t), &value, 0));
  }

result_t reg_get_int16(memid_t parent, const char *name, int16_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_int16;
  uint16_t buffer_len = sizeof(field_int16_t);

  field_int16_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  *value = field.value;
  return s_ok;
  }

result_t reg_set_int16(memid_t parent, const char *name, int16_t value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_int16, sizeof(int16_t), &value, 0));
  }

result_t reg_get_uint16(memid_t parent, const char *name, uint16_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_uint16;
  uint16_t buffer_len = sizeof(field_uint16_t);

  field_uint16_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  *value = field.value;
  return s_ok;
  }

result_t reg_set_uint16(memid_t parent, const char *name, uint16_t value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_uint16, sizeof(uint16_t), &value, 0));
  }

result_t reg_get_int32(memid_t parent, const char *name, int32_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_int32;
  uint16_t buffer_len = sizeof(field_int32_t);

  field_int32_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  *value = field.value;
  return s_ok;
  }

result_t reg_set_int32(memid_t parent, const char *name, int32_t value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_int32, sizeof(int32_t), &value, 0));
  }

result_t reg_get_uint32(memid_t parent, const char *name, uint32_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_uint32;
  uint16_t buffer_len = sizeof(field_uint32_t);

  field_uint32_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  *value = field.value;
  return s_ok;
  }

result_t reg_set_uint32(memid_t parent, const char *name, uint32_t value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_uint32, sizeof(uint32_t), &value, 0));
  }

result_t reg_get_xyz(memid_t parent, const char *name, xyz_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_xyz;
  uint16_t buffer_len = sizeof(field_xyz_t);

  field_xyz_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);
  
  exit_registry(result);

  memcpy(value, &field.value, sizeof(xyz_t));
  return s_ok;
  }

result_t reg_set_xyz(memid_t parent, const char *name, const xyz_t *value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_xyz, sizeof(xyz_t), value, 0));
  }

result_t reg_get_qtn(memid_t parent, const char *name, qtn_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_qtn;
  uint16_t buffer_len = sizeof(field_qtn_t);

  field_qtn_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  memcpy(value, &field.value, sizeof(qtn_t));
  return s_ok;
  }

result_t reg_set_qtn(memid_t parent, const char *name, const qtn_t *value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_qtn, sizeof(qtn_t), value, 0));
  }

result_t reg_get_lla(memid_t parent, const char *name, lla_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_lla;
  uint16_t buffer_len = sizeof(field_lla_t);

  field_lla_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  memcpy(value, &field.value, sizeof(lla_t));
  return s_ok;
  }

result_t reg_set_lla(memid_t parent, const char *name, const lla_t *value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_lla, sizeof(lla_t), value, 0));
  }

result_t reg_get_matrix(memid_t parent, const char *name, matrix_t *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_matrix;
  uint16_t buffer_len = sizeof(field_matrix_t);

  field_matrix_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  memcpy(value, &field.value, sizeof(matrix_t));
  return s_ok;
  }

result_t reg_set_matrix(memid_t parent, const char *name, matrix_t *value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_matrix, sizeof(matrix_t), value, 0));
  }

result_t reg_get_string(memid_t parent, const char *name, char *value, uint16_t *length)
  {
  result_t result;

  uint8_t data_type = field_string;
  uint16_t buffer_len = sizeof(field_string_t);

  field_string_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);
  
  exit_registry(result);
  
  uint16_t max_len = sizeof(field.value);
  
  if(length != 0)
    max_len = min(max_len, *length);

  // calculate actual length
  uint16_t i = 0;
  while(i < max_len-1)
    {
    if(value != 0)
      {
      // put string term
      value[i+1] = 0;
      value[i] = field.value[i];
      }
   
    if(field.value[i]== 0)
      break;
    
    i++;
    }

  if(length != 0)
    *length = i;

  return s_ok;
  }

result_t reg_set_string(memid_t parent, const char *name, const char *value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_string, 0, value, 0));
  }

result_t reg_get_float(memid_t parent, const char *name, float *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_float;
  uint16_t buffer_len = sizeof(field_float_t);

  field_float_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  *value = field.value;
  return s_ok;
  }

result_t reg_set_float(memid_t parent, const char *name, float value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_float, sizeof(float), &value, 0));
  }

result_t reg_get_bool(memid_t parent, const char *name, bool *value)
  {
  result_t result;
  if(value == 0)
    return e_bad_parameter;

  uint8_t data_type = field_bool;
  uint16_t buffer_len = sizeof(field_bool_t);

  field_bool_t field;

  enter_registry();
  if(failed(result = reg_get_value(parent, name, &data_type, 0, 0, &buffer_len, &field)))
    return exit_registry(result);

  exit_registry(result);
  
  *value = field.value;
  return s_ok;
  }

result_t reg_set_bool(memid_t parent, const char *name, bool value)
  {
  enter_registry();
  return exit_registry(reg_set_value(parent, name, field_bool, sizeof(bool), &value, 0));
  }

