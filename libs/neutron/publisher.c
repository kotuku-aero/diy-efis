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

typedef enum _filter_type_e
  {
  ft_none,
  ft_boxcar,
  ft_iir,
  ft_fir
  } filter_type_e;

typedef enum _capture_type_e
  {
  ct_fifo,      // use first sample after a publish
  ct_lifo,      // use last sample before a publish
  ct_min,       // use min of samples
  ct_max,       // use max of samples
  ct_avg        // use average of samples
  } capture_type_e;

typedef union _variant_t {
  float float_value;
  int8_t int8_value[4];
  uint8_t uint8_value[4];
  int16_t int16_value[2];
  uint16_t uint16_value[2];
  int32_t int32_value;
  uint32_t uint32_value;
} variant_t;                      // value that has been published


// this group of functions defines an area to publish regular status updates
// the structure will be filled in with the current values
typedef struct _published_datapoint_t
  {
  uint16_t can_id;              // id to publish
  uint16_t publish_rate;        // in number of ticks
  uint32_t next_publish_tick;   // will be set by scheduler, but can also be used

  union {
    uint16_t publish : 1;       // set to publish
    uint16_t loopback : 1;      // set to loopback to sender
    } flags;

  uint8_t value_type;           // the published value TYPE
  variant_t value;              // value that has been published

  capture_type_e capture_type;   // what type of capture to do
  uint32_t num_samples;         // number of samples between captures
  float accum;              // accumulated value for the capture filter

  // the filter type other than none is only really relevant to
  // floating point types
  filter_type_e filter_type;
  // max length of the filter
  uint16_t filter_length;
  // for all filters this is the filter values, based on time.
  handle_t values;
  // this is the filter coefficients for iir and fir filters
  handle_t coefficients;
  // if the filter is an iir or fir then the gain is used to
  // bring the value back to 1.0
  float gain;

  handle_t alarms;              // alarms to be raised based on the datapoint.
  } published_datapoint_t;

typedef struct _enum_descr {
    uint16_t value;
    const char *description;
} enum_descr ;

static vector_t published_datapoints;
static handle_t can_publisher_semp;

static const char *s_publisher_key = "neutron";
static const char *s_rate_value = "rate";
static const char *s_type_value = "type";
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

static const char *lookup_enum(enum_descr *descr, uint16_t num, uint16_t value)
  {
  uint16_t i;
  for(i = 0; i < num; i++)
    {
    if(descr[i].value == value)
      return descr[i].description;
    }

  return "invalid";
  }

static published_datapoint_t *find_datapoint(uint16_t id)
  {
  uint16_t i;
  uint16_t len;

  if(failed(vector_count(published_datapoints, &len)))
    return 0;

  published_datapoint_t *dp;
  if(failed(vector_begin(published_datapoints, (void **)&dp)))
    return 0;

  for(i = 0; i < len; i++)
    {

    if(dp[i].can_id == id)
      return dp + i;
    }
  
  return 0;
  }

static int8_t convert_to_int8(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return (int8_t) *((const float *)value);
    case CANAS_DATATYPE_INT32 :
    {
      int32_t val = *((const int32_t *)value);
      if(val > INT8_MAX)
        return INT8_MAX;
      if(val < INT8_MIN)
        return INT8_MIN;
      return (int8_t) val;
    }
    case CANAS_DATATYPE_UINT32 :
    {
      uint32_t val = *((const uint32_t *)value);
      if(val > INT8_MAX)
        return INT8_MAX;
      
      return (int8_t) val;
    }
    case CANAS_DATATYPE_SHORT :
    {
      int16_t val = *((const int16_t *)value);
      if(val > INT8_MAX)
        return INT8_MAX;
      if(val < INT8_MIN)
        return INT8_MIN;
      return (int8_t) val;
    }
    case CANAS_DATATYPE_USHORT :
    {
      uint16_t val = *((const uint16_t *)value);
      if(val > INT8_MAX)
        return INT8_MAX;
      
      return (int8_t) val;
    }
    case CANAS_DATATYPE_CHAR :
      return *((const int8_t *)value);
    case CANAS_DATATYPE_UCHAR :
    {
      uint8_t val = *((const uint8_t *)value);
      if(val > INT8_MAX)
        return INT8_MAX;
      
      return (int8_t) val;
    }
    }
  
  return 0;
  }

