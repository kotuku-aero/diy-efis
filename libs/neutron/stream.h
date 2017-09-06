#ifndef STREAM_H
#define	STREAM_H

#ifdef	__cplusplus
extern "C"
  {
#endif
  
#include "neutron.h"
  
struct _stream_handle_t;

typedef result_t (*stream_eof_fn)(struct _stream_handle_t *stream);
typedef result_t (*stream_read_fn)(struct _stream_handle_t *stream, void *buffer, uint16_t size, uint16_t *read);
typedef result_t (*stream_write_fn)(struct _stream_handle_t *stream, const void *buffer, uint16_t size);
typedef result_t (*stream_getpos_fn)(struct _stream_handle_t *stream, uint16_t *pos);
typedef result_t (*stream_setpos_fn)(struct _stream_handle_t *stream, uint16_t pos);
typedef result_t (*stream_length_fn)(struct _stream_handle_t *stream, uint16_t *length);
typedef result_t (*stream_truncate_fn)(struct _stream_handle_t *stream, uint16_t length);
typedef result_t (*stream_close_fn)(struct _stream_handle_t *stream);
typedef result_t (*stream_delete_fn)(struct _stream_handle_t *stream);
typedef result_t (*stream_path_fn)(struct _stream_handle_t *stream, bool full_path, uint16_t len, char *path);

typedef struct _stream_handle_t {
  uint16_t version;
  stream_eof_fn stream_eof;
  stream_read_fn stream_read;
  stream_write_fn stream_write;
  stream_getpos_fn stream_getpos;
  stream_setpos_fn stream_setpos;
  stream_length_fn stream_length;
  stream_truncate_fn stream_truncate;
  stream_close_fn stream_close;
  stream_delete_fn stream_delete;
  stream_path_fn stream_path;
  } stream_handle_t;

#ifdef	__cplusplus
  }
#endif

#endif	/* STREAM_H */

