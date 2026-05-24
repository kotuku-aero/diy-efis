#ifndef __au_nz_priv__h__
#define __au_nz_priv__h__

#include "../../src/proton/include/menu_window.h"


#ifdef __cplusplus
extern "C" {
#endif
extern result_t get_brightness(menu_item_t *edit, variant_t *value);
extern result_t set_brightness(menu_item_t *edit, const variant_t *value);
extern result_t get_fuel_total(menu_item_t *edit, variant_t *value);
extern result_t set_fuel_total(menu_item_t *edit, const variant_t *value);
extern result_t get_fuel_max(menu_item_t *edit, variant_t *value);
extern result_t get_autopilot_mode(menu_item_t *edit, uint32_t *value);
extern result_t get_fuel_max(menu_item_t *item, variant_t *value);
extern result_t on_auto_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern result_t on_hours_msg(handle_t hwnd, const canmsg_t* msg, void* wnddata);
extern void on_paint_auto(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
extern void on_paint_marquee(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
extern void on_paint_hours(handle_t canvas, const rect_t *wnd_rect, const canmsg_t* msg, void* wnddata);
extern result_t on_create_hsi_widget(handle_t hwnd, widget_t *widget);

#ifdef __cplusplus
}
#endif

#endif
