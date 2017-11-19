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
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "cli.h"

void cli_print_prompt(const cli_t *parser)
  {
  stream_puts(parser->cfg.console_out, "\r\n");
  stream_puts(parser->cfg.console_out, parser->prompt[parser->root_level]);
  stream_puts(parser->cfg.console_out, "> ");
  }

/**
 * \brief    Print the description of a node out.
 * \details  For keyword nodes,  the keyword itself is printed. For parameter
 *           nodes, a string of the form <[type]:[parameter]> is printed. For
 *           end nodes, \<LF\> is printed. This call should never be invoked with
 *           a root node.
 *
 * \param    parser     Pointer to the parser structure.
 * \param    node       Pointer to the node to be printed.
 * \param    add_lf     1 to add LF before printing the node; 0 otherwise.
 * \param    print_desc 1 to print the description of the node; 0 otherwise.
 */
static void cli_help_print_node(cli_t *parser, cli_node_t *node,
                                bool add_lf, bool print_desc)
  {

  if (add_lf)
    stream_puts(parser->cfg.console_out, "\r\n");

  switch (node->type)
    {
    case CLI_NODE_ROOT:
      break;
    case CLI_NODE_END:
      stream_puts(parser->cfg.console_out, "\r\n");
      break;
    case CLI_NODE_ENUM :
      {
      const enum_t *lnode = (const enum_t *)node->param;
      const char * token = CUR_TOKEN(parser)->token_buffer;
      uint16_t len = strlen(token);

      while (lnode->name != 0)
        {
        // try to match it
        if(strncmp(lnode->name, token, len) == 0)
          {
          stream_puts(parser->cfg.console_out, lnode->name);
          stream_puts(parser->cfg.console_out, "\r\n");
          }
        lnode++;
        }
      }
      break;
    case CLI_NODE_PATH :
      {
      vector_t matches = 0;
      if (succeeded(match_path(parser, CUR_TOKEN(parser)->token_buffer, false, 0, &matches)))
        {
        uint16_t len;
        if (succeeded(vector_count(matches, &len)))
          {
          uint16_t i;
          for (i = 0; i < len; i++)
            {
            const char * option;

            if (succeeded(vector_at(matches, i, &option)))
              {
              stream_puts(parser->cfg.console_out, option);
              stream_puts(parser->cfg.console_out, "\r\n");
              }
            }
          }
        }
      if (matches != 0)
        kfree_split(matches);
      }
      break;
    default:
      stream_puts(parser->cfg.console_out, node->param);
      if (print_desc && node->desc)
        {
        stream_puts(parser->cfg.console_out, " - ");
        stream_puts(parser->cfg.console_out, node->desc);
        }
      break;
    }
  }

/**
 * \brief    Print an error message due to some kind of parsing error.
 *
 * \param    parser Pointer to the parser structure.
 * \param    msg    Pointer to the error messsage string.
 *
 * \return   None.
 */
static void cli_print_error(cli_t *parser, const char *msg)
  {
  int n, m;

  stream_puts(parser->cfg.console_out, "\r\n");

  m = strlen(parser->prompt[parser->root_level]) + 1;

  for (n = 0; n < m + parser->last_good; n++)
    stream_putc(parser->cfg.console_out, ' ');

  stream_putc(parser->cfg.console_out, '^');
  stream_puts(parser->cfg.console_out, msg);
  }

/**
 * \brief    Reset the user input section of the parser structure.
 *
 * \param    parser Pointer to the parser structure.
 */
static void cli_input_reset(cli_t *parser)
  {
  if(parser->user_buf != 0)
    kfree((void *)parser->user_buf);

  parser->user_buf = 0;
  parser->user_input_cb = 0;
  }

static bool cli_is_user_input(cli_t *parser, bool *do_echo)
  {
  if (parser->user_buf != 0)
    {
    *do_echo = parser->user_do_echo;
    return true;
    }


  *do_echo = false;
  return false;
  }

static void cli_record_command(cli_t *parser, result_t rc)
  {
  /* Save the state of the command */
  parser->last_line_idx = CURRENT_LINE(parser);
  parser->last_rc = rc;
  parser->last_end_node = parser->cur_node;
  }

/**
 * \brief    If the command is not complete, attempt to complete the command.
 *           If there is a complete comamnd, execute the glue (& action)
 *           function of a command.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   s_ok if a valid command is executed; e_unexpected
 *           otherwise.
 */
