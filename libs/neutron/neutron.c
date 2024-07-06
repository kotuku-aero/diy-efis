#include "neutron.h"
#include "registry.h"
#include <stdint.h>

static handle_t trace_stream;
static uint16_t trace_level = TRACE_LEVEL;

static const char *dmesg_str = "dmesg";
static const char *trace_str = "trace-level";

result_t is_typeof(const handle_t hndl, const typeid_t *type, void **coerced_type)
  {
  if (hndl == 0 || type == 0)
    return e_bad_pointer;

  const base_t *handle = (const base_t *)hndl;

  // check types and the bases
  for (const typeid_t *chk = handle->type; chk != 0; chk = chk->base)
    {
    if (chk == type)
      {
      if (coerced_type != 0)
        *coerced_type = (void *) handle;
#ifdef _WIN32
      return check_handle(hndl);
#else
      return s_ok;
#endif
      }
    }

  return e_bad_type;
  }

result_t check_handle(const handle_t hndl)
  {
  result_t result;
  if (hndl == 0)
    return e_bad_pointer;

  const base_t *handle = (const base_t *)hndl;

  for (const typeid_t *chk = handle->type; chk != 0; chk = chk->base)
    if (chk->is_valid != 0 &&
      failed(result = (*chk->is_valid)(hndl)))
      return result;

  return s_ok;
  }

result_t close_handle(handle_t hndl)
  {
  if (hndl == 0)
    return e_bad_parameter;

  base_t *handle = (base_t *)hndl;

  if (handle->type == 0 || handle->type->etherealize == 0)
    return e_bad_parameter;

  // destroy the handle
  (*handle->type->etherealize)(hndl);
  memset(handle, 0, sizeof(base_t));

  neutron_free(handle);
  return s_ok;
  }

// the trace stream is an event stream
void trace_init()
  {

  if (failed(reg_stream_open(0, dmesg_str, STREAM_O_RDWR | STREAM_O_TRUNC, &trace_stream)))
    {
    reg_stream_create(0, dmesg_str, STREAM_O_RDWR | STREAM_O_CREAT | STREAM_O_TRUNC, &trace_stream);
    }
  else
    stream_truncate(trace_stream, 0);

  if (failed(reg_get_uint16(0, trace_str, &trace_level)))
    trace_level = TRACE_LEVEL;
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

/*
void trace(uint16_t level, const char *msg, ...)
  {
  va_list args;
  va_start(args, msg);

  if (trace_stream != 0 && level <= trace_level)
    {
    // send the timestamp
    stream_printf(trace_stream, "%ld %s ", ticks(), levels[level]);
    // send the level
    stream_vprintf(trace_stream, msg, args);
    }

  platform_trace(level, msg, args);
  }
  */

result_t neutron_random(uint32_t seed, uint32_t *rand)
  {
  // defer to the hardware
  return bsp_random(seed, rand);
  }

result_t sysinfo(uint32_t *machine_id, uint32_t *build, const char **image_name)
  {
  if (machine_id == 0 || build == 0)
    return e_bad_parameter;

  if (failed(bsp_cpu_id(machine_id, build, image_name)))
    {
    *machine_id = 0;
    *build = 0x00010101;
    *image_name = 0;
    }

  return s_ok;
  }

static handle_t status_update_semp;
static canmsg_t status_msg;

      // status information
      // [0] = 0x06  (uint32_t)
      // [1] = NodeID
      // [2] = Board Type
      // [3] = Board Version
      // [4] = Status

const neutron_parameters_t *neutron_params;

result_t update_board_status(e_board_status status, bool force_update)
  {
  // just access the big-endian data
  status_msg.data[1] = neutron_params->node_id;
  status_msg.data[2] = (uint8_t)status;
  status_msg.data[3] = neutron_params->hardware_id;
  
  if(force_update)
    semaphore_signal(status_update_semp);
  
  return s_ok;
  }

static void status_worker(void *arg)
  {
  while(true)
    {
    semaphore_wait(status_update_semp, neutron_params->status_interval);
    
    // send the message.
    can_send(&status_msg, 2);
    }
  }

result_t neutron_init(const neutron_parameters_t *params, bool create_worker)
  {
  neutron_params = params;

  // create the trace handler
  trace_init();
  
  result_t result;
  if(failed(result = semaphore_create(&status_update_semp)) ||
     failed(create_can_msg_status(&status_msg, 
                        neutron_params->node_id,
                        neutron_params->hardware_id,
                        bs_unknown)) ||
     failed(task_create("KRN", DEFAULT_STACK_SIZE, status_worker, 0, BELOW_NORMAL, 0)))
    {
    trace_error("Cannot create the kernel status worker\n");
    return result;
    }

  return s_ok;
  }

// only set on id_def_utc msg
static tm_t current_time;
result_t now(tm_t *tm)
  {
  if(tm == 0)
    return e_bad_parameter;
  
  memcpy(tm, &current_time, sizeof(tm_t));
  
  return s_ok;
  }

result_t settime(const tm_t *tm)
  {
  if(tm == 0)
    return e_bad_parameter;
  
  memcpy(&current_time, tm, sizeof(tm_t));
  return s_ok;
  }