static uint8_t convert_to_uint8(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return (uint8_t) *((const float *)value);
    case CANAS_DATATYPE_INT32 :
    {
      int32_t val = *((const int32_t *)value);
      if(val > UINT8_MAX)
        return UINT8_MAX;
      
      if(val < 0)
        return 0;

      return (uint8_t) val;
    }
    case CANAS_DATATYPE_UINT32 :
    {
      uint32_t val = *((const uint32_t *)value);
      if(val > UINT8_MAX)
        return UINT8_MAX;

      return (uint8_t) val;
    }
      break;
    case CANAS_DATATYPE_SHORT :
    {
      int16_t val = *((const int16_t *)value);
      if(val > UINT8_MAX)
        return UINT8_MAX;
      
      if(val < 0)
        return 0;

      return (uint8_t) val;
    }
      break;
    case CANAS_DATATYPE_USHORT :
    {
      uint16_t val = *((const uint16_t *)value);
      if(val > UINT8_MAX)
        return UINT8_MAX;
 
      return (uint8_t) val;
    }
      break;
    case CANAS_DATATYPE_CHAR :
    {
      int8_t val = *((const int8_t *)value);

      if(val < 0)
        return 0;

      return (const uint8_t) val;
    }
      break;
    case CANAS_DATATYPE_UCHAR :
      return *((const uint8_t *)value);
      break;    
    }
  
  return 0;
  }

static int16_t convert_to_int16(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return (int16_t) *((const float *)value);
    case CANAS_DATATYPE_INT32 :
    {
      int32_t val = *((const int32_t *)value);
      if(val > INT16_MAX)
        return INT16_MAX;
      
      if(val < INT16_MIN)
        return INT16_MIN;
      
      return (int16_t) val;
    }
    case CANAS_DATATYPE_UINT32 :
    {
      uint32_t val = *((const uint32_t *)value);
      if(val > INT16_MAX)
        return INT16_MAX;
      
      return (int16_t) val;
    }
    case CANAS_DATATYPE_SHORT :
      return *((const int16_t *)value);
    case CANAS_DATATYPE_USHORT :
    {
      uint16_t val = *((const uint16_t *)value);
      if(val > INT16_MAX)
        return INT16_MAX;
      
      return (int16_t) val;
    }
    case CANAS_DATATYPE_CHAR :
      return *((const int8_t *)value);
    case CANAS_DATATYPE_UCHAR :
      return *((const uint8_t *)value);
    }
  
  return 0;
  }

static uint16_t convert_to_uint16(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return (uint16_t) *((const float *)value);
    case CANAS_DATATYPE_INT32 :
    {
      int32_t val = *((const int32_t *)value);
      if(val > UINT16_MAX)
        return UINT16_MAX;

      return (uint16_t) val;
    }
    case CANAS_DATATYPE_UINT32 :
    {
      uint32_t val = *((const uint32_t *)value);
      if(val > UINT16_MAX)
        return UINT16_MAX;
      
      return (uint16_t) val;
    }
    case CANAS_DATATYPE_SHORT :
    {
      int16_t val = *((const int16_t *)value);
      if(val > UINT16_MAX)
        return UINT16_MAX;
      
      if(val < 0)
        return 0;
      
      return (uint16_t) val;
    }
    case CANAS_DATATYPE_USHORT :
      return *((const uint16_t *)value);
    case CANAS_DATATYPE_CHAR :
      return *((const int8_t *)value);
    case CANAS_DATATYPE_UCHAR :
      return *((const uint8_t *)value);
    }
  
  return 0;
  }

static int32_t convert_to_int32(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return (int32_t) *((const float *)value);
    case CANAS_DATATYPE_INT32 :
      return *((const int32_t *)value);
    case CANAS_DATATYPE_UINT32 :
    {
      uint32_t val = *((const uint32_t *)value);
      if(val > INT32_MAX)
        return INT32_MAX;
      
      return (int32_t) val;
    }
    case CANAS_DATATYPE_SHORT :
      return *((const int16_t *)value);
    case CANAS_DATATYPE_USHORT :
      return *((const uint16_t *)value);
    case CANAS_DATATYPE_CHAR :
      return *((const int8_t *)value);
    case CANAS_DATATYPE_UCHAR :
      return *((const uint8_t *)value);
    }
  
  return 0;
  }

static uint32_t convert_to_uint32(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return (uint32_t) *((const float *)value);
    case CANAS_DATATYPE_INT32 :
    {
      int32_t val = *((const int32_t *)value);
      if(val < 0)
        return 0;
      
      return (uint32_t) val;
    }
    case CANAS_DATATYPE_UINT32 :
      return *((const uint32_t *)value);
    case CANAS_DATATYPE_SHORT :
    {
      int16_t val = *((const int16_t *)value);
      if(val < 0)
        return 0;
      
      return (uint32_t) val;
    }
    case CANAS_DATATYPE_USHORT :
      return *((const uint16_t *)value);
    case CANAS_DATATYPE_CHAR :
    {
      int8_t val = *((const int8_t *)value);
      if(val < 0)
        return 0;
      
      return (uint32_t) val;
    }
    case CANAS_DATATYPE_UCHAR :
      return *((const uint8_t *)value);
    }
  
  return 0;
  }

