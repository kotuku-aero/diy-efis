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
#ifndef __CLI_H__
#define __CLI_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * CLI Parser assumes the following types are defined: uint8_t, int8_t,
 * uint16_t, int16_t, uint32_t, int32_t. In many platforms (including
 * all platforms with GNU toolchain), stdint.h defines these types.
 * Thus, including this file is sufficient. If your platform does not 
 * have these types defined, you need to define them here.
 */
#include "../neutron/neutron.h"
#include <stdint.h>
 ////////////////////////////////////////////////////////////////////////////////
 //
/**
* Split 's' with separator in 'sep'. An array
* of const char * strings is returned.
* @param s     string to split
* @param sep   separator to split string
* @return a vector of the strings as split.
*/
extern vector_t string_split(const char *s, char sep);
/**
* Free the result returned by string_split_len(), or do nothing if 'tokens' is 0.
* @param tokens
* @param count
*/
extern void kfree_split(vector_t tokens);

/**
 * Maximum number of nested sub-mode levels.
 */
#define CLI_MAX_NESTED_LEVELS  (4)

/*
Maximum size of the prompty
*/
#define MAX_PROMPT_LENGTH (32)

/**
 * Maximum number of characters in one token.
 */
#define CLI_MAX_TOKEN_SIZE     (256)

/**
 * Maximum number of token per line.
 */
#define CLI_MAX_NUM_TOKENS     (32)

/**
 * Maximum number of lines.
 */
#define CLI_MAX_LINES          (10)
#define CLI_MAX_LINE_LENGTH   (256)
/**
 * If defined, support some of Emacs key binding.
 */
#define CLI_EMACS_BINDING

/*
 * This is to match Cisco CLI behavior. For example, if there is a
 * command "show crypto interfaces", one can just enter "sh cry int"
 * if no other command has the same prefix form.
 */
#define SHORTEST_UNIQUE_KEYWORD

struct _cli_t;

/**
 * Parser node type.
 */
typedef enum {
    CLI_NODE_ROOT = 0,
    CLI_NODE_END,
    CLI_NODE_KEYWORD,
    CLI_NODE_ENUM,
    CLI_NODE_PATH,
    CLI_NODE_STRING,
    CLI_NODE_UINT16,
    CLI_NODE_UINT32,
    CLI_NODE_INT16,
    CLI_NODE_INT32,
    CLI_NODE_FLOAT,
    CLI_NODE_SCRIPT,
    CLI_NODE_MATRIX,
    CLI_NODE_XYZ,
    CLI_MAX_NODES
} cli_node_type_t;

/**
 * A node in the parser tree. It has a node type which determines
 * what type of token is accepted.
 */
typedef struct cli_node_ {
    cli_node_type_t   type;      /**< Token type */ 
    uint32_t              flags;     /**< Flags */
    void                  *param;    /**< Token-dependent parameter */
    char                  *desc;     /**< A per-node description string */
    /** Pointer to the next sibling in the same level of the tree */
    struct cli_node_        *sibling;
    /** Pointer to all its children in the next level of the tree */
    struct cli_node_        *children;
} cli_node_t;

/**
 * \struct   cli_token_t
 * \brief    A parsed token.
 */
typedef struct cli_token_ {
    /** Index (in the line) of the beginning of the token */
    uint16_t begin_ptr;    // if -1 then
    uint16_t token_buflen;
    uint16_t token_length;      // length of the token
    char *token_buffer;         // Local copy of the token

    // if this is > then a string character " was detected and will be matched
    uint16_t in_string;
    /** 
     * Pointer to the parent node whose one of its children matches
     * this token.
     */
    cli_node_t *parent;
    /**
     * Pointer to the parse node that matches this token. Note that this
     * field is only filled out when the token is pushed into the stack.
     */
    cli_node_t *node;
} cli_token_t;

#define CLI_NODE_FLAGS_OPT_START          (1 << 1)
#define CLI_NODE_FLAGS_OPT_END            (1 << 2)
#define CLI_NODE_FLAGS_OPT_PARTIAL        (1 << 3)

