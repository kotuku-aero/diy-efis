#ifndef __delta_h__
#define __delta_h__
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

/*  Implementation of a delta store.  The routines construct a delta record
 * that run-length encodes, based on a sample rate.  The sample rate is
 * not important but is assumed constant.
 * The series is stored in a form that allows for write-only encoding, and
 * a byte is written only once the data is full.
 */

#include "../neutron/neutron.h"

typedef enum {
  st_uint16,
  st_int16,
  st_uint32,
  st_int32,
  st_float
  } series_type;

typedef struct _series_t {
  uint16_t version;             // sizeof(series_t)  allows for updates
  char name[REG_NAME_MAX + 1];  // descriptive name (null terminated)
  uint8_t type;                 // type of data stored in the series (see series_type)
  uint32_t timestamp;           // when the series was started
  uint16_t interval;            // seconds between data stored
  } series_t;

/**
 * @function result_t create_deltastream(memid_t parent, const char *stream_name, const char *name, series_type type, uint32_t timestamp, uint16_t rate)
 * Create a delta stream and prepare it for writing.
 * @param parent        Registry key to create under
 * @param stream_name   Name of the stream
 * @param name          Descriptive name for the series
 * @param type          Type of the series
 * @param timestamp     Timestamp to start the series
 * @param rate          Rate at which samples are recorded
 * @param stream        Resulting stream
 * @return s_ok if the stream can be created
*/
extern result_t create_deltastream(memid_t parent,
                                   const char *stream_name,
                                   const char *name,
                                   series_type type,
                                   uint32_t timestamp,
                                   uint16_t rate,
                                   stream_p *stream);
/**
 * @function result_t open_deltastream(memid_t parent, const char *stream_name, stream_p *stream)
 * open a deltastream and seek to the start of the stream
 * @param parent        Registry key to open stream at
 * @param stream        Resulting stream
 * @return s_ok if the stream can be created
 * @param stream_name   Name of the stream
 */
extern result_t open_deltastream(memid_t parent, const char *stream_name, stream_p *stream);

extern result_t query_deltastream(stream_p stream, const series_t **result);

extern result_t seek_deltastream(stream_p stream, uint32_t timestamp);

#endif
