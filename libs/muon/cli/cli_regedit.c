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

extern const char *node_name;
static const char *cr_lf = "\r\n";

static void make_prompt(char *prompt, size_t maxlen, const char *dirname)
  {
  // the prompt is either the full path or ...\<dirname>\<dirname>
  size_t dirlen = strlen(dirname);

  *prompt = 0;
  const char *dirp = dirname;

  if (dirlen > (maxlen - 2))
    {
    // this is the case where we have to prepend an elipsis
    strcpy(prompt, ".../");
    maxlen -= 4;

    // work backward on the dirname looking for the first directry marker
    dirp += dirlen - maxlen;

    while (*dirp != '/' && *dirp != 0)
      dirp++;
    }
  else
    strcpy(prompt, "/");

  strncat(prompt, dirp, maxlen);

  neutron_free((void *)dirname);
  }

static void show_key(handle_t dest, memid_t key, bool full_path, bool recursive, uint16_t *indent)
  {
  char name[REG_NAME_MAX + 1];
  const char * path;
  if(full_path)
    path = get_full_path(key);
  else
    {
    reg_query_memid(key, 0, name, 0, 0);
    path = (char *)neutron_malloc(strlen(name)+1);
    strcpy((char *)path, name);
    }

  field_datatype type = 0;
  // must be 0 on first call
  memid_t child = 0;
  result_t result;

  while(succeeded(result = reg_enum_key(key, &type, 0, 0, REG_NAME_MAX + 1, name, &child)) && child != 0)
    {
    if (type > field_key)
      show_value(dest, key, type, name, indent, 0);

    // field_datatype has the field type, name is the child name
    type = 0;
    }

  type = field_key;
  child = 0;

  while(succeeded(result = reg_enum_key(key, &type, 0, 0, REG_NAME_MAX + 1, name, &child)))
    {
    do_indent(dest, *indent);
    stream_printf(dest, "dir    %s\r\n", name);
    *indent += 2;
    if(recursive)
      show_key(dest, child, false, true, indent);
    *indent -= 2;

    type = field_key;
    }

  }

result_t action(cli_t *context)
  {
  return s_ok;
  }

