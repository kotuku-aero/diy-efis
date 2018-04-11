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
  } published_datapoint_t;


static inline uint16_t sizeof_datapoint(uint16_t num_alarms)
  {
  uint16_t datapoint_size = sizeof(published_datapoint_t);
  
  datapoint_size += sizeof(alarm_t) * num_alarms;
  
  return datapoint_size;
  }
  
static inline alarm_t *get_alarm(published_datapoint_t *dp, uint16_t index)
  {
  uint16_t alarm_base = sizeof(published_datapoint_t);
  
  uint8_t *base = (uint8_t *)dp;
  base += alarm_base;
  base += sizeof(alarm_t) * index;
  
  return (alarm_t *)base;
  }

static inline published_datapoint_t *alloc_datapoint(uint16_t num_alarms)
  {
  uint16_t alloc_size = sizeof_datapoint(num_alarms);
  published_datapoint_t *dp = 
    (published_datapoint_t *)neutron_malloc(alloc_size);
  
  if(dp == 0)
    return 0;
  
  memset(dp, 0, alloc_size);
  dp->version = alloc_size;
  dp->num_alarms = num_alarms;
  
  return dp;
  }

typedef struct _enum_descr {
    uint16_t value;
    const char *description;
} enum_descr ;

static vector_p published_datapoints;
static semaphore_p can_publisher_semp;

static const char *s_neutron_key = "neutron";
static const char *s_rate_value = "rate";
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

result_t publish_float(uint16_t id, float value)
  {
  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  create_can_msg_float(&datapoint->value, datapoint->can_id, 0, value);

  return s_ok;
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
  set_can_id(&datapoint->value, datapoint->can_id);
  set_can_len(&datapoint->value, len + 4);
  for(index = 0; index < len; index++)
    datapoint->value.canas.data[index] = (uint8_t) *value++;

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
  set_can_id(&datapoint->value, datapoint->can_id);
  set_can_len(&datapoint->value, len + 4);
  for(index = 0; index < len; index++)
    datapoint->value.canas.data[index] = *value++;

  return s_ok;
  }

static const uint8_t int16_dt[] =
  {
  CANAS_DATATYPE_SHORT,
  CANAS_DATATYPE_SHORT2
  };

result_t publish_int16(uint16_t id, const int16_t *value, uint16_t len)
  {
  if (value == 0 || len == 0 || len > 2)
    return e_bad_parameter;

  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  datapoint->value.canas.data_type = int16_dt[len-1];
  set_can_id(&datapoint->value, datapoint->can_id);
   set_can_len(&datapoint->value, 4 + (len << 1));
 for(index = 0; index < len; index+=2)
    {
    datapoint->value.canas.data[index] = (*value >> 8);
    datapoint->value.canas.data[index+1] = *value >> 8;
    value++;
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
  if (value == 0 || len == 0 || len > 2)
    return e_bad_parameter;

  result_t result;
  uint16_t index;
  published_datapoint_t *datapoint;
  
  if(failed(result = find_datapoint(id, &datapoint, &index)))
    return result;
  
  datapoint->value.canas.data_type = uint16_dt[len-1];
  set_can_id(&datapoint->value, datapoint->can_id);
  set_can_len(&datapoint->value, 4 + (len << 1));
  for(index = 0; index < len; index+=2)
    {
    datapoint->value.canas.data[index] = (*value >> 8);
    datapoint->value.canas.data[index+1] = *value >> 8;
    value++;
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
   
  // count how many alarms there are.
  uint16_t num_alarms = 0;
  memid_t alarm_key = 0;

  uint16_t i;
  field_datatype key_type = field_key;

  // enumerate the child keys, these are alarms
  while(succeeded(reg_enum_key(key, &key_type, 0, 0, REG_NAME_MAX + 1, name, &alarm_key)))
    num_alarms++;
  
  dp = alloc_datapoint(num_alarms);

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

    // do each datapoint we have registered
    for(i = 0; i < num_datapoints; i++)
      {
      vector_at(published_datapoints, i, &dp);
      
      bool publish = dp->flags.publish &&
         dp->next_publish_tick <= ticks_changed;

      if(publish)
        {
        dp->next_publish_tick = dp->publish_rate;
        
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
    
    // we don't process received data that is published by the
    // publisher
    if(dp->flags.loopback)
      continue;
    
    uint16_t ai;
    for(ai = 0; ai < dp->num_alarms; ai++)
      {
      alarm_t *alarm = get_alarm(dp, ai);

      if(alarm->type == level_alarm &&
         alarm->reset_id == get_can_id(msg))
        {
        alarm->event_time = 0;

        int16_t value = 0;
        // reset the alarm
        publish_int16(alarm->alarm_id, &value, 1);
        }
      }

    // see if we are monitoring
    if (dp->can_id == get_can_id(msg) &&
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

result_t neutron_init(const neutron_parameters_t *params, bool create_worker)
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

  if (failed(reg_open_key(0, s_neutron_key, &publisher_key)))
    {
    if (failed(result = reg_create_key(0, s_neutron_key, &publisher_key)))
      return result;
    }

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

result_t publish_datapoint(uint16_t can_id, uint16_t rate, bool loopback, bool publish)
  {
  result_t result;
  memid_t publisher_key;
  result = reg_open_key(0, s_neutron_key, &publisher_key);

  if(result == e_path_not_found)
    result = reg_create_key(0, s_neutron_key, &publisher_key);

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

  if (failed(result = reg_set_bool(datapoint_key, s_loopback_value, loopback)))
    return result;

  if (failed(result = reg_set_bool(datapoint_key, s_publish_value, publish)))
    return result;

  // and start the datapoint running
  return create_datapoint(published_datapoints, can_id, datapoint_key);
  }
