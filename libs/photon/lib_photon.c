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
#include "../photon/photon.h"
#include "../ion/ion.h"
#include "../photon/window.h"

/*
create and return a duktape object that is a window.

The window has properties:
  left
  top
  right
  bottom
  parent
  first_child
  next
  previous
  z_order
  handle

also has methods
  invalidate
  move
  hide
  show
  insert_before

the following methods can be over-ridden
  on_paint
  on_show
  on_hide
  on_key0
  on_key1
  on_key2
  on_key3
  on_key4
  on_decka_up
  on_decka_dn
  on_deckb_up
  on_deckb_dn
  on_menu_up
  on_menu_dn
  on_menu_left
  on_menu_right
  on_menu_ok
  on_menu_cancel
  on_menu_select
  on_timer

typical structure of a duk photon application.

  function on_menu_show(this) {
    this.handle.rectangle(this.left, this.top. this.right, this.bottom, color.black);
    -- more rendering here --
  }
  var menu = create_window(screen, screen.left, screen.top, screen.right, screen.bottom);
  menu.on_show = on_menu_show;

  */

/////////////////////////////////////////////////////////////////
//
// handle object

static const char *prop_clip_rect = "clip_rect";
static const char *prop_pen = "pen";
static const char *prop_color = "color";
static const char *prop_width = "width";
static const char *prop_style = "style";
static const char *prop_x = "x";
static const char *prop_y = "y";
static const char *prop_font = "font";
static const char *prop_pixels = "pixels";

/**
 @param ctx       duktape context
 @param stk_bot   number of temporary params below the stack
 @param obj_idx   index of this on the stack
 @param rect      where to get the clip rect into
*/
static result_t duk_get_clip_rect(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, rect_t *rect)
  {
  // the clip_rect is an array of integers on the this pointer called clip_rect
  duk_push_lstring(ctx, prop_clip_rect, 0);
  if (!duk_get_prop(ctx, obj_idx))
    {
    duk_pop(ctx);
    rect->top = 0;
    rect->bottom = 0;
    rect->left = 0;
    rect->right = 0;

    return s_ok;
    }

  duk_int_t index = duk_get_int(ctx, stk_bot);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 0);
  rect->left = duk_get_int(ctx, stk_bot-1);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 1);
  rect->top = duk_get_int(ctx, stk_bot -1);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 2);
  rect->right = duk_get_int(ctx, stk_bot -1);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 3);
  rect->bottom = duk_get_int(ctx, stk_bot -1);
  duk_pop(ctx);

  return s_ok;
  }

/**
 get a rectangle, assumes is on order of left, top, right, bottom
 from obj_index
 @param ctx       duktape context
 @param obj_idx   index of the rectangle params
 @param rect      resulting rectangle
*/
static result_t get_rect(duk_context *ctx, duk_int_t obj_idx, rect_t *rect)
  {
  rect->left = duk_get_int(ctx, obj_idx);
  rect->top = duk_get_int(ctx, obj_idx+1);
  rect->right = duk_get_int(ctx, obj_idx+2);
  rect->bottom = duk_get_int(ctx, obj_idx+3);

  return s_ok;
  }

/**
 get a point
 @param ctx   object context
 @param stk_bot bottom of the value stack
 @param obj_idx index of the object
 @param pt        resulting point
 */

static result_t get_point(duk_context *ctx, duk_int_t obj_idx, point_t *pt)
  {
  pt->x = 0;
  pt->y = 0;

  // obj is x : value, y: value
  if (!duk_get_prop_string(ctx, obj_idx, prop_x))
    return e_bad_parameter;

  pt->x = (gdi_dim_t)duk_get_int(ctx, -1);
  duk_pop(ctx);

  if (!duk_get_prop_string(ctx, obj_idx, prop_y))
    return e_bad_parameter;

  pt->y = (gdi_dim_t)duk_get_int(ctx, -1);
  duk_pop(ctx);

  return s_ok;
  }

static result_t get_points(duk_context *ctx, duk_int_t param, point_t **pts, uint16_t *len)
  {
  if (ctx == 0 || pts == 0 || len == 0)
    return e_bad_parameter;

  *pts = 0;
  *len = 0;
  // first determine how many points;
  *len = duk_get_length(ctx, param);

  // allocate space
  *pts = (point_t *)neutron_malloc(sizeof(point_t) * *len);

  memset(*pts, 0, sizeof(point_t) * *len);

  // read the points from the property.
  uint16_t idx;
  for (idx = 0; idx < *len; idx++)
    {
    // read the array value
    if (!duk_get_prop_index(ctx, param, idx))
      continue;

    // should be an object with x: and y:
    // get the point
    if (failed(get_point(ctx, -1, &(*pts)[idx])))
      {
      neutron_free(*pts);
      *pts = 0;
      *len = 0;
      return DUK_RET_TYPE_ERROR;
      }
    }
  return s_ok;
  }

