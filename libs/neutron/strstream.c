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
#include "stream.h"
#include <stdlib.h>
#include <string.h>

typedef struct _strstream_t
  {
  stream_t stream;
  bool read_only;
  uint8_t *buffer;
  uint32_t size;        // bytes allocated
  uint32_t offset;      // current offset in the stream.
  uint32_t length;      // length of the stream, can be less than the buffer count
  } strstream_t;

static const typeid_t strstream_type;
static result_t strstream_close(handle_t hndl)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  if (!stream->read_only)
    neutron_free(stream->buffer);

  neutron_free(stream);
  return s_ok;
  }


static const typeid_t strstream_type = 
  {
  .name = "strstream_t",
  .etherealize = strstream_close,
  .base = &stream_type
  };

static result_t strstream_eof(handle_t hndl)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  return stream->offset == stream->length ? s_ok : s_false;  
  }

static result_t strstream_read(handle_t hndl, void *buffer, uint32_t size, uint32_t *read)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  if(buffer == 0 || size == 0)
    return e_bad_parameter;
  
  uint32_t num_to_read = stream->length - stream->offset;
  
  if(num_to_read == 0)
    return e_no_more_information;
  
  if(num_to_read > size)
    num_to_read = size;
  
  const uint8_t *src = stream->buffer;

  src += stream->offset;
  stream->offset += num_to_read;
  
  if(read != 0)
    *read = num_to_read;
  
  memcpy(buffer, src, num_to_read);
  return s_ok;
  }

static result_t strstream_write(handle_t hndl, const void *buffer, uint32_t size)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  if(buffer == 0 || size == 0)
    return e_bad_parameter;
  
  const uint8_t *bufp = (const uint8_t *)buffer;

  // expand to fit
  if (stream->size < stream->offset + size)
    {
    uint32_t num_needed = stream->size + size;
    num_needed = ((num_needed - 1) | 127) + 1;

    if ((stream->buffer == 0 && failed(result = neutron_malloc(num_needed, (void **)&stream->buffer))) ||
      failed(result = neutron_realloc(num_needed, (void **)&stream->buffer)))
        return result;
  
    stream->size = num_needed;
    }

  memcpy(stream->buffer + stream->offset, bufp, size);
  // seek to the new end of the file
  stream->offset += size;
  
  return s_ok;
  }

static result_t strstream_getpos(handle_t hndl, uint32_t *pos)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  if(pos == 0)
    return e_bad_parameter;
  
  *pos = stream->offset;
  
  return s_ok;
  }

static result_t strstream_setpos(handle_t hndl, int32_t pos, uint32_t whence)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;
  
  uint32_t new_pos;
  switch (whence)
    {
    case STREAM_SEEK_CUR :
      new_pos = (uint32_t)(((int32_t)stream->offset) + pos);
      break;
    case STREAM_SEEK_END :
      new_pos = (uint32_t)(((int32_t)stream->length) + pos);
      break;
    case STREAM_SEEK_SET :
      new_pos = (uint32_t) pos;
      break;
    }

  if(new_pos > stream->length)
    return e_bad_parameter;
  
  stream->offset = new_pos;
  
  return s_ok;
  }

static result_t strstream_length(handle_t hndl, uint32_t *length)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  if(length == 0)
    return e_bad_parameter;
  
  *length = stream->length;
  return s_ok;
  }

static result_t strstream_truncate(handle_t hndl, uint32_t length)
  {
  result_t result;
  strstream_t *stream;
  if (failed(result = is_typeof(hndl, &strstream_type, (void **)&stream)))
    return result;

  if(length > stream->length)
    return e_bad_parameter;
  
  if(stream->offset > length)
    stream->offset = length;
  
  stream->length = length;
  
  return s_ok;
  }

result_t strstream_create(const uint8_t *lit, uint32_t len, bool read_only, handle_t *hndl)
  {
  if(hndl == 0 ||
     (read_only && lit == 0) ||
     (read_only && len == 0))
    return e_bad_parameter;
  
  result_t result;
  strstream_t *stream;
  if (failed(result = neutron_malloc(sizeof(strstream_t), (void **)&stream)))
    return result;
  
  memset(stream, 0, sizeof (strstream_t));

  // set up the callbacks
  stream->stream.base.type = &strstream_type;
  stream->stream.eof = strstream_eof;
  stream->stream.getpos = strstream_getpos;
  stream->stream.length = strstream_length;
  stream->stream.read = strstream_read;
  stream->stream.setpos = strstream_setpos;
  stream->stream.truncate = strstream_truncate;

  if(read_only)
    {
    stream->length = len;
    stream->buffer = (uint8_t *) lit;
    stream->read_only = true;
    }
  else
    {
    stream->stream.write = strstream_write;

    if (lit != 0)
      stream_write((handle_t)stream, lit, len);
    }
    
  *hndl = (handle_t) stream;
  return s_ok;
  }
