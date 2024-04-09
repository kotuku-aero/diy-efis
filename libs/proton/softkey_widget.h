#ifndef __softkey_window_h__
#define __softkey_window_h__

#include "widget.h"
#include "../graviton/aircraft.h"

typedef struct _softkey_window_t {
  widget_t base;
  handle_t background_canvas;

  uint16_t enable_id;
  // message to send if tapped
  canmsg_t tapped_msg;
  // message to send if pressed, similar to a tap but longer
  // note an activated message is sent first
  canmsg_t pressed_msg;
  // delay between sending pressed messages
  // 0 = only once
  // 1-65535 msg delay (a pressed message occurs about 250 msec apart)
  uint16_t pressed_delay;
  uint16_t pressed_ctr;

  // these are optional and if present
  // will not draw base text
  // glyph size must match the button size
  handle_t glyph;
  handle_t selected_glyph;

  bool is_selected;
  bool is_enabled;
  } softkey_window_t;


/**
 * @brief Create a softkey window
 * @param parent parent window to use
 * @param rect   softkey rectangle
 * @param id     window ID
 * @param wnd    window data
 * @param hndl   optional handle of the window
 * @return 
*/
extern result_t create_softkey_window(handle_t parent, uint16_t id, aircraft_t* aircraft, softkey_window_t* wnd, handle_t* hndl);

#endif
