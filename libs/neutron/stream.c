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
#include "registry.h"
#include <string.h>

static result_t check_handle(stream_p hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *) hndl;
  if (stream->version < sizeof (stream_handle_t))
    return e_bad_parameter;

  return s_ok;
  }

result_t stream_close(stream_p hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_close == 0)
    return e_not_implemented;

  return (*(stream->stream_close))(stream);
  }

result_t stream_delete(stream_p hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_delete == 0)
    return e_not_implemented;

  return (*(stream->stream_delete))(stream);
  }

result_t stream_eof(stream_p hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_eof == 0)
    return e_not_implemented;

  return (*(stream->stream_eof))(stream);
  }

result_t stream_read(stream_p hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(buffer == 0 || size == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_read == 0)
    return e_not_implemented;

  return (*(stream->stream_read))(stream, buffer, size, read);
  }

result_t stream_write(stream_p hndl, const void *buffer, uint16_t size)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(buffer == 0 || size == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_write == 0)
    return e_not_implemented;

  return (*(stream->stream_write))(stream, buffer, size);
  }

result_t stream_copy(stream_p from, stream_p to)
  {
  result_t result;
  if(failed(result = check_handle(from)) ||
     failed(result = check_handle(to)))
    return result;

  uint16_t read = 0;
  byte_t *buffer = (byte_t *)neutron_malloc(128);
  if(buffer == 0)
    return e_not_enough_memory;

  uint32_t len;
  if (failed(stream_length(from, &len)))
    return result;

  uint32_t chunk = min(128, len);

  while (chunk > 0 &&
    succeeded(result = stream_read(from, buffer, chunk, &read)) &&
    read > 0 &&
    succeeded(result = stream_write(to, buffer, read)))
    {
    len -= chunk;
    chunk = min(128, len);
    }

  neutron_free(buffer);
  return result;
  }

result_t stream_getpos(stream_p hndl, uint32_t *pos)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(pos == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_getpos == 0)
    return e_not_implemented;

  return (*(stream->stream_getpos))(stream, pos);
  }

result_t stream_setpos(stream_p hndl, uint32_t pos)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_setpos == 0)
    return e_not_implemented;

  return (*(stream->stream_setpos))(stream, pos);
  }

result_t stream_length(stream_p hndl, uint32_t *length)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_length == 0)
    return e_not_implemented;

  return (*(stream->stream_length))(stream, length);
  }

result_t stream_truncate(stream_p hndl, uint32_t length)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_truncate == 0)
    return e_not_implemented;

  return (*(stream->stream_truncate))(stream, length);
  }

result_t stream_path(stream_p hndl, bool full_path, uint16_t len, char *path)
  {
  result_t result;
  if (failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if (stream->stream_path == 0)
    return e_not_implemented;

  return (*(stream->stream_path))(hndl, full_path, len, path);
  }

result_t stream_getc(stream_p hndl, char *ch)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_read == 0)
    return e_not_implemented;

  return (*(stream->stream_read))(stream, ch, 1, 0);
  }

result_t stream_putc(stream_p hndl, char ch)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_write == 0)
    return e_not_implemented;

  return (*(stream->stream_write))(stream, &ch, 1);
  }

result_t stream_puts(stream_p hndl, const char *str)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_write == 0)
    return e_not_implemented;

  uint16_t len = (uint16_t) strlen(str);
  return (*(stream->stream_write))(stream, str, len);
  }

result_t stream_gets(stream_p hndl, char *buffer, uint16_t len)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(buffer == 0 || len < 1)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  uint16_t ptr;
  for(ptr = 0; ptr < len-1; ptr++)
    {
    if(failed(result = stream_getc(hndl, buffer)))
      {
      *buffer = 0;
      return result;
      }

    if(*buffer == '\n')
      break;

    buffer++;
    }

  *buffer = 0;
  return s_ok;
  }
