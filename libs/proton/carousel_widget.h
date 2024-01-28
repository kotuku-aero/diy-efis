#ifndef __carousel_window_h__
#define __carousel_window_h__

#include "proton.h"

// this file describes a container window that is intended
// to be a root window for the system.  It contains a series
// of pages that can be cycled through.  As each page becomes
// visible messages are routed to that page.
// drawing is only done on the visible page.

typedef result_t (*create_page_fn)(handle_t parent, const rect_t* rect, uint16_t id, void* wnd, handle_t* hndl);

typedef struct _carousel_page_t {
  create_page_fn create_page;   // deferred page create function
  void *wnddata;                // window data to paste to create function
  wndproc_fn wndproc;           // window process to pass messages to
  handle_t hndl;                // when created this is the child window
  } carousel_page_t;

typedef struct _carousel_window_t {
  widget_t base;

  handle_t parent;
  handle_t hwnd;
  uint16_t num_pages;
  carousel_page_t* child_windows;
  // page to display initially 0..num_pages
  uint16_t selected_page;
  } carousel_window_t;


/**
 * @brief Create a carousel 
 * @param parent    The parent root window
 * @param rect      Parent relative rectangle to create in
 * @param id        id of this window
 * @param wnd       prototype windows
 * @param hndl      Handle to new window
 * @return s_ok of created ok
*/
extern result_t create_carousel_window(handle_t parent, uint16_t id, aircraft_t* aircraft, carousel_window_t*wnd, handle_t* hndl);

#define id_carousel_left (id_buttonpress +1)
#define id_carousel_right (id_carousel_left +1)

#endif

