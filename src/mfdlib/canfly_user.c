/*
 * canfly_user.c - CanFly OS User-Mode Library
 *
 * Auto-generated from syscalls.xml v1.0
 * Generated: 2026-03-03 12:12:25
 *
 * These functions execute entirely in user space. They use syscalls
 * (get_wndproc, begin_paint, end_paint, etc.) to interact with the
 * kernel but never require kernel callbacks into user space.
 *
 * Copyright (c) Kotuku Aerospace Ltd. All rights reserved.
 */

#include "../../include/canfly.h"
#include "../../include/sys_canfly.h"

#include <stdarg.h>
#include <stdio.h>

result_t dispatch_message(handle_t hwnd, const canmsg_t *msg)
  {
  wndproc_fn wndproc;
  void *wnddata = nullptr;
  result_t result;
  uint16_t last_msg_id = get_can_id(msg);

  // handle the input messages that are routed to the focused window
  if (last_msg_id == id_up ||
      last_msg_id == id_down ||
      last_msg_id == id_ok ||
      last_msg_id == id_cancel)
  {
    handle_t screen;
    if (failed(result = get_screen(&screen)))
      return result;

    handle_t focused_window;
    if (failed(get_focused_window(&focused_window)))
      return result;

    if (focused_window == nullptr)
    {
      // find the first window that can handle the message
      if (failed(result = get_first_child(screen, &focused_window)))
        return result;    // ignore the message

      if (failed(is_enabled(focused_window)))
      {
        if (failed(get_next_enabled_window(focused_window, &focused_window)))
          return s_ok;
      }

      set_focused_window(focused_window);
    }

    hwnd = focused_window;
    }

  // if this is sent to the screen (hwnd == 0) then it
  // is just handled inside the defwndproc which will send to all of
  // its children until it is handled (id_paint for example)
  if (hwnd == nullptr)
    {
    if (failed(result = get_screen(&hwnd)))
      return result;
    }

  // Retrieve the window procedure from the kernel
  result = get_wndproc(hwnd, &wndproc, &wnddata);
  if(failed(result))
    return result;

  if(wndproc == 0)
    return defwndproc(hwnd, msg, wnddata);

  // Call the window procedure in user space
  return wndproc(hwnd, msg, wnddata);
  }

result_t send_message(handle_t hwnd, const canmsg_t *msg)
  {
  if(hwnd == 0)
    {
    // Broadcast: walk all children of the screen
    handle_t screen;
    result_t result = get_screen(&screen);
    if(failed(result))
      return result;

    handle_t child;
    result = get_first_child(screen, &child);
    while(succeeded(result) && child != 0)
      {
      dispatch_message(child, msg);
      result = get_next_sibling(child, &child);
      }
    return s_ok;
    }

  return dispatch_message(hwnd, msg);
  }

result_t defwndproc(handle_t wnd, const canmsg_t* msg, void* wnddata)
  {
  wndproc_fn wndproc;

  handle_t hchild;
  uint16_t id = get_can_id(msg);

  switch (id)
    {
    case id_paint :
      {
      // we assume the widget has painted its canvas, we work over our children
      // in z-order
      uint16_t painting_order = 0;
      uint16_t next_z_order = 0;
      uint16_t max_order = 0;

      if (failed(get_first_child(wnd, &hchild)) || hchild == 0)
        return s_ok;

      begin_paint(wnd, 0);    // start painting, set ctr == 1 on first paint

      // paint in lowest order to highest, note if there is
      // no window with painting order of 0 then the first
      // pass does nothing other than finding the first child
      // to paint
      while (true)
        {
        handle_t painting_wnd = hchild;
        do
          {
          uint8_t z_order;
          get_z_order(painting_wnd, &z_order);

          // if the next order is more than the current one, then
          // cache it, but if the next_z_order is greater then
          // reduce it to the current one
          if (z_order > painting_order)
            {
            // get the next highest order
            if (z_order < next_z_order || next_z_order == painting_order)
              next_z_order = z_order;

            // figure out what the last one to do is.
            if (z_order > max_order)
              max_order = z_order;
            }

          if (z_order == painting_order &&
            succeeded(is_visible(painting_wnd)))
            {
            get_wndproc(painting_wnd, &wndproc, &wnddata);

            // call the window
            (*wndproc)(painting_wnd, msg, wnddata);
            }


          get_next_sibling(painting_wnd, &painting_wnd);
          } while (painting_wnd != 0);

        // if the painting z_order is the maximum then the loop is done
        if (painting_order >= max_order)
          break;

        painting_order = next_z_order;        // lowest paint z-order
        }

      end_paint(wnd);
      }
      break;
    case id_setfocus :
    case id_loosefocus :
    case id_ok :
    case id_cancel :
      break;
    default:

      // send the message to all children of this window.
      if (failed(get_first_child(wnd, &hchild)) || hchild == 0)
        return s_ok;

      do
        {
        if (succeeded(get_wndproc(hchild, &wndproc, &wnddata)))
          (*wndproc)(hchild, msg, wnddata);

        get_next_sibling(hchild, &hchild);
        } while (hchild != 0);
      break;
    }

  return s_ok;
  }

result_t is_dialog_message(handle_t hwnd, const canmsg_t *msg)
  {
  uint16_t id = get_can_id(msg);

  if(id == id_next)
    {
    handle_t next;
    if(succeeded(get_next_tabstop_window(hwnd, &next)))
      {
      set_focused_window(next);
      return s_ok;
      }
    }
  else if(id == id_prev)
    {
    handle_t prev;
    if(succeeded(get_previous_tabstop_window(hwnd, &prev)))
      {
      set_focused_window(prev);
      return s_ok;
      }
    }

  return s_false;
  }

#ifdef _DEBUG
static char debug_msg[256];
result_t platform_trace(uint16_t level, const char *msg, va_list va)
  {
  vsnprintf(debug_msg, sizeof(debug_msg), msg, va);

  return trace_message(level, debug_msg);
  }
#endif