#include "input_sense.h"
#include "../dspic-lib/microkernel.h"
#include "../dspic-lib/can_app_cfg.h"

static void input_sense_worker(void *);

static void input_sense_worker(void *parg)
  {
  input_sense_channels_t *channels = (input_sense_channels_t *)parg;

  }

void input_sense_init(input_sense_channels_t *channels)
  {
  /*
  TRISBbits.TRISB5 = 1;     // enable RB5 as input
  TRISBbits.TRISB4 = 1;     // enable RB4 input
  TRISEbits.TRISE5 = 1;     // enable RE5 as input
  TRISEbits.TRISE4 = 1;     // enable RE4 as input
  TRISDbits.TRISD0 = 1;     // enable RD0 as input
  TRISDbits.TRISD1 = 1;     // enable RD1 as input

  // and ptr processing the data on this task
  create_task(TRIM_TASK, input_sense_worker, channels);

  // create a scheduled task that will be called every 4000 msec
  schedule_task(TRIM_TASK, 4);
   */
  }
