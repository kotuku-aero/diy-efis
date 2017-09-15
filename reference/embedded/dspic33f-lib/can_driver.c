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
#include <p33Exxxx.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "can_driver.h"
#include "microkernel.h"
#include "eeprom.h"
#include "../dspic-lib/publisher.h"

static deque_t can_tx_queue;
static deque_t can_rx_queue;

#ifdef TON
#undef TON
#endif


#define FCY 		70000             		// 70 MHz
#define BITRATE 	125                 // 125kbs
#define PRSEG_LEN   1
#define PRSEG_VAL   (PRSEG_LEN -1)
#define SEG1PH_LEN  5
#define SEG1PH_VAL  (SEG1PH_LEN -1)
#define SEG2PH_LEN  3
#define SEG2PH_VAL  (SEG2PH_LEN -1)
#define NTQ 		(1 + PRSEG_LEN + SEG1PH_LEN + SEG2PH_LEN) // Number of Tq cycles which will make the
//CAN Bit Timing .
#define BRP_VAL		((FCY/(2* NTQ * BITRATE))-1)  //Formulae used for C1CFG1bits.BRP

#define NUM_OF_ECAN_BUFFERS 32

/** we use buffers 0..7 as a ring buffer
 */
static uint16_t ecan_msg_buffer[NUM_OF_ECAN_BUFFERS][8] __attribute__((aligned(NUM_OF_ECAN_BUFFERS * 16)));

static uint8_t message_code;

static published_datapoint_t *published_datapoints;
static uint16_t num_datapoints;
static semaphore_t can_publisher_semp;
static event_t can_tx_event;

void register_datapoints(published_datapoint_t *datapoints, uint16_t num)
  {
  published_datapoints = datapoints;
  num_datapoints = num;
  }

static published_datapoint_t *find_datapoint(uint16_t id)
  {
  int i;
  for(i = 0; i < num_datapoints; i++)
    {
    if(published_datapoints[i].can_id == id)
      return published_datapoints + i;
    }
  
  return 0;
  }

void publish_short(short value, uint16_t id)
  {
  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return;
  
  datapoint->published_value.id = datapoint->can_id;
  datapoint->published_value.msg.canas.data_type = CANAS_DATATYPE_SHORT;
  datapoint->published_value.flags = 6;
  datapoint->published_value.msg.canas.data[0] = value >> 8;
  datapoint->published_value.msg.canas.data[1] = value;

  // if this was not publishing we request it to be done
  // on the next tick
  if((datapoint->flags & PUBLISH_VALUE) == 0)
    {
    datapoint->next_publish_tick = 1;
    datapoint->flags |= PUBLISH_VALUE;
    }

  // see if we do a loopback on this one
  if((datapoint->flags & PUBLISH_LOOPBACK) != 0)
    push_back(&can_rx_queue, &datapoint->published_value, INDEFINITE_WAIT);
  }

void publish_float(float value, uint16_t id)
  {
  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return;
  
  datapoint->published_value.id = datapoint->can_id;
  datapoint->published_value.msg.canas.data_type = CANAS_DATATYPE_FLOAT;
  datapoint->published_value.flags = 8;

  unsigned long *ulvalue = (unsigned long *) &value;
  datapoint->published_value.msg.canas.data[0] = (uint8_t) (*ulvalue >> 24);
  datapoint->published_value.msg.canas.data[1] = (uint8_t) (*ulvalue >> 16);
  datapoint->published_value.msg.canas.data[2] = (uint8_t) (*ulvalue >> 0);
  datapoint->published_value.msg.canas.data[3] = (uint8_t) * ulvalue;

  // if this was not publishing we request it to be done
  // on the next tick
  if((datapoint->flags & PUBLISH_VALUE) == 0)
    {
    datapoint->next_publish_tick = 1;
    datapoint->flags |= PUBLISH_VALUE;
    }

  // see if we do a loopback on this one
  if((datapoint->flags & PUBLISH_LOOPBACK) != 0)
    push_back(&can_rx_queue, &datapoint->published_value, INDEFINITE_WAIT);
  }

