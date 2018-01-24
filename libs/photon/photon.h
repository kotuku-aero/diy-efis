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
#ifndef __photon_h__
#define __photon_h__

#include "../neutron/neutron.h"

#if defined (__cplusplus )
extern "C"
  {
#endif

/**
 * @file photon.h
 * Photon GDI library definitions
 */

typedef int32_t gdi_dim_t;

/**
 * @struct point_t
 */
typedef struct _point_t
  {
  gdi_dim_t x;
  gdi_dim_t y;
  } point_t;
  
/**
 * @struct extent_t
 */
typedef struct _extent_t
  {
  gdi_dim_t dx;
  gdi_dim_t dy;
  } extent_t;

/**
 * @struct rect_t
 */  
typedef struct _rect_t 
  {
  // same layout as point_t, point_t (DO NOT CHANGE)
  gdi_dim_t left;
  gdi_dim_t top;
  gdi_dim_t right;
  gdi_dim_t bottom;
  } rect_t;

typedef uint32_t color_t;

#define RGBA(r,g,b,a) ((color_t)((((color_t)(a))<<24)|(((color_t)(r))<<16)|(((color_t)(g))<<8)|((color_t)(b))))
#define RGB(r,g,b) RGBA((r),(g),(b),0xff)

#define rgb(red, green, blue) RGBA(red,green,blue,255)
#define rgba(alpha, red, green, blue) RGBA(red,green,blue,alpha)

#define blue(c) ((uint8_t)(( c ) & 255))
#define red(c) ((uint8_t)((( c ) >> 16) & 255))
#define green(c) ((uint8_t)((( c ) >> 8) & 255))
#define alpha(c) ((uint8_t)((( c ) >> 24) & 255))
#define color_white RGB(255, 255, 255)
#define color_black RGB(0, 0, 0)
#define color_gray RGB(128, 128, 128)
#define color_light_gray RGB(192, 192, 192)
#define color_dark_gray RGB(64, 64, 64)
#define color_red RGB(255, 0, 0)
#define color_pink RGB(255, 128, 128)
#define color_blue RGB(0, 0, 255)
#define color_green RGB(0, 255, 0)
#define color_lightgreen RGB(0, 192, 0)
#define color_yellow RGB(255, 255, 64)
#define color_magenta RGB(255, 0, 255)
#define color_cyan RGB(0, 255, 255)
#define color_pale_yellow RGB(255, 255, 208)
#define color_light_yellow RGB(255, 255, 128)
#define color_lime_green RGB(192, 220, 192)
#define color_teal RGB(64, 128, 128)
#define color_dark_green RGB(0, 128, 0)
#define color_maroon RGB(128, 0, 0)
#define color_purple RGB(128, 0, 128)
#define color_orange RGB(255, 192, 64)
#define color_khaki RGB(167, 151, 107)
#define color_olive RGB(128, 128, 0)
#define color_brown RGB(192, 128, 32)
#define color_navy RGB(0, 64, 128)
#define color_light_blue RGB(128, 128, 255)
#define color_faded_blue RGB(192, 192, 255)
#define color_lightgrey RGB(192, 192, 192)
#define color_darkgrey RGB(64, 64, 64)
#define color_paleyellow RGB(255, 255, 208)
#define color_lightyellow RGB(255, 255, 128)
#define color_limegreen RGB(192, 220, 192)
#define color_darkgreen RGB(0, 128, 0)
#define color_lightblue RGB(128, 128, 255)
#define color_fadedblue RGB(192, 192, 255)
#define color_hollow RGBA(0, 0, 0, 0)
  
/**
 * @struct bitmap_t
 */
typedef struct _bitmap_t
  {
  uint16_t version; // sizeof(bitmap_t)
  gdi_dim_t bitmap_width; // width of the bitmap_t in pixels
  gdi_dim_t bitmap_height; // height of the bitmap_t in pixels
  const color_t *pixels;  // array of pixels in color format (rgba)
  } bitmap_t;

typedef enum _pen_style
  {
  ps_solid,
  ps_dash,
  ps_dot,
  ps_dash_dot,
  ps_dash_dot_dot,
  ps_null
  } pen_style;

/**
 * @struct pen_t
 */
typedef struct _pen_t
  {
  color_t color;
  uint16_t width;
  pen_style style;
  } pen_t;
  
////////////////////////////////////////////////////////////////////////////
typedef uint32_t text_flags;
enum {
  eto_opaque = 0x00000002,
  eto_clipped = 0x00000004
  };

enum {
  e_not_png = -100, /* image data does not have a PNG header */
  e_png_malformed = -101, /* image data is not a valid PNG image */
  e_png_unsupported = -102, /* critical PNG chunk type is not supported */
  e_png_interlaced = -103, /* image interlacing is not supported */
  e_png_bad_format = -104, /* image color format is not supported */
  };
  
typedef result_t (*wndproc)(handle_t hwnd, const canmsg_t *msg);

#define HWND_BROADCAST  (handle_t)(0xffffffff)
  
// specific window messages
#define id_paint (id_user_defined_start -1)
#define id_create (id_user_defined_start -2)
#define id_close (id_user_defined_start -3)
#define id_timer (id_user_defined_start -4)
#define id_key0 (id_user_defined_start -5)
#define id_key1 (id_user_defined_start -6)
#define id_key2 (id_user_defined_start -7)
#define id_key3 (id_user_defined_start -8)
#define id_key4 (id_user_defined_start -9)
#define id_decka (id_user_defined_start -10)
#define id_deckb (id_user_defined_start -11)
#define id_menu_up (id_user_defined_start -12)
#define id_menu_dn (id_user_defined_start -13)
#define id_menu_left (id_user_defined_start -14)
#define id_menu_right (id_user_defined_start -15)
#define id_menu_ok (id_user_defined_start -16)
#define id_menu_cancel (id_user_defined_start -17)
#define id_menu_select (id_user_defined_start -18)
#define id_buttonpress (id_user_defined_start -19)

/**
 * @function open_screen(uint16_t orientation, wndproc cb, uint16_t id, handle_t *hwnd)
 * Open the display screen and return the handle to the window
 * @param orientation Window rotation
 * @param cb          window proceedure
 * @param id          id to assign
 * @param hwnd        created screen
 * @return s_ok if the screen was created ok
 */
extern result_t open_screen(uint16_t orientation, wndproc cb, uint16_t id, handle_t *hwnd);
/**
 * @function result_t get_screen(handle_t *screen)
 * Return the root display screen
 * @param screen      Root display window
 * @return s_ok if the screen has been opened.
 */
extern result_t get_screen(handle_t *screen);
/**
 * @function attach_ion(handle_t screen)
 * Attach an ion interpreter to the screen.  Will load the script as a startup script
 * and call on_attach if it exists
 * @param screen        Screen to attach to
 * @param key           Root registry key for the screen
 * @param startup_script  Script to execute
 * @param ci            Optional console in
 * @param co            Optional console out
 * @param cerr          Optional console error
 * @return s_ok if the interpreter was started ok.
 */
extern result_t attach_ion(handle_t screen, memid_t key, const char *startup_script, handle_t ci, handle_t co, handle_t cerr);
/**
 * @function detach_ion(handle_t screen)
 * Detach an interpreter and release all resources.  Will call on_detach if
 * the startup script defined it
 * @param screen      Screen to detach
 * @return s_ok if the interpreter was detached ok.
 */
extern result_t detach_ion(handle_t screen);
/**
 * @function create_window(handle_t parent, const rect_t *bounds, wndproc cb, uint16_t id, handle_t *hwnd)
 * Create a window.  Will create a new canvas
 * @param parent  Parent window.  if 0 then the top level window is created
 * @param bounds  Bounds of window.  if parant != 0 then relative to parent window
 * @param cb      Callback to receive messages
 * @param hwnd    resulting handle
 * @return s_ok if window created ok
 */
extern result_t create_window(handle_t parent, const rect_t *bounds, wndproc cb, uint16_t id, handle_t *hwnd);
/**
 * @function create_child_window(handle_t parent, const rect_t *bounds, wndproc cb, uint16_t id, handle_t *hwnd)
 * Create a window that is a child of another window.  The window will share the canvas
 * of the parent and paint on its canvas.  Also adds the window as a child of the parent
 * @param parent  Parent window.  if 0 then the top level window is created
 * @param bounds  Bounds of window.  if parant != 0 then relative to parent window
 * @param cb      Callback to receive messages
 * @param hwnd    resulting handle
 * @return s_ok if window created ok
 */
extern result_t create_child_window(handle_t parent, const rect_t *bounds, wndproc cb, uint16_t id, handle_t *hwnd);
/**
 * @function close_window(handle_t hwnd);
 * Close a window and release all resources
 * @param hwnd    window to close
 * @return s_ok if resources are released
 */
extern result_t close_window(handle_t hwnd);
/**
 * @function get_window_rect(handle_t window, rect_t *rect)
 * Return the window rectangle 
 * @param window  Window to return
 * @param rect    pointer to the rectangle
 * @return s_ok if returned
 */
extern result_t get_window_rect(handle_t window, rect_t *rect);
/**
* @function get_window_pos(handle_t window, rect_t *rect)
* Return the window rectangle in relation to the parent window
* @param window  Window to return
* @param rect    pointer to the rectangle
* @return s_ok if returned
*/
extern result_t get_window_pos(handle_t window, rect_t *rect);
/**
* @function set_window_pos(handle_t window, rect_t *rect)
* set the window rectangle in relation to the parent window
* @param window  window to move
* @param rect    pointer to the rectangle
* @return s_ok if returned
*/
extern result_t set_window_pos(handle_t window, rect_t *rect);
/**
 * @function get_wnddata(handle_t window, void **wnd_data)
 * Get stream associated with a window
 * @param window    handle to window
 * @param wnd_data  stream pointer associated with window
 * @return s_ok if received ok
 */
extern result_t get_wnddata(handle_t window, void **wnd_data);
/**
 * @function get_parent(handle_t window, handle_t *parent)
 * Return the handle to the parent of this window
 * @param window  window to query
 * @param parent  parent window
 * @return s_ok if a valid window.
 */
extern result_t get_parent(handle_t window, handle_t *parent);
/**
 * @function get_window_by_id(handle_t parent, uint16_t id, handle_t *child)
 * Find a child window with the given id
 * @param parent    Parent window to search
 * @param id        Id to find
 * @param child     Found window
 * @return s_ok if the window exists as a child, or grandchild
 */
extern result_t get_window_by_id(handle_t parent, uint16_t id, handle_t *child);
/**
 * @function get_first_child(handle_t parent, handle_t *child)
 * Get the first child of a parent
 * @param parent  Parent window
 * @param child   returned child
 * @return s_ok if the child exists
 */
extern result_t get_first_child(handle_t parent, handle_t *child);
/**
 * @function get_next_sibling(handle_t wnd, handle_t *sibling)
 * Return the next sibling of a window
 * @param wnd       Window to query
 * @param sibling   found sibling
 * @return s_ok if the sibling exists
 */
extern result_t get_next_sibling(handle_t wnd, handle_t *sibling);
/**
 * @function get_previous_sibling(handle_t wnd, handle_t *sibling)
 * Return the previous sibling of a window
 * @param wnd       Window to query
 * @param sibling   Sibling window
 * @return s_ok if there is a previous sibling
 */
extern result_t get_previous_sibling(handle_t wnd, handle_t *sibling);
/**
 * @function insert_before(handle_t wnd, handle_t sibling)
 * Insert the window before the sibling in the chain of windows
 * @param wnd       window to insert
 * @param sibling   sibling to insert before
 * @return s_ok if inserted
 */
extern result_t insert_before(handle_t wnd, handle_t sibling);
/**
 * @function insert_after(handle_t wnd, handle_t sibling)
 * Insert the window after the sibling in the chain of windows
 * @param wnd       window to insert
 * @param sibling   sibling to insert after
 * @return s_ok if inserted
 */
extern result_t insert_after(handle_t wnd, handle_t sibling);
/**
 * @function get_z_order(handle_t wnd, uint8_t *z_order)
 * Set the z-order assigned to a window
 * @param wnd       Window to set the z-order for
 * @param z_order   order to draw in, default = 0
 * @return s_ok if changed
 */
extern result_t get_z_order(handle_t wnd, uint8_t *z_order);
/**
 * @function set_z_order(handle_t wnd, uint8_t z_order)
 * set the z-order of a window
 * @param wnd       window to set z_order for
 * @param z_order   z-order
 * @return s_ok if read ok.
 */
extern result_t set_z_order(handle_t wnd, uint8_t z_order);
/**
 * @function set_wnddata(handle_t window, void *wnd_data)
 * Set window stream
 * @param window      Window to associate stream with
 * @param wnd_data    Data pointer
 * @return s_ok if set ok
 */
extern result_t set_wnddata(handle_t window, void *wnd_data);
/**
 * @function get_message(handle_t hwnd, window_msg_t *msg)
 * Gets the next message from the message queue.  Will suspend the process until
 * a message is available.
 * @param hwnd        A handle to the window whose messages are to be retrieved.
 *                    if hwnd is 0, then all messages are retrieved
 * @param msg         pointer to a canmsg_t that received the message
 * @return s_ok if a message was retrieved.
 */
extern result_t get_message(handle_t hscreen, handle_t *hwnd, canmsg_t *msg);
/**
 * @function dispatch_message(const window_msg_t *msg)
 * Dispatches a message to a window procedure. It is typically used to dispatch
 * a message retrieved by the get_message function.
 * @param msg     Message to despatch
 * @return s_ok if despatched ok
 */
extern result_t dispatch_message(handle_t hwnd, const canmsg_t *msg);
/**
 * @function post_message(handle_t hwnd, const canmsg_t *msg, uint32_t max_wait)
 * Places (posts) a message in the message queue associated with the specified
 * window and returns without waiting for the application to process the message.
 * @param hwnd        A handle to the window whose window procedure is to
 *                    receive the message.
 *                    If the handle is 0 then the message is posted to the
 *                    can-bus.  If the flags.loopback is set then the message
 *                    is posted to all windows as well.
 *                    If the handle is HWND_BROADCAST the message is only sent
 *                    to windows, not the can bus.
 * @param msg         Message to post.
 * @param max_wait    Maximum time to wait of queue is full.  INDEFINITE_WAIT for
 *                    blocking calling task if the queue is full, 0 to return
 *                    immediately
 * @return s_ok if posted ok
 */
extern result_t post_message(handle_t hwnd, const canmsg_t *msg, uint32_t max_wait);
/**
 * @function send_message(handle_t hwnd, const canmsg_t *msg)
 * Sends the specified message to a window or windows. The SendMessage function
 * calls the window procedure for the specified window and does not return
 * until the window procedure has processed the message.
 * @param hwnd        A handle to the window whose window procedure will
 *                    receive the message. If this parameter is HWND_BROADCAST
 *                    then the message is sent to all windows.
 * @param msg         Message to send
 * @return s_ok if posted ok.
 */
extern result_t send_message(handle_t hwnd, const canmsg_t *msg);
/**
 * @function defwndproc(const window_msg_t *msg)
 * Perform default processing of messages
 * @param hwnd  handle of window processing the message
 * @param msg   Message to process
 * @return s_ok if process, s_false if not handled
 */
extern result_t defwndproc(handle_t hwnd, const canmsg_t *msg);
/**
 * @function result_t add_handler(handle_t hwnd, uint16_t id, const char *func)
 * Register a javascript event handler with a window
 * @param hwnd        window to attach a script to
 * @param id          message id process
 * @param func        name of the function
 * @return  s_ok if the id can be processed and the function exists
 * @remark  The screen must have a valid scriptlet with the function
 * name as defined.  See the attach_script function
 */
extern result_t add_handler(handle_t hwnd, uint16_t id, const char *func);
/**
 * @function  remove_handler(handle_t hwnd, uint16_t id)
 * Remove a registered handler
 * @param hwnd      window to remove event handler from
 * @param id        message id to remove
 * @return s_ok if the handler is removed
 */
extern result_t remove_handler(handle_t hwnd, uint16_t id);
/**
 * @function compile_function(handle_t hwnd, const char *func, stream_p stream)
 * Compile a method and associate it with a window
 * @param hwnd      window to associate script with
 * @param func      function to create
 * @param stream    source code for the function
 * @return s_ok if the function is compiled as attached ok
 */
extern result_t compile_function(handle_t hwnd, const char *func, stream_p stream);

/**
 * @function canvas_close(handle_t hwnd)
 * Close a canvas.
 * @param   canvas to release
 * @return s_ok if all resources freed.
 */
extern result_t canvas_close(handle_t hwnd);
/**
 * @function create_rect_canvas(const extent_t *length, handle_t *hndl)
 * Create an off screen canvas
 * @param length    dimensions of the display context
 * @param hndl    created handle
 * @return s_ok if enough memory for canvas
 */
extern result_t create_rect_canvas(const extent_t *size, handle_t *hndl);
/**
 * @function create_bitmap_canvas(const bitmap_t *bitmap, handle_t *hndl)
 * Create a canvas from the dimensions of the bitmap and select the pixels
 * into the canvas
 * @param bitmap    bitmap to create from
 * @param hndl      created canvas
 * @return s_ok if canvas created ok
 */
extern result_t create_bitmap_canvas(const bitmap_t *bitmap, handle_t *hndl);
/**
* @function create_png_canvas(handle_t stream, handle_t *hndl)
* Create a canvas from the PNG image provided in the stream
* @param stream    stream to read the PNG image from
* @param hndl      created canvas
* @return s_ok if canvas created ok
*/
extern result_t create_png_canvas(handle_t stream, handle_t *hndl);
/**
 * @function load_png(handle_t canvas, handle_t stream, const point_t *pt)
 * Load a png image from the stream onto the canvas.
 * @param canvas    canvas to render png onto
 * @param strean    Stream to read from
 * @param pt        optional point to load image onto 0,0 if not given
 * @return s_ok if the canvas is large enough and the stream is a valid png
*/
extern result_t load_png(handle_t canvas, handle_t stream, const point_t *pt);
/**
 * Get the length of a canvas
 * @param canvas  Canvas to query
 * @param extent  extents of the canvas
 * @param bpp     optional bits per pixel for canvas
 * @return s_ok if a valid canvas
 */
extern result_t get_canvas_extents(handle_t canvas, extent_t *extent, uint16_t *bpp);
/**
 * @function get_orientation(handle_t hwnd, uint16_t *orientation)
 * Return the orientation of the window
 * @param hwnd        window to query
 * @param orientation orientation applied
 * @return s_ok if the window supports rotation
 */
extern result_t get_orientation(handle_t hwnd, uint16_t *orientation);
/**
 * @function set_orientation(handle_t hwnd, uint16_t orientation)
 * Set the orientation for the window
 * @param hwnd        window to change
 * @param orientation 0, 90, 180 or 270 are supported
 * @return s_ok if the window supports orientation
 */
extern result_t set_orientation(handle_t hwnd, uint16_t orientation);
/**
 * @function polyline(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, const point_t *points, uint16_t count)
 * Draw a polyline
 * @param canvas      canvas to draw on
 * @param clip_rect   rectangle to clip to
 * @param pen         pen to draw with
 * @param count       number of points
 * @param points      points to draw
 * @return s_ok if succeeded
 */
extern result_t polyline(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, uint16_t count, const point_t *points);
/**
 * @function ellipse(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, const rect_t *area)
 * Draw an ellipse
 * @param canvas      canvas to draw on
 * @param clip_rect   rectangle to clip to
 * @param pen         pen to draw ellipse with
 * @param color       color to fill ellipse with
 * @param area        area of the ellise
 * @return  s_ok if succeeded
 */
extern result_t ellipse(handle_t canvas,
                        const rect_t *clip_rect,
                        const pen_t *pen,
                        color_t color,
                        const rect_t *area);
/**
 * @function polygon(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, const point_t *points, uint16_t count)
 * Draw a polygon and optionally fill it
 * @param canvas      canvas to draw on
 * @param clip_rect   rectangle to clip to
 * @param pen         pen to draw lines with
 * @param color       color to fill with
 * @param count       number of points
 * @param points      points of the polygon
 * @return  s_ok if succeeded
 */
extern result_t polygon(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, uint16_t count, const point_t *points);
/**
* @function polypolygon(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, handle_t poly)
* Draw a polypolygon and optionally fill it
* @param canvas      canvas to draw on
* @param clip_rect   rectangle to clip to
* @param pen         pen to draw lines with
* @param color       color to fill with
* @param count       number of contours in the polypolygon
* @param lengths
* @return  s_ok if succeeded
*/
extern result_t polypolygon(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, uint16_t count, const uint16_t *lengths, const point_t *points);
/**
 * @function rectangle(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, const rect_t *area)
 * Draw a rectangle
 * @param canvas        canvas to draw on
 * @param clip_rect     rectangle to clip to
 * @param pen           pen to outline with, can be 0
 * @param color         color to fill with, can be color_hollow
 * @param area          area of rectangle
 * @return  s_ok if succeeded
 */
extern result_t rectangle(handle_t canvas,
                          const rect_t *clip_rect,
                          const pen_t *pen,
                          color_t color,
                          const rect_t *area);
/**
 * @function round_rect(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, const rect_t *area, gdi_dim_t radius)
 * Draw a rectangle with rounded corners
 * @param canvas        canvas to draw on
 * @param clip_rect     rectangle to clip to
 * @param pen           pen to outline with
 * @param color         color to fill with
 * @param area          area of rectangle
 * @param corners       radius of corners
 * @return  s_ok if succeeded
 */
extern result_t round_rect(handle_t canvas,
                           const rect_t *clip_rect,
                           const pen_t *pen,
                           color_t color,
                           const rect_t *area,
                           gdi_dim_t radius);
/**
 * @function bit_blt(handle_t canvas, const rect_t *clip_rect, const rect_t *dest_rect, handle_t src_canvas, const rect_t *src_clip_rect, const point_t *src_pt)
 * Copy pixels from one canvas to another
 * @param canvas        canvas to draw on
 * @param clip_rect     rectangle to clip to
 * @param dest_rect     rectangle to draw into
 * @param src_canvas    canvas to copy from
 * @param src_clip_rect area of canvas to copy from
 * @param src_pt        top-left point of the source rectangle
 * @param operation     raster operation
 * @return  s_ok if succeeded
 */
extern result_t bit_blt(handle_t canvas,
                        const rect_t *clip_rect,
                        const rect_t *dest_rect,
                        handle_t src_canvas,
                        const rect_t *src_clip_rect,
                        const point_t *src_pt);
/**
 * @function get_pixel(handle_t canvas, const rect_t *clip_rect, const point_t *pt, color_t *pixel)
 * Return pixel 
 * @param canvas      canvas to query
 * @param clip_rect   rectangle to clip to
 * @param pt          point to get
 * @return pixel at point
 */
extern result_t get_pixel(handle_t canvas,
                                const rect_t *clip_rect,
                                const point_t *pt,
                          color_t *pixel);
/**
 * @function set_pixel(handle_t canvas, const rect_t *clip_rect, const point_t *pt, color_t c, color_t *pixel)
 * Set a pixel
 * @param canvas      canvas to write to
 * @param clip_rect   rectangle to clip to
 * @param pt          point to wite
 * @param c           color to write
 * @param pixel       optional old pixel
 * @return s_ok if set ok
 */
extern result_t set_pixel(handle_t canvas,
                                const rect_t *clip_rect,
                                const point_t *pt,
                          color_t c,
                          color_t *pixel);
/**
 * @function arc(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, const point_t *pt, gdi_dim_t radius, int start, int end)
 * Draw an arc
 * @param canvas      canvas to draw on
 * @param clip_rect   rectangle to clip to
 * @param pen         pen to use for line
 * @param pt          center point
 * @param radius      radius of arc
 * @param start       start angle in degrees 0-359
 * @param end         end angle in degress 0-359
 * @return s_ok if completed
 */
extern result_t arc(handle_t canvas,
                           const rect_t *clip_rect,
                           const pen_t *pen,
                           const point_t *pt,
                           gdi_dim_t radius,
                           int start,
                           int end);
/**
 * @function pie(handle_t canvas, const rect_t *clip_rect, const pen_t *pen, color_t color, const point_t *pt, int start, int end, gdi_dim_t radii,vgdi_dim_t inner)
 * Draw a pie
 * @param canvas      canvas to write to
 * @param clip_rect   rectangle to clip to
 * @param pen         pen to use for outline
 * @param color       color to fill with
 * @param pt          center point
 * @param start       start angle in degrees 0-359
 * @param end         end angle in degrees 0-359
 * @param radii       pie radius
 * @param inner       innert radius
 * @return  s_ok if completed
 */
extern result_t pie(handle_t canvas,
                           const rect_t *clip_rect,
                           const pen_t *pen,
                           color_t color,
                           const point_t *pt,
                           int start,
                           int end,
                           gdi_dim_t radii,
                           gdi_dim_t inner);
/**
 * @function open_font(const char *path, const point_t *char_metrics, const point_t *device_metrics, handle_t  *font)
 * open a named font
 * @param name            Name of the font must be registered withe the load_font call
 * @param points          The charcater height in pixels
 * @param font            resulting font
 * @return s_ok if the font is loaded
 *
*/
extern result_t open_font(const char *name, uint16_t pixels, handle_t *font);
/**
 * @function load_font(const char *name, handle_t stream)
 * Load a font into the font cache
 * @param stream    Stream to read the font from
 * @return s_ok if the font was loaded into the cache and is available for use.
 */
extern result_t load_font(handle_t stream);
/**
 * @function register_font(const char *name, const uint8_t *buffer, uint16_t length)
 * Register a font image.  Internal function only.  Used to register a C array as a font
 * does not copy the data and will never be released
 * @param buffer  Binary image of the font
 * @param length  number of bytes in the file
 * @return s_ok if the point sizes in the font are registered ok
 */
extern result_t register_font(const uint8_t *buffer, uint16_t length);
/**
 * @function draw_text(handle_t canvas, const rect_t *clip_rect, handle_t  font, color_t fg, color_t bg, const char *str, uint16_t count, const point_t *src_pt, const rect_t *txt_clip_rect, text_flags format, uint16_t *char_widths)
 * Draw text
 * @param canvas      canvas to write to
 * @param clip_rect   rectangle to clip to
 * @param font        font to write
 * @param fg          foreground color
 * @param bg          background color
 * @param str         text to write
 * @param count       number of characters
 * @param src_pt      top left point of text
 * @param txt_clip_rect area to clip text to
 * @param format      text operation
 * @param char_widths optional array of widths of characters written
 * @return  s_ok if completed
 */
extern result_t draw_text(handle_t canvas,
                                 const rect_t *clip_rect,
                                 handle_t  font,
                                 color_t fg,
                                 color_t bg,
                                 const char *str,
                                 uint16_t count, 
                                 const point_t *src_pt,
                                 const rect_t *txt_clip_rect,
                                 text_flags format,
                                 uint16_t *char_widths);
/**
 * @function text_extent(handle_t canvas, handle_t  font, const char *str, uint16_t count, extent_t *extent)
 * Return the area text draws within
 * @param canvas      canvas to write to
 * @param font        font to use
 * @param str         text to write
 * @param count       number of characters
 * @param extent      resulting extents
 * @return  s_ok if completed
 */
extern result_t text_extent(handle_t canvas,
                            handle_t  font,
                            const char *str,
                            uint16_t count,
                            extent_t *extent);
/**
 * @function invalidate_rect(handle_t hwnd, const rect_t *rect)
 * Invalidate the area of the canvas.
 * @param hwnd  window to invalidate
 * @param rect  hint ast rectangle invalidated
 * @return s_ok if the window is inavlidated
 */
extern result_t invalidate_rect(handle_t hwnd, const rect_t *rect);
/**
 * @function is_invalid(handle_t hwnd)
 * Return true if the window is invalid
 * @param hwnd  window to test
 * @return s_ok if the window has been invalidated and needs rendering
 */
extern result_t is_invalid(handle_t hwnd);
/**
 * @function begin_paint(handle_t hwnd)
 * Notify the GDI a write operation is beginning
 * @param hwnd  handle to notify
 * @return s_ok if the canvas can be painted on
 */
extern result_t begin_paint(handle_t hwnd);
/**
 * @function end_paint(handle_t hwnd)
 * Notify the canvas that the update operation is complete and
 * clear the invalid flag
 * @param hwnd  handle to notify
 * @return s_ok if the canvas was successfully updated.
 */
extern result_t end_paint(handle_t hwnd);

////////////////////////////////////////////////////////////////////////////////
//
// Utility functions

/**
 * @function rotate_point(const point_t *center, point_t *pt, int angle)
 * Rotate a point by a number of degrees
 * @param center    Point to rotate around
 * @param pt        Point to be rotated
 * @param angle     degrees to rotate
 * @return pointer after rotation
 */
extern const point_t *rotate_point(const point_t *center, point_t *pt, int angle);

/**
 * @function point_in_rect(const point_t *pt, const rect_t *rect)
 * Determine if a point exists inside a rectangle
 * @param pt    Point to test
 * @param rect  Rectangle to test
 * @return true if the point is within the rectangle
 */
static inline bool point_in_rect(const point_t *pt, const rect_t *rect)
  {
  return pt->x >= rect->left && pt->y >= rect->top && pt->x < rect->right
      && pt->y < rect->bottom;
  }


static inline point_t *top_left(const rect_t *rect, point_t *pt)
  {
  pt->x = rect->left;
  pt->y = rect->top;

  return pt;
  }

static inline point_t *top_right(const rect_t *rect, point_t *pt)
  {
  pt->x = rect->right;
  pt->y = rect->top;

  return pt;
  }

static inline point_t *bottom_left(const rect_t *rect, point_t *pt)
  {
  pt->x = rect->left;
  pt->y = rect->bottom;

  return pt;
  }

static inline point_t *bottom_right(const rect_t *rect, point_t *pt)
  {
  pt->x = rect->right;
  pt->y = rect->bottom;

  return pt;
  }

static inline gdi_dim_t rect_width(const rect_t *rect)
  {
  gdi_dim_t result;

  result = rect->right - rect->left;
  if (result < 0)
    return -result;

  return result;
  }

static inline gdi_dim_t rect_height(const rect_t *rect)
  {
  gdi_dim_t result;

  result = rect->bottom - rect->top;
  if (result < 0)
    return -result;

  return result;
  }

static inline rect_t *copy_rect(const rect_t *from, rect_t *to)
  {
  to->left = from->left;
  to->top = from->top;
  to->bottom = from->bottom;
  to->right = from->right;

  return to;
  }


static inline const rect_t *offset_rect(const extent_t *ex, rect_t *rect)
  {
  rect->left += ex->dx;
  rect->right += ex->dx;
  rect->top += ex->dy;
  rect->bottom += ex->dy;

  return rect;
  }

static inline bool is_equal(const point_t *p1, const point_t *p2)
  {
  return p1->x == p2->x && p1->y == p2->y;
  }

static inline point_t *make_point(gdi_dim_t x, gdi_dim_t y, point_t *pt)
  {
  pt->x = x;
  pt->y = y;

  return pt;
  }

static inline point_t *copy_point(const point_t *p1, point_t *p2)
  {
  p2->x = p1->x;
  p2->y = p1->y;

  return p2;
  }

static inline point_t *swap_points(point_t *p1, point_t *p2)
  {
  gdi_dim_t td;

  td = p1->x;
  p1->x = p2->x;
  p2->x = td;
  td = p1->y;
  p1->y = p2->y;
  p2->y = td;

  return p1;
  }

static inline rect_t *make_rect_pt(const point_t *pt, const extent_t *ex, rect_t *rect)
  {
  rect->left = pt->x;
  rect->right = pt->x + ex->dx;
  rect->top = pt->y;
  rect->bottom = pt->y + ex->dy;

  return rect;
  }

static inline rect_t *make_rect(gdi_dim_t left, gdi_dim_t top, gdi_dim_t right, gdi_dim_t bottom, rect_t *rect)
  {
  rect->left = left;
  rect->top = top;
  rect->right = right;
  rect->bottom = bottom;
  
  return rect;
  }


static inline rect_t *intersect_rect(const rect_t *from, rect_t *to)
  {
  to->left = max(to->left, from->left);
  to->top = max(to->top, from->top);
  to->right = min(to->right, from->right);
  to->bottom = min(to->bottom, from->bottom);

  if (to->left >= to->right || to->top >= to->bottom)
    to->left = to->right = to->top = to->bottom = 0;

  return to;
  }

static inline const extent_t *rect_extents(const rect_t *from, extent_t *ex)
  {
  ex->dx = from->right - from->left;
  ex->dy = from->bottom - from->top;
  
  return ex;
  }

#define RESULT_DEFINED
#define HANDLE_DEFINED

#ifndef _CRT_FUNCTIONS_REQUIRED
extern float roundf(float value);
#endif
  

#ifdef __cplusplus
  }
#endif

#endif