// return the currently selected font as name, size
static result_t get_font(duk_context *ctx, duk_int_t param, handle_t *font)
  {

  return s_ok;
  }

/*
convert a parameter to a color.  Can be a string or an rgb value:

a constant is:
#RRGGBB
or
#AARRGGBB
*/

static result_t get_color(duk_context *ctx, duk_int_t param, color_t *color)
  {
  if (ctx == 0 || color == 0)
    return e_bad_parameter;

  color_t result = color_hollow;
  if (duk_is_string(ctx, param))
    {
    duk_size_t color_len = 0;
    const char *color = duk_get_lstring(ctx, param, &color_len);
    if (color_len > 0 && color != 0)
      {
      // see what color it is
      if (*color == '#')
        {
        // is an RGBA value
        result = (color_t)strtoul(color + 1, 0, 16);
        }
      else if (strcmp(color, "white") == 0)
        result = color_white;
      else if (strcmp(color, "black") == 0)
        result = color_black;
      else if (strcmp(color, "gray") == 0)
        result = color_gray;
      else if (strcmp(color, "lightgray") == 0)
        result = color_light_gray;
      else if (strcmp(color, "darkgray") == 0)
        result = color_dark_gray;
      else if (strcmp(color, "red") == 0)
        result = color_red;
      else if (strcmp(color, "pink") == 0)
        result = color_pink;
      else if (strcmp(color, "blue") == 0)
        result = color_blue;
      else if (strcmp(color, "green") == 0)
        result = color_green;
      else if (strcmp(color, "lightgreen") == 0)
        result = color_lightgreen;
      else if (strcmp(color, "yellow") == 0)
        result = color_yellow;
      else if (strcmp(color, "magenta") == 0)
        result = color_magenta;
      else if (strcmp(color, "cyan") == 0)
        result = color_cyan;
      else if (strcmp(color, "paleyellow") == 0)
        result = color_pale_yellow;
      else if (strcmp(color, "lightyellow") == 0)
        result = color_light_yellow;
      else if (strcmp(color, "limegreen") == 0)
        result = color_lime_green;
      else if (strcmp(color, "teal") == 0)
        result = color_teal;
      else if (strcmp(color, "dark_green") == 0)
        result = color_dark_green;
      else if (strcmp(color, "maroon") == 0)
        result = color_maroon;
      else if (strcmp(color, "purple") == 0)
        result = color_purple;
      else if (strcmp(color, "orange") == 0)
        result = color_orange;
      else if (strcmp(color, "khaki") == 0)
        result = color_khaki;
      else if (strcmp(color, "olive") == 0)
        result = color_olive;
      else if (strcmp(color, "brown") == 0)
        result = color_brown;
      else if (strcmp(color, "navy") == 0)
        result = color_navy;
      else if (strcmp(color, "lightblue") == 0)
        result = color_light_blue;
      else if (strcmp(color, "fadedblue") == 0)
        result = color_faded_blue;
      else if (strcmp(color, "lightgrey") == 0)
        result = color_lightgrey;
      else if (strcmp(color, "darkgrey") == 0)
        result = color_darkgrey;
      else if (strcmp(color, "paleyellow") == 0)
        result = color_paleyellow;
      else if (strcmp(color, "lightyellow") == 0)
        result = color_lightyellow;
      else if (strcmp(color, "limegreen") == 0)
        result = color_limegreen;
      else if (strcmp(color, "darkgreen") == 0)
        result = color_darkgreen;
      else if (strcmp(color, "lightblue") == 0)
        result = color_lightblue;
      else if (strcmp(color, "fadedblue") == 0)
        result = color_fadedblue;
      else if (strcmp(color, "hollow") == 0)
        result = color_hollow;
      }
    }
  else if (duk_is_number(ctx, param))
    result = duk_require_int(ctx, param);
  else
    return e_bad_type;

  *color = result;
  return s_ok;
  }