void publish_uint32(uint32_t value, uint16_t id)
  {
  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return;
  
  datapoint->published_value.id = datapoint->can_id;
  datapoint->published_value.msg.canas.data_type = CANAS_DATATYPE_ULONG;
  datapoint->published_value.flags = 8;
  datapoint->published_value.msg.canas.data[0] = value >> 24;
  datapoint->published_value.msg.canas.data[1] = value >> 16;
  datapoint->published_value.msg.canas.data[2] = value >> 8;
  datapoint->published_value.msg.canas.data[3] = value;

  // if this was not publishing we request it to be done
  // on the next tick
  if((datapoint->flags & PUBLISH_VALUE) == 0)
    {
    datapoint->next_publish_tick = 1;
    datapoint->flags |= PUBLISH_VALUE;
    }

  // see if we do a loopback on this one
  if((datapoint->flags & PUBLISH_LOOPBACK) != 0)
    push_back(&can_rx_queue, &datapoint->published_value, INDEFINITE_WAIT);
  }

short get_datapoint_short(uint16_t id)
  {
  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return 0;
  
  return get_param_short(&datapoint->published_value);
  }

float get_datapoint_float(uint16_t id)
  {
  published_datapoint_t *datapoint = find_datapoint(id);
  if(datapoint == 0)
    return 0;
  return get_param_float(&datapoint->published_value);
  }

void unpublish(published_datapoint_t *datapoint)
  {
  datapoint->flags &= ~PUBLISH_VALUE;
  }

static void publish_task(void *parg)
  {
  while(true)
    {
    uint8_t i;
    published_datapoint_t *datapoint;

    wait(&can_publisher_semp, 1);
    
    if(published_datapoints == 0)
      continue;

    // do each datapoint we have registered
    for(i = 0; i < num_datapoints; i++)
      {
      datapoint = published_datapoints + i;

      if(datapoint->flags & PUBLISH_VALUE)
        {
        if(--datapoint->next_publish_tick == 0)
          {
          datapoint->next_publish_tick = datapoint->publish_frequency;

          // send the message to the can queue
          publish(&datapoint->published_value);
          }
        }
      }
    }
  }

inline uint16_t make_txbuffer(uint8_t byte_0, uint8_t byte_1)
  {
  return (((uint16_t) byte_1) << 8) | byte_0;
  }

/**
 * Get the next buffer to send a can message to
 * @return -1 if no free buffers
 */
static int next_free_tx_buffer()
  {
  if(C1TR67CONbits.TXREQ7 == 0)
    return 7;

  if(C1TR67CONbits.TXREQ6 == 0)
    return 6;

  if(C1TR45CONbits.TXREQ5 == 0)
    return 5;

  if(C1TR45CONbits.TXREQ4 == 0)
    return 4;

  if(C1TR23CONbits.TXREQ3 == 0)
    return 3;

  if(C1TR23CONbits.TXREQ2 == 0)
    return 2;

  if(C1TR01CONbits.TXREQ1 == 0)
    return 1;

  if(C1TR01CONbits.TXREQ0 == 0)
    return 0;

  return -1;
  }

/**
 * Worker task to queue messages
 * @param parg
 */
