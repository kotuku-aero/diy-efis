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
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "neutron.h"
#include "bsp.h"

typedef enum _alarm_type_e
  {
  level_alarm,
  event_alarm
  } alarm_type_e;

typedef struct _alarm_t {
  // id of alarm to publish
  uint16_t alarm_id;
  // true if check minimum
  bool check_minimum;
  // minimum value for value, less than will cause alarm
  float min_value;
  // true if check maximum
  bool check_maximum;
  // maximum value for value, greater than will cause alarm
  float max_value;
  // alarm type
  // 0=level alarm, raised while level remains outside the limits
  // 1=event alarm, raised when the level goes outside limites
  alarm_type_e type;
  // period in MS that alarm must be outside of to be raised
  uint16_t period;
  // can-id that resets a event alarm
  uint16_t reset_id;
  // tick when alarm noted, if == ticks then no alarm
  uint32_t event_time;
  } alarm_t;

typedef enum _capture_type_e
  {
  ct_fifo,      // use first sample after a publish
  ct_lifo,      // use last sample before a publish
  ct_min,       // use min of samples
  ct_max,       // use max of samples
  ct_avg        // use average of samples
  } capture_type_e;
 
typedef struct _pre_filter_t {
  ////////////////////////////////////////////////////////////////
  // Pre-filter (capture accumulator)
  capture_type_e capture_type;    // what type of capture to do
                                  // can only be set if the value_type is
                                  // CANAS_DATATYPE_FLOAT otherwise ignored
  uint32_t num_samples;           // number of samples between captures
  float accum;              // accumulated value for the capture filter
  } pre_filter_t;

// this group of functions defines an area to publish regular status updates
// the structure will be filled in with the current values
typedef struct _published_datapoint_t
  {
  uint16_t version;               // should be size of actual type
  uint16_t can_id;                // id to publish
  uint16_t publish_rate;          // in number of ticks
  uint32_t next_publish_tick;     // will be set by scheduler, but can also be used

  union {
    uint16_t publish : 1;         // set to publish
    uint16_t loopback : 1;        // set to loopback to sender
    } flags;

  canmsg_t value;                 // value that is to been published
  
  ////////////////////////////////////////////////////////////////
  //
  // Optional alarms follow the optional filter
  uint16_t num_alarms;
 
  ////////////////////////////////////////////////////////////////
  // the filter type other than none is only really relevant to
  // floating point types
  filter_type_e filter_type;
  } published_datapoint_t;
    
typedef struct _boxcar_filter_datapoint_t
  {
  published_datapoint_t datapoint;
  pre_filter_t pre_filter;
  uint16_t frequency;           // how many 1msec steps per filter
  uint32_t last_tick;           // when last run
  uint16_t filter_length;
  uint16_t head;                // oldest datapoint
  uint16_t tail;                // newest datapoint
  float filter[];         // length is filter_length
  } boxcar_datapoint_t;

typedef struct _filter_datapoint_t
  {
  published_datapoint_t datapoint;
  pre_filter_t pre_filter;
  // we support MAC using hardware (bsp_mac) which is
  // assigned by the system.  On linux/windows is done in sofware
  // hardware can do it magically
  mac_fn function;
  uint16_t filter_length;
  uint16_t frequency;           // number of 1msec intervals
  uint32_t last_tick;           // when the mac last calculated
  float gain;
  filter_params_t filter[];
  } filter_datapoint_t;
    
static inline uint16_t sizeof_boxcar(uint16_t filter_length)
  {
  return sizeof(boxcar_datapoint_t) + 
    (sizeof(float) * filter_length);
  }

static inline uint16_t sizeof_fir(uint16_t filter_length)
  {
  return sizeof(boxcar_datapoint_t) + 
    (sizeof(filter_params_t) * filter_length);
  }

static inline boxcar_datapoint_t *as_boxcar(published_datapoint_t *dp)
  {
  return (boxcar_datapoint_t *)dp;
  }

static inline filter_datapoint_t *as_filter(published_datapoint_t *dp)
  {
  return (filter_datapoint_t *)dp;
  }

