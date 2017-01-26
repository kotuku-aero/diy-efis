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
#ifndef CAN_MSG_H
#define	CAN_MSG_H

#ifdef	__cplusplus
extern "C"
  {
#endif

#include <stdint.h>
#include "can_aerospace.h"


// the can driver is used to queue messages for the can bus.  Since
// the worker processes tend to run at fixed intervals, we need a seperate
// thread to process sending data to the can bus.

typedef enum
{
  CANAS_DATATYPE_NODATA,
  CANAS_DATATYPE_ERROR,

  CANAS_DATATYPE_FLOAT,

  CANAS_DATATYPE_LONG,
  CANAS_DATATYPE_ULONG,
  CANAS_DATATYPE_BLONG,

  CANAS_DATATYPE_SHORT,
  CANAS_DATATYPE_USHORT,
  CANAS_DATATYPE_BSHORT,

  CANAS_DATATYPE_CHAR,
  CANAS_DATATYPE_UCHAR,
  CANAS_DATATYPE_BCHAR,

  CANAS_DATATYPE_SHORT2,
  CANAS_DATATYPE_USHORT2,
  CANAS_DATATYPE_BSHORT2,

  CANAS_DATATYPE_CHAR4,
  CANAS_DATATYPE_UCHAR4,
  CANAS_DATATYPE_BCHAR4,

  CANAS_DATATYPE_CHAR2,
  CANAS_DATATYPE_UCHAR2,
  CANAS_DATATYPE_BCHAR2,

  CANAS_DATATYPE_MEMID,
  CANAS_DATATYPE_CHKSUM,

  CANAS_DATATYPE_ACHAR,
  CANAS_DATATYPE_ACHAR2,
  CANAS_DATATYPE_ACHAR4,

  CANAS_DATATYPE_CHAR3,
  CANAS_DATATYPE_UCHAR3,
  CANAS_DATATYPE_BCHAR3,
  CANAS_DATATYPE_ACHAR3,

  CANAS_DATATYPE_DOUBLEH,
  CANAS_DATATYPE_DOUBLEL,

  CANAS_DATATYPE_RESVD_BEGIN_,
  CANAS_DATATYPE_RESVD_END_  = 99,

  CANAS_DATATYPE_UDEF_BEGIN_ = 100,
  CANAS_DATATYPE_UDEF_END_   = 255,

  CANAS_DATATYPE_ALL_END_    = 255
  } canaerospace_data_type;
  
typedef struct _canas_msg_t
  {
  uint8_t node_id;
  uint8_t data_type;              // is canaerospace_data_type or custom
  uint8_t service_code;
  uint8_t message_code;           // incremented as each message is published
  uint8_t data[4];
  } canas_msg_t;

typedef struct
{
  union {
    union {
      length : 3;
      reply : 1;
      loopback : 1;
      id: 11;
      };
    uint16_t flags;
    };
  
  // following is 8 bytes
  union {
    canas_msg_t canas;
    uint8_t raw[8];
    };
} can_msg_t;

#define loopback_msg(id) (id | 0x0800)
#define reply_msg(id) (id | 0x1000)

// function to build a message.
extern can_msg_t *create_can_msg_short(can_msg_t *, uint16_t message_id, uint8_t service_code, short data);
extern can_msg_t *create_can_msg_long(can_msg_t *, uint16_t message_id, uint8_t service_code, long data);
extern can_msg_t *create_can_msg_float(can_msg_t *, uint16_t message_id, uint8_t service_code, float data);
extern can_msg_t *create_can_msg_uchar(can_msg_t *, uint16_t message_id, uint8_t service_code, uint8_t data);
extern can_msg_t *create_can_msg_uchar2(can_msg_t *, uint16_t message_id, uint8_t service_code, const uint8_t *data);
extern can_msg_t *create_can_msg_uchar3(can_msg_t *, uint16_t message_id, uint8_t service_code, const uint8_t *data);
extern can_msg_t *create_can_msg_uchar4(can_msg_t *, uint16_t message_id, uint8_t service_code, const uint8_t *data);
extern can_msg_t *create_can_msg_chksum(can_msg_t *, uint16_t message_id, uint8_t service_code, uint32_t data);

extern float get_param_float(const can_msg_t *msg);
extern short get_param_short(const can_msg_t *msg);
extern long get_param_long(const can_msg_t *msg);

#ifdef	__cplusplus
  }
#endif

#endif	/* CAN_MSG_H */

