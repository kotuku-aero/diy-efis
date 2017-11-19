#include "neutron_cli_impl.h"

extern const char *node_name;
static const char *cr_lf = "\r\n";

static void show_key(handle_t dest, memid_t key, bool full_path, bool recursive, uint16_t *indent)
  {
  char name[REG_NAME_MAX + 1];
  const char * path;
  if(full_path)
    path = get_full_path(key);
  else
    {
    reg_query_memid(key, 0, name, 0, 0);
    path = strdup(name);
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

  handle_t stream;
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

  handle_t stream;

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
  const char * dirname = get_full_path(memid);

  char * prompt[MAX_PROMPT_LENGTH];
  snprintf(prompt, MAX_PROMPT_LENGTH, "%s %s ", node_name, dirname);

  kfree(dirname);

  strncpy(context->prompt[context->root_level], prompt, MAX_PROMPT_LENGTH);
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
    stream_puts(context->cfg.console_err, "Error when creating key\r\n");
    return result;
    }

  // update the submode
  const char * dirname = get_full_path(memid);

  char prompt[MAX_PROMPT_LENGTH];
  snprintf(prompt, MAX_PROMPT_LENGTH, "%s %s ", node_name, dirname);
  kfree(dirname);

  strncpy(context->prompt[context->root_level], prompt, MAX_PROMPT_LENGTH);
  context->current[context->root_level] = memid;

  return s_ok;
  }

result_t ls_path_recursive_action(cli_t *context, const char * path)
  {
  uint16_t indent = 0;
  result_t result;
  memid_t key;
  handle_t matches = 0;

  bool recursive = strlen(context->tokens[2].buffer) != 0;

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

  memid_t parent;
  if(failed(reg_query_memid(context->current[context->root_level], 0, 0, 0, &parent)))
    parent = 0;

  context->current[context->root_level] = parent;
  // update the submode
  const char * dirname = get_full_path(parent);

  char prompt[MAX_PROMPT_LENGTH];
  snprintf(prompt, MAX_PROMPT_LENGTH, "%s %s ", node_name, dirname);
  kfree(dirname);

  strncpy(context->prompt[context->root_level], prompt, MAX_PROMPT_LENGTH);
  return s_ok;
  }
