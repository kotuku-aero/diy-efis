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
#ifndef __bsp_h__
#define	__bsp_h__

#include "../photon/photon.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These are the functions that must be provided by a port of ion to
 * a hardware platform.
 */
struct _canvas_t;

/**
* Get a pixel from the canvas
* @param canvas    Canvas to reference
* @param src       point on canvas
* @return color at point
*/
typedef color_t(*get_pixel_fn)(struct _canvas_t *canvas, const point_t *src);
/**
* Set a pixel on the canvas
* @param canvas  Canvas to reference
* @param dest    point on canvas
* @param color   color to set
*/
typedef void(*set_pixel_fn)(struct _canvas_t *canvas, const point_t *dest, color_t color);
/**
* Fast fill a region
* @param canvas      Canvas to write to
* @param dest        destination rectange to fill
* @param words       Number of words to fill
* @param fill_color  Fill color
*
* @remarks This routine assumes a landscape fill.  (_display_mode == 0 | 180)
* So if the display mode is 90 or 3 then things get performed vertically
*/
typedef void(*fast_fill_fn)(struct _canvas_t *canvas, const rect_t *dest, color_t fill_color);

/**
* Draw a line between 2 points
* @param canvas      canvas to draw on
* @param p1          first point
* @param p2          second point
* @param fill_color  color to fill with
*/
typedef void(*fast_line_fn)(struct _canvas_t *canvas, const point_t *p1, const point_t *p2, color_t fill_color);
/**
* Perform a fast copy from source to destination
* @param dest        Destination in pixel buffer (0,0)
* @param src_canvas  Canvas to copy from
* @param src         area to copy
* @param rop         copy mode
* @remarks The implementation can assume that the src rectangle will be clipped to the
* destination canvas metrics
*/
typedef void(*fast_copy_fn)(struct _canvas_t *canvas, const point_t *dest, const struct _canvas_t *src_canvas, const rect_t *src);
/**
* Perform a fast copy from source to destination
* @param canvas      Canvas to copy to
* @param dest        Destination in pixel buffer (0,0)
* @param src_bitmap      Bitmap to copy from
* @param src         area to copy
* @remarks The implementation can assume that the src rectangle will be clipped to the
* destination canvas metrics
*/
typedef void(*copy_bitmap_fn)(struct _canvas_t *canvas, const point_t *dest, const bitmap_t *src_bitmap, const rect_t *src);

/**
 * Check to see if a id_paint should be queued
 * @param wnd   Window handle
 * @return s_ok if an id_paint should be generated, s_false if deferred
 */
typedef result_t (*get_paint_msg_fn)(handle_t wnd);
/** Called when a begin paint function is called */
typedef result_t (*begin_paint_fn)(handle_t wnd);
/** Called when an end paint function is called */
typedef result_t (*end_paint_fn)(handle_t wnd);

// shared structure for all canvas's
typedef struct _canvas_t
  {
  // sizeof the canvas.  This should include any variable data.
  // minumum is sizeof(canvas_t)
  size_t version;
  // dimensions of the canvas
  gdi_dim_t width;          // width of the buffer
  gdi_dim_t height;         // hight of the buffer
  uint16_t bits_per_pixel;  // number of bits in a pixel
  uint16_t orientation;     // 0, 90, 180, 270
  point_t offset;           // if non-0 is the physical pixel offset
                            // of the canvas 0,0
  bool own_buffer;          // true if this canvas is owned.
  // when the event queue is empty and the get_message
  // function is called, this optional callback will be called
  // to get a wm_paint message
  get_paint_msg_fn queue_empty;
  // called to notify that the application is beginning to paint
  begin_paint_fn begin_paint;
  // called to notify that the application has completed a paint.
  end_paint_fn end_paint;
  // get pixel function
  get_pixel_fn get_pixel;
  set_pixel_fn set_pixel;
  fast_fill_fn fast_fill;
  fast_line_fn fast_line;
  fast_copy_fn fast_copy;
  copy_bitmap_fn bitmap_copy;
  } canvas_t;

/**
 * Return a new canvas that is the framebuffer
 * @param canvas  Newly opened canvas
 * @param device_metrics  canvas dpi
 * @return s_ok if canvas opened
 */
extern result_t bsp_canvas_open_framebuffer(canvas_t **canvas);
/**
 * Create a canvas given the extents given
 * @param size  Size of canvas to create
 * @param hndl  handle to new canvas
 * @return s_ok if created ok
 */
extern result_t bsp_canvas_create_rect(const extent_t *size, canvas_t **canvas);
/**
 * Create a canvas given the extents given as a child window canvas
 * @param parent_canvas The canvas that is the parent of the child
 * @param rect  Size and position of canvas to create
 * @param hndl  handle to new canvas
 * @return s_ok if created ok
 */
extern result_t bsp_canvas_create_child(canvas_t *parent_canvas, const rect_t *rect, canvas_t **canvas);
/**
 * Create a canvas and select the bitmap bits into it
 * @param bitmap  Device independent bitmap to create canvas from
 * @param canvas  resulting canvas with bitmap converted to Device dependent bitmap
 * @return s_ok if canvas created ok
 */
