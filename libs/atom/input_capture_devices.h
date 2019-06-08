#ifndef __input_capture_devices_h__
#define	__input_capture_devices_h__

#include "microkernel.h"

#define ACCUMULATOR_CHANNEL    0x80
#define CHANNEL_MASK 0x0F

/* Input capture devices are connected to one of the 16 possible DSPIC
 * input capture inputs.  Depending on the processor type, the pin functions
 * will be multiplexed.
 * This is a parameter driven input driver
 *
 * This code uses TMR2
 * On PIC32 this runs as IPL4
 */
struct _capture_channel_definition_t;

typedef struct _capture_channel_definition_t
  {
  const char *name;         // name of the config value
  uint8_t channel;          // this is the input capture channel number
  uint16_t can_id;
  uint16_t publish_rate;
  float scale;
  float offset;
  bool loopback;
  uint32_t capture_count;   // number of samples captured
  uint32_t timer_value;     // last value read from the timer capture register, if 0 then resync
  float result;                 // filter result
  } capture_channel_definition_t;

typedef struct
  {
  capture_channel_definition_t *channel_definition;
  int num_channels;
  uint8_t prescaler;            // used to calculate the timing accuracy.  must be 0 = 1, 1 = 8, 2 = 64, 3 = 256
  uint16_t divisor;             // divisor to set the sample rate
  } capture_channels_t;

// Must be created in a startup task.
  /**
   * @function capture_init(capture_channels_t *channels, uint16_t stack_length)
   * Initialize the input capture hardware
   * @param channels        Definitions for each channel
   * @param stack_length    Worker task stack size
   * @return 
   */
extern result_t capture_init(capture_channels_t *channels, uint16_t stack_length);

#endif	/* INPUT_CAPTURE_DEVICES_H */
