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
#include "widget.h"
#include <stdlib.h>
#include <string.h>

result_t create_child_widget(handle_t parent, memid_t key, wndproc cb, handle_t *hwnd)
  {
  result_t result;
  rect_t rect;
  uint16_t id = 0;

  int16_t value;
  if (failed(result = reg_get_int16(key, "origin-x", &value)))
    return result;
  rect.left = (gdi_dim_t) value;

  if (failed(result = reg_get_int16(key, "origin-y", &value)))
    return result;
  rect.top = (gdi_dim_t) value;

  if (failed(result = reg_get_int16(key, "extent-x", &value)))
    return result;
  rect.right = rect.left + (gdi_dim_t) value;

  if (failed(result = reg_get_int16(key, "extent-y", &value)))
    return result;
  rect.bottom = rect.top + (gdi_dim_t) value;

  reg_get_uint16(key, "id", &id);

  if(failed(result = create_child_window(parent, &rect, cb, id, hwnd)))
    return result;
  
  // set the z-order
  if(succeeded(result= reg_get_uint16(key, "z-order", &id)))
    {
    if(id < 256)
      set_z_order(*hwnd, (uint8_t)id);
    }

  return s_ok;
  }

result_t lookup_enum(memid_t key,
                             const char *name,
                             const char **values,
                             int max_values,
                             int *value)
  {
  result_t result;
  char str[REG_STRING_MAX + 1];

  if (name == 0 ||
      values == 0 ||
      value == 0)
    return e_bad_parameter;

  if (failed(result = reg_get_string(key, name, str, 0)))
    return result;

  int i;
  for (i = 0; i < max_values; i++)
    if (strcasecmp(str, values[i]) == 0)
      {
      *value = i;
      return s_ok;
      }

  return e_not_found;
  }

result_t lookup_font(memid_t key, const char *name, const char *hints, handle_t  *font)
  {
  result_t result;
  if(name == 0 ||
     font == 0)
    return e_bad_parameter;
  
  *font = 0;

  memid_t font_key;
  if(failed(result = reg_open_key(key, name, &font_key)))
    return result;

  char font_name[REG_STRING_MAX];
  
  if(failed(result = reg_open_string(font_key, "name", font_name, 0)))
      return result;

  uint16_t pts;
  if(failed(result = reg_get_uint16(font_key, "size", &pts)))
    return result;

  // pts is the vertical size.
  return create_font(font_name, pts, hints, font);
  }

typedef struct _color_lookup_t
  {
  const char *name;
  color_t color;
  } color_lookup_t;

static color_lookup_t color_lookups[] = {
  { "color_white", color_white},
  { "color_black", color_black},
  { "color_gray", color_gray},
  { "color_light_gray", color_light_gray},
  { "color_dark_gray", color_dark_gray},
  { "color_red", color_red},
  { "color_pink", color_pink},
  { "color_blue", color_blue},
  { "color_green", color_green},
  { "color_lightgreen", color_lightgreen},
  { "color_yellow", color_yellow},
  { "color_magenta", color_magenta},
  { "color_cyan", color_cyan},
  { "color_pale_yellow", color_pale_yellow},
  { "color_light_yellow", color_light_yellow},
  { "color_lime_green", color_lime_green},
  { "color_teal", color_teal},
  { "color_dark_green", color_dark_green},
  { "color_maroon", color_maroon},
  { "color_purple", color_purple},
  { "color_orange", color_orange},
  { "color_khaki", color_khaki},
  { "color_olive", color_olive},
  { "color_brown", color_brown},
  { "color_navy", color_navy},
  { "color_light_blue", color_light_blue},
  { "color_faded_blue", color_faded_blue},
  { "color_hollow", color_hollow},
  { "color_lightgrey", color_lightgrey},
  { "color_darkgrey", color_darkgrey},
  { "color_paleyellow", color_paleyellow},
  { "color_lightyellow", color_lightyellow},
  { "color_limegreen", color_limegreen},
  { "color_darkgreen", color_darkgreen},
  { "color_lightblue", color_lightblue},
  { "color_fadedblue", color_fadedblue},
  { 0, 0}
  };

