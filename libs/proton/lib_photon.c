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
  canvas

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
    this.canvas.rectangle(this.left, this.top. this.right, this.bottom, color.black);
    -- more rendering here --
  }
  var menu = create_window(screen, screen.left, screen.top, screen.right, screen.bottom);
  menu.on_show = on_menu_show;

  */

/////////////////////////////////////////////////////////////////
//
// canvas object

static const char *prop_clip_rect = "clip_rect";
static const char *prop_pen = "pen";
static const char *prop_pen_color = "color";
static const char *prop_pen_width = "width";
static const char *prop_pen_style = "style";
static const char *prop_x = "x";
static const char *prop_y = "y";

/**
 @param ctx       duktape context
 @param stk_bot   number of temporary params below the stack
 @param obj_idx   index of this on the stack
 @param rect      where to get the clip rect into
*/
static result_t get_clip_rect(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, rect_t *rect)
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
  // the clip_rect is an array of integers on the this pointer called clip_rect
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
static result_t get_point(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, point_t *pt)
  {
  pt->x = 0;
  pt->y = 0;

  // obj is x : value, y: value
  duk_push_lstring(ctx, prop_x, 0);
  if (!duk_get_prop(ctx, obj_idx))
    {
    duk_pop(ctx);
    return s_ok;
    }
  pt->x = (gdi_dim_t)duk_get_int(ctx, stk_bot -1);
  duk_pop(ctx);

  duk_push_string(ctx, prop_y, 0);
  if (!duk_get_prop(ctx, obj_idx))
    {
    duk_pop(ctx);
    return s_ok;
    }
  pt->y = (gdi_dim_t)duk_get_int(ctx, -2);
  duk_pop(ctx);

  return s_ok;
  }

// TODO figure this out
static result_t get_points(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, point_t **pts, uint16_t *len)
  {
  *pts = 0;
  *len = 0;
  // first determine how many points;

  // allocate space
  *pts = neutron_malloc(sizeof(point_t) * *len);

  // read the points from the property.
  uint16_t idx;
  for (idx = 0; idx < *len; idx++)
    {
    // read the array value
    if (!duk_get_prop_index(ctx, obj_idx, idx))
      continue;

    // get the point
    if (failed(get_point(ctx, stk_bot, stk_bot -1, &(*pts)[idx])))
      {
      neutron_free(*pts);
      *pts = 0;
      *len = 0;
      return DUK_RET_TYPE_ERROR;
      }
    }
  return s_ok;
  }

// return the pen array and its values
static result_t get_pen(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, pen_t *pen)
  {

  return s_ok;
  }

// return the currently selected font
static result_t get_font(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, handle_t *font)
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
static const char *prop_color = "color";
static const char *prop_background = "background";

