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
#include "neutron_cli_impl.h"

static const char *publisher_key = "neutron";
static const char *s_rate_value = "rate";
static const char *s_type_value = "type";
static const char *s_sample_type_value = "sample";
static const char *s_filter_type_value = "filter";
static const char *s_filter_length_value = "length";
static const char *s_coefficient_value = "coeff%d";   // will have 1..16 appended
static const char *s_filter_gain_value = "gain";
static const char *s_loopback_value = "loopback";
static const char *s_publish_value = "publish";
static const char *s_alarm_min_value = "min";
static const char *s_alarm_max_value = "max";
static const char *s_alarm_type_value = "type";
static const char *s_alarm_period_value = "period";
static const char *s_alarm_reset_value = "reset";
static const char *publisher_name = "neutron ";

result_t neutron_action(cli_t *context)
  {
  result_t result;
  memid_t key;

  if(failed(result = open_key(0, publisher_key, true, &key)))
    return result;

  return cli_submode_enter(context, key, publisher_name);
  }

result_t neutron_publish_id_action(cli_t *context, uint16_t id)
  {
  result_t result;
  memid_t key;

  char name[REG_NAME_MAX + 1];
  snprintf(name, REG_NAME_MAX, "%d", id);

  if (failed(result = reg_open_key(get_context(context), name, &key)))
    {
    if (failed(result = reg_create_key(get_context(context), name, &key)))
      return result;
    }

  const enum_t *enum_desc;
  char prompt[MAX_PROMPT_LENGTH];
  if (succeeded(find_enum_name(can_ids, id, &enum_desc)))
    snprintf(prompt, MAX_PROMPT_LENGTH, "%s publish %s", publisher_name, enum_desc->name);
  else
    snprintf(prompt, MAX_PROMPT_LENGTH, "%s publish %d ", publisher_name, id);

  return cli_submode_enter(context, key, prompt);
  }

result_t neutron_publish_id_exit_action(cli_t *context)
  {
  cli_submode_exit(context);

  return s_ok;
  }

result_t neutron_exit_action(cli_t *context)
  {
  cli_submode_exit(context);

  return s_ok;
  }

static result_t show_published_id(handle_t dest, memid_t key, uint16_t key_id)
  {
  char name[32];
  result_t result;
  const enum_t *enum_desc;

  if (succeeded(find_enum_name(can_ids, key_id, &enum_desc)))
    stream_printf(dest, "publish %s\r\n", enum_desc->name);
  else
    stream_printf(dest, "publish %d\r\n", key_id);

  uint16_t v_uint16;
  if (succeeded(reg_get_uint16(key, s_rate_value, &v_uint16)))
    stream_printf(dest, "  rate %d\r\n", v_uint16);

  if (succeeded(reg_get_uint16(key, s_type_value, &v_uint16)) &&
     succeeded(find_enum_name(publish_types, v_uint16, &enum_desc)))
      stream_printf(dest, "  type %s\r\n", enum_desc->name);

  if (succeeded(reg_get_uint16(key, s_sample_type_value, &v_uint16)) &&
    succeeded(find_enum_name(sample_types, v_uint16, &enum_desc)))
    stream_printf(dest, "  sample %s\r\n", enum_desc->name);

  if (succeeded(reg_get_uint16(key, s_filter_type_value, &v_uint16)))
    {
    if(succeeded(find_enum_name(filter_types, v_uint16, &enum_desc)))
      stream_printf(dest, "  filter %s\r\n", enum_desc->name);

    // if we are a none filter the don't process
    if (v_uint16 > 0)
      {
      uint16_t filter_type = v_uint16;
      // must have a length!
      if (succeeded(reg_get_uint16(key, s_filter_length_value, &v_uint16)))
        {
        // always clip this, as long filters are NEVER supported
        if (v_uint16 > 16)
          v_uint16 = 16;

        stream_printf(dest, "    length %d\r\n", v_uint16);

        // is fir or iir filter
        if (filter_type > 1)
          {
          float gain;
          // should have a gain..
          if (succeeded(reg_get_float(key, s_filter_gain_value, &gain)))
            stream_printf(dest, "    gain %f\r\n", gain);

          uint16_t length = v_uint16;

          // work over each coefficient.
          for (v_uint16 = 0; v_uint16 < length; v_uint16++)
            {
            snprintf(name, 32, s_coefficient_value, v_uint16 + 1);
            float coeff;
            if (failed(reg_get_float(key, name, &coeff)))
              coeff = 0;

            // now dump it...
            stream_printf(dest, "    coeff %d %f\r\n", v_uint16, (double)coeff);
            }
          }
        }
      }
    }

  bool loopback;
  if (succeeded(reg_get_bool(key, s_loopback_value, &loopback)))
    stream_printf(dest, "  loopback %s\r\n", loopback ? "true" : "false");

  bool publish;
  if (succeeded(reg_get_bool(key, s_publish_value, &publish)))
    stream_printf(dest, "  publish %s\r\n", publish ? "true" : "false");

  memid_t alarm_key = 0;

  uint16_t i;
  char alarm_name[REG_NAME_MAX + 1];
  field_datatype key_type;

  // enumerate the child keys, these are alarms
  do
    {
    key_type = field_key;

    if (failed(result = reg_enum_key(key, &key_type, 0, 0, REG_NAME_MAX + 1, alarm_name, &alarm_key)))
      break;

    bool is_valid = true;
    uint16_t can_id = 0;
    // ensure the name is a can-id
    for (i = 0; i < REG_NAME_MAX && alarm_name[i] != 0; i++)
      {
      if (!isdigit(alarm_name[i]))
        {
        is_valid = false;
        break;
        }

      can_id *= 10;
      can_id += alarm_name[i] - '0';
      }

    float v_float;

    if (is_valid)
      {
      if (succeeded(find_enum_name(can_ids, can_id, &enum_desc)))
        stream_printf(dest, "  alarm %s\r\n", enum_desc->name);
      else
        stream_printf(dest, "  alarm %d\r\n", can_id);

      if (succeeded(reg_get_float(alarm_key, s_alarm_min_value, &v_float)))
        stream_printf(dest, "    min %f\r\n", v_float);

      if (succeeded(reg_get_float(alarm_key, s_alarm_max_value, &v_float)))
        stream_printf(dest, "    max %f\r\n", v_float);

      if (succeeded(reg_get_uint16(alarm_key, s_alarm_type_value, &v_uint16)) && 
        succeeded(find_enum_name(alarm_types, v_uint16, &enum_desc)))
        stream_printf(dest, "    type %s\r\n", enum_desc->name);

      if (succeeded(reg_get_uint16(alarm_key, s_alarm_period_value, &v_uint16)))
        stream_printf(dest, "    period %d\r\n", v_uint16);

      if (succeeded(reg_get_uint16(alarm_key, s_alarm_reset_value, &v_uint16)))
        {
        if(succeeded(find_enum_name(can_ids, v_uint16, &enum_desc)))
          stream_printf(dest, "    reset %s\r\n", enum_desc->name);
        else
          stream_printf(dest, "    reset $d\r\n", v_uint16);
        }
      }
    } while (true);

  return s_ok;
  }

