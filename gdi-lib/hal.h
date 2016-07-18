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
#ifndef __hal_h__
#define __hal_h__

#include "stddef.h"
#include "canvas.h"
#include "device.h"
#include "screen.h"
#include "can_driver.h"
#include "can_aerospace.h"

#include <vector>
#include <string>

namespace kotuku
  {
  class window_t;

  class layout_window_t;
  class menu_window_t;
  class notification_window_t;

///////////////////////////////////////////////////////////////////////////////
//
//  Hardware abstraction layer.
//
//  This defines all of the functions that need to be implemented to support
// vFC based applications.
//

  class hal_t
    {
  public:

    virtual ~hal_t();

    // initialize the hal
    virtual result_t initialize(const char *);

    /////////////////////////////////////////////////////////////////////////
    // clock functions. Return a 32bit clock value based on
    // # of 100nanosecond intervals since January 1 1970
    // the
    typedef int64_t time_base_t;

    // return the time
    virtual time_base_t now() = 0;
    // make time from the values passed
    virtual time_base_t mktime(uint16_t year, uint8_t month, uint8_t day,
        uint8_t hour, uint8_t minute, uint8_t second, uint16_t milliseconds,
        uint32_t nanoseconds) = 0;
    // split a time value
    virtual void gmtime(time_base_t when, uint16_t *year, uint8_t *month,
        uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second,
        uint16_t *milliseconds, uint32_t *nanoseconds) = 0;

    virtual void assert_failed() = 0;
    virtual void debug_output(int level, const char *) = 0;

    // memory checking functions
    virtual result_t is_bad_read_pointer(const void *, size_t) = 0;
    virtual result_t is_bad_write_pointer(void *, size_t) = 0;
    virtual long interlocked_increment(volatile long &) = 0;
    virtual long interlocked_decrement(volatile long &) = 0;

    // event functions
    virtual handle_t event_create(bool manual_reset, bool initial_state) = 0;
    virtual void event_close(handle_t) = 0;
    virtual bool event_wait(handle_t, uint32_t timeout_ms) = 0;
    virtual void event_set(handle_t) = 0;

    // critical section functions
    virtual handle_t critical_section_create() = 0;
    virtual void critical_section_close(handle_t) = 0;
    virtual void critical_section_lock(handle_t) = 0;
    virtual void critical_section_unlock(handle_t) = 0;

    // thread functions
    typedef uint32_t (*thread_func)(void *);

    virtual handle_t thread_create(thread_func, size_t stack_size, void *param,
        unsigned int *id) = 0;
    virtual void thread_set_priority(handle_t, uint8_t) = 0;
    virtual uint8_t thread_get_priority(handle_t) = 0;
    virtual void thread_suspend(handle_t) = 0;
    virtual void thread_resume(handle_t) = 0;
    virtual void thread_sleep(uint32_t) = 0;
    virtual bool thread_wait(handle_t, uint32_t time_ms) = 0;
    virtual uint32_t thread_exit_code(handle_t) = 0;
    virtual void thread_close(handle_t) = 0;
    virtual void thread_terminate(handle_t, uint32_t) = 0;
    virtual unsigned int thread_current_id(handle_t) = 0;
    virtual bool thread_yield(handle_t) = 0;
    virtual thread_t::status_t thread_status(handle_t) = 0;

    // config value functions

    result_t get_config_value(const char *name, std::string &rslt) const
      {
      return get_config_value(0, name, rslt);
      }

    result_t get_config_value(const char *name, int &rslt) const
      {
      return get_config_value(0, name, rslt);
      }

    result_t get_config_value(const char *name, double &rslt) const
      {
      return get_config_value(0, name, rslt);
      }

    result_t get_config_value(const char *name, bool &rslt) const
      {
      return get_config_value(0, name, rslt);
      }

    result_t get_config_value(const char *name,
        std::vector<uint8_t> &rslt) const
      {
      return get_config_value(0, name, rslt);
      }

    virtual result_t get_config_value(const char *section, const char *name,
        std::string &rslt) const;
    virtual result_t get_config_value(const char *section, const char *name,
        int &rslt) const;
    virtual result_t get_config_value(const char *section, const char *name,
        double &rslt) const;
    virtual result_t get_config_value(const char *section, const char *name,
        bool &rslt) const;
    virtual result_t get_config_value(const char *section, const char *name,
        std::vector<uint8_t> &) const;

    virtual result_t get_section_names(const char *,
        std::vector<std::string> &names) const
      {
      names = _section_names;
      return s_ok;
      }

    // errors
    virtual result_t set_last_error(result_t error);
    virtual result_t get_last_error();

    // screen functions.
    virtual screen_t *screen_create(screen_t *, const extent_t &) = 0;
    virtual screen_t *screen_create(screen_t *, const bitmap_t &) = 0;
    virtual screen_t *screen_create(screen_t *, const rect_t &) = 0;
    virtual void screen_close(window_t *) = 0;

    // can driver functions
    virtual result_t publish(const can_msg_t &msg) = 0;

    virtual result_t set_can_provider(canaerospace_provider_t *provider) = 0;
    virtual result_t get_can_provider(canaerospace_provider_t **provider) = 0;

    virtual int trace_level() const = 0;

    /**
     * Return the main window of the application
     * @return Window that is used as the drawing surface
     */
    virtual layout_window_t *root_window() = 0;
    /**
     * Return an overlay window that allows for the display of a menu
     * @return Window that is an overlay window
     */
    virtual menu_window_t *menu_window() = 0;
    /**
     * Return a window that can display alerts
     * @return Window that can show alerts
     */
    virtual notification_window_t *alert_window() = 0;

  protected:
    hal_t();
  private:
    result_t _last_error;
    static int ini_parser(void* user, const char* section, const char* name,
        const char* value);
    // note the section/name is stored as a string [section]name
    typedef std::map<std::string, std::string> ini_values_t;
    ini_values_t _ini_values;
    std::vector<std::string> _section_names;
    };

  extern hal_t *the_hal();

  inline result_t hal_t::set_last_error(result_t error)
    {
    return _last_error = error;
    }

  inline result_t hal_t::get_last_error()
    {
    return _last_error;
    }
  }
;

#endif