static float convert_to_float(const void *value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
      return  *((const float *)value);
    case CANAS_DATATYPE_INT32 :
      return *((const int32_t *)value);
    case CANAS_DATATYPE_UINT32 :
      return *((const uint32_t *)value);
    case CANAS_DATATYPE_SHORT :
      return *((const int16_t *)value);
    case CANAS_DATATYPE_USHORT :
      return *((const uint16_t *)value);
    case CANAS_DATATYPE_CHAR :
      return *((const int8_t *)value);
    case CANAS_DATATYPE_UCHAR :
      return *((const uint8_t *)value);
    }
  
  return 0;
  }

static void assign_variant(variant_t *dest, float value, uint8_t dt)
  {
  switch(dt)
    {
    case CANAS_DATATYPE_FLOAT :
       dest->float_value = value;
       break;
    case CANAS_DATATYPE_INT32 :
      dest->int32_value = convert_to_int32(&value, CANAS_DATATYPE_FLOAT);
      break;
    case CANAS_DATATYPE_UINT32 :
      dest->uint32_value = convert_to_uint32(&value, CANAS_DATATYPE_FLOAT);
      break;
    case CANAS_DATATYPE_SHORT :
      dest->int16_value[0] = convert_to_int16(&value, CANAS_DATATYPE_FLOAT);
      break;
    case CANAS_DATATYPE_USHORT :
      dest->uint16_value[0] = convert_to_uint16(&value, CANAS_DATATYPE_FLOAT);
      break;
    case CANAS_DATATYPE_CHAR :
      dest->int8_value[0] = convert_to_int8(&value, CANAS_DATATYPE_FLOAT);
      break;
    case CANAS_DATATYPE_UCHAR :
      dest->uint8_value[0] = convert_to_uint8(&value, CANAS_DATATYPE_FLOAT);
      break;
    }
  }

extern void publish_local(const canmsg_t *msg);

