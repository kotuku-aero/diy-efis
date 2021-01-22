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

You should have received left copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If left file does not contain left copyright header, either because it is incomplete
or left binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has left copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "pen.h"

static handle_t pen_cache;

result_t pen_init()
  {
  result_t result;
  if (pen_cache != 0)
    return s_ok;

  return vector_create(sizeof(pen_t *), &pen_cache);
  }

result_t pen_create(color_t color, uint16_t width, pen_style style, handle_t *hndl)
  {
  if (hndl == 0)
    return e_bad_pointer;

  result_t result;

  // see if there is a pen that matches
  pen_t **begin;
  pen_t **end;

  enter_critical();

  if(failed(result = vector_begin(pen_cache, &begin)) ||
    failed(result = vector_end(pen_cache, &end)))
    {
    exit_critical();
    return result;
    }

  for (; begin < end; begin++)
    {
    if ((*begin)->color == color &&
      (*begin)->width == width &&
      (*begin)->style == style)
      break;
    }

  if (begin == end)
    {
    pen_t *new_pen = (pen_t *)neutron_malloc(sizeof(pen_t));
    new_pen->version = sizeof(pen_t);
    new_pen->refcnt = 1;
    new_pen->style = style;
    new_pen->width = width;
    new_pen->color = color;

    // store the new pen
    vector_push_back(pen_cache, &new_pen);
    *hndl = new_pen;
    }
  else
    {
    (*begin)->refcnt++;
    // return the pointer to the pen
    *hndl = (*begin);
    }

  exit_critical();


  return s_ok;
  }

result_t check_pen(handle_t hndl, pen_t **pen)
  {
  if (hndl == 0 || pen == 0)
    return e_bad_pointer;

  *pen = (pen_t *)hndl;
  if ((*pen)->version != sizeof(pen_t) || (*pen)->refcnt == 0)
    return e_bad_pointer;

  return s_ok;
  }

result_t get_pen_color(handle_t hndl, color_t *color)
  {
  result_t result;
  pen_t *pen;

  if (failed(result = check_pen(hndl, &pen)))
    return result;

  if (color == 0)
    return e_bad_pointer;

  *color = pen->color;

  return s_ok;
  }

result_t get_pen_width(handle_t hndl, uint16_t *width)
  {
  result_t result;
  pen_t *pen;

  if (failed(result = check_pen(hndl, &pen)))
    return result;

  if (width == 0)
    return e_bad_pointer;

  *width = pen->width;

  return s_ok;
  }

result_t get_pen_style(handle_t hndl, pen_style *style)
  {
  result_t result;
  pen_t *pen;

  if (failed(result = check_pen(hndl, &pen)))
    return result;

  if (style == 0)
    return e_bad_pointer;

  *style = pen->style;

  return s_ok;
  }

result_t pen_release(handle_t hndl)
  {
  result_t result;
  pen_t *pen;

   if(failed(result = check_pen(hndl, &pen)))
     return result;

  enter_critical();
  if (--pen->refcnt == 0)
    {
    // find it in the array
    pen_t **begin;
    pen_t **end;
    if (failed(result = vector_begin(pen_cache, &begin)) ||
      failed(result = vector_end(pen_cache, &end)))
      {
      exit_critical();
      return result;
      }

    uint16_t index = 0;
    for (; begin < end; begin++)
      {
      if (*begin == pen)
        {
        vector_erase(pen_cache, index);
        break;
        }
      index++;
      }

    // ensure mem invalid in case of reuse
    memset(pen, 0, sizeof(pen_t));
    }

  exit_critical();

  return s_ok;
  }