void can_tx_task(void *parg)
  {
  can_tx_event.flags |= EVENT_AUTORESET;
  while(true)
    {
    int next_buffer;
    
    can_msg_t msg;

    // spin till a buffer is free
    while((next_buffer = next_free_tx_buffer()) == -1)
      wait_for_event(&can_tx_event, INDEFINITE_WAIT);

    // block till a message is ready
    pop_front(&can_tx_queue, &msg, INDEFINITE_WAIT);
    int length = msg.flags & DLC_MASK;
    int priority;

    if(msg.id < 128)
      priority = 3;
    else if(msg.id < 300)
      priority = 2;
    else if(msg.id < 1800)
      priority = 1;
    else
      priority = 0;

    ecan_msg_buffer[next_buffer][0] = msg.id << 2;
    ecan_msg_buffer[next_buffer][1] = 0;
    ecan_msg_buffer[next_buffer][2] = (length & 0x000f);
    ecan_msg_buffer[next_buffer][3] = make_txbuffer(msg.msg.canas.node_id, msg.msg.canas.data_type);
    ecan_msg_buffer[next_buffer][4] = make_txbuffer(msg.msg.canas.service_code, msg.msg.canas.message_code);
    ecan_msg_buffer[next_buffer][5] = make_txbuffer(msg.msg.canas.data[0], msg.msg.canas.data[1]);
    ecan_msg_buffer[next_buffer][6] = make_txbuffer(msg.msg.canas.data[2], msg.msg.canas.data[3]);
    ecan_msg_buffer[next_buffer][7] = 0;

    switch(next_buffer)
      {
      case 0:
        C1TR01CONbits.TX0PRI = priority;
        C1TR01CONbits.RTREN0 = 0;
        C1TR01CONbits.TXREQ0 = 1;
        break;
      case 1:
        C1TR01CONbits.TX1PRI = priority;
        C1TR01CONbits.RTREN1 = 0;
        C1TR01CONbits.TXREQ1 = 1;
        break;
      case 2:
        C1TR23CONbits.TX2PRI = priority;
        C1TR23CONbits.RTREN2 = 0;
        C1TR23CONbits.TXREQ2 = 1;
        break;
      case 3:
        C1TR23CONbits.TX3PRI = priority;
        C1TR23CONbits.RTREN3 = 0;
        C1TR23CONbits.TXREQ3 = 1;
        break;
      case 4:
        C1TR45CONbits.TX4PRI = priority;
        C1TR45CONbits.RTREN4 = 0;
        C1TR45CONbits.TXREQ4 = 1;
        break;
      case 5:
        C1TR45CONbits.TX5PRI = priority;
        C1TR45CONbits.RTREN5 = 0;
        C1TR45CONbits.TXREQ5 = 1;
        break;
      case 6:
        C1TR67CONbits.TX6PRI = priority;
        C1TR67CONbits.RTREN6 = 0;
        C1TR67CONbits.TXREQ6 = 1;
        break;
      case 7:
        C1TR67CONbits.TX7PRI = priority;
        C1TR67CONbits.RTREN7 = 0;
        C1TR67CONbits.TXREQ7 = 1;
        break;
      }
    }
  }

void publish_raw(can_msg_t *msg)
  {
  push_back(&can_tx_queue, msg, INDEFINITE_WAIT);
  }

static can_parameter_type_1_t node_id;

void publish_reply(can_msg_t *msg)
  {
  msg->msg.canas.node_id = node_id.value.SHORT;
  publish_raw(msg);
  }

void publish(can_msg_t *msg)
  {
  msg->msg.canas.message_code = ++message_code;
  publish_reply(msg);
  }

static message_listener_t *listeners = 0;

void subscribe(message_listener_t *handler)
  {
  handler->next = listeners;
  listeners = handler;
  }

static uint8_t ids_reply[4];

void publish_ids(const can_msg_t *service_msg)
  {
  can_msg_t msg;

  uint16_t my_id = node_id.value.SHORT;

  // send our node id
  ids_reply[2] = (uint8_t) (my_id >> 8);
  ids_reply[3] = (uint8_t) my_id;

  publish(create_can_msg_uchar4(&msg, service_msg->id + 1, 0, ids_reply));
  }

void can_rx_task(void *parg)
  {
  while(true)
    {
    can_msg_t msg;
    pop_front(&can_rx_queue, &msg, INDEFINITE_WAIT);
    message_listener_t *subscriber;

    for(subscriber = listeners; subscriber != 0; subscriber = subscriber->next)
      (subscriber->handler)(&msg);

    // handle the builtin services next
    switch(msg.id)
      {
      case node_service_channel_0:
      case node_service_channel_1:
      case node_service_channel_2:
      case node_service_channel_3:
      case node_service_channel_4:
      case node_service_channel_5:
      case node_service_channel_6:
      case node_service_channel_7:
      case node_service_channel_8:
      case node_service_channel_9:
      case node_service_channel_10:
      case node_service_channel_11:
      case node_service_channel_12:
      case node_service_channel_13:
      case node_service_channel_14:
      case node_service_channel_15:
      case node_service_channel_16:
      case node_service_channel_17:
      case node_service_channel_18:
      case node_service_channel_19:
      case node_service_channel_20:
      case node_service_channel_21:
      case node_service_channel_22:
      case node_service_channel_23:
      case node_service_channel_24:
      case node_service_channel_25:
      case node_service_channel_26:
      case node_service_channel_27:
      case node_service_channel_28:
      case node_service_channel_29:
      case node_service_channel_30:
      case node_service_channel_31:
      case node_service_channel_32:
      case node_service_channel_33:
      case node_service_channel_34:
      case node_service_channel_35:
        if((msg.msg.canas.node_id == 0 && msg.msg.canas.service_code == id_ids_service) || // accept broadcast messages, for ID only
           msg.msg.canas.node_id == node_id.value.SHORT) // accept addressed messages
          {
          switch(msg.msg.canas.service_code)
            {
            case id_ids_service:
              publish_ids(&msg);
              break;
            case id_nss_service:
              break;
            case id_dds_service:
              process_dds(&msg);
              break;
            case id_dus_service:
              process_dus(&msg);
              break;
            case id_scs_service:
              break;
            case id_tis_service:
              break;
            case id_fps_service:
              break;
            case id_sts_service:
              break;
            case id_fss_service:
              break;
            case id_tcs_service:
              break;
            case id_bss_service:
              break;
            case id_nis_service:
              process_nis(&msg);
              break;
            case id_mis_service:
              process_mis(&msg);
              break;
            case id_mcs_service:
              break;
            case id_css_service:
              process_css(&msg);
              break;
            case id_dss_service:
              break;
            }
          }
        break;
      }
    }
  }

