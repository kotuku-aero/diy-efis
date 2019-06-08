#include "memory.h"

#include <stdlib.h>
#include <string.h>

extern void memory_init(uint8_t **heap, uint32_t *length);

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */

/*-----------------------------------------------------------*/


block_link_t start_of_heap;
block_link_t *end_of_heap = 0;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
size_t free_bytes_remaining = 0U;
size_t minimum_free_bytes_remaining = 0U;

size_t block_allocated_bit = 0;

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void insert_block_into_free_list(block_link_t *pxBlockToInsert);


/*-----------------------------------------------------------*/

void *neutron_malloc(size_t wanted_size)
  {
  block_link_t *the_block;
  block_link_t *previous_block;
  block_link_t *new_block_link;
  void *allocated_block = 0;

  enter_critical();
  {
     /* Check the requested block size is not so large that the top bit is
    set.  The top bit of the block size member of the block_link_t structure
    is used to determine who owns the block - the application or the
    kernel, so it must be free. */
    if ((wanted_size & block_allocated_bit) == 0)
      {
      /* The wanted size is increased so it can contain a block_link_t
      structure in addition to the requested amount of bytes. */
      if (wanted_size > 0)
        {
        wanted_size += sizeof (block_link_t);

        /* Ensure that blocks are always aligned to the required number
        of bytes. */
          /* Byte alignment required. */
        wanted_size = ((wanted_size -1) | 0x07)+1;
        }

      if ((wanted_size > 0) && (wanted_size <= free_bytes_remaining))
        {
        /* Traverse the list from the start	(lowest address) block until
        one	of adequate size is found. */
        previous_block = &start_of_heap;
        the_block = start_of_heap.next_free_block;
        while ((the_block->block_size < wanted_size) && (the_block->next_free_block != 0))
          {
          previous_block = the_block;
          the_block = the_block->next_free_block;
          }

        /* If the end marker was reached then a block of adequate size
        was	not found. */
        if (the_block != end_of_heap)
          {
          /* Return the memory space pointed to - jumping over the
          block_link_t structure at its start. */
          allocated_block = (void *) (((uint8_t *) previous_block->next_free_block) + sizeof (block_link_t));

          /* This block is being returned for use so must be taken out
          of the list of free blocks. */
          previous_block->next_free_block = the_block->next_free_block;

          /* If the block is larger than required it can be split into
          two. */
          if ((the_block->block_size - wanted_size) > (sizeof (block_link_t) << 1))
            {
            /* This block is to be split into two.  Create a new
            block following the number of bytes requested. The void
            cast is used to prevent byte alignment warnings from the
            compiler. */
            new_block_link = (void *) (((uint8_t *) the_block) + wanted_size);
            //configASSERT( ( ( ( size_t ) pxNewBlockLink ) & 1 ) == 0 );

            /* Calculate the sizes of two blocks split from the
            single block. */
            new_block_link->block_size = the_block->block_size - wanted_size;
            the_block->block_size = wanted_size;

            /* Insert the new block into the list of free blocks. */
            insert_block_into_free_list(new_block_link);
            }

          free_bytes_remaining -= the_block->block_size;

          if (free_bytes_remaining < minimum_free_bytes_remaining)
            {
            minimum_free_bytes_remaining = free_bytes_remaining;
            }

          /* The block is being returned - it is allocated and owned
          by the application and has no "next" block. */
          the_block->block_size |= block_allocated_bit;
          the_block->next_free_block = 0;
          }
        }
      }
  }
  (void) exit_critical();

  if (allocated_block == 0)
    {
    malloc_failed_hook(wanted_size);
    }

  //configASSERT( ( ( ( size_t ) pvReturn ) & ( size_t ) 1 ) == 0 );
  return allocated_block;
  }

/*-----------------------------------------------------------*/

void neutron_free(void *pv)
  {
  uint8_t *p_bytes = (uint8_t *) pv;
  block_link_t *block_link;

  if (pv != 0)
    {
    /* The memory being freed will have an block_link_t structure immediately
    before it. */
    p_bytes -= sizeof (block_link_t);

    /* This casting is to keep the compiler from issuing warnings. */
    block_link = (void *) p_bytes;

    /* Check the block is actually allocated. */
    //configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
    //configASSERT( pxLink->pxNextFreeBlock == 0 );

    if ((block_link->block_size & block_allocated_bit) != 0)
      {
      if (block_link->next_free_block == 0)
        {
        /* The block is being returned to the heap - it is no longer
        allocated. */
        block_link->block_size &= ~block_allocated_bit;

        enter_critical();
        {
          /* Add this block to the list of free blocks. */
          free_bytes_remaining += block_link->block_size;
          // traceFREE( pv, pxLink->xBlockSize );
          insert_block_into_free_list(((block_link_t *) block_link));
        }
        (void) exit_critical();
        }
      }
    }
  }

static void insert_block_into_free_list(block_link_t *block_to_insert)
  {
  block_link_t *iterator;
  uint8_t *p_bytes;

  /* Iterate through the list until a block is found that has a higher address
  than the block being inserted. */
  for (iterator = &start_of_heap; iterator->next_free_block < block_to_insert; iterator = iterator->next_free_block)
    {
    /* Nothing to do here, just iterate to the right position. */
    }

  /* Do the block being inserted, and the block it is being inserted after
  make a contiguous block of memory? */
  p_bytes = (uint8_t *) iterator;
  if ((p_bytes + iterator->block_size) == (uint8_t *) block_to_insert)
    {
    iterator->block_size += block_to_insert->block_size;
    block_to_insert = iterator;
    }

  /* Do the block being inserted, and the block it is being inserted before
  make a contiguous block of memory? */
  p_bytes = (uint8_t *) block_to_insert;
  if ((p_bytes + block_to_insert->block_size) == (uint8_t *) iterator->next_free_block)
    {
    if (iterator->next_free_block != end_of_heap)
      {
      /* Form one big block from the two blocks. */
      block_to_insert->block_size += iterator->next_free_block->block_size;
      block_to_insert->next_free_block = iterator->next_free_block->next_free_block;
      }
    else
      {
      block_to_insert->next_free_block = end_of_heap;
      }
    }
  else
    {
    block_to_insert->next_free_block = iterator->next_free_block;
    }

  /* If the block being inserted plugged a gab, so was merged with the block
  before and the block after, then it's pxNextFreeBlock pointer will have
  already been set, and should not be set here as that would make it point
  to itself. */
  if (iterator != block_to_insert)
    iterator->next_free_block = block_to_insert;
  }

void *neutron_realloc(void *pv, size_t new_size)
  {
  void *result;
  uint8_t *p_bytes = (uint8_t *) pv;

  /* The memory being freed will have an block_link_t structure immediately
  before it. */
  p_bytes -= sizeof (block_link_t);

  /* This casting is to keep the compiler from issuing warnings. */
  block_link_t *block_link = (void *) p_bytes;
  
  size_t old_size = block_link->block_size;
  
  if(old_size > new_size)
    return pv;            // nothing to be done.
  
  result = neutron_malloc(new_size);
  
  memcpy(result, pv, old_size);
  
  neutron_free(pv);

  return result;
  }

char *neutron_strdup(const char *str)
  {
  if (str == 0)
    return 0;

  size_t len = strlen(str);

  char *cpy = (char *) neutron_malloc(len + 1);
  if (cpy == 0)
    return 0;

  strcpy(cpy, str);
  return cpy;
  }

