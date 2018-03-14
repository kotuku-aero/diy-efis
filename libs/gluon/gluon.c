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
#include "gluon.h"

#include "../neutron/stream.h"

/* When a query is run, all streams that could be within the start/end time are
 * collected and returned within a wrapper.  This makes a result stream
 * since the data can be sparse within the streams the wrapper has to return e_not_found
 * if a datapoint outside the stream bounds is found.
*/

typedef struct _deltastream_info_t {
  const series_t *series;
  stream_p stream;
  } deltastream_info_t;

typedef struct _query_column_t {
  stream_handle_t stream;             // callback functions.
  series_t series;                    // descriptor for the series
  uint32_t position;                  // current (next read) timestamp
  stream_p active_stream;             // stream that should hold the position
  uint16_t num_streams;               // how many streams returned in the search
  deltastream_info_t streams[];       // variable length streams.  These are delta streams
  } query_column_t;

static inline result_t as_column(stream_handle_t *hndl, query_column_t **column)
  {
  if (hndl->version != sizeof(query_column_t))
    return e_bad_handle;

  *column = (query_column_t *)hndl;

  return s_ok;
  }

static result_t columnstream_eof(stream_handle_t *hndl)
  {
  return e_unexpected;
  }

static result_t columnstream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  result_t result;
  query_column_t *column;

  if (failed(result = as_column(hndl, &column)))
    return result;

  if (column->active_stream == 0)
    return e_not_found;

  return stream_read(column->active_stream, buffer, size, read);
  }

static result_t columnstream_write(stream_handle_t *hndl, const void *buffer, uint16_t size)
  {
  return e_unexpected;
  }

static result_t columnstream_getpos(stream_handle_t *hndl, uint32_t *pos)
  {
  result_t result;
  query_column_t *column;

  if (failed(result = as_column(hndl, &column)))
    return result;

  *pos = column->position;

  return s_ok;
  }

static result_t columnstream_setpos(stream_handle_t *hndl, uint32_t pos)
  {
  result_t result;
  query_column_t *column;

  if (failed(result = as_column(hndl, &column)))
    return result;

  column->active_stream = 0;
  uint16_t series;
  for (series = 0; series < column->num_streams; series++)
    {
    if (column->streams[series].series->timestamp > pos)
      break;

    if (column->streams[series].series->timestamp <= pos)
      column->active_stream = column->streams[series].stream;
    }

  if(column->active_stream == 0)
    return e_not_found;

  column->position = pos;
  // and move what should hold the datapoint to the correct place.
  // could still report no data.
  return stream_setpos(column->active_stream, pos);
  }

static result_t columnstream_length(stream_handle_t *hndl, uint32_t *length)
  {
  return e_unexpected;
  }

static result_t columnstream_truncate(stream_handle_t *hndl, uint32_t length)
  {
  return e_unexpected;
  }

static result_t columnstream_close(stream_handle_t *hndl)
  {

  return s_ok;
  }

static result_t columnstream_delete(stream_handle_t *hndl)
  {
  return e_unexpected;
  }

static result_t columnstream_path(stream_handle_t *hndl, bool full_path, uint16_t len, char *path)
  {
  return e_unexpected;
  }


static void gluon_worker(void *parg)
  {
  }

result_t gluon_init()
  {
  return e_not_implemented;
  }

uint32_t gluon_now()
  {
  return e_not_implemented;
  }

result_t gluon_parse(const char *dt, uint32_t *value)
  {
  return e_not_implemented;
  }

result_t gluon_gmtime(const gluon_tm_t *tm, uint32_t *time)
  {
  return e_not_implemented;
  }

result_t gluon_mktime(uint32_t time, gluon_tm_t *tm)
  {
  return e_not_implemented;
  }

result_t gluon_select(const char *series, uint32_t from, uint32_t to, vector_p columns, vector_p *results)
  {
  return e_not_implemented;
  }

result_t gluon_enumerate_series(vector_p *result)
  {
  return e_not_implemented;
  }

result_t gluon_get_series_descr(memid_t handle, gluon_series_t *descr)
  {
  return e_not_implemented;
  }

result_t gluon_create_series(const char *series, uint16_t rate, memid_t *handle)
  {
  return e_not_implemented;
  }

result_t gluon_open_series(const char *series, memid_t handle)
  {
  return e_not_implemented;
  }

result_t gluon_enumerate_values(memid_t series, vector_p *results)
  {
  return e_not_implemented;
  }

result_t gluon_add_value(memid_t series, uint16_t can_id, const char *name)
  {
  return e_not_implemented;
  }

result_t gluon_remove_value(memid_t series, uint16_t can_id)
  {
  return e_not_implemented;
  }

result_t gluon_add_triggers(memid_t handle, uint16_t len, trigger_def_t *defnitions)
  {
  return e_not_implemented;
  }

result_t gluon_list_triggers(memid_t handle, vector_p *triggers)
  {
  return e_not_implemented;
  }

result_t gluon_remove_trigger(memid_t handle, memid_t trigger)
  {
  return e_not_implemented;
  }

result_t gluon_get_descr(stream_p series, gluon_series_t *result)
  {
  return e_not_implemented;
  }