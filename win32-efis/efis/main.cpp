#include "../../widgets/canfly.h"
#include "../../widgets/application.h"
#include "../../widgets/watchdog.h"
#include "../../widgets/can_aerospace.h"
#include "../../widgets/screen.h"
#include "gdi_screen.h"

#ifdef RGB
#undef RGB
#endif

#include "win32_hal.h"

#include <windows.h>

kotuku::application_t *kotuku::application_t::instance;


static kotuku::watchdog_t *watchdog;
#ifdef _WIN32_WCE
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
#else
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
  {
  const char *config = 0;
  if(lpCmdLine != NULL)
    config = (char *)lpCmdLine;
  else
    config = "..\\..\\configs\\efis.ini";

  FILE *fd = fopen(config, "r");

  long len = ftell(fd);
  char *buffer = new char[len + 1];
  size_t chars = fread(buffer, 1, len, fd);
  buffer[chars] = 0;

  kotuku::application_t::instance = new kotuku::application_t(new kotuku::win32_hal_t());

  kotuku::application_t::instance->initialize(buffer);

  watchdog = new kotuku::watchdog_t(kotuku::application_t::instance->hal()->root_window(), kotuku::application_t::instance);

  // turn on all events
  kotuku::application_t::instance->publishing_enabled(true);
  kotuku::application_t::instance->hal()->root_window()->paint(true);
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

        kotuku::application_t::instance->hal()->root_window()->paint(false);

        metric = kotuku::gdi_screen_t::gdi_metric();
        break;
      default :
        DispatchMessage(&msg);
        break;
      }
    }
  return 0;
  }
