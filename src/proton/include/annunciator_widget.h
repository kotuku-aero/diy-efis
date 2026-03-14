#ifndef __annunciator_widget_h__
#define __annunciator_widget_h__

#include "widget.h"
#include "converters.h"

typedef struct _annunciator_t annunciator_t;

typedef struct _annunciator_t {
  widget_t base;
  handle_t background_canvas;
  } annunciator_t;

typedef struct _text_annunciator_t {
  annunciator_t base;

  uint16_t can_id;
  color_t label_color;
  gdi_dim_t label_offset;
  text_flags label_format;
  
  color_t text_color;
  gdi_dim_t text_offset;
  text_flags text_format;

  const font_t* small_font;
  bool compact;
  paint_fn on_paint_background;
  } text_annunciator_t;

typedef struct _auto_annunciator_t {
  text_annunciator_t base;

  const char *fmt;
  const selected_unit_t* converter;

  variant_type value_type;
  variant_t value;
  } auto_annunciator_t;

extern void on_draw_text(handle_t canvas, const rect_t* wnd_rect, annunciator_t* wnd, const char* value);
extern void on_paint_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);
extern void on_paint_text_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);

//-----------------------------------------------------------------------------
// Autopilot mode annunciator
//
// draws a text annunciator showing the current autopilot mode
#define AP_MODE_ENABLED   0x0001

#define AP_MODE_CRS 0x0010
#define AP_MODE_HDG 0x0020

#define AP_MODE_ALT 0x0100
#define AP_MODE_VS  0x0200

#define AP_MODE_REVERSE_LEFT  0x1000
#define AP_MODE_REVERSE_RIGHT 0x2000

#define AP_MODE_HDG_ALT  (AP_MODE_HDG | AP_MODE_ALT | AP_MODE_ENABLED)
#define AP_MODE_CRS_ALT  (AP_MODE_CRS | AP_MODE_ALT | AP_MODE_ENABLED)
#define AP_MODE_HDG_VS   (AP_MODE_HDG | AP_MODE_VS | AP_MODE_ENABLED)
#define AP_MODE_CRS_VS   (AP_MODE_CRS | AP_MODE_VS | AP_MODE_ENABLED)

typedef struct _apmode_annunciator_t {
  text_annunciator_t base;
  uint16_t mode;  // current mode
  } apmode_annunciator_t;

extern result_t on_apmode_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_apmode(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);

//-----------------------------------------------------------------------------
// Waypoint annunciator
typedef struct _waypoint_annunciator_t {
  text_annunciator_t base;
  char ident[8];   // waypoint ident (7char) + null
  } waypoint_annunciator_t;

extern result_t on_waypoint_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_waypoint(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);

//-----------------------------------------------------------------------------
// UTC time
typedef struct _datetime_annunciator_t {
  text_annunciator_t base;
  const char *format;
  tm_t clock;
  bool is_null;
  char txt[64];   // text representation
  } datetime_annunciator_t;

extern result_t on_datetime_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_datetime(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);

//-----------------------------------------------------------------------------
// hp
typedef struct _hp_annunciator_t {
  text_annunciator_t base;

  uint16_t hp;        // hp * 100
  } hp_annunciator_t;

extern result_t on_hp_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_hp(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

//-----------------------------------------------------------------------------
// kmag
typedef struct _ecu_annunciator_t {
  annunciator_t base;

  e_board_status left_status;
  int16_t left_advance;

  e_board_status right_status;
  int16_t right_advance;

  uint16_t afr_map_mode;

  } ecu_annunciator_t;

extern result_t on_kmag_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_kmag(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

//-----------------------------------------------------------------------------
// hobbs
typedef struct _hobbs_annunciator_t {
  text_annunciator_t base;

  uint32_t hobbs;        // time in minutes
  } hobbs_annunciator_t;

extern result_t on_hobbs_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_hobbs(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

extern result_t on_auto_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);

extern result_t annunciator_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata);
/**
 * @brief Create a annunciator window
 * @param wnd
 * @param hndl
 * @return
*/
extern result_t create_annunciator_widget(handle_t parent, uint32_t flags, annunciator_t* wnd, handle_t* hndl);

#endif
