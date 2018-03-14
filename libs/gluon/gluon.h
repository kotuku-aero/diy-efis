#ifndef __gluon_h__
#define __gluon_h__
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

#include "../neutron/neutron.h"
#include "delta.h"

extern result_t gluon_init();

/* All timestamps in Gluon are the number of seconds since 2001/01/01 */
/**
 * @function uint32_t gluon_now()
 * return the current time.
 * @return current timestamp.  returns 2001/01/01 00:00:00 if the UTC
 * date/time is unknown.
*/
extern uint32_t gluon_now();

typedef struct _gluon_tm_t {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  } gluon_tm_t;

/**
 * @function uint32_t gluon_parse(const char *dt)
 * Parse a string and return the timestamp
 * @param dt    Date-Time string.  Format is yyyy-mm--dd hh:mm:ss
 * @param value Gluon timestamp, or 0 if not valid
 * @return s_ok if a valid time string
*/
extern result_t gluon_parse(const char *dt, uint32_t *time);
extern result_t gluon_gmtime(const gluon_tm_t *tm, uint32_t *time);
extern result_t gluon_mktime(uint32_t time, gluon_tm_t *tm);

typedef struct _gluon_value_t {
  uint16_t can_id;
  char name[REG_NAME_MAX];
  uint16_t reserved;            // always 0 but packs structure and makes string term
  series_type type;
  } gluon_value_t;

/**
 * @function result_t gluon_select(const char *series, uint32_t from, uint32_t to, vector_p *results)
 * Return all of the time-series that are contained within the from and to times for the given series
 * @param series  Name of the series
 * @param from    timestamp from, set to 0 for all available is returned with first sample available
 * @param to      timestamp to, set to UIN32_MAX for all available, is returned with last sample available
 * @param columns what columns to return
 * @param results Vector of stream_p that are the series.  Must be closed with stream_close
 * @return s_ok if the series has values that overlap
*/
extern result_t gluon_select(const char *series, uint32_t *from, uint32_t *to, vector_p columns, vector_p *results);

typedef struct _gluon_series_t {
  memid_t memid;
  char name[REG_NAME_MAX];
  uint16_t rate;
  } gluon_series_t;

extern result_t gluon_enumerate_series(vector_p *result);
extern result_t gluon_get_series_descr(memid_t handle, gluon_series_t *descr);

extern result_t gluon_create_series(const char *series, uint16_t rate, memid_t *handle);
extern result_t gluon_open_series(const char *series, memid_t handle);

extern result_t gluon_enumerate_values(memid_t series, vector_p *results);

extern result_t gluon_add_value(memid_t series, uint16_t can_id, const char *name);
extern result_t gluon_remove_value(memid_t series, uint16_t can_id);
extern result_t gluon_get_descr(stream_p series, gluon_series_t *result);

typedef enum {
  tt_eq,
  tt_le,
  tt_gt,
  tt_eqf,
  tt_lef,
  tt_gtf
  } trigger_type;

typedef struct _trigger_def_t {
  memid_t memid;
  trigger_type type;
  uint16_t can_id;
  union {
    int16_t int16_val;
    float float_val;
    } value;
  } trigger_def_t;

extern result_t gluon_add_triggers(memid_t handle, uint16_t len, trigger_def_t *defnitions);
extern result_t gluon_list_triggers(memid_t handle, vector_p *triggers);
extern result_t gluon_remove_trigger(memid_t handle, memid_t trigger);

#endif