result_t neutron_ls_id_action(cli_t *context, uint16_t *id)
  {
  result_t result;
  memid_t key;

  if (failed(result = reg_open_key(0, publisher_key, &key)))
    return result;

  memid_t published_id = 0;

  if (id == 0)
    {
    // we expect the publisher to have numeric keys.

    uint16_t i;
    char name[REG_NAME_MAX + 1];
    field_datatype key_type;

    // enumerate the child keys, these are alarms
    do
      {
      key_type = field_key;

      if (failed(result = reg_enum_key(key, &key_type, 0, 0, REG_NAME_MAX + 1, name, &published_id)))
        break;

      bool is_valid = true;
      uint16_t can_id = 0;

      // ensure the name is a can-id
      for (i = 0; i < REG_NAME_MAX && name[i] != 0; i++)
        {
        if (!isdigit(name[i]))
          {
          is_valid = false;
          break;
          }

        can_id *= 10;
        can_id += name[i] - '0';
        }

      if (is_valid)
        {
        show_published_id(context->cfg.console_out, published_id, can_id);
        stream_puts(context->cfg.console_out, "\r\n");
        }
      } while (true);

      return s_ok;
    }

  char key_name[32];
  snprintf(key_name, 32, "%d", *id);

  result = reg_open_key(key, key_name, &published_id);
  neutron_free(key_name);

  if (failed(result))
    return result;

  return show_published_id(context->cfg.console_out, published_id, *id);
  }

result_t neutron_rm_id_action(cli_t *context, uint16_t id_)
  {
  return e_not_implemented;
  }

// can_aerospace.c has these
result_t neutron_publish_id_rate_rate_action(cli_t *context, uint16_t rate_)
  {
  return reg_set_uint16(get_context(context), s_rate_value, rate_);
  }

result_t neutron_publisher_publish_id__type_can_action(cli_t *context, uint16_t can_)
  {
  return reg_set_uint16(get_context(context), s_type_value, can_);
  }

result_t neutron_publish_id_sample_sample_action(cli_t *context, uint16_t sample_)
  {
  return reg_set_uint16(get_context(context), s_sample_type_value, sample_);
  }

result_t neutron_publish_id_filter_filter_type_length_value_action(cli_t *context, uint16_t length)
  {
  // setting the length will remove all of the old settings and set
  // the filter coefficients to 1.0

  uint16_t filter_type;
  if (succeeded(reg_get_uint16(get_context(context), s_filter_type_value, &filter_type)) &&
    filter_type >= 2)
    {
    uint16_t old_length = 0;
    uint16_t i;

    char sample_name[REG_NAME_MAX + 1];
    // get the old length (if set)
    if (succeeded(reg_get_uint16(get_context(context), s_filter_length_value, &old_length)))
      {
      for (i = 0; i < old_length; i++)
        {
        snprintf(sample_name, REG_NAME_MAX, s_coefficient_value, i + 1);

        if (i < length)
          reg_set_float(get_context(context), sample_name, 1.0);
        else
          reg_delete_value(get_context(context), sample_name);
        }
      }

    for (i = old_length; i < length; i++)
      {
      snprintf(sample_name, REG_NAME_MAX, s_coefficient_value, i + 1);

      reg_set_float(get_context(context), sample_name, 1.0);
      }
    }

  return reg_set_uint16(get_context(context), s_filter_length_value, length);
  }

