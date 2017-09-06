#ifndef __neutron_cli_impl_h__
#define __neutron_cli_impl_h__

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "neutron_cli.h"

extern result_t open_key(memid_t current, const char *path, bool create, memid_t *memid);
extern string_t get_full_path(memid_t key);
extern result_t edit_script(cli_t *context, const char *title, handle_t stream);
extern result_t find_enum_name(const enum_t *enums, uint16_t value, const enum_t **name);
extern result_t show_value(handle_t dest, memid_t key, field_datatype type, const char *name, uint16_t *indent, const enum_t *lookup);
extern void do_indent(handle_t dest, uint16_t indent);
extern result_t create_can_msg(canmsg_t *msg, uint16_t can_id, uint16_t type, uint16_t session, string_t val1, string_t val2, string_t val3, string_t val4);

#endif
