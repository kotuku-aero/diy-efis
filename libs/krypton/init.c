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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "krypton.h"
#include "../neutron/neutron.h"
#include "../neutron/bsp.h"

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include "getopt.h"

static HANDLE reg_fd;
static HANDLE reg_mm;
static uint8_t *reg_buffer;

uint8_t *fb_buffer;

static semaphore_p mutex;

void enter_critical()
  {
  semaphore_wait(mutex, INDEFINITE_WAIT);
  }

void exit_critical()
  {
  semaphore_signal(mutex);
  }

handle_t bsp_thread_mutex()
  {
  semaphore_p tls_mutex = TlsGetValue(0);
  if (tls_mutex == 0)
    {
    // create our thread specific handle
    semaphore_create(&tls_mutex);

    TlsSetValue(0, tls_mutex);
    }

  return tls_mutex;
  }

static void create_or_open_mmap(const char *filename, uint32_t size, HANDLE *hndl, HANDLE *mm_hndl, uint8_t **buffer)
  {
  *hndl = CreateFile(filename,
    GENERIC_WRITE | GENERIC_READ,
    FILE_SHARE_WRITE,
    NULL,
    OPEN_ALWAYS,
    NULL,
    NULL);

  // fill file to 128k
  if (GetFileSize(*hndl, NULL) == 0)
    {
    byte_t blk[4096];
    memset(blk, 0, 4096);

    int i;
    for (i = 0; i < size; i += 4096)
      WriteFile(*hndl, blk, 4096, NULL, NULL);
    }

  // memory map the registry
  *mm_hndl =
    CreateFileMapping(reg_fd,
      NULL,
      PAGE_READWRITE,
      0,
      size,
      NULL);

  *buffer = MapViewOfFile(*mm_hndl, FILE_MAP_ALL_ACCESS, 0, 0, size);

  }

extern void start_fb(uint16_t x, uint16_t y, uint8_t *);


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
"  -x <x-pixels>  Set the screen width to x-pixels\n"
"  -y <y-pixels>  Set the screen height to y-pixels\n"
"  -f <path>      Use <path> as the base of the filesystem"
"  -h             Print this help message\n"
" Values for MSH:\n"
" diy-efis -c 32768 -x 320 -y 240 -d <path> diy-efis.reg\n"
" Values for PI-TFT:\n"
" diy-efis -c 32768 -f /dev/fb1 -x 480 -y 320 -f /diy-efis -d /dev/i2c-1 diy-efis.reg\n";

static result_t print_error(const char *msg, result_t result)
  {
  fprintf(stderr, "%s %d\n", msg, result);
  return result;
  }

extern const char *i2c_device_s;
extern const char *screen_x_s;
extern const char *screen_y_s;

extern result_t krypton_fs_init(const char *root_path, filesystem_p *handle);

result_t krypton_init(int argc, char **argv)
  {
  result_t result;
  semaphore_create(&mutex);
  semaphore_signal(mutex);

  uint32_t reg_size = 4096 * 32;
  bool factory_reset = false;
  const char *fb_size = "32768";
  const char *width = "320";
  const char *height = "240";

  const char *fs_path = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hc:f:x:y:d:f:")) != -1)
    {
    switch (opt)
      {
      case 'h':
        puts(help);
        return s_false;
      case 'c':
        factory_reset = true;
        fb_size = optarg;
        break;
      case 'x':
        width = optarg;
        break;
      case 'y':
        height = optarg;
        break;
      case 'f' :
        fs_path = optarg;
        break;
      default:
        print_error(help, s_false);
        exit(-1);
      }
    }

  // open the registry
  create_or_open_mmap(argv[optind], reg_size, &reg_fd, &reg_mm, &reg_buffer);

  reg_size >>= 5;         // make number of blocks

  if (failed(result = bsp_reg_init(factory_reset, (uint16_t)reg_size, 128)) &&
    result != e_not_initialized)
    return result;

  // start the can systems
  if (failed(result = can_aerospace_init(&init_params, factory_reset, false)) &&
    result != e_path_not_found)
    return result;

  handle_t fshndl;
  if (fs_path != 0)
    {
    char root_path[MAX_PATH];

    if (GetFullPathName(fs_path, MAX_PATH, root_path, NULL) == 0)
      printf("Cannot get the path of the root filesystem, ignoring parameter.  No filesystem loaded");
    else
      {
      filesystem_p fs;
      if (failed(result = krypton_fs_init(root_path, &fs)))
        return result;

      // register the filesystem
      if (failed(result = mount("/", fs, 0, &fshndl)))
        return result;
      }
    }

  uint16_t x = (uint16_t) atoi(width);
  uint16_t y = (uint16_t) atoi(height);

  // see if we have a screen defined

  uint32_t size = x * y * sizeof(color_t);
  // this creates the emulator
  fb_buffer = (uint8_t *)malloc(size);

  memset(fb_buffer, 0, size);

  start_fb(x, y, fb_buffer);

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
  FlushViewOfFile(reg_buffer + offset, bytes_to_write);

  return s_ok;
  }