static inline uint16_t sizeof_datapoint(filter_type_e filter_type, uint16_t filter_length, uint16_t num_alarms)
  {
  uint16_t datapoint_size = 0;
  switch(filter_type)
    {
    case ft_none :
      datapoint_size = sizeof(published_datapoint_t);
      break;
    case ft_boxcar :
      datapoint_size = sizeof_boxcar(filter_length);
      break;
    case ft_iir :
    case ft_fir :
      datapoint_size = sizeof_fir(filter_length);
      break;
    }
  
  datapoint_size += sizeof(alarm_t) * num_alarms;
  
  return datapoint_size;
  }
  
static inline alarm_t *get_alarm(published_datapoint_t *dp, uint16_t index)
  {
  uint16_t alarm_base = 0;
  switch(dp->filter_type)
    {
    case ft_none :
      alarm_base = sizeof(published_datapoint_t);
      break;
    case ft_boxcar :
      alarm_base = sizeof_boxcar(((boxcar_datapoint_t *)dp)->filter_length);
      break;
    case ft_iir :
    case ft_fir :
      alarm_base = sizeof_fir(((filter_datapoint_t *)dp)->filter_length);
      break;
    }
  
  uint8_t *base = (uint8_t *)dp;
  base += alarm_base;
  base += sizeof(alarm_t) * index;
  
  return (alarm_t *)base;
  }

static inline published_datapoint_t *alloc_datapoint(filter_type_e filter_type,
                                                     uint16_t filter_length,
                                                     uint16_t num_alarms)
  {
  uint16_t alloc_size = sizeof_datapoint(filter_type, filter_length, num_alarms);
  published_datapoint_t *dp = 
    (published_datapoint_t *)neutron_malloc(alloc_size);
  
  if(dp == 0)
    return 0;
  
  memset(dp, 0, alloc_size);
  dp->version = alloc_size;
  dp->filter_type = filter_type;
  dp->num_alarms = num_alarms;
  
  switch(filter_type)
    {
    case ft_boxcar :
      as_boxcar(dp)->filter_length = filter_length;
      break;
    case ft_iir :
    case ft_fir :
      as_filter(dp)->filter_length = filter_length;
      break;
    default :
      break;
    }
  
  return dp;
  }

typedef struct _enum_descr {
    uint16_t value;
    const char *description;
} enum_descr ;

static vector_p published_datapoints;
static semaphore_p can_publisher_semp;

static const char *s_publisher_key = "neutron";
static const char *s_rate_value = "rate";
static const char *s_sample_type_value = "sample";
static const char *s_filter_type_value = "filter";
static const char *s_filter_length_value = "length";
static const char *s_coefficient_value = "coeff%d";   // will have 1..16 appended
static const char *s_filter_gain_value = "gain";
static const char *s_loopback_value = "loopback";
static const char *s_publish_value = "publish";
static const char *s_alarm_min_value = "min";
static const char *s_alarm_max_value = "max";
static const char *s_alarm_type_value = "type";
static const char *s_alarm_period_value = "period";
static const char *s_alarm_reset_value = "reset";

static canmsg_t register_msg;

/**
 * Find the index of a datapoint in the published datapoints
 * @param id
 * @return 
 */
static result_t find_datapoint(uint16_t id, published_datapoint_t **dp, uint16_t *index)
  {
  uint16_t i;
  uint16_t len;
  result_t result;

  if(failed(result = vector_count(published_datapoints, &len)))
    return result;
  
  if(index == 0)
    index = &i;

  for(*index = 0; *index < len; *index = *index +1)
    {
    if(failed(result = vector_at(published_datapoints, *index, dp)))
      return result;
    
    if((*dp)->can_id == id)
      return s_ok;
    }
  
  return e_not_found;
  }

extern void publish_local(const canmsg_t *msg);

static inline pre_filter_t *as_pre_filter(published_datapoint_t *datapoint)
  {
  // TODO: alignment???
  return (pre_filter_t *)(((uint8_t *)datapoint)+sizeof(published_datapoint_t));
  }

