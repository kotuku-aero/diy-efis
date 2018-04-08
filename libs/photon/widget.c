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

 /*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static const unsigned char charmap[] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
  };

int strcasecmp(const char *s1, const char *s2)
  {
  register const unsigned char *cm = charmap,
  *us1 = (const unsigned char *)s1,
  *us2 = (const unsigned char  *)s2;

  while (cm[*us1] == cm[*us2++])
    if (*us1++ == '\0')
      return (0);

  return (cm[*us1] - cm[*--us2]);
  }

static result_t get_x(handle_t hwnd, variant_t *value)
  {
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect.left;

  return s_ok;
  }

static result_t set_x(handle_t hwnd, const variant_t *value)
  {
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect.left != value->v_int16;
  if (changed)
    {
    rect.left = value->v_int16;
    set_window_pos(hwnd, &rect);
    }

  return s_ok;
  }

static result_t get_y(handle_t hwnd, variant_t *value)
  {
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect.top;

  return s_ok;
  }

static result_t set_y(handle_t hwnd, const variant_t *value)
  {
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect.top != value->v_int16;
  if (changed)
    {
    rect.top = value->v_int16;
    return set_window_pos(hwnd, &rect);
    }

  return s_ok;
  }

static result_t get_width(handle_t hwnd, variant_t *value)
  {
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect_width(&rect);

  return s_ok;
  }

static result_t set_width(handle_t hwnd, const variant_t *value)
  {
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect_width(&rect) != value->v_int16;
  if (changed)
    {
    rect.right = rect.left + value->v_int16;
    return set_window_pos(hwnd, &rect);
    }

  return s_ok;
  }

static result_t get_height(handle_t hwnd, variant_t *value)
  {
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect_height(&rect);

  return s_ok;
  }

static result_t set_height(handle_t hwnd, const variant_t *value)
  {
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect_height(&rect) != value->v_int16;
  if (changed)
    {
    rect.bottom = rect.top + value->v_int16;
    return set_window_pos(hwnd, &rect);
    }

  return s_ok;
  }

static result_t get_zorder(handle_t hwnd, variant_t *value)
  {
  value->dt = field_uint16;
  uint8_t order;
  get_z_order(hwnd, &order);

  value->v_uint16 = order;

  return s_ok;
  }

static result_t set_zorder(handle_t hwnd, const variant_t *value)
  {
  if (value->dt != field_uint16)
    return e_bad_type;

  if (value->v_uint16 > 255)
    return e_bad_parameter;

  uint8_t order;
  get_z_order(hwnd, &order);


  bool changed = order != value->v_uint16;
  if (changed)
    return set_z_order(hwnd, (uint8_t)value->v_uint16);

  return s_ok;
  }

static canmsg_t create_msg = {
  .id = id_create
  };

result_t create_child_widget(handle_t parent, memid_t key, wndproc cb, handle_t *hwnd)
  {
  result_t result;
  rect_t rect;
  uint16_t id = 0;

  int16_t value;
  if (failed(result = reg_get_int16(key, "x", &value)))
    return result;
  rect.left = (gdi_dim_t) value;

  if (failed(result = reg_get_int16(key, "y", &value)))
    return result;
  rect.top = (gdi_dim_t) value;

  if (failed(result = reg_get_int16(key, "width", &value)))
    return result;
  rect.right = rect.left + (gdi_dim_t) value;

  if (failed(result = reg_get_int16(key, "height", &value)))
    return result;
  rect.bottom = rect.top + (gdi_dim_t) value;

  reg_get_uint16(key, "id", &id);

  char type[REG_NAME_MAX];
  const char *prototype = 0;
  if (succeeded(reg_get_string(key, "type", type, 0)))
    {
    // testing hack.  Until we remove all of the native code
    //prototype = type;
    }

  if(failed(result = create_child_window(parent, &rect, cb, id, key, prototype, hwnd)))
    return result;
  
  // set the z-order
  if(succeeded(result= reg_get_uint16(key, "zorder", &id)))
    {
    if(id < 256)
      set_z_order(*hwnd, (uint8_t)id);
    }

  add_property(*hwnd, "x", get_x, set_x, field_int16);
  add_property(*hwnd, "y", get_y, set_y, field_int16);
  add_property(*hwnd, "width", get_width, set_width, field_int16);
  add_property(*hwnd, "height",get_height, set_height, field_int16);
  add_property(*hwnd, "zorder", get_zorder, set_zorder, field_uint16);

  // The widget properties are loaded ok, now we can register them

  // see if any scriptlets are loadable.  These are used to decorate
  // the prototype
  memid_t handlers;
  if (succeeded(reg_open_key(key, "scripts", &handlers)))
    {
    // enumerate the keys
    field_datatype dt = field_stream;
    char name[REG_NAME_MAX];

    memid_t child = 0;
    while (succeeded(reg_enum_key(handlers, &dt, 0, 0, REG_NAME_MAX, name, &child)))
      {
      stream_p stream;
      if (succeeded(stream_open(handlers, name, &stream)))
        {
        if (failed(result = compile_function(*hwnd, name, stream)))
          {
          trace_debug("Cannot compile scriptlet %s when loading window\r\n", name);
          }

        dt = field_stream;
        }
      }

    // all of the scripts should be attached to the window.  Now we attach the events
    memid_t events;
    if (succeeded(reg_open_key(key, "events", &events)))
      {
      // enumerate the keys
      field_datatype dt = field_string;
      char name[REG_NAME_MAX];
      char event_fn[REG_STRING_MAX];

      memid_t child = 0;
      while (succeeded(reg_enum_key(events, &dt, 0, 0, REG_NAME_MAX, name, &child)))
        {
        uint16_t can_id = (uint16_t)strtoul(name, 0, 10);

        if (can_id > 0)
          {
          if (succeeded(reg_get_string(events, name, event_fn, 0)))
            add_handler(*hwnd, can_id, event_fn);
          }
        dt = field_string;
        }
      }
    }
  
  send_message(*hwnd, &create_msg);

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

result_t lookup_font(memid_t key, const char *name, handle_t *font)
  {
  result_t result;
  if(name == 0 ||
     font == 0)
    return e_bad_parameter;
  
  *font = 0;

  memid_t font_key;
  if(failed(result = reg_open_key(key, name, &font_key)))
    return e_not_found;

  char font_name[REG_STRING_MAX];
  
  if(failed(result = reg_get_string(font_key, "name", font_name, 0)))
      return result;

  uint16_t pts;
  if(failed(result = reg_get_uint16(font_key, "size", &pts)))
    return result;

  // pts is the vertical size.
  return open_font(font_name, pts, font);
  }

typedef struct _color_lookup_t
  {
  const char *name;
  color_t color;
  } color_lookup_t;

static color_lookup_t color_lookups[] = {
  { "white", color_white},
  { "black", color_black},
  { "gray", color_gray},
  { "light_gray", color_light_gray},
  { "dark_gray", color_dark_gray},
  { "red", color_red},
  { "pink", color_pink},
  { "blue", color_blue},
  { "green", color_green},
  { "lightgreen", color_lightgreen},
  { "yellow", color_yellow},
  { "magenta", color_magenta},
  { "cyan", color_cyan},
  { "pale_yellow", color_pale_yellow},
  { "light_yellow", color_light_yellow},
  { "lime_green", color_lime_green},
  { "teal", color_teal},
  { "dark_green", color_dark_green},
  { "maroon", color_maroon},
  { "purple", color_purple},
  { "orange", color_orange},
  { "khaki", color_khaki},
  { "olive", color_olive},
  { "brown", color_brown},
  { "navy", color_navy},
  { "light_blue", color_light_blue},
  { "faded_blue", color_faded_blue},
  { "hollow", color_hollow},
  { "lightgrey", color_lightgrey},
  { "darkgrey", color_darkgrey},
  { "paleyellow", color_paleyellow},
  { "lightyellow", color_lightyellow},
  { "limegreen", color_limegreen},
  { "darkgreen", color_darkgreen},
  { "lightblue", color_lightblue},
  { "fadedblue", color_fadedblue},
  { 0, 0}
  };

result_t lookup_color(memid_t key, const char *name, color_t *color)
  {
  result_t result;
  
  if(name == 0 ||
     color == 0)
    return e_bad_parameter;

  // determine the type of the registry key
  field_datatype dt = 0;
  memid_t memid;
  if(failed(result = reg_query_child(key, name, &memid, &dt, 0)))
    return result;

  if(dt == field_string)
    {
    // handle a string type
    char str[REG_STRING_MAX];
  
    *color = color_hollow;

    if (failed(result = reg_get_string(key, name, str, 0)))
      return result;

    if (str[0] == '0' || str[0] == '-')
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
  else if (dt = field_uint32)
    {
    // numeric field
    return reg_get_uint32(key, name, color);
    }

  return e_bad_parameter;
  }

typedef struct _pen_value_lookup_t
  {
  const char *name;
  pen_style style;
  } pen_value_lookup_t;

static const pen_value_lookup_t pen_values[] = {
  { "solid", ps_solid},
  { "dash", ps_dash},
  { "dot", ps_dot},
  { "dash_dot", ps_dash_dot},
  { "dash_dot_dot", ps_dash_dot_dot},
  { "null", ps_null},
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

result_t display_roller(canvas_t *canvas,
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
  text_extent(canvas, small_font, "00", 2, &size_medium);

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

      draw_text(canvas, bounds, small_font, fg_color, bg_color,
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
  text_extent(canvas, large_font, str, len, &large_size);

  pt.x -= large_size.dx;
  pt.y = bounds->top;
  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= large_size.dy >> 1;

  draw_text(canvas, bounds, large_font, fg_color, bg_color,
            str, len, &pt, bounds, eto_clipped, 0);

  return s_ok;
  }

// create a generic widget
result_t create_widget(handle_t parent, memid_t key, handle_t *hwnd)
  {
  result_t result;

  // create our window.  This does not 
  if (failed(result = create_child_widget(parent, key, defwndproc, hwnd)))
    return result;

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);

  return s_ok;
  }
