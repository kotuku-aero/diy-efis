#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "cli.h"

result_t match_path(cli_t *context, const char * path, bool ignore_wildcard, memid_t *key, vector_t *matches)
  {
  result_t result;
  if (matches == 0)
    return e_bad_parameter;

  memid_t search_dir = get_context(context);
  *matches = 0;

  // handle some special cases first
  if (strcmp(path, "/") == 0)
    {
    if(key != 0)
      *key = 0;
    return s_ok;
    }

  if (strcmp(path, "..") == 0)
    {
    if (key == 0)
      return e_bad_parameter;

    if (failed(result = reg_query_memid(search_dir, 0, 0, 0, key)))
      return result;

    return s_ok;
    }

  if (strcmp(path, ".") == 0)
    {
    if (key == 0)
      return e_bad_parameter;

    *key = search_dir;
    return s_ok;
    }

  if (path[0] == '/')
    search_dir = 0;         // absolute path

  // split the path into directories
  vector_t directories = string_split(path, '/');

  uint16_t num_dirs;
  if (failed(result = vector_count(directories, &num_dirs)))
    {
    kfree_split(directories);
    return result;
    }

  // if empty string then we are done.
  if (num_dirs == 0)
    {
    *matches = 0;

    if(key != 0)
      *key = 0;
    return s_ok;
    }

  if (key != 0)
    *key = search_dir;

  // walk the path, respect the . and .. in the path
  uint16_t dir_num;
  for (dir_num = 0; dir_num < num_dirs - 1; dir_num++)
    {
    const char * dirname;
    if (failed(vector_at(directories, dir_num, &dirname)))
      {
      kfree_split(directories);
      return result;
      }

    if (strcmp(dirname, ".") == 0)
      continue;
    else if (strcmp(dirname, "..")== 0)
      {
      if (failed(result = reg_query_memid(search_dir, 0, 0, 0, &search_dir)))
        {
        kfree_split(directories);
        return result;
        }
      }
    else if (failed(result = reg_open_key(search_dir, dirname, &search_dir)))
      {
      kfree_split(directories);
      return result;
      }

    if (key != 0)
      *key = search_dir;
    }

  const char * fname;
  if (failed(vector_at(directories, dir_num, &fname)))
    {
    kfree_split(directories);
    return result;
    }

  char name[REG_NAME_MAX + 1];
  memid_t child = 0;
  field_datatype dt = field_none;
  uint16_t fname_length = strlen(fname);

  do
    {
    if (failed(result = reg_enum_key(search_dir, &dt, 0, 0, REG_NAME_MAX, name, &child)))
      {
      if (result == e_not_found)
        result = s_ok;
      break;
      }

    bool add_name = true;
    uint16_t i;
    for (i = 0; i < fname_length && i < REG_NAME_MAX; i++)
      {
      if (ignore_wildcard && (fname[i] == '*' || fname[i] == '?'))
        break;            // found a wildcard so use this name

      if (fname[i] != name[i])
        {
        add_name = false;
        break;        // no match, either length or character
        }
      }

    // full match see if we add it
    if (add_name)
      {
      if (*matches == 0)
        {
        if (failed(result = vector_create(sizeof(char *), matches)))
          {
          kfree_split(directories);
          return result;
          }
        }

      // add the string.
      char * new_name = strdup(name);
      if (failed(vector_push_back(*matches, &new_name)))
        {
        kfree_split(directories);
        return result;
        }
      }

    dt = field_none;

    } while (child != 0);
  
  // release the arrays
  kfree_split(directories);
  return s_ok;
  }

/***********************************************************************
 * TOKEN MATCH FUNCTIONS - These functions are used by cli_match()
 *     to check if a token matches a node type.
 ***********************************************************************/

/*
 * cli_match_root - Token matching function for root. There is no
 *     parsing for root node. Parser should crash if this function
 *     is ever called.
 */
result_t cli_match_root(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  return e_unexpected;
  }

/*
 * cli_match_end - Token matching function for end. There is no
 *     parsing for end node. It always returns no match.
 */
result_t cli_match_end(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  return e_unexpected;
  }

/*
 * cli_match_keyword - Token matching function for keywords. It can
 *     be alphanumeric string with '-' and '_'.
 */
