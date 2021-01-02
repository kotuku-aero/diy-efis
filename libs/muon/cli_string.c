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
#include "muon.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

vector_p string_split(const char *s, char sep)
  {
  if(s == 0 || sep == 0)
    return 0;

  vector_p tokens;
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
        str = (char *)neutron_malloc(end - start + 1);
        memcpy(str, &s[start], end - start);
        str[end - start] = 0;
        vector_push_back(tokens, &str);
        start = end+1;
        }
      }
    }

  if(end > start && end > 0)
    {
    str = (char *)neutron_malloc(end - start + 1);
    memcpy(str, &s[start], end - start);
    str[end - start] = 0;
    vector_push_back(tokens, &str);
    }

  return tokens;
  }

void kfree_split(vector_p tokens)
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
    neutron_free(str);
    }

  vector_close(tokens);
  }