result_t publish_float(uint16_t id, float value)
  {
  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  if(datapoint->filter_type == ft_none)
    {
    create_can_msg_float(&datapoint->value, datapoint->can_id, 0, value);
    }
  else
    {
    pre_filter_t *pre_filter = as_pre_filter(datapoint);

    // decide what pre-filter algorithm is applied to the sample.
    switch(pre_filter->capture_type)
      {
    case ct_fifo :
      if(pre_filter->num_samples != 0)
        break;
    case ct_lifo :
      pre_filter->accum = value;
      break;
    case ct_min :
      if(value < pre_filter->accum)
        pre_filter->accum = value;
      break;
    case ct_max :
      if(value > pre_filter->accum)
        pre_filter->accum = value;
      break;
    case ct_avg :
      pre_filter->accum += value;
      break;
      }

    // this is used when an average value is needed
    pre_filter->num_samples ++;
    }
  return s_ok;
  }

result_t lookup_float(uint16_t id, float *value)
  {
  if (value == 0)
    return e_bad_parameter;

  result_t result;
   published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  return get_param_float(&datapoint->value, value);
  }

static const uint8_t int8_dt[] =
  {
  CANAS_DATATYPE_CHAR,
  CANAS_DATATYPE_CHAR2,
  CANAS_DATATYPE_CHAR3,
  CANAS_DATATYPE_CHAR4
  };

result_t publish_int8(uint16_t id, const int8_t *value, uint16_t len)
  {
  if (value == 0 || len == 0 || len > 4)
    return e_bad_parameter;

  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  datapoint->value.canas.data_type = int8_dt[len-1];
  datapoint->value.id = datapoint->can_id;
  for(index = 0; index < len; index++)
    datapoint->value.canas.data[index] = (uint8_t) *value++;

  return s_ok;
  }

result_t lookup_int8(uint16_t id, int8_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len == 0 || *len > 4)
    return e_bad_parameter;

  result_t result;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  switch (datapoint->value.canas.data_type)
    {
    case CANAS_DATATYPE_CHAR :
      *value = datapoint->value.canas.data[0];
      break;
    case CANAS_DATATYPE_CHAR2 :
      if (*len < 2)
        return e_bad_parameter;
      value[0] = datapoint->value.canas.data[0];
      value[1] = datapoint->value.canas.data[1];
      break;
    case CANAS_DATATYPE_CHAR3 :
      if (*len < 3)
        return e_bad_parameter;
      value[0] = datapoint->value.canas.data[0];
      value[1] = datapoint->value.canas.data[1];
      value[2] = datapoint->value.canas.data[2];
      break;
    case CANAS_DATATYPE_CHAR4 :
      if (*len < 4)
        return e_bad_parameter;
      value[0] = datapoint->value.canas.data[0];
      value[1] = datapoint->value.canas.data[1];
      value[2] = datapoint->value.canas.data[2];
      value[3] = datapoint->value.canas.data[3];
      break;
    default :
      return e_bad_parameter;
    }
  return s_ok;
  }

static const uint8_t uint8_dt[] =
  {
  CANAS_DATATYPE_UCHAR,
  CANAS_DATATYPE_UCHAR2,
  CANAS_DATATYPE_UCHAR3,
  CANAS_DATATYPE_UCHAR4
  };

result_t publish_uint8(uint16_t id, const uint8_t *value, uint16_t len)
  {
  if (value == 0 || len == 0 || len > 4)
    return e_bad_parameter;

  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  datapoint->value.canas.data_type = uint8_dt[len-1];
  datapoint->value.id = datapoint->can_id;
  for(index = 0; index < len; index++)
    datapoint->value.canas.data[index] = *value++;

  return s_ok;
  }

