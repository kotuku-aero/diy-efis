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
#ifndef __publisher_h__
#define	__publisher_h__

#include <stdint.h>

#ifdef	__cplusplus
extern "C"
  {
#endif /* __cplusplus */

#include "../can-aerospace/can_msg.h"

extern void send_param_float(float value, uint16_t id);
extern void send_param_short(short value, uint16_t id);

// queue a can message to be sent.  Will update node_id and message_code to correct values
extern void publish(can_msg_t *);
extern void publish_raw(can_msg_t *);
// queue a message after setting the node_id
extern void publish_reply(can_msg_t *);

typedef void (*msg_hook_fn)(const can_msg_t *);

typedef struct message_listener_ {
  void *next;
  msg_hook_fn handler;
  } message_listener_t;

// register a call-back function to handle messages.
extern void subscribe(message_listener_t *handler);

// this group of functions defines an area to publish regular status updates
// the structure will be filled in with the current values
typedef struct _published_datapoint_t
  {
  uint16_t can_id;              // id to publish
  uint16_t publish_frequency;   // in number of ticks
  uint8_t flags;                // see publish/un-publish calls
  uint16_t next_publish_tick;   // will be set by scheduler
  can_msg_t published_value;
  } published_datapoint_t;

// if set the value will be looped back to the code.
// no check for circular dependencies
#define PUBLISH_LOOPBACK 0x02
// set when a publish call is made, may be cleared to stop publishing
#define PUBLISH_VALUE 0x80  
  
  extern void register_datapoints(published_datapoint_t *, uint16_t num_datapoints);
  /**
   * Update the datapoint with a short value
   * @param value  Value to publish
   * @param datapoint datapoint definition
   */
  extern void publish_short(short value, uint16_t id);
  /**
   * Update the datapoint with a float value
   * @param value value to publish
   * @param datapoint datapoint definition
   */
  extern void publish_float(float value, uint16_t id);
   /**
   * Update the datapoint with a unsigned 32 bit value
   * @param value  Value to publish
   * @param datapoint datapoint definition
   */
  extern void publish_uint32(uint32_t value, uint16_t id);
 
  /**
   * Return the value of a datapoint as a short
   * @param datapoint
   * @return 
   */
  extern short get_datapoint_short(uint16_t id);
  /**
   * Return the value of a datapoint as afloat
   * @param datapoint
   * @return 
   */
  extern float get_datapoint_float(uint16_t id);
/**
   * Stop publishing a datapoint
   * @param datapoint datapoint definition
   */
  extern void unpublish(published_datapoint_t *datapoint);


#ifdef	__cplusplus
  }
#endif /* __cplusplus */

#endif