result_t cli_match_keyword(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  int kw_len;
  int match_len;

  //assert(token && node && (CLI_NODE_KEYWORD == node->type) && is_complete);

  uint16_t len = strlen(token->buffer);
  //kw_len = strnlen(node->param, CLI_MAX_TOKEN_SIZE);
  const char *kw = (const char *)node->param;
  kw_len = strlen(kw);
  if (len > kw_len)
    {
    *is_complete = 0;
    return e_unexpected;
    }

  match_len = (kw_len < len ? kw_len : len);

  if (!strncmp(token->buffer, node->param, match_len))
    {
    *is_complete = (match_len == kw_len);
    return s_ok;
    }

  *is_complete = 0;
  return e_unexpected;
  }

result_t cli_match_enum(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  uint16_t len = strlen(token->buffer);
  const enum_t *cp;
  uint16_t kw_len;

  for (cp = (const enum_t *)node->param; cp->name != 0; cp++)
    {
    kw_len = (uint16_t)strlen(cp->name);
    if (len > kw_len)
      continue;

    if (strncmp(token->buffer, cp->name, len) == 0)
      {
      *is_complete = (kw_len == len);
      return s_ok;
      }
    }

  *is_complete = false;
  return e_unexpected;
  }

/*
 * cli_match_string - Token matching function for strings. There is
 *     no restriction on strings.
 */
