#ifndef __window_h__
#define __window_h__

#include "photon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef result_t (*wndproc_fn)(handle_t hwnd, const canmsg_t *msg, void *wnddata);
extern const typeid_t window_type;

  /**
   * @brief Create a new child window.
   * @param parent parent of the child. Cannot be 0
   * @param bounds 
   * @param cb 
   * @param wnddata
   * @param id 
   * @param canvas 
   * @return 
  */
  extern result_t window_create(handle_t parent, const rect_t *bounds, wndproc_fn cb, void *wnddata, uint16_t id, handle_t *hndl);
    /**
     * @brief Get the parent of a window or screen
     * @param hwnd    handle to window
     * @param parent  parent of the window
     * @return
    */
  extern result_t window_parent(handle_t hwnd, handle_t *parent);
    /**
     * @brief Associate user data with a window or screen
     * @param hwnd      Handle to window
     * @param wnddata   Data
     * @return
    */
  extern result_t set_wnddata(handle_t hwnd, void *wnddata);
    /**
     * @brief return any window data associated with a window
     * @param hwnd      handle to window
     * @param wnddata   Data to store
     * @return
    */
  extern result_t get_wnddata(handle_t hwnd, void **wnddata);
    /**
     * @brief get the user space window proceedure
     * @param hwnd      Handle to window
     * @param wnddata   Data.
     * @return
    */
  extern result_t get_wndproc(handle_t hwnd, wndproc_fn *wndproc, void **wnddata);
  /**
   * @brief get a window's id
   * @param canvas  handle to window
   * @param id    resulting id
   * @return s_ok if a valid window
  */
  extern result_t get_window_id(handle_t hwnd, uint16_t *id);
  /**
   * @brief set a window's id
   * @param canvas  handle to window
   * @param id    resulting id
   * @return s_ok if a valid window
  */
  extern result_t set_window_id(handle_t hwnd, uint16_t id);
  /**
   * @brief Insert the window before the sibling in the chain of windows
   * @param wnd       window to insert
   * @param sibling   sibling to insert before
   * @return s_ok if inserted
   */
  extern result_t insert_before(handle_t hwnd, handle_t sibling);
  /**
   * @brief Insert the window after the sibling in the chain of windows
   * @param wnd       window to insert
   * @param sibling   sibling to insert after
   * @return s_ok if inserted
   */
  extern result_t insert_after(handle_t hwnd, handle_t sibling);
  /**
 * @brief return the first child of the window
 * @return s_ok if a valid window
*/
  extern result_t get_first_child(handle_t hwnd, handle_t *child);
  /**
   * @brief return the next sibling of a window
   * @param sibling sibling
   * @return s_ok if a valid window
  */
  extern result_t get_next_sibling(handle_t hwnd, handle_t *sibling);
  /**
   * @brief return the previous sibling of a window
   * @param sibling
   * @return s_ok is a previous sibling
  */
  extern result_t get_previous_sibling(handle_t hwnd, handle_t *sibling);
  /**
   * @brief Make a window visible
   * @param hwnd window to make visible
   * @return s_ok if a valid window
  */
  extern result_t show_window(handle_t hwnd);
  /**
   * @brief hide a window
   * @param hwnd window to hide
   * @return s_ok if window is hidden
  */
  extern result_t hide_window(handle_t hwnd);
  /**
   * @brief determine if the window is visible
   * @param hwnd window
   * @return s_ok if visible, s_false if not
  */
  extern result_t is_visible(handle_t hwnd);
  /**
 * @brief Return the position of the canvas.  This is relative to the
 * parent hwnd, if one exists
 * @return reference to the position
*/
  extern result_t window_getpos(handle_t hwnd, rect_t *pos);
  /**
   * @brief Set the position of the window relative to the parent window
   * @param hwnd window
   * @param pos
  */
  extern result_t window_setpos(handle_t hwnd, const rect_t *pos);
  /**
   * @brief Return the window rectange.
   * @param hwnd 
   * @param rect 
   * @return 
  */
  extern result_t window_rect(handle_t hwnd, rect_t *rect);
  /**
   * @brief Return the window painting order
   * @param hwnd window
   * @return order to draw in
  */
  extern result_t get_z_order(handle_t hwnd, uint8_t *value);
  /**
   * @brief Set the window painting order
   * @param value order
  */
  extern result_t set_z_order(handle_t hwnd, uint8_t value);
  /**
   * @brief Invalidate a rectangle.  In most cases rect is ignored
   * @param rect rectangle to invalidate.  Use position() in most cases
  */
  extern result_t invalidate_background_rect(handle_t hwnd, const rect_t *rect);
  /**
     * @brief Invalidate a rectangle.  In most cases rect is ignored
     * @param rect rectangle to invalidate.  Use position() in most cases
    */
  extern result_t invalidate_foreground_rect(handle_t hwnd, const rect_t* rect);
  /**
     * @brief Invalidate a rectangle.  In most cases rect is ignored
     * @param rect rectangle to invalidate.  Use position() in most cases
    */
  extern result_t invalidate_overlay_rect(handle_t hwnd, const rect_t* rect);
  /**
   * @brief Window needs to be painted.  Reset when on_paint is processed
   * @return true if the window is invalid and needs painting
  */
  extern result_t is_invalid(handle_t hwnd);
  /**
   * @brief Find a window by its id
   * @param id    id to find
   * @return s_ok if found
  */
  extern result_t get_window_by_id(handle_t hwnd, uint16_t id, handle_t *child);
  /**
   * @brief notify that window is starting to paint on the gdi
   * @param canvas
  */
  extern result_t begin_background_paint(handle_t hwnd, handle_t* canvas);
  /**
   * @brief notify that the window has finished painting
   * @param canvas
  */
  extern result_t end_background_paint(handle_t hwnd);
  /**
   * @brief notify that window is starting to paint on the gdi
   * @param canvas 
  */
  extern result_t begin_foreground_paint(handle_t hwnd, handle_t *canvas);
  /**
   * @brief notify that the window has finished painting
   * @param canvas 
  */
  extern result_t end_foreground_paint(handle_t hwnd);
  /**
  * @brief notify that window is starting to paint on the gdi
  * @param canvas
 */
  extern result_t begin_overlay_paint(handle_t hwnd, handle_t* canvas);
  /**
   * @brief notify that the window has finished painting
   * @param canvas
  */
  extern result_t end_overlay_paint(handle_t hwnd);
  /**
   * @brief return the canvas to render the background onto
   * @param hwnd window requesting the canvas
   * @return s_ok if the canvas exists
  */
  extern result_t get_background_canvas(handle_t hwnd, handle_t *canvas);
  /**
   * @brief Return the canvas to render the foreground onto
   * @param hwnd window requesting the canvas
   * @return s_ok if the canvas exists
  */
  extern result_t get_foreground_canvas(handle_t hwnd, handle_t *canvas);
  /**
   * @brief Return the overlay canvas to render the foreground onto
   * @param hwnd window requesting the canvas
   * @return s_ok if the canvas exists
  */
  extern result_t get_overlay_canvas(handle_t hwnd, handle_t *canvas);
  /**
   * @brief handle the default message processing.  Should be called after all
   * window processing a message is done
   * @param wnd window to process message for
   * @param msg message to process
  */
  extern result_t defwndproc(handle_t wnd, const canmsg_t *msg, void *wnddata);

  /**
   * @brief Return the root window that all windows should paint on
   * @param layer to get the root window for
   * @param canvas handle to the layer
   * @return s_ok if the layer is supported.
  */
  extern result_t get_screen(handle_t *hndl);
  /**
   * @brief Post a message to a screen or window
   * @param canvas window to post to, 0 to post to all windows
   * @param msg message to sent
   * @param delay maximum delay to wait for the window queue
   * @return s_ok if the message posted ok
  */
  extern result_t post_message(handle_t hwnd, const canmsg_t *msg, uint32_t delay);
  /**
   * @brief Send a message to a window and don't return till the message is processed
   * @param canvas window to send message to, 0 to broadcast
   * @param msg message to send
   * @return s_ok if processed
  */
  extern result_t send_message(handle_t hwnd, const canmsg_t *msg);
  /**
   * @brief get the next message from the message queue
   * @param layer layer to get messages from
   * @param hwnd window the message is for, 0 if a broadbase message
   * @param msg message to process
   * @return s_ok if messages are available
  */
  extern result_t get_message(handle_t * hwnd, canmsg_t *msg);

