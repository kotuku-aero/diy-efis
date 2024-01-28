#ifndef __comm_window_h__
#define __comm_window_h__

#include "widget.h"
#include "../graviton/aircraft.h"

typedef struct _comm_window_t comm_window_t;
typedef struct _comm_button_t comm_button_t;

typedef struct _comm_button_t {
  rect_t btn_rect;
  rect_t tx_active_rect;
  rect_t rx_active_rect;
  rect_t name_rect;
  rect_t value_rect;
  rect_t area_rect;

  color_t button_color;
  color_t boarder_color;
  } comm_button_t;

typedef struct _comm_window_t {
  widget_t base;

  uint16_t comm_freq;       // comms frequency * 1000
  bool tx_active;           // tx is active
  bool rx_active;
  char comm_freq_area[REG_NAME_MAX];

  uint16_t stby_freq;
  char stby_freq_area[REG_NAME_MAX];

  uint16_t xpdr_code;
  uint16_t stby_xpdr_code;

  bool ident_active;

  extent_t button_ex;       // size of the buttons
  uint16_t gutter;


  // calculated
  comm_button_t comm_btn;
  comm_button_t stby_btn;
  comm_button_t xpdr_btn;
  comm_button_t stby_xpdr_btn;
  comm_button_t ident_btn;
  comm_button_t vfr_btn;
  
  } comm_window_t;

extern result_t create_comm_window(handle_t parent, uint16_t id, aircraft_t* aircraft, comm_window_t* wnd, handle_t* hndl);


#endif
