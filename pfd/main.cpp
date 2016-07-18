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
#include "../widgets/watchdog.h"
#include "../widgets/pfd_application.h"
#include "../widgets/layout_window.h"
#include "../linux-hal/linux_hal.h"
#include "../widgets/layout_window.h"
#include "../widgets/notification_window.h"
#include "../widgets/menu_window.h"
#include "../gdi-lib/raster_screen.h"
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdint.h>
#include <string>
#include <string.h>

static kotuku::layout_window_t *root_window;
static kotuku::menu_window_t *menu_window;
static kotuku::notification_window_t *alert_window;
static kotuku::watchdog_t *watchdog;
static kotuku::pfd_application_t *_the_app = 0;
static kotuku::linux_hal_t *hal_impl = 0;

static const char *hal_section = "linux-hal";

namespace kotuku
  {
class memory_screen_t: public raster_screen_t
  {
public:
  memory_screen_t(color_t *buffer, bool owns, const screen_metrics_t &metrics);
  virtual ~memory_screen_t();

  // canvas implementation routines.
  virtual screen_t *create_canvas(screen_t *, const extent_t &);
  virtual screen_t *create_canvas(screen_t *, const bitmap_t &);
  virtual screen_t *create_canvas(screen_t *, const rect_t &);

  virtual void invalidate_rect(const rect_t &);
protected:
  color_t *_buffer;
  virtual uint8_t *point_to_address(const point_t &pt);
  virtual const uint8_t *point_to_address(const point_t &pt) const;
private:
  bool _owns;
  };

class framebuffer_screen_t: public memory_screen_t, public thread_t
  {
public:
  /**
   * Create a new screen
   * @param fbfd      File to memory map
   * @param metrics   Details of the framebuffer
   */
  framebuffer_screen_t(hal_t *hal, const screen_metrics_t &metrics);
  virtual ~framebuffer_screen_t();

  layout_window_t *root_window() const
    {
    return _root_window;
    }
  void root_window(layout_window_t *window)
    {
    _root_window = window;
    }

  virtual void display_mode(int);
  virtual int display_mode() const;

  void vsync();
protected:
  virtual point_t &apply_rotation(const point_t &src, point_t &dst) const;
  virtual uint8_t *point_to_address(const point_t &pt);
  virtual const uint8_t *point_to_address(const point_t &pt) const;
  virtual gdi_dim_t pixel_increment() const;
  virtual color_t get_pixel(const uint8_t *src) const;
  virtual void set_pixel(uint8_t *dest, color_t color) const;
private:

  enum framebuffer_draw_state
    {
    fbds_forceredraw,
    fbds_checkredraw,
    fbds_draw
    };
  framebuffer_draw_state _state = fbds_forceredraw;

  static uint32_t do_run(void *);
  uint32_t run();

  layout_window_t *_root_window;
  int _display_mode;
  struct fb_var_screeninfo _vinfo;
  struct fb_fix_screeninfo _finfo;

  gdi_dim_t _pixel_increment;
  uint32_t _red_mask;
  uint32_t _blue_mask;
  uint32_t _green_mask;
  size_t _red_shift;
  size_t _blue_shift;
  size_t _green_shift;

  // if we are a double buffered screen this this will hold the
  // primary display
  uint8_t *_display_buffer;
  size_t _display_buffer_length;
  int _fbfd_display;          // actual framebuffer
  };

inline framebuffer_screen_t *as_screen_handle(screen_t *h)
  {
  return reinterpret_cast<kotuku::framebuffer_screen_t *>(h);
  }

static framebuffer_screen_t *root_screen = 0;

result_t do_create_screen(hal_t *hal, screen_type_t screen_type, window_t **new_window)
  {
  int screen_x;
  int screen_y;

  if(root_screen == 0)
    {
    if(failed(hal->get_config_value(hal_section, "screen-x", screen_x)))
      screen_x = 640;

    if(failed(hal->get_config_value(hal_section, "screen-y", screen_y)))
      screen_y = 480;

    root_screen = new framebuffer_screen_t(hal, screen_metrics_t(screen_x, screen_y, 32));

    root_window = new layout_window_t(root_screen);
    root_screen->root_window(root_window);
    }

  switch(screen_type)
    {
    case st_root :
     *new_window = root_window;
      break;
    case st_menu :
      if(menu_window == 0)
        {
        int menu_window_x;
        int menu_window_y;
        int menu_window_width;
        int menu_window_height;

        if(failed(hal->get_config_value(hal_section, "menu-window-x", menu_window_x)))
          menu_window_x = 0;

        if(failed(hal->get_config_value(hal_section, "menu-window-y", menu_window_y)))
          menu_window_y = 320;

        if(failed(hal->get_config_value(hal_section, "menu-window-width", menu_window_width)))
          menu_window_width = 480;

        if(failed(hal->get_config_value(hal_section, "menu-window-width", menu_window_height)))
          menu_window_height = 320;


        // our window is an overlay window, using the z-order of the root window
        menu_window = new menu_window_t(*root_window,
            kotuku::rect_t(kotuku::point_t((gdi_dim_t)menu_window_x, (gdi_dim_t)menu_window_y),
                kotuku::extent_t((gdi_dim_t)menu_window_width, (gdi_dim_t)menu_window_height)),
                "main-menu");
        }

      *new_window = menu_window;
      break;
    case st_notifications :
      if(alert_window == 0)
        {
        int alert_window_x;
        int alert_window_y;
        int alert_window_width;
        int alert_window_height;

        if(failed(hal->get_config_value(hal_section, "alert-window-x", alert_window_x)))
          alert_window_x = 0;

        if(failed(hal->get_config_value(hal_section, "alert-window-y", alert_window_y)))
          alert_window_y = 320;

        if(failed(hal->get_config_value(hal_section, "alert-window-width", alert_window_width)))
          alert_window_width = 480;

        if(failed(hal->get_config_value(hal_section, "alert-window-width", alert_window_height)))
          alert_window_height = 320;


        // our window is an overlay window, using the z-order of the root window
        alert_window = new notification_window_t(*root_window,
            kotuku::rect_t(kotuku::point_t((gdi_dim_t)alert_window_x, (gdi_dim_t)alert_window_y),
                kotuku::extent_t((gdi_dim_t)alert_window_width, (gdi_dim_t)alert_window_height)));
        }

      *new_window = alert_window;
      break;
    }


  return s_ok;
  }
  };