#ifdef _DEBUG
void memory_check()
  {
  if (_heapchk() != _HEAPOK)
    {
    trace_error("Heap is corrupted\n");
    DebugBreak();
    }
  }
#else
#define memory_check()
#endif

void *neutron_calloc(size_t count, size_t size)
  {
  memory_check();

  return calloc(count, size);
  }

void *neutron_malloc(size_t size)
  {
  memory_check();

  return malloc(size);
  }

void neutron_free(void *mem)
  {
  memory_check();

  if(mem != 0)
    free(mem);
  }

void *neutron_realloc(void *mem, size_t new_size)
  {
  memory_check();

  return realloc(mem, new_size);
  }

char *neutron_strdup(const char *str)
  {
  size_t len = strlen(str);
  char *buf = (char *)neutron_malloc(len + 1);

  memcpy(buf, str, len + 1);
  return buf;
  }

typedef struct _semaphore_t {
  HANDLE handle;
  volatile LONG cnt;
  } semaphore_t;

result_t semaphore_create(semaphore_p *handle)
  {
  semaphore_t *semp = (semaphore_t *)neutron_malloc(sizeof(semaphore_t));

  semp->handle = CreateSemaphore(NULL, 0, INT_MAX, NULL);
  semp->cnt = 0;

  *handle = semp;

  return s_ok;
  }

result_t semaphore_close(semaphore_p hndl)
  {
  semaphore_t *semp = (semaphore_t *)hndl;
  CloseHandle(semp->handle);
  neutron_free(semp);

  return s_ok;
  }

result_t semaphore_signal(semaphore_p hndl)
  {
  semaphore_t *semp = (semaphore_t *)hndl;
  InterlockedDecrement(&semp->cnt);

  ReleaseSemaphore(semp->handle, 1, NULL);
  return s_ok;

  }

result_t semaphore_wait(semaphore_p hndl, uint32_t ticks)
  {
  semaphore_t *semp = (semaphore_t *)hndl;
  InterlockedIncrement(&semp->cnt);

  DWORD result = WaitForSingleObject(semp->handle, ticks);

  switch (result)
    {
    case WAIT_TIMEOUT :
      return e_timeout_error;
    case WAIT_FAILED :
      return e_unexpected;
    }

  return s_ok;
  }

#if defined(_DEBUG) & !defined(_NO_TRACE)
static char buffer[256];
void platform_trace(uint16_t level, const char *msg, va_list va)
  {
  
  vsnprintf(buffer, 256, msg, va);

  OutputDebugString(buffer);
  }
#endif

bool has_wait_tasks(semaphore_p hndl)
  {
  semaphore_t *semp = (semaphore_t *)hndl;
  return semp->cnt > 0;
  }

uint32_t ticks()
  {
  return GetTickCount();
  }

typedef struct _tw_t {
  task_callback cb;
  void *parg;
  } tw_t;

static DWORD WINAPI win_cb(void *parg)
  {
  tw_t *tw = (tw_t *)parg;

  (*tw->cb)(tw->parg);

  free(tw);

  return 0;
  }

result_t task_create(const char *name,
  uint16_t stack_size,
  task_callback pfn,
  void *param,
  uint8_t priority,
  task_p *task)
  {
  tw_t *tw = (tw_t *)neutron_malloc(sizeof(tw_t));
  tw->cb = pfn;
  tw->parg = param;

  HANDLE the_task = CreateThread(NULL, stack_size, win_cb, tw, 0, NULL);

  if(task != 0)
    *task = the_task;

  SetThreadPriority(task, priority);

  return s_ok;
  }

result_t set_task_priority(task_p h, uint8_t p)
  {
  SetThreadPriority(h, p);

  return s_ok;
  }

result_t get_task_priority(task_p h, uint8_t *p)
  {
  *p = GetThreadPriority(h);

  return s_ok;
  }

result_t close_task(task_p h)
  {
  TerminateThread(h, 0);

  return s_ok;
  }

result_t task_suspend(task_p h)
  {
  SuspendThread(h);

  return s_ok;
  }

result_t task_resume(task_p h)
  {
  ResumeThread(h);

  return s_ok;
  }

result_t task_sleep(unsigned long n)
  {
  Sleep(n);

  return s_ok;
  }

void yield()
  {
  Sleep(0);         // allow a context switch
  }

void *ion_malloc(size_t len)
  {
  return malloc(len);
  }

void ion_free(void *buffer)
  {
  free(buffer);
  }