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
#include "thread.h"
#include "assert.h"
#include "hal.h"
#include "application.h"

#include <map>
#include <vector>
#include <algorithm>

kotuku::event_t::event_t(bool manual_reset,
bool initial_state)
  {
  _handle = application_t::instance->hal()->event_create(manual_reset, initial_state);
  }

void kotuku::event_t::set()
  {
  application_t::instance->hal()->event_set(_handle);
  }

kotuku::event_t::lock_t::lock_t(const event_t &evt, uint32_t timeout)
  {
  _was_aquired = application_t::instance->hal()->event_wait(evt._handle, timeout);
  }

kotuku::event_t::lock_t::~lock_t()
  {
  }

kotuku::critical_section_t::critical_section_t()
  {
  _handle = application_t::instance->hal()->critical_section_create();
  }

kotuku::critical_section_t::~critical_section_t()
  {
  application_t::instance->hal()->critical_section_close(_handle);
  }

kotuku::critical_section_t::lock_t::lock_t(const critical_section_t &cs) :
    _cs(cs)
  {
  application_t::instance->hal()->critical_section_lock(_cs._handle);
  }

kotuku::critical_section_t::lock_t::~lock_t()
  {
  application_t::instance->hal()->critical_section_unlock(_cs._handle);
  }

void kotuku::thread_t::suspend()
  {
  application_t::instance->hal()->thread_suspend(_handle);
  }

void kotuku::thread_t::resume()
  {
  application_t::instance->hal()->thread_resume(_handle);
  }

void kotuku::thread_t::terminate()
  {
  application_t::instance->hal()->thread_terminate(_handle, (uint32_t) -1);
  }

uint32_t kotuku::thread_t::wait_for_exit(uint32_t timeout)
  {
  if(!application_t::instance->hal()->thread_wait(_handle, timeout))
    return 0;

  return application_t::instance->hal()->thread_exit_code(_handle);
  }

uint32_t kotuku::thread_t::terminate_and_wait(uint32_t timeout,
    uint32_t terminate_code)
  {
  if(!application_t::instance->hal()->thread_wait(_handle, timeout))
    application_t::instance->hal()->thread_terminate(_handle, terminate_code);

  return application_t::instance->hal()->thread_exit_code(_handle);
  }

kotuku::thread_t::status_t kotuku::thread_t::get_status() const
  {
  return application_t::instance->hal()->thread_status(_handle);
  }

kotuku::thread_t::priority_t kotuku::thread_t::priority() const
  {
  return application_t::instance->hal()->thread_get_priority(_handle);
  }

kotuku::thread_t::priority_t kotuku::thread_t::priority(priority_t p)
  {
  priority_t old_p = priority();
  application_t::instance->hal()->thread_set_priority(_handle, p);
  return old_p;
  }

unsigned int kotuku::thread_t::id() const
  {
  return application_t::instance->hal()->thread_current_id(_handle);
  }

kotuku::thread_t::thread_t(size_t size, void *p_this, threadfunc pfun)
  {
  _handle = application_t::instance->hal()->thread_create(pfun, size, p_this, 0);
  }

kotuku::thread_t::~thread_t()
  {
  application_t::instance->hal()->thread_close(_handle);
  }

void kotuku::thread_t::sleep(uint32_t timems)
  {
  application_t::instance->hal()->thread_sleep(timems);
  }

bool kotuku::thread_t::should_terminate() const
  {
  return application_t::instance->hal()->thread_yield(_handle);
  }