// return the pen array and its values
static result_t get_pen(duk_context *ctx, duk_int_t param, pen_t *pen)
  {
  result_t result;
  // obj is color : value, width: value
  if (ctx == 0 || pen == 0 || !duk_is_object(ctx, param))
    return e_bad_parameter;

  // get the object that is passed
  if (!duk_is_object(ctx, param))
    return e_bad_parameter;

  // two params
  if (duk_has_prop_string(ctx, param, prop_color))
    {
    duk_get_prop_string(ctx, param, prop_color);
    result = get_color(ctx, -1, &pen->color);
    duk_pop(ctx);
    if (failed(result))
      return result;
    }
  else
    pen->color = color_hollow;

  if (duk_has_prop_string(ctx, param, prop_width))
    {
    duk_get_prop_string(ctx, param, prop_width);
    pen->width = duk_get_int(ctx, -1);
    duk_pop(ctx);
    }
  else
    pen->width = 1;

  pen->style = ps_solid;

  return s_ok;
}

typedef struct _fontinfo_t {
  char name[REG_STRING_MAX];
  uint16_t pixels;
  } fontinfo_t;

// return the pen array and its values
static result_t get_fontinfo(duk_context *ctx, duk_int_t param, fontinfo_t *fi)
  {
  // obj is color : value, width: value
  if (ctx == 0 || fi == 0 || !duk_is_object(ctx, param))
    return e_bad_parameter;

  // get the object that is passed
  if (!duk_is_object(ctx, param))
    return e_bad_parameter;

  // two params
  if (duk_has_prop_string(ctx, param, prop_font))
    {
    duk_get_prop_string(ctx, param, prop_font);
    const char *font_name = duk_get_string(ctx, -1);
    strncpy(fi->name, font_name, REG_STRING_MAX - 1);
    fi->name[REG_STRING_MAX - 1] = 0;

    duk_pop(ctx);
    }
  else
    return e_bad_parameter;

  if (duk_has_prop_string(ctx, param, prop_pixels))
    {
    duk_get_prop_string(ctx, param, prop_pixels);
    fi->pixels = duk_get_int(ctx, -1);
    duk_pop(ctx);
    }
  else
    return e_bad_parameter;

  return s_ok;
  }

// passed in: array[](object x:, y:), object(width: , color:)
static duk_ret_t lib_polyline(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle; 
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;
  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  pen_t pen;
  if (failed(get_pen(ctx, 1, &pen)))
    return DUK_RET_TYPE_ERROR;

  point_t *pts = 0;
  uint16_t len = 0;
  if (failed(get_points(ctx, 0, &pts, &len)))
    return DUK_RET_TYPE_ERROR;

  if (failed(polyline(handle, &clip_rect, &pen, len, pts)))
    retval = DUK_RET_TYPE_ERROR;

  neutron_free(pts);

  return retval;
  }

