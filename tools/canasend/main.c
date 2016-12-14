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
/*
 * cansend.c - simple command line tool to send CAN Aerospace -frames via CAN_RAW sockets
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "../can-aerospace/can_msg.h"

typedef int (*parse_option_fn)(int opt, int argc, char **argv, int num_opts, uint8_t *dst);

typedef struct _can_datatype_t {
  const char *name;
  int num_args;
  canaerospace_data_type data_type;
  parse_option_fn parse_arg;
} can_datatype_t;

static int parse_float(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  float value = (float) atof(argv[opt]);

  uint8_t *pvalue = (uint8_t *)&value;

  dst[0] = pvalue[3];
  dst[1] = pvalue[2];
  dst[2] = pvalue[1];
  dst[3] = pvalue[0];

  return 4;
  }

static int parse_long(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  int32_t value = atol(argv[opt]);

  uint8_t *pvalue = (uint8_t *)&value;

  dst[0] = pvalue[3];
  dst[1] = pvalue[2];
  dst[2] = pvalue[1];
  dst[3] = pvalue[0];

  return 4;
  }

static int parse_ulong(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  uint32_t value = atol(argv[opt]);

  uint8_t *pvalue = (uint8_t *)&value;

  dst[0] = pvalue[3];
  dst[1] = pvalue[2];
  dst[2] = pvalue[1];
  dst[3] = pvalue[0];

  return 4;
  }

static int parse_short(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  int16_t value = (short)atoi(argv[opt]);

  uint8_t *pvalue = (uint8_t *)&value;

  dst[0] = pvalue[1];
  dst[1] = pvalue[0];

  return 2;
  }

static int parse_ushort(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  uint16_t value = (short)atoi(argv[opt]);

  uint8_t *pvalue = (uint8_t *)&value;

  dst[0] = pvalue[1];
  dst[1] = pvalue[0];

  return 2;
  }

static int parse_char(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  // TODO: handle hex!
  *dst = (uint8_t) argv[opt];
  return 1;
  }

static int parse_udef(int opt, int argc, char **argv, int num_opts, uint8_t *dst)
  {
  // TODO:
  return 0;
  }

static const can_datatype_t data_types[] =
    {
        { "NODATA", 0, CANAS_DATATYPE_NODATA, 0 },
        { "ERROR", 0, CANAS_DATATYPE_ERROR, 0 },
        { "FLOAT", 1, CANAS_DATATYPE_FLOAT, parse_float },
        { "LONG", 1, CANAS_DATATYPE_LONG, parse_long },
        { "ULONG", 1, CANAS_DATATYPE_ULONG, parse_ulong },
        { "SHORT", 1, CANAS_DATATYPE_SHORT, parse_short },
        { "USHORT", 1, CANAS_DATATYPE_USHORT, parse_ushort },
        { "CHAR", 1, CANAS_DATATYPE_CHAR, parse_char },
        { "UCHAR", 1, CANAS_DATATYPE_UCHAR, parse_char },
        { "SHOR2T", 2, CANAS_DATATYPE_SHORT2, parse_short },
        { "USHORT2", 2, CANAS_DATATYPE_USHORT2, parse_short },
        { "CHAR4", 4, CANAS_DATATYPE_CHAR4, parse_char },
        { "UCHAR4", 4, CANAS_DATATYPE_UCHAR4, parse_char },
        { "CHAR2", 2, CANAS_DATATYPE_CHAR2, parse_char },
        { "UCHAR2", 2, CANAS_DATATYPE_UCHAR2, parse_char },
        { "MEMID", 1, CANAS_DATATYPE_MEMID, parse_ulong },
        { "CHKSUM", 1, CANAS_DATATYPE_CHKSUM, parse_ulong },
        { "CHAR3", 3, CANAS_DATATYPE_CHAR3, parse_char },
        { "UCHAR3", 3, CANAS_DATATYPE_UCHAR3, parse_char },
        { "USER", 1, CANAS_DATATYPE_UDEF_BEGIN_, parse_udef },
        { 0, 0, 0, 0 }
     };

int main(int argc, char **argv)
  {
  const can_datatype_t *dt = data_types;
  canas_msg_t msg;
  int length = 0;
  int args;
  uint8_t *dp = msg.data;
  int opt = 3;
  int canid;

  /* check command line options */
  if(argc < 3)
    {
    fprintf(stderr, "Usage: %s <CanID> <DataType> <value> [value].\n", argv[0]);
    return 1;
    }

  canid = atoi(argv[1]);

  while(dt->name != 0)
    {
    if(strcasecmp(dt->name, argv[2])== 0)
      break;

    dt++;
    }

  if(dt->name == 0)
    {
    fprintf(stderr, "Error: %s is not a valid data type\n", argv[1]);
    return 1;
    }

  if(argc - dt->num_args < 3)
    {
    fprintf(stderr, "Error: %s requires %d arguments\n", argv[1], dt->num_args);
    return 1;
    }

  for(args = dt->num_args; args > 0; args--)
    {
    length += (*dt->parse_arg)(opt, argc, argv, dt->num_args, dp);
    opt++;
    }

  msg.node_id = 0;
  msg.service_code = 0;
  msg.message_code = 0;
  msg.data_type = dt->data_type;

  // send the data
  printf("%03.3x#%02.2x.%02.2x.%02.2x.%02.2x", canid, msg.node_id, msg.data_type, msg.message_code, msg.service_code);
  for(args = 0; args < length; args++)
    printf(".%02.2x", msg.data[args]);

  return 0;
  }