static result_t cli_execute_cmd(cli_t *parser)
  {
  bool do_echo;
  result_t rc = s_ok;
  /*
   * Enter a command.
   */
  if ((CLI_STATE_TOKEN == parser->state) ||
      (CLI_STATE_WHITESPACE == parser->state))
    {
    cli_node_t *child;

    if (CLI_STATE_TOKEN == parser->state)
      {
      cli_token_t *token;
      cli_node_t *match;
      bool is_complete;

      token = CUR_TOKEN(parser);
      int num_matched = cli_match(parser, token, parser->cur_node, &match, &is_complete);
      if (num_matched > 0 && is_complete)
        {
        cli_complete_fn fn = cli_complete_fn_tbl[match->type];
        if (fn)
          fn(parser, match, token);

        rc = cli_input(parser, ' ', CLI_CHAR_REGULAR);
        }
      else
        {
        cli_print_error(parser, "Incomplete command\r\n");
        rc = e_incomplete_command;

        /* Reset the internal buffer, state and cur_node */
        cli_record_command(parser, rc);
        cli_fsm_reset(parser);
        cli_print_prompt(parser);
        return rc;
        }
      }

    /* Look for a single keyword node child */
    child = parser->cur_node->children;

    // if this is an optional node, all other nodes are
    // also optional so just skip to end
    if((child->flags & CLI_NODE_FLAGS_OPT_START)!= 0)
      {
      // move to the end state
      while (child != 0 &&
          child->type != CLI_NODE_END)
        {
        // skip down the list of completed
        child = child->children;
        }
      }
    else if (child->type != CLI_NODE_END)
      child = 0;        // signal an error

    if (child)
      {
      /* Execute the glue function */
      parser->cur_node = child;
      stream_puts(parser->cfg.console_out, "\r\n");
      if (failed(rc = ((cli_glue_fn)child->param)(parser)))
        {
        stream_printf(parser->cfg.console_err, "Error %ld returned\r\n", rc);
        }
      }
    else
      {
      if (parser->token_tos)
        {
        cli_print_error(parser, "Incomplete command\n");
        rc = e_incomplete_command;
        }
      }
    }
  else if (CLI_STATE_ERROR == parser->state)
    {
    cli_print_error(parser, "Parse error\r\n");
    rc = e_parse_error;
    }

  /* Reset FSM states and advance to the next line */
  cli_record_command(parser, rc);
  cli_fsm_reset(parser);
  if (!cli_is_user_input(parser, &do_echo))
    {
    cli_print_prompt(parser);
    }
  return rc;
  }

static result_t cli_match_prefix(cli_t *parser, cli_token_t *token,
                                 const cli_node_t *parent,
                                 const char ch, const int offset)
  {
  bool local_is_complete;
  cli_node_t *child;
  result_t rc;

  for (child = parent->children; 0 != child; child = child->sibling)
    {
    rc = cli_match_fn_tbl[child->type](parser, token, child, &local_is_complete);
    if (failed(rc))
      {
      continue;
      }
    if (CLI_NODE_KEYWORD != child->type)
      {
      return e_unexpected;
      }

    /* There is a match. Make sure that it is part of this node as well */
    if (*((char *) child->param + offset) != ch)
      {
      return e_unexpected;
      }
    }
  return s_ok;
  }

/**
 * \brief    Generate context-sensitive help.
 *
 * \param    parser Pointer to the parser structure.
 */
static result_t cli_help(cli_t *parser)
  {
  cli_node_t *node;
  cli_token_t *token;
  bool local_is_complete;

  if (CLI_STATE_WHITESPACE == parser->state)
    {
    /* Just print out every children */
    for (node = parser->cur_node->children; 0 != node;
         node = node->sibling)
      {
      cli_help_print_node(parser, node, true, true);
      }
    }
  else if (CLI_STATE_ERROR == parser->state)
    {
    /*
     * We have some problem parsing. Just print out the last known
     * good parse point and list the valid options.
     */
    cli_print_error(parser, "Last known good parse point.");
    for (node = parser->cur_node->children; 0 != node;
         node = node->sibling)
      {
      cli_help_print_node(parser, node, true, true);
      }
    }
  else
    {
    /* We have a partial match */
    node = parser->cur_node->children;
    token = CUR_TOKEN(parser);
    for (node = parser->cur_node->children; 0 != node;
         node = node->sibling)
      {
      if (succeeded(cli_match_fn_tbl[node->type](parser, token, node, &local_is_complete)))
        {
        cli_help_print_node(parser, node, true, true);
        }
      }
    }
  cli_line_print(parser, true, true);
  return s_ok;
  }

