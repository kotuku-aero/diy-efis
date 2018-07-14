#ifndef __analog_devices_h__
#define __analog_devices_h__

#include "microkernel.h"

// this structure defines the dsPIC analog channel ports
// in our designs we have systems that have up to 4 mux'd inputs
// controlled by RE0 and RE1
// There are up to 9 analog channels that can be used.
// in addition a low pass filter can be assigned to filter the analog signal.
// the nominal sample rate is 4Khz, although the conversion is done at 600kp/s
// with up to 16 channels this equates to 37500 max samples.

// this code uses TMR3

// On PIC32 this runs as IPL7 (including timer3)

struct _channel_definition_t;
typedef float (*publish_analog_result)(struct _channel_definition_t *channel, float value);

typedef struct _channel_definition_t
  {
  char name[REG_STRING_MAX+1];          // parameter name
  uint16_t channel;
  uint16_t can_id;                      // type of publishing done.
  float offset;                         // offset to add to result
  float scale;                          // scale of the result
  uint16_t publish_rate;                // how often to publish
  bool loopback;                        // true if we receive as well as send
  uint16_t analog_value;                // read from the ADC every 1msec
  uint16_t num_samples;                 // when == publish_rate will be published
  float value;                          // filtered value
 } analog_channel_definition_t;
 
typedef uint16_t (*select_channel_fn)(uint16_t channel);

typedef struct
  {
  analog_channel_definition_t *channel_definition;
  int num_channels;
  float analog_factor;          // set to 0.000244140625 for 12 bit devices
  select_channel_fn channel_select;
  } analog_channels_t;

/**
 * Start the Analog subsystem
 * @param channels    definitions for the analog channels
 */
extern result_t analog_init(analog_channels_t *channels, uint16_t stack_length, bool factory_reset);

#endif
