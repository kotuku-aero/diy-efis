#include "muon.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static string_t string_make_room_for(string_t s, uint16_t addlen);

typedef struct _string_header_t
  {
    uint16_t len; /* used */
    uint16_t alloc; /* excluding the header and null terminator */
  } string_header_t;

static inline string_header_t *as_hdr(const string_t s)
  {
  return (string_header_t *)s - sizeof(string_header_t);
  }

static inline char *as_str(string_header_t *s)
  {
  return ((char *)s) + sizeof(string_header_t);
  }

uint16_t string_length(const string_t s)
  {
  if (s == 0)
    return 0;

  return as_hdr(s)->len;
  }

static inline uint16_t sdsavail(const string_t s)
  {
  if (s == 0)
    return 0;

  return as_hdr(s)->alloc - as_hdr(s)->len;
  }

static inline void sdssetlen(string_t s, uint16_t newlen)
  {
  as_hdr(s)->len = newlen;
  }

static inline void sdsinclen(string_t s, uint16_t inc)
  {
  as_hdr(s)->len += inc;
  }

/* sdsalloc() = sdsavail() + string_length() */
static inline uint16_t string_alloc(const string_t s)
  {
  return as_hdr(s)->alloc;
  }

static inline void sdssetalloc(string_t s, uint16_t newlen)
  {
  as_hdr(s)->alloc= newlen;
  }

string_t string_create_len(const char *init, uint16_t initlen)
  {
  string_header_t *sh;
  string_t s;

  uint16_t len = sizeof(string_header_t) + initlen + 1;

  len = ((len-1)| 31)+1;

  sh = kmalloc(len);

  if (!init)
    memset(sh, 0, sizeof(string_header_t) + initlen + 1);

  if (sh == 0)
    return 0;

  s = as_str(sh);

  sh->len = initlen;
  sh->alloc = len;

  if (initlen && init)
    memcpy(s, init, initlen);

  s[initlen] = '\0';
  return s;
  }

string_t string_create(const char *init)
  {
  if(init == 0)
    return string_create_len(0, 0);

  return string_create_len(init, (uint16_t)strlen(init));
  }

string_t string_dup(const string_t s)
  {
  return string_create_len(s, string_length(s));
  }

void string_free(string_t s)
  {
  if (s == 0)
    return;

  string_header_t *sh = as_hdr(s);
  kfree(sh);
  }

void string_clear(string_t s)
  {
  sdssetlen(s, 0);
  s[0] = '\0';
  }

/* Enlarge the free space at the end of the string_t string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the string_t string as returned
 * by string_length(), but only the free buffer space we have. */
static string_t string_make_room_for(string_t s, uint16_t addlen)
  {
  string_header_t *sh, *newsh;
  uint16_t avail = sdsavail(s);
  uint16_t len, newlen;

  /* Return ASAP if there is enough space left. */
  if (avail > addlen)
    return s;

  len = string_length(s);

  newlen = (len + addlen);

  // make it not fragment the cache
  uint16_t alloc_len = sizeof(string_header_t) + newlen + 1;

  alloc_len = ((alloc_len -1)| 31)+1;

  sh = as_hdr(s);

  newsh = krealloc(sh, alloc_len);
  if (newsh == 0)
    return 0;

  s = as_str(newsh);

  sdssetalloc(s, newlen);
  return s;
  }

string_t string_ensure_size(string_t s, uint16_t len)
  {
  if(s == 0)
    s = string_create(0);

  string_header_t *sh = as_hdr(s);
  // make sure terminator is allowed for
  len++;

  if(sh->alloc >= len)
    return s;

  // otherwise resize it.
  return string_make_room_for(s, len - sh->alloc);
  }

void string_set_length(string_t s, uint16_t len)
  {
  if(s == 0)
    return; // never set length

  string_header_t *sh = as_hdr(s);

  if(sh->len < len)
    return;

  sh->len = len;
  s[len] = 0;
  }