/**
 * \brief    Complete one level in the parse tree.
 * \details  There are serveral cases we will complete one level:
 *
 *           1. If in TOKEN state, the token is unique and complete.
 *           2. If in WHITESPACE state, there is only one child and it
 *              is not a LF.
 */
static int cli_complete_one_level(cli_t *parser)
  {
  cli_token_t *token;
  cli_node_t *match;
  bool is_complete = false;
  int num_matches;
  bool keep_going = false;
  char *ch_ptr;

  switch (parser->state)
    {
    case CLI_STATE_ERROR:
      /* If we are in ERROR, there cannot be a match. So, just quit */
      stream_putc(parser->cfg.console_out, '\a');
      break;
    case CLI_STATE_WHITESPACE:
      if (parser->cur_node && parser->cur_node->children &&
          !parser->cur_node->children->sibling &&
          (CLI_NODE_KEYWORD == parser->cur_node->children->type))
        {
        ch_ptr = parser->cur_node->children->param;
        while (*ch_ptr)
          {
          cli_input(parser, *ch_ptr, CLI_CHAR_REGULAR);
          ch_ptr++;
          }
        cli_input(parser, ' ', CLI_CHAR_REGULAR);
        }
      else
        {
        /*
         * If we are in WHITESPACE, just dump all children. Since there is no
         * way any token can match to a 0 string.
         */
        cli_help(parser);
        }
      break;
    case CLI_STATE_TOKEN:
    {
      /* Complete a command */
      token = CUR_TOKEN(parser);
      num_matches = cli_match(parser, token, parser->cur_node, &match, &is_complete);
      if (num_matches == 1)
        {
        cli_complete_fn fn = cli_complete_fn_tbl[match->type];
        /*
         * If the only matched node is a keyword, we feel the rest of
         * keyword in. Otherwise, we assume this parameter is complete
         * and just insert a space.
         */
        if (fn)
          {
          fn(parser, match, token);
          }
        cli_input(parser, ' ', CLI_CHAR_REGULAR);

        keep_going = 1;
        }
      else
        {
        int offset, orig_offset;
        /*
         * If we have more than one match, we should try to complete
         * as much as possible. To do that, we grab the node in the
         * (first) matched node and check that the next character
         * from it is common among all matched nodes. If it is common
         * to all matched nodes, we continue to feed them into the
         * parser. However, this is only useful for keywords. If there
         * is a parameter token in the match, we automatically abort.
         */
        uint16_t len = token->token_length;
        offset = orig_offset = len;
        ch_ptr = ((char *)match->param) + len;
        while (('\0' != *ch_ptr) &&
               (s_ok ==
                cli_match_prefix(parser, token, parser->cur_node, *ch_ptr, offset)))
          {
          cli_input(parser, *ch_ptr, CLI_CHAR_REGULAR);

          ch_ptr++;
          offset++;
          }
        if (orig_offset == offset)
          {
          /* If there is no common prefix at all, just display help */
          cli_help(parser);
          }
        }
      break;
    }
    default:
      return false;
    }

  return keep_going;
  }

static const char eol = 0;

