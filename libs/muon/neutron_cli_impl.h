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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __neutron_cli_impl_h__
#define __neutron_cli_impl_h__

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "neutron_cli.h"

extern result_t open_key(memid_t current, const char *path, bool create, memid_t *memid);
extern const char * get_full_path(memid_t key);
extern result_t edit_script(cli_t *context, const char *title, handle_t stream);
extern result_t find_enum_name(const enum_t *enums, uint16_t value, const enum_t **name);
extern result_t show_value(handle_t dest, memid_t key, field_datatype type, const char *name, uint16_t *indent, const enum_t *lookup);
extern void do_indent(handle_t dest, uint16_t indent);
extern result_t create_can_msg(canmsg_t *msg, uint16_t can_id, uint16_t type, uint16_t session, const char * val1, const char * val2, const char * val3, const char * val4);

#endif
