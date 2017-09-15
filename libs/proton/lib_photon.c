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
  invalidate

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

// assumes stack[0] is this
static result_t get_clip_rect(duk_context *ctx, rect_t *rect)
  {
  // the clip_rect is an array of integers on the this pointer called clip_rect
  duk_push_lstring(ctx, prop_clip_rect, 0);
  if (!duk_get_prop(ctx, -1))
    {
    duk_pop(ctx);
    rect->top = 0;
    rect->bottom = 0;
    rect->left = 0;
    rect->right = 0;

    return s_ok;
    }
  duk_int_t index = duk_get_int(ctx, 0);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 0);
  rect->left = duk_get_int(ctx, -1);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 1);
  rect->top = duk_get_int(ctx, -1);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 2);
  rect->right = duk_get_int(ctx, -1);
  duk_pop(ctx);

  duk_get_prop_index(ctx, index, 3);
  rect->bottom = duk_get_int(ctx, -1);
  duk_pop(ctx);

  return s_ok;
  }

// get a point that is at -1 on the stack
static result_t get_point(duk_context *ctx, point_t *pt)
  {
  pt->x = 0;
  pt->y = 0;

  // obj is x : value, y: value
  duk_push_lstring(ctx, prop_x, 0);
  if (!duk_get_prop(ctx, -1))
    {
    duk_pop(ctx);
    return s_ok;
    }
  pt->x = (gdi_dim_t)duk_get_int(ctx, -2);
  duk_pop(ctx);

  duk_push_string(ctx, prop_y, 0);
  if (!duk_get_prop(ctx, -1))
    {
    duk_pop(ctx);
    return s_ok;
    }
  pt->y = (gdi_dim_t)duk_get_int(ctx, -2);
  duk_pop(ctx);

  return s_ok;
  }

static result_t get_points(duk_context *ctx, duk_int_t obj_idx, point_t **pts, uint16_t *len)
  {
  *pts = 0;
  *len = 0;
  // first determine how many points;

  // allocate space
  *pts = kmalloc(sizeof(point_t) * *len);

  // read the points from the property.
  uint16_t idx;
  for (idx = 0; idx < *len; idx++)
    {
    // read the array value
    if (!duk_get_prop_index(ctx, obj_idx, idx))
      continue;

    // get the point
    if (failed(get_point(ctx, &(*pts)[idx])))
      {
      kfree(*pts);
      *pts = 0;
      *len = 0;
      return DUK_RET_TYPE_ERROR;
      }
    }
  return s_ok;
  }

static duk_ret_t lib_polyline(duk_context *ctx)
  {
  // get our context
  duk_push_this(ctx);
  // and get the magic number
  handle_t canvas = (handle_t) duk_get_magic(ctx, 0);

  // get the clipping rectangle.
  rect_t clip_rect;

  if (failed(get_clip_rect(ctx, &clip_rect)))
    {
    duk_pop(ctx);
    return DUK_RET_TYPE_ERROR;
    }



  return 0;
  }

// ellipse(const rect_t *clip_rect, const pen_t *pen, color_t color, const rect_t *area)
static duk_ret_t lib_ellipse(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_polygon(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_rectangle(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_round_rect(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_bit_blt(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_pixel(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_set_pixel(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_arc(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_pie(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_draw_text(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_text_extent(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_get_transform(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_set_transform(duk_context *ctx)
  {
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

static duk_ret_t lib_create_font(duk_context *ctx)
  {
  return 1;
  }

static duk_ret_t lib_release_font(duk_context *ctx)
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
  // extern result_t create_font(const char *path, const point_t *char_metrics, const point_t *device_metrics, font_t *font);
  register_function(ctx, lib_create_font, "create_font", 1);
  // extern result_t release_font(font_t font);
  register_function(ctx, lib_release_font, "release_font", 1);

  register_function(ctx, lib_get_transform, "get_transform", 1);
  register_function(ctx, lib_set_transform, "set_transform", 2);
  }