result_t cli_match_string(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {

  *is_complete = 1;
  uint16_t len = strlen(token->buffer);

  return len < REG_STRING_MAX ? s_ok : e_unexpected;
  }

result_t cli_match_path(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  memid_t dir;
  vector_t matched_filenames = 0;
  result_t result;
    
  if (failed(result = match_path(context, token->buffer, true, &dir, &matched_filenames)))
    return result;

  if (matched_filenames == 0)
    *is_complete = true;
  else
    {
    uint16_t num_matches;
    if (failed(result = vector_count(matched_filenames, &num_matches)))
      {
      kfree_split(matched_filenames);
      return result;
      }

    if (num_matches == 1)
      {
      const char * str;
      if(failed(result = vector_at(matched_filenames, 0, &str)))
        {
        kfree_split(matched_filenames);
        return result;
        }

      *is_complete = strcmp(str, token->buffer) == 0;
      }
    else
      *is_complete = false;
    }

  kfree_split(matched_filenames);
  return s_ok;
  }

/*
 * cli_match_uint - Token matching function for 16-bit or 32-bit unsigned
 *     decimals or hexadecimals. Match against /[0-9]+|0x[0-9a-fA-F]+/.
 */
result_t cli_match_uint(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  int n, is_dec = 1;

  //assert(token && node && is_complete &&
  //       ((CLI_NODE_UINT16 == node->type) ||
  //        (CLI_NODE_UINT32 == node->type)));

  *is_complete = 0;
  //assert(token_len > 0);
  const char *str = token->buffer;
  uint16_t len = strlen(token->buffer);

  /* The first character must be 0-9 */
  if (!isdigit(str[0]))
    return e_unexpected;

  if (1 == len)
    {
    *is_complete = 1;
    return s_ok;
    }

  /* The 2nd character (optional) must be 0-9 or 'x' */
  if ('x' == str[1])
    {
    if ('0' != str[0])
      {
      return e_unexpected;
      }
    is_dec = 0;
    }
  else if (!isdigit(str[1]))
    {
    return e_unexpected;
    }
  if (2 == len)
    {
    *is_complete = is_dec;
    return s_ok;
    }

  /*
   * The rest of characters are either decmial or hex depending on
   * the first 2 characters are equal to '0x' or not
   */
  if (is_dec)
    {
    for (n = 2; n < len; n++)
      {
      if (!isdigit(str[n]))
        return e_unexpected;
      }
    }
  else
    {
    for (n = 2; n < len; n++)
      {
      if (!isxdigit(str[n]))
        return e_unexpected;
      }
    }
  *is_complete = 1;
  return s_ok;
  }

/*
 * cli_match_int - Token matching function for 32-bit or 64-bit signed
 *     decimals. Match against /[01]{0,1}[0-9]+/.
 */
result_t cli_match_int(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  int n;

  //assert(token && node && is_complete &&
  //       ((CLI_NODE_INT16 == node->type) ||
  //        (CLI_NODE_INT32 == node->type)));

  *is_complete = 0;
  const char *str = token->buffer;
  uint16_t len = strlen(token->buffer);


  /* 1st digit can be 0-9,-,+ */
  if (!isdigit(str[0]) && ('-' != str[0]) && ('+' != str[0]))
    return e_unexpected;

  if (1 == len)
    {
    if (isdigit(str[0]))
      *is_complete = 1;
    return s_ok;
    }

  /* All subsequent characters must be digits */
  for (n = 1; n < len; n++)
    {
    if (!isdigit(str[n]))
      return e_unexpected;
    }

  *is_complete = 1;
  return s_ok;
  }

/*
 * cli_match_hex - Token matching function for 16-bit or 32-bit
 *     hexadecimals.
 */
result_t cli_match_hex(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  int n;

  //assert(token && node && is_complete &&
  //       ((CLI_NODE_HEX16 == node->type) ||
  //        (CLI_NODE_HEX32 == node->type)));
  const char *str = token->buffer;
  uint16_t len = strlen(token->buffer);


  *is_complete = 0;
  //assert(len > 0);
  if ('0' != str[0])
    return e_unexpected;

  if (1 == len)
    return s_ok;

  if ('x' != str[1])
    return e_unexpected;

  if (2 == len)
    return s_ok;

  for (n = 2; n < len; n++)
    {
    if (!isxdigit(str[n]))
      return e_unexpected;
    }

  *is_complete = 1;
  return s_ok;
  }

/**
 * Match a floating point
 * @param str     String pointer to update
 * @param terms   valid terminating chars
 * @return s_ok if a floating point was matched
 */
static result_t match_float(const char **_str, const char *terms)
  {
  bool has_dec_pt = false;
  const char *str = *_str;
  /* Handle '+', '-' separately */
  if (('+' == *str) || ('-' == *str))
    {
    str++;
    if(*str == 0)
      {
      *_str = str;
      return s_ok;
      }
    }

  /* Handle a first '.' */
  if ('.' == *str)
    {
   str++;
    if(*str == 0)
      {
      *_str = str;
      return s_ok;
      }

    has_dec_pt = true;
    }
  else if (!isdigit(*str))
    return e_unexpected;

  str++;
  if(*str == 0)
    {
    *_str = str;
    return s_ok;
    }

  /* Work on the rest of them */
  while(*str != 0)
    {
    if(terms != 0)
      {
      const char *tp = terms;
      while(*tp != 0)
        {
        if(*tp == *str)
          {
          *_str = str;
          return s_ok;
          }

        tp++;
        }
      }
    if('.' == *str)
      {
      if(has_dec_pt)
        return e_unexpected;

      has_dec_pt = true;
      str++;
      if(*str == 0)
        {
        *_str = str;
        return s_ok;
        }
      }

    if (!isdigit(*str))
      return e_unexpected;
    str++;
    if(*str == 0)
      {
      *_str = str;
      return s_ok;
      }

    }

  *_str = str;
  return s_ok;
  }

/*
 * cli_match_float - Token matching function for double precision floating
 *     point value.
 */
result_t cli_match_float(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  //assert(token && node && (CLI_NODE_FLOAT == node->type) && is_complete);
  const char *str = token->buffer;

  result_t result = match_float(&str, 0);
  *is_complete = *str == 0;
  return result;
  }

result_t cli_match_bool(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  uint16_t i;
  //assert(token && node && (CLI_NODE_FLOAT == node->type) && is_complete);
  uint16_t len = strlen(token->buffer);

  *is_complete = false;

  if(isdigit(token->buffer[0]))
    {
    for(i = 0; i < len; i++)
      if(!isdigit(token->buffer[i]))
        return e_unexpected;

    *is_complete = true;
    return s_ok;
    }
  else if (strncmp(token->buffer, "true", len) == 0)
    {
    *is_complete = strlen(token->buffer) == 4;
    return s_ok;
    }
  else if(strncmp(token->buffer, "false", len) == 0)
    {
    *is_complete = strlen(token->buffer) == 5;
    return s_ok;
    }

  return e_unexpected;
  }

result_t cli_match_script(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  *is_complete = 1;
  return s_ok;
  }

static result_t match_tuple(const char **_str, bool *is_complete)
  {
  result_t result;
  const char *str = *_str;

  if(*str != '[')
    return e_unexpected;

  str++;

  // partial match
  if(*str == 0)
    return s_ok;

  if(failed(result = match_float(&str, ",")))
    return result;

  if(*str == ',')
    str++;

  // partial match
  if(*str == 0)
    return s_ok;

  if(failed(result = match_float(&str, ",")))
    return result;

  if(*str == ',')
    str++;

  if(*str == 0)
    return s_ok;

  if(failed(result = match_float(&str, "]")))
    return result;

  *is_complete = *str == ']';

  if(*is_complete)
    str++;

  *_str = str;
  return s_ok;
  }

result_t cli_match_xyz(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  *is_complete = false;
  // a matrix is defined as [<float>,<float>,<float>]
  const char *str = token->buffer;
  return match_tuple(&str, is_complete);
  }

result_t cli_match_matrix(cli_t *context, cli_token_t *token, cli_node_t *node, bool *is_complete)
  {
  *is_complete = false;
  // a matrix is defined as [<float>,<float>,<float>][<float>,<float>,<float>][<float>,<float>,<float>]
  const char *str = token->buffer;
  result_t result;

  if(failed(result = match_tuple(&str, is_complete)) ||
     !*is_complete)
     return result;

  *is_complete = false;

  if(*str == 0)
    return s_ok;

   if(failed(result = match_tuple(&str, is_complete)) ||
     !*is_complete)
     return result;

  *is_complete = false;

  if(*str == 0)
    return s_ok;

   return match_tuple(&str, is_complete);
  }

/***********************************************************************
 * TOKEN COMPLETE FUNCTIONS - These functions are used by
 *     cli_complete_one_level() to provide context-sensitive
 *     completion.
 ***********************************************************************/
result_t cli_complete_keyword(cli_t *parser, const cli_node_t *node, cli_token_t *token)
  {
  result_t result;
  const char *ch_ptr;

  uint16_t len = strlen(token->buffer);

  ch_ptr = ((const char *)node->param) + len;
  while(*ch_ptr)
    {
    if(failed(result = cli_input(parser, *ch_ptr, CLI_CHAR_REGULAR)))
      return result;

    ch_ptr++;
    }
  return s_ok;
  }

result_t cli_complete_path(cli_t *context, const cli_node_t *node, cli_token_t *token)
  {
  result_t result;
  memid_t memid = 0;
  int16_t match_len = -1;
  int16_t n;

  const char * name;
  const char * match_name = 0;

  // match the path name to the first found name
  vector_t matched_paths = 0;
  if (failed(result = match_path(context, token->buffer, false, &memid, &matched_paths)))
    {
    return result;      // not sure how we got here, just bail...
    }

  // find the last part of the path
  vector_t directories = string_split(token->buffer, '/');
  uint16_t num_matches;
  if (failed(result = vector_count(directories, &num_matches)))
    {
    kfree_split(directories);
    kfree_split(matched_paths);
    return result;
    }

  const char * last_path = 0;
  const char * empty_str = 0;

  if (failed(result = vector_at(directories, num_matches - 1, &last_path)) ||
      failed(result = vector_set(directories, num_matches -1, &empty_str)))
    {
    kfree_split(directories);
    kfree_split(matched_paths);
    return result;
    }

  kfree_split(directories);
  directories = 0;
  uint16_t len = strlen(last_path);

  if (failed(result = vector_count(matched_paths, &num_matches)))
    {
    kfree(last_path);
    kfree_split(matched_paths);
    return result;
    }

  uint16_t match_num;
  // find the best match for the string
  for (match_num = 0; match_num < num_matches; match_num++)
    {
    if(failed(result = vector_at(matched_paths, match_num, &name)))
      {
      kfree(last_path);
      kfree_split(matched_paths);
      return result;
      }
    if (strncmp(name, last_path, len) == 0)
      {
      /* Prefix matches. See what is the longest suffix */
      if (match_len == -1)
        {
        /* First match. Cover the whole thing */
        match_len = strlen(name);

        match_name = name;
        }
      else
        {
        /* Second and after matches. Intersect with previous match */
        for (n = len; n < match_len; n++)
          {
          if (name[n] != match_name[n])
            {
            break;
            }
          }
        match_len = n;
        }
      }
    }

  if (match_name == 0)
    {
    kfree(last_path);
    kfree_split(matched_paths);
    return e_unexpected;
    }

  for (n = len; n < match_len; n++)
    if (failed(result = cli_input(context, match_name[n], CLI_CHAR_REGULAR)))
      return result;

  kfree(last_path);
  kfree_split(matched_paths);

  return s_ok;
  }

result_t cli_complete_enum(cli_t *parser, const cli_node_t *node, cli_token_t *token)
  {
  result_t result;
  int16_t match_len = -1;
  int16_t n;
  uint16_t len = strlen(token->buffer);
  const enum_t *id;
  const enum_t *match_id;

  // find the best match for the string
  for (id = (const enum_t *)node->param; id->name != 0; id++)
    {
    if (strncmp(id->name, token->buffer, len) == 0)
      {
      /* Prefix matches. See what is the longest suffix */
      if (match_len == -1)
        {
        /* First match. Cover the whole thing */
        match_len = strlen(id->name);

        match_id = id;
        }
      else
        {
        /* Second and after matches. Intersect with previous match */
        for (n = len; n < match_len; n++)
          {
          if (id->name[n] != match_id->name[n])
            {
            break;
            }
          }
        match_len = n;
        }
      }
    }

  if (match_id->name == 0)
    return e_unexpected;


  for (n = len; n < match_len; n++)
    if(failed(result = cli_input(parser, match_id->name[n], CLI_CHAR_REGULAR)))
      return result;

  return s_ok;
  }

/***********************************************************************
 * TOKEN GET FUNCTIONS - These functions are used by glue functions
 *     to extract the parameters and call the action function.
 ***********************************************************************/
/*
 * cli_get_string - Token get function for a string.
 */
result_t cli_get_string(const cli_token_t *token, const char * *value)
  {
  if(token == 0 || value == 0)
    return e_bad_parameter;

  if(strlen(token->buffer) == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  *value = token->buffer;
  return s_ok;
  }

result_t cli_get_path(const cli_token_t *token, const char * *value)
  {
  if (token == 0 || value == 0)
    return e_bad_parameter;

  if (strlen(token->buffer) == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  *value = token->buffer;
  return s_ok;
  }

result_t convert_string_to_enum(const const char * token, const enum_t *enums, uint16_t *value)
  {
  if (token == 0 || value == 0)
    return e_bad_parameter;

  if (strlen(token) == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  if (isdigit(*token))
    {
    // numeric
    errno = 0;
    uint32_t v32 = strtoul(token, 0, 0);

    if (errno != 0)
      return e_bad_parameter;

    if (v32 > UINT16_MAX)
      return e_bad_parameter;

    *value = (uint16_t)v32;
    return s_ok;
    }

  // look up string...
  const enum_t *id;
  for (id = enums; id->name != 0; id++)
    {
    if (strcmp(token, id->name) == 0)
      {
      *value = id->value;
      return s_ok;
      }
    }
  return e_not_found;
  }

result_t cli_get_enum(const cli_token_t *token, const enum_t *enums, uint16_t *value)
  {
  if (token == 0 || value == 0)
    return e_bad_parameter;

  return convert_string_to_enum(token->buffer, enums, value);
  }

result_t cli_get_bool(const cli_token_t *token, const enum_t *enums, bool *value)
  {
  if (token == 0 || value == 0)
    return e_bad_parameter;

  uint16_t val = 0;
  result_t result;
  if (failed(result = convert_string_to_enum(token->buffer, enums, &val)))
    return result;

  *value = val != 0;
  return s_ok;
  }

/*
 * cli_get_uint - Token get function for 16-bit unsigned integer.
 */
result_t cli_get_uint16(const cli_token_t *token, uint16_t *value)
  {
  result_t result;
  if(token == 0 || value == 0)
    return e_bad_parameter;

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;

  if (len == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  errno = 0;
  uint32_t tmp = strtoul(token->buffer, 0, 0);

  if (errno != 0)
    return e_bad_parameter;

  if(tmp > 65535)
    return e_bad_parameter;

  *value = (uint16_t)tmp;
  return s_ok;
  }

/*
 * cli_get_uint32 - Token get function for 64-bit unsigned integer.
 */
result_t cli_get_uint32(const cli_token_t *token, uint32_t *value)
  {
  if(token == 0 || value == 0)
    return e_bad_parameter;

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;

  if (len == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  errno = 0;
  *value = strtoul(token->buffer, 0, 0);

  if (errno != 0)
    return e_bad_parameter;

  return s_ok;
  }

/*
 * cli_get_int - Token get function for 32-bit integer.
 */
result_t cli_get_int16(const cli_token_t *token, int16_t *value)
  {
  result_t result;

  if(token == 0 || value == 0)
    return e_bad_parameter;

  *value = 0;

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;

  if (len == 0)
    return e_unexpected; /* optional argument wasn't provided */

  errno = 0;
  int32_t temp = strtol(token->buffer, 0, 0);

  if (errno != 0)
    return e_bad_parameter;

  if (temp > 32768)
    return e_bad_parameter;

  if (temp < -32767)
    return e_bad_parameter;

  *value = (int16_t)temp;

  return s_ok;

  }

/*
 * cli_get_int64 - Token get function for 64-bit integer.
 */
result_t cli_get_int32(const cli_token_t *token, int32_t *value)
  {
  result_t result;

  if(token == 0 || value == 0)
    return e_bad_parameter;

  *value = 0;

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;
  if (len == 0)
    return e_unexpected; /* optional argument wasn't provided */

  errno = 0;
  *value = strtol(token->buffer, 0, 0);

  if (errno != 0)
    return e_bad_parameter;

  return s_ok;
  }

/*
 * cli_get_float - Token get function for 64-bit floating point value.
 */
result_t cli_get_float(const cli_token_t *token, float *value)
  {
  if(token == 0 || value == 0)
    return e_bad_parameter;

  *value = 0;

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;
  if (len == 0)
    return e_unexpected; /* optional argument wasn't provided */

  double val;

  if (1 != sscanf(str, "%lf", &val))
    {
    *value = 0.0;
    return e_unexpected;
    }

  *value = (float)val;
  return s_ok;
  }

result_t cli_get_script(const cli_token_t *token, const char * *value)
  {
  if(token == 0 || value == 0)
    return e_bad_parameter;

  if(strlen(token->buffer) == 0)
    {
    *value = 0;
    return e_unexpected; /* optional argument wasn't provided */
    }

  *value = token->buffer;
  return s_ok;
  }

result_t cli_get_matrix(const cli_token_t *token, matrix_t *value)
  {
  if(token == 0 || value == 0)
    return e_bad_parameter;

  memset(value, 0, sizeof(matrix_t));

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;
  if (len == 0)
    return e_unexpected; /* optional argument wasn't provided */

  double v00, v01, v02, v10, v11, v12, v20, v21, v22;

  if (9 != sscanf(str, "[%lf,%lf,%lf][%lf,%lf,%lf][%lf,%lf,%lf]",
                  &v00, &v01, &v02,
                  &v10, &v11, &v12,
                  &v20, &v21, &v22))
    {
    memset(value, 0, sizeof(matrix_t));
    return e_unexpected;
    }

  value->v[0][0] = (float)v00;
  value->v[0][1] = (float)v01;
  value->v[0][2] = (float)v02;
  value->v[1][0] = (float)v10;
  value->v[1][1] = (float)v11;
  value->v[1][2] = (float)v12;
  value->v[2][0] = (float)v20;
  value->v[2][1] = (float)v21;
  value->v[2][2] = (float)v22;

  return s_ok;
  }

result_t cli_get_xyz(const cli_token_t *token, xyz_t *value)
  {
  if(token == 0 || value == 0)
    return e_bad_parameter;

  memset(value, 0, sizeof(xyz_t));

  uint16_t len = strlen(token->buffer);
  const char *str = token->buffer;
  if (len == 0)
    return e_unexpected; /* optional argument wasn't provided */

  double x, y, z;

  if (3 != sscanf(str, "[%lf,%lf,%lf]", &x, &y, &z))
    {
    memset(value, 0, sizeof(xyz_t));
    return e_unexpected;
    }

  value->x = (float)x;
  value->y = (float)y;
  value->z = (float)z;

  return s_ok;
  }