enum {
  e_not_png = -100, /* image data does not have a PNG header */
  e_png_malformed = -101, /* image data is not a valid PNG image */
  e_png_unsupported = -102, /* critical PNG chunk type is not supported */
  e_png_interlaced = -103, /* image interlacing is not supported */
  e_png_bad_format = -104, /* image color format is not supported */
  };


/**
 * @brief message structure touch message
*/
typedef struct _touch_msg_t {
  uint16_t flags;           // message flags
  point_t location;         // screen relative position
  uint16_t arg;             // depends on message
  } touch_msg_t;

/* touch message flags */
#define TOUCH_BEGIN   0x0001
#define TOUCH_INERTIA 0x0002
#define TOUCH_END     0x0004
#define id_photon 1400
#define id_first_internal_msg id_photon
/* specific window messages
*
* Note these are always local messages so byte[0] of the
* raw message is ne
*/
#define id_timer 1400
#define id_paint_foreground 1401
#define id_paint_background 1402
#define id_paint_overlay 1403
#define id_request_paint_foreground 1404
#define id_request_paint_background 1405
#define id_request_paint_overlay 1406
#define id_close 1407             // the window is closed
#define id_end_dialog 1408        // the dialog is dismissed
/*
* Touch screen interfaces.
* 
*/
#define id_touch_tap      1410    // a simple tap
#define id_touch_two_tap  1411    // a two finger tap
                                  // arg is distance between fingers
