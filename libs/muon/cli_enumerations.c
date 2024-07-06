/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#include "neutron_cli.h"
#include "cli_enumerations.h"

const enum_t booleans[] = {
    { "false", 0 },
    { "true", 1 },
    { "0", 0 },
    { "1", 1 },
    { 0, 0 }
  };

const enum_t filter_types[] = {
    { "boxcar", 1 },
    { "fir", 3 },
    { "iir", 2 },
    { "none", 0 },
    { 0, 0 }
  };

const enum_t publish_types[] = {
    { "float", CANAS_DATATYPE_FLOAT },
    { "int16", CANAS_DATATYPE_INT16 },
    { "int32", CANAS_DATATYPE_INT32 },
    { "int8", CANAS_DATATYPE_INT8 },
    { "uint16", CANAS_DATATYPE_UINT16 },
    { "uint32", CANAS_DATATYPE_UINT32 },
    { "uint8", CANAS_DATATYPE_UINT8 },
    { 0, 0 }
  };

const enum_t can_types[] = {
    { "int8", CANAS_DATATYPE_INT8 },
    { "float", CANAS_DATATYPE_FLOAT },
    { "int32", CANAS_DATATYPE_INT32 },
    { "nodata", CANAS_DATATYPE_NODATA },
    { "int16", CANAS_DATATYPE_INT16 },
    { "uint8", CANAS_DATATYPE_UINT8 },
    { "uint32", CANAS_DATATYPE_UINT32 },
    { "uint16", CANAS_DATATYPE_UINT16 },
    { 0, 0}
  };

const enum_t sample_types[] = {
    { "avg", 4 },
    { "fifo", 0 },
    { "lifo", 1 },
    { "max", 3 },
    { "min", 2 },
    { 0, 0 }
  };

const enum_t alarm_types[] = {
    { "event", 1 },
    { "level", 0 },
    { 0, 0 }
  };
/*
#define CANFLYID(id, n, t, s) { #id , n },

const enum_t can_ids[] = {

#include "../../resources/CanFlyID.def"

    { 0,0 }
  };
*/