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

#include <stdio.h>

kotuku::application_t *kotuku::application_t::instance;
kotuku::hal_t *kotuku::application_t::hal;

static kotuku::watchdog_t *watchdog;

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
  {
  const char *config = 0;
  if(lpCmdLine != NULL && strlen(lpCmdLine) > 0)
    config = (char *)lpCmdLine;
  else
    config = "..\\..\\configs\\efis.ini";

  FILE *fd = 0;
  
  errno_t err = fopen_s(&fd, config, "r");
  if(err != 0)
    {
    printf("Error cannot open config file : %d", err);
    return -1;
    }

  fseek(fd, 0, SEEK_END);
  long len = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char *buffer = new char[len + 1];
  size_t chars = fread(buffer, 1, len, fd);
  buffer[chars] = 0;

  fclose(fd);
  kotuku::application_t::hal = new kotuku::win32_hal_t();
  kotuku::application_t::hal->initialize(buffer);

  delete buffer;

  kotuku::application_t::instance = new kotuku::application_t();

  watchdog = new kotuku::watchdog_t(kotuku::application_t::hal->root_window(), kotuku::application_t::instance);

  // turn on all events
  kotuku::application_t::instance->publishing_enabled(true);
  kotuku::application_t::hal->root_window()->paint(true);
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

        kotuku::application_t::hal->root_window()->paint(false);

        metric = kotuku::gdi_screen_t::gdi_metric();
        break;
      default :
        DispatchMessage(&msg);
        break;
      }
    }
  return 0;
  }
