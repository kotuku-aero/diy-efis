#include "stream.h"
#include "registry.h"
#include <string.h>

static result_t check_handle(handle_t hndl)
  {
  if(hndl == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *) hndl;
  if (stream->version < sizeof (stream_handle_t))
    return e_bad_parameter;

  return s_ok;
  }

result_t stream_close(handle_t hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_close == 0)
    return e_not_implemented;

  return (*stream->stream_close)(stream);
  }

result_t stream_delete(handle_t hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_delete == 0)
    return e_not_implemented;

  return (*stream->stream_delete)(stream);
  }

result_t stream_eof(handle_t hndl)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_eof == 0)
    return e_not_implemented;

  return (*stream->stream_eof)(stream);
  }

result_t stream_read(handle_t hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(buffer == 0 || size == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_read == 0)
    return e_not_implemented;

  return (*stream->stream_read)(stream, buffer, size, read);
  }

result_t stream_write(handle_t hndl, const void *buffer, uint16_t size)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(buffer == 0 || size == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_write == 0)
    return e_not_implemented;

  return (*stream->stream_write)(stream, buffer, size);
  }

result_t stream_copy(handle_t from, handle_t to)
  {
  result_t result;
  if(failed(result = check_handle(from)) ||
     failed(result = check_handle(to)))
    return result;

  uint16_t read = 0;
  byte_t *buffer = (byte_t *)kmalloc(128);
  if(buffer == 0)
    return e_not_enough_memory;

  uint16_t len;
  if (failed(stream_length(from, &len)))
    return result;

  uint16_t chunk = min(128, len);

  while (chunk > 0 &&
    succeeded(result = stream_read(from, buffer, chunk, &read)) &&
    read > 0 &&
    succeeded(result = stream_write(to, buffer, read)))
    {
    len -= chunk;
    chunk = min(128, len);
    }

  kfree(buffer);
  return result;
  }

result_t stream_getpos(handle_t hndl, uint16_t *pos)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  if(pos == 0)
    return e_bad_parameter;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_getpos == 0)
    return e_not_implemented;

  return (*stream->stream_getpos)(stream, pos);
  }

result_t stream_setpos(handle_t hndl, uint16_t pos)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_setpos == 0)
    return e_not_implemented;

  return (*stream->stream_setpos)(stream, pos);
  }

result_t stream_length(handle_t hndl, uint16_t *length)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_length == 0)
    return e_not_implemented;

  return (*stream->stream_length)(stream, length);
  }

result_t stream_truncate(handle_t hndl, uint16_t length)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_truncate == 0)
    return e_not_implemented;

  return (*stream->stream_truncate)(stream, length);
  }

result_t stream_path(handle_t hndl, bool full_path, uint16_t len, char *path)
  {
  result_t result;
  if (failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if (stream->stream_path == 0)
    return e_not_implemented;

  return (*stream->stream_path)(hndl, full_path, len, path);
  }

result_t stream_getc(handle_t hndl, char *ch)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_read == 0)
    return e_not_implemented;

  return (*stream->stream_read)(stream, ch, 1, 0);
  }

result_t stream_putc(handle_t hndl, char ch)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_write == 0)
    return e_not_implemented;

  return (*stream->stream_write)(stream, &ch, 1);
  }

result_t stream_puts(handle_t hndl, const char *str)
  {
  result_t result;
  if(failed(result = check_handle(hndl)))
    return result;

  stream_handle_t *stream = (stream_handle_t *)hndl;

  if(stream->stream_write == 0)
    return e_not_implemented;

  uint16_t len = (uint16_t) strlen(str);
  return (*stream->stream_write)(stream, str, len);
  }

result_t stream_gets(handle_t hndl, char *buffer, uint16_t len)
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
