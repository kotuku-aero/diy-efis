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
#include "stream.h"
#include <stdlib.h>
#include <string.h>

typedef struct _manifest_handle_t
  {
  stream_handle_t stream;
  // only 1 of these can be valid at a time
  handle_t reg_stream;      // source registry stream if not 0
  const char *literal;      // in-memory stream if not 0
  uint16_t source_length;   // size of the source buffer
  uint16_t offset;      // current offset in the stream.
  uint16_t length;      // length of the stream, can be less than the buffer count

  int32_t window_pos;   // decoder start offset (initially -100 which is invalid
  char buffer[4];      // sliding work buffer for the decoder.
  } manifest_handle_t;

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

static result_t decode_byte(manifest_handle_t *stream, uint16_t offset, uint8_t *value)
  {
  result_t result;
  // fast divide by 3 = (n * 86) / 256
  //
  // is ((offset * 64) + (offset * 16) + (offset * 4) + (offset * 2)) / 256 
  int32_t required_window = ((offset << 6) + (offset << 4) + (offset << 2) + (offset << 1)) >> 8;

  // adjust to base 64 chars (4 chars == 3 bytes)
  required_window << 2;

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
  required_window >> 1;
  required_window += (required_window >> 1);

  offset -= (uint16_t) required_window;   // will be 0, 1, 2

  switch (offset)
    {
    case 0 :
      *value = (esab64[stream->buffer[0]]<< 2) | (esab64[stream->buffer[1]] >> 4);
      break;
    case 1 :
      *value = (esab64[stream->buffer[1]] << 4) | (esab64[stream->buffer[2]] << 4);
      break;
    case 2 :
      *value = (esab64[stream->buffer[2]] << 6) | esab64[stream->buffer[3]];
     break;
    default :
      return e_unexpected;
    }
  
  return s_ok;
  }

static result_t check_handle(handle_t hndl)
  {
  if (hndl == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;
  if (stream->version != sizeof(manifest_handle_t))
    return e_bad_parameter;

  return s_ok;
  }

static result_t manifest_stream_eof(stream_handle_t *hndl)
  {
  result_t result;
  if (hndl == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)hndl;

  return stream->offset == stream->length ? s_ok : s_false;
  }

static result_t manifest_stream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  if (hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)hndl;

  uint16_t num_to_read = stream->length - stream->offset;

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

static result_t manifest_stream_getpos(stream_handle_t *hndl, uint16_t *pos)
  {
  result_t result;
  if (hndl == 0 || pos == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)hndl;
  *pos = stream->offset;

  return s_ok;
  }

static result_t manifest_stream_setpos(stream_handle_t *hndl, uint16_t pos)
  {
  result_t result;
  if (hndl == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)hndl;

  if (pos > stream->length)
    return e_bad_parameter;

  stream->offset = pos;

  return s_ok;
  }

static result_t manifest_stream_length(stream_handle_t *hndl, uint16_t *length)
  {
  result_t result;
  if (hndl == 0 || length == 0)
    return e_bad_parameter;

  if (failed(result = check_handle(hndl)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)hndl;

  *length = stream->length;
  return s_ok;
  }

static result_t manifest_stream_close(stream_handle_t *hndl)
  {
  result_t result;
  if (failed(result = check_handle(hndl)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)hndl;

  if(stream->reg_stream != 0)
     result = stream_close(stream->reg_stream);

  neutron_free(stream);

  return result;
  }

static void init_stream(manifest_handle_t *stream)
  {
  memset(stream, 0, sizeof(manifest_handle_t));

  // set up the callbacks
  stream->stream.version = sizeof(manifest_handle_t);
  stream->stream.stream_eof = manifest_stream_eof;
  stream->stream.stream_getpos = manifest_stream_getpos;
  stream->stream.stream_length = manifest_stream_length;
  stream->stream.stream_read = manifest_stream_read;
  stream->stream.stream_setpos = manifest_stream_setpos;
  stream->stream.stream_truncate = 0;
  stream->stream.stream_write = 0;
  stream->stream.stream_close = manifest_stream_close;
  stream->stream.stream_delete = 0;
  }

result_t manifest_open(memid_t key, const char *path, handle_t *hndl)
  {
  result_t result;
  handle_t sh;
  // open a stream
  if(failed(result = stream_open(key, path, &sh)))
    return result;

  manifest_handle_t *stream = (manifest_handle_t *)neutron_malloc(sizeof(manifest_handle_t));

  init_stream(stream);

  stream->reg_stream = sh;

  if (failed(result = stream_length(sh, &stream->source_length)))
    {
    stream_close(sh);
    neutron_free(stream);

    return result;
    }

  *hndl = stream;
  return s_ok;

  }

result_t manifest_create(const char *literal, handle_t *hndl)
  {
  result_t result;
  manifest_handle_t *stream = (manifest_handle_t *)neutron_malloc(sizeof(manifest_handle_t));

  init_stream(stream);
  stream->literal = literal;
  stream->source_length = strlen(literal);

  *hndl = stream;
  return s_ok;
  }
