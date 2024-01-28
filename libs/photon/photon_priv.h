#ifndef __photon_bsp_h__
#define	__photon_bsp_h__
#include <stdio.h>

#include "window.h"
#include "../neutron/neutron.h"
#include "../neutron/type_vector.h"

#ifdef __cplusplus
extern "C" {
#endif
  // define sortable vector types

  type_vector_t(point)

  typedef struct _edge_t {
    point_t p1;
    point_t p2;

    gdi_dim_t fn;
    gdi_dim_t mn;
    gdi_dim_t d;
    } edge_t;

  typedef struct _intersection_t {
    point_t pt;
    uint16_t dist;
    } intersection_t;

  type_vector_t(edge)

#ifndef _WIN32
  extern float roundf(float value);
#endif

  typedef struct _framebuffer_t  framebuffer_t;

  /**
    * @brief Called when a window begins painting
  */
  typedef result_t(*begin_paint_fn)(framebuffer_t* fb);
  /**
    * @brief Called when a window ends painting
  */
  typedef result_t(*end_paint_fn)(framebuffer_t* fb);
  /**
    * @brief Called when the message queue is empty and the windows
    * have been drawn
  */
  typedef result_t(*queue_empty_fn)(framebuffer_t* fb);
  /**
  * Get a pixel from the canvas
  * @param canvas    Canvas to reference
  * @param src       point on canvas
  * @return color at point
  */
  typedef result_t(*get_pixel_fn)(framebuffer_t* fb, const point_t* src, color_t* out);
  /**
  * Set a pixel on the canvas
  * @param canvas  Canvas to reference
  * @param dest    point on canvas
  * @param color   color to set
  * @return returns the existing pixel
  */
  typedef result_t(*set_pixel_fn)(framebuffer_t* fb, const point_t* dest, color_t color, color_t* out);
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
  typedef result_t(*fast_fill_fn)(framebuffer_t* fb, const rect_t* dest, color_t fill_color);

  /**
  * Draw a line between 2 points
  * @param canvas      canvas to draw on
  * @param p1          first point
  * @param p2          second point
  * @param fill_color  color to fill with
  */
  typedef result_t(*fast_line_fn)(framebuffer_t* fb, const point_t* p1, const point_t* p2, color_t fill_color);
  /**
  * Perform a fast copy from source to destination
  * @param dest        Destination in pixel buffer (0,0)
  * @param src_canvas  Canvas to copy from
  * @param src         area to copy
  * @param operation   copy mode
  * @remarks The implementation can assume that the src rectangle will be clipped to the
  * destination canvas metrics
  */
  typedef result_t(*fast_copy_fn)(framebuffer_t* fb, const point_t* dest, const framebuffer_t* src_canvas,
    const rect_t* src, raster_operation operation);

  typedef enum
  {
    fbds_idle,                // idle, write to buffer
    fbds_painting,            // queue empty so paint buffer
    fbds_in_sync,             // waiting for the buffer to be sync'd to the display
    fbds_in_sync_need_paint   // need another paint when the buffer is released
  } framebuffer_draw_state;

  typedef enum _layer_type {
    lt_background,
    lt_foreground,
    lt_overlay
    } layer_type;

  typedef struct _framebuffer_t {
    layer_type type;
    // get pixel function
    get_pixel_fn get_pixel;
    set_pixel_fn set_pixel;
    fast_fill_fn fast_fill;
    fast_line_fn fast_line;
    fast_copy_fn fast_copy;
    // draw functions
    begin_paint_fn begin_paint;
    end_paint_fn end_paint;
    queue_empty_fn queue_empty;
    // dimensions of the canvas
    rect_t position;          // this is the allocated position relative to the
                              // canvas.  if own_buffer is true then is the extent of
                              // the buffer
    bool invalid;             // the framebuffer has been changed.
    bool is_surface;
    framebuffer_draw_state state;
    // count of how many build paint messages
    int paint_depth;

  } framebuffer_t;


  typedef struct _canvas_t {
    base_t base;
    framebuffer_t* fb;        // the actual framebuffer

    // cached values used by the polygon/polyline engine
    points_t path;            // the points to draw
    points_t clip_points;     // clipper boundary
    points_t solution;        // clipped points

    edges_t edges;            // used by the polygon filler
    } canvas_t;

  /**
   * @brief Clip the path loaded onto the canvas to the rectangle.  Result is in solution
   * @param canvas      Canvas to clip to
   * @param clip_rect   Rectangle to clip lines to
   * @return s_ok if clipped ok
  */
  extern result_t clip_path(canvas_t* canvas, const rect_t* clip_rect);

  extern const typeid_t canvas_type;

  typedef struct _window_t window_t;

  typedef struct _window_t {
    base_t base;

    handle_t background_canvas;
    handle_t foreground_canvas;
    handle_t overlay_canvas;

    // this is relative to the parent.
    // copied to each canvas
    rect_t position;

    // next in the list. 0 == end
    window_t* next;
    // previous in the list. 0 == end
    window_t* previous;
    // parent canvas, 0 = root window
    window_t* parent;
    // first child of this window
    window_t* child;
    // true if this window is visible
    bool visible;
    // window procedure used to process messages.
    wndproc_fn wndproc;
    // id of the window
    uint16_t id;
    // painting order
    uint8_t z_order;
    // users data
    void* wnddata;
  } window_t;

  extern const typeid_t window_type;

  /**
   * @brief Screen object.  Only ever 1
  */
  typedef struct _screen_t {
    window_t base;

    // set when a wm_paint is found
    bool needs_paint;
    // this is a deque of events and window handles
    handle_t event_queue;
  } screen_t;

  extern const typeid_t screen_type;

#ifndef _WIN32
  /**
   * @brief Post a message from an ISR
   * @param msg message to post
   * @return s_ok if posted ok
   */
  extern result_t post_message_from_isr(const canmsg_t* msg);
#endif

  /**
   * Return a new canvas that is the framebuffer
   * @param orientation The orientation of the layer drawing surface
   * @param lt    type of later to open
   * @param fb    framebuffer that is the physical device.
   * @return s_ok if canvas opened
   */
  extern result_t bsp_open_layer(uint16_t orientation, layer_type lt, framebuffer_t** fb);
  /**
   * Create a canvas given the extents given
   * @param size  Size of canvas to create
   * @param hndl  handle to new canvas
   * @return s_ok if created ok
   */
  extern result_t bsp_framebuffer_create_rect(const extent_t* size, framebuffer_t** fb);
  /**
   * Create a canvas given the extents given as a child window canvas
   * @param parent_canvas The canvas that is the parent of the child
   * @param rect  Size and position of canvas to create
   * @param fb  background canvas
   * @return s_ok if created ok
   */
  extern result_t bsp_framebuffer_create_child(framebuffer_t* parent_canvas, const rect_t* rect, framebuffer_t** fb);
  /**
   * Create a canvas and select the bitmap bits into it
   * @param bitmap  Device independent bitmap to create canvas from
   * @param canvas  resulting canvas with bitmap converted to Device dependent bitmap
   * @return s_ok if canvas created ok
   */
  extern result_t bsp_framebuffer_create_bitmap(const bitmap_t* bitmap, framebuffer_t** fb);
  /**
   * Close a canvas and return all resources
   * @param hndl  handle to canvas
   * @param free_handle true if the underlying handle is to be released
   * @return s_ok if released ok
   */
  extern result_t bsp_framebuffer_close(framebuffer_t* fb);


  /////////////////////////////////////////////////////////////////////////////
  //
  // Framebuffer functions

  /**
   * Normally called by the vsync on hardware to say that a redraw should happen
   * @return s_ok if processed ok
   */
  extern result_t window_sync(framebuffer_t* fb);
  /**
   * @brief mark the window as being invalid.
   * @param fb canvas to mark
   * @return s_ok when processed
  */
  extern result_t window_mark_invalid(framebuffer_t* fb);
  /**
   * @brief Get if the window needs painting
   * @param fb canvas to test
   * @return s_ok if needs painting
  */
  extern result_t window_is_invalid(framebuffer_t *fb);
  /**
   * Update the framebuffer in-memory image to the hardware
   * @return
   */
  extern result_t bsp_sync_framebuffer(framebuffer_t* fb);
  /**
   * Called when the hardware has finished processing the framebuffer
   * will allow painting onto the framebuffer
   * @return
   */
  extern result_t window_sync_done(framebuffer_t* fb);
  /**
   * Query if a paint message should be sent
   * @return s_ok if send paint, s_false if not
   */
  extern result_t window_queue_empty(framebuffer_t* fb);
  /**
   * The gdi is started painting, may be called more than once
   * @return s_ok if paint ok
   */
  extern result_t window_begin_paint(framebuffer_t* fb);
  /**
   * The gdi has finished painting
   * @return s_ok if processed.
   */
  extern result_t window_end_paint(framebuffer_t* fb);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// 
  /// CanFly/Neutron startup
  ///
  /// Only call this is there is a GDI required and a framebuffer is available. 
  /// The device needs to be linked with the photon library.
  /// 

  typedef struct _photon_parameters_t {
    extent_t gdi_extents;
    uint16_t orientation;
  } photon_parameters_t;

  extern result_t photon_init(const photon_parameters_t* params, bool init_mode);

  extern result_t initialize_screen(const photon_parameters_t* params);

  // internal implementation
  extern result_t create_canvas_from_framebuffer(framebuffer_t *fb, handle_t *hndl);

  /**
   * @brief Clip and render a polygon
   * @param canvas      Canvas, the member path holds the points to be rendered
   * @param clip_rect   Screen rectangle to clip to
   * @param pen         Color to outline polygon with
   * @param fill        Color to fill with
   * @return 
  */
  extern result_t polygon_impl(canvas_t* canvas, const rect_t* clip_rect, color_t pen, color_t fill);
  /**
   * @brief Draw a polyline
   * @param canvas      Canvas the member path holds the points to be renddred
   * @param clip_rect   Screen rectangle to clip to
   * @param pen         Color to draw the line as
   * @return 
  */
  extern result_t polyline_impl(canvas_t* canvas, const rect_t* clip_rect, color_t pen);
  extern const point_t* _rotate_point(const point_t* center, int16_t angle, point_t* pt);
  extern result_t ellipse_impl(canvas_t* canvas, const rect_t* clip_rect, color_t pen, color_t fill, const rect_t* area);

  extern bool clip_line(point_t* p1, point_t* p2, const rect_t* clip_rect);
  static inline point_t* pt_at(points_t* vec, uint16_t index) { return points_begin(vec) + index; }


#ifdef __cplusplus
}
#endif

#endif	/* BSP_H */

