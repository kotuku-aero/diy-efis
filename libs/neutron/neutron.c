#include "neutron.h"

static handle_t trace_stream;
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