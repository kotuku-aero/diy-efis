#ifndef __annunciator_widget_h__
#define __annunciator_widget_h__

#include "proton.h"
#include "converters.h"

typedef struct _annunciator_t annunciator_t;

typedef struct _annunciator_t {
  widget_t base;

  } annunciator_t;

typedef struct _text_annunciator_t {
  annunciator_t base;

  uint16_t can_id;
  color_t label_color;
  color_t text_color;
  const font_t* small_font;
  int32_t label_offset;
  int32_t text_offset;
  } text_annunciator_t;

typedef struct _auto_annunciator_t {
  text_annunciator_t base;

  const char *fmt;
  const selected_unit_t** converter;

  variant_type value_type;
  variant_t value;
  } auto_annunciator_t;

extern void on_paint_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnd);

//-----------------------------------------------------------------------------
// UTC time
typedef struct _utc_annunciator_t {
  text_annunciator_t base;

  uint16_t clock;
  } utc_annunciator_t;

extern bool on_def_utc_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
extern void on_paint_utc(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnd);

//-----------------------------------------------------------------------------
// Hours
typedef struct _hours_annunciator_t {
  text_annunciator_t base;

  uint32_t hours; // hobbs hours, stored in AHRS as hours * 100
  } hours_annunciator_t;

extern result_t on_hours_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
extern void on_paint_hours(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

//-----------------------------------------------------------------------------
// hp
typedef struct _hp_annunciator_t {
  text_annunciator_t base;

  uint16_t hp;        // hp * 100
  } hp_annunciator_t;

extern result_t on_hp_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
extern void on_paint_hp(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnddata);

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

extern result_t on_kmag_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
extern void on_paint_kmag(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnddata);

//-----------------------------------------------------------------------------
// hobbs
typedef struct _hobbs_annunciator_t {
  text_annunciator_t base;

  int16_t hobbs;        // hobbs * 100
  } hobbs_annunciator_t;

extern result_t on_hobbs_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
extern void on_paint_hobbs(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnddata);

extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
extern void on_paint_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnd);
extern void on_paint_background_auto(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, annunciator_t* wnd);

extern result_t annunciator_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata);
/**
 * @brief Create a annunciator window
 * @param wnd
 * @param hndl
 * @return
*/
extern result_t create_annunciator_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, annunciator_t* wnd, handle_t* hndl);

#endif
