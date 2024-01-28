
#ifndef __pfd__h__
#define __pfd__h__

#include "../../libs/proton/proton.h"
#include "../../libs/proton/menu_window.h"


#ifdef __cplusplus
extern "C" {
#endif


#define id_units  (id_app_action_id_first + 1)

#define id_cancel_alarm  (id_app_action_id_first + 2)

#define id_snooze_alarm  (id_app_action_id_first + 3)

  extern result_t get_brightness(menu_item_spin_edit_t *edit, int32_t *value);
  
  extern result_t set_brightness(menu_item_spin_edit_t *edit, int32_t value);
  
  extern result_t get_fuel_map(menu_item_checklist_t *checklist, uint16_t *value);

  extern result_t get_selected_units(menu_item_checklist_t *checklist, uint16_t *value);

  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_hours_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_hours_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_auto_msg(handle_t hwnd, uint16_t can_id, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_hours(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_hours(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
    
  extern result_t on_create_hsi_widget(handle_t hwnd, widget_t *widget);
    
  extern result_t on_create_autopilot_widget(handle_t hwnd, widget_t *widget);
    
  extern result_t on_create_navigator_widget(handle_t hwnd, widget_t *widget);
    

    extern result_t create_pfd(handle_t hwnd, aircraft_t *aircraft);

#ifdef __cplusplus
}
#endif

#endif
  