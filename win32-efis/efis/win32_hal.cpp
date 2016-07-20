// This hal provides an emulation environment using 
// the windows environment
#define UNICODE
#include "win32_hal.h"
#include "windows_screen.h"

#ifndef _WIN32_WCE
#include <crtdbg.h>
#endif
#include "serial_can_device.h"

#include "../../gdi-lib/can_aerospace.h"
#include "../../widgets/layout_window.h"
#include "../../widgets/pfd_application.h"
#include "../../widgets/menu_window.h"
#include "../../widgets/notification_window.h"

#pragma intrinsic(sin, cos, atan2)

// This uses a static buffer so is not re-entrant
static wchar_t *w_buffer = 0;
static int w_buflen = 0;

const wchar_t *to_wstring(const char *str)
  {
  size_t len = strlen(str);

  int buffer_needed = MultiByteToWideChar(CP_ACP, 0, str, len, 0, 0);

  if(buffer_needed > w_buflen)
    {
    delete[] w_buffer;
    buffer_needed |= 511;
    buffer_needed++;

    w_buffer = new wchar_t[buffer_needed];
    w_buflen = buffer_needed;
    }

  w_buffer[MultiByteToWideChar(CP_ACP, 0, str, len, w_buffer, w_buflen)] = 0;

  return w_buffer;
  }

static char *buffer = 0;
static int buflen = 0;

const char *to_string(const wchar_t *str)
  {
  size_t len = wcslen(str);

  int buffer_needed = WideCharToMultiByte(CP_ACP, 0, str, len, 0, 0, NULL, NULL);

  if(buffer_needed > buflen)
    {
    delete[] buffer;
    buffer_needed |= 511;
    buffer_needed++;

    buffer = new char[buffer_needed];
    buflen = buffer_needed;
    }

  buffer[WideCharToMultiByte(CP_ACP, 0, str, len, buffer, buflen, NULL, NULL)] = 0;

  return buffer;
  }

extern "C" {
char *strdup(const char *s)
  {
  if(s == 0)
    return 0;

  char *value = new char[strlen(s) +1];
  strcpy(value, s);

  return value;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Global functions to help with the initialization
//

static const char *hal_section = "win32-hal";
static const char *node_id_key = "node-id";
static const char *trace_level_key = "trace-level";
static const char *rotation_key = "rotation";
static const char *layout_key = "layout";

enum trace_levels
{
  LOG_ERROR,
  LOG_WARNINGS,
  LOG_INFORMATION,
  LOG_DEBUG
};

kotuku::win32_hal_t::win32_hal_t()
  {
  _root_window = 0;
  _menu_window = 0;
  _alert_window = 0;
  _cs = 0;

  _node_id = 0x60;      // first node
  }

result_t kotuku::win32_hal_t::initialize(const char *config_key)
  {
  if(config_key != 0)
    _root_key = config_key;

  result_t result;
  if(failed(result = hal_t::initialize(config_key)))
    return result;

  std::string device;
  if(failed(get_config_value(hal_section, "can-device", device)))
    device = "COM3:";

  int baud_rate;
  if(failed(get_config_value(hal_section, "baud-rate", baud_rate)))
    baud_rate = 240000;

  _can_interface = new serial_can_device_t(device, baud_rate);
  
  int node_id;
  if(succeeded(get_config_value(hal_section, node_id_key, node_id)))
    _node_id = (uint8_t) node_id;

  if(failed(get_config_value(hal_section, trace_level_key, _trace_level)))
    _trace_level = LOG_DEBUG;

  return s_ok;
  }

int kotuku::win32_hal_t::trace_level() const
  {
  return _trace_level;
  }

LRESULT CALLBACK ScreenProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
  switch(uMsg)
    {
    case WM_PAINT :
      {
        kotuku::windows_screen_t *handle = 
          reinterpret_cast<kotuku::windows_screen_t *>(GetWindowLongW(hwnd, GWL_USERDATA));

      if(handle == 0)
        {

        }
      else
        {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        kotuku::rect_t paint_rect(ps.rcPaint.left,
          ps.rcPaint.top,
          ps.rcPaint.right,
          ps.rcPaint.bottom);

        handle->paint(dc, paint_rect);

        EndPaint(hwnd, &ps);
        }
      }
      return 0;
    case WM_ERASEBKGND :
      return 0;
    case WM_CLOSE :
      PostMessageW(NULL, WM_QUIT, 0, 0);
      break;
    default :
      break;
    }

  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }

kotuku::hal_t::time_base_t  kotuku::win32_hal_t::now()
  {
  SYSTEMTIME st;
  GetSystemTime(&st);

  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);

  return hal_t::time_base_t(ft.dwHighDateTime ) << 32 | hal_t::time_base_t(ft.dwLowDateTime);
  }