kotuku::application_t *kotuku::the_app()
  {
  return _the_app;
  }

kotuku::hal_t *kotuku::the_hal()
  {
  if(hal_impl == 0)
    hal_impl = new kotuku::linux_hal_t(kotuku::do_create_screen);

  return hal_impl;
  }

static bool needs_vsync;

static bool watchdog_callback(void *parg)
  {
  if(needs_vsync)
    kotuku::root_screen->vsync();

  return true;
  }

int main(int argc, char **argv)
  {
  const char *ini_path;
  if(argc > 1)
    ini_path = argv[1];
  else
    ini_path = "efis.ini";

  kotuku::the_hal()->initialize(ini_path);
  _the_app = new kotuku::pfd_application_t(root_window);

  watchdog = new kotuku::watchdog_t(root_window, _the_app);

  if(failed(kotuku::the_hal()->get_config_value(hal_section, "vsync-enable", needs_vsync)))
    needs_vsync = false;

  // enable the application can thread
  _the_app->resume();

  // turn on all events
  _the_app->publishing_enabled(true);

  // start the application
  watchdog->run(watchdog_callback, 0, 17);

  return 0;
  }

// set to the screen framebuffer handle.
// if single bufferred will be /dev/fb0, double bufferred will be /dev/fb1
static int fbfd = -1;

static color_t *mmap_framebuffer(kotuku::hal_t *hal, const kotuku::screen_metrics_t &metrics)
  {
  // try to open the actual framebuffer.
  // If this is a single-bufferred screen then it will usually be /dev/fb0
  // if double buffered then it will
  std::string framebuffer;
  if(failed(hal->get_config_value(hal_section, "framebuffer-device", framebuffer)))
    framebuffer = "/dev/fb0";

  // determine if we are double-buffered
  // open the framebuffer device
  fbfd = open(framebuffer.c_str(), O_RDWR);
  int pixel_increment = 4;

  struct fb_var_screeninfo vinfo;
  if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) != -1)
    {
    vinfo.grayscale = 0;
    ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);

    pixel_increment = vinfo.bits_per_pixel >> 3;
    }

  // calc bytes to store the values
  int length = (metrics.screen_x * metrics.screen_y) * pixel_increment;

  // map the device to memory
  return (color_t *) mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  }