result_t lookup_uint8(uint16_t id, uint8_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len == 0 || *len > 4)
    return e_bad_parameter;

  result_t result;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  switch (datapoint->value.canas.data_type)
    {
    case CANAS_DATATYPE_UCHAR:
      *value = datapoint->value.canas.data[0];
      break;
    case CANAS_DATATYPE_UCHAR2:
      if (*len < 2)
        return e_bad_parameter;
      value[0] = datapoint->value.canas.data[0];
      value[1] = datapoint->value.canas.data[1];
      break;
    case CANAS_DATATYPE_UCHAR3:
      if (*len < 3)
        return e_bad_parameter;
      value[0] = datapoint->value.canas.data[0];
      value[1] = datapoint->value.canas.data[1];
      value[2] = datapoint->value.canas.data[2];
      break;
    case CANAS_DATATYPE_UCHAR4:
      value[0] = datapoint->value.canas.data[0];
      value[1] = datapoint->value.canas.data[1];
      value[2] = datapoint->value.canas.data[2];
      value[3] = datapoint->value.canas.data[3];
      break;
    default:
      return e_bad_parameter;
    }
  return s_ok;
  }

static const uint8_t int16_dt[] =
  {
  CANAS_DATATYPE_SHORT,
  CANAS_DATATYPE_SHORT2
  };

result_t publish_int16(uint16_t id, const int16_t *value, uint16_t len)
  {
  if (value == 0 || len == 0 || len > 4)
    return e_bad_parameter;

  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  datapoint->value.canas.data_type = int16_dt[len-1];
  datapoint->value.id = datapoint->can_id;
  for(index = 0; index < len; index+=2)
    {
    datapoint->value.canas.data[index] = (*value >> 8);
    datapoint->value.canas.data[index+1] = *value >> 8;
    value++;
    }

  return s_ok;
  }

result_t lookup_int16(uint16_t id, int16_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len == 0 || *len > 2)
    return e_bad_parameter;

  result_t result;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  switch (datapoint->value.canas.data_type)
    {
    case CANAS_DATATYPE_SHORT:
      *value = 
        (datapoint->value.canas.data[0] << 8) |
        datapoint->value.canas.data[1];
      break;
    case CANAS_DATATYPE_SHORT2:
      if (*len < 2)
        return e_bad_parameter;
      value[0] = 
        (datapoint->value.canas.data[0] << 8) |
        datapoint->value.canas.data[1];
      value[1] = 
        (datapoint->value.canas.data[2] << 8) |
        datapoint->value.canas.data[3];
      break;
    default:
      return e_bad_parameter;
    }
  return s_ok;
  }

static const uint8_t uint16_dt[] =
  {
  CANAS_DATATYPE_USHORT,
  CANAS_DATATYPE_USHORT2
  };

result_t publish_uint16(uint16_t id, const uint16_t *value, uint16_t len)
  {
  if (value == 0 || len == 0 || len > 4)
    return e_bad_parameter;

  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  datapoint->value.canas.data_type = uint16_dt[len-1];
  datapoint->value.id = datapoint->can_id;
  for(index = 0; index < len; index+=2)
    {
    datapoint->value.canas.data[index] = (*value >> 8);
    datapoint->value.canas.data[index+1] = *value >> 8;
    value++;
    }

  return s_ok;
  }

result_t lookup_uint16(uint16_t id, uint16_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len == 0 || *len > 2)
    return e_bad_parameter;

  result_t result;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  switch (datapoint->value.canas.data_type)
    {
    case CANAS_DATATYPE_USHORT:
      *value = 
        (datapoint->value.canas.data[0] << 8) |
        datapoint->value.canas.data[1];
      break;
    case CANAS_DATATYPE_USHORT2:
      if (*len < 2)
        return e_bad_parameter;
      value[0] = 
        (datapoint->value.canas.data[0] << 8) |
        datapoint->value.canas.data[1];
      value[1] = 
        (datapoint->value.canas.data[2] << 8) |
        datapoint->value.canas.data[3];
      break;
    default:
      return e_bad_parameter;
    }
  return s_ok;
  }

result_t publish_int32(uint16_t id, int32_t value)
  {
  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  create_can_msg_int32(&datapoint->value, datapoint->can_id, 0, value);
  
  return s_ok;
  }

result_t lookup_int32(uint16_t id, int32_t *value)
  {
  if (value == 0)
    return e_bad_parameter;

  result_t result;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  *value = (datapoint->value.canas.data[0] << 24) |
    (datapoint->value.canas.data[1] << 16) |
    (datapoint->value.canas.data[2] << 8) |
    datapoint->value.canas.data[3];
  
  return s_ok;
  }