static color_t get_color(duk_context *ctx, duk_int_t stk_bot, duk_int_t obj_idx, bool get_bg)
  {
  color_t result = color_hollow;
  // return the foreground color, or background color
  duk_push_string(ctx, get_bg ? prop_background : prop_color);
  if (duk_get_prop(ctx, -1) > 0)
    {
    duk_size_t color_len = 0;
    const char *color = duk_get_lstring(ctx, obj_idx, &color_len);
    if (color_len > 0 && color != 0)
      {
      // see what color it is
      if (*color == '#')
        {
        // is an RGBA value
        result = (color_t)strtoul(color + 1, 0, 16);
        }
      else if (strcmp(color, "white"))
        result = color_white;
      else if (strcmp(color, "black"))
        result = color_gray;
      else if (strcmp(color, "gray"))
        result = color_light_gray;
      else if (strcmp(color, "lightgray"))
        result = color_dark_gray;
      else if (strcmp(color, "darkgray"))
        result = color_dark_gray;
      else if (strcmp(color, "red"))
        result = color_red;
      else if (strcmp(color, "pink"))
        result = color_pink;
      else if (strcmp(color, "blue"))
        result = color_blue;
      else if (strcmp(color, "green"))
        result = color_green;
      else if (strcmp(color, "lightgreen"))
        result = color_lightgreen;
      else if (strcmp(color, "yellow"))
        result = color_yellow;
      else if (strcmp(color, "magenta"))
        result = color_magenta;
      else if (strcmp(color, "cyan"))
        result = color_cyan;
      else if (strcmp(color, "paleyellow"))
        result = color_pale_yellow;
      else if (strcmp(color, "lightyellow"))
        result = color_light_yellow;
      else if (strcmp(color, "limegreen"))
        result = color_lime_green;
      else if (strcmp(color, "teal"))
        result = color_teal;
      else if (strcmp(color, "dark_green"))
        result = color_dark_green;
      else if (strcmp(color, "maroon"))
        result = color_maroon;
      else if (strcmp(color, "purple"))
        result = color_purple;
      else if (strcmp(color, "orange"))
        result = color_orange;
      else if (strcmp(color, "khaki"))
        result = color_khaki;
      else if (strcmp(color, "olive"))
        result = color_olive;
      else if (strcmp(color, "brown"))
        result = color_brown;
      else if (strcmp(color, "navy"))
        result = color_navy;
      else if (strcmp(color, "lightblue"))
        result = color_light_blue;
      else if (strcmp(color, "fadedblue"))
        result = color_faded_blue;
      else if (strcmp(color, "lightgrey"))
        result = color_lightgrey;
      else if (strcmp(color, "darkgrey"))
        result = color_darkgrey;
      else if (strcmp(color, "paleyellow"))
        result = color_paleyellow;
      else if (strcmp(color, "lightyellow"))
        result = color_lightyellow;
      else if (strcmp(color, "limegreen"))
        result = color_limegreen;
      else if (strcmp(color, "darkgreen"))
        result = color_darkgreen;
      else if (strcmp(color, "lightblue"))
        result = color_lightblue;
      else if (strcmp(color, "fadedblue"))
        result = color_fadedblue;
      else if (strcmp(color, "hollow"))
        result = color_hollow;
      }
    }

  duk_pop(ctx);

  return result;
  }