string_t string_push_back(string_t s, char ch)
  {
  s = string_make_room_for(s, 1);

  string_header_t *hdr = as_hdr(s);
  as_str(hdr)[hdr->len] = ch;
  hdr->len++;
  as_str(hdr)[hdr->len]=0;

  return s;
  }

uint16_t string_size(string_t s)
  {
  if(s == 0)
    return 0;

  uint16_t alloc = string_alloc(s);
  return sizeof(string_header_t) + alloc + 1;
  }

string_t string_grow_zero(string_t s, uint16_t len)
  {
  uint16_t curlen = string_length(s);

  if (len <= curlen)
    return s;

  s = string_make_room_for(s, len - curlen);
  if (s == 0)
    return 0;

  /* Make sure added region doesn't contain garbage */
  memset(s + curlen, 0, (len - curlen + 1)); /* also set trailing \0 byte */
  sdssetlen(s, len);
  return s;
  }

static string_t string_cat_len(string_t s, const char *t, uint16_t len)
  {
  uint16_t curlen = string_length(s);
  s = string_make_room_for(s, len);

  if (s == 0)
    return 0;

  memcpy(s + curlen, t, len);
  sdssetlen(s, curlen + len);
  s[curlen + len] = '\0';
  return s;
  }

string_t string_cat(string_t s, const char *t)
  {
  return string_cat_len(s, t, (uint16_t)strlen(t));
  }

string_t string_combine(string_t s, const string_t t)
  {
  return string_cat_len(s, t, string_length(t));
  }

string_t string_copy(string_t s, const char *t)
  {
  uint16_t len = (uint16_t)strlen(t);
  if (string_alloc(s) < len)
    {
    s = string_make_room_for(s, len - string_length(s));
    if (s == 0)
      return 0;
    }
  memcpy(s, t, len);
  s[len] = '\0';
  sdssetlen(s, len);
  return s;
  }

string_t string_vprintf(const char *fmt, va_list ap)
  {
  va_list cpy;
  char *buf;
  char *t;
  uint16_t buflen = (uint16_t)strlen(fmt) * 2;
  buf = kmalloc(buflen);
  if (buf == 0)
    return 0;

  string_t s = string_create_len(0, 0);

  /* Try with buffers two times bigger every time we fail to
   * fit the string in the current buffer size. */
  while (true)
    {
    buf[buflen - 2] = '\0';
    va_copy(cpy, ap);
    vsnprintf(buf, buflen, fmt, cpy);
    va_end(cpy);
    if (buf[buflen - 2] != '\0')
      {
      kfree(buf);
      buflen *= 2;
      buf = kmalloc(buflen);
      if (buf == 0)
        return 0;
      continue;
      }
    break;
    }

  /* Finally concat the obtained string to the string and return it. */
  t = string_create(buf);

  kfree(buf);

  return t;
  }

string_t string_printf(const char *fmt, ...)
  {
  va_list ap;
  char *t;
  va_start(ap, fmt);

  t = string_vprintf(fmt, ap);
  va_end(ap);
  return t;
  }

void string_trim(string_t s, const char *cset)
  {
  char *start, *end, *sp, *ep;
  uint16_t len;

  sp = start = s;
  ep = end = s + string_length(s) - 1;
  while (sp <= end && strchr(cset, *sp))
    sp++;
  while (ep > sp && strchr(cset, *ep))
    ep--;
  len = (sp > ep) ? 0 : ((ep - sp) + 1);
  if (s != sp)
    memmove(s, sp, len);
  s[len] = '\0';
  sdssetlen(s, len);
  }

void string_substring(string_t s, uint16_t start, uint16_t end)
  {
  uint16_t newlen, len = string_length(s);

  if (len == 0)
    return;
  if (start < 0)
    {
    start = len + start;
    if (start < 0)
      start = 0;
    }
  if (end < 0)
    {
    end = len + end;
    if (end < 0)
      end = 0;
    }
  newlen = (start > end) ? 0 : (end - start) + 1;
  if (newlen != 0)
    {
    if (start >= (signed) len)
      {
      newlen = 0;
      }
    else if (end >= (signed) len)
      {
      end = len - 1;
      newlen = (start > end) ? 0 : (end - start) + 1;
      }
    }
  else
    {
    start = 0;
    }
  if (start && newlen)
    memmove(s, s + start, newlen);
  s[newlen] = 0;
  sdssetlen(s, newlen);
  }