result_t publish_uint32(uint16_t id, uint32_t value)
  {
  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  create_can_msg_uint32(&datapoint->value, datapoint->can_id, 0, value);

  return s_ok;
  }

result_t lookup_uint32(uint16_t id, uint32_t *value)
  {
  if (value == 0)
    return e_bad_parameter;

  result_t result;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, 0)))
    return result;

  *value = (datapoint->value.canas.data[0] << 24) |
    (datapoint->value.canas.data[1] << 16) |
    (datapoint->value.canas.data[2] << 8) |
    datapoint->value.canas.data[3];
  
  return s_ok;
  }

static result_t parse_boxcar(boxcar_datapoint_t *dp, memid_t key)
  {
  // a boxcar has a length and not much else.  Is
  // an average of a sliding window.  Basically a low pass
  // filter without gain compensation
 // type of sample accumulation
  uint16_t v_uint16;
  if(succeeded(reg_get_uint16(key, s_sample_type_value, &v_uint16)))
    dp->pre_filter.capture_type = (capture_type_e)v_uint16;
  
  return s_ok;
  }

static result_t parse_filter(filter_datapoint_t *dp, memid_t key)
  {
  uint16_t v_uint16;
  if(succeeded(reg_get_uint16(key, s_sample_type_value, &v_uint16)))
    dp->pre_filter.capture_type = (capture_type_e)v_uint16;
  
  char name[REG_NAME_MAX + 1];
  // work over each coefficient.
  uint16_t index;
  for(index = 1; index <= dp->filter_length; index++)
    {
    snprintf(name, REG_NAME_MAX, s_coefficient_value, index);
    float coeff;
    if(failed(reg_get_float(key, name, &coeff)))
      coeff = 0;

    // assign the filter to the fir/iir
    dp->filter[index].coefficient = coeff;
    }

  float gain;
  // pick up the gain of the filter
  if(failed(reg_get_float(key, s_filter_gain_value, &gain)))
    gain = 1.0;
  
  dp->gain = gain;
  
  dp->last_tick = ticks();
  
  // assign the mac processor.
  // this is to allow some sort of fir/iir optimization
  dp->function = bsp_get_mac(key);
  
  return s_ok;
  }

/**
 * Load a datapoint from the registry
 * @param datapoints  Array to add datapoint to
 * @param key         key to load from
 * @return s_ok if loaded ok
 */
