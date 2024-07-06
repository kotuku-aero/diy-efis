/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#include "muon.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

charps_t *string_split(const char *s, char sep)
  {
  if(s == 0 || sep == 0)
    return 0;

  charps_t  *tokens;
  charps_create(&tokens);
  
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
        neutron_malloc(end - start + 1, (void **)&str);

        memcpy(str, &s[start], end - start);
        str[end - start] = 0;
        charps_push_back(tokens, str);
        start = end+1;
        }
      }
    }

  if(end > start && end > 0)
    {
    neutron_malloc(end - start + 1, (void **)&str);
    memcpy(str, &s[start], end - start);
    str[end - start] = 0;
    charps_push_back(tokens, str);
    }

  return tokens;
  }

void close_and_free_charps(charps_t *tokens)
  {
  if (tokens == 0)
    return;

  uint16_t len = charps_count(tokens);

  uint16_t i;
  for(i = 0; i < len; i++)
    {
    char * str = charps_begin(tokens)[i];
    neutron_free(str);
    }

  charps_close(tokens);
  }
