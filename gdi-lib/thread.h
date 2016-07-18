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
#ifndef __thread_h__
#define __thread_h__

#include "stddef.h"

namespace kotuku {
//
// class event_t
// ~~~~~ ~~~~~~~~~~~~~~~
//
  class event_t
    {
  public:
    event_t(bool manual_reset = false, bool initial_state = false);

    static const uint32_t no_wait = 0;
    static const uint32_t no_limit = (uint32_t) -1;

    void set();

    class lock_t
      {
    public:
      // timeout is in ms
      lock_t(const event_t&, uint32_t timeout = no_limit);
      ~lock_t();

      bool was_aquired() const; // see if really Aquired, or just timed-out
    private:
      bool _was_aquired;
      };
    friend class lock_t;

    handle_t handle()
      {
      return _handle;
      }
  private:
    event_t(const event_t &);
    event_t& operator =(const event_t &);
    handle_t _handle;
    };

//
// class critical_section_t
// ~~~~~ ~~~~~~~~~~~~~~~~
// lightweight intra-process thread_t synchronization. can only be used with
// other critical sections, and only within the same process.
//
  class critical_section_t
    {
  public:
    critical_section_t();
    ~critical_section_t();

    class lock_t
      {
    public:
      lock_t(const critical_section_t&);
      ~lock_t();
    private:
      const critical_section_t &_cs;
      };
    friend class lock_t;

  private:
    critical_section_t(const critical_section_t&);
    const critical_section_t& operator =(const critical_section_t&);

    handle_t _handle;
    };

//
// class thread_t
// ~~~~~ ~~~~~~~
//
// thread_t public interface
// ~~~~~~~~~~~~~~~~~~~~~~~~
//   suspend();       suspends execution of the thread_t.
//   resume();        resumes execution of a suspended thread_t.
//
//   terminate();      sets an internal flag that indicates that the 
//               thread_t should exit. the derived class can check
//               the state of this flag by calling
//               should_terminate().
//
//   wait_for_exit(uint32_t timeout = no_limit);
//               blocks the calling thread_t until the internal
//               thread_t exits or until the time specified by
//               timeout, in milliseconds,expires. a timeout of
//               no_limit says to wait indefinitely.
//
//   terminate_and_wait(uint32_t timeout = no_limit);
//               combines the behavior of terminate() and
//               wait_for_exit(). sets an internal flag that
//               indicates that the thread_t should exit and blocks
//               the calling thread_t until the internal thread_t
//               exits or until the time specified by timeout, in
//               milliseconds, expires. a timeout of no_limit says
//               to wait indefinitely.
//
//   get_status();      gets the current status_t of the thread_t.
//               see thread_t::status_t for possible values.
//
//   priority();     gets the priority of the thread_t.
//   priority();     sets the priority of the thread_t.
//
//   enum status_t;      identifies the states that the class can be in.
//
//     Created       the class has been Created but the thread_t has 
//               not been started.
//     Running       the thread_t is Running.
//
//     Suspended      the thread_t has been Suspended.
//
//     Finished      the thread_t has Finished execution.
//
//     Invalid       the object is Invalid. currently this happens
//               only when the operating system is unable to
//               start the thread_t.
//
//
// thread_t protected interface
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   thread_t();       creates an object of type thread_t.
//   virtual ~thread_t();   destroys the object.
//
//   const thread_t& operator =(const thread_t&);
//               the target object must be in either the
//               Created state or the Finished state. if
//               so, puts the object into the Created
//               state. if the object is not in either the
//               Created state or the Finished state it
//               is an error and an exception will be
//               thrown.
//
//   thread_t(const thread_t&);
//               pts the object into the Created
//               state, just like the default constructor.
//               does not copy any of the internal details
//               of the thread_t being copied.
//
//   virtual uint32_t run() = 0;
//               the function that does the work. calling
//               start() creates a thread_t that begins
//               executing run() with the 'this' pointer
//               pointing to the thread_t-based object.
//
//   int should_terminate() const;
//               returns a non-zero value to indicate
//               that terminate() or terminate_and_wait()
//               has been called. if this capability is
//               being used, the thread_t should call
//               should_terminate() regularly, and if it
//               returns a non-zero value the thread_t
//               finish its processing and exit.
//
// examplehandle_t
// ~~~~~~~
//   class timerthread : public thread_t
//   {
//    public:
//     timerthread() : count(0) {}
//    private:
//     int run();
//     int count;
//   };
//
//   int timerthread::run()
//   {
//    // loop 10 times
//    while(count++ < 10) {
//     sleep(1000);  // delay 1 second
//     cout << "iteration " << count << endl;
//    }
//    return 0;
//   }
//
//   int main()
//   {
//    timerthread timer;
//    timer.start();
//    sleep(20000);   // delay 20 seconds
//    return 0;
//   }
//
// internal states
// ~~~~~~~~~~~~~~~
// Created:  the object has been Created but its thread_t has not been
//       started. the only valid transition from this state is
//       to Running, which happens on a call to start(). in
//       particular, a call to suspend() or resume() when the
//       object is in this state is an error and will throw an
//       exception.
//
// Running:  the thread_t has been started successfully. there are two
//       transitions from this state:
//
//         when the user calls suspend() the object moves into
//         the Suspended state.
//
//         when the thread_t exits the object moves into the
//         Finished state.
//
//       calling resume() on an object that is in the Running
//       state is an error and will throw an exception.
//
// Suspended: the thread_t has been Suspended by the user. subsequent
//       calls to suspend() nest, so there must be as many calls
//       to resume() as there were to suspend() before the thread_t
//       actually resumes execution.
//
// Finished:  the thread_t has Finished executing. there are no valid
//       transitions out of this state. this is the only state
//       from which it is legal to invoke the destructor for the
//       object. invoking the destructor when the object is in
//       any other state is an error and will throw an exception.
//

