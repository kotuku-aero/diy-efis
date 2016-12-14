#ifndef __canfly_h__
#define __canfly_h__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __ION__
// Embedded solution

#define _TIME_T_DEFINED
#define _TIME_T

// Kotuku specif pic32mz embedded kernel
#include <ion.h>

#undef abs
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#else

#include <stdint.h>

#if _MSC_VER
#pragma warning(disable : 4355 4996)
#pragma warning(disable : 4312 4311 4267 4244)

#if !defined(STRICT)
#define STRICT
#endif

#define OEMRESOURCE
#define _WIN32_WINNT 0x0401
// Windows version
#include <windows.h>

#undef handle_t
#undef DEFINE_GUID

#ifdef RGB
#undef RGB
#endif

#ifdef DEFINE_TS
#undef DEFINE_TS
#endif


typedef int ptrdiff_t;
//typedef int mbstate_t;
#define snprintf _snprintf
#define strcasecmp _stricmp
#include "../can-aerospace/can_msg.h"
#include <time.h>

typedef RECT rect_t;
typedef POINT point_t;
typedef SIZE extent_t;
typedef GUID guid_t;
typedef LONG gdi_dim_t;

#elif __linux__
#include "../can-aerospace/can_msg.h"
#include <time.h>
typedef int32_t gdi_dim_t;

// these are the internal types
typedef struct _point_t
  {
  gdi_dim_t x;
  gdi_dim_t y;
  } point_t;

typedef struct _extent_t
  {
  gdi_dim_t dx;
  gdi_dim_t dy;
  } extent_t;

typedef struct _rect_t
  {
  gdi_dim_t left;
  gdi_dim_t top;
  gdi_dim_t right;
  gdi_dim_t bottom;
  } rect_t;

// definition compatible with the MSVC GUID generator
// {187DB158-69BC-485a-BB83-5155BB72F839}
// DEFINE_GUID(<<name>>, 
// 0x187db158, 0x69bc, 0x485a, 0xbb, 0x83, 0x51, 0x55, 0xbb, 0x72, 0xf8, 0x39);
#define DEFINE_GUID(T, l1, w1, w2, b0, b1, b2, b3, b4, b5, b6, b7) \
const guid_t (T) (l1, w1, w2, b0, b1, b2, b3, b4, b5, b6, b7);

#endif
// these types are needed to compile.  ION has similar definitions
typedef int32_t result_t;
typedef void *handle_t;
typedef uint8_t byte_t;

enum {
  s_ok = 0,
  s_false = -1,
  e_invalid_handle = 0x80010001,
  e_not_implemented = 0x80010002,
  e_bad_pointer = 0x80010003,
  e_bad_parameter = 0x80010004,
  e_path_not_found = 0x80010004,
  e_operation_pending = 0x80030001,
  e_operation_cancelled = 0x80030002,
  e_invalid_operation = 0x80030003,
  e_buffer_too_small = 0x80030004,
  e_ioctl_too_small = 0x80030005,
  e_ioctl_version_error = 0x80030006,
  e_device_busy = 0x80030007,
  e_generic_error = 0x80030008,
  e_timeout_error = 0x80030009,
  e_no_space = 0x8003000A,
  e_not_enough_memory = 0x800300B,
  };

typedef uint32_t color_t;

#ifdef RGB
#undef RGB
#endif

#define RGB(r,g,b) ((color_t)((((uint8_t)r)<<16)|(((uint8_t)g)<<8)|((uint8_t)b)))

inline color_t rgb(uint8_t red, uint8_t green, uint8_t blue)
  {
  return (((color_t)red) << 16) | (((color_t)green) << 8) | ((color_t)blue);
  }

inline uint8_t blue(color_t c)
  {
  return (uint8_t)(c & 255);
  }

inline uint8_t red(color_t c)
  {
  return (uint8_t)((c >> 16) & 255);
  }

inline uint8_t green(color_t c)
  {
  return (uint8_t)((c >> 8) & 255);
  }

