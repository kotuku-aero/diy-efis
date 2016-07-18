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
#ifndef PARAMETER_DEFINITIONS_H
#define	PARAMETER_DEFINITIONS_H

#include "data_types.h"
#include <stdint.h>

#ifdef	__cplusplus
extern "C"
  {
#endif

  /*    These are the definitions for the parameters that are supported on
   * the MIS, DUS, and DDS service
   */

  // parameter name, returned by NIS message
  typedef struct _mis_message_t {
    uint8_t memid;              // memory ID used to access this parameter
    uint8_t parameter_type;     // Type of parameter, see comments below
    uint8_t parameter_length;   // length of parameter sent
    uint8_t name_length;        // number of characters in name (1..16)
    char name[16];              // Name of the parameter
    } mis_message_t;
    
  // total is 20 bytes

  /*  Base structure for all parameters
   * 
   * A parameter can be up to 256 bytes long
   */
    
    typedef struct _can_parameter_definition_t {
      uint8_t data_type;              // type of data stored or published
      uint8_t reserved;               // not sent
      uint16_t length;                // sizeof(parameter_type_x)
      } can_parameter_definition_t;

  /*    Parameter Type 0
   * 
   * Used by analog channels in most cases
  * The DUS/DDS service allows for defining can channels.
  * a MEMID of 0..number of configurable channels
  *
  * So a request would be:
  * DDS -> nodeid, MEMID, service_code = 2, message_code = 8, source id = 0
  *
  * This will read the first identifier (8 records or 32 bytes)
  *
  * Depending on the module, there are a variable number of parameter definitions
   */
  typedef struct _can_parameter_type_0_t {
    can_parameter_definition_t hdr;
    uint16_t can_id;                  // id used to identify this parameter
    uint16_t publish_rate;            // publish rate.  Assuming a 4khz sample time, is count of 250 microseconds
    float scale;                      // trim value to scale result by
    float offset;                     // offset value for the parameter
    } can_parameter_type_0_t;

    /*    Parameter Type 1
     * 
     * Used by modules that store settings.  
     * 
     * NOTE: only the actual length will be sent in the dds/dus messages
     * however numblks is set to 64 assuming a 256 byte parameter read
     * when the first dus is sent
     */
    typedef struct _can_parameter_type_1_t {
      can_parameter_definition_t hdr;
      union {
        int16_t SHORT;                // a short value
        uint16_t USHORT;              // unsigned short value
        int32_t LONG;                 // long value
        uint32_t ULONG;               // unsigned long value
        float FLOAT;                  // a floating point value
        xyz_t XYZ;                    // a series of 3 axes
        matrix_t MATRIX;              // a matrix of floating point values
        char STRING[1];               // variable length string
        } value;
      } can_parameter_type_1_t;
      
#define SIZE_SHORT_SETTING (sizeof(can_parameter_definition_t) + sizeof(int16_t))
#define SIZE_USHORT_SETTING (sizeof(can_parameter_definition_t) + sizeof(uint16_t))
#define SIZE_LONG_SETTING (sizeof(can_parameter_definition_t) + sizeof(int32_t))
#define SIZE_ULONG_SETTING (sizeof(can_parameter_definition_t) + sizeof(uint32_t))
#define SIZE_FLOAT_SETTING (sizeof(can_parameter_definition_t) + sizeof(float))
#define SIZE_MATRIX_SETTING (sizeof(can_parameter_definition_t) + sizeof(matrix_t))
#define SIZE_XYZ_SETTING (sizeof(can_parameter_definition_t) + sizeof(xyz_t))
   
// these are the hdr.datatype values for a type 1 parameter
typedef enum {
  p1t_short,
  p1t_ushort,
  p1t_long,
  p1t_ulong,
  p1t_float,
  p1t_xyz,
  p1t_matrix,
  p1t_string
  } param_type1_datatype;
      
extern uint8_t build_parameter1_short(can_parameter_type_1_t *param, short value);
extern uint8_t build_parameter1_uint16(can_parameter_type_1_t *param, uint16_t value);
extern uint8_t build_parameter1_float(can_parameter_type_1_t *param, float value);
extern uint8_t build_parameter1_matrix(can_parameter_type_1_t *param, const matrix_t *value);

extern uint32_t to_big_endian32(uint32_t);
extern uint16_t to_big_endian16(uint16_t);
   
#ifdef	__cplusplus
  }
#endif

#endif	/* PARAMETER_DEFINITIONS_H */

