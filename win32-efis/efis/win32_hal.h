#ifndef __win32_hal_h__
#define __win32_hal_h__

#if !defined(STRICT)
#define STRICT
#endif

#define NOMINMAX
#define OEMRESOURCE
#define _WIN32_WINNT 0x0401
#define UNICODE
#define _UNICODE

#include <windows.h>


#undef handle_t
#undef DEFINE_GUID

#ifdef RGB
#undef RGB
#endif

#ifdef DEFINE_TS
#undef DEFINE_TS
#endif

#pragma warning(disable : 4312 4311 4267 4244)

#include "../../gdi-lib/stddef.h"
#include "../../gdi-lib/application.h"
#include "../../gdi-lib/window.h"
#include "../../gdi-lib/spatial.h"
#include "../../gdi-lib/hal.h"
#include "../../gdi-lib/trace.h"
#include <deque>
#include <vector>
#include <map>
#include <algorithm>
#include <math.h>

// actual root window in our application
#include "../../widgets/layout_window.h"
#include "win32_hal_device.h"
#include "serial_can_device.h"

extern const wchar_t *to_wstring(const char *str);
extern const wchar_t *child_window_class;
extern const wchar_t *screen_name;

namespace kotuku {
  class windows_screen_t;
  class win32_hal_t : public hal_t {
  public:
    win32_hal_t();
    result_t initialize(const char *config_key);

    virtual time_base_t now();
    // make time from the values passed
    virtual time_base_t mktime(unsigned short year,
      uint8_t month,
      uint8_t day,
      uint8_t hour,
      uint8_t minute,
      uint8_t second,
      unsigned short milliseconds,
      uint32_t nanoseconds);
    // split a time value
    virtual void gmtime(time_base_t when,
      unsigned short *year,
      uint8_t *month,
      uint8_t *day,
      uint8_t *hour,
      uint8_t *minute,
      uint8_t *second,
      unsigned short *milliseconds,
      uint32_t *nanoseconds);

    virtual void assert_failed();
    virtual void debug_output(int level, const char *);

    // memory checking functions
    virtual result_t is_bad_read_pointer(const void *, size_t);
    virtual result_t is_bad_write_pointer(void *, size_t);
    virtual long interlocked_increment(volatile long &);
    virtual long interlocked_decrement(volatile long &);

    // event functions
    virtual handle_t event_create(bool manual_reset, bool initial_state);
    virtual void event_close(handle_t);
    virtual bool event_wait(handle_t, uint32_t timeout_ms);
    virtual void event_set(handle_t);

    // critical section functions
    virtual handle_t critical_section_create();
    virtual void critical_section_close(handle_t);
    virtual void critical_section_lock(handle_t);
    virtual void critical_section_unlock(handle_t);

    // thread functions
    virtual handle_t thread_create(thread_func, size_t stack_size, void *param, unsigned int *id);
    virtual void thread_set_priority(handle_t, uint8_t);
    virtual uint8_t thread_get_priority(handle_t);
    virtual void thread_suspend(handle_t);
    virtual void thread_resume(handle_t);
    virtual void thread_sleep(uint32_t);
    virtual bool thread_wait(handle_t, uint32_t time_ms);
    virtual uint32_t thread_exit_code(handle_t);
    virtual void thread_close(handle_t);
    virtual void thread_terminate(handle_t, uint32_t);
    virtual unsigned int thread_current_id(handle_t);
    virtual bool thread_yield(handle_t);
    virtual thread_t::status_t thread_status(handle_t);

    // error functions
    virtual result_t set_last_error(result_t);
    virtual result_t get_last_error();

    // screen functions.
    virtual screen_t *screen_create(screen_t *, const rect_t &);
    virtual screen_t *screen_create(screen_t *, const extent_t &);
    virtual screen_t *screen_create(screen_t *, const bitmap_t &);
    virtual void screen_close(window_t *);

    virtual result_t publish(const can_msg_t &msg);
    virtual result_t set_can_provider(canaerospace_provider_t *);
    virtual result_t get_can_provider(canaerospace_provider_t **provider) { *provider = _prov; return s_ok; }

    // called by the timer
    virtual int trace_level() const;
    void post_msg(unsigned short id, short value);

    // these should only be called once!
    virtual layout_window_t *root_window();
    virtual menu_window_t *menu_window();
    virtual notification_window_t *alert_window();
  protected:
    critical_section_t *_cs;
    can_device_t *_can_interface;

    windows_screen_t *_screen;

    layout_window_t *_root_window;
    menu_window_t *_menu_window;
    notification_window_t *_alert_window;

    int _trace_level;
    uint8_t _node_id;
    volatile long _sequence;

    canaerospace_provider_t *_prov;
    std::vector<std::string> _section_names();
    std::string _root_key;
    result_t ensure_path(const char *section, HKEY &rslt) const;
  };

  inline win32_hal_t *hal_impl()
    {
    return reinterpret_cast<win32_hal_t *>(the_hal());
    }

};

#endif
