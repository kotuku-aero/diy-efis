#ifndef __marquee_window_h__
#define __marquee_window_h__

#include "proton.h"
#include "annunciator_widget.h"


typedef struct _alarm_t {
  widget_t base;
  const uint16_t* can_ids;  // the id's that can raise the alarm of the alarm
  size_t num_ids;           // number of can-id's
  int32_t priority;         // display priority
  const char* message;      // message

  bool is_alarm;            // set true if an alarm has been raised
  bool can_park;            // the alarm can be parked
  bool is_parked;              // set when the alarm is cleared, and the user parks it (button 2)
  bool is_seen;             // set when the user acknowledges the alarm (button 1)

  } alarm_t;

typedef struct _marquee_widget_t {
  widget_t base;

  // this is the base id used for each annunciator
  uint16_t base_widget_id;
  uint16_t base_alarm_id;

  // this is the number in the array of annunciators
  int32_t selected_index;           // if set -1 then an alarm is raised and displayed
  annunciator_t **annunciators;
  size_t num_annunciators;
  
  int32_t selected_alarm;           // if >= 0 then the alarm is displayed
  // This is displayed when an ararm is raised.
  alarm_t *alarms;
  size_t num_alarms;

  } marquee_widget_t;

/**
 * @brief 
 * @param parent 
 * @param rect 
 * @param id 
 * @param wnd 
 * @param hndl 
 * @return 
*/
extern result_t create_marquee_widget(handle_t parent, uint16_t id, aircraft_t *aircraft, marquee_widget_t* wnd, handle_t* hndl);
/**
 * @brief Switch the marquee into alarm display mode, or exit the alarm display
 * @param wnd Marquee window
 * @param is_alarm true if there are active alarms
*/
extern void raise_alarm(marquee_widget_t*wnd, bool is_alarm);
/**
 * @brief Show the child widget based on the index
 * @param widget 
 * @param child 
 * @return 
*/
extern result_t show_marquee_child(handle_t widget, uint32_t child);
/**
 * @brief Create an alarm annunciator
 * @param parent Marquee window
 * @param id     child ID
 * @param wnd    details of the alarms
 * @param out 
 * @return 
*/
extern result_t create_alarm_annunciator(handle_t parent, uint16_t id, alarm_t* wnd, handle_t* out);

extern void on_paint_alarm_foreground(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata);

#endif // !__marquee_window_h__
