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
#include "../widgets/menu_window.h"
#include "../widgets/notification_window.h"
#include "x11_screen.h"

static kotuku::watchdog_t *watchdog;
static kotuku::pfd_application_t *_the_app = 0;
static kotuku::linux_hal_t *hal_impl = 0;

static Display *display;
static int screen_id;
static Colormap colormap;
static Window root_window;
static Window diy_efis_window;
static int depth;

/* handler for XGetImage BadMatch errors */
static int x11_error_handler(Display *dpy, XErrorEvent *event)
  {
  return (event->request_code == 73 /*X_GetImage*/
  && event->error_code == BadMatch);
  }

namespace kotuku
  {
  x11_screen_t *create_window(hal_t *hal, Window parent, const rect_t &rect)
    {
    XSetWindowAttributes attr;
    attr.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    attr.background_pixel = BlackPixel(display, screen_id);

    Window x_window = XCreateWindow(display, parent, rect.left, rect.top, rect.width(), rect.height(), 0,
        depth, InputOutput, DefaultVisual(display, screen_id),
        CWEventMask | CWBackPixel, &attr);

    if(parent == root_window)
      {
      diy_efis_window = x_window;

      char name[80];
      sprintf(name, "diy-efis: %d * %d * %d bpp", rect.width(), rect.height(), depth);

      XChangeProperty(display, x_window,
          XA_WM_NAME, XA_STRING, 8,
          PropModeReplace, reinterpret_cast<unsigned char *>(name), strlen(name));
      }

    XMapWindow(display, x_window);

    Pixmap iconPixmap;
    XWMHints xwmhints;

    xwmhints.icon_pixmap = iconPixmap;
    xwmhints.initial_state = NormalState;
    xwmhints.flags = IconPixmapHint | StateHint;

    XSetWMHints(display, x_window, &xwmhints);
    XClearWindow(display, x_window);
    XSync(display, 0);

    return new x11_screen_t(display, x_window, rect, depth);
    }

  result_t do_create_root_screen(hal_t *hal, screen_type_t screen_type, window_t **window)
    {
    int window_width;
    int window_height;
    int pos_x;
    int pos_y;

    switch(screen_type)
      {
    case st_root :
      if(failed(hal->get_config_value("linux-hal", "screen-x", window_width)))
        window_width = 480;

      if(failed(hal->get_config_value("linux-hal", "screen-y", window_height)))
        window_height = 640;

      *window = new layout_window_t(create_window(hal, root_window, rect_t(0, 0, window_width, window_height)));
      break;
    case st_menu :
      if(failed(hal->get_config_value("linux-hal", "menu-width", window_width)))
        window_width = 480;

      if(failed(hal->get_config_value("linux-hal", "menu-height", window_height)))
        window_height = 320;

      if(failed(hal->get_config_value("linux-hal", "menu-pos-x", window_height)))
        pos_x = 0;

      if(failed(hal->get_config_value("linux-hal", "menu-pos-y", window_height)))
        pos_y = 320;

      *window = new menu_window_t(create_window(hal, root_window, rect_t(pos_x, pos_y, window_width, window_height)), "root-menu");
      break;
    case st_notifications :
      if(failed(hal->get_config_value("linux-hal", "menu-width", window_width)))
        window_width = 480;

      if(failed(hal->get_config_value("linux-hal", "menu-height", window_height)))
        window_height = 320;

      if(failed(hal->get_config_value("linux-hal", "menu-pos-x", window_height)))
        pos_x = 0;

      if(failed(hal->get_config_value("linux-hal", "menu-pos-y", window_height)))
        pos_y = 320;

      *window = new notification_window_t(create_window(hal, root_window, rect_t(pos_x, pos_y, window_width, window_height)), "alerts");
      break;
      }
  }
  };

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
  XInitThreads();

  char *host;
  if((host = (char *) getenv("DISPLAY")) == NULL)
    return 0;


  if((display = XOpenDisplay(host)) == NULL)
    return 0;

  screen_id = DefaultScreen(display);
  colormap = DefaultColormap(display, screen_id);
  root_window = RootWindow(display, screen_id);
  depth = DefaultDepth(display, screen_id);

  XSelectInput(display, root_window, SubstructureNotifyMask);

  XSetErrorHandler(x11_error_handler);

  const char *ini_path;
  if(argc > 1)
    ini_path = argv[1];
  else
    ini_path = "efis.ini";

  kotuku::the_hal()->initialize(ini_path);
  _the_app = new kotuku::pfd_application_t(kotuku::the_hal()->root_window());

  watchdog = new kotuku::watchdog_t(kotuku::the_hal()->root_window(), _the_app);

  // start the application
  kotuku::application_t *app = kotuku::the_app();
  kotuku::the_hal()->root_window()->repaint(true);

  _the_app->publishing_enabled(true);
  XEvent e;
  while(1)
    {
      XNextEvent(display, &e);

      switch (e.type)
        {
        case VisibilityNotify :
        case Expose:
          kotuku::the_hal()->root_window()->repaint(true);
          XSync(display, False);
          XFlush(display);
          break;
        }
      // Handle Windows Close Event
//       if(e.type==ClientMessage)
//         break;

    }

  return 0;
  }
