/**
 * \file     cli_token_tbl.c
 * \brief    Parser token processing tables.
 * \version  \verbatim $Id: cli_token_tbl.c 152 2011-09-24 09:16:00Z henry $ \endverbatim
 */
/*
 * Copyright (c) 2008-2009, 2011, Henry Kwok
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the project nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY HENRY KWOK ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL HENRY KWOK BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cli.h"
#include <string.h>

/********** Token match functions **********/
extern result_t cli_match_root(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_end(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_keyword(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_string(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_path(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_uint(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_int(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_float(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_script(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_matrix(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_xyz(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);
extern result_t cli_match_enum(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete);

/**
 * \brief    An table of match functions.
 * \details  This array is indexed by CLI Parser node type. Each element
 *           contains a function pointer of a match function that checks
 *           if a token conforms to a certain node type.
 */
cli_match_fn cli_match_fn_tbl[] =
  {
  cli_match_root,
  cli_match_end,
  cli_match_keyword, 
  cli_match_enum,
  cli_match_path,
  cli_match_string,
  cli_match_uint,     // uint16
  cli_match_uint,     // uint32
  cli_match_int,      // int16
  cli_match_int,      // int32
  cli_match_float,
  cli_match_script,
  cli_match_matrix,
  cli_match_xyz
  };

/**
 * \brief    An table of completion functions.
 * \details  This array is indexed by CLI Parser node type. Each element
 *           contains a function pointer of a completion function that
 *           attempts to complete a token given its node type.
 */
cli_complete_fn cli_complete_fn_tbl[] = 
  {
  0,                     // root
  0,                     // end
  cli_complete_keyword,     // keyword
  cli_complete_enum,        // enumeration type
  cli_complete_path,        // a path
  0,                     // string
  0,                     // uint16
  0,                     // uint32
  0,                     // int16
  0,                     // int32
  0,                     // float
  0,                     // script
  0,                     // matrix
  0                      // xyz
  };
