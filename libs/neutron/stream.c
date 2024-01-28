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
#include "registry.h"
#include <string.h>

const typeid_t stream_type = { "stream_t" };

result_t stream_eof(handle_t hndl)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->eof == 0)
    return e_not_implemented;

  return (*(stream->eof))(hndl);
  }


result_t stream_read(handle_t hndl, void *buffer, uint32_t size, uint32_t *read)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->read == 0)
    return e_not_implemented;

  return (*(stream->read))(hndl, buffer, size, read);
  }

result_t stream_write(handle_t hndl, const void *buffer, uint32_t size)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->write == 0)
    return e_not_implemented;

  return (*(stream->write))(hndl, buffer, size);
  }

#ifdef min
#undef min
#endif

static uint32_t min(uint32_t a, uint32_t b)
  {
  return a < b ? a : b;
  }

result_t stream_copy(handle_t from, handle_t to)
  {
  result_t result = s_ok;

  uint32_t read = 0;
  uint8_t *buffer;
  if (failed(result = neutron_malloc(128, (void **)&buffer)))
    return result;

  uint32_t len;
  if (failed(stream_length(from, &len)))
    return result;

  uint32_t chunk = min(128, (uint32_t) len);

  while (chunk > 0 &&
    succeeded(result = stream_read(from, buffer, chunk, &read)) &&
    read > 0 &&
    succeeded(result = stream_write(to, buffer, read)))
    {
    len -= chunk;
    chunk = min(128, (uint32_t)len);
    }

  neutron_free(buffer);
  return result;
  }

result_t stream_getpos(handle_t hndl, uint32_t *pos)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->getpos == 0)
    return e_not_implemented;

  return (*(stream->getpos))(hndl, pos);
  }

result_t stream_setpos(handle_t hndl, int32_t pos, uint32_t whence)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->setpos == 0)
    return e_not_implemented;

  return (*(stream->setpos))(hndl, pos, whence);
  }

result_t stream_length(handle_t hndl, uint32_t *length)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->length == 0)
    return e_not_implemented;

  return (*(stream->length))(hndl, length);
  }

result_t stream_truncate(handle_t hndl, uint32_t length)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->truncate == 0)
    return e_not_implemented;

  return (*(stream->truncate))(hndl, length);
  }

result_t stream_getc(handle_t hndl, char *ch)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->read == 0)
    return e_not_implemented;

  return (*(stream->read))(hndl, ch, 1, 0);
  }

result_t stream_putc(handle_t hndl, char ch)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->write == 0)
    return e_not_implemented;

  return (*(stream->write))(hndl, &ch, 1);
  }

result_t stream_puts(handle_t hndl, const char *str)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(stream->write == 0)
    return e_not_implemented;

  uint16_t len = (uint16_t) strlen(str);
  return (*(stream->write))(hndl, str, len);
  }

result_t stream_gets(handle_t hndl, char *buffer, uint16_t len)
  {
  result_t result;
  stream_t *stream;
  if (failed(result = is_typeof(hndl, &stream_type, (void **)&stream)))
    return result;

  if(buffer == 0 || len < 1)
    return e_bad_parameter;

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