result_t cli_input(cli_t *parser, char ch, cli_char_t ch_type)
  {
  bool do_echo;
  int n;
  result_t rc;

  if (parser == 0)
    return e_bad_parameter;
/*
  // check to see if the characters are user input
  if (cli_is_user_input(parser, &do_echo))
    {
    // Process user input
    if (CLI_CHAR_REGULAR != ch_type)
      return s_ok;

    const char *str;
    vector_begin(parser->user_buf, &str);
    uint16_t len;
    vector_count(parser->user_buf, &len);

    if ('\n' == ch)
      {
      // check for a line that has continuation characters
      if(len > 0 &&
         str[len-1] != '\\')
        {
        // We have a complete input. Call the callback.
        vector_push_back(parser->user_buf,  &eol);

        if(parser->user_input_cb != 0)
          rc = parser->user_input_cb(parser, parser->user_buf);

        cli_input_reset(parser);
        cli_print_prompt(parser);
        return rc;
        }

      // otherwise remove the trailing \ as this indicates a line continuation
      vector_pop_back(parser->user_buf, 0);
      len--;
      len--;
      while(len-- > 0 && isspace(str[len]))
        vector_pop_back(parser->user_buf, 0);     // remove the trailing white space
      }

    if ((parser->cfg.ch_erase == ch) || (parser->cfg.ch_del == ch))
      {
      if(len > 0)
        vector_pop_back(parser->user_buf, 0);

      if (parser->user_do_echo)
        stream_putc(parser->cfg.console_out, '\b');
      }
    else
      {
      vector_push_back(parser->user_buf, &ch);

      if (parser->user_do_echo)
        stream_putc(parser->cfg.console_out, ch);
      }
    return s_ok;
    }
*/
  switch (ch_type)
    {
    case CLI_CHAR_REGULAR:
    {
      if ((parser->cfg.ch_complete == ch) ||
          (parser->cfg.ch_help == ch))
        {
        /*
         * Completion and help character do not go into the line
         * buffer. So, do nothing.
         */
        break;
        }

      if ((parser->cfg.ch_erase == ch) || (parser->cfg.ch_del == ch))
        {
        rc = cli_line_delete(parser);
        if (failed(rc))
          return rc;
        }
      else if ('\n' == ch)
        {
        /* Put the rest of the line into parser FSM */
        for (n = cli_line_current(parser);
             n < cli_line_last(parser); n++)
          {
          rc = cli_fsm_input(parser, cli_line_char(parser, n));
          }
        }
      else
        {
        cli_line_insert(parser, ch);
        }
      break;
    }
    case CLI_CHAR_UP_ARROW:
    {
      rc = cli_line_prev_line(parser);

      /* Reset the token stack and re-enter the command */
      cli_fsm_reset(parser);
      for (n = 0; n < cli_line_current(parser); n++)
        {
        rc = cli_fsm_input(parser, cli_line_char(parser, n));
        }

      return s_ok;
    }
    case CLI_CHAR_DOWN_ARROW:
    {
      rc = cli_line_next_line(parser);

      /* Reset the token stack and re-enter the command */
      cli_fsm_reset(parser);
      for (n = 0; n < cli_line_current(parser); n++)
        {
        rc = cli_fsm_input(parser, cli_line_char(parser, n));
        }

      return s_ok;
    }
    case CLI_CHAR_LEFT_ARROW:
    {
      ch = cli_line_prev_char(parser);
      if (!ch)
        {
        stream_putc(parser->cfg.console_out, '\a');
        return s_ok;
        }
      break;
    }
    case CLI_CHAR_RIGHT_ARROW:
    {
      ch = cli_line_next_char(parser);
      if (!ch)
        {
        stream_putc(parser->cfg.console_out, '\a');
        return s_ok;
        }
      break;
    }
    case CLI_CHAR_FIRST:
    {
      do
        {
        ch = cli_line_prev_char(parser);
        if (ch)
          {
          cli_fsm_input(parser, ch);
          }
        }
      while (ch);
      return s_ok;
    }
    case CLI_CHAR_LAST:
    {
      do
        {
        ch = cli_line_next_char(parser);
        if (ch)
          {
          cli_fsm_input(parser, ch);
          }
        }
      while (ch);
      return s_ok;
    }
    default:
    {
      /* An unknown character. Alert and continue */
      stream_putc(parser->cfg.console_out, '\a');
      return e_unexpected;
    }
    } /* switch (ch_type) */

  /* Handle special characters */
  if (ch == parser->cfg.ch_complete)
    {
    cli_complete_one_level(parser);
    return s_ok;
    }
  else if (ch == parser->cfg.ch_help)
    {
    /* Ask for context sensitve help */
    cli_help(parser);
    return s_ok;
    }
  else if ('\n' == ch)
    {
    rc = cli_execute_cmd(parser);
    cli_line_advance(parser);
    return rc;
    }

  return cli_fsm_input(parser, (char) ch);
  }

result_t cli_run(cli_t *parser)
  {
  char ch;
  cli_char_t ch_type = 0;

  if (!VALID_PARSER(parser))
    return e_bad_parameter;

  cli_print_prompt(parser);
  parser->done = 0;

  while (!parser->done)
    {
    stream_getc(parser->cfg.console_in, &ch);
    if ('\x1B' == ch)
      {
      stream_getc(parser->cfg.console_in, &ch);
      if ('[' == ch)
        {
        stream_getc(parser->cfg.console_in, &ch);
        switch (ch)
          {
          case 'A':
            ch_type = CLI_CHAR_UP_ARROW;
            break;
          case 'B':
            ch_type = CLI_CHAR_DOWN_ARROW;
            break;
          case 'C':
            ch_type = CLI_CHAR_RIGHT_ARROW;
            break;
          case 'D':
            ch_type = CLI_CHAR_LEFT_ARROW;
            break;
          }
        }
      }
    else if (isalnum(ch) || ('\n' == ch) ||
             ispunct(ch) || (' ' == ch) ||
             (ch == parser->cfg.ch_erase) ||
             (ch == parser->cfg.ch_del) ||
             (ch == parser->cfg.ch_help) ||
             (ch == parser->cfg.ch_complete))
      {
      ch_type = CLI_CHAR_REGULAR;
      }

    cli_input(parser, ch, ch_type);
    } /* while not done */

  return s_ok;
  }