static result_t create_datapoint(vector_p datapoints, uint16_t can_id, memid_t key)
  {
  result_t result;
  uint16_t v_uint16;
  published_datapoint_t *dp;
  char name[REG_NAME_MAX + 1];
  alarm_t alarm;
  
  // get the filter type and number of alarms first
  filter_type_e filter_type = ft_none;          // no filter
  uint16_t value_type;
  uint16_t filter_length = 0;

  // type of filter.  The filter runs at the publish frequency
  if(succeeded(reg_get_uint16(key, s_filter_type_value, &v_uint16)))
    filter_type = (filter_type_e) v_uint16;

  if(filter_type != ft_none)
    {
    // must have a length!
    if(succeeded(reg_get_uint16(key, s_filter_length_value, &v_uint16)))
      {
      // always clip this, as long filters are NEVER supported
      if(v_uint16 > 16)
        v_uint16 = 16;

      filter_length = v_uint16;
      }
    else
      filter_type = ft_none;
    }
  
  // count how many alarms there are.
  uint16_t num_alarms = 0;
  memid_t alarm_key = 0;

  uint16_t i;
  field_datatype key_type = field_key;

  // enumerate the child keys, these are alarms
  while(succeeded(reg_enum_key(key, &key_type, 0, 0, REG_NAME_MAX + 1, name, &alarm_key)))
    num_alarms++;
  
  dp = alloc_datapoint(filter_type, filter_length, num_alarms);

  if(dp == 0)
    return e_not_enough_memory;
  
  dp->can_id = can_id;
  
  // the can_id, type and length are all set so we now work over the
  // settings for the datapoint
  if(succeeded(reg_get_uint16(key, s_rate_value, &v_uint16)))
    {
    dp->publish_rate = v_uint16;
    dp->flags.publish = 1;
    }
  else
    dp->flags.publish = 0;

  switch(dp->filter_type)
    {
    case ft_none :
      break;
    case ft_boxcar :
      parse_boxcar(as_boxcar(dp), key);
      break;
    case ft_fir :
    case ft_iir :
      parse_filter(as_filter(dp), key);
      break;
    }

  bool v_bool;
  if(succeeded(reg_get_bool(key, s_loopback_value, &v_bool)))
    dp->flags.loopback = 1;

  if(succeeded(reg_get_bool(key, s_publish_value, &v_bool)))
    dp->flags.publish = v_bool;
  else
    dp->flags.publish = true;

  alarm_key = 0;

  uint16_t alarm_num = 0;
  // enumerate the child keys, these are alarms
  do
    {
    key_type = field_key;

    if(failed(result = reg_enum_key(key, &key_type, 0, 0, REG_NAME_MAX + 1, name, &alarm_key)))
      break;

    bool is_valid = true;
    uint16_t can_id = 0;
    // ensure the name is a can-id
    for(i = 0; i < REG_NAME_MAX; i++)
      {
      if(!isdigit(name[i]))
        {
        is_valid = false;
        break;
        }

      can_id *= 10;
      can_id += name[i] - '0';
      }

    float v_float;

    if(is_valid)
      {
      alarm_t *alarm = get_alarm(dp, alarm_num);

      alarm->alarm_id = can_id;

      if(succeeded(reg_get_float(alarm_key, s_alarm_min_value, &v_float)))
        alarm->min_value = v_float;

      if(succeeded(reg_get_float(alarm_key, s_alarm_max_value, &v_float)))
        alarm->max_value = v_float;

      if(succeeded(reg_get_uint16(alarm_key, s_alarm_type_value, &v_uint16)))
        alarm->type = (alarm_type_e)v_uint16;

      if(succeeded(reg_get_uint16(alarm_key, s_alarm_period_value, &v_uint16)))
        alarm->period = v_uint16;

      if(succeeded(reg_get_uint16(alarm_key, s_alarm_reset_value, &v_uint16)))
        alarm->reset_id = v_uint16;
      }
    
    alarm_num++;
    } while(true);

  // add the published parameter
  vector_push_back(datapoints, &dp);

  return s_ok;
  }

