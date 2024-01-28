#ifndef __type_vector_h__
#define __type_vector_h__
/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define type_vector_malloc neutron_malloc
#define type_vector_free neutron_free
#define type_vector_realloc neutron_realloc

#define type_vector_t(_ty) \
static inline  uint16_t _ty##_alloc_size(uint16_t size)   \
  {   \
  return ((size - 1) | 31) + 1;   \
  }   \
   \
typedef struct _##_ty##s_t {   \
  _ty##_t *buffer;   \
  uint16_t length;   \
  uint16_t end;   \
  } _ty##s_t;   \
   \
typedef void (*incarnate_##_ty##_fn)( _ty##_t *element);   \
   \
static inline  result_t _ty##s_ensure_space( _ty##s_t *vec, uint16_t length)   \
  {   \
  result_t result; \
  if (vec->buffer != 0 &&   \
    vec->end >= length)   \
    return s_ok;   \
   \
  uint16_t array_size = _ty##_alloc_size(length);   \
   \
  if (vec->length == 0 && vec->buffer != 0)   \
    {   \
    type_vector_free(vec->buffer);   \
    vec->buffer = 0;   \
    }   \
   \
  if (vec->buffer != 0) \
    { \
    if(failed(result = type_vector_realloc(array_size * sizeof( _ty##_t ), (void **)&vec->buffer))) \
      return result; \
    } \
  else \
    { \
    if(failed(result = type_vector_malloc(array_size * sizeof( _ty##_t ), (void **) &vec->buffer ))) \
      return result; \
    } \
   \
  vec->end = array_size;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_create( _ty##s_t **vec)   \
  {   \
  result_t result; \
  if(failed(result = type_vector_malloc(sizeof( _ty##s_t ), (void **)vec))) \
    return result; \
   \
  memset(*vec, 0, sizeof( _ty##s_t ));   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_close( _ty##s_t *vec)   \
  {   \
  if (vec->buffer != 0)   \
    type_vector_free(vec->buffer);   \
   \
  return type_vector_free(vec);   \
  }   \
   \
static inline  result_t _ty##s_expand( _ty##s_t *vec, uint16_t size, incarnate_##_ty##_fn callback)   \
  {   \
  result_t result;   \
   \
  if (size == 0)   \
    return e_bad_parameter;   \
   \
  if (failed(result = _ty##s_ensure_space(vec, vec->length + size)))   \
    return result;   \
   \
  if (callback != 0)   \
    {   \
    uint16_t elem;   \
    for (elem = vec->length; elem < (vec->length + size); elem++)   \
      (*callback)(vec->buffer + elem);   \
    }   \
  else   \
    memset(vec->buffer + vec->length, 0, sizeof( _ty##_t ) * size);   \
   \
  vec->length += size;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_push_back( _ty##s_t *vec, const _ty##_t *element)   \
  {   \
  result_t result;   \
  if (element == 0)   \
    return e_bad_parameter;   \
   \
  if (failed(result = _ty##s_ensure_space(vec, vec->length + 1)))   \
    return result;   \
   \
  memcpy(vec->buffer + vec->length, element, sizeof( _ty##_t ));   \
  vec->length++;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_pop_back( _ty##s_t *vec, _ty##_t *element)   \
  {   \
  if (vec->length == 0)   \
    return e_not_found;   \
   \
  if (element != 0)   \
    memcpy(element, vec->buffer + vec->length - 1, sizeof( _ty##_t ));   \
   \
  vec->length--;   \
   \
  return s_ok;   \
  }   \
   \
static inline  uint16_t _ty##s_count( _ty##s_t *vec)   \
  {   \
  return vec->length;   \
  }   \
   \
static inline  result_t _ty##s_truncate( _ty##s_t *vec, uint16_t length)   \
  {   \
  if (vec->length < length)   \
    return e_bad_parameter;   \
   \
  vec->length = length;   \
   \
  return s_ok;   \
  }   \
   \
static inline  _ty##_t * _ty## s_begin( _ty##s_t *vec)   \
  {   \
  return vec->buffer;   \
  }   \
   \
static inline  _ty##_t * _ty##s_end( _ty##s_t *vec)   \
  {   \
  return vec->buffer + vec->length;   \
  }   \
   \
static inline  result_t _ty##s_set( _ty##s_t *vec, uint16_t pos, const _ty##_t *element)   \
  {   \
  if (element == 0)   \
    return e_bad_parameter;   \
   \
  if (pos >= vec->length)   \
    return e_bad_parameter;   \
   \
  memcpy(vec->buffer + pos, element, sizeof( _ty##_t ));   \
   \
  return s_ok;   \
  }   \
   \
static inline  bool _ty##s_empty( _ty##s_t *vec)   \
  {   \
  return vec->length == 0;   \
  }   \
   \
static inline  result_t _ty##s_clear( _ty##s_t *vec)   \
  {   \
  vec->length = 0;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_insert( _ty##s_t *vec, uint16_t at, const _ty##_t *element)   \
  {   \
  result_t result;   \
   \
  if (at > vec->length)   \
    return e_bad_parameter;   \
   \
  if (failed(result = _ty##s_ensure_space(vec, vec->length + 1)))   \
    return result;   \
   \
  if (at < vec->length)   \
    {   \
    uint16_t i;   \
    for (i = vec->length; i > at; i--)   \
      memcpy(vec->buffer + i, vec->buffer + (i - 1), sizeof( _ty##_t ));   \
    }   \
   \
  memcpy(vec->buffer + at, element, sizeof( _ty##_t ));   \
  vec->length++;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_erase( _ty##s_t *vec, uint16_t at)   \
  {   \
  if (at >= vec->length)   \
    return e_bad_parameter;   \
   \
  if (vec->length > 1)   \
    {   \
    memcpy(vec->buffer + at, vec->buffer + (at + 1), (vec->length - 1) * sizeof( _ty##_t ));   \
    }   \
   \
  vec->length--;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_assign( _ty##s_t *vec, uint16_t length, const _ty##_t *elements)   \
  {   \
  result_t result;   \
  if (length == 0 ||   \
    elements == 0)   \
    return e_bad_parameter;   \
   \
  if (failed(result = _ty##s_clear(vec)))   \
    return result;   \
   \
  if (failed(result = _ty##s_ensure_space(vec, length)))   \
    return result;   \
   \
  memcpy(vec->buffer, elements, sizeof( _ty##_t ) * length);   \
  vec->length = length;   \
   \
  return s_ok;   \
  }   \
   \
static inline  result_t _ty##s_append( _ty##s_t *vec, uint16_t length, const _ty##_t *elements)   \
  {   \
  result_t result;   \
   \
  if (length == 0 ||   \
    elements == 0)   \
    return e_bad_parameter;   \
   \
  if (failed(result = _ty##s_ensure_space(vec, vec->length + length)))   \
    return result;   \
   \
  memcpy(vec->buffer + vec->length, elements, sizeof( _ty##_t ) * length);   \
  vec->length += length;   \
   \
  return s_ok;   \
  }   \


#ifdef __cplusplus
}
#endif

#endif