  class thread_t
    {
  public:
    static const uint32_t no_limit = (uint32_t) -1;

    enum status_t
      {
      running = 0,
      sleeping,
      suspended,
      blocked,
      finished,
      creating,
      terminated = 16
      };

    void suspend();
    void resume();

    virtual void terminate();
    uint32_t wait_for_exit(uint32_t timeout = no_limit);
    uint32_t terminate_and_wait(uint32_t timeout = no_limit,
        uint32_t terminate_code = (uint32_t) -1);

    status_t get_status() const;

    // you cannot assign a priority below 8
    typedef uint8_t priority_t;
    static const priority_t lowest = 8;
    static const priority_t normal = 16;
    static const priority_t highest = 31;
    static const priority_t num_priorities = 32;

    // return the priority assigned to this thread_t
    priority_t priority() const;
    // change the priority, attempts to assign < 256 will fail
    priority_t priority(priority_t);

    unsigned int id() const;        // returns the thread_id of this thread_t
  protected:
    typedef uint32_t (*threadfunc)(void *p_this);

    thread_t(size_t size, void *p_this, threadfunc tfun);
    virtual ~thread_t();

    void sleep(uint32_t timems);

    // returns true if the thread should terminate.
    // also yields if suspend has been called.
    // place this in the run loop:
    //
    // while(!should_terminate())
    //  {
    //  ... do something ...
    //  }
    bool should_terminate() const;

  private:
    thread_t();
    thread_t(const thread_t&);
    const thread_t& operator =(const thread_t&);

    handle_t _handle;
    };

// utility functions for thread status
  inline thread_t::status_t operator |(thread_t::status_t l,
      thread_t::status_t r)
    {
    return (thread_t::status_t) (((int) l) | ((int) r));
    }

  inline thread_t::status_t operator &(thread_t::status_t l,
      thread_t::status_t r)
    {
    return (thread_t::status_t) (((int) l) & ((int) r));
    }

  inline thread_t::status_t operator ~(thread_t::status_t l)
    {
    return (thread_t::status_t) (~((int) l));
    }

  inline bool event_t::lock_t::was_aquired() const
    {
    return _was_aquired;
    }
  };

#endif
