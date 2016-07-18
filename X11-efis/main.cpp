/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "../widgets/watchdog.h"
#include "../widgets/pfd_application.h"
#include "../widgets/layout_window.h"
#include "../linux-hal/linux_hal.h"
#include "../widgets/layout_window.h"
#include "x11_screen.h"

static kotuku::layout_window_t *root_window;
static kotuku::watchdog_t *watchdog;
static kotuku::pfd_application_t *_the_app = 0;
static kotuku::linux_hal_t *hal_impl = 0;

namespace kotuku
  {
  class x11_root_screen_t: public x11_screen_t
    {
  public:
    x11_root_screen_t(Display *display, Drawable d, size_t x, size_t y, size_t bpp);
    ~x11_root_screen_t();

    layout_window_t *root_window() const
      {
      return _root_window;
      }
    void root_window(window_t *window)
      {
      _root_window = reinterpret_cast<layout_window_t *>(window);
      }
  private:
    layout_window_t *_root_window;
    };
  }

// this class holds the handles
inline kotuku::x11_root_screen_t *as_screen_handle(kotuku::screen_t *h)
  {
  return reinterpret_cast<kotuku::x11_root_screen_t *>(h);
  }

/* handler for XGetImage BadMatch errors */
static int x11_error_handler(Display *dpy, XErrorEvent *event)
  {
  return (event->request_code == 73 /*X_GetImage*/
  && event->error_code == BadMatch);
  }

namespace kotuku
  {
  result_t do_create_root_screen(hal_t *hal, screen_type_t screen_type, window_t **root_window)
    {
    int screen_x;
    int screen_y;

    if(failed(hal->get_config_value("linux-hal", "screen-x", screen_x)))
      screen_x = 640;

    if(failed(hal->get_config_value("linux-hal", "screen-y", screen_y)))
      screen_y = 480;

    char *host;
    if((host = (char *) getenv("DISPLAY")) == NULL)
    return 0;

    Display *display;
    if((display = XOpenDisplay(host)) == NULL)
    return 0;

    int screen_id = DefaultScreen(display);
    Colormap colormap = DefaultColormap(display, screen_id);
    Window root = RootWindow(display, screen_id);
    int depth = DefaultDepth(display, screen_id);

    XSelectInput(display, root, SubstructureNotifyMask);

    XSetWindowAttributes attr;
    attr.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    attr.background_pixel = BlackPixel(display, screen_id);

    Window window = XCreateWindow(display, root, 0, 0, screen_x, screen_y, 0,
        depth, InputOutput, DefaultVisual(display, screen_id),
        CWEventMask | CWBackPixel, &attr);

    char name[80];
    sprintf(name, "diy-efis: %d * %d * %d bpp", screen_x, screen_y, depth);

    XChangeProperty(display, window,
        XA_WM_NAME, XA_STRING, 8,
        PropModeReplace, reinterpret_cast<unsigned char *>(name), strlen(name));

    XMapWindow(display, window);

    Pixmap iconPixmap;
    XWMHints xwmhints;

    xwmhints.icon_pixmap = iconPixmap;
    xwmhints.initial_state = NormalState;
    xwmhints.flags = IconPixmapHint | StateHint;

    XSetWMHints(display, window, &xwmhints);
    XClearWindow(display, window);
    XSync(display, 0);

    XSetErrorHandler(x11_error_handler);

    x11_root_screen_t *x11root_screen = new kotuku::x11_root_screen_t(display, window, screen_x, screen_y, depth);
    *screen = x11root_screen;

    *root_window = new kotuku::layout_window_t(*screen);
    x11root_screen->root_window(*root_window);
    }
  }

kotuku::application_t *kotuku::the_app()
  {
  return _the_app;
  }

kotuku::hal_t *kotuku::the_hal()
  {
  if(hal_impl == 0)
    hal_impl = new kotuku::linux_hal_t(kotuku::do_create_root_screen);

  return hal_impl;
  }

int main(int argc, char **argv)
  {
  kotuku::the_hal()->initialize("efis.ini");
  _the_app = new kotuku::pfd_application_t(kotuku::the_hal()->root_window());

  watchdog = new kotuku::watchdog_t(root_window, _the_app);

  // start the application
  kotuku::application_t *app = kotuku::the_app();
  kotuku::the_hal()->root_window()->repaint(true);
  XEvent e;
  while(1)
    {
    if(kotuku::the_hal()->screen() != 0)
      {
      XNextEvent(reinterpret_cast<kotuku::x11_screen_t *>(kotuku::the_hal()->screen())->display(), &e);

      switch (e.type)
        {
        case VisibilityNotify :
        case Expose:
          reinterpret_cast<kotuku::layout_window_t *>(kotuku::the_hal()->root_window())->repaint(true);
          XSync(reinterpret_cast<kotuku::x11_screen_t *>(kotuku::the_hal()->screen())->display(), False);
          XFlush(reinterpret_cast<kotuku::x11_screen_t *>(kotuku::the_hal()->screen())->display());
          break;
        }
      // Handle Windows Close Event
//       if(e.type==ClientMessage)
//         break;
      }
    }

  return 0;
  }

kotuku::x11_root_screen_t::x11_root_screen_t(Display *display, Drawable d, size_t x, size_t y,
    size_t bpp) :
x11_screen_t(display, d, rect_t(0, 0, x, y), bpp)
  {
  }

kotuku::x11_root_screen_t::~x11_root_screen_t()
  {
  }

