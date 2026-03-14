#include "../../include/marquee_widget.h"
#include "../../include/menu_window.h"

void on_paint_marquee(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  marquee_widget_t* wnd = (marquee_widget_t*)wnddata;

  on_paint_widget_background(canvas, wnd_rect, msg, wnddata);
  }

static int32_t get_annunciator_index(marquee_widget_t* wnd, annunciator_t* ann)
  {
  for (int32_t i = 0; i < wnd->num_annunciators; i++)
    {
    if (wnd->annunciators[i] == ann)
      return i;
    }

  return -1;
  }

static void select_annunciator(handle_t hwnd, marquee_widget_t* wnd, int32_t index)
  {
  if (index < 0 || index >= wnd->num_annunciators)
    return;

  handle_t hann;
  if (succeeded(get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hann)))
    hide_window(hann);

  wnd->selected_index = index;
  if (wnd->selected_alarm < 0 &&
    succeeded(get_window_by_id(hwnd, index + wnd->base_widget_id, &hann)))
    {
    show_window(hann);

    invalidate(hann);
    }
  }

result_t marquee_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t can_id = get_can_id(msg);
  marquee_widget_t* wnd = (marquee_widget_t*)wnddata;
  annunciator_t* ann = wnd->annunciators[wnd->selected_index];
  bool changed = false;
  handle_t hann = 0;
  handle_t hchild;


  if (wnd != 0)
    {
    if (can_id == id_marquee_next)
      {
      int32_t next_index = wnd->selected_index + 1;
      if (next_index >= wnd->num_annunciators)
        next_index = 0;

      select_annunciator(hwnd, wnd, next_index);

      changed = true;
      }
    else if (can_id == id_marquee_prev)
      {
      int32_t next_index = wnd->selected_index - 1;
      if (next_index < 0)
        next_index = wnd->num_annunciators - 1;

      select_annunciator(hwnd, wnd, next_index);

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
              bool alarm_state;
              get_param_bool(msg, &alarm_state);

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
                // lower numbers are greater priority
                if (wnd->selected_alarm < 0 || wnd->alarms[wnd->selected_alarm].priority > alarm->priority)
                  {
                  if (wnd->selected_alarm >= 0)
                    {
                    get_window_by_id(hwnd, wnd->selected_alarm + wnd->base_alarm_id, &hchild);
                    // hide the currently selected alarm
                    hide_window(hchild);
                    }
                  wnd->selected_alarm = i;
                  }
                }
              }
            }
          }

        if (found_alarm)
          {
          if (succeeded(get_window_by_id(hwnd, wnd->base_alarm_id + wnd->selected_alarm, &hchild)))
            {
            show_window(hchild);
            use_alarm_keys(true);     // use the correct keyset

            // hide the annunciator
            if (succeeded(get_window_by_id(hwnd, wnd->base_widget_id + wnd->selected_index, &hchild)))
              hide_window(hchild);

            // if an annunciator is assigned to the alarm, then show that
            // when the alarm is cancelled
            alarm_t* the_alarm = wnd->alarms + wnd->selected_alarm;

            if (the_alarm->annunciator != nullptr)
              wnd->selected_index = get_annunciator_index(wnd, the_alarm->annunciator);

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
          if (ann->base.on_message(hwnd, msg, ann) &&
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
    invalidate(hwnd);

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

  int32_t current_alarm = marquee->selected_alarm;
  marquee->selected_alarm = -1;

  bool found_alarm = false;
  // check each of the alarms, and if needed raise the alarm
  for (int32_t i = 0; !found_alarm && i < marquee->num_alarms; i++)
    {
    if (i == current_alarm)
      continue;

    // check this alarm to see if it should be shown
    alarm_t* alarm = marquee->alarms + i;
    if (alarm->is_alarm &&
      !alarm->is_seen &&
      !alarm->is_parked)
      {
      found_alarm = true;
      // lower numbers are greater priority
      if (marquee->selected_alarm < 0 || marquee->alarms[marquee->selected_alarm].priority > alarm->priority)
        marquee->selected_alarm = i;
      }
    }

  handle_t hchild = 0;
  // Hide the existing alarm
  if (succeeded(get_window_by_id(parent, marquee->base_alarm_id + current_alarm, &hchild)))
    hide_window(hchild);

  if (found_alarm)
    {
    if (succeeded(get_window_by_id(parent, marquee->base_alarm_id + marquee->selected_alarm, &hchild)))
      show_window(hchild);
    }
  else
    {
    // show the selected annunciator
    if (succeeded(get_window_by_id(parent, marquee->base_widget_id + marquee->selected_index, &hchild)))
      show_window(hchild);
    }

  invalidate(parent);

  // make sure the menu system is re-enabled.
  use_alarm_keys(found_alarm);
  }

static void dismiss_alarm(handle_t hwnd, alarm_t* alarm)
  {
  result_t result;

  hide_window(hwnd);
  handle_t parent;
  if (failed(result = window_parent(hwnd, &parent)))
    return;

  marquee_widget_t* marquee;
  if (failed(result = get_wnddata(parent, (void**)&marquee)))
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

result_t create_marquee_widget(handle_t parent, uint32_t flags, marquee_widget_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, marquee_wndproc, &wnd->base, &hndl)))
    return result;

  show_window(hndl);

  if (out != 0)
    *out = hndl;

  return s_ok;
  }