static bool init_done = false;

bool can_config(uint16_t memid, semaphore_t *worker_task)
  {
  if(!init_done)
    {
    read_parameter_data(memid, sizeof (can_parameter_type_1_t), &node_id, 0, worker_task);
    init_done = true;
    }

  return true;
  }

void can_init(uint8_t hardware_revision,
              uint8_t software_revision,
              can_msg_t *tx_buffer,
              uint16_t tx_length,
              can_msg_t *rx_buffer,
              uint16_t rx_length,
              uint16_t *tx_stack,
              uint16_t tx_stack_length,
              int8_t *tx_worker_id,
              uint16_t *rx_stack,
              uint16_t rx_stack_length,
              int8_t *rx_worker_id,
              uint16_t *publisher_stack,
              uint16_t publisher_stack_length,
              int8_t *publisher_worker_id)
  {
  int8_t task_id;
  ids_reply[0] = hardware_revision;
  ids_reply[1] = software_revision;

  init_deque(&can_tx_queue, sizeof(can_msg_t), tx_buffer, tx_length);
  init_deque(&can_rx_queue, sizeof(can_msg_t), rx_buffer, rx_length);

  // configure dma channel 0 & 1 to service the can
  DMA0CONbits.SIZE = 0x0;
  DMA0CONbits.DIR = 0x1;
  DMA0CONbits.AMODE = 0x2;
  DMA0CONbits.MODE = 0x0;
  DMA0REQ = 0x46;
  DMA0CNT = 7;
  DMA0PAD = (volatile unsigned int) &C1TXD;
  DMA0STAL = (uint16_t) &ecan_msg_buffer;
  DMA0STAH = 0;
  IEC0bits.DMA0IE = 1;
  DMA0CONbits.CHEN = 0x1;
  

  // assign dma1 for receive
  DMA1CONbits.SIZE = 0x0;
  DMA1CONbits.DIR = 0x0;
  DMA1CONbits.AMODE = 0x2;
  DMA1CONbits.MODE = 0x0;
  DMA1REQ = 0x22;
  DMA1CNT = 7;
  DMA1PAD = (volatile unsigned int) &C1RXD;
  DMA1STAL = (uint16_t) &ecan_msg_buffer[8];
  DMA1STAH = 0;
  IEC0bits.DMA1IE = 1;
  DMA1CONbits.CHEN = 0x1;

  /* Set the ECAN module for Configuration Mode before writing into the Baud
  Rate Control Registers */
  C1CTRL1bits.REQOP = 4;
  
  while(C1CTRL1bits.OPMODE != 4);
  
  C1CTRL1bits.WIN = 0;

  // set up the can bus interface
  C1CTRL1bits.CANCKS = 0; // Select can clock == Fp (70mhz)
  C1CFG1bits.SJW = 00; //Synchronized jump width time is 1 x TQ when SJW is equal to 00
  C1CFG1bits.BRP = BRP_VAL; //((FCY/(2*NTQ*BITRATE))-1)

  C1CFG2bits.PRSEG = PRSEG_VAL; // Preamble
  C1CFG2bits.SEG1PH = SEG1PH_VAL;
  C1CFG2bits.SEG2PH = SEG2PH_VAL;
  C1CFG2bits.SEG2PHTS = 1; // use PS2 for calcs

  C1FCTRLbits.DMABS = 4; // test only 8 buffers... 6; // 32 buffers in device ram starting
  C1FCTRLbits.FSA = 8; // fifo starts at buffer 8
  
  C1TR01CONbits.TXEN0 = 1;
  C1TR01CONbits.TXEN1 = 1;
  C1TR23CONbits.TXEN2 = 1;
  C1TR23CONbits.TXEN3 = 1;
  C1TR45CONbits.TXEN4 = 1;
  C1TR45CONbits.TXEN5 = 1;
  C1TR67CONbits.TXEN6 = 1;
  C1TR67CONbits.TXEN7 = 1;
 
  
  IEC2bits.C1IE = 1;
  
  // select the buffer bits
  C1CTRL1bits.WIN = 1;

  // set mask to 1 to not mask any id's
  C1RXM0SID = 0x0000;
  C1RXF0SID = 0;
  
  C1BUFPNT1bits.F0BP = 15;
  
  /* Acceptance Filter 0 to check for Standard Identifier */
  C1RXM0SIDbits.MIDE = 0x1;
  C1RXF0SIDbits.EXIDE= 0x0;
  
  
  C1CTRL1bits.WIN = 0;

  //Interrupt Section of CAN Peripheral
  //C1INTF = 0; //Reset all The CAN Interrupts
  // enable the rx and tx interrupts
  //C1INTE = 0x0003;
  C1INTEbits.RBIE = 1;
  C1INTEbits.TBIE = 1;
  C1INTEbits.ERRIE = 1;
  
  C1CTRL1bits.REQOP = 0;

  while(C1CTRL1bits.OPMODE != 0); // wait for ready

  resume(task_id = create_task("CAN_TX", tx_stack, tx_stack_length, can_tx_task, 0, NORMAL_PRIORITY));
  if(tx_worker_id != 0)
    *tx_worker_id = task_id;
  
  resume(task_id = create_task("CAN_RX", rx_stack, rx_stack_length, can_rx_task, 0, NORMAL_PRIORITY + 1));
  if(rx_worker_id != 0)
    *rx_worker_id = task_id;

  // create the publisher schedule
  resume(task_id = create_task("CAN_PUBLISHER", publisher_stack, publisher_stack_length, publish_task, 0, NORMAL_PRIORITY));
  if(publisher_worker_id != 0)
    *publisher_worker_id = task_id;
  }

