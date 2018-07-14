#ifndef __memory_h__
#define	__memory_h__

#ifdef	__cplusplus
extern "C"
  {
#endif
#include "../../libs/neutron/neutron.h"
/**
 * Called when memory is exhausted
 * @param size
 */
extern void malloc_failed_hook(uint16_t size);

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct _block_link_t
  {
  struct _block_link_t *next_free_block; /*<< The next free block in the list. */
  size_t block_size; /*<< The size of the free block. */
  } block_link_t;

/* Create a couple of list links to mark the start and end of the list. */
extern block_link_t start_of_heap;
extern block_link_t *end_of_heap;
/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
extern size_t free_bytes_remaining;
extern size_t minimum_free_bytes_remaining;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an block_link_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
extern size_t block_allocated_bit;

#ifdef	__cplusplus
  }
#endif

#endif	/* MEMORY_H */