static duk_ret_t lib_polyline(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t) duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  point_t *pts = 0;
  uint16_t len = 0;
  if (failed(get_points(ctx, -1, -1, &pts, &len)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(polyline(canvas, &clip_rect, &pen, len, pts)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop(ctx);
  neutron_free(pts);

  return retval;
  }

// this.ellipse(left, top, right, bottom)
static duk_ret_t lib_ellipse(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  color_t fill_color = get_color(ctx, -1, -1, true);

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(ellipse(canvas, &clip_rect, &pen, fill_color, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  return retval;
  }

// this.polygon([[x:0,y:0],[x:100,y:0],[x:100,y:100],[x:0,y:100],[x:0,y:0]]) 
static duk_ret_t lib_polygon(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  color_t fill_color = get_color(ctx, -1, -1, true);

  point_t *pts = 0;
  uint16_t len = 0;
  if (failed(get_points(ctx, -1, 0, &pts, &len)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(polygon(canvas, &clip_rect, &pen, fill_color, len, pts)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop(ctx);
  neutron_free(pts);

  return retval;
  }

// this.rectangle(left, top, right, bottom)
static duk_ret_t lib_rectangle(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  color_t fill_color = get_color(ctx, -1, -1, true);

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(rectangle(canvas, &clip_rect, &pen, fill_color, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  return retval;
  }

// this.round_rect(left, top, right, bottom, radius)
static duk_ret_t lib_round_rect(duk_context *ctx)
  {
  duk_ret_t retval = 0;
  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  color_t fill_color = get_color(ctx, -1, -1, true);

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(round_rectangle(canvas, &clip_rect, &pen, fill_color, &rect, (gdi_dim_t) duk_get_int(ctx, 4))))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  return retval;
  }

// bit_blt(handle_t canvas, const rect_t *clip_rect, const rect_t *dest_rect, handle_t src_canvas, const rect_t *src_clip_rect, const point_t *src_pt)
// this.bit_blt(left, top, right, bottom, canvas, [x:0, y:0])
static duk_ret_t lib_bit_blt(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) < 5)
    return DUK_RET_TYPE_ERROR;

  if(!duk_is_object(ctx, 4))
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  color_t fill_color = get_color(ctx, -1, -1, true);

  rect_t rect;
  if (failed(get_rect(ctx, 0, &rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  handle_t src_canvas;
  rect_t src_rect;
  point_t src_pt = { 0, 0 };

  // get the property that is the canvas
  if (failed(duk_get_prop(ctx, 4)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  if (failed(get_clip_rect(ctx, -2, -2, &src_rect)))
    {
    duk_pop2(ctx);
    return DUK_RET_TYPE_ERROR;
    }
  
  if (duk_get_top(ctx) >= 6)
    get_point(ctx, -2, 5, &src_pt);
  
  if (failed(bit_blt(canvas, &clip_rect, &rect, src_canvas, &src_rect, &src_pt)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop2(ctx);

  return retval;
  }

static duk_ret_t lib_get_pixel(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) < 1)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  point_t pt;
  get_point(ctx, -1, 0, &pt);

  color_t color;

  if (failed(get_pixel(canvas, &clip_rect, &pt, &color)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  duk_pop(ctx);
  duk_push_uint(ctx, color);

  return 1;
  }

static duk_ret_t lib_set_pixel(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) < 2)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  point_t pt;
  get_point(ctx, -1, 0, &pt);

  color_t color = (color_t)duk_get_uint(ctx, 1);

  if (failed(get_pixel(canvas, &clip_rect, &pt, &color)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop(ctx);

  return retval;
  }

// arc(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, const point_t *pt, gdi_dim_t radius, int start, int end)
// this.arc(center_pt, radius, start, end)
static duk_ret_t lib_arc(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) != 4)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  point_t pt;
  get_point(ctx, -1, 0, &pt);
  gdi_dim_t radius = (gdi_dim_t)duk_get_int(ctx, 1);
  int start = duk_get_int(ctx, 2);
  int end = duk_get_int(ctx, 3);

  if (failed(arc(canvas, &clip_rect, &pen, &pt, radius, start, end)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop(ctx);

  return retval;
  }

// pie(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, const point_t *pt, int start, int end, gdi_dim_t radii, gdi_dim_t inner)
// this.pie(centre_pt, start, end, radius, inner)
static duk_ret_t lib_pie(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) != 5)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  color_t fill = get_color(ctx, -1, -1, true);

  point_t pt;
  get_point(ctx, -1, 0, &pt);
  int start = duk_get_int(ctx, 1);
  int end = duk_get_int(ctx, 2);
  gdi_dim_t radius = (gdi_dim_t)duk_get_int(ctx, 3);
  gdi_dim_t inner = (gdi_dim_t)duk_get_int(ctx, 4);

  if (failed(pie(canvas, &clip_rect, &pen, &pt, fill, start, end, radius, inner)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop(ctx);

  return retval;
  }

// draw_text(handle_t canvas, const rect_t *clip_rect, handle_t  font, color_t fg, color_t bg, const char *str, uint16_t count, const point_t *src_pt, const rect_t *txt_clip_rect, text_flags format, uint16_t *char_widths)
// this.text(str, point, clip_rect)
static duk_ret_t lib_draw_text(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) < 2)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, -1, -1, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  pen_t pen;
  if (failed(get_pen(ctx, -1, -1, &pen)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  color_t fill = get_color(ctx, -1, -1, true);

  point_t pt;
  if (failed(get_point(ctx, -1, 1, &pt)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  duk_size_t len = 0;
  const char *str = duk_get_lstring(ctx, 0, &len);

  if (len == 0 || str == 0)
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
  }

  handle_t font = 0;

  if (failed(get_font(ctx, -1, -1, &font)) ||
    font == 0)
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  color_t fg = get_color(ctx, -1, -1, false);
  color_t bg = get_color(ctx, -1, -1, true);

  rect_t text_clip_rect;
  rect_t *p_tcr = 0;
  if (duk_get_top(ctx) > 2)
    {
    if (failed(get_rect(ctx, 2, &text_clip_rect)))
      {
      duk_pop(ctx);
      return DUK_RET_TYPE_ERROR;
      }
    p_tcr = &text_clip_rect;
    }

  text_flags flags = (bg != color_hollow ? eto_opaque : 0) | (p_tcr != 0 ? eto_clipped : 0);

  if (failed(draw_text(canvas, &clip_rect, font, fg, bg, str, len, &pt, p_tcr, flags, 0)))
    retval = DUK_RET_TYPE_ERROR;

  duk_pop(ctx);

  return retval;
  }

// text_extent(handle_t canvas, handle_t  font, const char *str, uint16_t count, extent_t *extent)
// json ext = this.extent(str)
static duk_ret_t lib_text_extent(duk_context *ctx)
  {
  duk_ret_t retval = 0;

  if (duk_get_top(ctx) != 1)
    return DUK_RET_TYPE_ERROR;

  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  handle_t font = 0;

  if (failed(get_font(ctx, -1, -1, &font)) ||
    font == 0)
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  duk_size_t len = 0;
  const char *str = duk_get_lstring(ctx, 0, &len);

  if (len == 0 || str == 0)
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  extent_t ex;

  if (failed(text_extent(canvas, font, str, len, &ex)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }

  duk_pop(ctx);

  char buf[32];
  snprintf(buf, 32, "{ dx: %d, dy: %d }", ex.dx, ex.dy);

  duk_push_string(ctx, buf);

  return 1;
  }

static duk_ret_t lib_get_pen(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_set_pen(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_color(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_set_color(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_background(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_set_background(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_clip_rect(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_set_clip_rect(duk_context *ctx)
  {
   
  return 1;
  }

static duk_ret_t lib_get_canvas_width(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_canvas_height(duk_context *ctx)
  {
  return 1;
  }

static const duk_function_list_entry lib_canvas_funcs[] = {
    { "polyline", lib_polyline, 0 },
    { "ellipse", lib_ellipse, 0 },
    { "polygon", lib_polygon, 0 },
    { "rectangle", lib_rectangle, 0 },
    { "round_rect", lib_round_rect, 0 },
    { "bit_blt", lib_bit_blt, 0 },
    { "get_pixel", lib_get_pixel, 0 },
    { "set_pixel", lib_set_pixel, 0 },
    { "arc", lib_arc, 0 },
    { "pie", lib_pie, 0 },
    { "draw_text", lib_draw_text, 0 },
    { "text_extent", lib_text_extent, 0 },
    { "width", lib_get_canvas_width, 0 },
    { "height", lib_get_canvas_height, 0 },
    { "color", lib_get_color, 0 },
    { "color", lib_set_color, 1 },
    { "background_color", lib_get_background, 0 },
    { "background_color", lib_set_background, 1 },
    { "clip_rect", lib_get_clip_rect, 0 },
    { "clip_rect", lib_set_clip_rect, 1 },
    { NULL, NULL, 0 }
  };

static duk_ret_t lib_canvas_dtor(duk_context *ctx)
  {
  handle_t canvas = (handle_t)duk_get_magic(ctx, 0);

  canvas_close(canvas);

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

static duk_ret_t create_canvas(duk_context *ctx, handle_t canvas)
  {
  // create the object
  // Push special this binding to the function being constructed
  duk_push_c_function(ctx, lib_canvas_ctor, 0);
  duk_push_object(ctx);
  // Store the function destructor
  duk_push_c_function(ctx, lib_canvas_dtor, 0);
  duk_set_finalizer(ctx, -2);

  // add the methods
  duk_put_function_list(ctx, -1, lib_canvas_funcs);

  // add the handle property
  duk_set_magic(ctx, 0, (duk_int_t)canvas);

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

  handle_t canvas;
  result_t result;
  if (failed(result = create_rect_canvas(&ex, &canvas)))
    {
    // TODO: throw an exception
    return 0;
    }

  return create_canvas(ctx, canvas);
  }

static duk_ret_t lib_create_bitmap_canvas(duk_context *ctx)
  {
  // TODO: implement loading a bitmap from a resource stream!
  return 1;
  }

////////////////////////////////////////////////////////////////////////
//
// Create a canvas

static duk_ret_t lib_create_child_window(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_parent(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_window_by_id(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_first_child(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_next_sibling(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_previous_sibling(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_begin_paint(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_end_paint(duk_context *ctx)
  {
  return 1;
  }

static const duk_function_list_entry lib_window_funcs[] = {
    { "begin_paint", lib_begin_paint, 0 },
    { "end_paint", lib_end_paint, 0 },
    // create_child_window(x, y, width, height, id)
    { "create_child_window", lib_create_child_window, 5 },
    // should be a property...
    { "parent", lib_get_parent, 0},
    { "get_window_by_id", lib_get_window_by_id, 1 },
    // should be a property
    { "first_child", lib_get_first_child, 0 },
    { "next_sibling", lib_get_next_sibling, 0 },
    { "previous_sibling", lib_get_previous_sibling, 0 },
    { NULL, NULL, 0 }
  };

static duk_ret_t lib_window_dtor(duk_context *ctx)
  {
  handle_t hwnd = (handle_t)duk_get_magic(ctx, 0);

  window_close(hwnd);

  return 0;
  }

static duk_ret_t lib_window_ctor(duk_context *ctx)
  {
  if (!duk_is_constructor_call(ctx))
    return DUK_RET_TYPE_ERROR;

  // in case we are released...
  duk_set_magic(ctx, 0, 0);
  return 0;
  }

static duk_ret_t create_duk_window(duk_context *ctx, handle_t hwnd)
  {
  // create the object
  // Push special this binding to the function being constructed
  duk_push_c_function(ctx, lib_window_ctor, 0);
  duk_push_object(ctx);
  // Store the function destructor
  duk_push_c_function(ctx, lib_window_dtor, 0);
  duk_set_finalizer(ctx, -2);

  // add the methods
  duk_put_function_list(ctx, -1, lib_window_funcs);

  // add the handle property
  duk_set_magic(ctx, 0, (duk_int_t)hwnd);


  return 1;
  }

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


//////////////////////////////////////////////////////////////////////////////
void register_ion_functions(duk_context *ctx, handle_t co)
  {
  //extern result_t create_window(handle_t parent, const rect_t *bounds, wndproc cb, uint16_t id, handle_t *hwnd);
  register_function(ctx, lib_create_window, "create_window", 6);
  // extern result_t create_rect_canvas(int x, int y, handle_t *hndl);
  register_function(ctx, lib_create_rect_canvas, "create_rect_canvas", 1);
  // extern result_t create_bitmap_canvas(const bitmap_t *bitmap, handle_t *hndl);
  register_function(ctx, lib_create_bitmap_canvas, "create_bitmap_canvas", 1);
  // extern result_t get_orientation(handle_t hwnd, uint16_t *orientation);
  register_function(ctx, lib_get_orientation, "get_orientation", 1);
  // extern result_t create_font(const char *path, const point_t *char_metrics, const point_t *device_metrics, handle_t  *font);
  register_function(ctx, lib_open_font, "open_font", 1);
  // extern result_t release_font(handle_t  font);
  register_function(ctx, lib_load_font, "load_font", 1);
  }