void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
  {
  uint16_t icode;
  while((icode = C1VEC) != 64)
    {
    int index = icode;
    if(index == 65)
      {
      // ERROR interrupt
      C1INTFbits.ERRIF = 0;       // clear the error.
      if(C1INTFbits.TXBO == 1)
        {
        // tx buffer in bus off
        }
      if(C1INTFbits.RBOVIF == 1)
        {
        // rx overflow
        }
      }
    else if(icode >= 0 && icode < 8)
      {
      C1INTFbits.TBIF = 0; // clear the flag
      set_event_from_isr(&can_tx_event); // signal an interrupt
      }
    else if(icode >= 8 && icode < 32)
      {
      uint16_t *ptr;
      uint16_t value;
      can_msg_t frame;
      // process message from buffer
      index = C1FIFObits.FNRB;

      frame.id = ecan_msg_buffer[index][0] >> 2;
      frame.flags = ecan_msg_buffer[index][2] & 0x000f;

      ptr = &ecan_msg_buffer[index][3];

      value = *ptr;
      ptr++;

      frame.msg.canas.node_id = (uint8_t) value;
      frame.msg.canas.data_type = (uint8_t) (value >> 8);

      value = *ptr;
      ptr++;
      frame.msg.canas.service_code = (uint8_t) value;
      frame.msg.canas.message_code = (uint8_t) (value >> 8);

      value = *ptr;
      ptr++;
      frame.msg.canas.data[0] = (uint8_t) value;
      frame.msg.canas.data[1] = (uint8_t) (value >> 8);

      value = *ptr;
      frame.msg.canas.data[2] = (uint8_t) value;
      frame.msg.canas.data[3] = (uint8_t) (value >> 8);
      
      if(index < 16)
        C1RXFUL1 &= ~(1 << index);
      else
        C1RXFUL2 &= ~(1 << (index -16));

      push_back_from_isr(&can_rx_queue, &frame);

      C1INTFbits.RBIF = 0;

      if(index < 16)
        C1RXFUL1 &= ~(1 << index);
      else
        C1RXFUL2 &= ~(1 << (index - 16));
      }
    }
  
  // clear the IRQ
  IFS2bits.C1IF = 0;
  }

