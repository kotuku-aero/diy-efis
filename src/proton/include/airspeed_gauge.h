#ifndef __airspeed_gauge_h__
#define __airspeed_gauge_h__


#include "gauge_widget.h"

/**
 * @brief decorare the airspeed gauge with a background that includes the speed ranges
 * @param canvas 
 * @param wnd_rect 
 * @param msg 
 * @param wnddata 
 */
extern void on_paint_airspeed_gauge_background(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

#endif
