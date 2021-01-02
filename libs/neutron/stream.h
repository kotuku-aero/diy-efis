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
typedef result_t (*stream_getpos_fn)(struct _stream_handle_t *stream, uint32_t *pos);
typedef result_t (*stream_setpos_fn)(struct _stream_handle_t *stream, uint32_t pos);
typedef result_t (*stream_length_fn)(struct _stream_handle_t *stream, uint32_t *length);
typedef result_t (*stream_truncate_fn)(struct _stream_handle_t *stream, uint32_t length);
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