extern result_t bsp_canvas_create_bitmap(const bitmap_t *bitmap, canvas_t **canvas);
/**
 * Close a canvas and return all resources
 * @param hndl  handle to canvas
 * @return s_ok if released ok
 */
extern result_t bsp_canvas_close(canvas_t *hndl);
/**
 * Open the registry functions.  Must be called from the low level code
 * @param factory_reset  true if reset device back to an empty eeprom
 * @param size	Number of memid's
 * @param row_size	Maximum size of row that can be read/written
 * @param task_callback	Syncronisation semaphore to use if needed
 * @return s_ok if opened ok
 */
extern result_t bsp_reg_init(bool factory_reset, uint16_t size, uint16_t row_size);
/**
 * Read blocks from the registry memory
 * @param memid			Starting memid
 * @param bytes_to_read	Total bytes to read.  Note this will always be multiples of the row size
 * @param buffer		Data buffer to read into
 * @param task_callback	Syncronisation semaphore to use if needed
 * @return s_ok if opened ok
 */
extern result_t bsp_reg_read_block(uint32_t memid, uint16_t bytes_to_read, void *buffer);
/**
 * Read blocks to the registry memory
 * @param memid			Starting memid
 * @param bytes_to_write	Total bytes to write.  Note this will always be multiples of the row size
 * @param buffer		Data buffer to write from
 * @param task_callback	Syncronisation semaphore to use if needed
 * @return s_ok if opened ok
 */
extern result_t bsp_reg_write_block(uint32_t memid, uint16_t bytes_to_write, const void *buffer);

typedef struct _neutron_parameters_t
  {
  // name of the node reported in the nis message
  const char *node_name;
  // node identifier.  Published in id msg
  uint8_t node_id;
  // type of the node
  uint8_t node_type;
  // Revision to support in id msg
  uint8_t hardware_revision;
  // Revision to support in id msg
  uint8_t software_revision;
  // size of can transmit buffer, if 0 default size used
  uint16_t tx_length;
  // size of can tx worker task stack, if 0 default size used
  uint16_t tx_stack_length;
  // size of can receive buffer, if 0 default size used
  uint16_t rx_length;
  // size of can rx worker task stack, if 0 default size used
  uint16_t rx_stack_length;
  // can publisher worker, if 0 default size used
  uint16_t publisher_stack_length;
  // rate to operate the can bus as
  uint32_t bitrate;
  // clock supplied to the can controller
  uint32_t fsys;
  } neutron_parameters_t;

/**
 * Initialize the can-aerospace subsystem, then initialize neutron
 * @param params
 * @param init_mode
 * @param create_publish_task
 * @return 
 */  
extern result_t can_aerospace_init(const neutron_parameters_t *params, bool init_mode, bool create_publish_task);

/**
 * Initialize Neutron
 * @param params      setup and memory parameters
 * @param init_mode   true if a factory reset
 * @param worker      Mutual exclusion semaphore.
 * @return s_ok if started ok
 */
extern result_t neutron_init(const neutron_parameters_t *params, bool init_mode, bool create_worker);
/**
 * Worker process
 * @param pargs Arguments
 */
extern void publish_task(void *pargs);

/////////////////////////////////////////////////////////////////////////////
//
// These are calls to the implementation of the hardware
/**
 * Initialize the hardware
 * @param rx_queue  Receive message queue.  The hardware needs to make an ISR safe call to push data onto queue
 */
extern result_t bsp_can_init(deque_p rx_queue, uint32_t fsys, uint32_t bitrate);
/**
 * Send a message
 * @param msg
 */
extern result_t bsp_send_can(const canmsg_t *msg);
/**
 * Set the rate of the canbus
 * @param rate
 * @return 
 */
extern result_t bsp_set_can_rate(uint16_t rate);

/////////////////////////////////////////////////////////////////////////////
//
// Framebuffer funcions

/**
 * Normally called by the vsync on hardware to say that a redraw should happen
 * @return s_ok if processed ok
 */
extern result_t bsp_sync();
/**
 * Update the framebuffer in-memory image to the hardware
 * @return
 */
extern result_t bsp_sync_framebuffer();
/**
 * Called when the hardware has finished processing the framebuffer
 * will allow painting onto the framebuffer
 * @return
 */
extern result_t bsp_sync_done();
/**
 * Query if a paint message should be sent
 * @return s_ok if send paint, s_false if not
 */
extern result_t bsp_queue_empty(handle_t wnd);
/**
 * The gdi is started painting, may be called more than once
 * @return s_ok if paint ok
 */
extern result_t bsp_begin_paint(handle_t wnd);
/**
 * The gdi has finished painting
 * @return s_ok if processed.
 */
extern result_t bsp_end_paint(handle_t wnd);

#ifdef __cplusplus
  }
#endif


#endif	/* BSP_H */

