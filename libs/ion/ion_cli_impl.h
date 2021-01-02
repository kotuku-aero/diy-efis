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
#ifndef __ion_cli_impl_h__
#define __ion_cli_impl_h__

#include "ion_cli.h"

extern result_t ion_add_id_name_msg_handler_action(cli_t *context, uint16_t ion_add_id_, const char * ion_add_id_name_, const char * ion_add_id_name_msg_handler_);

extern result_t ion_del_id_name_action(cli_t *context, uint16_t ion_del_id_, const char * ion_del_id_name_);

extern result_t ion_cat_name_action(cli_t *context, const char * ion_cat_name_);

extern result_t ion_edit_name_action(cli_t *context, const char * ion_edit_name_);

extern result_t ion_create_name_action(cli_t *context, const char * ion_create_name_);

extern result_t ion_rm_name_action(cli_t *context, const char * ion_rm_name_);

extern result_t ion_ls_name_action(cli_t *context, const char * ion_ls_name_);

extern result_t ion_debug_name_action(cli_t *context, const char * ion_debug_name_);

extern result_t ion_exit_action(cli_t *context);

extern result_t ion_action(cli_t *context);

#endif
