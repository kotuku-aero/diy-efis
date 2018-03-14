#include "gluon_cli.h"
#include "gluon.h"

#include <stdio.h>

// list captures
result_t gluon_ls_name_action(cli_t *context, const char * gluon_ls_name_)
  {
  return e_not_implemented;
  }

// remove a capture and all data
result_t gluon_rm_name_action(cli_t *context, const char * gluon_rm_name_)
  {
  return e_not_implemented;
  }

// print text up to the width with a space
static void print_column(cli_t *context, uint16_t width, const char *value)
  {
  if (context->cfg.console_out == 0)
    return;

  uint16_t n;
  for (n = 0; n < width; n++)
    {
    if (*value == 0)
      stream_putc(context->cfg.console_out, ' ');
    else
      stream_putc(context->cfg.console_out, *value++);
    }

  stream_putc(context->cfg.console_out, ',');
  }

static const char *crlf = "\r\n";
static const char *int16_fmt = "%d";
static const char *int32_fmt = "%d";
static const char *uint16_fmt = "%d";
static const char *uint32_fmt = "%d";
static const char *flt_fmt = "%f";
static const char *timestamp_fmt = "%4d-%02.2d-%02.2d %02.2d:%02.2d%02.2d";
#define COLUMN_WIDTH 20

// query from and to
// the results are in a CSV format
result_t gluon_show_name_from_to_action(cli_t *context, const char *series, const char * from, const char * to)
  {
  result_t result;
  stream_p *series_streams = 0;
  gluon_value_t *columns = 0;
  vector_p results = 0;
  vector_p column_definitions = 0;

  uint32_t from_tm = 0;
  uint32_t to_tm = UINT32_MAX;
  uint16_t column;
  uint16_t num_columns = 0;
  gluon_series_t definition;

  do 
    {
    if (from != 0 && failed(result = gluon_parse(from, &from_tm)))
      break;

    if (to != 0 && failed(gluon_parse(to, &to_tm)))
      break;

    // work over the series and sort them into a cross-tab
    memid_t handle;
    if (failed(result = gluon_open_series(series, &handle)))
      break;

    if (failed(result = gluon_get_series_descr(handle, &definition)))
      break;

    if (failed(result = gluon_enumerate_values(handle, &column_definitions)))
      break;

    if (failed(result = vector_count(column_definitions, &num_columns)))
      break;

    if (failed(result = vector_begin(column_definitions, (void **)&columns)))
      break;

    char buffer[COLUMN_WIDTH] = { 0 };

    // print the timestamp
    print_column(context, COLUMN_WIDTH, "timestamp");

    // print the series names
    for (column = 0; column < num_columns; column++)
      print_column(context, COLUMN_WIDTH, columns[column].name);

    if (failed(result = vector_close(column_definitions)))
      {
      column_definitions = 0;
      break;
      }

    // get all of the series that we need.
    if (failed(result = gluon_select(series, &from, &to, columns, &results)))
      break;

    if (failed(result = vector_begin(results, (void **)&series_streams)))
      return result;

    stream_puts(context->cfg.console_out, crlf);

    if (failed(result))
      break;


    for (; from <= to; from += definition.rate)
      {
      // firstly we position all streams at the same column.
      // if the setpos fails with e_not_found then the column is null
      bool empty_tuple = true;
      for (column = 0; column < num_columns; column++)
        {
        // position the cross-tab to the correct series value
        if (failed(result = stream_setpos(series_streams[column], from)) &&
          result != e_not_found)
          break;
        if (succeeded(result))
          empty_tuple = false;
        }

      // don't print a timestamp if its empty
      if (empty_tuple)
        continue;

      for (column = 0; column < num_columns; column++)
        {

        // format and print the timestamp column
        gluon_tm_t tm;
        gluon_mktime(from, &tm);
        snprintf(buffer, COLUMN_WIDTH, timestamp_fmt, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
        print_column(context, COLUMN_WIDTH, buffer);

        // print the value
        switch (columns[column].type)
          {
          case st_float:
            {
            float value;
            if (failed(result = stream_read(series_streams[column], &value, sizeof(float), 0)))
              {
              if (result != e_not_found)
                break;            // is not a null value
              buffer[0] = 0;      // no data available at that series point
              }
            else
              snprintf(buffer, COLUMN_WIDTH, flt_fmt, value);
            }
            break;
          case st_int16 :
            {
            int16_t value;
            if (failed(result = stream_read(series_streams[column], &value, sizeof(int16_t), 0)))
              {
              if (result != e_not_found)
                break;            // is not a null value
              buffer[0] = 0;      // no data available at that series point
              }
            else
              snprintf(buffer, COLUMN_WIDTH, int16_fmt, value);
            }
            break;
          case st_int32 :
            {
            int32_t value;
            if (failed(result = stream_read(series_streams[column], &value, sizeof(int32_t), 0)))
              {
              if (result != e_not_found)
                break;            // is not a null value
              buffer[0] = 0;      // no data available at that series point
              }
            else
              snprintf(buffer, COLUMN_WIDTH, int32_fmt, value);
            }
            break;
          case st_uint16 :
            {
            uint16_t value;
            if (failed(result = stream_read(series_streams[column], &value, sizeof(uint16_t), 0)))
              {
              if (result != e_not_found)
                break;            // is not a null value
              buffer[0] = 0;      // no data available at that series point
              }
            else
              snprintf(buffer, COLUMN_WIDTH, uint16_fmt, value);
            }
            break;
          case st_uint32 :
            {
            uint32_t value;
            if (failed(result = stream_read(series_streams[column], &value, sizeof(uint32_t), 0)))
              {
              if (result != e_not_found)
                break;            // is not a null value
              buffer[0] = 0;      // no data available at that series point
              }
            else
              snprintf(buffer, COLUMN_WIDTH, uint32_fmt, value);
            }
            break;
          }

        }

      if (failed(result))
        break;
      stream_puts(context->cfg.console_out, crlf);
      }
    } while (false);

  if (results != 0)
    {
    // close all streams
    for (column = 0; column < num_columns; column++)
      stream_close(series_streams[column]);

    vector_close(results);
    }

  if (column_definitions != 0)
    vector_close(column_definitions);

  return result;
  }



result_t gluon_query_name_action(cli_t *context, const char * gluon_query_name_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_rate_rate_action(cli_t *context, uint16_t gluon_capture_name_rate_rate_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_eq_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_eq_id_, int16_t gluon_capture_name_trigger_eq_id_value_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_lt_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_lt_id_, int16_t gluon_capture_name_trigger_lt_id_value_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_gt_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_gt_id_, int16_t gluon_capture_name_trigger_gt_id_value_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_eqf_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_eqf_id_, float gluon_capture_name_trigger_eqf_id_value_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_ltf_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_ltf_id_, float gluon_capture_name_trigger_ltf_id_value_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_gtf_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_gtf_id_, float gluon_capture_name_trigger_gtf_id_value_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_rm_eq_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_eq_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_rm_lt_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_lt_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_rm_gt_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_gt_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_rm_eqf_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_eqf_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_rm_ltf_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_ltf_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_rm_gtf_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_gtf_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_exit_action(cli_t *context)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_trigger_action(cli_t *context)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_ls_action(cli_t *context)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_id_id_name_action(cli_t *context, uint16_t gluon_capture_name_id_id_, const char * gluon_capture_name_id_id_name_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_rm_id_action(cli_t *context, uint16_t gluon_capture_name_rm_id_)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_exit_action(cli_t *context)
  {
  return e_not_implemented;
  }



result_t gluon_capture_name_action(cli_t *context, const char * gluon_capture_name_)
  {
  return e_not_implemented;
  }



result_t gluon_exit_action(cli_t *context)
  {
  return e_not_implemented;
  }



result_t gluon_action(cli_t *context)
  {
  return e_not_implemented;
  }



