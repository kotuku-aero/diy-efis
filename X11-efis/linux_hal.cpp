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
#include "linux_hal.h"

#include <deque>
#include <vector>
#include <map>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/signal.h>

#include "../gdi-lib/stddef.h"
#include "../gdi-lib/application.h"
#include "../gdi-lib/window.h"
#include "../gdi-lib/screen.h"
#include "../gdi-lib/spatial.h"
#include "../gdi-lib/can_driver.h"
#include "../gdi-lib/assert.h"
#include "linux_can_driver.h"
#include "comm_device.h"
#include <syslog.h>
#include "../widgets/pfd_application.h"
#include "../widgets/layout_window.h"

namespace kotuku
  {

  }

int kotuku::linux_hal_t::_trace_level = 4;

kotuku::linux_hal_t::linux_hal_t(create_root_screen pfn)
: _screen_creator(pfn)
  {
  _provider = 0;
  _can_interface = 0;
  _trace_level = 4;
  }

int kotuku::linux_hal_t::trace_level() const
  {
  return _trace_level;
  }

static const char *linux_hal_section = "linux-hal";

result_t kotuku::linux_hal_t::initialize(const char *ini_file)
  {
  result_t result;
  if(failed(result = hal_t::initialize(ini_file)))
    return result;

  get_config_value(linux_hal_section, "trace-level", _trace_level);

  std::string can_device;

  if(!succeeded(
      get_config_value(linux_hal_section, "socketcan-device", can_device)))
    can_device = "can0";

  _can_interface = new linux_can_driver_t();

  int receive_timeout = -1;
  get_config_value(linux_hal_section, "receive-timeout", receive_timeout);

  int send_timeout = -1;
  get_config_value(linux_hal_section, "send-timeout", send_timeout);

  _can_interface->initialize(can_device.c_str(), receive_timeout, send_timeout);

  window_t *wnd;

  (*_screen_creator)(this, st_root, &wnd);
  _root_window = reinterpret_cast<layout_window_t *>(wnd);

  return s_ok;
  }

result_t kotuku::linux_hal_t::publish(const can_msg_t &msg)
  {
  return _can_interface->publish(msg);
  }

result_t kotuku::linux_hal_t::set_can_provider(
    canaerospace_provider_t *pdata)
  {
  _provider = pdata;
  return _can_interface->set_can_provider(pdata);
  }

result_t kotuku::linux_hal_t::get_can_provider(
    canaerospace_provider_t **prov)
  {
  *prov = _provider;
  return s_ok;
  }

kotuku::menu_window_t *kotuku::linux_hal_t::menu_window()
  {
  if(_menu_window == 0)
    {
    window_t *wnd;

    (*_screen_creator)(this, st_menu, &wnd);
    _menu_window = reinterpret_cast<menu_window_t *>(wnd);
    }

  return _menu_window;
  }

kotuku::notification_window_t *kotuku::linux_hal_t::alert_window()
  {
  if(_alert_window == 0)
    {
    window_t *wnd;

    (*_screen_creator)(this, st_notifications, &wnd);
    _alert_window = reinterpret_cast<notification_window_t *>(wnd);
    }

  return _alert_window;
  }

#include <sys/timeb.h>

kotuku::linux_hal_t::time_base_t kotuku::linux_hal_t::now()
  {
  timeb now;
  ftime(&now);

  interval_t nowl = now.time;

  interval_t as_interval = (nowl * interval_per_second) + (now.millitm * 10000);

  return hal_t::time_base_t(as_interval);
  }

kotuku::linux_hal_t::time_base_t kotuku::linux_hal_t::mktime(unsigned short year,
    uint8_t month, uint8_t day, uint8_t hour,
    uint8_t minute, uint8_t second, unsigned short milliseconds,
    uint32_t nanoseconds)
  {
  ::tm now;

  now.tm_year = year - 1900;
  now.tm_mon = month - 1;
  now.tm_mday = day;
  now.tm_hour = hour;
  now.tm_min = minute;
  now.tm_sec = second;

  interval_t nowl = ::mktime(&now);
  nowl *= interval_per_second;

  return hal_t::time_base_t(nowl);
  }