void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
  {
  IFS0bits.DMA0IF = 0;
  }

void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
  {
  IFS0bits.DMA1IF = 0;
  }

can_msg_t *create_can_msg_float(can_msg_t *msg, uint16_t message_id, uint8_t service_code, float value)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_FLOAT;
  msg->msg.canas.service_code = service_code;

  unsigned long *ulvalue = (unsigned long *) &value;
  msg->msg.canas.data[0] = (uint8_t) (*ulvalue >> 24);
  msg->msg.canas.data[1] = (uint8_t) (*ulvalue >> 16);
  msg->msg.canas.data[2] = (uint8_t) (*ulvalue >> 0);
  msg->msg.canas.data[3] = (uint8_t) * ulvalue;

  return msg;
  }

can_msg_t *create_can_msg_short(can_msg_t *msg, uint16_t message_id, uint8_t service_code, short data)
  {
  msg->flags = 6;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_SHORT;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = (uint8_t) (data >> 8);
  msg->msg.canas.data[1] = (uint8_t) data;

  return msg;
  }

can_msg_t *create_can_msg_long(can_msg_t *msg, uint16_t message_id, uint8_t service_code, long data)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_LONG;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = (uint8_t) (data >> 24);
  msg->msg.canas.data[1] = (uint8_t) (data >> 16);
  msg->msg.canas.data[2] = (uint8_t) (data >> 8);
  msg->msg.canas.data[3] = (uint8_t) data;

  return msg;
  }

can_msg_t *create_can_msg_chksum(can_msg_t *msg, uint16_t message_id, uint8_t service_code, uint32_t data)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_CHKSUM;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = (uint8_t) (data >> 24);
  msg->msg.canas.data[1] = (uint8_t) (data >> 16);
  msg->msg.canas.data[2] = (uint8_t) (data >> 8);
  msg->msg.canas.data[3] = (uint8_t) data;

  return msg;
  }

can_msg_t *create_can_msg_uchar(can_msg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t value)
  {
  msg->flags = 5;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_UCHAR;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = value;

  return msg;
  }

can_msg_t *create_can_msg_uchar2(can_msg_t *msg, uint16_t message_id, uint8_t service_code, const uint8_t *value)
  {
  msg->flags = 6;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_UCHAR2;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = value[0];
  msg->msg.canas.data[1] = value[1];

  return msg;
  }

can_msg_t *create_can_msg_uchar3(can_msg_t *msg, uint16_t message_id, uint8_t service_code, const uint8_t *value)
  {
  msg->flags = 7;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_UCHAR3;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = value[0];
  msg->msg.canas.data[1] = value[1];
  msg->msg.canas.data[2] = value[2];

  return msg;
  }

can_msg_t *create_can_msg_uchar4(can_msg_t *msg, uint16_t message_id, uint8_t service_code, const uint8_t *value)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_UCHAR4;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = value[0];
  msg->msg.canas.data[1] = value[1];
  msg->msg.canas.data[2] = value[2];
  msg->msg.canas.data[3] = value[3];

  return msg;
  }

void send_param_float(float value, uint16_t id)
  {
  can_msg_t msg;
  publish(create_can_msg_float(&msg, id, 0, value));
  }

void send_param_short(short value, uint16_t id)
  {
  can_msg_t msg;
  publish(create_can_msg_short(&msg, id, 0, value));
  }

float get_param_float(const can_msg_t *msg)
  {
  unsigned long value = (((unsigned long) msg->msg.canas.data[0]) << 24) |
    (((unsigned long) msg->msg.canas.data[1]) << 16) |
    (((unsigned long) msg->msg.canas.data[2]) << 8) |
    ((unsigned long) msg->msg.canas.data[3]);
  return *(float *) (&value);
  }

short get_param_short(const can_msg_t *msg)
  {
  return (short) ((((unsigned short) msg->msg.canas.data[0]) << 8) | ((unsigned short) msg->msg.canas.data[1]));
  }

long get_param_long(const can_msg_t *msg)
  {
  return (long) (
                 (((unsigned long) msg->msg.canas.data[0]) << 24) |
                 (((unsigned long) msg->msg.canas.data[1]) << 16) |
                 (((unsigned long) msg->msg.canas.data[2]) << 8) |
                 ((unsigned long) msg->msg.canas.data[3]));
  }