kotuku::framebuffer_screen_t::framebuffer_screen_t(hal_t *hal, const screen_metrics_t &metrics)
:  memory_screen_t(mmap_framebuffer(hal, metrics), false, metrics),
   thread_t(4096, this, do_run),
   _root_window(0),
   _display_buffer(0)
  {
  int display_mode;
  if(failed(hal->get_config_value(hal_section, "rotation", _display_mode)))
    _display_mode = 0;

  //Get fixed screen information
  ioctl(fbfd, FBIOGET_FSCREENINFO, &_finfo);
  ioctl(fbfd, FBIOGET_VSCREENINFO, &_vinfo);

  if(_vinfo.bits_per_pixel == 0)
    {
    // handle operation on the emulation platform
    _vinfo.bits_per_pixel = 32;
    _vinfo.red.length = 8;
    _vinfo.red.offset = 16;
    _vinfo.green.length = 8;
    _vinfo.green.offset = 8;
    _vinfo.blue.length = 8;
    _vinfo.blue.offset = 0;
    }
  // cache value
  _pixel_increment = gdi_dim_t(_vinfo.bits_per_pixel >> 3);
  _display_buffer_length = (metrics.screen_x * metrics.screen_y) * _pixel_increment;

  _red_mask = (1 << _vinfo.red.length)-1;
  _blue_mask = (1 << _vinfo.blue.length)-1;
  _green_mask = (1 << _vinfo.green.length)-1;

  _red_shift = 8 - _vinfo.red.length;
  _blue_shift = 8 - _vinfo.blue.length;
  _green_shift = 8 - _vinfo.green.length;

  // this has assigned the drawing framebuffer and set the translation modes.
  // now determine if there is a double-buffer device
  std::string framebuffer;
  if(succeeded(hal->get_config_value(hal_section, "doublebuffer-device", framebuffer)))
    {
    // determine if we are double-buffered
    // open the framebuffer device
    _fbfd_display = open(framebuffer.c_str(), O_RDWR);

    struct fb_var_screeninfo vinfo;
    ioctl(_fbfd_display, FBIOGET_VSCREENINFO, &vinfo);

    _vinfo.grayscale = 0;
    ioctl(_fbfd_display, FBIOPUT_VSCREENINFO, &vinfo);
    ioctl(_fbfd_display, FBIOGET_VSCREENINFO, &vinfo);

    // map the device to memory
    _display_buffer = (uint8_t *) mmap(0, _display_buffer_length, PROT_READ | PROT_WRITE, MAP_SHARED, _fbfd_display, 0);

    // start the sync worker
    resume();
    }
  }

uint32_t kotuku::framebuffer_screen_t::do_run(void *pthis)
  {
  return reinterpret_cast<kotuku::framebuffer_screen_t *>(pthis)->run();
  }

void kotuku::framebuffer_screen_t::vsync()
  {
  if(_root_window == 0)
    return;             // wait for a window to be created

  // this code works by waiting for a vsync (every 60hz) then on the first one
  // will ask any changed windows to update the frame buffer
  // if there was a change, on the next vsync the frame buffer is copied
  // to the display buffer.  Hopefully before the vsync ends so that the display
  // does not encounter any tearing
  // only works if the frame buffer and display buffer are identical.
  // We use memcpy as that tends to be a generic assembler routine with hardware
  // fast copy.
  switch(_state)
    {
    case fbds_forceredraw :
      _root_window->invalidate();
      _root_window->repaint(true);
      _state = fbds_draw;
      break;
    case fbds_checkredraw :
      if(_root_window->repaint(false))
        _state = fbds_draw;
      break;
    case fbds_draw :
      if(_display_buffer != 0)      // only if double buffered
        memcpy(_display_buffer, _buffer, _display_buffer_length);
      _state = fbds_checkredraw;
      break;
    }
  }

uint32_t kotuku::framebuffer_screen_t::run()
  {
  while(!should_terminate())
    {
    // we assume the device is 0
    int arg = 0;
    ioctl(_fbfd_display, FBIO_WAITFORVSYNC, &arg);

    vsync();
    }

  return 0;
  }

kotuku::framebuffer_screen_t::~framebuffer_screen_t()
  {

  }

int kotuku::framebuffer_screen_t::display_mode() const
  {
  return _display_mode;
  }

void kotuku::framebuffer_screen_t::display_mode(int value)
  {
  _display_mode = value;
  }

void kotuku::memory_screen_t::invalidate_rect(const rect_t &)
  {

  }

kotuku::memory_screen_t::memory_screen_t(color_t *buffer, bool owns, const screen_metrics_t &metrics)
: _buffer(buffer),
  _owns(owns),
  raster_screen_t(metrics)
  {
  }

kotuku::memory_screen_t::~memory_screen_t()
  {
  if(_owns)
    {
    delete[] _buffer;
    _buffer = 0;
    }
  }

kotuku::screen_t *kotuku::memory_screen_t::create_canvas(screen_t *, const extent_t &extents)
  {
  color_t *buffer = new color_t[extents.dx * extents.dy];

  return new memory_screen_t(buffer, true, screen_metrics_t(extents.dx, extents.dy, bits_per_pixel));
  }

kotuku::screen_t *kotuku::memory_screen_t::create_canvas(screen_t *, const bitmap_t &bitmap)
  {
  return new memory_screen_t(const_cast<color_t *>(bitmap.pixels), false, screen_metrics_t(bitmap.bitmap_width, bitmap.bitmap_height, bitmap.bpp));
  }

