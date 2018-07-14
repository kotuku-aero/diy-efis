#ifndef __input_sense_h__
#define __input_sense_h__

#include <stdbool.h>
#include <stdint.h>
#include "arinc.h"

typedef enum
{
  reg_b = 0x02C6,
  reg_c = 0x02CC,
  reg_d = 0x02D2,
  reg_e = 0x02D8,
  reg_f = 0x02DE
} dspic_port_t;

typedef bool (*preprocess_input_channel)(bool value);

typedef struct
{
  dspic_port_t port;      // base address of sense channel
  uint16_t mask;            // bit 0..15
  uint8_t config_memid;    // offset into config channel for this input
  preprocess_input_channel result_proc;
  published_msg_t can_channel;  // details and storage of the publish
  int publish_count;        // number of ticks the signal has been active
                            // once this == publish_frequency the signal is sent
                            // if the config scale is < 0 then the signal
                            // will repeat
} input_sense_channel_t;

typedef struct _input_sense_channels_t
{
  input_sense_channel_t *channels;
  uint8_t num_channels;
} input_sense_channels_t;

extern void input_sense_init(input_sense_channels_t *);

#endif
