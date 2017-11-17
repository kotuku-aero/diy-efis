#include "cli.h"
#include <string.h>


int cli_match(cli_t *parser,
              cli_token_t *token,
              cli_node_t *parent, cli_node_t **match, bool *is_complete)
  {
  int num_matches = 0;
  bool local_is_complete;
  cli_node_t *child;
  result_t rc;

  *match = 0;
  *is_complete = 0;
  for (child = parent->children; 0 != child; child = child->sibling)
    {
    local_is_complete = false;
    rc = cli_match_fn_tbl[child->type](parser, token, child, &local_is_complete);
    
    if (succeeded(rc))
      {
      num_matches++;
      /* 
       * Return only the highest priority match unless the lower 
       * priority match is complete and the higher one is only 
       * partially matched.
       */
      if (!(*match))
        {
        /* Return only the highest priority match */
        *match = child;
        *is_complete = local_is_complete;
        }
      else
        {
        if (!(*is_complete) && local_is_complete)
          {
          *match = child;
          *is_complete = local_is_complete;
          }
        }
      }
    }

  /* 
   * There is a only one match and it is a keyword. Consider this a 
   * complete match.
   */
  if ((1 == num_matches) && (CLI_NODE_KEYWORD == (*match)->type || CLI_NODE_PATH == (*match)->type))
    {
    *is_complete = true;
    }
  
  return num_matches;
  }

/**
 * Reset the token stack in parser FSM.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   None.
 */
static void cli_token_stack_reset(cli_t *parser)
  {
  int n;
  cli_token_t *token;

  parser->last_good = -1;
  parser->current_pos = 0;
  parser->token_tos = 0;
  for (n = 0; n < CLI_MAX_NUM_TOKENS; n++)
    {
    token = &parser->tokens[n];
    token->begin_ptr = 0;
    token->parent = 0;
    token->node = 0;
    if(token->buffer == 0)
      token->buffer = string_create(0);
    else
      string_clear(token->buffer);
    }
  }

/**
 * Process a BS in WHITESPACE state. 
 *
 * \details  There are two possibilities: 1) after erasing a character, 
 *           we are in the end of a token -> erase character/TOKEN, 
 *           2) after erasing a character, the previous character is 
 *           still a SPC -> erase character/WHITESPACE.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [BS].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_ws_erase(cli_t *parser, char ch, bool *ch_processed)
  {
  cli_token_t *token;

  if (parser->current_pos > 0)
    {
    *ch_processed = true;
    parser->current_pos--;
    if (0 < parser->token_tos)
      {
      token = &parser->tokens[parser->token_tos - 1];
      if (token->begin_ptr + string_length(token->buffer) >= parser->current_pos)
        {
        parser->cur_node = token->parent;
        token->parent = 0;

        /* Pop the token on top of the stack */
        token = CUR_TOKEN(parser);
        token->begin_ptr = 0;
        string_clear(token->buffer);
        token->parent = 0;
        token->node = 0;
        parser->token_tos--;
        return CLI_STATE_TOKEN;
        }
      }
    }
  else
    {
    *ch_processed = false;
    }
  return CLI_STATE_WHITESPACE;
  }

/**
 * Process a SPC in WHITESPACE state.
 *
 * \details  There are one possibility: 1) there is space left -> insert 
 *           SPC/WHITESPACE.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_ws_space(cli_t *parser, char ch, bool *ch_processed)
  {
  *ch_processed = true;
  return CLI_STATE_WHITESPACE;
  }

/**
 * Process a character in WHITESPACE state.
 *
 * \details  There are two possibilities: 1) the new character is a valid 
 *           token -> insert char/TOKEN, 2) the new character is an invalid 
 *           token -> insert char/ERROR.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_ws_char(cli_t *parser, char ch, bool *ch_processed)
  {
  cli_node_t *match;
  bool is_complete;
  cli_token_t *token;
  token = CUR_TOKEN(parser);

  *ch_processed = false;

  if (ch == '"')
    {
    if (token->in_string > 0)
      token->in_string--;
    else
      {
      token->in_string++;
      token->begin_ptr = parser->current_pos;
      return CLI_STATE_TOKEN;
      }
    }
  else
    /* A valid token found. Add to token stack */
    token->buffer = string_push_back(token->buffer, ch);

  *ch_processed = true;

  // TODO handle \" or \\n
  
  if (token->in_string == 0 &&
      !cli_match(parser, token, parser->cur_node, &match, &is_complete))
    {
    string_set_length(token->buffer, string_length(token->buffer)-1);
    return CLI_STATE_ERROR; /* no token match */
    }

  token->begin_ptr = parser->current_pos;
  return CLI_STATE_TOKEN;
  }

