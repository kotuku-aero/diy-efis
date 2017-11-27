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
#include "deque.h"

#include <string.h>
#include <stdlib.h>

result_t deque_create(uint16_t element_size, uint16_t length, handle_t *handle)
  {
  result_t result;
  if(element_size == 0 ||
      length == 0 ||
      handle == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)malloc(sizeof(deque_t));

  if(deque == 0)
    return e_not_enough_memory;

  if(failed(result = semaphore_create(&deque->readers)))
    {
    free(deque);
    return result;
    }

  if(failed(result = semaphore_create(&deque->writers)))
    {
    semaphore_close(deque->readers);
    free(deque);
    return result;
    }

  deque->buffer = malloc(element_size * length);
  if(deque->buffer == 0)
    {
    semaphore_close(deque->readers);
    semaphore_close(deque->writers);
    free(deque);
    return e_not_enough_memory;
    }

  deque->length = length;
	deque->back = deque->buffer;
	deque->front = deque->buffer;
	deque->end = ((uint8_t *)deque->buffer) + (length * element_size);
  deque->count = 0;
  deque->size = element_size;

  *handle = deque;

  return s_ok;
  }

result_t deque_close(handle_t hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;
  semaphore_close(deque->readers);
  semaphore_close(deque->writers);
  free(deque->buffer);
  free(deque);

  return s_ok;
  }

result_t can_pop(handle_t hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;

  return deque->count > 0 ? s_ok : s_false;
  }

result_t count(handle_t hndl, uint16_t *value)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;

  *value = deque->count;

  return s_ok;
  }

result_t push_back(handle_t hndl, const void *item, uint32_t max_wait)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;

  enter_critical();
  while(deque->count >= deque->length)
    {
    exit_critical();
    if(failed(semaphore_wait(deque->writers, max_wait)))        // suspend task
      return e_timeout_error;
    enter_critical();
    }
  
  // copy the item into the queue
  memcpy(deque->back, item, deque->size);

  deque->count++;
  deque->back = ((uint8_t *)deque->back) + deque->size;

  if(deque->back >= deque->end)
    deque->back = deque->buffer;
  
  // if a task is waiting on the
  if(succeeded(has_wait_tasks(deque->readers)))
    {
    exit_critical();
    semaphore_signal(deque->readers);
    }
  else
    exit_critical();

  return s_ok;
  }

result_t pop_front(handle_t hndl, void *item, uint32_t max_wait)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;

  enter_critical();
  while(deque->count == 0)
    {
    exit_critical();
    if(failed(semaphore_wait(deque->readers, max_wait)))     // wait on an item
      return e_timeout_error;

    enter_critical();
    }

  if(item != 0)
    memcpy(item, deque->front, deque->size);
  
  deque->front = ((uint8_t *)deque->front) + deque->size;
  deque->count--;
  
  if(deque->front == deque->end)
    deque->front = deque->buffer;
  
  // if we have tasks waiting on a slot then signal them
  if(deque->writers != 0 && succeeded(has_wait_tasks(deque->writers)))
    {
    exit_critical();
    semaphore_signal(deque->writers);
    }
  else
    exit_critical();

  return s_ok;
  }

result_t at(handle_t hndl, uint16_t offset, void *buffer)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;

  if(buffer == 0)
    return e_bad_parameter;

  enter_critical();
  if(offset >= deque->count)
    {
    exit_critical();

    return e_bad_parameter;
    }

  uint16_t num_in_front = (((uint8_t *)deque->end) - ((uint8_t *)deque->front)) / deque->size;

  uint8_t *item_ptr;

  if(offset < num_in_front)
    item_ptr = ((uint8_t *)deque->front) + (offset * deque->size);
  else
    {
    // remove the 'n' items at the front of the queue
    offset -= num_in_front;
    // item is more toward the back where the items are being pushed
    item_ptr = ((uint8_t *)deque->buffer) + (offset * deque->size);
    }

  memcpy(buffer, item_ptr, deque->size);

  exit_critical();
  return s_ok;
  }

result_t capacity(handle_t hndl, uint16_t *value)
  {
  if(hndl == 0)
    return e_bad_parameter;

  deque_t *deque = (deque_t *)hndl;

  *value = deque->length - deque->count;

  return s_ok;
  }