result_t show_marquee_child(handle_t hwnd, uint32_t child)
  {
  result_t result;
  handle_t hchild;

  marquee_widget_t* wnd;
  if (failed(result = get_wnddata(hwnd, (void**)&wnd)))
    return result;

  // hide the selected child
  if (failed(result = get_window_by_id(hwnd, wnd->selected_index + wnd->base_widget_id, &hchild)) ||
    failed(hide_window(hchild)))
    return result;


  if (failed(result = get_window_by_id(hwnd, child + wnd->base_widget_id, &hchild)))
    return result;

  return show_window(hchild);
  }

void on_paint_alarm_foreground(handle_t canvas, const rect_t* wnd_rect, const canmsg_t* msg, void* wnddata)
  {
  alarm_t* alarm = (alarm_t*)wnddata;

  // erase the old text
  rectangle(canvas, wnd_rect, alarm->base.background_color, alarm->base.background_color, wnd_rect);

  point_t origin = { rect_width(wnd_rect) >> 1, rect_height(wnd_rect) >> 1 };

  extent_t ex;
  text_extent(alarm->base.name_font, 0, alarm->message, &ex);

  origin.x -= ex.dx >> 1;
  origin.y -= ex.dy >> 1;

  draw_text(canvas, wnd_rect, alarm->base.name_font, alarm->base.name_color, alarm->base.background_color,
    0, alarm->message, &origin, wnd_rect, 0, 0);
  }

static bool is_response_for_this_alarm(handle_t hwnd, alarm_t *alarm)
  {
  // the keyboard message is sent to all alarms, so we need to check if this
  // is the current alarm
  handle_t parent;
  if (failed(window_parent(hwnd, &parent)))
    return false;
  marquee_widget_t* marquee;
  if (failed(get_wnddata(parent, (void**)&marquee)))
    return false;

  // see if the selected alarm is current
  alarm_t* current_alarm = marquee->alarms + marquee->selected_alarm;
  return current_alarm == alarm;
  }

result_t alarm_wndproc(handle_t hwnd, const canmsg_t* msg, void* wnddata)
  {
  uint16_t can_id = get_can_id(msg);
  alarm_t* wnd = (alarm_t*)wnddata;

  bool changed = false;
  switch (can_id)
    {
    case id_paint:
      on_paint_widget(hwnd, msg, wnddata);
      break;

    case id_alarm_close:
      if (is_response_for_this_alarm(hwnd, wnd))
        {
        dismiss_alarm(hwnd, wnd);
        return s_ok;
        }
      break;
    case id_alarm_park:
      if (is_response_for_this_alarm(hwnd, wnd))
        {
        park_alarm(hwnd, wnd);
        return s_ok;
        }
      break;
    }

  return s_false;
  }

result_t create_alarm_annunciator(handle_t parent, uint32_t flags, alarm_t* wnd, handle_t* out)
  {
  result_t result;
  handle_t hndl;
  if (failed(result = create_widget(parent, flags, alarm_wndproc, &wnd->base, &hndl)))
    return result;

  hide_window(hndl);

  if (out != 0)
    *out = hndl;

  return s_ok;
  }