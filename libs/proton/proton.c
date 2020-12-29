#include "../neutron/bsp.h"
#include "proton.h"

// this is the screen and to which the interpreter runs
handle_t main_window = 0;

extern const uint8_t neo[];
extern const uint16_t neo_length;

// if parg is given then is a handle to a stream which will be closed

void run_proton(proton_args_t *args)
  {
  result_t result;
  memid_t key;


  // try to start the screen.  This is hardware dependent and will be
  // implemented in the hardware abstraction
  if (succeeded(result = open_screen(args->orientation, defwndproc, 0, &main_window)))
    {
    // see if a splash screen is loaded
    if (args->stream != 0)
      {
      // determine if the destination window is suitable for
      // expanding the PNG.  We must have a color that
      // is 32 bits.  If not we create a canvas that is
      // guaranteed to be 32 bits
      extent_t ex;
      uint16_t bpp;
      if (succeeded(get_canvas_extents(main_window, &ex, &bpp)) &&
          bpp < 32)
        {
        handle_t canvas = 0;
        if (succeeded(create_rect_canvas(&ex, &canvas)) &&
            succeeded(load_png(canvas, args->stream, 0)))
          {
          rect_t wnd_rect = {0, 0, ex.dx, ex.dy};
          point_t pt = {0, 0};

          // copy the rendered bitmap over
          bit_blt(main_window, &wnd_rect, &wnd_rect, canvas, &wnd_rect, &pt);
          }
        canvas_close(canvas);
        }
      else
        load_png(main_window, args->stream, 0);
      stream_close(args->stream);
      }

    // load the neon font.
    register_font(neo, neo_length);

    // attach the ion interpreter to the screen
    if (failed(result = attach_ion(main_window, key, 0,
                                   args->ci, args->co, args->cerr)))
      return;

    // run the message queue
    // despatch messages
    canmsg_t msg;
    handle_t hwnd;
    // returns s_false to stop the queue
    while (true)
      if (succeeded(get_message(main_window, &hwnd, &msg)))
        dispatch_message(hwnd, &msg);
    }
  }

