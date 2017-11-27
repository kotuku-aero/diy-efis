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
#include "neutron_cli_impl.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *script_key = "scripts";
static const char *event_key = "events";
static const char *script_name = "scr";
extern const char * get_full_path(memid_t key);

// events are stored in the key called events.
result_t script_action(cli_t *context)
  {
  result_t result;
  memid_t key;

  if(failed(result = reg_open_key(0, script_key, &key)))
    {
    if(result != e_path_not_found)
      return result;

    if(failed(result = reg_create_key(0, script_key, &key)))
      return result;

    memid_t events;
    // create the events key
    if(failed(result = reg_create_key(key, event_key, &events)))
      {
      // remove the new key
      reg_delete_key(key);
      return result;
      }
    }

  char new_prompt[MAX_PROMPT_LENGTH];
  snprintf(new_prompt, MAX_PROMPT_LENGTH, "%s: ", script_name);

  return cli_submode_enter(context, key, new_prompt);
  }

result_t script_exit_action(cli_t *context)
  {
  cli_submode_exit(context);

  return s_ok;
  }

result_t script_ls_name_action(cli_t *context, const char * path)
  {
  // see if a name given

  result_t result;
  memid_t child = 0;
  field_datatype dt = 0;
  char name[REG_NAME_MAX + 1];
  char buffer[32];

  if (path != 0 && strcmp(path, event_key)== 0)
    {
    memid_t key;

    if (failed(result = reg_open_key(get_context(context), event_key, &key)))
        return result;

    // enumerate the keys
    dt = field_key;
    char name[REG_NAME_MAX + 1];
    char event_fn[REG_STRING_MAX + 1];

    memid_t child = 0;

    while (succeeded(reg_enum_key(key, &dt, 0, 0, REG_NAME_MAX, name, &child)))
      {
      uint16_t can_id = strtoul(name, 0, 10);

      // see if it is an enum value
      const enum_t *enum_value;
      if (succeeded(find_enum_name(can_ids, can_id, &enum_value)))
        {
        stream_puts(context->cfg.console_out, enum_value->name);
        stream_puts(context->cfg.console_out, "\r\n");
        }
      else
        stream_printf(context->cfg.console_out, "%d\r\n", can_id);

      // now enumerate the strings in it
      memid_t handler = 0;
      dt = field_string;

      while (succeeded(reg_enum_key(child, &dt, 0, 0, REG_NAME_MAX, name, &handler)))
        {
        const char * str;
        if (succeeded(reg_get_string(child, name, event_fn, 0)))
          stream_printf(context->cfg.console_out, "  %s void %s(canmsg_t *);\r\n", name, event_fn);

        dt = field_string;
        }

      dt = field_key;
      }
    }

  dt = field_stream;
  while(succeeded(result = reg_enum_key(get_context(context), &dt, 0, 0, REG_NAME_MAX, name, &child)))
    {
    name[REG_NAME_MAX] = 0;

    bool matched = true;
    // see if there is a wildcard match
    if(path != 0)
      {
      uint16_t len = strlen(path);
      uint16_t i;
      uint16_t p;
      for(i = 0, p=0; p < len && i < REG_NAME_MAX; i++)
        {
        if(path[p] == '?')
          {
          p++;
          continue;
          }

        if(path[p] == '*')
          break;

        if(name[i] == 0)
          {
          if(path[p] != 0 &&
             path[p] != '*')
            matched = false;

          break;
          }

        if(name[i] != path[p])
          {
          matched = false;
          break;
          }
        }
      }

    if (matched)
      {
      memid_t key;

      // we have to stat the file
      handle_t hndl;
      if (failed(stream_open(get_context(context), name, &hndl)))
        continue;

      uint16_t length;
      stream_length(hndl, &length);
      stream_close(hndl);

      // format as a 6 byte buffer
      uint16_t len = 0;
      uint16_t div = 10000;
      bool leading_0 = true;
      while (div > 0)
        {
        uint16_t n = length / div;
        length -= n * div;
        if (n > 0 || !leading_0 || div == 1)
          {
          buffer[len++] = '0' + n;
          leading_0 = false;
          }
        div /= 10;

        if (length == 0)
          break;
        }
      while (len < 6)
        buffer[len++] = ' ';

      buffer[6] = 0;
      strcpy(buffer + 6, name);
      strcat(buffer, "\r\n");

      stream_puts(context->cfg.console_out, buffer);
      }

    dt = field_stream;
    }

  return s_ok;
  }