result_t uint16_name_value_action(cli_t *context, const char * name_, uint16_t value_)
  {
  if(name_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_uint16(get_context(context), name_, value_);
  }

result_t int16_name_value_action(cli_t *context, const char * name_, int16_t value_)
  {
  if(name_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_int16(get_context(context), name_, value_);
   }

result_t uint32_name_value_action(cli_t *context, const char * name_, uint32_t value_)
  {
  if(name_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_uint32(get_context(context), name_, value_);
  }

result_t int32_name_value_action(cli_t *context, const char * name_, int32_t value_)
  {
  if(name_ == 0)
    return e_bad_parameter;
  // set the value
  return reg_set_int32(get_context(context), name_, value_);
  }

result_t xyz_name_value_action(cli_t *context, const char * name_, xyz_t *value_)
  {
  if(name_ == 0 ||
      value_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_xyz(get_context(context), name_, value_);
  }

result_t matrix_name_value_action(cli_t *context, const char * name_, matrix_t *value_)
  {
  if(name_ == 0 ||
      value_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_matrix(get_context(context), name_, value_);
  }

result_t string_name_value_action(cli_t *context, const char * name_, const char * value_)
  {
  if(name_ == 0 ||
      value_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_string(get_context(context), name_, value_);
  }

result_t bool_name_value_action(cli_t *context, const char * name_, uint16_t value_)
  {
  if(name_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_bool(get_context(context), name_, value_ != 0);
  }

result_t float_name_value_action(cli_t *context, const char * name_, float value_)
  {
  if(name_ == 0)
    return e_bad_parameter;

  // set the value
  return reg_set_float(get_context(context), name_, value_);
  }

result_t edit_name_action(cli_t *context, const char * name_)
  {
  if(name_ == 0)
    return e_bad_parameter;

  result_t result;

  stream_p stream;
  if(failed(stream_open(get_context(context), name_, &stream)))
    {
    if(failed(result = stream_create(get_context(context), name_, &stream)))
      return result;
    }

  // open the editor and edit the script.
  return edit_script(context, name_, stream);
  }

result_t cat_name_action(cli_t *context, const char * name)
  {
  result_t result;

  if(name == 0)
    return e_bad_parameter;

  stream_p stream;

  if(failed(result = stream_open(get_context(context), name, &stream)))
    return result;

  stream_copy(stream, context->cfg.console_out);

  stream_close(stream);

  stream_puts(context->cfg.console_out, "\r\n");

  return result;
  }

result_t rm_name_action(cli_t *context, const char * name_)
  {
  result_t result;
  if(failed(result = reg_delete_value(get_context(context), name_)))
    {
    stream_puts(context->cfg.console_err, "Error when deleting value\r\n");
    return result;
    }
  return s_ok;
  }

result_t rmdir_path_action(cli_t *context, const char * name_)
  {
  result_t result;
  memid_t key;

  if(failed(result = open_key(get_context(context), name_, false, &key)) ||
      failed(result = reg_delete_key(key)))
    {
    stream_puts(context->cfg.console_err, "Error when deleting key\r\n");
    return result;
    }
  return s_ok;
  }

result_t mkdir_path_action(cli_t *context, const char * name_)
  {
  // open a sub-key
  result_t result;
  memid_t memid;
  if(failed(result = open_key(get_context(context), name_, true, &memid)))
    {
    stream_puts(context->cfg.console_err, "Error when creating key\r\n");
    return result;
    }

  // update the submode
  make_prompt(context->prompt[context->root_level], MAX_PROMPT_LENGTH, get_full_path(memid));
  context->current[context->root_level] = memid;

  return s_ok;
  }

result_t cd_path_action(cli_t *context, const char * name_)
  {
  // open a sub-key
  result_t result;
  memid_t memid = 0;
  if(failed(result = open_key(get_context(context), name_, false, &memid)))
    {
    stream_puts(context->cfg.console_err, "Error when changine to key\r\n");
    return result;
    }

  // update the submode
  make_prompt(context->prompt[context->root_level], MAX_PROMPT_LENGTH, get_full_path(memid));
  context->current[context->root_level] = memid;

  return s_ok;
  }

result_t ls_path_recursive_action(cli_t *context, const char * path)
  {
  uint16_t indent = 0;
  result_t result;
  memid_t key;
  vector_p matches = 0;

  bool recursive = context->tokens[2].token_buffer != 0 && strlen(context->tokens[2].token_buffer) != 0;

  if (strlen(path) == 0)
    {
    show_key(context->cfg.console_out, get_context(context), true, false, &indent);
    return s_ok;
    }

  // expand wildcards
  if (failed(result = match_path(context, path, true, &key, &matches)))
    return result;

  // work over each one.
  field_datatype dt = 0;
  memid_t child = 0;
  uint16_t i;
  char name[REG_NAME_MAX + 1];

  if (matches == 0)
    {
    // this is the case when a directory is given and no search
    show_key(context->cfg.console_out, key, false, recursive, &indent);
    }
  else
    {
    // decide what to do
    uint16_t len;
    if (failed(result = vector_count(matches, &len)))
      {
      kfree_split(matches);
      return result;
      }

    for (i = 0; i < len; i++)
      {
      const char * name;
      if (failed(result = vector_at(matches, i, &name)))
        {
        kfree_split(matches);
        return result;
        }

      if (failed(result = reg_query_child(key, name, &child, &dt, 0)))
        {
        if (result == e_not_found)
          continue;      // very weird!

        kfree_split(matches);
        return result;
        }

      if (dt > field_key)
        show_value(context->cfg.console_out, child, dt, name, &indent, 0);
      }

    bool needs_newline = true;

    // now the directories
    for (i = 0; i < len; i++)
      {
      const char * name;
      if (failed(result = vector_at(matches, i, &name)))
        {
        kfree_split(matches);
        return result;
        }

      if (failed(result = reg_query_child(key, name, &child, &dt, 0)))
        {
        if (result == e_not_found)
          continue;      // very weird!

        kfree_split(matches);
        return result;
        }

      if (dt == field_key)
        {
        if (needs_newline)
          {
          stream_puts(context->cfg.console_out, cr_lf);
          stream_printf(context->cfg.console_out, "dir %s\r\n", name);
          }
        indent += 2;
        show_key(context->cfg.console_out, child, false, recursive, &indent);
        }
      }

    kfree_split(matches);
    }
  return s_ok;
  }

result_t exit_action(cli_t *context)
  {
  if(context->current[context->root_level] == 0)
    return cli_quit(context);

  return s_ok;
  }
