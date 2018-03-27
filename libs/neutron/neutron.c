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
#include "neutron.h"
#include "bsp.h"

static stream_p trace_stream;
#ifdef _DEBUG
static uint16_t trace_level = 7;
#else
static uint16_t trace_level = 3;
#endif

static const char *dmesg_str = "dmesg";
static const char *trace_str = "trace-level";

// the trace stream is an event stream
void trace_init()
  {
  if (failed(stream_open(0, dmesg_str, &trace_stream)))
    {
    stream_create(0, dmesg_str, &trace_stream);
    }
  else
    stream_truncate(trace_stream, 0);

  if (failed(reg_get_uint16(0, trace_str, &trace_level)))
#ifdef _DEBUG
    trace_level = 7;
#else
    trace_level = 3;
#endif
  }

static const char *levels[8] = {
  "emergency",
  "alert",
  "critical",
  "error",
  "warning",
  "notice",
  "info",
  "debug"
  };

void trace(uint16_t level, const char *msg, ...)
  {
  va_list args;
  va_start(args, msg);

  if (trace_stream != 0 && trace_level <= level)
    {
    // send the timestamp
    stream_printf(trace_stream, "%ld %s ", ticks(), levels[level]);
    // send the level
    stream_vprintf(trace_stream, msg, args);
    stream_puts(trace_stream, "\r\n");
    }

  platform_trace(level, msg, args);
  }

static const char *s_alarm_min_value = "min";
static const char *s_alarm_max_value = "max";
static const char *s_alarm_type_value = "type";
static const char *s_alarm_period_value = "period";
static const char *s_alarm_reset_value = "reset";
static const char *s_publisher_key = "neutron";

static canmsg_t register_msg;

void publish_task(void *parg)
  {
  semaphore_p can_publisher_semp;
  int nis_timeout = 100;

  semaphore_create(&can_publisher_semp);
  
  uint32_t last_tick = ticks();
  uint32_t dispatch_tick;
  uint32_t ticks_changed;
  
  while(true)
    {
    semaphore_wait(can_publisher_semp, nis_timeout);
    
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
    }
  }

static bool alarm_hook(const canmsg_t *msg, void *parg)
  {
  /*
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
*/
  return true;
  }

static msg_hook_t alarm_cb = { 0, 0, alarm_hook };

result_t neutron_init(const neutron_parameters_t *params, bool init_mode, bool create_worker)
  {
  task_p task_handle;
  result_t result;

  register_msg.canas.data[0] = params->node_id;
  register_msg.canas.data[1] = params->node_type;
  register_msg.canas.data[2] = params->hardware_revision;
  register_msg.canas.data[3] = params->software_revision;

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

    /*
    if(is_valid)
      {
      uint16_t can_id = atoi(name);

      create_datapoint(published_datapoints, can_id, key);
      }
     * */
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