#define id_touch_press    1412    // a press, like a tap but longer
#define id_touch_press_and_tap 1413 // one finger pressed, second tap
                                  // arg is distance between
#define id_touch_zoom     1414    // the zoom gesture
                                  // location is center of zoom
                                  // arg is distance
#define id_touch_pan      1415    // the pan gesture
                                  // location is end of gesture
                                  // arg is distance
#define id_touch_rotate   1416    // the rotate gesture
                                  // location is center of rotate
                                  // if flags & TOUCH_BEGIN the arg is start angle (0-360)
                                  // othwewise relative rotation in +/- 360

#define id_usermsg  1420
// 1480-1499 reserved for menu window events
#define id_last_internal_msg 1499

/**
 * @brief Register the window as a touch input window
 * @param hwnd  Window to receive touch messages
 * @param flags not used, set to 0
 * @return s_ok if window registered
*/
extern result_t register_touch_window(handle_t hwnd, uint16_t flags);

/**
 * @brief Determine if the tap event is for this window
 * similar to pt_in_window function but also uses z-order
 * @param hwnd window to test
 * @param msg  message to test
 * @param blur distance that if this is the topmost window, the pt is assumed
 * to be within if hit.  Allows for tapping edge of a window
 * @return s_ok if point is in the window
*/
extern result_t is_window_tap(handle_t hwnd, const canmsg_t* msg, uint16_t blur);
/**
 * @brief Convert the message to a touch message
 * @param msg touch message to decode
 * @return s_ok if decoded
*/
extern result_t get_touch_msg(const canmsg_t *msg, const touch_msg_t **out);

#ifdef __cplusplus
  }
#endif

#endif
