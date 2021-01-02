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
#include "stream.h"
#include <stdlib.h>
#include <string.h>

typedef struct _strstream_handle_t
  {
  stream_handle_t stream;
  
  // if the stream.stream_write function is 0 then this is a readonly
  // buffer
  union {
    vector_p rw_buffer;      // vector... if 0 then a readonly stream
    const uint8_t *ro_buffer; // readonly vector
    };
  
  uint32_t offset;      // current offset in the stream.
  uint32_t length;      // length of the stream, can be less than the buffer count
  } strstream_handle_t;

static result_t check_handle(stream_handle_t *stream)
  {
  if(stream == 0)
    return e_bad_parameter;
  
  if (stream->version != sizeof (strstream_handle_t))
    return e_bad_parameter;

  return s_ok;
  }

result_t strstream_get(stream_p hndl, const void **lit)
  {
  result_t result;
  if(lit == 0 || hndl == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle((stream_handle_t *)hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  if(stream->stream.stream_write == 0)
    {
    *lit = stream->ro_buffer;
    return s_ok;
    }
  
  return vector_begin(stream->rw_buffer, (void **)lit);
  }

static result_t strstream_eof(stream_handle_t *hndl)
  {
  result_t result;
  if(hndl == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  return stream->offset == stream->length ? s_ok : s_false;  
  }

static result_t strstream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  if(hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  uint32_t num_to_read = stream->length - stream->offset;
  
  if(num_to_read == 0)
    return e_no_more_information;
  
  if(num_to_read > size)
    num_to_read = size;
  
  const uint8_t *src;
  if(stream->stream.stream_write == 0)
    src = stream->ro_buffer;
  else
    {
    if(failed(result = vector_begin(stream->rw_buffer, (void **)&src)))
      return result;
    }
  
  src += stream->offset;
  stream->offset += num_to_read;
  
  if(read != 0)
    *read = num_to_read;
  
  memcpy(buffer, src, num_to_read);
  return s_ok;
  }

static result_t strstream_write(stream_handle_t *hndl, const void *buffer, uint16_t size)
  {
  result_t result;
  if(hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  uint16_t length;
  if(failed(result = vector_count(stream->rw_buffer, &length)))
    return result;
  
  // see if this is an assignment.  in that case update the section of the stream
  for(; stream->offset < length && size > 0; stream->offset++, size--)
    {
    if(failed(result = vector_set(stream->rw_buffer, stream->offset, buffer)))
      return result;
    
    buffer = ((const uint8_t *)buffer) +1;
    }
  
  // if none left then done
  if(size != 0)
    {  
    // append what is left
    if(failed(result = vector_append(stream->rw_buffer, size, buffer)))
      return result;
  
    // seek to the new end of the file
    stream->offset += size;
    }
  
  if(stream->offset > stream->length)
    stream->length = stream->offset;
  
  return s_ok;
  }

static result_t strstream_getpos(stream_handle_t *hndl, uint32_t *pos)
  {
  result_t result;
  if(hndl == 0 || pos == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  *pos = stream->offset;
  
  return s_ok;
  }

static result_t strstream_setpos(stream_handle_t *hndl, uint32_t pos)
  {
  result_t result;
  if(hndl == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  if(pos > stream->length)
    return e_bad_parameter;
  
  stream->offset = pos;
  
  return s_ok;
  }

static result_t strstream_length(stream_handle_t *hndl, uint32_t *length)
  {
  result_t result;
  if(hndl == 0 || length == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  *length = stream->length;
  return s_ok;
  }

static result_t strstream_truncate(stream_handle_t *hndl, uint32_t length)
  {
  result_t result;
  if(hndl == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  if(length > stream->length)
    return e_bad_parameter;
  
  if(stream->offset > length)
    stream->offset = length;
  
  stream->length = length;
  
  return s_ok;
  }

static result_t strstream_close(stream_handle_t *hndl)
  {
  return stream_delete((stream_p)hndl);
  }

static result_t strstream_delete(stream_handle_t *hndl)
  {
  result_t result;
  if(hndl == 0)
    return e_bad_parameter;
  
  if(failed(result = check_handle(hndl)))
    return result;
  
  strstream_handle_t *stream = (strstream_handle_t *)hndl;
  
  if(stream->stream.stream_write != 0 &&
     failed(result = vector_close(stream->rw_buffer)))
    return result;
  
  neutron_free(stream);
  return s_ok;
  }

result_t strstream_create(const uint8_t *lit, uint32_t len, bool read_only, stream_p *hndl)
  {
  if(hndl == 0 ||
     (read_only && lit == 0) ||
     (read_only && len == 0))
    return e_bad_parameter;
  
  strstream_handle_t *stream = (strstream_handle_t *)neutron_malloc(sizeof(strstream_handle_t));
  
  memset(stream, 0, sizeof (strstream_handle_t));

  // set up the callbacks
  stream->stream.version = sizeof(strstream_handle_t);
  stream->stream.stream_eof = strstream_eof;
  stream->stream.stream_getpos = strstream_getpos;
  stream->stream.stream_length = strstream_length;
  stream->stream.stream_read = strstream_read;
  stream->stream.stream_setpos = strstream_setpos;
  stream->stream.stream_truncate = strstream_truncate;
  stream->stream.stream_close = strstream_close;
  stream->stream.stream_delete = strstream_delete;
  
  if(read_only)
    {
    stream->length = len;
    stream->ro_buffer = lit;
    }
  else
    {
    stream->stream.stream_write = strstream_write;

    if(lit != 0)
      vector_copy(sizeof(uint8_t), len, lit, &stream->rw_buffer);
    else
      vector_create(sizeof(uint8_t), &stream->rw_buffer);

    uint16_t len;
    vector_count(stream->rw_buffer, &len);
    stream->length = len;
    }
    
  *hndl = (stream_p) &stream->stream;
  return s_ok;
  }
