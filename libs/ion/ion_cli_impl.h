#ifndef __ion_cli_impl_h__
#define __ion_cli_impl_h__

#include "ion_cli.h"

extern result_t ion_add_id_name_msg_handler_action(cli_t *context, uint16_t ion_add_id_, string_t ion_add_id_name_, string_t ion_add_id_name_msg_handler_);

extern result_t ion_del_id_name_action(cli_t *context, uint16_t ion_del_id_, string_t ion_del_id_name_);

extern result_t ion_cat_name_action(cli_t *context, string_t ion_cat_name_);

extern result_t ion_edit_name_action(cli_t *context, string_t ion_edit_name_);

extern result_t ion_create_name_action(cli_t *context, string_t ion_create_name_);

extern result_t ion_rm_name_action(cli_t *context, string_t ion_rm_name_);

extern result_t ion_ls_name_action(cli_t *context, string_t ion_ls_name_);

extern result_t ion_debug_name_action(cli_t *context, string_t ion_debug_name_);

extern result_t ion_exit_action(cli_t *context);

extern result_t ion_action(cli_t *context);

#endif
