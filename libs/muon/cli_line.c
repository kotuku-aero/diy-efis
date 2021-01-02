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
#include "cli.h"
#include <string.h>

#define LINE_ALL(line)       &((line)->buf[0])
#define LINE_CURRENT(line)   &(line)->buf[(line)->current]

result_t cli_line_init(cli_line_t *line)
  {
  line->current = 0;
  line->buffer = (char *)neutron_malloc(CLI_MAX_LINE_LENGTH+1);
  line->buffer[0] = 0;
  line->buflen = CLI_MAX_LINE_LENGTH;

  return s_ok;
  }

result_t cli_line_reset(cli_line_t *line)
  {
  if (!line)
    {
    return e_bad_parameter;
    }

  line->current = 0;
  line->buffer[0] = 0;

  return s_ok;
  }

result_t cli_line_insert(cli_t *parser, char ch)
  {
  int n;
  cli_line_t *line;

  if (parser == 0 || ch == 0)
    return e_bad_parameter;

  line = &parser->lines[parser->cur_line];

  uint16_t len = strlen(line->buffer);
  if (len >= (line->buflen - 1))
    {
    // expand the buffer
    char *new_buffer = (char *)neutron_malloc(line->buflen + 32);
    strcpy(new_buffer, line->buffer);
    line->buflen += 32;
    neutron_free(line->buffer);
    line->buffer = new_buffer;
    }

  // we can't assume the memove handles overlaps
  uint16_t pos;
  for (pos = len + 1; pos > line->current; pos--)
    line->buffer[pos] = line->buffer[pos - 1];

  line->buffer[line->current] = ch;

  /*
   * Insert the new character and update the line display. We do not
   * have full curse support here. Instead, we simply assume all
   * characters are on the same line and use backspace to move the
   * cursor.
   */
  stream_putc(parser->cfg.console_out, ch);

  line->current++; /* update current position */
  stream_puts(parser->cfg.console_out, &line->buffer[line->current]);

  uint16_t last = strlen(line->buffer);
  /* Move cursor back to the current position */
  for (n = line->current; n < last; n++)
    stream_putc(parser->cfg.console_out, '\b');

  return s_ok;
  }

result_t cli_line_delete(cli_t *parser)
  {
  cli_line_t *line;
  int n;

  if (parser == 0)
    return e_bad_parameter;

  line = &parser->lines[parser->cur_line];

  uint16_t length = strlen(line->buffer);

  if (line->current == 0 || length == 0)
    {
    /* Line is empty or we're at the beginning of the line */
    return e_not_found;
    }

  line->current--;
  uint16_t len = strlen(line->buffer);

  memmove(&line->buffer[line->current], &line->buffer[line->current + 1], (len + 1) - line->current);
  length--;

  /* Update the display */
  stream_putc(parser->cfg.console_out, '\b');
  if (length == line->current)
    stream_puts(parser->cfg.console_out, " \b");  // erase end of line
  else
    {
    stream_puts(parser->cfg.console_out, &line->buffer[line->current]);
    stream_puts(parser->cfg.console_out, " \b");

    for (n = line->current; n < length; n++)
      stream_putc(parser->cfg.console_out, '\b');
    }

  return s_ok;
  }

result_t cli_line_print(const cli_t *parser, int print_prompt, int new_line)
  {
  const cli_line_t *line;
  int n;

  if(parser == 0)
    return e_bad_parameter;

  if (new_line)
    stream_puts(parser->cfg.console_out, "\r\n");

  if (print_prompt)
    cli_print_prompt(parser);

  line = &parser->lines[parser->cur_line];

  stream_puts(parser->cfg.console_out, line->buffer);

  uint16_t last = strlen(line->buffer);

  /* Move the cursor back the current position */
  for (n = line->current; n < last; n++)
    stream_putc(parser->cfg.console_out, '\b');

  return s_ok;
  }

uint16_t cli_line_current(const cli_t *parser)
  {
  return parser->lines[parser->cur_line].current;
  }

uint16_t cli_line_last(const cli_t *parser)
  {
  return strlen(parser->lines[parser->cur_line].buffer);
  }

char cli_line_current_char(const cli_t *parser)
  {
  const cli_line_t *line = &parser->lines[parser->cur_line];
  return line->buffer[line->current];
  }

char cli_line_char(const cli_t *parser, uint16_t pos)
  {
  const cli_line_t *line = &parser->lines[parser->cur_line];

  return line->buffer[pos];
  }

char cli_line_next_char(cli_t *parser)
  {
  char retval;
  cli_line_t *line;

  line = &parser->lines[parser->cur_line];

  uint16_t last =strlen(line->buffer);
  if (last == line->current)
    {
    /* Already at the end of the line */
    return 0;
    }


  retval = line->buffer[line->current];

  stream_putc(parser->cfg.console_out, retval);
  line->current++;

  return retval;
  }

char cli_line_prev_char(cli_t *parser)
  {
  cli_line_t *line;

  line = &parser->lines[parser->cur_line];
  if (!line->current)
    {
    /* Already at the beginning of the line */
    return 0;
    }

  stream_putc(parser->cfg.console_out, '\b');
  line->current--;

  return '\b';
  }

result_t cli_line_next_line(cli_t *parser)
  {
  int n;

  if (!VALID_PARSER(parser))
    return e_bad_parameter;

  /* Erase the current line */
  for (n = 0; n < cli_line_last(parser); n++)
    stream_puts(parser->cfg.console_out, "\b \b");

  /* Go to the next line */
  parser->cur_line++;
  if (parser->max_line < parser->cur_line)
    parser->cur_line = 0;

  /* Print out the new line */
  cli_line_print(parser, 0, 0);

  return s_ok;
  }

result_t cli_line_prev_line(cli_t *parser)
  {
  int n;

  if (!VALID_PARSER(parser))
    {
    return e_bad_parameter;
    }

  /* Erase the current line */
  for (n = 0; n < cli_line_last(parser); n++)
    {
    stream_puts(parser->cfg.console_out, "\b \b");
    }

  /* Go to the previous line */
  parser->cur_line--;
  if (0 > parser->cur_line)
    {
    parser->cur_line = parser->max_line;
    }

  /* Print out the new line */
  cli_line_print(parser, 0, 0);

  return s_ok;
  }

result_t cli_line_advance(cli_t *parser)
  {
  if (!VALID_PARSER(parser))
    return e_bad_parameter;

  parser->cur_line++;
  if (CLI_MAX_LINES <= parser->cur_line)
    {
    parser->cur_line = 0;
    }
  if (parser->max_line < parser->cur_line)
    {
    parser->max_line = parser->cur_line;
    }

  return cli_line_reset(&parser->lines[parser->cur_line]);
  }
