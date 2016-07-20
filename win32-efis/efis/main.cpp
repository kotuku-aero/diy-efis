#include "../../gdi-lib/stddef.h"
#include "../../widgets/pfd_application.h"
#include "../../widgets/watchdog.h"
#include "../../gdi-lib/can_aerospace.h"
#include "../../gdi-lib/screen.h"
#include "gdi_screen.h"

#ifdef RGB
#undef RGB
#endif

#include "win32_hal.h"

#include <windows.h>


static kotuku::watchdog_t *watchdog;
static kotuku::pfd_application_t *_the_app = 0;

kotuku::application_t *kotuku::the_app()
  {
  return _the_app;
  }

kotuku::hal_t *kotuku::the_hal()
  {
  static kotuku::win32_hal_t *hal_impl = 0;
  if(hal_impl == 0)
    hal_impl = new kotuku::win32_hal_t();

  return hal_impl;
  }

#ifdef _WIN32_WCE
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
#else
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
  {
  reinterpret_cast<kotuku::win32_hal_t *>(kotuku::the_hal())->initialize("..\\..\\configs\\efis.ini");
  _the_app = new kotuku::pfd_application_t(kotuku::the_hal()->root_window());

  watchdog = new kotuku::watchdog_t(kotuku::the_hal()->root_window(), _the_app);

  // turn on all events
  _the_app->publishing_enabled(true);
  _the_app->root_window()->paint(true);
  int metric;

  SetTimer(NULL, NULL, 200, NULL);

  MSG msg;
  while(GetMessageW(&msg, NULL, 0, 0))
    {
    switch(msg.message)
      {
      case WM_PAINT :
      case WM_TIMER :
        kotuku::gdi_screen_t::gdi_metric(0);

        _the_app->root_window()->paint(false);

        metric = kotuku::gdi_screen_t::gdi_metric();
        break;
      default :
        DispatchMessage(&msg);
        break;
      }
    }
  return 0;
  }