kotuku::hal_t::time_base_t  kotuku::win32_hal_t::mktime(unsigned short year,
  uint8_t month,
  uint8_t day,
  uint8_t hour,
  uint8_t minute,
  uint8_t second,
  unsigned short milliseconds,
  uint32_t nanoseconds)
  {
  SYSTEMTIME st;
  st.wYear = year;
  st.wMonth = month;
  st.wDay = day;
  st.wHour = hour;
  st.wMinute = minute;
  st.wSecond = second;
  st.wMilliseconds = milliseconds;

  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);

  return hal_t::time_base_t(ft.dwHighDateTime ) << 32 | hal_t::time_base_t(ft.dwLowDateTime);
  }

// split a time value
void  kotuku::win32_hal_t::gmtime(time_base_t when,
  unsigned short *year,
  uint8_t *month,
  uint8_t *day,
  uint8_t *hour,
  uint8_t *minute,
  uint8_t *second,
  unsigned short *milliseconds,
  uint32_t *nanoseconds)
  {
  FILETIME ft;
  ft.dwHighDateTime = DWORD(when >> 32);
  ft.dwLowDateTime = DWORD(when);

  SYSTEMTIME st;
  FileTimeToSystemTime(&ft, &st);

  if(year != 0)
    *year = (unsigned short) st.wYear;

  if(month != 0)
    *month = (uint8_t) st.wMonth;

  if(day != 0)
    *day = (uint8_t) st.wDay;

  if(hour != 0)
    *hour = (uint8_t) st.wHour;

  if(minute != 0)
    *minute = (uint8_t) st.wMinute;

  if(second != 0)
    *second = (uint8_t) st.wSecond;

  if(milliseconds != 0)
    *milliseconds = (unsigned short) st.wMilliseconds;

  if(nanoseconds != 0)
    *nanoseconds = 0;
  }

void  kotuku::win32_hal_t::assert_failed()
  {
  if(__debug_flag)
    DebugBreak();
  }

void  kotuku::win32_hal_t::debug_output(int level, const char *msg)
  {
  OutputDebugStringW(to_wstring(msg));
  }

// memory checking functions
result_t kotuku::win32_hal_t::is_bad_read_pointer(const void *p,
  size_t n)
  {
  return (IsBadReadPtr(p, n) == TRUE) ? e_bad_pointer : s_ok;
  }

result_t kotuku::win32_hal_t::is_bad_write_pointer(void *p,
  size_t n)
  {
  return (IsBadWritePtr(p, n) == TRUE) ? e_bad_pointer : s_ok;
  }

long  kotuku::win32_hal_t::interlocked_increment(volatile long &n)
  {
  return InterlockedIncrement((long *)&n);
  }

long  kotuku::win32_hal_t::interlocked_decrement(volatile long &n)
  {
  return InterlockedDecrement((long *)&n);
  }

// event functions
handle_t  kotuku::win32_hal_t::event_create(bool manual_reset,
  bool initial_state)
  {
  return (handle_t) CreateEventW(NULL, manual_reset, initial_state, NULL);
  }

void  kotuku::win32_hal_t::event_close(handle_t h)
  {
  CloseHandle(HANDLE(h));
  }

bool  kotuku::win32_hal_t::event_wait(handle_t h, uint32_t timeout_ms)
  {
  return WaitForSingleObject(HANDLE(h), timeout_ms) == WAIT_OBJECT_0;
  }

void  kotuku::win32_hal_t::event_set(handle_t h)
  {
  SetEvent(HANDLE(h));
  }

// critical section functions
handle_t  kotuku::win32_hal_t::critical_section_create()
  {
  LPCRITICAL_SECTION cs = new CRITICAL_SECTION;
  InitializeCriticalSection(cs);
  return handle_t(cs);
  }

void  kotuku::win32_hal_t::critical_section_close(handle_t h)
  {
  //LPCRITICAL_SECTION cs = reinterpret_cast<LPCRITICAL_SECTION>(h);
  //DeleteCriticalSection(cs);
  //delete cs;
  }

void  kotuku::win32_hal_t::critical_section_lock(handle_t h)
  {
  if(h != 0)
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(h));
  }

void  kotuku::win32_hal_t::critical_section_unlock(handle_t h)
  {
  if(h != 0)
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(h));
  }