// split a time value
void kotuku::linux_hal_t::gmtime(time_base_t when, unsigned short *year,
    uint8_t *month, uint8_t *day, uint8_t *hour,
    uint8_t *minute, uint8_t *second, unsigned short *milliseconds,
    uint32_t *nanoseconds)
  {
  ::time_t gm_now = (::time_t) (when / interval_per_second);
  ::tm *now = ::gmtime(&gm_now);

  if(year != 0)
    *year = (unsigned short) now->tm_year + 1900;

  if(month != 0)
    *month = (uint8_t) now->tm_mon + 1;

  if(day != 0)
    *day = (uint8_t) now->tm_mday;

  if(hour != 0)
    *hour = (uint8_t) now->tm_hour;

  if(minute != 0)
    *minute = (uint8_t) now->tm_min;

  if(second != 0)
    *second = (uint8_t) now->tm_sec;

  if(milliseconds != 0)
    *milliseconds = (unsigned short) ((when - (when / interval_per_second))
        / 10000);

  if(nanoseconds != 0)
    *nanoseconds = 0;
  }

void kotuku::linux_hal_t::assert_failed()
  {
  ::raise(SIGTRAP);
  }

static const char *msgfmt = "%s";

void kotuku::linux_hal_t::debug_output(int level, const char *msg)
  {
  if(level <= _trace_level)
    {
    // write to a syslog server
    syslog(level | LOG_LOCAL0 | LOG_DAEMON, msgfmt, msg);
#ifdef _DEBUG
    perror(msg);
#endif
    }
  }

// TODO: critical sections here

// memory checking functions
static bool _already_handled = false;
static jmp_buf _handler;

void mem_test_handler(int nSig)
  {
  if(!_already_handled)
    {
    _already_handled = true;
    longjmp(_handler, 1);
    }
  }

result_t kotuku::linux_hal_t::is_bad_read_pointer(const void *p,
    size_t n)
  {
  void (*prev_handler)(int sig) = 0;
  _already_handled = false;
  if(setjmp(_handler))
    {
    if(prev_handler != 0)
      signal(SIGSEGV, prev_handler);
    return e_bad_pointer;
    }

  prev_handler = signal(SIGSEGV, mem_test_handler);
  int test_val = 0;
  const uint8_t *bp = reinterpret_cast<const uint8_t *>(p);
  while(n--)
    test_val += *bp++;

  signal( SIGSEGV, prev_handler);

  return s_ok;
  }

result_t kotuku::linux_hal_t::is_bad_write_pointer(void *p, size_t n)
  {
  void (*prev_handler)(int sig) = 0;
  _already_handled = false;
  if(setjmp(_handler))
    {
    if(prev_handler != 0)
      signal(SIGSEGV, prev_handler);
    return e_bad_pointer;
    }

  prev_handler = signal(SIGSEGV, mem_test_handler);
  uint8_t *bp = reinterpret_cast<uint8_t *>(p);
  while(n--)
    {
    *bp = *bp ^ 0xff;
    *bp = *bp ^ 0xff;
    bp++;
    }

  signal( SIGSEGV, prev_handler);

  return s_ok;
  }

long kotuku::linux_hal_t::interlocked_increment(volatile long &n)
  {
  return __sync_fetch_and_add(&n, 1);
  }

long kotuku::linux_hal_t::interlocked_decrement(volatile long &n)
  {
  return __sync_fetch_and_sub(&n, 1);
  }

#include <semaphore.h>

// event functions
handle_t kotuku::linux_hal_t::event_create(bool manual_reset,
    bool initial_state)
  {
  sem_t *semaphore = new sem_t();
  sem_init(semaphore, 0, 0);

  return reinterpret_cast<handle_t>(semaphore);
  }

void kotuku::linux_hal_t::event_close(handle_t h)
  {
  sem_t *sem = reinterpret_cast<sem_t *>(h);
  sem_destroy(sem);
  delete sem;
  }

bool kotuku::linux_hal_t::event_wait(handle_t h, uint32_t timeout_ms)
  {
  sem_t *sem = reinterpret_cast<sem_t *>(h);

  if((long) timeout_ms == -1)
    return sem_wait(sem) == 0;

  timespec ts;
  if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    return false;

  timeout_ms += ts.tv_nsec / 1000000;

  ts.tv_sec += timeout_ms / 1000;
  ts.tv_nsec = (timeout_ms % 1000) * 1000000;

  return sem_timedwait(sem, &ts) == 0;
  }

