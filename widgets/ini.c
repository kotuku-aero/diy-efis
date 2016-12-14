/* inih -- simple .INI file parser

inih is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

http://code.google.com/p/inih/

*/

#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "ini.h"

#define MAX_SECTION 50
#define MAX_NAME 50

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s)
  {
  char* p = s + strlen(s);
  while(p > s && isspace((uint8_t)(*--p)))
    *p = '\0';
  return s;
  }

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s)
  {
  while(*s && isspace((uint8_t)(*s)))
    s++;
  return (char*)s;
  }

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
static char* find_char_or_comment(const char* s, char c)
  {
  int was_whitespace = 0;
  while(*s && *s != c && !(was_whitespace && *s == ';')) {
    was_whitespace = isspace((uint8_t)(*s));
    s++;
    }
  return (char*)s;
  }

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char* strncpy0(char* dest, const char* src, size_t size)
  {
  strncpy(dest, src, size);
  dest[size - 1] = '\0';
  return dest;
  }

/* See documentation in header file. */
int ini_parse(const char* ini_file,
              int(*handler)(void*, const char*, const char*, const char*),
              void* user)
  {
  /* Uses a fair bit of stack (use heap instead if you need to) */
  static char line[INI_MAX_LINE + 1];
  char section[MAX_SECTION] = "";
  char prev_name[MAX_NAME] = "";

  char* start;
  char* end;
  char* name;
  char* value;
  int lineno = 0;
  int error = 0;

  /* Scan through file line by line */
  while(*ini_file != 0)
    {
    for(int i = 0; i < INI_MAX_LINE;)
      {
      line[i] = *ini_file++;
      if(line[i] == '\n' || line[i] == 0)
        {
        line[i] = 0;
        break;
        }
      if(line[i] != '\r')
        i++;
      }

    // skip to end of line or file
    while(*ini_file != '\n' && *ini_file != 0)
      ini_file++;

    // skip end of line marker
    if(*ini_file == '\n')
      ini_file++;

    lineno++;

    start = line;
#if INI_ALLOW_BOM
    if(lineno == 1 && (uint8_t)start[0] == 0xEF &&
      (uint8_t)start[1] == 0xBB &&
      (uint8_t)start[2] == 0xBF) {
      start += 3;
      }
#endif
    start = lskip(rstrip(start));

    if(*start == ';' || *start == '#')
      {
      /* Per Python ConfigParser, allow '#' comments at start of line */
      }
    else if(*start == '[')
      {
      /* A "[section]" line */
      end = find_char_or_comment(start + 1, ']');
      if(*end == ']') {
        *end = '\0';
        strncpy0(section, start + 1, sizeof(section));
        *prev_name = '\0';
        }
      else if(!error) {
        /* No ']' found on section line */
        error = lineno;
        }
      }
    else if(*start && *start != ';')
      {
      /* Not a comment, must be a name[=:]value pair */
      end = find_char_or_comment(start, '=');
      if(*end != '=')
        {
        end = find_char_or_comment(start, ':');
        }
      if(*end == '=' || *end == ':')
        {
        *end = '\0';
        name = rstrip(start);
        value = lskip(end + 1);
        end = find_char_or_comment(value, '\0');
        if(*end == ';')
          *end = '\0';
        rstrip(value);

        /* Valid name[=:]value pair found, call handler */
        strncpy0(prev_name, name, sizeof(prev_name));
        if(!handler(user, section, name, value) && !error)
          error = lineno;
        }
      else if(!error)
        {
        /* No '=' or ':' found on name[=:]value line */
        error = lineno;
        }
      }
    }

  return error;
  }