void publish_task(void *parg)
  {
  int nis_timeout = 100;

  semaphore_create(&can_publisher_semp);
  
  uint32_t last_tick = ticks();
  uint32_t dispatch_tick;
  uint32_t ticks_changed;
  
  while(true)
    {
    semaphore_wait(can_publisher_semp, 1);
    
    dispatch_tick = ticks();
    
    ticks_changed = dispatch_tick - last_tick;
    last_tick = dispatch_tick;
    
    if(ticks_changed == 0)
      continue;
    
    nis_timeout -= ticks_changed;
    
    if(register_msg.canas.node_id != 0 && nis_timeout <= 0)
      {
#ifndef _DEBUG
      can_send(&register_msg);
#endif
      nis_timeout = 100;
      }

    uint16_t num_datapoints;

    if(failed(vector_count(published_datapoints, &num_datapoints)))
      continue;

    published_datapoint_t *dp;
    uint16_t  i;

    float index_value;

    // do each datapoint we have registered
    for(i = 0; i < num_datapoints; i++)
      {
      vector_at(published_datapoints, i, &dp);
      
      bool publish = dp->flags.publish &&
         dp->next_publish_tick <= ticks_changed;

      if(publish)
        {
        dp->next_publish_tick = dp->publish_rate;
        
        if(dp->filter_type != ft_none)
          {
          pre_filter_t *pre_filter = as_pre_filter(dp);
        
          // see if the accumulator is an average,
          if(pre_filter->capture_type == ct_avg)
            pre_filter->accum /= pre_filter->num_samples;

          pre_filter->num_samples = 0;
        
          // determine the filter type
          if(dp->filter_type = ft_boxcar)
            {
            boxcar_datapoint_t *filt = as_boxcar(dp);

            // pick up the pre-filter value
            filt->filter[filt->tail++] = pre_filter->accum;

            if(filt->tail >= filt->filter_length)
              filt->tail = 0;

            if(filt->tail == filt->head)
              {
              filt->head++;
              if(filt->head >= filt->filter_length)
                filt->head = 0;
              }

            uint16_t num_values = 1;
            uint16_t ring_ptr = filt->head;
            pre_filter->accum = filt->filter[filt->head];
            do
              {
              ring_ptr++;
              if(ring_ptr >= filt->filter_length)
                ring_ptr = 0;

              if(ring_ptr == filt->tail)
                break;

              pre_filter->accum += filt->filter[ring_ptr];
              num_values++;
              } while(ring_ptr != filt->tail);
            
            // calculate the average value
            pre_filter->accum /= num_values;
            }
          else
            {
            filter_datapoint_t *filt = as_filter(dp);
            // calculate the accumulated filter using the next datapoint
            pre_filter->accum = (filt->function)(dp->filter_type, pre_filter->accum, filt->filter_length, filt->filter);
            }
 
          // set the floating point type
          create_can_msg_float(&dp->value, dp->can_id, 0, (float) pre_filter->accum);
          
          pre_filter->accum = 0;
          }
        
        // send the message to the can queue.  This will decorate it as
        // well
        if(dp->flags.publish)
          can_send(&dp->value);

        // if we loop back the message then it is published internally as well
        if(dp->flags.loopback)
          publish_local(&dp->value);

        // it is quite valid not not have any publish or local flags.  In this
        // case the publisher is just used as a filter/store for can values.
        uint32_t now = ticks();
        uint16_t i;
        for (i = 0; i < dp->num_alarms; i++)
          {
          alarm_t *alarm = get_alarm(dp, i);

          bool send_alarm = false;

          float fvalue;
          get_param_float(&dp->value, &fvalue);
          if (alarm->check_maximum &&
            fvalue > alarm->max_value)
            {
            // check length
            if (alarm->event_time == 0)
              alarm->event_time = now;

            if ((alarm->event_time + alarm->period) <= now)
              send_alarm = true;
            }

          if (!send_alarm &&
            alarm->check_minimum &&
            fvalue < alarm->min_value)
            {
            // check length
            if (alarm->event_time == 0)
              alarm->event_time = now;

            if ((alarm->event_time + alarm->period) <= now)
              send_alarm = true;
            }

          if (send_alarm)
            {
            int16_t value = 1;
            publish_int16(alarm->alarm_id, &value, 1);

            // update the alarm
            alarm->event_time = 0;
            }
          else if (alarm->type == event_alarm)       // event alarm, otherwise manual reset
            {
            int16_t value = 0;
            publish_int16(alarm->alarm_id, &value, 1);
            }
          }
        }

      if(dp->next_publish_tick < ticks_changed)
        dp->next_publish_tick = 0;
      else
        dp->next_publish_tick -= ticks_changed;
      }
    }
  }

static bool alarm_hook(const canmsg_t *msg, void *parg)
  {
  uint16_t num_datapoints;

  if(failed(vector_count(published_datapoints, &num_datapoints)))
    return false;

  published_datapoint_t *dp;
  uint16_t dpi;
  for(dpi = 0; dpi < num_datapoints; dpi++)
    {
    if(failed(vector_at(published_datapoints, dpi, &dp)))
      return true;
    
    uint16_t ai;
    for(ai = 0; ai < dp->num_alarms; ai++)
      {
      alarm_t *alarm = get_alarm(dp, ai);

      if(alarm->type == level_alarm &&
         alarm->reset_id == msg->id)
        {
        alarm->event_time = 0;

        int16_t value = 0;
        // reset the alarm
        publish_int16(alarm->alarm_id, &value, 1);
        }
      }

    // see if we are monitoring
    if (dp->can_id == msg->id &&
      !dp->flags.loopback &&
      !dp->flags.publish)
      {
      // this is a monitored datapoint.  Just capture the value
      memcpy(&dp->value, msg, sizeof(canas_msg_t));
      }
    }

  return true;
  }

static msg_hook_t alarm_cb = { 0, 0, alarm_hook };