static result_t publish_value(const void *value, uint8_t type, uint16_t id)
  {
  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return e_not_found;
  
  variant_t sample_value;

  switch(datapoint->value_type)
    {
    case CANAS_DATATYPE_FLOAT :
      sample_value.float_value =  convert_to_float(value, type);
      break;
    case CANAS_DATATYPE_INT32 :
      sample_value.int32_value = convert_to_int32(value, type);
      break;
    case CANAS_DATATYPE_UINT32 :
      sample_value.uint32_value = convert_to_uint32(&value, type);
      break;
    case CANAS_DATATYPE_SHORT :
      sample_value.int16_value[0] = convert_to_int16(&value, type);
      break;
    case CANAS_DATATYPE_USHORT :
      sample_value.uint16_value[0] = convert_to_uint16(&value, type);
      break;
    case CANAS_DATATYPE_CHAR :
      sample_value.int8_value[0] = convert_to_int8(&value, type);
      break;
    case CANAS_DATATYPE_UCHAR :
      sample_value.uint8_value[0] = convert_to_uint8(&value, type);
      break;
    }

  switch(datapoint->capture_type)
    {
  case ct_fifo :
    if(datapoint->num_samples != 0)
      break;
  case ct_lifo :
    assign_variant(&sample_value, datapoint->accum, datapoint->value_type);
    break;
  case ct_min :
    switch(datapoint->value_type)
      {
      case CANAS_DATATYPE_FLOAT :
        if(sample_value.float_value < datapoint->accum)
          datapoint->accum = sample_value.float_value;
        break;
      case CANAS_DATATYPE_INT32 :
        if(sample_value.int32_value < convert_to_int32(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.int32_value;
        break;
      case CANAS_DATATYPE_UINT32 :
        if(sample_value.uint32_value < convert_to_uint32(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.uint32_value;
        break;
      case CANAS_DATATYPE_SHORT :
        if(sample_value.int16_value < convert_to_int16(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.int16_value[0];
        break;
      case CANAS_DATATYPE_USHORT :
        if(sample_value.uint16_value < convert_to_uint16(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.uint16_value[0];
        break;
      case CANAS_DATATYPE_CHAR :
        if(sample_value.int8_value < convert_to_int8(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.int8_value[0];
        break;
      case CANAS_DATATYPE_UCHAR :
        if(sample_value.uint8_value < convert_to_uint8(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.uint8_value[0];
        break;
      }
    break;
  case ct_max :
    switch(datapoint->value_type)
      {
      case CANAS_DATATYPE_FLOAT :
        if(sample_value.float_value > datapoint->accum)
          datapoint->accum = sample_value.float_value;
        break;
      case CANAS_DATATYPE_INT32 :
        if(sample_value.int32_value > convert_to_int32(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.int32_value;
        break;
      case CANAS_DATATYPE_UINT32 :
        if(sample_value.uint32_value > convert_to_uint32(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.uint32_value;
        break;
      case CANAS_DATATYPE_SHORT :
        if(sample_value.int16_value > convert_to_int16(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.int16_value[0];
        break;
      case CANAS_DATATYPE_USHORT :
        if(sample_value.uint16_value > convert_to_uint16(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.uint16_value[0];
        break;
      case CANAS_DATATYPE_CHAR :
        if(sample_value.int8_value > convert_to_int8(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.int8_value[0];
        break;
      case CANAS_DATATYPE_UCHAR :
        if(sample_value.uint8_value > convert_to_uint8(&datapoint->accum, CANAS_DATATYPE_FLOAT))
          datapoint->accum = sample_value.uint8_value[0];
        break;
      }
    break;
  case ct_avg :
    // avaerage always uses floating point (which can overflow!)
    switch(datapoint->value_type)
      {
      case CANAS_DATATYPE_FLOAT :
        datapoint->accum += sample_value.float_value;
        break;
      case CANAS_DATATYPE_INT32 :
        datapoint->accum += sample_value.int32_value;
        break;
      case CANAS_DATATYPE_UINT32 :
        datapoint->accum += sample_value.uint32_value;
        break;
      case CANAS_DATATYPE_SHORT :
        datapoint->accum += sample_value.int16_value[0];
        break;
      case CANAS_DATATYPE_USHORT :
        datapoint->accum += sample_value.uint16_value[0];
        break;
      case CANAS_DATATYPE_CHAR :
        datapoint->accum += sample_value.int8_value[0];
        break;
      case CANAS_DATATYPE_UCHAR :
        datapoint->accum += sample_value.uint8_value[0];
        break;
      }
    break;
    }

  datapoint->num_samples ++;

  return s_ok;
  }

result_t publish_float(uint16_t id, float value)
  {
  return publish_value(&value, CANAS_DATATYPE_FLOAT, id);
  }

result_t get_datapoint_float(uint16_t id, float *value)
  {
  if (value == 0)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if (datapoint == 0)
    return e_not_found;

  *value = convert_to_float(&datapoint->value, datapoint->value_type);
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

  return publish_value(value, int8_dt[len-1], id);
  }

result_t get_datapoint_int8(uint16_t id, int8_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len < 0 || *len > 4)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if (datapoint == 0)
    return e_not_found;

  switch (datapoint->value_type)
    {
    case CANAS_DATATYPE_CHAR :
      *value = datapoint->value.int8_value[0];
      break;
    case CANAS_DATATYPE_CHAR2 :
      if (len < 3)
        return e_bad_parameter;
      value[0] = datapoint->value.int8_value[0];
      value[1] = datapoint->value.int8_value[1];
      break;
    case CANAS_DATATYPE_CHAR3 :
      if (len < 4)
        return e_bad_parameter;
      value[0] = datapoint->value.int8_value[0];
      value[1] = datapoint->value.int8_value[1];
      value[2] = datapoint->value.int8_value[2];
      break;
    case CANAS_DATATYPE_CHAR4 :
      value[0] = datapoint->value.int8_value[0];
      value[1] = datapoint->value.int8_value[1];
      value[2] = datapoint->value.int8_value[2];
      value[3] = datapoint->value.int8_value[3];
      break;
    default :
      *value = convert_to_int8(&datapoint->value, datapoint->value_type);
      *len = 1;
      break;
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
  if (value == 0 || len > 4 || len == 0)
    return e_bad_parameter;

  return publish_value(value, uint8_dt[len - 1], id);
  }

result_t get_datapoint_uint8(uint16_t id, uint8_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len < 0 || *len > 4)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if (datapoint == 0)
    return e_not_found;

  switch (datapoint->value_type)
    {
    case CANAS_DATATYPE_UCHAR:
      *value = datapoint->value.uint8_value[0];
      break;
    case CANAS_DATATYPE_UCHAR2:
      if (len < 3)
        return e_bad_parameter;
      value[0] = datapoint->value.uint8_value[0];
      value[1] = datapoint->value.uint8_value[1];
      break;
    case CANAS_DATATYPE_UCHAR3:
      if (len < 4)
        return e_bad_parameter;
      value[0] = datapoint->value.uint8_value[0];
      value[1] = datapoint->value.uint8_value[1];
      value[2] = datapoint->value.uint8_value[2];
      break;
    case CANAS_DATATYPE_UCHAR4:
      value[0] = datapoint->value.uint8_value[0];
      value[1] = datapoint->value.uint8_value[1];
      value[2] = datapoint->value.uint8_value[2];
      value[3] = datapoint->value.uint8_value[3];
      break;
    default:
      *value = convert_to_uint8(&datapoint->value, datapoint->value_type);
      *len = 1;
      break;
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
  if (len > 2 || len == 0)
    return e_bad_parameter;

  return publish_value(value, int16_dt[len-1], id);
  }

result_t get_datapoint_int16(uint16_t id, int16_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len < 0 || *len > 2)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if (datapoint == 0)
    return e_not_found;

  switch (datapoint->value_type)
    {
    case CANAS_DATATYPE_SHORT:
      *value = datapoint->value.int16_value[0];
      break;
    case CANAS_DATATYPE_SHORT2:
      if (len < 2)
        return e_bad_parameter;
      value[0] = datapoint->value.int16_value[0];
      value[1] = datapoint->value.int16_value[1];
      break;
    default:
      *value = convert_to_int16(&datapoint->value, datapoint->value_type);
      *len = 1;
      break;
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
  return publish_value(value, uint16_dt[len - 1], id);
  }

result_t get_datapoint_uint16(uint16_t id, uint16_t *value, uint16_t *len)
  {
  if (value == 0 || len == 0 ||
    *len < 0 || *len > 2)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if (datapoint == 0)
    return e_not_found;

  switch (datapoint->value_type)
    {
    case CANAS_DATATYPE_USHORT:
      *value = datapoint->value.int16_value[0];
      break;
    case CANAS_DATATYPE_USHORT2:
      if (len < 2)
        return e_bad_parameter;
      value[0] = datapoint->value.uint16_value[0];
      value[1] = datapoint->value.uint16_value[1];
      break;
    default:
      *value = convert_to_uint16(&datapoint->value, datapoint->value_type);
      *len = 1;
      break;
    }
  return s_ok;
  }

result_t publish_int32(uint16_t id, int32_t value)
  {
  return publish_value(&value, CANAS_DATATYPE_INT32, id);
  }

result_t get_datapoint_int32(uint16_t id, int32_t *value)
  {
  if (value == 0)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if (datapoint == 0)
    return e_not_found;

  *value = convert_to_int32(&datapoint->value, datapoint->value_type);
  return s_ok;
  }

result_t publish_uint32(uint16_t id, uint32_t value)
  {
  return publish_value(&value, CANAS_DATATYPE_UINT32, id);
  }

result_t get_datapoint_uint32(uint16_t id, uint32_t *value)
  {
  if (value == 0)
    return e_bad_parameter;

  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return e_not_found;
  
  *value = convert_to_uint32(&datapoint->value, datapoint->value_type);
  return s_ok;
  }

/**
 * Load a datapoint from the registry
 * @param datapoints  Array to add datapoint to
 * @param key         key to load from
 * @return s_ok if loaded ok
 */
static result_t load_datapoint(handle_t datapoints, uint16_t can_id, memid_t key)
  {
  result_t result;
  uint16_t v_uint16;
  published_datapoint_t dp;
  char name[REG_NAME_MAX + 1];
  alarm_t alarm;

  memset(&dp, 0, sizeof(published_datapoint_t));

  dp.can_id = can_id;
  if(succeeded(reg_get_uint16(key, s_rate_value, &v_uint16)))
    {
    dp.publish_rate = v_uint16;
    dp.flags.publish = 1;
    }
  else
    dp.flags.publish = 0;

  // type of can id published
  if(succeeded(reg_get_uint16(key, s_type_value, &v_uint16)))
    dp.value_type = (uint8_t) v_uint16;

  // type of sample accumulation
  if(succeeded(reg_get_uint16(key, s_sample_type_value, &v_uint16)))
    dp.capture_type = (capture_type_e)v_uint16;

  // type of filter.  The filter runs at the publish frequency
  if(succeeded(reg_get_uint16(key, s_filter_type_value, &v_uint16)))
    dp.filter_type = (filter_type_e) v_uint16;

  // if we are a none filter the don't process
  if(v_uint16 > 0)
    {
    // must have a length!
    if(succeeded(reg_get_uint16(key, s_filter_length_value, &v_uint16)))
      {
      // always clip this, as long filters are NEVER supported
      if(v_uint16 > 16)
        v_uint16 = 16;

      dp.filter_length = v_uint16;

      // create the values filter, can be boxcar, or fir/iir
      vector_create(sizeof(float), &dp.values);

      float coeff = 0.0;
      for(v_uint16 = 0; v_uint16 < dp.filter_length; v_uint16++)
        vector_push_back(dp.values, &coeff);

      // is fir or iir filter
      if(dp.filter_type > 1)
        {
        // create the coefficient array
        vector_create(sizeof(float), &dp.coefficients);

        // work over each coefficient.
        for(v_uint16 = 0; v_uint16 < dp.filter_length; v_uint16++)
          {

          snprintf(name, REG_NAME_MAX, s_coefficient_value, v_uint16+1);
          float coeff;
          if(failed(reg_get_float(key, name, &coeff)))
            coeff = 0;

          vector_push_back(dp.coefficients, &coeff);
          }
        
        // pick up the gain of the filter
        if(failed(reg_get_float(key, s_filter_gain_value, &dp.gain)))
          dp.gain = 1.0;
        }
      }
    }

  bool v_bool;
  if(succeeded(reg_get_bool(key, s_loopback_value, &v_bool)))
    dp.flags.loopback = 1;

  if(succeeded(reg_get_bool(key, s_publish_value, &v_bool)))
    dp.flags.publish = v_bool;
  else
    dp.flags.publish = true;

  memid_t alarm_key = 0;

  uint16_t i;
  field_datatype key_type;

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
      if(dp.alarms == 0)
        // create the alarms
        vector_create(sizeof(alarm_t), &dp.alarms);

      memset(&alarm, 0, sizeof(alarm_t));

      alarm.alarm_id = can_id;

      if(succeeded(reg_get_float(alarm_key, s_alarm_min_value, &v_float)))
        alarm.min_value = v_float;

      if(succeeded(reg_get_float(alarm_key, s_alarm_max_value, &v_float)))
        alarm.max_value = v_float;

      if(succeeded(reg_get_uint16(alarm_key, s_alarm_type_value, &v_uint16)))
        alarm.type = (alarm_type_e)v_uint16;

      if(succeeded(reg_get_uint16(alarm_key, s_alarm_period_value, &v_uint16)))
        alarm.period = v_uint16;

      if(succeeded(reg_get_uint16(alarm_key, s_alarm_reset_value, &v_uint16)))
        alarm.reset_id = v_uint16;

      vector_push_back(dp.alarms, &alarm);
      }
    } while(true);

  // add the published parameter
  vector_push_back(datapoints, &dp);

  return s_ok;
  }

static void publish_task(void *parg)
  {
  int nis_timeout = 100;

  semaphore_create(&can_publisher_semp);
  
  uint32_t last_tick = ticks();
  uint8_t i;
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

    published_datapoint_t *datapoints;
    if(failed(vector_begin(published_datapoints, (void *)&datapoints)))
        continue;

    published_datapoint_t *dp;
    canmsg_t msg;
    uint16_t  i;

    float index_value;
    float filtered_value;

    // do each datapoint we have registered
    for(i = 0; i < num_datapoints; i++)
      {
      dp = &datapoints[i];
      
      bool publish = dp->flags.publish &&
         dp->next_publish_tick <= ticks_changed;

      if(publish)
        {
        dp->next_publish_tick = dp->publish_rate;
        
        // see if the accumulator is an average,
        if(dp->capture_type == ct_avg)
          dp->accum /= dp->num_samples;
        
        dp->num_samples = 0;
        
        // determine the filter type
        switch(dp->filter_type)
          {
          case ft_none :
            // just send the value
            // If the capture type is an average of the samples we
            // always store the value as a floating point.
            if(dp->capture_type == ct_avg)
              {
              switch(dp->value_type)
                {
                case CANAS_DATATYPE_FLOAT :
                  dp->value.float_value = dp->accum;
                  break;
                case CANAS_DATATYPE_INT32 :
                  dp->value.int32_value = convert_to_int32(&dp->accum, CANAS_DATATYPE_FLOAT);
                  break;
                case CANAS_DATATYPE_UINT32 :
                  dp->value.uint32_value = convert_to_uint32(&dp->accum, CANAS_DATATYPE_FLOAT);
                  break;
                case CANAS_DATATYPE_SHORT :
                  dp->value.int16_value[0] = convert_to_int16(&dp->accum, CANAS_DATATYPE_FLOAT);
                  break;
                case CANAS_DATATYPE_USHORT :
                  dp->value.uint16_value[0] = convert_to_uint16(&dp->accum, CANAS_DATATYPE_FLOAT);
                  break;
                case CANAS_DATATYPE_CHAR :
                  dp->value.int8_value[0] = convert_to_int8(&dp->accum, CANAS_DATATYPE_FLOAT);
                  break;
                case CANAS_DATATYPE_UCHAR :
                  dp->value.uint8_value[0] = convert_to_uint8(&dp->accum, CANAS_DATATYPE_FLOAT);
                  break;
                  }
              }
            else
              // otherwise a integer type so we just copy the value.
              memcpy(&dp->value, &dp->accum, sizeof(variant_t));
            break;
          case ft_boxcar :
            vector_erase(dp->values, 0);

            // get the accumulated value.
            // TODO: handle min max etc
            index_value =  convert_to_float(&dp->accum, dp->value_type);
            vector_push_back(dp->values, &index_value);
            
            dp->accum = 0;

            // must be a float type for this to work....
            for(i = 0; i < dp->filter_length; i++)
              {
              vector_at(dp->values, i, &index_value);
              dp->accum += index_value;
              }

            // calculate the average value
            dp->accum /= dp->filter_length;
            
            switch(dp->value_type)
              {
              case CANAS_DATATYPE_FLOAT :
                dp->value.float_value = dp->accum;
                break;
              case CANAS_DATATYPE_INT32 :
                dp->value.int32_value = convert_to_int32(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_UINT32 :
                dp->value.uint32_value = convert_to_uint32(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_SHORT :
                dp->value.int16_value[0] = convert_to_int16(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_USHORT :
                dp->value.uint16_value[0] = convert_to_uint16(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_CHAR :
                dp->value.int8_value[0] = convert_to_int8(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_UCHAR :
                dp->value.uint8_value[0] = convert_to_uint8(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              }
            break;
          default : // iir or fir
            {
            // todo: this should use the dsp fir code (MAC)
            vector_erase(dp->values, 0);

            // get the accumulated value.
            index_value =  convert_to_float(&dp->value, dp->value_type);
            vector_insert(dp->values, 0, &index_value);

            float *coeff;
            vector_begin(dp->coefficients, (void *)&coeff);

            float *values;
            vector_begin(dp->values, (void *) &values);

            dp->accum = 0;

            // must be a float type for this to work....
            for(i = 0; i < dp->filter_length; i++)
               dp->accum += coeff[i] * values[i];
            
            // multiply by the gain if set
            if(dp->gain != 1.0)
              dp->accum *= dp->gain;

            // now convert back to the actual type
            switch(dp->value_type)
              {
              case CANAS_DATATYPE_FLOAT :
                break;
              case CANAS_DATATYPE_INT32 :
                dp->value.int32_value = convert_to_int32(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_UINT32 :
                dp->value.uint32_value = convert_to_uint32(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_SHORT :
                dp->value.int16_value[0] = convert_to_int16(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_USHORT :
                dp->value.uint16_value[0] = convert_to_uint16(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_CHAR :
                dp->value.int8_value[0] = convert_to_int8(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              case CANAS_DATATYPE_UCHAR :
                dp->value.uint8_value[0] = convert_to_uint8(&dp->accum, CANAS_DATATYPE_FLOAT);
                break;
              }
            }
            break;
            }

        switch(dp->value_type)
          {
          case CANAS_DATATYPE_FLOAT :
            create_can_msg_float(&msg, 0, 0, dp->value.float_value);
            break;
          case CANAS_DATATYPE_INT32 :
            create_can_msg_int32(&msg, 0, 0, dp->value.int32_value);
            break;
          case CANAS_DATATYPE_UINT32 :
            create_can_msg_uint32(&msg, 0, 0, dp->value.uint32_value);
            break;
          case CANAS_DATATYPE_SHORT :
            create_can_msg_int16(&msg, 0, 0, dp->value.int16_value[0]);
            break;
          case CANAS_DATATYPE_SHORT2:
            create_can_msg_int16_2(&msg, 0, 0, dp->value.int16_value[0], dp->value.int16_value[2]);
            break;
          case CANAS_DATATYPE_USHORT :
            create_can_msg_uint16(&msg, 0, 0, dp->value.uint16_value[0]);
            break;
          case CANAS_DATATYPE_USHORT2 :
            create_can_msg_uint16_2(&msg, 0, 0, dp->value.uint16_value[0], dp->value.uint16_value[1]);
            break;
          case CANAS_DATATYPE_CHAR :
            create_can_msg_int8(&msg, 0, 0, dp->value.int8_value[0]);
            break;
          case CANAS_DATATYPE_CHAR2:
            create_can_msg_int8_2(&msg, 0, 0, dp->value.int8_value[0], dp->value.int8_value[1]);
            break;
          case CANAS_DATATYPE_CHAR3:
            create_can_msg_int8_3(&msg, 0, 0, dp->value.int8_value[0], dp->value.int8_value[1], dp->value.int8_value[2]);
            break;
          case CANAS_DATATYPE_CHAR4:
            create_can_msg_int8_4(&msg, 0, 0, dp->value.int8_value[0], dp->value.int8_value[1], dp->value.int8_value[2], dp->value.int8_value[3]);
            break;
          case CANAS_DATATYPE_UCHAR :
            create_can_msg_uint8(&msg, 0, 0, dp->value.uint8_value);
            break;
          case CANAS_DATATYPE_UCHAR2:
            create_can_msg_uint8_2(&msg, 0, 0, dp->value.uint8_value[0], dp->value.uint8_value[1]);
            break;
          case CANAS_DATATYPE_UCHAR3:
            create_can_msg_uint8_3(&msg, 0, 0, dp->value.uint8_value[0], dp->value.uint8_value[1], dp->value.uint8_value[2]);
            break;
          case CANAS_DATATYPE_UCHAR4:
            create_can_msg_uint8_4(&msg, 0, 0, dp->value.uint8_value[0], dp->value.uint8_value[1], dp->value.uint8_value[2], dp->value.uint8_value[3]);
            break;
          }
        
        // send the message to the can queue.  This will decorate it as
        // well
        if(dp->flags.publish)
          can_send(&msg);

        // if we loop back the message then it is published internally as well
        if(dp->flags.loopback)
          publish_local(&msg);

        // it is quite valid not not have any publish or local flags.  In this
        // case the publisher is just used as a filter/store for can values.
        
        // work over the alarms next.
        alarm_t *alarms;
        uint16_t num_alarms;
        
        // get our alarms vector
        vector_begin(dp->alarms, (void **)&alarms);
        vector_count(dp->alarms, &num_alarms);
        
        uint32_t now = ticks();
        uint16_t i;
        for(i = 0; i < num_alarms; i++)
          {
          alarm_t *alarm = &alarms[i];
          
          bool send_alarm = false;
          
          if(alarm->check_maximum &&
             dp->accum > alarm->max_value)
            {
            // check length
            if(alarm->event_time == 0)
              alarm->event_time = now;
            
            if((alarm->event_time + alarm->period) <= now)
              send_alarm = true;
            }
          
          if(!send_alarm &&
             alarm->check_minimum &&
             dp->accum < alarm->min_value)
            {
            // check length
            if(alarm->event_time == 0)
              alarm->event_time = now;
            
            if((alarm->event_time + alarm->period) <= now)
              send_alarm = true;
            }
          
          if(send_alarm)
            {
            int16_t value = 1;
            publish_int16(alarm->alarm_id, &value, 1);
            alarm->event_time = 0;
            }
          else if(alarm->type == event_alarm)       // event alarm, otherwise manual reset
            {
            int16_t value = 0;
            publish_int16(alarm->alarm_id, &value, 1);
            }
          }
        
        dp->accum = 0;           // reset the accumulator
        }

      dp->next_publish_tick -= ticks_changed;
      }
    }
  }

static void alarm_hook(const canmsg_t *msg)
  {
  uint16_t num_datapoints;

  if(failed(vector_count(published_datapoints, &num_datapoints)))
    return;

  published_datapoint_t *datapoints;
  if(failed(vector_begin(published_datapoints, (void *)&datapoints)))
      return;
  
  uint16_t dpi;
  for(dpi = 0; dpi < num_datapoints; dpi++)
    {
    published_datapoint_t *dp = &datapoints[dpi];
    
    // work over the alarms next.
    alarm_t *alarms;
    uint16_t num_alarms;

    // get our alarms vector
    vector_begin(dp->alarms, (void **) &alarms);
    vector_count(dp->alarms, &num_alarms);
    
    uint16_t ai;
    for(ai = 0; ai < num_alarms; ai++)
      {
      alarm_t *alarm = &alarms[ai];
      
      if(alarm->type == level_alarm &&
         alarm->reset_id == msg->id)
        {
        alarm->event_time = 0;
        int16_t value = 0;
        // reset the alarm
        publish_int16(alarm->alarm_id, &value, 1);
        }
      }
    }
  }

static msg_hook_t alarm_cb = { 0, 0, alarm_hook };

extern result_t trace_init();

result_t neutron_init(const neutron_parameters_t *params, bool init_mode)
  {
  handle_t task_id;
  result_t result;

  register_msg.canas.data[0] = params->node_id;
  register_msg.canas.data[1] = params->node_type;
  register_msg.canas.data[2] = params->hardware_revision;
  register_msg.canas.data[3] = params->software_revision;

  if(failed(result = vector_create(sizeof(published_datapoint_t), &published_datapoints)))
    return result;

  memid_t publisher_key;
  if(failed(result = (init_mode
      ? reg_create_key(0, s_publisher_key, &publisher_key)
      : reg_open_key(0, s_publisher_key, &publisher_key))))
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
      return result;

    bool is_valid = true;
    uint16_t can_id = 0;
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

      load_datapoint(published_datapoints, can_id, key);
      }
    } while(true);

  subscribe(&alarm_cb);

  // create the publisher schedule
  return task_create("CAN_PUBLISHER",
                     params->publisher_stack_length,
                     publish_task, 0,
                     NORMAL_PRIORITY, &task_id);
  }