inline uint8_t alpha(color_t c)
  {
  return (uint8_t)((c >> 24) & 255);
  }

#define color_white RGB(255, 255, 255)
#define color_black RGB(0, 0, 0)
#define color_gray RGB(128, 128, 128)
#define color_light_gray RGB(192, 192, 192)
#define color_dark_gray RGB(64, 64, 64)
#define color_red RGB(255, 0, 0)
#define color_pink RGB(255, 128, 128)
#define color_blue RGB(0, 0, 255)
#define color_green RGB(0, 255, 0)
#define color_lightgreen RGB(0, 192, 0)
#define color_yellow RGB(255, 255, 64)
#define color_magenta RGB(255, 0, 255)
#define color_cyan RGB(0, 255, 255)
#define color_pale_yellow RGB(255, 255, 208)
#define color_light_yellow RGB(255, 255, 128)
#define color_lime_green RGB(192, 220, 192)
#define color_teal RGB(64, 128, 128)
#define color_dark_green RGB(0, 128, 0)
#define color_maroon RGB(128, 0, 0)
#define color_purple RGB(128, 0, 128)
#define color_orange RGB(255, 192, 64)
#define color_khaki RGB(167, 151, 107)
#define color_olive RGB(128, 128, 0)
#define color_brown RGB(192, 128, 32)
#define color_navy RGB(0, 64, 128)
#define color_light_blue RGB(128, 128, 255)
#define color_faded_blue RGB(192, 192, 255)
#define color_lightgrey RGB(192, 192, 192)
#define color_darkgrey RGB(64, 64, 64)
#define color_paleyellow RGB(255, 255, 208)
#define color_lightyellow RGB(255, 255, 128)
#define color_limegreen RGB(192, 220, 192)
#define color_darkgreen RGB(0, 128, 0)
#define color_lightblue RGB(128, 128, 255)
#define color_fadedblue RGB(192, 192, 255)
#define color_hollow 0xFFFFFFFF

typedef struct _bitmap_t
  {
  size_t version; // sizeof(bitmap_t)
  gdi_dim_t bitmap_width; // width of the bitmap_t in pixels
  gdi_dim_t bitmap_height; // height of the bitmap_t in pixels
  const color_t *pixels;  // array of pixels in color format (rgba)
  } bitmap_t;

typedef struct _metrics_t
  {
  size_t screen_x;          // number of pixels in the x direction
  size_t screen_y;          // number of pixels in the y direction
  size_t bits_per_pixel;    // number of bits in a pixel
  } metrics_t;

typedef enum _pen_style
  {
  ps_solid,
  ps_dash,
  ps_dot,
  ps_dash_dot,
  ps_dash_dot_dot,
  ps_null
  } pen_style;

typedef struct _pen_t
  {
  color_t color;
  size_t width;
  pen_style style;
  } pen_t;

typedef struct _font_t
  {
  size_t version; // sizeof(font_t)
  size_t bitmap_height; // height of the bitmap
  char first_char; // first character in this font
  char last_char; // last character in this font
  char default_char; // character to use as the default
  const uint8_t *bitmap_pointer; // pointer to the bitmap for all characters
                                 // this is written as columns so if the char width > 8 pixels
                                 // the _next column is + bitmap_height
  const short *char_table; // pointer to a set of offsets for each character
                           // char_table[ch * 2] = width
                           // char_table[(ch * 2) + 1] = offset to first mask uint8_t
  } font_t;

////////////////////////////////////////////////////////////////////////////
typedef uint32_t text_flags;
enum {
  eto_opaque = 0x00000002,
  eto_clipped = 0x00000004
  };

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

inline bool failed(result_t sc)
  {
  return (sc & 0x80000000) != 0;
  }

inline bool succeeded(result_t sc)
  {
  return (sc & 0x80000000) == 0;
  }

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

  extern long interlocked_increment(volatile long &);
  extern long interlocked_decrement(volatile long &);

#endif