handle_t  kotuku::win32_hal_t::thread_create(thread_func pfn,
  size_t stack_size,
  void *param,
  unsigned int *id)
  {
  return (handle_t) CreateThread(NULL, stack_size, (LPTHREAD_START_ROUTINE)pfn, param, CREATE_SUSPENDED, (LPDWORD)id);
  }

void  kotuku::win32_hal_t::thread_set_priority(handle_t h,
  uint8_t p)
  {
  SetThreadPriority(HANDLE(h), p);
  }

uint8_t  kotuku::win32_hal_t::thread_get_priority(handle_t h)
  {
  return (uint8_t) GetThreadPriority(HANDLE(h));
  }

void  kotuku::win32_hal_t::thread_suspend(handle_t h)
  {
  SuspendThread(HANDLE(h));
  }

void  kotuku::win32_hal_t::thread_resume(handle_t h)
  {
  ResumeThread(HANDLE(h));
  }

void  kotuku::win32_hal_t::thread_sleep(uint32_t n)
  {
  Sleep(n);
  }

bool  kotuku::win32_hal_t::thread_yield(handle_t h)
  {
  return thread_status(h) == thread_t::terminated;
  }

kotuku::thread_t::status_t  kotuku::win32_hal_t::thread_status(handle_t h)
  {
  DWORD exitCode;
  if(::GetExitCodeThread(h, &exitCode) && exitCode != STILL_ACTIVE)
    return thread_t::terminated;

  return thread_t::running;
  }

bool  kotuku::win32_hal_t::thread_wait(handle_t h,
  uint32_t time_ms)
  {
  return WaitForSingleObject(HANDLE(h), time_ms) == WAIT_OBJECT_0;
  }

uint32_t  kotuku::win32_hal_t::thread_exit_code(handle_t h)
  {
  DWORD code;
  GetExitCodeThread(HANDLE(h), &code);

  return code;
  }

void  kotuku::win32_hal_t::thread_close(handle_t h)
  {
  CloseHandle(HANDLE(h));
  }

void  kotuku::win32_hal_t::thread_terminate(handle_t h, uint32_t term_code)
  {
  TerminateThread(HANDLE(h), term_code);
  }

unsigned int  kotuku::win32_hal_t::thread_current_id(handle_t)
  {
  return ::GetCurrentThreadId();
  }

result_t  kotuku::win32_hal_t::set_last_error(result_t r)
  {
  SetLastError(r);
  return GetLastError();
  }

result_t kotuku::win32_hal_t::get_last_error()
  {
  return GetLastError();
  }

static const wchar_t *class_name = L"EFIS_Screen";
static const wchar_t *alert_window_class = L"EFIS_Alert";
static const wchar_t *menu_window_class = L"EFIS_Menu";
const wchar_t *screen_name = L"diy-efis";
const wchar_t *child_window_class = L"EFIS_Canvas";

kotuku::layout_window_t * kotuku::win32_hal_t::root_window()
  {
  if(_root_window == 0)
    {
    int screen_x;
    int screen_y;
    if(failed(get_config_value(hal_section, "screen-x", screen_x)))
      screen_x = 480;

    if(failed(get_config_value(hal_section, "screen-y", screen_y)))
      screen_y = 640;

    // we create the screen with a caption and a border

    WNDCLASSW wndclass;
    memset(&wndclass, 0, sizeof(WNDCLASS));
    wndclass.lpfnWndProc = ScreenProc;
    wndclass.lpszClassName = class_name;

    RegisterClassW(&wndclass);

    wndclass.lpszClassName = child_window_class;
    wndclass.style = CS_PARENTDC;

    RegisterClassW(&wndclass);

    if(failed(get_config_value("hal", "screen-y", screen_y)))
      screen_y = 640;

    HWND hwnd = CreateWindowExW(0, class_name, screen_name, WS_VISIBLE | WS_POPUP | WS_CAPTION,
      0, 0, screen_x, screen_y + GetSystemMetrics(SM_CYCAPTION) + (GetSystemMetrics(SM_CYBORDER)<<1),
      NULL, NULL, NULL, NULL);

    size_t bpp = (size_t) GetDeviceCaps(GetDC(hwnd), BITSPIXEL);

    _screen = new windows_screen_t(hwnd, screen_x, screen_y, bpp);

    _root_window =  new kotuku::layout_window_t(_screen);

    SetWindowLong(hwnd, GWL_USERDATA, (LONG) _screen);

    _screen->owner(_root_window);
    }

  return _root_window;
  }

