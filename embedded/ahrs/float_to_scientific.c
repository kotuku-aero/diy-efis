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
#include "ahrs.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

char *float_to_scientific(float value, int precision, char *buffer, unsigned int length)
  {
  char *s, *s0;
  int exponent;
  int L;
  int i;
  
  if (length < 5)
    return 0;

  s = buffer;

  if(value < 0)
    {
    /* set sign for everything, including 0's and NaNs */
    *buffer = '-';
    value *= -1;
    s++;
    }

  s0 = s;

  if (!value)
    {
    strcpy(buffer, "0");

    return buffer;
    }

  exponent = (int) log10(value);
	if(value >= 10.0)
	  {
		while(value >= 10.0)
			value /= 10.0;
	  }
	else if (value < 1.0)
	  {
    value *= pow(10, abs(exponent));
		value *= 10.0;
    exponent--;
	  }

  precision++;
  /* Generate ilim digits, then fix them up. */
  for (i = 1;; i++, value *= 10.)
    {
    L = value;
    value -= L;
    *s++ = '0' + (int) L;
    if(i == 1)
      *s++ = '.';
    if (i == precision)
      {
      if (value > 0.5)
        {
        // round up
        while (*--s == '9')
        if (s == s0)
          {
          *s = '0';
          break;
          }

        ++*s++;
        break;;
        }
      else if (value < 0.5)
        {
        while (*--s == '0');
        if(*s == '.')
          s += 2;
        else
          s++;
        break;
        }
      break;
      }
    }

  // print the exponent
  if(exponent != 0)
    {
    *s++ = 'E';
    itoa(s, exponent, 10);
    }
  else
    *s = 0;

  return buffer;
  }
