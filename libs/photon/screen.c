#include "photon_priv.h"

#define APP_QUEUE_SIZE 128
#define PHOTON_QUEUE_TIMEOUT 100

const typeid_t screen_type =
  {
  .name = "screen",
  .base = &window_type
  };

typedef struct _queued_msg_t {
  handle_t wnd;
  canmsg_t msg;
  } queued_msg_t;

static const photon_parameters_t *params;
static handle_t screen_hook;

screen_t *_screen;

#define PA_TO_KVA1(pa)	((void *) ((pa) | 0xa0000000))

result_t get_screen(handle_t *hndl)
  {
  if (hndl == 0)
    return e_bad_parameter;

  result_t result;

  if (_screen != 0)
    {
    *hndl = (handle_t)_screen;
    return s_ok;
    }

  enter_critical();
  if (_screen != 0)
    {
    exit_critical();
    *hndl = (handle_t)_screen;
    return s_ok;
    }

  exit_critical();
  
  screen_surface_t *surface;

  if (failed(result = neutron_malloc(sizeof(screen_t), (void **)&_screen)))
    return result;

  memset(_screen, 0, sizeof(screen_t));

  _screen->base.base.type = &screen_type;
  _screen->base.position.right = params->gdi_extents.dx;
  _screen->base.position.bottom = params->gdi_extents.dy;
  
  if(failed(result = bsp_open_framebuffer(&surface)) ||
    failed(result = create_canvas_from_framebuffer(&surface->base.base, (handle_t *) &_screen->base.canvas)))
    return result;
  
  rect_copy(&surface->base.base.position, &_screen->base.position);
  _screen->base.wndproc = 0;      // no wndproc for the _screen as it is a queue only
  _screen->base.visible = true;

  _screen->queue_empty = window_queue_empty;

  if (failed(result = deque_create(sizeof(queued_msg_t), APP_QUEUE_SIZE, &_screen->event_queue)))
    {
    neutron_free(_screen);
    return result;
    }

  *hndl = (handle_t) _screen;

  return result;
  }

static bool screen_hook_fn(const canmsg_t *canmsg, void *parg)
  {
  queued_msg_t msg;
  msg.wnd = 0;
  memcpy(&msg.msg, canmsg, sizeof(canmsg_t));

  // send the bus message to all layers that are active
  if(_screen != 0)
    push_back(_screen->event_queue, (void *)&msg, PHOTON_QUEUE_TIMEOUT);

  return true;
  }

result_t initialize_screen(const photon_parameters_t *_params)
  {
  params = _params;

  return subscribe(screen_hook_fn , 0, &screen_hook);
  }


result_t post_message(handle_t hndl, const canmsg_t *canmsg, uint32_t delay)
  {
  if(_screen == 0)
    return e_unexpected;

  queued_msg_t msg;
  msg.wnd = hndl;
  memcpy(&msg.msg, canmsg, sizeof(canmsg_t));

  return push_back(_screen->event_queue, &msg, delay);
  }

#ifndef _WIN32
#include "../atom/microkernel.h"
result_t post_message_from_isr(const canmsg_t *canmsg)
  {
  queued_msg_t msg;
  msg.wnd = 0;
  memcpy(&msg.msg, canmsg, sizeof(canmsg_t));
  
  return push_back_from_isr(_screen->event_queue, &msg);
  }
#endif

extern const canmsg_t paint_msg;

result_t get_message(handle_t *hndl, canmsg_t *canmsg)
  {
  if (hndl == 0 || canmsg == 0)
    return e_bad_pointer;

  result_t result = s_false;
  queued_msg_t msg;
  screen_surface_t *surface = (screen_surface_t*) _screen->base.canvas->fb->surface;

  while (result != s_ok)
    {
    uint32_t delay = surface->base.invalid_count > 0 ? 0 : INDEFINITE_WAIT;
    switch (result = pop_front(_screen->event_queue, &msg, delay))
      {
      case e_timeout:
         if(surface->base.invalid_count > 0 &&
           surface->state == fbds_idle)      // only paint when screen is done.
          {
          (*_screen->queue_empty)(surface);
          *hndl = 0;
          memcpy(canmsg, &paint_msg, sizeof(canmsg_t));
          return s_ok;
          }
        return s_false;
      case s_ok:
        break;
      default:
        return result;
      }

    // the above is a 0 timeout so when the queue is empty
  // a paint message can be processed
    uint16_t can_id = get_can_id(&msg.msg);

    if (can_id == id_paint_background)
      {
      _screen->needs_paint = true;
      result = s_false;
      continue;
      }
    else if (can_id == id_paint_foreground)
      {
      _screen->needs_paint = true;
      result = s_false;
      continue;
      }
    else if (can_id == id_paint_overlay)
      {
      _screen->needs_paint = true;
      result = s_false;
      continue;
      }

    *hndl = msg.wnd;
    memcpy(canmsg, &msg.msg, sizeof(canmsg_t));
    }

  return s_ok;
  }