// this.ellipse(left, top, right, bottom, fill_color, pen)
static duk_ret_t lib_ellipse(duk_context *ctx)
  {
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  pen_t pen;
  if (failed(get_pen(ctx, 2, &pen)))
    return DUK_RET_TYPE_ERROR;

  color_t fill_color;
  
  if (failed(get_color(ctx, 2, &fill_color)))
    fill_color = color_hollow;

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    return DUK_RET_TYPE_ERROR;

  if (failed(ellipse(handle, &clip_rect, &pen, fill_color, &rect)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// this.polygon([[x:0,y:0],[x:100,y:0],[x:100,y:100],[x:0,y:100],[x:0,y:0]], color, pen) 
static duk_ret_t lib_polygon(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  pen_t pen;
  if (failed(get_pen(ctx, 2, &pen)))
    return DUK_RET_TYPE_ERROR;

  color_t fill_color;
  
  if (failed(get_color(ctx, 1, &fill_color)))
    fill_color = color_hollow;

  point_t *pts = 0;
  uint16_t len = 0;
  if (failed(get_points(ctx, 0, &pts, &len)))
    return DUK_RET_TYPE_ERROR;

  if (failed(polygon(handle, &clip_rect, &pen, fill_color, len, pts)))
    retval = DUK_RET_TYPE_ERROR;

  neutron_free(pts);

  return retval;
  }

// this.rectangle(left, top, right, bottom, fill_color)
static duk_ret_t lib_rectangle(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  color_t fill_color;
  
  if (failed(get_color(ctx, 4, &fill_color)))
    fill_color = color_hollow;

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    return DUK_RET_TYPE_ERROR;

  if (failed(rectangle(handle, &clip_rect, 0, fill_color, &rect)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// this.round_rect(left, top, right, bottom, radius, fill_color, outline_pen)
static duk_ret_t lib_round_rect(duk_context *ctx)
  {
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  pen_t pen;
  if (failed(get_pen(ctx, 6, &pen)))
    return DUK_RET_TYPE_ERROR;

  color_t fill_color;
  if (failed(get_color(ctx, 5, &fill_color)))
    fill_color = color_hollow;

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    return DUK_RET_TYPE_ERROR;

  if (failed(round_rect(handle, &clip_rect, &pen, fill_color, &rect, (gdi_dim_t) duk_get_int(ctx, 4))))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// this.bit_blt(left, top, right, bottom, canvas, [x:0, y:0])
static duk_ret_t lib_bit_blt(duk_context *ctx)
  {
  if (duk_get_top(ctx) < 5)
    return DUK_RET_TYPE_ERROR;

  if(!duk_is_object(ctx, 4))
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  // get the source canvas
  duk_get_prop(ctx, 4);
  handle_t src_canvas;
  result = get_handle(ctx, -1, &src_canvas);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  rect_t src_rect;
  extent_t ex;
  get_canvas_extents(src_canvas, &ex, 0);
  src_rect.left = 0;
  src_rect.top = 0;
  src_rect.right = ex.dx;
  src_rect.bottom = ex.dy;

  point_t src_pt = { 0, 0 };
  
  get_point(ctx, 5, &src_pt);

  canvas_t *canvas;
  if(failed(get_canvas(handle, &canvas)) ||
    failed(bit_blt(handle, &clip_rect, &rect, src_canvas, &src_rect, &src_pt)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

static duk_ret_t lib_get_pixel(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) != 1)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  point_t pt;
  get_point(ctx, 0, &pt);

  color_t color;

  if (failed(get_pixel(handle, &clip_rect, &pt, &color)))
    return DUK_RET_TYPE_ERROR;

  duk_push_uint(ctx, color);

  return 1;
  }

static duk_ret_t lib_set_pixel(duk_context *ctx)
  {
  if (duk_get_top(ctx) < 2)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  point_t pt;
  get_point(ctx, 0, &pt);

  color_t color = (color_t)duk_get_uint(ctx, 1);

  if (failed(get_pixel(handle, &clip_rect, &pt, &color)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// arc(handle_t handle, const rect_t *clip_rect, const pen_t *pen, const point_t *pt, gdi_dim_t radius, int start, int end)
// this.arc(center_pt, radius, start, end, pen)
static duk_ret_t lib_arc(duk_context *ctx)
  {
  if (duk_get_top(ctx) != 5)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  pen_t pen;
  if (failed(get_pen(ctx, 4, &pen)))
    return DUK_RET_TYPE_ERROR;

  point_t pt;
  get_point(ctx, 0, &pt);
  gdi_dim_t radius = (gdi_dim_t)duk_get_int(ctx, 1);
  int start = duk_get_int(ctx, 2);
  int end = duk_get_int(ctx, 3);

  if (failed(arc(handle, &clip_rect, &pen, &pt, radius, start, end)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// pie(handle_t handle, const rect_t *clip_rect, const pen_t *pen, color_t color, const point_t *pt, int start, int end, gdi_dim_t radii, gdi_dim_t inner)
// this.pie(centre_pt, start, end, radius, inner, fill_color, pen)
static duk_ret_t lib_pie(duk_context *ctx)
  {
  if (duk_get_top(ctx) < 5)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  pen_t pen;
  pen_t *outline_pen = 0;
  if (duk_get_top(ctx) == 7)
    {
    if (failed(get_pen(ctx, 6, &pen)))
      return DUK_RET_TYPE_ERROR;
    outline_pen = &pen;
    }  

  color_t fill = color_hollow;
  if (duk_get_top(ctx) > 5)
    {
    if (failed(get_color(ctx, 5, &fill)))
      return DUK_RET_TYPE_ERROR;
    }

  point_t pt;
  get_point(ctx, 0, &pt);
  int start = duk_get_int(ctx, 1);
  int end = duk_get_int(ctx, 2);
  gdi_dim_t radius = (gdi_dim_t)duk_get_int(ctx, 3);
  gdi_dim_t inner = (gdi_dim_t)duk_get_int(ctx, 4);

  if (failed(pie(handle, &clip_rect, outline_pen, fill, &pt, start, end, radius, inner)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// draw_text(handle_t handle, const rect_t *clip_rect, handle_t  font, color_t fg, color_t bg, const char *str,
// uint16_t count, const point_t *src_pt, const rect_t *txt_clip_rect, text_flags format, uint16_t *char_widths)
// this.text(str, font, fg, bg, left, top, right, bottom, x, y)
//
// str      text to render
// font     name and size of font to render
// fg       forground color
// bg       background color
// left, top, right, bottom     bounding rectange
// x, y     point to render from (top left)
static duk_ret_t lib_draw_text(duk_context *ctx)
  {
  if (duk_get_top(ctx) != 10)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_window_rect(handle, &clip_rect)))
    return DUK_RET_TYPE_ERROR;

  color_t fg;
  if (failed(get_color(ctx, 2, &fg)))
    return DUK_RET_TYPE_ERROR;

  color_t bg;
  if(failed(get_color(ctx, 3, &bg)))
    return DUK_RET_TYPE_ERROR;

  point_t pt;
  if (failed(get_point(ctx, 8, &pt)))
    return DUK_RET_TYPE_ERROR;

  duk_size_t len = 0;
  const char *str = duk_get_lstring(ctx, 0, &len);

  rect_t rect;
  if(failed(get_rect(ctx, 4, &rect)))
    return DUK_RET_TYPE_ERROR;

  fontinfo_t fi;
  if (failed(get_fontinfo(ctx, 1, &fi)))
    return DUK_RET_TYPE_ERROR;

  handle_t font = 0;
  if(failed(open_font(fi.name, fi.pixels, &font)))
    return DUK_RET_TYPE_ERROR;

  text_flags flags = (bg != color_hollow ? eto_opaque : 0);

  if (failed(draw_text(handle, &clip_rect, font, fg, bg, str, len, &pt, &rect, flags, 0)))
    return DUK_RET_TYPE_ERROR;

  return 0;
  }

// text_extent(handle_t handle, handle_t  font, const char *str, uint16_t count, extent_t *extent)
// object = this.extent(str, font)
static duk_ret_t lib_text_extent(duk_context *ctx)
  {
  if (duk_get_top(ctx) != 2)
    return DUK_RET_TYPE_ERROR;

  fontinfo_t fi;
  if (failed(get_fontinfo(ctx, 1, &fi)))
    return DUK_RET_TYPE_ERROR;

  handle_t font = 0;
  if (failed(open_font(fi.name, fi.pixels, &font)))
    return DUK_RET_TYPE_ERROR;

  duk_size_t len;
  const char *str = duk_get_lstring(ctx, 0, &len);

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  if (failed(result))
    return DUK_RET_TYPE_ERROR;

  if (len == 0 || str == 0)
    return DUK_RET_TYPE_ERROR;

  extent_t ex;

  if (failed(text_extent(handle, font, str, len, &ex)))
    return DUK_RET_TYPE_ERROR;

  duk_push_object(ctx);

  duk_push_int(ctx, ex.dx);
  duk_put_prop_string(ctx, -2, "dx");
  duk_push_int(ctx, ex.dy);
  duk_put_prop_string(ctx, -2, "dy");

  return 1;
  }

static duk_ret_t lib_get_extents(duk_context *ctx)
  {
  if (duk_get_top(ctx) != 1)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  extent_t ex;
  if (failed(get_canvas_extents(handle, &ex, 0)))
    return DUK_RET_TYPE_ERROR;

  duk_push_object(ctx);

  duk_push_int(ctx, ex.dx);
  duk_put_prop_string(ctx, -2, "dx");
  duk_push_int(ctx, ex.dy);
  duk_put_prop_string(ctx, -2, "dy");

  return 1;
  }

const duk_function_list_entry lib_canvas_funcs[] = {
    { "polyline", lib_polyline, 2 },
    { "ellipse", lib_ellipse, 6 },
    { "polygon", lib_polygon, 3 },
    { "rectangle", lib_rectangle, 5 },
    { "round_rect", lib_round_rect, 7 },
    { "bit_blt", lib_bit_blt, 6 },
    { "get_pixel", lib_get_pixel, 1 },
    { "set_pixel", lib_set_pixel, 2 },
    { "arc", lib_arc, 5 },
    { "pie", lib_pie, 7 },
    { "draw_text", lib_draw_text, 11 },
    { "text_extent", lib_text_extent, 3 },
    { "extents", lib_get_extents, 0 },
    { NULL, NULL, 0 }
  };

static duk_ret_t lib_canvas_dtor(duk_context *ctx)
  {
  handle_t handle = (handle_t)duk_get_magic(ctx, 0);

  canvas_close(handle);

  return 0;
  }

static duk_ret_t lib_canvas_ctor(duk_context *ctx)
  {
  if (!duk_is_constructor_call(ctx))
    return DUK_RET_TYPE_ERROR;

  // in case we are released...
  duk_set_magic(ctx, 0, 0);
  return 0;
  }

extern const char *handle_prop;

static duk_ret_t create_canvas(duk_context *ctx, handle_t handle)
  {
  // create the object
  // Push special this binding to the function being constructed
  duk_push_c_function(ctx, lib_canvas_ctor, 0);
  duk_idx_t obj_idx = duk_push_object(ctx);
  // Store the function destructor
  duk_push_c_function(ctx, lib_canvas_dtor, 0);
  duk_set_finalizer(ctx, -2);

  // add the methods
  duk_put_function_list(ctx, -1, lib_canvas_funcs);

  // add the handle property
  duk_push_pointer(ctx, handle);
  duk_put_prop_string(ctx, obj_idx, handle_prop);

  return 1;
  }

/////////////////////////////////////////////////////////////////
//
// Canvas functions

static duk_ret_t lib_create_rect_canvas(duk_context *ctx)
  {
  extent_t ex;
  ex.dx = duk_get_uint(ctx, 0);
  ex.dy = duk_get_uint(ctx, 1);

  canvas_t *handle;
  result_t result;
  if (failed(result = create_rect_canvas(&ex, &handle)))
    {
    // TODO: throw an exception
    return 0;
    }

  return create_canvas(ctx, handle);
  }

static duk_ret_t lib_create_bitmap_canvas(duk_context *ctx)
  {
  // TODO: implement loading a bitmap from a resource stream!
  return 1;
  }

////////////////////////////////////////////////////////////////////////
//
// Create a handle

static duk_ret_t lib_create_child_window(duk_context *ctx)
  {
  return DUK_RET_TYPE_ERROR;
  }

static duk_ret_t lib_get_window_by_id(duk_context *ctx)
  {
  return DUK_RET_TYPE_ERROR;
  }

static duk_ret_t lib_get_next_sibling(duk_context *ctx)
  {
  return DUK_RET_TYPE_ERROR;
  }

static duk_ret_t lib_get_previous_sibling(duk_context *ctx)
  {
  return DUK_RET_TYPE_ERROR;
  }

static duk_ret_t lib_begin_paint(duk_context *ctx)
  {
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  canvas_t *canvas;
  begin_paint(handle, &canvas);
  return create_canvas(ctx, canvas);
  }

static duk_ret_t lib_end_paint(duk_context *ctx)
  {
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  end_paint(handle);
  return 0;
  }

static duk_ret_t lib_invalidate(duk_context *ctx)
  {
  // get our context
  duk_push_this(ctx);
  result_t result;
  // and get the magic number
  handle_t handle;
  result = get_handle(ctx, -1, &handle);
  duk_pop(ctx);

  invalidate_rect(handle, 0);
  return 0;
  }

const duk_function_list_entry lib_window_funcs[] = {
    { "invalidate", lib_invalidate, 0 },
    { "begin_paint", lib_begin_paint, 0 },
    { "end_paint", lib_end_paint, 0 },
    { "create_child_window", lib_create_child_window, 6 },
    { "get_window_by_id", lib_get_window_by_id, 1 },
    { "next_sibling", lib_get_next_sibling, 0 },
    { "previous_sibling", lib_get_previous_sibling, 0 },
    { NULL, NULL, 0 }
  };

// var wnd = create_window(parent, left, top, right, bottom, id);
static duk_ret_t lib_create_window(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_orientation(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_open_font(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_load_font(duk_context *ctx)
  {
  return 1;
  }


static void register_function(duk_context *ctx, duk_c_function fn, const char *name, duk_idx_t nargs)
  {
  duk_push_c_function(ctx, fn, nargs);
  duk_put_global_string(ctx, name);
  }

//////////////////////////////////////////////////////////////////////////////
result_t register_photon_functions(duk_context *ctx, handle_t co)
  {
  register_function(ctx, lib_create_window, "create_window", 6);
  register_function(ctx, lib_create_rect_canvas, "create_rect_canvas", 1);
  register_function(ctx, lib_create_bitmap_canvas, "create_bitmap_canvas", 1);
  register_function(ctx, lib_get_orientation, "get_orientation", 1);
  register_function(ctx, lib_open_font, "open_font", 1);
  register_function(ctx, lib_load_font, "load_font", 1);

  return s_ok;
  }