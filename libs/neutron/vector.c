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
#include "neutron.h"

typedef struct _vector_impl_t {
  size_t version;
  uint16_t element_size;  // how large each element is
  byte_t *buffer;      // buffer of data
  uint16_t length;      // how much data allocated
  uint16_t end;         // end of the vector

} vector_impl_t;

static inline result_t is_valid_vector(handle_t hndl)
  {
  if(hndl == 0 ||
      ((vector_impl_t *)hndl)->version != sizeof(vector_impl_t))
    return e_bad_handle;

  return s_ok;
  }

static inline uint16_t alloc_size(uint16_t size)
  {
  return ((size-1)|31)+1;
  }

static result_t ensure_space(vector_impl_t *vec, uint16_t length)
  {
  if(vec->buffer != 0 &&
      vec->end >= length)
    return s_ok;

  size_t array_size = alloc_size(length);

  if(vec->length == 0 && vec->buffer != 0)
    {
    neutron_free(vec->buffer);
    vec->buffer = 0;
    }

  if(vec->buffer != 0)
    vec->buffer = (byte_t *)neutron_realloc(vec->buffer, array_size * vec->element_size);
  else
    vec->buffer = (byte_t *)neutron_malloc(array_size * vec->element_size);

  if(vec->buffer == 0)
    {
    vec->end = 0;
    vec->length = 0;
    return e_no_space;
    }

  vec->end = array_size;

  return s_ok;
  }

static inline byte_t *calculate_offset(vector_impl_t *vec, uint16_t at)
  {
  return &vec->buffer[at * vec->element_size];
  }

result_t vector_create(uint16_t element_size, handle_t *hndl)
  {
  if(element_size == 0 ||
      hndl == 0)
    return e_bad_parameter;

  vector_impl_t *vec = (vector_impl_t *)neutron_malloc(sizeof(vector_impl_t));

  if(vec == 0)
    return e_no_space;

  memset(vec, 0, sizeof(vector_impl_t));
  vec->version = sizeof(vector_impl_t);
  vec->element_size = element_size;

  *hndl = vec;
  return s_ok;
  }

result_t vector_copy(uint16_t element_size, uint16_t length, const void *elements, handle_t *hndl)
  {
  result_t result;

  if(length == 0 ||
      element_size == 0 ||
      elements == 0 ||
      hndl == 0)
    return e_bad_parameter;

  vector_impl_t *vec = (vector_impl_t *)neutron_malloc(sizeof(vector_impl_t));

  if(vec == 0)
    return e_no_space;


  memset(vec, 0, sizeof(vector_impl_t));
  vec->version = sizeof(vector_impl_t);
  vec->element_size = element_size;

  if(failed(result = ensure_space(vec, length)))
    {
    neutron_free(vec);
    return e_no_space;
    }

  memcpy(vec->buffer, elements, element_size * length);
  vec->length = length;

  *hndl = vec;
  return s_ok;
  }

result_t vector_close(handle_t hndl)
  {
  result_t result;
  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(vec->buffer != 0)
    neutron_free(vec->buffer);

  neutron_free(vec);

  return s_ok;
  }

result_t vector_expand(handle_t hndl, uint16_t size, incarnate_element_fn callback, void *parg)
  {
  result_t result;
  
  if(hndl == 0 || size == 0)
    return e_bad_parameter;
  
  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(failed(result = ensure_space(vec, vec->length + size)))
    return result;

  if(callback != 0)
    {
    uint16_t elem;
    // incarnate the element
    for(elem = vec->length; elem < (vec->length + size); elem++)
      (*callback)(calculate_offset(vec, elem), parg);
    }
  else
    memset(calculate_offset(vec, vec->length), 0, vec->element_size * size);

  vec->length += size;

  return s_ok;
  }

result_t vector_push_back(handle_t hndl, const void *element)
  {
  result_t result;
  
  if(hndl == 0 || element == 0)
    return e_bad_parameter;
  
  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(failed(result = ensure_space(vec, vec->length +1)))
    return result;

  memcpy(calculate_offset(vec, vec->length), element, vec->element_size);
  vec->length++;

  return s_ok;
  }

result_t vector_pop_back(handle_t hndl, void *element)
  {
  result_t result;
  
  if(hndl == 0)
    return e_bad_parameter;
  
  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(vec->length == 0)
    return e_not_found;
  
  if(element != 0)
    memcpy(element, calculate_offset(vec, vec->length-1), vec->element_size);
  
  vec->length--;

  return s_ok;
  }