extern result_t trace_init();

result_t neutron_init(const neutron_parameters_t *params, bool init_mode, bool create_worker)
  {
  task_p task_handle;
  result_t result;

  register_msg.canas.data[0] = params->node_id;
  register_msg.canas.data[1] = params->node_type;
  register_msg.canas.data[2] = params->hardware_revision;
  register_msg.canas.data[3] = params->software_revision;

  if(failed(result = vector_create(sizeof(published_datapoint_t *), &published_datapoints)))
    return result;

  memid_t publisher_key;
  if(init_mode)
    result = reg_create_key(0, s_publisher_key, &publisher_key);
  else
    result = reg_open_key(0, s_publisher_key, &publisher_key);
  
  if(failed(result))
    return result;

  // create the trace handler
  trace_init();

  // enumerate all of the keys
  field_datatype key_type;
  memid_t key = 0;
  uint16_t i;
  char name[REG_NAME_MAX+1];

  do
    {
    key_type = field_key;

    if(failed(result = reg_enum_key(publisher_key, &key_type, 0, 0, REG_NAME_MAX + 1, name, &key)))
      {
      if(result == e_not_found)
        break;
      
      return result;
      }

    bool is_valid = true;
    // ensure the name is a can-id
    for(i = 0; i < REG_NAME_MAX; i++)
      {
      if (name[i] == 0)
        break;

      if(!isdigit(name[i]))
        {
        is_valid = false;
        break;
        }
      }

    if(is_valid)
      {
      uint16_t can_id = atoi(name);

      create_datapoint(published_datapoints, can_id, key);
      }
    } while(true);

  subscribe(&alarm_cb);

  if(create_worker)
    // create the publisher schedule
    return task_create("CAN_PUBLISHER",
                       params->publisher_stack_length == 0 
                          ? DEFAULT_STACK_SIZE 
                          : params->publisher_stack_length,
                       publish_task, 0,
                       NORMAL_PRIORITY, &task_handle);
  
  return s_ok;
  }

result_t publish_datapoint(uint16_t can_id, uint16_t rate, filter_type_e filter_type, uint16_t filter_length, bool loopback, bool publish)
  {
  result_t result;
  memid_t publisher_key;
  result = reg_open_key(0, s_publisher_key, &publisher_key);

  if(result == e_path_not_found)
    result = reg_create_key(0, s_publisher_key, &publisher_key);

  if (failed(result))
    return result;

  char name[REG_NAME_MAX + 1];
  snprintf(name, REG_NAME_MAX, "%d", can_id);

  // see if the datapoint exists
  memid_t datapoint_key;
  if (succeeded(reg_open_key(publisher_key, name, &datapoint_key)))
    return e_exists;

  // create the key
  if (failed(result = reg_create_key(publisher_key, name, &datapoint_key)))
    return result;

  if (failed(result = reg_set_uint16(datapoint_key, s_rate_value, rate)))
    return result;

  if (failed(result = reg_set_uint16(datapoint_key, s_sample_type_value, ct_lifo)))
    return result;

  // just average the values published
  if (failed(result = reg_set_uint16(datapoint_key, s_filter_type_value, filter_type)))
    return result;

  if (filter_length > 0 &&
    failed(result = reg_set_uint16(datapoint_key, s_filter_length_value, filter_length)))
    return result;

  if (failed(result = reg_set_bool(datapoint_key, s_loopback_value, loopback)))
    return result;

  if (failed(result = reg_set_bool(datapoint_key, s_publish_value, publish)))
    return result;

  // and start the datapoint running
  return create_datapoint(published_datapoints, can_id, datapoint_key);
  }

result_t monitor_datapoint(uint16_t can_id)
  {
  result_t result;
  published_datapoint_t *dp;
  if (failed(find_datapoint(can_id, &dp, 0)))
    {
    // there is no datapoint being published
    dp = alloc_datapoint(ft_none, 0, 0);
    if (dp == 0)
      return e_not_enough_memory;

    dp->can_id = can_id;
    // add the monitored datapoint
    return vector_push_back(published_datapoints, &dp);
    }

  return s_ok;
  }