result_t lookup_color(memid_t key, const char *name, color_t *color)
  {
  result_t result;
  
  if(name == 0 ||
     color == 0)
    return e_bad_parameter;
  
  char str[REG_STRING_MAX];
  
  *color = color_hollow;

  if (failed(result = reg_get_string(key, name, str, 0)))
    return result;

  if (str[0] == '0')
    {
    *color = (color_t) strtoul(str, 0, 0);
    return s_ok;
    }

  color_lookup_t *colors = color_lookups;

  while (colors->name != 0)
    {
    if (strcasecmp(str, colors->name) == 0)
      {
      *color = colors->color;
      return s_ok;
      }

    colors++;
    }

  return e_not_found;
  }

typedef struct _pen_value_lookup_t
  {
  const char *name;
  pen_style style;
  } pen_value_lookup_t;

static const pen_value_lookup_t pen_values[] = {
  { "ps_solid", ps_solid},
  { "ps_dash", ps_dash},
  { "ps_dot", ps_dot},
  { "ps_dash_dot", ps_dash_dot},
  { "ps_dash_dot_dot", ps_dash_dot_dot},
  { "ps_null", ps_null},
  { 0, ps_solid}
  };

result_t lookup_pen_style(memid_t key, const char *name, pen_style *value)
  {
  result_t result;
  if(name == 0 ||
     value == 0)
    return e_bad_parameter;
  
  const pen_value_lookup_t *lookup = pen_values;
  
  char str[REG_STRING_MAX];

  *value = ps_solid;
  
  if (failed(result = reg_get_string(key, name, str, 0)))
    return result;

  while (lookup->name != 0)
    {
    if (strcmp(str, lookup->name) == 0)
      {
      *value = lookup->style;
      return s_ok;
      }

    lookup++;
    }

  return e_not_found;
  }

result_t lookup_pen(memid_t key, pen_t *pen)
  {
  result_t result;
  
  if(failed(result = lookup_color(key, "color", &pen->color)) ||
     failed(result = lookup_pen_style(key, "style", &pen->style)) ||
     failed(result = reg_get_uint16(key, "width", &pen->width)))
    return result;

  return s_ok;
  }

result_t display_roller(handle_t hwnd,
                        const rect_t *bounds,
                        uint32_t value,
                        int digits,
                        color_t bg_color,
                        color_t fg_color,
                        handle_t  large_font,
                        handle_t  small_font)
  {
  // we need to work out the size of the roller digits first
//  const handle_t  *old_font = cv.font(&arial_12_font);
  
  extent_t size_medium;
  text_extent(hwnd, small_font, "00", 2, &size_medium);

  point_t pt = {
    bounds->right - (digits == 1 ? size_medium.dx >>= 1 : size_medium.dx),
    bounds->top
    };
  
  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= size_medium.dy >> 1;

  if (digits == 1)
    value *= 10;

  // calc the interval / pixel ratio
  pt.y += (value % 10) * (size_medium.dy / 10.0);
  gdi_dim_t minor = (value / 10) * 10;

  gdi_dim_t large_value = minor / 100;
  minor %= 100;

  while (pt.y > bounds->top)
    {
    pt.y -= size_medium.dy;
    minor += 10;
    }

  char str[64];

  while (pt.y <= bounds->bottom)
    {
    // draw the text + digits first
    minor %= 100;
    if (minor < 0)
      minor += 100;

    if (minor >= 0)
      {
      if (digits == 1)
        sprintf(str, "%d", (int) minor / 10);
      else
        sprintf(str, "%02.2d", (int) minor);

      draw_text(hwnd, bounds, small_font, fg_color, bg_color,
                str, 0, &pt, bounds, eto_clipped, 0);
      }

    minor -= 10;
    pt.y += size_medium.dy;
    }

  // now the larger value

  sprintf(str, "%d", (int) large_value);
  size_t len = strlen(str);

  // calc the size
  //cv.font(&arial_15_font);
  extent_t large_size;
  text_extent(hwnd, large_font, str, len, &large_size);

  pt.x -= large_size.dx;
  pt.y = bounds->top;
  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= large_size.dy >> 1;

  draw_text(hwnd, bounds, large_font, fg_color, bg_color,
            str, len, &pt, bounds, eto_clipped, 0);

  return s_ok;
  }