result_t script_rm_name_action(cli_t *context, const char * name)
  {
  result_t result;
  handle_t stream;

  // see if it exists
  if (failed(result = stream_open(get_context(context), name, &stream)))
    {
    if (result != e_path_not_found)
      return result;        // other error.

    stream_printf("Unknown stream %s\r\n", name);
    return result;
    }

  return stream_delete(stream);
  }

result_t script_create_name_action(cli_t *context, const char * name)
  {
  result_t result;
  handle_t stream;

  // see if it exists
  if (failed(result = stream_open(get_context(context), name, &stream)))
    {
    if (result != e_path_not_found)
      return result;        // other error.

    if (failed(result = stream_create(get_context(context), name, &stream)))
      return result;
    }

  char * full_path = (char *)neutron_malloc(256);
  full_path[0] = 0;
  if (failed(result = stream_path(stream, true, 256, full_path)))
    {
    stream_close(stream);
    neutron_free(full_path);
    return result;
    }

  result = edit_script(context, full_path, stream);

  neutron_free(full_path);

  return result;
  }

result_t script_edit_name_action(cli_t *context, const char * name)
  {
  result_t result;
  handle_t stream;

  if(failed(result = stream_open(get_context(context), name, &stream)))
    return result;

  char * full_path = (char*)neutron_malloc(256);
  if (failed(result = stream_path(stream, true, 256, full_path)))
    {
    stream_close(stream);
    neutron_free(full_path);
    return result;
    }

  result = edit_script(context, full_path, stream);

  neutron_free(full_path);

  return result;
  }

result_t script_cat_name_action(cli_t *context, const char * name)
  {
  result_t result;
  handle_t stream;

  if(failed(result = stream_open(get_context(context), name, &stream)))
    return result;

  stream_copy(stream, context->cfg.console_out);

  stream_close(stream);

  stream_puts(context->cfg.console_out, "\r\n");

  return s_ok;
  }

result_t script_add_id_name_msg_handler_action(cli_t *context, uint16_t event_id, const char * script_path, const char * event_handler)
  {
  memid_t key;
  result_t result;

  if (failed(result = reg_open_key(get_context(context), event_key, &key)))
    {
    if (result != e_path_not_found)
      return result;

    if (failed(result = reg_create_key(get_context(context), event_key, &key)))
      return result;
    }

  char key_name[32];

  snprintf(key_name, 32, "%d", event_id);

  // each event is a key, we add the string type and script path.
  memid_t event_key;
  if (failed(result = reg_open_key(key, key_name, &event_key)))
    {
    if (result != e_path_not_found ||
      failed(result = reg_create_key(key, key_name, &event_key)))
      {
      neutron_free(key_name);
      return result;
      }
    }

  neutron_free(key_name);

  // TODO: check the event handler exists in the parent directory

  // TODO: check the length of the strings....

  // we have an event handler, so let us set the prompts
  return reg_set_string(event_key, script_path, event_handler);
  }

result_t script_del_id_name_action(cli_t *context, uint16_t del_id, const char * script_name)
  {
  result_t result;
  memid_t key;

  if (failed(result = reg_open_key(get_context(context), event_key, &key)))
    return result;

  char id_str[32];
  snprintf(id_str, 32, "%d", del_id);

  result = reg_open_key(get_context(context), id_str, &key);
  neutron_free(id_str);

  if (failed(result))
    return result;

  return reg_delete_value(key, script_name);
  }