kotuku::screen_t *kotuku::memory_screen_t::create_canvas(screen_t *h,
    const rect_t &rect)
  {
  return create_canvas(h, rect.extents());
  }

uint8_t *kotuku::memory_screen_t::point_to_address(const point_t &pt)
  {
  return reinterpret_cast<uint8_t *>(_buffer + (pt.y * screen_x) + pt.x);
  }

const uint8_t *kotuku::memory_screen_t::point_to_address(const point_t &pt) const
  {
  return reinterpret_cast<const uint8_t *>(_buffer + (pt.y * screen_x) + pt.x);
  }

kotuku::point_t &kotuku::framebuffer_screen_t::apply_rotation(const point_t &src, point_t &dst) const
  {
  gdi_dim_t temp;
  switch(_display_mode)
    {
    case 0 :
      dst = src;
      break;
    case 90 :
      temp = src.y;
      dst.y = screen_x - src.x -1;
      dst.x = temp;
      break;
    case 180 :
      temp = screen_y - src.y -1;
      dst.y = screen_x - src.x -1;
      dst.x = temp;
      break;
    case 270 :
      temp = screen_x - src.y -1;
      dst.y = src.x;
      dst.x = temp;
      break;
    }

  return dst;
  }

uint8_t *kotuku::framebuffer_screen_t::point_to_address(const point_t &pt)
  {
  // first rotate the point to ensure the offsets are correct
  point_t dest;
  apply_rotation(pt, dest);

  uint32_t pixel_offset = 0;

  switch(display_mode())
    {
    case 0 :
      pixel_offset = ((uint32_t(dest.y) * uint32_t(screen_x)) + uint32_t(dest.x));
      break;
    case 90 :
      pixel_offset = (uint32_t(dest.y) * uint32_t(screen_y))  + uint32_t(dest.x);
      break;
    case 180 :
      pixel_offset = ((uint32_t(dest.y) * uint32_t(screen_x)) - uint32_t(dest.x));
      break;
    case 270 :
      pixel_offset = (uint32_t(dest.y) * -uint32_t(screen_y))  + uint32_t(dest.x);
      break;
    }

  return reinterpret_cast<uint8_t *>(_buffer) + (pixel_offset * pixel_increment());
  }

const uint8_t *kotuku::framebuffer_screen_t::point_to_address(const point_t &pt) const
  {
  // first rotate the point to ensure the offsets are correct
  point_t dest;
  apply_rotation(pt, dest);

  uint32_t pixel_offset = 0;

  switch(display_mode())
    {
    case 0 :
      pixel_offset = ((uint32_t(dest.y) * uint32_t(screen_x)) + uint32_t(dest.x));
      break;
    case 90 :
      pixel_offset = (uint32_t(dest.y) * uint32_t(screen_y))  + uint32_t(dest.x);
      break;
    case 180 :
      pixel_offset = ((uint32_t(dest.y) * uint32_t(screen_x)) - uint32_t(dest.x));
      break;
    case 270 :
      pixel_offset = (uint32_t(dest.y) * -uint32_t(screen_y))  + uint32_t(dest.x);
      break;
    }

  return reinterpret_cast<const uint8_t *>(_buffer) + (pixel_offset * pixel_increment());
  }


gdi_dim_t kotuku::framebuffer_screen_t::pixel_increment() const
  {
  return _pixel_increment;       // return in bytes
  }

color_t kotuku::framebuffer_screen_t::get_pixel(const uint8_t *src) const
  {
  uint32_t pixel = 0;
  if(_pixel_increment == 2)
    pixel = *reinterpret_cast<const uint16_t *>(src);
  else
    pixel = *reinterpret_cast<const uint32_t *>(src);

  color_t red = ((pixel >> _vinfo.red.offset) & _red_mask) << _red_shift;
  color_t blue = ((pixel >> _vinfo.blue.offset) & _blue_mask) << _blue_shift;
  color_t green = ((pixel >> _vinfo.green.offset) & _green_mask) << _green_shift;
  return rgb(red, green, blue);
  }

void kotuku::framebuffer_screen_t::set_pixel(uint8_t *dest, color_t color) const
  {
  gdi_dim_t bytes_per_pixel = _pixel_increment;
  uint32_t pixel = 0;
  pixel = (red(color) >> _red_shift) << _vinfo.red.offset;
  pixel |= (blue(color) >> _blue_shift) << _vinfo.blue.offset;
  pixel |= (green(color) >> _green_shift) << _vinfo.green.offset;

  while(bytes_per_pixel--)
    {
    *dest++ = (uint8_t) pixel;
    pixel >>= 8;
    }
  }