result_t vector_count(handle_t hndl, uint16_t *value)
  {
  result_t result;
  if(hndl == 0 ||
      value == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  *value = vec->length;

  return s_ok;
  }

result_t vector_truncate(handle_t hndl, uint16_t length)
  {
  result_t result;
  if (hndl == 0)
    return e_bad_parameter;

  if (failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;

  if(vec->length < length)
    return e_bad_parameter;

  // elements past the length are lost
  vec->length = length;

  return s_ok;
  }

result_t vector_at(handle_t hndl, uint16_t pos, void *element)
  {
  result_t result;
  if(hndl == 0 ||
      element == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(pos >= vec->length)
    return e_bad_parameter;

  memcpy(element, calculate_offset(vec, pos), vec->element_size);

  return s_ok;
  }

result_t vector_set(handle_t hndl, uint16_t pos, const void *element)
  {
  result_t result;
  if(hndl == 0 ||
      element == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(pos >= vec->length)
    return e_bad_parameter;

  memcpy(calculate_offset(vec, pos), element, vec->element_size);

  return s_ok;
  }

result_t vector_begin(handle_t hndl, void **it)
  {
  result_t result;
  if(hndl == 0 ||
      it == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(failed(result = ensure_space(vec, 32)))
    return result;

  *it = vec->buffer;
  return s_ok;
  }
result_t vector_end(handle_t hndl, void **it)
  {
  result_t result;
  if(hndl == 0 ||
      it == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(failed(result = ensure_space(vec, 32)))
    return result;

  *it = calculate_offset(vec, vec->length);
  return s_ok;
  }

result_t vector_empty(handle_t hndl)
  {
  result_t result;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;

  return vec->length == 0 ? s_ok : s_false;
  }

result_t vector_clear(handle_t hndl)
  {
  result_t result;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  vec->length = 0;

  return s_ok;
  }

result_t vector_insert(handle_t hndl, uint16_t at, const void *element)
  {
  result_t result;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(at > vec->length)
    return e_bad_parameter;

  if(failed(result = ensure_space(vec, vec->length + 1)))
    return result;

  if(at < vec->length)
    {
    // move elements up
    uint16_t i;
    for(i = vec->length; i > at; i--)
      memcpy(calculate_offset(vec, i), calculate_offset(vec, i-1), vec->element_size);
    }

  memcpy(calculate_offset(vec, at), element, vec->element_size);
  vec->length++;

  return s_ok;
  }

static inline void *address_of(vector_impl_t *vec, uint16_t pos)
  {
  return ((uint8_t *)vec->buffer) + (vec->element_size *pos);
  }

static void sort(vector_impl_t *vec, compare_element_fn comp, swap_fn swap, uint16_t begin, uint16_t end)
  {
  if(end > begin)
    {
    void *pivot = address_of(vec, begin);
    uint16_t l = begin;
    uint16_t r = end;
    while(l < r)
      {
      if((*comp)(address_of(vec, l), pivot) <= 0)
        {
        l++;
        }
      else if((*comp)(address_of(vec, r), pivot) > 0)
        {
        r--;
        }
      else if(l < r)
        {
        (*swap)(address_of(vec, l), address_of(vec, r));
        }
      }
    l--;
    (*swap)(address_of(vec, begin), address_of(vec, l));
    sort(vec, comp, swap, begin, l);
    sort(vec, comp, swap, r, end);
    }
  }

result_t vector_sort(vector_t hndl, compare_element_fn comp, swap_fn swap)
  {
  result_t result;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;

  sort(vec, comp, swap, 0, vec->length-1);

  return s_ok;
  }

result_t vector_erase(handle_t hndl, uint16_t at)
  {
  result_t result;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;
  if(at >= vec->length)
    return e_bad_parameter;
  // move elements down
  uint16_t i;
  for(i = at; i < vec->length; i++)
    memcpy(calculate_offset(vec, i), calculate_offset(vec, i+1), vec->element_size);

  vec->length--;

  return s_ok;
  }

result_t vector_assign(handle_t hndl, uint16_t length, const void *elements)
  {
  result_t result;
  if(hndl == 0 ||
      length == 0 ||
      elements == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;

  if(failed(result = vector_clear(hndl)))
    return result;

  if(failed(result = ensure_space(vec, length)))
    return result;

  // assign the elements
  memcpy(calculate_offset(vec, 0), elements, vec->element_size * length);
  vec->length = length;

  return s_ok;
  }

result_t vector_append(handle_t hndl, uint16_t length, const void *elements)
  {
  result_t result;
  if(hndl == 0 ||
      length == 0 ||
      elements == 0)
    return e_bad_parameter;

  if(failed(result = is_valid_vector(hndl)))
    return result;

  vector_impl_t *vec = (vector_impl_t *)hndl;

  if(failed(result = ensure_space(vec, vec->length + length)))
    return result;

  memcpy(calculate_offset(vec, vec->length), elements, vec->element_size * length);
  vec->length += length;

  return s_ok;
  }