result_t cli_init(cli_cfg_t *cfg, cli_t *parser)
  {
  int n;

  if (!parser || !cfg || !cfg->root || !cfg->ch_erase)
    {
    return e_bad_parameter;
    }

  parser->cfg = *cfg;

  /* Initialize sub-mode states */
  parser->root_level = 0;

  parser->root[0] = parser->cfg.root;       // root node
  strncpy(parser->prompt[0], parser->cfg.prompt, MAX_PROMPT_LENGTH);   // root prompt
  parser->current[0] = 0;                   // root memid in the registry

  for(n = 1; n < CLI_MAX_NESTED_LEVELS; n++)
    parser->prompt[n][0] = 0;

  /* Initialize line buffering states */
  parser->max_line = 0;
  parser->cur_line = 0;

  // TODO: make lines dynamic.....
  for (n = 0; n < CLI_MAX_LINES; n++)
    cli_line_init(&parser->lines[n]);

  for (n = 0; n < CLI_MAX_NUM_TOKENS; n++)
    memset(&parser->tokens[n], 0, sizeof(cli_token_t));

    /* Initialize parser FSM state */
  cli_fsm_reset(parser);

  /* Clear the user input state */
  cli_input_reset(parser);

  return s_ok;
  }

void cli_cleanup(cli_t *parser)
  {
  int n;

  for (n = 0; n < CLI_MAX_LINES; n++)
    {
    if(parser->lines[n].buffer != 0)
      kfree(parser->lines[n].buffer);

    parser->lines[n].buffer = 0;
    }

  for (n = 0; n < CLI_MAX_NUM_TOKENS; n++)
    {
    if(parser->tokens[n].token_buffer != 0)
      kfree(parser->tokens[n].token_buffer);

    memset(&parser->tokens[n], 0, sizeof(cli_token_t));
    }
  }

result_t cli_quit(cli_t *parser)
  {
  if (!parser)
    {
    return e_bad_parameter;
    }
  parser->done = 1;
  return s_ok;
  }

result_t cli_submode_enter(cli_t *parser, memid_t key, const char * prompt)
  {
  cli_node_t *new_root;

  if (!parser)
    return e_bad_parameter;

  if ((CLI_MAX_NESTED_LEVELS - 1) == parser->root_level)
    return e_unexpected;

  parser->root_level++;

  new_root = parser->cur_node;

  parser->root[parser->root_level] = new_root;
  strncpy(parser->prompt[parser->root_level], prompt, MAX_PROMPT_LENGTH);
  parser->current[parser->root_level] = key;

  return s_ok;
  }

result_t cli_submode_exit(cli_t *parser)
  {
  if (!parser)
    return e_bad_parameter;

  if (parser->root_level == 0)
    return e_unexpected;

  // release the dynamic prompt
  parser->prompt[parser->root_level][0] = 0;
  parser->current[parser->root_level] = 0;
  parser->root[parser->root_level] = 0;

  parser->root_level--;
  return s_ok;
  }

static result_t cli_walk_internal(cli_t *parser, cli_node_t *node,
                  cli_walker_fn pre_fn, cli_walker_fn post_fn,
                  void *cookie)
  {
  result_t rc;
  cli_node_t *cur_node;

  if (pre_fn)
    {
    rc = pre_fn(parser, node, cookie);
    if (s_ok != rc)
      {
      return rc;
      }
    }

  if (CLI_NODE_END != node->type)
    {
    cur_node = node->children;
    while (cur_node)
      {
      cli_walk_internal(parser, cur_node, pre_fn, post_fn, cookie);
      cur_node = cur_node->sibling;
      }
    }

  if (post_fn)
    {
    rc = post_fn(parser, node, cookie);
    if (s_ok != rc)
      {
      return rc;
      }
    }

  return s_ok;
  }

result_t cli_walk(cli_t *parser, cli_walker_fn pre_fn,
         cli_walker_fn post_fn, void *cookie)
  {
  if (!VALID_PARSER(parser) || (!pre_fn && !post_fn))
    {
    return e_bad_parameter;
    }

  return cli_walk_internal(parser, parser->root[parser->root_level],
                           pre_fn, post_fn, cookie);
  }