#define VALID_PARSER(p)  (p)

/** Return the current line index */
#define CURRENT_LINE(p)  ((p)->cur_line)

/**
 * \struct   cli_line_t
 * \brief    A parser line structure.
 * \details  A parser line represents a line of user input in the parser.
 */
typedef struct {
    uint16_t  current;   /**< Point to the current character. */
    /** Buffer that holds the user input characters. */
    uint16_t buflen;
    char *buffer;
} cli_line_t;

/**
 * Return the number of tokens in the cparser for a particular parsed command.
 */
#define CLI_NUM_TOKENS(p)      (p->token_tos)

/**
 * User input callback. This function is called after a user input 
 * is completed.
 */
typedef result_t (*cli_input_cb)(struct _cli_t *context, const char * buffer);

/**
 * \struct   cli_cfg_
 * \brief    Contains all configurable parameters of a parser.
 */
typedef struct cli_cfg_ {
    cli_node_t  *root;
    char ch_complete;
    char ch_erase;
    char ch_del;
    char ch_help;
    const char *prompt;
    int32_t  flags;
    handle_t console_in;
    handle_t console_out;
    handle_t console_err;
} cli_cfg_t;

/**
 * \brief    Parser FSM states.
 * \details  There are 3 possible states in parser FSM.
 */
typedef enum cli_state_ {
    CLI_STATE_WHITESPACE = 0,
    CLI_STATE_TOKEN,
    CLI_STATE_ERROR,
    CLI_MAX_STATES
} cli_state_t;

/**
 * \brief    CLI parser structrure.
 * \details  This structure contains all configuration and running states.
 *           of a parser instance.
 */
typedef struct _cli_t {
    /** Parser configuration structure */
    cli_cfg_t     cfg;
    /** Current nested level */
    uint16_t  root_level;
    /** Parse tree root node at different nested levels */
    cli_node_t *root[CLI_MAX_NESTED_LEVELS];
    /** Parser prompt at different nested levels */
    char prompt[CLI_MAX_NESTED_LEVELS][MAX_PROMPT_LENGTH];
    // current registry key that is open
    memid_t current[CLI_MAX_NESTED_LEVELS];
    /** Current node */
    cli_node_t    *cur_node;

    /********** FSM states **********/
    cli_state_t   state;       /**< Current state */
    int16_t             token_tos;   /**< Token stack top pointer */
    int16_t             current_pos; /**< Current cursor in the line */
    /** Last cursor position that is in a non-error state */
    int16_t             last_good;
    /** Token stack */
    cli_token_t   tokens[CLI_MAX_NUM_TOKENS]; /* parsed tokens */

    /********** Line buffering states **********/
    int16_t             max_line;
    int16_t             cur_line;
    cli_line_t          lines[CLI_MAX_LINES];

    /** Flag indicating if the parser should continue to except input */
    bool               done;   

    /********** User input **********/
    /** Pointer to the buffer provided by user for input */
    const char * user_buf;
    /** Whether to echo user input */
    bool               user_do_echo;
    /** Callback function when the input is complete */
    cli_input_cb  user_input_cb;

    /********** Last executed command **********/
    /** Index to the line buffer that holds the command */
    int               last_line_idx;
    /** Result code of the command */
    result_t  last_rc;
    /** End node of the command. 0 if the command is invalid. */
    cli_node_t    *last_end_node;
} cli_t;

typedef result_t (*cli_glue_fn)(cli_t *parser);
typedef result_t (*cli_token_fn)(char *token, int token_len,
                                             int *is_complete);

static inline memid_t get_context(cli_t *parser)
  {
  return parser->current[parser->root_level];
  }
/**
 * Character type return by cli_getch().
 */