kotuku::menu_window_t *kotuku::win32_hal_t::menu_window()
  {
  if(_menu_window = 0)
    {
    int window_x;
    int window_y;
    int width;
    int height;
    if(failed(get_config_value(hal_section, "menu-width", width)))
      width = 480;

    if(failed(get_config_value(hal_section, "menu-height", height)))
      height = 320;

    if(failed(get_config_value(hal_section, "menu-x", window_x)))
      window_x = 0;

    if(failed(get_config_value(hal_section, "menu-y", window_y)))
      window_y = 0;

    // we create the screen with a caption and a border

    WNDCLASSW wndclass;
    memset(&wndclass, 0, sizeof(WNDCLASS));
    wndclass.lpfnWndProc = ScreenProc;
    wndclass.lpszClassName = menu_window_class;
    wndclass.style = CS_PARENTDC;

    RegisterClassW(&wndclass);

    HWND hwnd = CreateWindowExW(0, class_name, screen_name, WS_VISIBLE | WS_POPUP | WS_CAPTION,
      window_x, window_y, width, height,
      _screen->handle(), NULL, NULL, NULL);

    size_t bpp = (size_t) GetDeviceCaps(GetDC(hwnd), BITSPIXEL);

    windows_screen_t *screen = new windows_screen_t(hwnd, width, height, bpp);

    _menu_window =  new kotuku::menu_window_t(screen, "menu-window");
    screen->owner(_menu_window);
    }
  return _menu_window;
  }

kotuku::notification_window_t *kotuku::win32_hal_t::alert_window()
  {
  if(_menu_window = 0)
    {
    int window_x;
    int window_y;
    int width;
    int height;
    if(failed(get_config_value(hal_section, "alert-width", width)))
      width = 480;

    if(failed(get_config_value(hal_section, "alert-height", height)))
      height = 320;

    if(failed(get_config_value(hal_section, "alert-x", window_x)))
      window_x = 0;

    if(failed(get_config_value(hal_section, "alert-y", window_y)))
      window_y = 0;

    // we create the screen with a caption and a border

    WNDCLASSW wndclass;
    memset(&wndclass, 0, sizeof(WNDCLASS));
    wndclass.lpfnWndProc = ScreenProc;
    wndclass.lpszClassName = alert_window_class;
    wndclass.style = CS_PARENTDC;

    RegisterClassW(&wndclass);

    HWND hwnd = CreateWindowExW(0, class_name, screen_name, WS_VISIBLE | WS_POPUP | WS_CAPTION,
      window_x, window_y, width, height,
      _screen->handle(), NULL, NULL, NULL);

    size_t bpp = (size_t) GetDeviceCaps(GetDC(hwnd), BITSPIXEL);

    windows_screen_t *screen = new windows_screen_t(hwnd, width, height, bpp);

    _alert_window =  new notification_window_t(screen, "alert-window");
    screen->owner(_alert_window);
    }

  return _alert_window;
  }

kotuku::screen_t * kotuku::win32_hal_t::screen_create(screen_t *h, const rect_t &rect)
  {
  windows_screen_t *handle = as_screen_handle(h);
  return handle->create_canvas(h, rect);
  }

kotuku::screen_t * kotuku::win32_hal_t::screen_create(screen_t *h, const extent_t &sz)
  {
  windows_screen_t *handle = as_screen_handle(h);
  return handle->create_canvas(h, sz);
  }

kotuku::screen_t * kotuku::win32_hal_t::screen_create(screen_t *h, const bitmap_t &bm)
  {
  windows_screen_t *handle = as_screen_handle(h);
  return handle->create_canvas(h, bm);
  }

void  kotuku::win32_hal_t::screen_close(window_t *h)
  {
  delete h;
  }

result_t kotuku::win32_hal_t::publish(const can_msg_t &msg)
   {
   return _can_interface->send(msg);
   }

result_t kotuku::win32_hal_t::set_can_provider(canaerospace_provider_t *prov)
  {
  _prov = prov;
  return _can_interface->set_can_provider(prov);
  }
void  kotuku::win32_hal_t::post_msg(unsigned short id, short value)
  {
  can_msg_t msg;
  msg.id = id;
  msg.flags = 6;
  msg.msg.raw[0] = _node_id;
  msg.msg.raw[1] = 6;          // short type
  msg.msg.raw[2] = 0;
  msg.msg.raw[3] = (uint8_t) __interlocked_increment(_sequence);
  msg.msg.raw[4] = (uint8_t) (value >> 8);
  msg.msg.raw[5] = (uint8_t) value;

  publish(msg);
  }