result_t neutron_publish_id_filter_filter_type_gain_value_action(cli_t *context, float value_)
  {
  return reg_set_float(get_context(context), s_filter_gain_value, value_);
  }

result_t neutron_publish_id_filter_filter_type_coeff_index_value_action(cli_t *context, uint16_t index, float value)
  {
  // check the length
  uint16_t length;
  result_t result;
  if (failed(result = reg_get_uint16(get_context(context), s_filter_length_value, &length)))
    return result;

  if (index >= length)
    return e_bad_parameter;

  uint16_t filter_type;
  if (failed(result = reg_get_uint16(get_context(context), s_filter_type_value, &filter_type)))
    return result;

  if (filter_type < 2)
    return e_bad_parameter;

  char sample_name[REG_NAME_MAX + 1];
  snprintf(sample_name, REG_NAME_MAX, s_coefficient_value, index + 1);

  return reg_set_float(get_context(context), sample_name, value);
  }

result_t neutron_publish_id_filter_filter_type_exit_action(cli_t *context)
  {
  cli_submode_exit(context);
  return s_ok;
  }

result_t neutron_publish_id_type_type_action(cli_t *context, uint16_t type)
  {
  return reg_set_uint16(get_context(context), s_type_value, type);
  }

result_t neutron_publish_id_filter_filter_type_action(cli_t *context, uint16_t type_)
  {
  result_t result;
  if (failed(result = reg_set_uint16(get_context(context), s_filter_type_value, type_)))
    return result;

  if (type_ == 0)      // special case...
    return s_ok;

  // is filter mode.
  // todo: delete coefficients?

  char prompt[MAX_PROMPT_LENGTH];
  const enum_t *enum_desc;
  if (succeeded(find_enum_name(filter_types, type_, &enum_desc)))
    snprintf(prompt, MAX_PROMPT_LENGTH, "%s publish filter %s", publisher_name, enum_desc->name);
  else
    snprintf(prompt, MAX_PROMPT_LENGTH, "%s publish filter %d", publisher_name, type_);

  return cli_submode_enter(context, get_context(context), prompt);
  }

result_t neutron_publish_id_loopback_loopback_action(cli_t *context, bool loopback_)
  {
  return reg_set_bool(get_context(context), s_loopback_value, loopback_);
  }

result_t neutron_publish_id_publish_is_published_action(cli_t *context, bool is_published_)
  {
  return reg_set_bool(get_context(context), s_publish_value, is_published_);
  }

result_t neutron_publish_id_alarm_alarm_id_min_min_value_action(cli_t *context, float value_)
  {
  return reg_set_float(get_context(context), s_alarm_min_value, value_);
  }

result_t neutron_publish_id_alarm_alarm_id_max_max_value_action(cli_t *context, float value_)
  {
  return reg_set_float(get_context(context), s_alarm_max_value, value_);
  }

result_t neutron_publish_id_alarm_alarm_id_period_length_action(cli_t *context, uint16_t length_)
  {
  return reg_set_uint16(get_context(context), s_alarm_period_value, length_);
  }

// level,event
result_t neutron_publish_id_alarm_alarm_id_type_alarm_type_action(cli_t *context, uint16_t type_)
  {
  return reg_set_uint16(get_context(context), s_alarm_type_value, type_);
  }

extern result_t neutron_publish_id_alarm_alarm_id_reset_reset_id_action(cli_t *context, uint16_t value_)
  {
  return reg_set_uint16(get_context(context), s_alarm_reset_value, value_);
  }

result_t neutron_publish_id_alarm_alarm_id_exit_action(cli_t *context)
  {
  cli_submode_exit(context);

  return s_ok;
  }

result_t neutron_publish_id_alarm_alarm_id_action(cli_t *context, uint16_t alarm_id_)
  {
  result_t result;
  memid_t memid;

  // print the value
  char name[REG_NAME_MAX + 1];
  snprintf(name, REG_NAME_MAX, "%d", alarm_id_);

  if (failed(result = reg_open_key(get_context(context), name, &memid)))
    {
    if (failed(result = reg_create_key(get_context(context), name, &memid)))
      return result;
    }

  // we have the alarm key

  const enum_t *enum_desc;
  char prompt[MAX_PROMPT_LENGTH];
  if (succeeded(find_enum_name(can_ids, alarm_id_, &enum_desc)))
    snprintf(prompt, MAX_PROMPT_LENGTH, "%s publish alarm: %s", publisher_name, enum_desc->name);
  else
    snprintf(prompt, MAX_PROMPT_LENGTH, "%s publish alarm: %d ", publisher_name, alarm_id_);

  return cli_submode_enter(context, memid, prompt);
  }
