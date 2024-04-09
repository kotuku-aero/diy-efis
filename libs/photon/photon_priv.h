#ifndef __photon_bsp_h__
#define	__photon_bsp_h__
#include <stdio.h>

#include "window.h"
#include "../neutron/neutron.h"
#include "../neutron/type_vector.h"
#include "photon.h"

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

  typedef struct _gdi_t  gdi_t;
  typedef struct _screen_surface_t screen_surface_t;
  /**
  * Get a pixel from the canvas
  * @param canvas    Canvas to reference
  * @param src       point on canvas
  * @return color at point
  */
  typedef result_t(*get_pixel_fn)(gdi_t* fb, const point_t* src, color_t* out);
  /**
  * Set a pixel on the canvas
  * @param canvas  Canvas to reference
  * @param dest    point on canvas
  * @param color   color to set
  * @return returns the existing pixel
  */
  typedef result_t(*set_pixel_fn)(gdi_t* fb, const point_t* dest, color_t color, color_t* out);
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
  typedef result_t(*fast_fill_fn)(gdi_t* fb, const rect_t* dest, color_t fill_color);

  /**
  * Draw a line between 2 points
  * @param canvas      canvas to draw on
  * @param p1          first point
  * @param p2          second point
  * @param fill_color  color to fill with
  */
  typedef result_t(*fast_line_fn)(gdi_t* fb, const point_t* p1, const point_t* p2, color_t fill_color);
  /**
  * Perform a fast copy from source to destination
  * @param dest        Destination in pixel buffer (0,0)
  * @param src_canvas  Canvas to copy from
  * @param src         area to copy
  * @param operation   copy mode
  * @remarks The implementation can assume that the src rectangle will be clipped to the
  * destination canvas metrics
  */
  typedef result_t(*fast_copy_fn)(gdi_t* fb, const point_t* dest, const gdi_t* src_canvas,
    const rect_t* src, raster_operation operation);
  /**
   * @brief This is called when all messages are read, painting
   * will start.
  */
  typedef result_t(*queue_empty_fn)(screen_surface_t* surface);

  /* This streucture is used to hold the details of a physical framebuffer
  *
  * The HAL should return this structure when the framebuffer is opened or created
  */
  typedef struct _surface_t  surface_t;

  /**
   * @brief This holds a drawing handle.  For child windows this structure holds
   * a handle to the physical buffer.
  */
  typedef struct _gdi_t {
    // get pixel function
    get_pixel_fn get_pixel;
    set_pixel_fn set_pixel;
    fast_fill_fn fast_fill;
    fast_line_fn fast_line;
    fast_copy_fn fast_copy;
    bool invalid;             // the framebuffer has been changed.

    // dimensions of the canvas
    rect_t position;            // this is the allocated position relative to the parent.
    // this is cached so that the invalid flag can be
    // set quickly, otherwise the parent framebuffer must
    // be found, or when == 0 that is the surface
    bool is_surface;            // true if this is the surface 
    surface_t *surface;         // actual drawing surface
    rect_t absolute_position;   // this is the absolute position of framebuffer on the canvas
                                // re-calculated when the set_position is called
  } gdi_t;

  /**
   * @brief This is a surface that holds details of a physcial framebuffer
   * 
   */
  typedef struct _surface_t {
    gdi_t base;
    int invalid_count;        // number of times the surface has been invalidated
    int paint_depth;          // depth of the paint stack

    volatile color_t* buffer; // if != 0 then this is the actual buffer
    bool release_buffer;      // true if the base surface
    extent_t extent;          // dimensions of the buffer
    uint16_t orientation;     // 0, 90, 180, 270
    } surface_t;


  typedef enum
    {
    fbds_idle,                // idle, write to buffer
    fbds_painting,            // queue empty so paint buffer
    fbds_in_sync,             // waiting for the buffer to be sync'd to the display
    fbds_in_sync_need_paint,  // need another paint when the buffer is released
    } framebuffer_draw_state;

  typedef struct _screen_surface_t {
    surface_t base;

    framebuffer_draw_state state;
    } screen_surface_t;

  extern result_t get_screen_surface(handle_t hwnd, screen_surface_t** surface);

  // drawing routines for a standard framebuffer
  // these can be replaced by the HAL and use hardware acceleration if required
  extern result_t surface_get_pixel(gdi_t* fb, const point_t* src, color_t* out);
  extern result_t surface_set_pixel(gdi_t* fb, const point_t* dest, color_t color, color_t* out);
  extern result_t surface_fast_fill(gdi_t* fb, const rect_t* dest, color_t fill_color);
  extern result_t surface_fast_line(gdi_t* fb, const point_t* p1, const point_t* p2, color_t fill_color);
  extern result_t surface_fast_copy(gdi_t* fb, const point_t* dest, const gdi_t* src_canvas, const rect_t* src, raster_operation operation);
  extern result_t surface_copy_bitmap(gdi_t* fb, const point_t* dest_pt, const bitmap_t* src_bitmap, const rect_t* src_rect);
  extern volatile color_t* point_to_address(surface_t* fb, const point_t* src, extent_t* stride);

  typedef struct _canvas_t {
    base_t base;
    // cached values used by the polygon/polyline engine
    points_t path;            // the points to draw
    points_t clip_points;     // clipper boundary
    points_t solution;        // clipped points

    edges_t edges;            // used by the polygon filler

    gdi_t* fb;                // the actual framebuffer, allocated by HAL
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

    canvas_t *canvas;

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

  typedef uint32_t(*pack_pixel_fn)(surface_t* fb, color_t color);
  typedef color_t(*unpack_pixel_fn)(surface_t* fb, uint32_t pixel);

  /**
   * @brief Screen object.  Only ever 1
  */
  typedef struct _screen_t {
    window_t base;

    queue_empty_fn queue_empty;

    // this is a deque of events and window handles
    handle_t event_queue;
  } screen_t;

  extern const typeid_t screen_type;


  // utility functions to support converting coordinates to
  // absolute coordinates
  static inline const point_t *to_absolute_pt(const point_t *pt, const gdi_t *fb, point_t *out)
  {
    gdi_dim_t x = pt->x + fb->absolute_position.left;
    gdi_dim_t y = pt->y + fb->absolute_position.top;

    return point_create(x, y, out);
  }

  static inline const rect_t *to_absolute_rect(const rect_t *rect, const gdi_t *fb, rect_t *out)
  {
    point_t top_left;
    rect_top_left(rect, &top_left);
    point_t bottom_right;
    rect_bottom_right(rect, &bottom_right);

    to_absolute_pt(&top_left, fb, &top_left);
    to_absolute_pt(&bottom_right, fb, &bottom_right);

    out->left = top_left.x;
    out->top = top_left.y;
    out->right = bottom_right.x;
    out->bottom = bottom_right.y;

    return out;
  }

#ifndef _WIN32
  /**
   * @brief Post a message from an ISR
   * @param msg message to post
   * @return s_ok if posted ok
   */
  extern result_t post_message_from_isr(const canmsg_t* msg);
#endif
  /**
   * @brief Start the framebuffer system.  Called before any GDI functions
   * @param dimension     Pixels, x and y
   * @param orientation   Orientaion, only 0, 90, 180 and 270 supported
   * @return s_ok if the framebuffer started ok
  */
  extern result_t bsp_start_framebuffer(const extent_t *dimension, uint16_t orientation);
  /**
   * Return a new canvas that is the framebuffer
   * @param orientation The orientation of the layer drawing surface
   * @param fb    framebuffer that is the physical device.
   * @return s_ok if canvas opened
   */
  extern result_t bsp_open_framebuffer(screen_surface_t** fb);
  /**
   * Create a canvas given the extents given
   * @param size  Size of canvas to create
   * @param hndl  handle to new canvas
   * @return s_ok if created ok
   */
  extern result_t bsp_framebuffer_create_rect(const extent_t* size, gdi_t** fb);
  /**
   * Create a canvas and select the bitmap bits into it
   * @param bitmap  Device independent bitmap to create canvas from
   * @param canvas  resulting canvas with bitmap converted to Device dependent bitmap
   * @return s_ok if canvas created ok
   */
  extern result_t bsp_framebuffer_create_bitmap(const bitmap_t* bitmap, gdi_t** fb);
  /**
   * Close a canvas and return all resources
   * @param hndl  handle to canvas
   * @param free_handle true if the underlying handle is to be released
   * @return s_ok if released ok
   */
  extern result_t bsp_framebuffer_close(gdi_t* fb);
  /**
   * @brief allocate memory for the framebuffer
   * @param size size of memory to allocate
   * @param mem Pointer to memory
   * @return s_ok if allocated ok
  */
  extern result_t bsp_fb_malloc(size_t size, void** mem);
  /**
   * @brief Free a memory area allocated by bsp_fb_malloc
   * @param mem memory to free
   * @return s_ok if allocated ok.
  */
  extern result_t bsp_fb_free(void* mem);


  /////////////////////////////////////////////////////////////////////////////
  //
  // Framebuffer functions

  /**
   * Normally called by the vsync on hardware to say that a redraw should happen
   * @return s_ok if processed ok
   */
  extern result_t gdi_sync(screen_surface_t*gdi);
  /**
   * @brief Called when a paint is complete
   * @param hwnd 
   * @return 
  */
  extern result_t gdi_painting_done(screen_surface_t *gdi);
  /**
   * Update the framebuffer in-memory image to the hardware
   * called when the windows are painted
   * @return
   */
  extern result_t bsp_sync_framebuffer();
  /**
   * Called when the hardware has finished processing the framebuffer
   * will allow painting onto the framebuffer
   * @return
   */
  extern result_t window_sync_done(screen_surface_t* gdi);
  /**
   * Query if a paint message should be sent
   * @return s_ok if send paint, s_false if not
   */
  extern result_t window_queue_empty(screen_surface_t* gdi);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// 
  /// CanFly/Neutron startup
  ///
  /// Only call this is there is a GDI required and a framebuffer is available. 
  /// The device needs to be linked with the photon library.
  /// 

  typedef struct _photon_parameters_t {
    extent_t gdi_extents;
  } photon_parameters_t;

  extern result_t photon_init(const photon_parameters_t* params, bool init_mode);

  extern result_t initialize_screen(const photon_parameters_t* params);

  // internal implementation
  extern result_t create_canvas_from_framebuffer(gdi_t *fb, handle_t *hndl);

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

