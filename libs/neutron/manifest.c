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

typedef struct _manifest_handle_t
  {
  stream_t stream;
  // only 1 of these can be valid at a time
  handle_t reg_stream;      // source registry stream if not 0
  const char *literal;      // in-memory stream if not 0
  uint32_t source_length;   // size of the source buffer
  uint32_t offset;      // current offset in the stream.
  uint32_t length;      // length of the stream, can be less than the buffer count

  int32_t window_pos;   // decoder start offset (initially -100 which is invalid
  char buffer[16];      // sliding work buffer for the decoder.
  } manifest_stream_t;

static const typeid_t manifest_stream_type;

extern result_t reg_stream_close(handle_t hndl);

static result_t manifest_stream_close(handle_t hndl)
  {
  result_t result;
  manifest_stream_t *stream;
  if (failed(result = is_typeof(hndl, &manifest_stream_type, (void **)&stream)))
    return result;

  if (stream->reg_stream != 0)
    result = reg_stream_close(stream->reg_stream);

  neutron_free(stream);

  return result;
  }

static const typeid_t manifest_stream_type = 
  {
  .name = "manifest",
  .etherealize = manifest_stream_close,
  .base = &stream_type
  };

// inefficent but very fast...
static const uint8_t esab64[256] =
  {
  /* ASCII table */
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
  64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
  64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
  };

static result_t decode_byte(manifest_stream_t *stream, uint16_t offset, uint8_t *value)
  {
  result_t result;
  int32_t offs_32 = offset;
  int32_t required_window = offs_32 / 3;

  // adjust to base 64 chars (4 chars == 3 bytes)
  required_window <<= 2;

  // see if the bytes exist in the window
  if (stream->window_pos != required_window)
    {
    // fetch the characters
    stream->window_pos = required_window;
    
    if (stream->literal != 0)
      memcpy(stream->buffer, stream->literal + required_window, 4);
    else
      {
      if(failed(result = stream_read(stream->reg_stream, stream->buffer, 4, 0)))
        return result;
      }
    }

  // calculate the window * 3
  required_window >>= 1;
  required_window += (required_window >> 1);

  offs_32 -= required_window;   // will be 0, 1, 2

  switch (offs_32)
    {
    case 0 :
      *value = (esab64[stream->buffer[0]]<< 2) | (esab64[stream->buffer[1]] >> 4);
      break;
    case 1 :
      *value = (esab64[stream->buffer[1]] << 4) | (esab64[stream->buffer[2]] >> 2);
      break;
    case 2 :
      *value = (esab64[stream->buffer[2]] << 6) | esab64[stream->buffer[3]];
     break;
    default :
      return e_unexpected;
    }
  
  return s_ok;
  }

static result_t manifest_stream_eof(handle_t hndl)
  {
  result_t result;
  manifest_stream_t *stream;
  if (failed(result = is_typeof(hndl, &manifest_stream_type, (void **)&stream)))
    return result;

  return stream->offset == stream->length ? s_ok : s_false;
  }

static result_t manifest_stream_read(handle_t hndl, void *buffer, uint32_t size, uint32_t *read)
  {
  result_t result;
  if (hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  manifest_stream_t *stream = (manifest_stream_t *)hndl;

  uint32_t num_to_read = stream->length - stream->offset;

  if (num_to_read == 0)
    return e_no_more_information;

  if (num_to_read > size)
    num_to_read = size;

  uint8_t *ptr = (uint8_t *)buffer;

  while (num_to_read--)
    {
    if(failed(result = decode_byte(stream, stream->offset, ptr)))
      return result;

    stream->offset++;
    ptr++;
    if (read != 0)
      *read = *read + 1;
    }

  return s_ok;
  }

static result_t manifest_stream_getpos(handle_t hndl, uint32_t *pos)
  {
  result_t result;
  manifest_stream_t *stream;
  if (failed(result = is_typeof(hndl, &manifest_stream_type, (void **)&stream)))
    return result;

  *pos = stream->offset;

  return s_ok;
  }

static result_t manifest_stream_setpos(handle_t hndl, uint32_t pos, uint32_t dir)
  {
  result_t result;
  manifest_stream_t *stream;
  if (failed(result = is_typeof(hndl, &manifest_stream_type, (void **)&stream)))
    return result;

  if (pos > stream->length)
    return e_bad_parameter;

  stream->offset = pos;

  return s_ok;
  }

static result_t manifest_stream_length(handle_t hndl, uint32_t *length)
  {
  result_t result;
  manifest_stream_t *stream;
  if (failed(result = is_typeof(hndl, &manifest_stream_type, (void **)&stream)))
    return result;

  *length = stream->length;
  return s_ok;
  }

static void init_stream(manifest_stream_t *stream)
  {
  memset(stream, 0, sizeof(manifest_stream_t));

  // set up the callbacks
  stream->stream.base.type = &manifest_stream_type;
  stream->stream.eof = manifest_stream_eof;
  stream->stream.getpos = manifest_stream_getpos;
  stream->stream.length = manifest_stream_length;
  stream->stream.read = manifest_stream_read;
  stream->stream.setpos = manifest_stream_setpos;
  stream->stream.truncate = 0;
  stream->stream.write = 0;
  }

result_t manifest_open(memid_t key, const char *path, handle_t *hndl)
  {
  result_t result;
  handle_t sh;
  // open a stream
  if(failed(result = reg_stream_open(key, path, STREAM_O_RD, &sh)))
    return result;

  manifest_stream_t *stream;
  if (failed(result = neutron_malloc(sizeof(manifest_stream_t), (void **)&stream)))
    return result;

  init_stream(stream);

  stream->reg_stream = sh;

  if (failed(result = stream_length(sh, &stream->source_length)))
    {
    close_handle(sh);
    neutron_free(stream);

    return result;
    }

  if (failed(result = stream_read(stream->reg_stream, stream->buffer, 4, 0)))
    return result;

  *hndl = (handle_t) stream;
  return s_ok;

  }

result_t manifest_create(const char *literal, handle_t *hndl)
  {
  result_t result;
  manifest_stream_t *stream;
  if (failed(result = neutron_malloc(sizeof(manifest_stream_t), (void **)&stream)))
    return result;

  init_stream(stream);
  stream->literal = literal;
  stream->source_length = strlen(literal);
  stream->length = (stream->source_length / 4) * 3;

  memcpy(stream->buffer, stream->literal, 4);

  *hndl =  (handle_t) stream;
  return s_ok;
  }