typedef enum {
    CLI_CHAR_UNKNOWN = 0, /**< Character that cannot be classified */
    CLI_CHAR_REGULAR,     /**< All alpha-numeric + punctuation charcters */
    CLI_CHAR_UP_ARROW,    /**< Up arrow (recall previous command) */
    CLI_CHAR_DOWN_ARROW,  /**< Down arrow (recall next command) */
    CLI_CHAR_LEFT_ARROW,  /**< Left arrow (prev. character in the command) */
    CLI_CHAR_RIGHT_ARROW, /**< Right arrow (next character in the command) */
    CLI_CHAR_FIRST,       /**< Go to the first character in the command */
    CLI_CHAR_LAST,        /**< Go to the first character in the command */
    CLI_MAX_CHAR
} cli_char_t;

/**
 * \typedef  cli_walker_fn
 * \brief    Walker function prototype used in cli_walk().
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the current node being walked.
 * \param    cookie An opaque pointer passed from cli_walk().
 *
 * \return   s_ok to continue the walk; e_unexpected to abort.
 */
typedef result_t (*cli_walker_fn)(cli_t *parser, cli_node_t *node, void *cookie);

/**
 * \brief    Initialize a parser.
 *
 * \param    cfg Pointer to the parser configuration structure.
 *
 * \retval   parser Pointer to the initialized parser.
 * \return   s_ok if succeeded; e_unexpected if failed.
 */
extern result_t cli_init(cli_cfg_t *cfg, cli_t *parser);

/**
Cleanup and release all resources
*/
extern void cli_cleanup(cli_t *parser);

/**
 * \brief    Input a character to the parser.
 *
 * \param    parser  Pointer to the parser structure.
 * \param    ch      Character to be input.
 * \param    ch_type Character type.
 *
 * \return   s_ok if succeeded; e_unexpected if failed.
 */
extern result_t cli_input(cli_t *parser, char ch, cli_char_t ch_type);

/**
 * \brief    Run the parser. 
 * \details  This function is a wrapper around cli_input(). It first 
 *           calls cli_io_init(). Then, it calls cli_getch() and 
 *           feeds character into the parser until it quits.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   s_ok if succeeded; e_unexpected if failed.
 */
extern result_t cli_run(cli_t *parser);

/**
 * \brief    Walk the parse tree in the parser.
 *
 * \param    parser  Pointer to the parser structure.
 * \param    pre_fn  Walker function that called before tranverse its children.
 * \param    post_fn Walker function that called after transvere its children.
 * \param    cookie  An opaque pointer that is passed back to the caller via
 *                   callback functions 'pre_fn' and 'post_fn'.
 *
 * \return   s_ok if succeeded; e_unexpected if failed.
 */
extern result_t cli_walk(cli_t *parser, cli_walker_fn pre_fn,
                              cli_walker_fn post_fn, void *cookie);

/**
 * \brief    Walk the parser tree and generate a list of all available commands.
 *
 * \param    parser Pointer to the parser structure.
 * \param    str    Pointer to a filter string. If it is 0, all
 *                  commands in the parse tree are displayed. Otherwise,
 *                  only commands with keywords that contain 'str' as
 *                  a substring are displayed.
 *
 * \return   s_ok if succeeded; e_bad_parameter if 
 *           the parser structure is invalid.
 */
extern result_t cli_help_cmd(cli_t *parser, char *str);

/**
 * \brief    Exit a parser session.
 * \details  This call causes the parser to exit and returns from 
 *           cli_run().
 *
 * \param    parser - Pointer to the parser structure.
 *
 * \return   s_ok if succeeded; e_bad_parameter if failed.
 */
extern result_t cli_quit(cli_t *parser);

/**
 * Enter a submode
 * @param parser    The parser structure
 * @param key       Key to set the root for the mode
 * @param prompt    Prompt for the submode
 * @return s_ok if succeeded; e_bad_parameter if the input 
 *           parameters are invalid; e_unexpected if there too many levels 
 *           of submode already.
 */
extern result_t cli_submode_enter(cli_t *parser, memid_t key, const char * prompt);

/**
 * \brief    Leave a submode. 
 * \details  The previous mode context and prompt are automatically restored.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   s_ok if succeeded; e_bad_parameter if the input 
 *           parameters are invalid; e_unexpected if the parser has not 
 *           entered any submode.
 */
