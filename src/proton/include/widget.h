#ifndef __widget_h__
#define __widget_h__

#include "../include/proton.h"

/**
 * @brief General purpose paint handler template
*/
typedef void (*paint_fn)(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
/**
 * @brief Message hook for the widget
*/
typedef result_t(*on_message_fn)(handle_t hwnd, const canmsg_t* msg, void* wnddata);

typedef struct _widget_t widget_t;
/**
 * @brief Create hook, called before the derived widget is created
*/
typedef result_t (*on_create_widget_fn)(handle_t hwnd, widget_t* widget);

/**
 * Base struct for an event.
 */
typedef struct _event_args_t
  {
  // is sizeof(event);
  uint32_t version;

  } event_args_t;

/**
 * Raised when a setting is changed
 */
typedef struct _settings_changed_event_t
  {
  event_args_t base;

  const char *setting;
  variant_t value;
  } settings_changed_event_t;

/**
 * @brief Used to cache settings from the map
*/
typedef result_t(*on_event_fn)(widget_t* widget, const event_args_t *args);

/**
 * @brief base structure for the widget wnddata.
*/


#define BORDER_LEFT     0x8000
#define BORDER_TOP      0x4000
#define BORDER_RIGHT    0x2000
#define BORDER_BOTTOM   0x1000
#define BORDER_NONE     0x0000
#define FILL_BACKGROUND 0x0800
#define DRAW_NAME       0x0400

typedef struct _widget_t {
  handle_t hwnd;
  // note these 2 are only used in the construction of the widget and
  // shold not really be here.
  uint32_t style;
  rect_t rect;              // relative to parent

  gdi_dim_t background_gutter; // if non-zero indent background by this amount
  color_t gutter_color;     // color to paint the gutter
  color_t border_color;
  color_t background_color;
  color_t name_color;
  const char *name;
  const font_t* name_font;
  point_t name_pt;
  uint32_t status_timeout;    // msec to delay before the widget shows a red-cross
                              // to indicate the data stream is not valid
  uint16_t sensor_id;
  uint16_t alarm_id;          // can-id for an alarm to highlight the control
  color_t alarm_color;        // color to paint the alarm

  /**
   * @brief if > 0 then the widget will have this z_order
  */
  uint8_t z_order;

  // paint handler functions.  Will be called by
  // on_widget_msg if not 0
  paint_fn on_paint;
  paint_fn on_paint_background;
  on_event_fn on_settings_changed;
  on_message_fn on_message;
  on_create_widget_fn on_create;

  bool sensor_failed;         // set when the widget sensor is not responding
  uint32_t sensor_timer;      // counts till the sensor is not detected anymore
  bool is_alarm;              // set when an alarm is seen
  void *extra;                // extra data for widget
  } widget_t;

/**
 * @brief Create a widget using the supplied parameters
 * @param parent  Parent window to use
 * @param id      Id of the new widget
 * @param cb      Window proceedure
 * @param wnd     Widget data to store with the window
 * @param hwnd    Result of the create
 * @return s_ok if the widget created ok
*/
extern result_t create_widget(handle_t parent, uint32_t flags, wndproc_fn cb, widget_t *wnd, handle_t* hwnd);
/**
 * @brief Draw the base widget.
 * @param hwnd  window handle
 * @param msg window message
 * @param wnddata widget data
*/
extern result_t on_paint_widget(handle_t hwnd, const canmsg_t *msg, widget_t *wnddata);
/**
 * @brief Draw the base widget
 * @param canvas    canvas to paint on
 * @param rect      window rect
 * @param msg       message
 * @param widget    the widget data
*/
extern void on_paint_widget_background(handle_t canvas, const rect_t* rect, const canmsg_t* msg, widget_t* widget);
/**
 * Display a roller
 * @param canvas   Canvas to paint on
 * @param bounds    Area for roller
 * @param value     Value, lower nn digits are displayed smaller
 * @param digits    Number of digits to draw in small font, 0 for none
 * @param fg_color
 * @return
 */
extern result_t on_display_roller(handle_t hwnd,
  const rect_t *bounds,
  int32_t value,
  int digits,
  color_t fg_color,
  const font_t* large_font,
  const font_t* small_font);

/**
 * @brief Paint the background for a roller.
 * @param canvas      Canvas to paint on
 * @param bounds      extent of the roller
 * @param bg_color    Backgound color to use
 * @param border_color  Color to outline in
 * @param left_ptr    Draw pointer to the left
 * @param value_box   Resulting box to draw the value in
 * @return 
*/
extern result_t on_paint_roller_background(handle_t canvas,
  const rect_t* bounds,
  color_t bg_color,
  color_t border_color,
  bool left_ptr,
  rect_t *value_box);

/**
 * @brief Get the color of the sidget background
 * @param hwnd handle to window
 * @param color color to get
 * @return
*/
extern result_t get_background_color(handle_t hwnd, color_t* color);
/**
 * @brief Set the color of the widget background
 * @param hwnd handle to window
 * @param color color to get
 * @return
*/
extern result_t set_background_color(handle_t hwne, color_t color);
/**
 * @brief Return s_true id the parent widget is a modal dialog
 * @param parent parent to text
 * @return
*/
extern result_t is_dialog_modal(handle_t parent);

extern result_t widget_wndproc(handle_t hndl, const canmsg_t *msg, void *wnddata);

#endif
