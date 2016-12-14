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
#ifndef __linux_hal_h__
#define __linux_hal_h__

#include <map>
#include "../gdi-lib/hal.h"

namespace kotuku
  {
  class linux_can_driver_t;
  class layout_window_t;

  typedef enum _screen_type_t {
    st_root,
    st_menu,
    st_notifications
  } screen_type_t;
  // this hal interfaces to a generic linux system which can have different
  // types of screens.  To enable a generic hal this code creates
  // a factory functor to create a screen at runtime
  typedef result_t (*create_root_screen)(hal_t *, screen_type_t, window_t **);

  class linux_hal_t: public hal_t
    {
  public:
    linux_hal_t(create_root_screen);

    result_t initialize(const char *path);

    virtual time_base_t now();
    // make time from the values passed
    virtual time_base_t mktime(unsigned short year, uint8_t month,
        uint8_t day, uint8_t hour, uint8_t minute,
        uint8_t second, unsigned short milliseconds,
        uint32_t nanoseconds);
    // split a time value
    virtual void gmtime(time_base_t when, unsigned short *year,
        uint8_t *month, uint8_t *day, uint8_t *hour,
        uint8_t *minute, uint8_t *second,
        unsigned short *milliseconds, uint32_t *nanoseconds);

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
    virtual handle_t thread_create(thread_func, size_t stack_size, void *param,
        unsigned int *id);
    virtual void thread_set_priority(handle_t, uint8_t);
    virtual uint8_t thread_get_priority(handle_t);
    virtual void thread_suspend(handle_t);
    virtual void thread_resume(handle_t);
    virtual void thread_sleep(uint32_t);
    virtual bool thread_wait(handle_t, uint32_t time_ms);
    virtual uint32_t thread_exit_code(handle_t);
    virtual void thread_close(handle_t);
    /**
     * Called when a vertical sync is detected to perform
     * screen redraw
     */
    void update_window();

    virtual void thread_terminate(handle_t, uint32_t);
    virtual unsigned int thread_current_id(handle_t);
    virtual bool thread_yield(handle_t);
    virtual thread_t::status_t thread_status(handle_t);

    // screen functions.
    virtual screen_t *screen_create(screen_t *, const extent_t &);
    virtual screen_t *screen_create(screen_t *, const bitmap_t &);
    virtual screen_t *screen_create(screen_t *, const rect_t &);
    virtual void screen_close(window_t *);

    // can driver functions
    virtual result_t publish(const can_msg_t &msg);
    virtual result_t set_can_provider(canaerospace_provider_t *pdata);
    virtual result_t get_can_provider(canaerospace_provider_t **);

    virtual int trace_level() const;

    virtual layout_window_t *root_window();
    virtual menu_window_t *menu_window();
    virtual notification_window_t *alert_window();
  protected:
    typedef std::map<std::string, device_handle_t> device_map_t;
    device_map_t _devices;

    result_t _last_error;

    canaerospace_provider_t *_provider;
    linux_can_driver_t *_can_interface;

    static int _trace_level;

//  critical_section_t _cs;

    screen_t *_screen;
    layout_window_t *_root_window;
    menu_window_t *_menu_window;
    notification_window_t *_alert_window;

    create_root_screen _screen_creator;
    };
  }

inline kotuku::layout_window_t *kotuku::linux_hal_t::root_window()
  {
  return _root_window;
  }

#endif
