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
#include "electron.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static uint8_t *reg_buffer;

//handle_t bsp_thread_mutex()
//  {
//  semaphore_p tls_mutex = TlsGetValue(0);
//  if (tls_mutex == 0)
//    {
//    // create our thread specific handle
//    semaphore_create(&tls_mutex);
//
//    TlsSetValue(0, tls_mutex);
//    }
//
//  return tls_mutex;
//  }

static semaphore_p mutex;

void enter_critical()
  {
  semaphore_wait(mutex, INDEFINITE_WAIT);
  }

void exit_critical()
  {
  if (mutex != 0)
    semaphore_signal(mutex);
  }

static neutron_parameters_t init_params = {
  .hardware_revision = 1,
  .software_revision = 1,
  .node_type = unit_mfd,
  .node_id = mfd_node_id_last
  };

static const char *help =
"diy-efis ALPHA_1\n"
"diy-efis <Options> registry_path\n"
"Options only required if creating a new registry:\n"
"  -c <size>      Create a new registry with <size> blocks, old path will be deleted\n"
"  -f <path>      Set the framebuffer path to <path>\n"
"  -x <x-pixels>  Set the screen width to x-pixels\n"
"  -y <y-pixels>  Set the screen height to y-pixels\n"
"  -d <device>    Open the comm-device <device>\n"
"                 Device can be i2c:/dev/i2c-1 or slcan:/dev/ttyUSB0\n"
"                 or which wver driver to use"
"  -h             Print this help message\n"
" Values for PI-TFT:\n"
" diy-efis -c 32768 -f /dev/fb1 -x 480 -y 320 -d i2c:/dev/i2c-1 diy-efis.reg\n"
" Values for PI and slcan:\n"
" diy-efis -c 32768 -f /dev/fb1 -x 800 -y 480 -d slcan:/dev/ttyUSB0 diy-efis.reg\n";

static result_t print_error(const char *msg, result_t result)
  {
  fprintf(stderr, "%s %d\n", msg, result);
  return result;
  }

const char *device_s = "device";
extern const char *screen_x_s;
extern const char *screen_y_s;
extern const char *framebuffer_device_s;

result_t electron_init(int argc, char **argv)
  {
  result_t result;
  semaphore_create(&mutex);
  semaphore_signal(mutex);

  uint32_t reg_size = 32768 * 32;
  bool factory_reset = false;
  const char *fb_size = "32768";
  const char *fb_path = "/dev/fb1";
  const char *width = "320";
  const char *height = "240";
  const char *device = "i2c:/dev/i2c-1";

  int opt;
  while ((opt = getopt(argc, argv, "hc:f:x:y:d:")) != -1)
    {
    switch (opt)
      {
      case 'h' :
        puts(help);
        return s_false;
      case 'c' :
        factory_reset = true;
        fb_size = optarg;
        break;
      case 'f' :
        fb_path = optarg;
        break;
      case 'x':
        width = optarg;
        break;
      case 'y':
        height = optarg;
        break;
      case 'd' :
        device = optarg;
        break;
      default:
        return print_error(help, s_false);
      }
    }

  int fbfd;
  
  if(!factory_reset)
    fbfd = open(argv[optind], O_RDWR);

  if (factory_reset || fbfd < 0)      // may not exist
    {
    reg_size = (uint32_t)atoi(fb_size);
    reg_size <<= 5;         // 32 byte blocks

    factory_reset = true;
    fbfd = open(argv[optind], O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

    // fill the registry with 00
    uint8_t *buffer = (uint8_t *)malloc(4096);
    memset(buffer, 0, 4096);

    uint32_t n_bytes;
    for(n_bytes = reg_size; n_bytes > 0; n_bytes -= 4096)
      write(fbfd, buffer, 4096);

    free(buffer);
    }
  else
    {
    struct stat sb;
    fstat(fbfd, &sb);
    reg_size = sb.st_size;
    }

  reg_buffer = (uint8_t *)mmap(0, reg_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  // open the registry

  reg_size >>= 5;         // make number of blocks

  if (failed(result = bsp_reg_init(factory_reset, (uint16_t)reg_size, 128)) &&
    result != e_not_initialized)
    return result;

  if (factory_reset || failed(result))
    {
    memid_t key;
    // set up the initial registry
    if (failed(result = reg_create_key(0, "electron", &key)))
      return print_error("Cannot create electron key\n", result);

    if (failed(result = reg_set_string(key, framebuffer_device_s, fb_path)))
      return print_error("Cannot set framebuffer path\n", result);

    uint16_t value = (uint16_t)atoi(width);

    if(failed(result = reg_set_uint16(key, screen_x_s, value)))
      return print_error("Cannot set screen_x\n", result);

    value = (uint16_t)atoi(height);
    if(failed(result = reg_set_uint16(key, screen_y_s, value)))
      return print_error("Cannot set screen_y\n", result);

    // determine the driver name
    const char *device_name = device;

    if (strncmp(device_name, "i2c:", 4) == 0)
      {
      device_name += 4;

      if (failed(result = reg_set_string(key, device_s, device_name)))
        return print_error("Cannot set i2c device name\n", result);

      if (failed(result = reg_set_bool(0, "12c_can", true)))
        return print_error("Cannot select an 12c device", result);
      }
    else if (strncmp(device_name, "slcan:", 6) == 0)
      {
      device_name += 6;

      if (failed(result = reg_set_string(key, device_s, device_name)))
        return print_error("Cannot set i2c device name\n", result);

      if (failed(result = reg_set_bool(0, "12c_can", false)))
        return print_error("Cannot select an 12c device", result);

      }
    else
      return print_error("Cannot determine the CAN driver port", e_bad_parameter);
    }

  return factory_reset ? s_false : s_ok;
  }

#define BLOCK_SHIFT 5

result_t bsp_reg_read_block(uint32_t offset, uint16_t bytes_to_read, void *buffer)
  {
  memcpy(buffer, reg_buffer + offset, bytes_to_read);
  return s_ok;
  }

result_t bsp_reg_write_block(uint32_t offset, uint16_t bytes_to_write, const void *buffer)
  {
  memcpy(reg_buffer + offset, buffer, bytes_to_write);
  msync(reg_buffer + offset, bytes_to_write, MS_ASYNC);

  return s_ok;
  }

#if defined(_DEBUG) & !defined(_NO_TRACE)
void platform_trace(uint16_t level, const char *msg, va_list va)
  {
  vfprintf(stderr, msg, va);
  }
#endif
