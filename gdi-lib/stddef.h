#ifndef __stddef_h__
#define __stddef_h__

#ifdef _MSC_VER
#pragma warning(disable : 4355 4996)
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../ion/types.h"

#ifdef _MSC_VER
// Windows version

typedef int ptrdiff_t;
//typedef int mbstate_t;
#define snprintf _snprintf
#define strcasecmp _stricmp
#include "../can-aerospace/can_msg.h"
#include <time.h>
#elif __linux__
#include "../can-aerospace/can_msg.h"
#include <time.h>
#else
// Embedded solution

#define _TIME_T_DEFINED
#define _TIME_T

#include "../ion/ion.h"

#undef abs
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

#ifdef UNDER_CE
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace std
  {

  template<typename _Ty> inline _Ty min(_Ty l, _Ty r)
    {
    return l > r ? r : l;
    }

  template<typename _Ty> inline _Ty max(_Ty l, _Ty r)
    {
    return l < r ? r : l;
    }

  template<typename _Ty> inline _Ty abs(_Ty v)
    {
    return ::abs(v);
    }
  }

#define _TRACE
#include "stdint.h"
#endif

#include <string>
#include <vector>

// this sets up the debug flag
#if defined(_DEBUG) | defined(DEBUG)
const bool __debug_flag = true;
#else
const bool __debug_flag = false;
#endif

#if defined(_DEBUG) | defined(DEBUG) | defined(_TRACE) | defined(TRACE)
const bool __tron = true;
#else
const bool __tron = false;
#endif

// this sets the endianess of the system and provides conversion functions
#if defined(BIGENDIAN)

inline uint16_t to_big_endian(uint16_t value)
  {
  return value;
  }

inline unsigned int to_big_endian(unsigned int value)
  {
  return value;
  }

inline uint32_t to_big_endian(uint32_t value)
  {
  return value;
  }

inline uint16_t to_local_endian(uint16_t value)
  {
  return value;
  }

inline unsigned int to_local_endian(unsigned int value)
  {
  return value;
  }

inline uint32_t to_local_endian(uint32_t value)
  {
  return value;
  }

const bool __big_endian = true;
#else

inline uint16_t to_big_endian(uint16_t value)
  {
  return (value << 8) | (value >> 8);
  }

inline uint32_t to_big_endian(uint32_t value)
  {
  return (to_big_endian((uint16_t) value) << 16)
    | to_big_endian((uint16_t) (value >> 16));
  }

inline uint16_t to_local_endian(uint16_t value)
  {
  return (value << 8) | (value >> 8);
  }

inline uint32_t to_local_endian(uint32_t value)
  {
  return (to_big_endian((uint16_t) value) << 16)
    | to_big_endian((uint16_t) (value >> 16));
  }

const bool __big_endian = false;
#endif