/**
 * Process a BS in TOKEN state.
 *
 * \details  There are two possibilities: 1) This is not the last character
 *           of the token -> delete character/TOKEN, 2) This is the last
 *           character of the token -> delete character/WHITESPACE.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_tok_erase(cli_t *parser, char ch, bool *ch_processed)
  {
  cli_token_t *token;

  token = CUR_TOKEN(parser);
  string_set_length(token->buffer, string_length(token->buffer)-1);

  parser->current_pos--;
  *ch_processed = true;
  if (string_length(token->buffer) == 0)
    {
    token->begin_ptr = 0;
    return CLI_STATE_WHITESPACE;
    }
  return CLI_STATE_TOKEN;
  }

/**
 * Process a character in TOKEN state.
 *
 * \details  There are three possibilities: 1) there are too many
 *           characters in the token already -> do nothing/
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_tok_char(cli_t *parser, char ch, bool *ch_processed)
  {
  cli_node_t *match;
  bool is_complete;
  cli_token_t *token = CUR_TOKEN(parser);

  *ch_processed = false;

  if (ch == '"')
    {
    if (token->in_string > 0)
      token->in_string--;
    else
      {
      token->in_string++;
      token->begin_ptr = parser->current_pos;
      return CLI_STATE_TOKEN;
      }
    }
  else
    {
    /* A valid token found. Add to token stack */
    if (string_length(token->buffer) < CLI_MAX_TOKEN_SIZE)
      token->buffer = string_push_back(token->buffer, ch);
    else
      return CLI_STATE_ERROR;
    }

  *ch_processed = true;

  if (token->in_string == 0 &&
      !cli_match(parser, token, parser->cur_node, &match, &is_complete))
    {
    string_set_length(token->buffer, string_length(token->buffer)-1);
    return CLI_STATE_ERROR;
    }

  return CLI_STATE_TOKEN;
  }

/**
 * Process a SPC in TOKEN state.
 *
 * \details  There are two possibilities: 1) The token was unique and 
 *           complete -> insert character, update cur_node/WHITESPACE,
 *           2) Otherwise -> insert character/ERROR.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_tok_space(cli_t *parser, char ch, bool *ch_processed)
  {
  cli_node_t *match;
  bool is_complete;
  cli_token_t *token = CUR_TOKEN(parser);

  if (token->in_string > 0)
    {
    return cli_tok_char(parser, ch, ch_processed);
    }

  *ch_processed = true;

  if ((1 <= cli_match(parser, token, parser->cur_node, &match, &is_complete)) &&
      (is_complete))
    {
    /* Save the parent node for this token and "close" the token */
    token->parent = parser->cur_node;
    token->node = match;

    /* Push it into the stack */
    parser->token_tos++;

    token = CUR_TOKEN(parser);

    parser->cur_node = match;

    return CLI_STATE_WHITESPACE;
    }
  return CLI_STATE_ERROR;
  }

/**
 * Process a BS in ERROR state.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_err_erase(cli_t *parser, char ch, bool *ch_processed)
  {
  *ch_processed = true;
  parser->current_pos--;
  if ((parser->last_good + 1) == parser->current_pos)
    {
    cli_token_t *token;
    token = CUR_TOKEN(parser);
    if (token->begin_ptr + string_length(token->buffer) >= parser->current_pos)
      {
      return CLI_STATE_TOKEN;
      }
    return CLI_STATE_WHITESPACE;
    }
  return CLI_STATE_ERROR;
  }

/**
 * Process a SPC in ERROR state.
 *
 * \details  There is one possibility: 1) -> insert SPC/ERROR
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_err_space(cli_t *parser, char ch, bool *ch_processed)
  {
  *ch_processed = true;
  return CLI_STATE_ERROR;
  }

/**
 * Process a character in ERROR state.
 *
 * \details  There is one possibility: 1) for any characters
 *           -> insert character/ERROR
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Character to be input which must [SPC].
 *
 * \retval   ch_processed 1 if the character is used; 0 if the character 
 *                        is rejected by the parser FSM.
 * \return   New parser state.
 */
static cli_state_t cli_err_char(cli_t *parser, char ch, bool *ch_processed)
  {
  *ch_processed = true;
  return CLI_STATE_ERROR;
  }

/* Define a table of function based on the (state, input type) */
typedef cli_state_t(*cli_state_func)(cli_t *parser, char ch, bool *ch_processed);
cli_state_func cli_state_func_tbl[CLI_MAX_STATES][3] = {
  { cli_ws_erase, cli_ws_space, cli_ws_char},
  { cli_tok_erase, cli_tok_space, cli_tok_char},
  { cli_err_erase, cli_err_space, cli_err_char}
  };

result_t cli_fsm_input(cli_t *parser, char ch)
  {
  int input_type;
  bool ch_processed;
  int n;
  int m;

  /*
   * We classify the input into one of 3 classes: backspace (BS),
   * whitespace (SPC), regular characters (CHAR). We also check
   * if the buffer is empty when erasing or the buffer is full 
   * when inserting. A more systematic approach is to check
   * this in 9 state-input functions. But checking here reduces
   * the amount of code by a good amount.
   */
  if ((parser->cfg.ch_erase == ch) || (parser->cfg.ch_del == ch))
    {
    /* 
     * Line buffer code already checks that there is character 
     * in token stack
     */
    input_type = 0;
    }
  else
    {
    if (' ' == ch)
      {
      input_type = 1;
      }
    else
      {
      input_type = 2;
      }
    }
  ch_processed = false;
  parser->state =
    cli_state_func_tbl[parser->state][input_type](parser, ch, &ch_processed);

  if (ch_processed)
    {
    if (0 != input_type)
      parser->current_pos++;

    if (CLI_STATE_ERROR != parser->state)
      parser->last_good = parser->current_pos - 1;
    }

  return s_ok;
  }

void cli_fsm_reset(cli_t *parser)
  {
  cli_token_stack_reset(parser);
  parser->cur_node = parser->root[parser->root_level];
  parser->state = CLI_STATE_WHITESPACE;
  }

