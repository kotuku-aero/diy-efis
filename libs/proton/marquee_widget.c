#include "marquee_widget.h"
#include "menu_window.h"

///////////////////////////////////////////////////////////////////////////////

result_t marquee_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t can_id = get_can_id(msg);
  marquee_widget_t* wnd = (marquee_widget_t*)wnddata;
  annunciator_t* ann = wnd->annunciators[wnd->selected_index];
  bool changed = false;
  handle_t hann = 0;

  if (wnd != 0)
    {
    if (can_id == id_marquee_next)
      {
      if (succeeded(get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hann)))
        hide_window(hann);

      wnd->selected_index++;
      if (wnd->selected_index >= wnd->num_annunciators)
        wnd->selected_index = 0;

      if (wnd->selected_alarm < 0 &&
          succeeded(get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hann)))
        show_window(hann);

      changed = true;
      }
    else if (can_id == id_marquee_prev)
      {

      if (succeeded(get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hann)))
        hide_window(hann);

      wnd->selected_index--;
      if (wnd->selected_index < 0)
        wnd->selected_index = wnd->num_annunciators - 1;

      if (wnd->selected_alarm < 0 &&
        succeeded(get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hann)))
        show_window(hann);

      changed = true;
      }
    else
      {
      if (can_id < 256)      // CANID's 0-255 are the alarms
        {
        bool found_alarm = false;
        // check each of the alarms, and if needed raise the alarm
        for (int32_t i = 0; !found_alarm && i < wnd->num_alarms; i++)
          {
          alarm_t* alarm = wnd->alarms + i;
          for (size_t a = 0; !found_alarm && a < alarm->num_ids; a++)
            {
            if (alarm->can_ids[a] == can_id)
              {
              // Alarms are a state machine
              //
              // Initial state = !is_alarm & !is_seen & !is_parked
              // 
              // alarm = 1 and !is_alarm       alarm raised
              //
              // found_alarm = true | is_alarm = true  .. this shows al alarm
              //
              // Button 1 pressed  is_alarm = true | seen_alarm = true   .. hide alarm
              // Button 2 pressed  is_alarm = true, is_parked = true, found_alarm = false  .. hide alarm
              // 
              // alarm = 0        alarm cleared   is_alarm = false
              //
              // is_alarm = false
              // if !is_parked then is_alarm = false
              uint16_t alarm_state;
              get_param_uint16(msg, &alarm_state);

              if (alarm_state == 1)
                {
                if (!alarm->is_alarm && !alarm->is_parked)
                  {
                  found_alarm = true;
                  alarm->is_alarm = true;
                  alarm->is_seen = false;
                 }
                }
              else
                {
                alarm->is_alarm = false;
                alarm->is_seen = false;
                }

              if (found_alarm)
                {
                // higher numbers are greater priority
                if (wnd->selected_alarm < 0 || wnd->alarms[wnd->selected_alarm].priority < alarm->priority)
                  wnd->selected_alarm = i;
                }
              }
            }
          }

        if (found_alarm)
          {
          handle_t hchild;
          if(succeeded(get_window_by_id(hwnd, wnd->base_alarm_id + wnd->selected_alarm, &hchild)))
            {
            show_window(hchild);
            use_alarm_keys(true);     // use the correct keyset

            if(succeeded(get_window_by_id(hwnd, wnd->base_widget_id + wnd->selected_index, &hchild)))
              hide_window(hchild);

            // change the keys to the alarm keys

            changed = true;
            }
          }
        }
      else
        {
        // iterate over all of the annunciators and see if the message is
        // for them
        for (int16_t i = 0; i < wnd->num_annunciators; i++)
          {
          ann = wnd->annunciators[i];

          // call the message handler
          if (ann->base.on_message(hwnd, can_id, msg, ann) &&
            i == wnd->selected_index)
            {
            get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hann);
            changed = true;             // only invalidate if the selected annunciator has changed
            }
          }
        }
      }
    }

  if (changed)
    {
    invalidate_background_rect(hwnd, 0);
    invalidate_foreground_rect(hwnd, 0);
    //invalidate_background_rect(hann, 0);
    }

  return widget_wndproc(hwnd, msg, wnddata);
  }