extern result_t cli_submode_exit(cli_t *parser);
/**
 * \brief    Read a raw string from the user via the terminal.
 *
 * \param    parser   Pointer to the parser structure.
 * \param    prompt   Pointer to a prompt string. If 0, nothing is printed.
 * \param    echoed   1 to echo the input; 0 otherwise.
 * \param    cb       Callback function that is called when input is complete.
 *
 * \return   s_ok if succeeded;
 *           e_bad_parameter if the input parameters are invalid.
 */
extern result_t cli_user_input(cli_t *parser, const char *prompt,
                                    int echoed, cli_input_cb cb);
/**
 * \brief    Match function pointer.
 * \details  This function pointer is the prototype of all
 *           CLI Parser match functions.
 *
 * \param    token     Pointer to the token.
 * \param    token_len Number of valid characters in the token.
 * \param    node      The parse tree node to be matched against.
 *
 * \retval   is_complete Return 1 from if the token matches the entire
 *                       token; 0 otherwise.
 * \return   s_ok if it is a complete or partial match; 
 *           e_unexpected if it does match the node.
 */
typedef result_t (*cli_match_fn)(cli_t *context, cli_token_t *token,  cli_node_t *node, bool *is_complete);

/**
 * \brief    Completion function pointer.
 * \details  This function pointer is the prototype of all
 *           CLI Parser match functions.
 *
 * \param    parser    Pointer to the parser.
 * \param    node      Pointer to the current matching parse node.
 * \param    token     Pointer to the token.
 * \param    token_len Number of valid characters in the token.
 */
typedef result_t (*cli_complete_fn)(cli_t *context,
                                    const cli_node_t *node,
                                    cli_token_t *token);

/**
 * \brief    Get function pointer.
 * \details  This function pointer is the prototype of all CLI Parser 
 *           get functions.
 *
 * \param    token     Pointer to the token.
 *
 * \retval   val       Pointer to the returned parameter value.
 * \return   s_ok if succeeded; e_unexpected otherwise.
 */
typedef result_t (*cli_get_fn)(cli_t *context, const cli_token_t *token, void *val);

extern cli_match_fn    cli_match_fn_tbl[CLI_MAX_NODES];
extern cli_complete_fn cli_complete_fn_tbl[CLI_MAX_NODES];


// this structure describes an enumeration
// MUST be sorted in name order or the completions won't work
// the values can be any order
typedef struct _enum_t {
  const char *name;
  uint16_t value;
  } enum_t;

/********** Token complete functions **********/
extern result_t cli_complete_keyword(cli_t *parser, const cli_node_t *node, cli_token_t *token);
extern result_t cli_complete_enum(cli_t *parser, const cli_node_t *node, cli_token_t *token);
extern result_t cli_complete_path(cli_t *parser, const cli_node_t *node, cli_token_t *token);

extern result_t match_path(cli_t *context, const char * path, bool ignore_wildcard, memid_t *key, vector_t *matches);

/********** Token get functions **********/
extern result_t cli_get_string(const cli_token_t *token, const char * *value);
extern result_t cli_get_path(const cli_token_t *token, const char * *value);
extern result_t cli_get_uint16(const cli_token_t *token, uint16_t *value);
extern result_t cli_get_uint32(const cli_token_t *token, uint32_t *value);
extern result_t cli_get_int16(const cli_token_t *token, int16_t *value);
extern result_t cli_get_int32(const cli_token_t *token, int32_t *value);
extern result_t cli_get_float(const cli_token_t *token, float *value);
extern result_t cli_get_script(const cli_token_t *token, const char * *value);
extern result_t cli_get_matrix(const cli_token_t *token, matrix_t *value);
extern result_t cli_get_xyz(const cli_token_t *token, xyz_t *value);
extern result_t cli_get_enum(const cli_token_t *token, const enum_t *enums, uint16_t *value);
extern result_t cli_get_bool(const cli_token_t *token, const enum_t *enums, bool *value);
extern result_t convert_string_to_enum(const char * token, const enum_t *enums, uint16_t *value);