#include "errors.h"

  // These are the std extension definitions
  //////////////////////////////////////////////////////////////////////////////
  //

  inline long make_long(short lo, short hi)
    {
    return long(lo) | (long(hi) << 16);
    }

  inline short lo_word(long l)
    {
    return short(l);
    }

  inline short hi_word(long l)
    {
    return short(l >> 16);
    }

  //////////////////////////////////////////////////////////////////////////////
  // thread safe increment and decrement operators
  extern long __interlocked_increment(volatile long &);
  extern long __interlocked_decrement(volatile long &);

  //////////////////////////////////////////////////////////////////////////////
  // this is the base class of most handles passed to the i/o system
  typedef void *handle_t;

  //////////////////////////////////////////////////////////////////////////////
  // General trace and debug routines
  //
  // these routines write to cerr that is assigned to the windows debugger output
  // stream if running in debug mode.

  extern void __assert(const char *file, int line, bool check);
  extern void __trace(int level, const char *msg, char *params);

  extern result_t __is_bad_read_pointer(const void *ptr, size_t size);
  extern result_t __is_bad_write_pointer(void *ptr, size_t size);

  typedef uint8_t byte_t;

  template<typename _Ty>
  class object_ptr_t
    {
  public:

    object_ptr_t(_Ty *_p = 0) :
    _ptr(_p)
      {
      add_ref();
      }

    ~object_ptr_t()
      {
      release();
      }

    object_ptr_t(const object_ptr_t<_Ty> &_p) :
    _ptr(const_cast<object_ptr_t<_Ty> &> (_p).get())
      {
      add_ref();
      }

    const object_ptr_t<_Ty> &operator=(_Ty *_p)
      {
      if (_p != _ptr)
        {
        release();
        _ptr = _p;
        add_ref();
        }

      return *this;
      }

    const object_ptr_t<_Ty> &operator=(const object_ptr_t<_Ty> &_p)
      {
      if (_ptr != _p._ptr)
        {
        release();
        _ptr = const_cast<object_ptr_t<_Ty> &> (_p)._ptr;
        add_ref();
        }

      return *this;
      }

    bool operator<(const object_ptr_t<_Ty> &p) const
      {
      if (_ptr == 0 || p._ptr == 0)
        return true;

      return *_ptr < *p._ptr;
      }

    /*
     _Ty **operator &()
     {
     release();
     return &_ptr;
     }
     */

    _Ty *operator->()
      {
      return _ptr;
      }

    const _Ty *operator->() const
      {
      return _ptr;
      }

    _Ty &operator*()
      {
      return *_ptr;
      }

    const _Ty &operator*() const
      {
      return *_ptr;
      }

    operator _Ty *()
      {
      return _ptr;
      }

    operator _Ty *() const
      {
      return const_cast<_Ty *> (_ptr);
      }

    _Ty *get()
      {
      return _ptr;
      }

    const _Ty *get() const
      {
      return _ptr;
      }

    operator bool() const
      {
      return _ptr != 0;
      }

    bool operator!() const
      {
      return _ptr == 0;
      }
  private:

    long release()
      {
      _Ty *p = _ptr;
      _ptr = 0;
      if (p)
        return p->release();

      return 0;
      }

    void add_ref()
      {
      if (_ptr)
        _ptr->add_ref();
      }
    _Ty *_ptr;
      };

  extern long interlocked_increment(volatile long &);
  extern long interlocked_decrement(volatile long &);

  class object_t
    {
  public:

    virtual ~object_t()
      {
      }
    // reference counting functions

    virtual long add_ref()
      {
      return interlocked_increment(_refcnt);
      }

    virtual long release()
      {
      long refcnt = interlocked_decrement(_refcnt);
      if (refcnt == 0)
        delete this;

      return refcnt;
      }

  private:
    volatile long _refcnt;
      };

  struct guid_t
    {
    uint32_t l1;
    uint16_t w1;
    uint16_t w2;
    uint8_t b[8];

    guid_t();
    guid_t(uint32_t, uint16_t, uint16_t, uint8_t,
           uint8_t, uint8_t, uint8_t, uint8_t,
           uint8_t, uint8_t, uint8_t);
    guid_t(const guid_t &);
    guid_t &operator=(const guid_t &);
      };

  extern const guid_t null_guid;

  inline guid_t::guid_t()
    {
    }

  inline guid_t::guid_t(uint32_t _l1, uint16_t _w1,
                        uint16_t _w2, uint8_t _b0, uint8_t _b1,
                        uint8_t _b2, uint8_t _b3, uint8_t _b4,
                        uint8_t _b5, uint8_t _b6, uint8_t _b7)
    {
    l1 = _l1;
    w1 = _w1;
    w2 = _w2;
    b[0] = _b0;
    b[1] = _b1;
    b[2] = _b2;
    b[3] = _b3;
    b[4] = _b4;
    b[5] = _b5;
    b[6] = _b6;
    b[7] = _b7;
    }

  inline guid_t::guid_t(const guid_t &t)
    {
    *this = t;
    }

  inline guid_t &guid_t::operator=(const guid_t &t)
    {
    memcpy(this, &t, sizeof (guid_t));
    return *this;
    }

  inline bool operator==(const guid_t &g1, const guid_t &g2)
    {
    return memcmp(&g1, &g2, sizeof (guid_t)) == 0;
    }

  inline bool operator!=(const guid_t &g1, const guid_t &g2)
    {
    return memcmp(&g1, &g2, sizeof (guid_t)) != 0;
    }

  inline bool operator<(const guid_t &g1, const guid_t &g2)
    {
    return memcmp(&g1, &g2, sizeof (guid_t)) < 0;
    }

  // definition compatible with the MSVC GUID generator
  // {187DB158-69BC-485a-BB83-5155BB72F839}
  // DEFINE_GUID(<<name>>, 
  // 0x187db158, 0x69bc, 0x485a, 0xbb, 0x83, 0x51, 0x55, 0xbb, 0x72, 0xf8, 0x39);
#define DEFINE_GUID(T, l1, w1, w2, b0, b1, b2, b3, b4, b5, b6, b7) \
const guid_t (T) (l1, w1, w2, b0, b1, b2, b3, b4, b5, b6, b7);

#endif