static void close_alarm(handle_t parent, handle_t hwnd, marquee_widget_t* marquee)
  {
  if (marquee->selected_alarm < 0)
    return;

  alarm_t* selected_alarm = marquee->alarms + marquee->selected_alarm;
  selected_alarm->is_seen = true;
#ifdef _DEBUG
// TODO: Should be cleared by device
  selected_alarm->is_alarm = false;
#endif

  bool found_alarm = false;
  // check each of the alarms, and if needed raise the alarm
  for (int32_t i = 0; !found_alarm && i < marquee->num_alarms; i++)
    {
    if (i == marquee->selected_alarm)
      continue;

    // check this alarm to see if it should be shown
    alarm_t* alarm = marquee->alarms + i;
    for (size_t a = 0; !found_alarm && a < alarm->num_ids; a++)
      {
      if (alarm->is_alarm &&
        !alarm->is_seen &&
        !alarm->is_parked)
        {
        found_alarm = true;
        // higher numbers are greater priority
        if (marquee->selected_alarm < 0 || marquee->alarms[marquee->selected_alarm].priority < alarm->priority)
          marquee->selected_alarm = i;
        }
      }
    }

  handle_t hchild = 0;
  if (found_alarm)
    {
    if (succeeded(get_window_by_id(parent, marquee->base_alarm_id + marquee->selected_alarm, &hchild)))
      show_window(hchild);
    }
  else
    {
    marquee->selected_alarm = -1;
    if (succeeded(get_window_by_id(parent, marquee->base_widget_id + marquee->selected_index, &hchild)))
      show_window(hchild);
    }

  invalidate_background_rect(hchild, 0);
  invalidate_foreground_rect(hchild, 0);

  // make sure the menu system is re-enabled.
  use_alarm_keys(false);
  }

static void dismiss_alarm(handle_t hwnd, alarm_t *alarm)
  {
  result_t result;

  hide_window(hwnd);
  handle_t parent;
  if(failed(result = window_parent(hwnd, &parent)))
    return;

  marquee_widget_t *marquee;
  if(failed(result = get_wnddata(parent, (void **)&marquee)))
    return;

  close_alarm(parent, hwnd, marquee);
  }

static void park_alarm(handle_t hwnd, alarm_t* alarm)
  {
  result_t result;
  hide_window(hwnd);

  handle_t parent;
  if (failed(result = window_parent(hwnd, &parent)))
    return;

  marquee_widget_t* marquee;
  if (failed(result = get_wnddata(parent, (void**)&marquee)))
    return;

  if (marquee->selected_alarm < 0)
    return;

  alarm_t* selected_alarm = marquee->alarms + marquee->selected_alarm;

  if (selected_alarm->can_park)
    selected_alarm->is_parked = true;

  close_alarm(parent, hwnd, marquee);
  }

result_t create_marquee_widget(handle_t parent, uint16_t id, aircraft_t* aircraft, marquee_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, marquee_wndproc, &wnd->base, &hndl)))
    return result;

  if (out != 0)
    *out = hndl;

  return s_ok;
  }

result_t show_marquee_child(handle_t hwnd, uint32_t child)
  {
  result_t result;
  handle_t hchild;

  marquee_widget_t* wnd;
  if(failed(result = get_wnddata(hwnd, (void **)&wnd)))
    return result;

  // hide the selected child
  if(failed(result = get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hchild)) ||
    failed(hide_window(hchild)))
    return result;


  if(failed(result = get_window_by_id(hwnd, child + wnd->base_widget_id, &hchild)))
    return result;

  return show_window(hchild);
  }

void on_paint_alarm_foreground(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  alarm_t* alarm = (alarm_t*)wnddata;

  // erase the old text
  rectangle(canvas, wnd_rect, color_hollow, color_hollow, wnd_rect);

  point_t origin = { rect_width(wnd_rect) >> 1, rect_height(wnd_rect) >> 1};

  extent_t ex;
  text_extent(alarm->base.name_font, 0, alarm->message, &ex);

  origin.x -= ex.dx >> 1;
  origin.y -= ex.dy >> 1;

  draw_text(canvas, wnd_rect, alarm->base.name_font, alarm->base.name_color, color_hollow,
    0, alarm->message, &origin, wnd_rect, 0, 0);
  }

result_t alarm_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t can_id = get_can_id(msg);
  alarm_t* wnd = (alarm_t*)wnddata;

  bool changed = false;
  switch (can_id)
    {
    case id_paint_background :
    case id_paint_foreground :
      on_paint_widget(hwnd, msg, wnddata);
      break;

    case id_alarm_close :
      dismiss_alarm(hwnd, wnd);
      break;
    case id_alarm_park :
      park_alarm(hwnd, wnd);
      break;
    }

  if (changed)
    invalidate_foreground_rect(hwnd, 0);

  return s_ok;
  }

result_t create_alarm_annunciator(handle_t parent, uint16_t id, alarm_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, id, alarm_wndproc, &wnd->base, &hndl)))
    return result;

  hide_window(hndl);

  if (out != 0)
    *out = hndl;

  return s_ok;
  }