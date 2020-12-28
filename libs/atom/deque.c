#include "microkernel.h"
#include <stdlib.h>
#include <string.h>
 
typedef struct _deque_t
  {
  semaphore_t readers;
  semaphore_t writers;
  uint8_t *back;
  uint8_t *front;
  uint8_t *end;
  uint16_t length;      // number of elements that can be queued
  uint16_t size;        // size of elements in the queue
  uint16_t count;
  uint8_t buffer[];     // varlength
  } deque_t;
  
result_t push_back_from_isr(deque_p deque, const void *item)
  {
  if(deque == 0 || item == 0)
    return e_bad_parameter;
  
  if(deque->count >= deque->length)
    return e_no_space;             // failed
  
  // copy the item into the queue
  memcpy(deque->back, item, deque->size);

  deque->count++;
  deque->back += deque->size;

  // if this is the last element then start at the front
  if(deque->back >= deque->end)
    deque->back = deque->buffer;
  
  // if there is a task blocking on the queue
  // tne signal it
  if(succeeded(has_wait_tasks(&deque->readers)))
    signal_from_isr(&deque->readers);

  return s_ok;
  }

result_t pop_front_from_isr(deque_p deque, void *item)
  {
  if(deque == 0 || item == 0)
    return e_bad_parameter;
  
  if(deque->count == 0)
    return s_false;

  if(item != 0)
    memcpy(item, deque->front, deque->size);
  
  
  deque->front += deque->size;
  deque->count--;

  // if this is the last element then start at the front
  if(deque->front == deque->end)
    deque->front = deque->buffer;
  
  // if we have writers wanting to put data into the
  // queue then signal there is a free spot
  if(succeeded(has_wait_tasks(&deque->writers)))
    signal_from_isr(&deque->writers);
  
  return s_ok;
  }

result_t deque_create(uint16_t element_size, uint16_t length, deque_p *handle)
  {
  if(element_size == 0 ||
      length == 0 ||
      handle == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)neutron_malloc(sizeof(deque_t) + (element_size * length));
  
  memset(deque, 0, sizeof(deque_t));

  if(deque == 0)
    return e_not_enough_memory;

  deque->length = length;
	deque->back = deque->buffer;
	deque->front = deque->buffer;
	deque->end = deque->buffer + (length * element_size);
  deque->size = element_size;

  *handle = deque;

  return s_ok;
  }

result_t deque_close(deque_p deque)
  {
  if(deque == 0)
    return e_bad_parameter;

  neutron_free(deque);

  return s_ok;
  }

result_t can_pop(deque_p deque)
  {
  if(deque == 0)
    return e_bad_parameter;

  return deque->count > 0 ? s_ok : s_false;
  }

result_t count(deque_p deque, uint16_t *value)
  {
  if(deque == 0)
    return e_bad_parameter;

  *value = deque->count;

  return s_ok;
  }

result_t push_back(deque_p deque, const void *item, uint32_t max_wait)
  {
  if(deque == 0)
    return e_bad_parameter;

  enter_critical();
  while(deque->count >= deque->length)
    {
    if(max_wait == 0)
      {
      exit_critical();

      return e_no_space;
      }
    
    if(failed(semaphore_wait(&deque->writers, max_wait)))        // suspend task
      return e_timeout_error;
    
    enter_critical();
    }
  
  // copy the item into the queue
  memcpy(deque->back, item, deque->size);

  deque->count++;
  deque->back += deque->size;

  if(deque->back >= deque->end)
    deque->back = deque->buffer;
  
  // if a task is waiting on the queue then we release the
  // task
  if(succeeded(has_wait_tasks(&deque->readers)))
    signal_from_isr(&deque->readers);

  exit_critical();

  return s_ok;
  }

result_t pop_front(deque_p deque, void *item, uint32_t max_wait)
  {
  if(deque == 0)
    return e_bad_parameter;

  enter_critical();
  if(deque->count == 0)
    {
    if(failed(semaphore_wait(&deque->readers, max_wait)))     // wait on an item
      return e_timeout_error;

    enter_critical();
    }

  if(item != 0)
    memcpy(item, deque->front, deque->size);
  
  deque->front += deque->size;
  deque->count--;
  
  if(deque->front == deque->end)
    deque->front = deque->buffer;
  
  // if we have tasks waiting on a slot then signal them
  if(succeeded(has_wait_tasks(&deque->writers)))
    signal_from_isr(&deque->writers);

  exit_critical();

  return s_ok;
  }

result_t at(deque_p deque, uint16_t offset, void *buffer)
  {
  if(deque == 0)
    return e_bad_parameter;

  if(buffer == 0)
    return e_bad_parameter;

  enter_critical();
  if(offset >= deque->count)
    {
    exit_critical();

    return e_bad_parameter;
    }

  uint16_t num_in_front = (deque->end - deque->front) / deque->size;

  uint8_t *item_ptr;

  if(offset < num_in_front)
    item_ptr = deque->front + (offset * deque->size);
  else
    {
    // remove the 'n' items at the front of the queue
    offset -= num_in_front;
    // item is more toward the back where the items are being pushed
    item_ptr = deque->buffer + (offset * deque->size);
    }

  memcpy(buffer, item_ptr, deque->size);

  exit_critical();
  return s_ok;
  }

result_t capacity(deque_p deque, uint16_t *value)
  {
  if(deque == 0)
    return e_bad_parameter;

  *value = deque->length - deque->count;

  return s_ok;
  }

result_t reset(deque_p deque)
  {
  if(deque == 0)
    return e_bad_parameter;

	deque->back = deque->buffer;
	deque->front = deque->buffer;
  deque->count = 0;
  
  return s_ok;
  }