void kotuku::linux_hal_t::event_set(handle_t h)
  {
  sem_t *sem = reinterpret_cast<sem_t *>(h);
  sem_post(sem);
  }

// critical section functions
handle_t kotuku::linux_hal_t::critical_section_create()
  {
  pthread_mutex_t *mutex = new pthread_mutex_t();
  pthread_mutexattr_t mutexattr;
  pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
  pthread_mutex_init(mutex, &mutexattr);
  pthread_mutexattr_destroy(&mutexattr);

  return mutex;
  }

void kotuku::linux_hal_t::critical_section_close(handle_t h)
  {
  if(h != 0)
    {
    pthread_mutex_t *mutex = reinterpret_cast<pthread_mutex_t *>(h);
    pthread_mutex_destroy(mutex);
    delete mutex;
    }
  }

void kotuku::linux_hal_t::critical_section_lock(handle_t h)
  {
  if(h != 0)
    {
    pthread_mutex_t *mutex = reinterpret_cast<pthread_mutex_t *>(h);
    pthread_mutex_lock(mutex);
    }
  }

void kotuku::linux_hal_t::critical_section_unlock(handle_t h)
  {
  if(h != 0)
    {
    pthread_mutex_t *mutex = reinterpret_cast<pthread_mutex_t *>(h);
    pthread_mutex_unlock(mutex);
    }
  }

namespace kotuku
  {
  class thread_handle_t
    {
  public:
    thread_handle_t(hal_t::thread_func pfn, size_t stack_size, void *param);
    ~thread_handle_t();
    void suspend();
    void resume();
    void terminate(uint32_t);
    bool yield();
    uint32_t thread_exit_code();
    void set_priority(uint8_t);
    uint8_t get_priority();
    unsigned int thread_id();
    thread_t::status_t status();
  private:
    pthread_t _thread;
    pthread_mutex_t _suspend_mutex;
    pthread_cond_t _suspend_cond;
    thread_t::status_t _status;
    hal_t::thread_func _thread_func;
    void *_param;
    static void *run(void *);
    uint32_t _thread_exit_code;
    };
  }

kotuku::thread_handle_t::thread_handle_t(hal_t::thread_func pfn,
    size_t stack_size, void *param)
  {
  _thread_func = pfn;
  _param = param;

  _status = thread_t::creating;

  pthread_mutex_init(&_suspend_mutex, NULL);
  pthread_mutex_lock(&_suspend_mutex);

  pthread_condattr_t condattr;
  pthread_condattr_init(&condattr);
  pthread_cond_init(&_suspend_cond, &condattr);
  pthread_condattr_destroy(&condattr);

  pthread_attr_t threadattr;

  pthread_attr_init(&threadattr);
  pthread_attr_setstacksize(&threadattr, stack_size);
  pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
  pthread_create(&_thread, &threadattr, run, this);
  pthread_attr_destroy(&threadattr);
  }

kotuku::thread_handle_t::~thread_handle_t()
  {
  pthread_cancel(_thread);
  }

void *kotuku::thread_handle_t::run(void *p)
  {
  thread_handle_t *pthis = reinterpret_cast<thread_handle_t *>(p);

  // started suspended
  pthread_mutex_lock(&(pthis->_suspend_mutex));
  if((pthis->_status & thread_t::creating) != 0)
    {
    while((pthis->_status & thread_t::creating) != 0)
      pthread_cond_wait(&(pthis->_suspend_cond), &(pthis->_suspend_mutex));
    }
  else
    pthread_mutex_unlock(&(pthis->_suspend_mutex));

  // run the thread.
  try
    {
    pthis->_thread_exit_code = pthis->_thread_func(pthis->_param);
    }
  catch(...)
    {
    // TODO: make this more sensible.
    pthis->_thread_exit_code = -1;
    }

  return 0;
  }

void kotuku::thread_handle_t::suspend()
  {
  _status = _status | thread_t::suspended;
  }