typedef struct help_stack_
  {
  char *filter;
  int tos;
  cli_node_t *nodes[CLI_MAX_NUM_TOKENS + 2];
  } help_stack_t;

/**
 * \brief    Pre-order walker function used by cli_help_cmd().
 * \details  Its main function is to push into the help stack when recurse into
 *           the next level.
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the current parse tree node.
 * \param    cookie Pointer to the help stack.
 *
 * \return   Return s_ok always.
 */
static result_t cli_help_pre_walker(cli_t *parser, cli_node_t *node, void *cookie)
  {
  help_stack_t *hs = (help_stack_t *) cookie;

  hs->nodes[hs->tos] = node;
  hs->tos++;

  return s_ok;
  }

/**
 * \brief    Post-order walker function used by cli_help_cmd().
 * \details  Its main function is to print out a command description and to
 *           pop the help stack.
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the current parse tree node.
 * \param    cookie Pointer to the help stack.
 *
 * \return   Return s_ok always.
 */
static result_t cli_help_post_walker(cli_t *parser, cli_node_t *node, void *cookie)
  {
  help_stack_t *hs = (help_stack_t *) cookie;
  int n, do_print;

  if ((CLI_NODE_END == node->type) &&
      (!(node->flags & CLI_NODE_FLAGS_OPT_PARTIAL)))
    {
    do_print = 0;
    if (hs->filter)
      {
      /* We have a filter string. Check if it matches any keyword */
      for (n = 0; n < hs->tos; n++)
        {
        if (CLI_NODE_ENUM == hs->nodes[n]->type)
          {
          const enum_t *lnode = (const enum_t *) hs->nodes[n]->param;

          while (lnode->name != 0)
            {
            if (strstr(lnode->name, hs->filter))
              {
              do_print = 1;
              break;
              }
            lnode++;
            }
          if (do_print)
            {
            break;
            }
          }
        if (CLI_NODE_KEYWORD != hs->nodes[n]->type)
          {
          continue;
          }
        if (strstr(hs->nodes[n]->param, hs->filter))
          {
          do_print = 1; /* Yes, print it */
          break;
          }
        }
      }
    else
      do_print = 1;

    if (do_print)
      {
      cli_node_t *cur_node;
      int m, num_braces = 0;

      if (node->desc)
        {
        stream_puts(parser->cfg.console_out, node->desc);
        stream_puts(parser->cfg.console_out, "\r\n  ");
        }
      else
        {
        stream_puts(parser->cfg.console_out, "\r\n  ");
        }
      for (n = 0; n < hs->tos; n++)
        {
        cur_node = hs->nodes[n];
        if ((CLI_NODE_ROOT == cur_node->type) ||
            (CLI_NODE_END == cur_node->type))
          {
          continue;
          }
        if (cur_node->flags & CLI_NODE_FLAGS_OPT_START)
          {
          stream_puts(parser->cfg.console_out, "{ ");
          num_braces++;
          }
        cli_help_print_node(parser, cur_node, false, false);
        stream_putc(parser->cfg.console_out, ' ');
        if (cur_node->flags & CLI_NODE_FLAGS_OPT_END)
          {
          for (m = 0; m < num_braces; m++)
            {
            stream_puts(parser->cfg.console_out, "} ");
            }
          }
        }
      stream_puts(parser->cfg.console_out, "\r\n\n");
      }
    }

  /* Pop the stack */
  hs->tos--;
  return s_ok;
  }

result_t cli_help_cmd(cli_t *parser, char *str)
  {
  help_stack_t help_stack;

  memset(&help_stack, 0, sizeof (help_stack));
  help_stack.filter = str;
  return cli_walk(parser, cli_help_pre_walker,
                  cli_help_post_walker, &help_stack);
  }

result_t cli_user_input(cli_t *parser, const char *prompt, int do_echo, cli_input_cb cb)
  {
  bool tmp_do_echo;

  if (!parser)
    {
    return e_bad_parameter;
    }

  if (cli_is_user_input(parser, &tmp_do_echo))
    {
    return e_unexpected; /* only one user input at a time */
    }

  /* Print the prompt */
  if (prompt)
    stream_puts(parser->cfg.console_out, prompt);

  /* Save the state */
  parser->user_buf = 0;
  parser->user_input_cb = cb;
  parser->user_do_echo = do_echo;

  return s_ok;
  }
