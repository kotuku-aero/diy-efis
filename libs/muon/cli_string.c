#include "muon.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

vector_t string_split(const char *s, char sep)
  {
  if(s == 0 || sep == 0)
    return 0;

  vector_t tokens;
  vector_create(sizeof(const char *), &tokens);
  uint16_t len = (uint16_t)strlen(s);
  uint16_t start = 0;
  uint16_t end;
  char * str;

  for(end = 0; end < len; end++)
    {
    if(s[end] == sep)
      {
      if(end > start)
        {
        str = (char *)kmalloc(end - start + 1);
        memcpy(str, &s[start], end - start);
        str[end - start] = 0;
        vector_push_back(tokens, &str);
        start = end+1;
        }
      }
    }

  if(end > start && end > 0)
    {
    str = (char *)kmalloc(end - start + 1);
    memcpy(str, &s[start], end - start);
    str[end - start] = 0;
    vector_push_back(tokens, &str);
    }

  return tokens;
  }

void kfree_split(vector_t tokens)
  {
  if (tokens == 0)
    return;

  uint16_t len;
  vector_count(tokens, &len);

  uint16_t i;
  for(i = 0; i < len; i++)
    {
    char * str;
    vector_at(tokens, i, &str);
    kfree(str);
    }

  vector_close(tokens);
  }
