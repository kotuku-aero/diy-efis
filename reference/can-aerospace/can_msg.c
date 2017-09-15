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
#include "can_msg.h"

can_msg_t *create_can_msg_float(can_msg_t *msg, uint16_t message_id, uint8_t service_code, float value)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_FLOAT;
  msg->msg.canas.service_code = service_code;

  unsigned long *ulvalue = (unsigned long *)&value;
  msg->msg.canas.data[0] = (unsigned char)(*ulvalue >> 24);
  msg->msg.canas.data[1] = (unsigned char)(*ulvalue >> 16);
  msg->msg.canas.data[2] = (unsigned char)(*ulvalue >> 0);
  msg->msg.canas.data[3] = (unsigned char)*ulvalue;
  
  return msg;
  }
  
can_msg_t *create_can_msg_short(can_msg_t *msg, uint16_t message_id, uint8_t service_code, short data)
  {
  msg->flags = 6;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_SHORT;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = (uint8_t)(data >> 8);
  msg->msg.canas.data[1] = (uint8_t)data;
  
  return msg;
  }

can_msg_t *create_can_msg_long(can_msg_t *msg, uint16_t message_id, uint8_t service_code, long data)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_LONG;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = (uint8_t)(data >> 24);
  msg->msg.canas.data[1] = (uint8_t)(data >> 16);
  msg->msg.canas.data[2] = (uint8_t)(data >> 8);
  msg->msg.canas.data[3] = (uint8_t)data;

  return msg;
  }

can_msg_t *create_can_msg_chksum(can_msg_t *msg, uint16_t message_id, uint8_t service_code, uint32_t data)
  {
  msg->flags = 8;
  msg->id = message_id;
  msg->msg.canas.data_type = CANAS_DATATYPE_CHKSUM;
  msg->msg.canas.service_code = service_code;
  msg->msg.canas.data[0] = (uint8_t)(data >> 24);
  msg->msg.canas.data[1] = (uint8_t)(data >> 16);
  msg->msg.canas.data[2] = (uint8_t)(data >> 8);
  msg->msg.canas.data[3] = (uint8_t)data;

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


float get_param_float(const can_msg_t *msg)
  {
  unsigned long value = (((unsigned long) msg->msg.canas.data[0]) << 24) |
                        (((unsigned long) msg->msg.canas.data[1]) << 16) |
                        (((unsigned long) msg->msg.canas.data[2]) << 8) |
                        ((unsigned long) msg->msg.canas.data[3]);
  return *(float *)(&value);
  }

short get_param_short(const can_msg_t *msg)
  {
  return (short)((((unsigned short) msg->msg.canas.data[0]) << 8) | ((unsigned short) msg->msg.canas.data[1]));
  }

long get_param_long(const can_msg_t *msg)
  {
  return (long)(
    (((unsigned long) msg->msg.canas.data[0]) << 24) |
    (((unsigned long) msg->msg.canas.data[1]) << 16) | 
    (((unsigned long) msg->msg.canas.data[2]) << 8) | 
     ((unsigned long) msg->msg.canas.data[3]));
  }

