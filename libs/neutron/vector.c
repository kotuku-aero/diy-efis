/*
 * vector.c
 *
 *  Created on: 18/02/2017
 *      Author: peter
 */
#include "neutron.h"

typedef struct _vector_impl_t {
  size_t version;
  uint16_t element_size;  // how large each element is
  byte_t *buffer;     // buffer of data
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
    kfree(vec->buffer);
    vec->buffer = 0;
    }

  if(vec->buffer != 0)
    vec->buffer = (byte_t *)krealloc(vec->buffer, array_size * vec->element_size);
  else
    vec->buffer = (byte_t *)kmalloc(array_size * vec->element_size);

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

  vector_impl_t *vec = (vector_impl_t *)kmalloc(sizeof(vector_impl_t));

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

  vector_impl_t *vec = (vector_impl_t *)kmalloc(sizeof(vector_impl_t));

  if(vec == 0)
    return e_no_space;


  memset(vec, 0, sizeof(vector_impl_t));
  vec->version = sizeof(vector_impl_t);
  vec->element_size = element_size;

  if(failed(result = ensure_space(vec, length)))
    {
    kfree(vec);
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
    kfree(vec->buffer);

  kfree(vec);

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