/**
 * \brief    Print the CLI prompt.
 * \details  If in privileged mode, prepend a '+'.
 *
 * \param    parser Pointer to the parser structure.
 */
extern void cli_print_prompt(const cli_t *parser);
/**
 * Reset a line buffer into an "empty" state.
 *
 * \param    line Pointer to a line structure.
 *
 * \return   s_ok if succeeded; e_bad_parameter if the line 
 *           is 0.
 */ 
extern result_t cli_line_reset(cli_line_t *line);
/**
 * Initialize a line buffer.
 */
result_t cli_line_init(cli_line_t *line);
/**
 * Insert a character into a line buffer at the current position.
 *
 * \param    parser Pointer to a parser structure.
 * \param    ch     Character to be inserted.
 *
 * \return   s_ok if succeeded; e_bad_parameter if inputs
 *           are invalid; e_no_space if the line buffer is full.
 */
extern result_t cli_line_insert(cli_t *parser, char ch);

/**
 * Delete a character from the line buffer immediately before
 * the current position. The current position is moved back by one.
 *
 * \param    parser Pointer to a parser structure.
 *
 * \return   s_ok if succeeded; e_bad_parameter if inputs
 *           are invalid; e_not_found if the line buffer is empty.
 */
extern result_t cli_line_delete(cli_t *parser);

/**
 * Move the current position of the current line to the next character.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   A character to be fed into parser FSM; 0 if no character
 *           is to be fed.
 */
extern char cli_line_next_char(cli_t *parser);

/**
 * Move the current position of the current line to the previous character.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   A character to be fed into parser FSM; 0 if no character
 *           is to be fed.
 */
extern char cli_line_prev_char(cli_t *parser);

/**
 * Move the current line to the next line.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   s_ok if succeeded; e_bad_parameter if inputs
 *           are invalid.
 */
extern result_t cli_line_next_line(cli_t *parser);

/**
 * Move the current line to the previous line.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   s_ok if succeeded; e_bad_parameter if inputs
 *           are invalid.
 */
extern result_t cli_line_prev_line(cli_t *parser);

/**
 * Print the current line including the current line.
 *
 * \param    parser       Pointer to the parser structure.
 * \param    print_prompt 1 if the parser prompt should be printed first.
 * \param    new_line     1 if a linefeed should be printed first;
 *                        0 otherwise.
 *
 * \return   None. Crash on failure.
 */
extern result_t cli_line_print(const cli_t *parser, int print_prompt, int new_line);

/**
 * Return the current position of the current line.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   Current position of the current line.
 */
extern uint16_t cli_line_current(const cli_t *parser);

/**
 * Return the last position of the current line.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   Last position of the current line. This index points to
 *           the terminating 0 character of the current line.
 */
extern uint16_t cli_line_last(const cli_t *parser);

/**
 * Return the character of the the current position of the current line.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   Character of the current position of the current line.
 */
extern char cli_line_current_char(const cli_t *parser);

extern char cli_line_char(const cli_t *parser, uint16_t pos);

extern result_t cli_line_advance(cli_t *parser);
#define CUR_TOKEN(p) (&((p)->tokens[(p)->token_tos]))

/**
 * Reset all parser FSM states.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   None.
 */
extern void cli_fsm_reset(cli_t *parser);

/**
 * Input a character to parser FSM.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Input character.
 *
 * \return   s_ok if succeeded; e_unexpected otherwise.
 */
extern result_t cli_fsm_input(cli_t *parser, char ch);

/**
 * Walk through all children of a node. Return a match node if one is found.
 *
 * \param    parser    Pointer to the parser structure.
 * \param    token     Pointer to the beginning of the token.
 * \param    parent    Pointer to the parent node.
 *
 * \retval   match       Pointer to a node that matches the token.
 *                       If there are multiple matches, the highest priority match
 *                       is returned.
 * \retval   is_complete 1 if the token completely matches 
 * \return   Number of matches.
 */
extern int cli_match(cli_t *parser, cli_token_t *token,
                  cli_node_t *parent, cli_node_t **match,
                  bool *is_complete);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CLI_H__ */
