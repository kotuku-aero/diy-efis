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
#ifndef __ion_types_h__
#define __ion_types_h__

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t result_t;
typedef void *handle_t;

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
  e_access = -1, /* Search permission is denied for a directory in a file's path prefix. */
  e_busy = -2, /* The device or resource is in use. */
  e_exist = -3, /* The named file already exists. */
  e_inval = -4, /* Invalid argument. */
  e_noent = -5, /* No such file or directory */
  e_io = -6, /* I/O error */
  e_isdir = -7, /* Attempt to open a directory for writing or to rename a file to be a directory. */
  e_name_too_long = -8, /* Length of a filename string exceeds PATH_MAX and _POSIX_NO_TRUNC is in effect. */
  e_path = -9, /* invalid path */
  e_nfile = -10, /* Too many files are currently open in the system. */
  e_ndir = -11, /* Too many directories are currently open in the system. */
  e_nodev = -12, /* No such device. Attempt to perform an inappropriate function to a device. */
  e_nomem = -13, /* No memory available. */
  e_nospc = -14, /* No space left on disk. */
  e_notdir = -15, /* A component of the specified pathname was not a directory when a directory was expected. */
  e_notempty = -16, /* Attempt to delete or rename a non-empty directory. */
  e_perm = -17, /* Operation is not permitted. */
  e_rofs = -18, /* Read-only file system. */
  e_xdev = -19, /* Attempt to link a file to another file system. in rename() (volume is different) */
  e_badf = -20, /* Invalid file descriptor. */
  e_spipe = -21, /* Illegal seek */
  e_attach = -22, /* attached mass-storage */
  e_eject = -23, /* Media was ejected. */
  e_cfs = -24, /* File system was corruped. */
  e_cfat = -25, /* FAT table was corrupted. */
  e_afat = -26, /* FAT table invalid access. */
  e_fcache = -27, /* FAT cache problem */
  e_lcache = -28, /* LIM cache problem */
  e_eos = -29, /* The searching reached to the end of entry. use only internally */
  e_outof = -30, /* out of access */
  e_noinit = -31, /* File sytem was not initialized */
  e_nomnt = -32, /* File system was not mounted */
  e_nofmt = -33, /* File system was not formatted */
  e_mbr = -34, /* MBR was not formatted */
  e_port = -35, /* File system become miss porting */
  e_osd = -36, /* OS problerms */
  e_overw = -37, /* Internel datas were overwritten */
  e_wbf = -38, /* Write buffer flush fail */
  e_plo = -39, /* Power Loss occurred(Only uses internally) */
  e_unknown = -99,
  e_errno = -100
  };

typedef int32_t gdi_dim_t;

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

typedef uint32_t color_t;

#define RGB(r,g,b) ((((uint8_t)r)<<16)|(((uint8_t)g)<<8)|((uint8_t)b))

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
#define color_hollow 0xffffffff
#define color_lightgrey RGB(192, 192, 192)
#define color_darkgrey RGB(64, 64, 64)
#define color_paleyellow RGB(255, 255, 208)
#define color_lightyellow RGB(255, 255, 128)
#define color_limegreen RGB(192, 220, 192)
#define color_darkgreen RGB(0, 128, 0)
#define color_lightblue RGB(128, 128, 255)
#define color_fadedblue RGB(192, 192, 255)
  
typedef struct _bitmap_t
  {
  size_t version; // sizeof(bitmap_t)
  size_t bpp; // bits per pixel
  const color_t *palette; // 0 if not color mapped
  gdi_dim_t bitmap_width; // width of the bitmap_t in pixels
  gdi_dim_t bitmap_height; // height of the bitmap_t in pixels
  const color_t *pixels;  // array of pixels in color format (rgba)
  void *user_data;        // user data placed here.
  } bitmap_t;
  
typedef struct _metrics_t
  {
  size_t screen_x;          // number of pixels in the x direction
  size_t screen_y;          // number of pixels in the y direction
  size_t row_size;          // number of pixels for a row.
  size_t bits_per_pixel;    // number of bits in a pixel
  const color_t *palette;     // optional palette to use for color lookups
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
  
///////////////////////////////////////////////////////////////////////////////
//
//  Canvas operations
//
typedef enum _raster_operation {
  rop_r2_black = 1, // 0
  rop_r2_notmergepen = 2, // dpon
  rop_r2_masknotpen = 3, // dpna
  rop_r2_notcopypen = 4, // pn
  rop_r2_maskpennot = 5, // pdna
  rop_r2_not = 6, // dn
  rop_r2_xorpen = 7, // dpx
  rop_r2_notmaskpen = 8, // dpan
  rop_r2_maskpen = 9, // dpa
  rop_r2_notxorpen = 10, // dpxn
  rop_r2_nop = 11, // d
  rop_r2_mergenotpen = 12, // dpno
  rop_r2_copypen = 13, // p
  rop_r2_mergepennot = 14, // pdno
  rop_r2_mergepen = 15, // dpo
  rop_r2_white = 16, // 1
  rop_r2_last = 16,
  rop_srccopy = 0x00cc0020, // dest = source
  rop_srcpaint = 0x00ee0086, // dest = source or dest
  rop_srcand = 0x008800c6, // dest = source and dest
  rop_srcinvert = 0x00660046, // dest = source xor dest
  rop_srcerase = 0x00440328, // dest = source and (not dest)
  rop_notsrccopy = 0x00330008, // dest = (not source)
  rop_notsrcerase = 0x001100a6, // dest = (not src) and (not dest)
  rop_mergecopy = 0x00c000ca, // dest = (source and pattern)
  rop_mergepaint = 0x00bb0226, // dest = (not source) or dest
  rop_patcopy = 0x00f00021,  // dest = pattern
  rop_patpaint = 0x00fb0a09, // dest = dpsnoo
  rop_patinvert = 0x005a0049, // dest = pattern xor dest
  rop_dstinvert = 0x00550009, // dest = (not dest)
  rop_blackness = 0x00000042, // dest = black
  rop_whiteness = 0x00ff0062 // dest = white
  } raster_operation;
  
////////////////////////////////////////////////////////////////////////////
typedef uint32_t text_flags;
enum {
  eto_opaque = 0x00000002,
  eto_clipped = 0x00000004
  };

#ifdef __cplusplus
}
#endif

#endif