void string_tolower(string_t s)
  {
  int len = string_length(s), j;

  for (j = 0; j < len; j++)
    s[j] = tolower(s[j]);
  }

void string_toupper(string_t s)
  {
  int len = string_length(s), j;

  for (j = 0; j < len; j++)
    s[j] = toupper(s[j]);
  }

int string_compare(const string_t s1, const string_t s2)
  {
  uint16_t l1, l2, minlen;
  int cmp;

  l1 = string_length(s1);
  l2 = string_length(s2);
  minlen = (l1 < l2) ? l1 : l2;
  cmp = memcmp(s1, s2, minlen);
  if (cmp == 0)
    return l1 - l2;
  return cmp;
  }

vector_t string_split(const char *s, char sep)
  {
  if(s == 0 || sep == 0)
    return 0;

  vector_t tokens;
  vector_create(sizeof(string_t), &tokens);
  uint16_t len = (uint16_t)strlen(s);
  uint16_t start = 0;
  uint16_t end;
  string_t str;

  for(end = 0; end < len; end++)
    {
    if(s[end] == sep)
      {
      if(end > start)
        {
        str = string_create_len(&s[start], end - start);
        vector_push_back(tokens, &str);
        start = end+1;
        }
      }
    }

  if(end > start && end > 0)
    {
    str = string_create_len(&s[start], end - start);
    vector_push_back(tokens, &str);
    }

  return tokens;
  }

void string_free_split(vector_t tokens)
  {
  if (tokens == 0)
    return;

  uint16_t len;
  vector_count(tokens, &len);

  uint16_t i;
  for(i = 0; i < len; i++)
    {
    string_t str;
    vector_at(tokens, i, &str);
    string_free(str);
    }

  vector_close(tokens);
  }

void string_translate(string_t s, const char *from, const char *to)
  {
  uint16_t j, i, l = string_length(s);
  uint16_t setlen = (uint16_t) strlen(from);

  for (j = 0; j < l; j++)
    {
    for (i = 0; i < setlen; i++)
      {
      if (s[j] == from[i])
        {
        s[j] = to[i];
        break;
        }
      }
    }
  return;
  }

string_t string_join(char **argv, uint16_t argc, char *sep)
  {
  string_t join = string_create_len(0, 0);
  int j;

  for (j = 0; j < argc; j++)
    {
    join = string_cat(join, argv[j]);
    if (j != argc - 1)
      join = string_cat(join, sep);
    }
  return join;
  }

string_t string_insert(string_t s, uint16_t pos, char ch)
  {
  if(s == 0)
    return string_create_len(&ch, 1);

  s = string_make_room_for(s, 1);
  string_header_t *sh = as_hdr(s);

  if(pos >= sh->len)
    {
    // append char
    as_str(sh)[sh->len] = ch;
    sh->len++;
    as_str(sh)[sh->len] = 0;
    }
  else
    {
    uint16_t i;
    for(i = sh->len; i > pos; i--)
      as_str(sh)[i] = as_str(sh)[i-1];

    as_str(sh)[pos] = ch;
    sh->len++;
    }

  return s;
  }

void string_remove(string_t s, uint16_t pos)
  {
  string_header_t *sh = as_hdr(s);

  if(sh->len == 0 || pos >= sh->len)
    return;

  if(pos == (sh->len-1))
    as_str(sh)[sh->len-1] = 0;
  else
    {
    uint16_t i;
    for(i = pos; i < sh->len; i++)
      as_str(sh)[i] = as_str(sh)[i+1];
    }
  sh->len--;

  return;
  }