void kotuku::thread_handle_t::resume()
  {
  bool release_thread = false;
  // see if starting
  if((_status & thread_t::creating) != 0)
    {
    _status = _status & ~thread_t::creating;
    release_thread = true;
    }

  if((_status & thread_t::suspended) == 0)
    {
    _status = _status & thread_t::suspended;
    release_thread = true;
    }

  if(release_thread)
    pthread_cond_signal(&_suspend_cond);

  pthread_mutex_unlock(&_suspend_mutex);
  }

void kotuku::thread_handle_t::terminate(uint32_t wait_ms)
  {
  pthread_mutex_lock(&_suspend_mutex);
  _status = _status | thread_t::terminated;
  pthread_mutex_unlock(&_suspend_mutex);

  // wait for the termination event
  }

bool kotuku::thread_handle_t::yield()
  {
  pthread_mutex_lock(&_suspend_mutex);

  if((_status & thread_t::suspended) != 0)
    {
    // suspend the thread untill we are signalled.
    while((_status & thread_t::suspended) != thread_t::running)
      pthread_cond_wait(&(_suspend_cond), &(_suspend_mutex));
    }
  else
    pthread_mutex_unlock(&_suspend_mutex);

  return (_status & thread_t::terminated) != thread_t::running;
  }

uint32_t kotuku::thread_handle_t::thread_exit_code()
  {
  return _thread_exit_code;
  }

void kotuku::thread_handle_t::set_priority(uint8_t)
  {
  }

uint8_t kotuku::thread_handle_t::get_priority()
  {
  return 0;
  }

unsigned int kotuku::thread_handle_t::thread_id()
  {
  return 0;
  }

kotuku::thread_t::status_t kotuku::thread_handle_t::status()
  {
  return _status;
  }

handle_t kotuku::linux_hal_t::thread_create(thread_func pfn,
    size_t stack_size, void *param, unsigned int *id)
  {
  thread_handle_t *thread = new thread_handle_t(pfn, stack_size, param);
  if(id != 0)
    *id = thread->thread_id();

  return thread;
  }

void kotuku::linux_hal_t::thread_set_priority(handle_t h, uint8_t p)
  {
  reinterpret_cast<thread_handle_t *>(h)->set_priority(p);
  }

uint8_t kotuku::linux_hal_t::thread_get_priority(handle_t h)
  {
  return reinterpret_cast<thread_handle_t *>(h)->get_priority();
  }

void kotuku::linux_hal_t::thread_suspend(handle_t h)
  {
  reinterpret_cast<thread_handle_t *>(h)->suspend();
  }

void kotuku::linux_hal_t::thread_resume(handle_t h)
  {
  reinterpret_cast<thread_handle_t *>(h)->resume();
  }

void kotuku::linux_hal_t::thread_sleep(uint32_t n)
  {
  usleep(n * 1000);
  }

bool kotuku::linux_hal_t::thread_wait(handle_t h, uint32_t time_ms)
  {
  return false;
  }

bool kotuku::linux_hal_t::thread_yield(handle_t h)
  {
  reinterpret_cast<thread_handle_t *>(h)->yield();
  }

uint32_t kotuku::linux_hal_t::thread_exit_code(handle_t h)
  {
  return 0;
  }

kotuku::thread_t::status_t kotuku::linux_hal_t::thread_status(handle_t h)
  {
  return reinterpret_cast<thread_handle_t *>(h)->status();
  }

void kotuku::linux_hal_t::thread_close(handle_t h)
  {
  delete reinterpret_cast<thread_handle_t *>(h);
  }

void kotuku::linux_hal_t::thread_terminate(handle_t h, uint32_t term_code)
  {
  pthread_t thread = reinterpret_cast<pthread_t>(h);
  }

unsigned int kotuku::linux_hal_t::thread_current_id(handle_t h)
  {
  return reinterpret_cast<thread_handle_t *>(h)->thread_id();
  }

kotuku::screen_t *kotuku::linux_hal_t::screen_create(screen_t *h, const extent_t &sz)
  {
  return h->create_canvas(h, sz);
  }

kotuku::screen_t *kotuku::linux_hal_t::screen_create(screen_t *h,
    const bitmap_t &bm)
  {
  return h->create_canvas(h, bm);
  }

kotuku::screen_t *kotuku::linux_hal_t::screen_create(screen_t *h, const rect_t &r)
  {
  return h->create_canvas(h, r);
  }

void kotuku::linux_hal_t::screen_close(window_t *h)
  {
  delete h;
  }
