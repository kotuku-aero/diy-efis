/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#include "krypton.h"
#include "../../libs/neutron/neutron.h"
#include "../../libs/neutron/stream.h"
#include "slcan.h"
#include "../../libs/muon/cli.h"
#include "../../libs/muon/neutron_cli.h"

#undef STREAM_SEEK_SET
#undef STREAM_SEEK_END
#undef STREAM_SEEK_CUR

#define handle_t win_handle_t
#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include "getopt.h"
#undef handle_t

static HANDLE reg_fd;
static HANDLE reg_mm;
static uint8_t *reg_buffer;

static handle_t mutex;

result_t __assert(const char *file, int line, bool check, const char *msg)
  {
  if (!check)
    {
    if(msg == 0)
      trace_error("Assert failed at %s : %d", file, line);
    else
      trace_error("Assert failed at %s : %d\n%s", file, line, msg);

    DebugBreak();
    }

  return s_ok;
  }

void enter_critical()
  {
  semaphore_wait(mutex, INDEFINITE_WAIT);
  }

void exit_critical()
  {
  semaphore_signal(mutex);
  }

int32_t interlocked_increment(volatile int32_t *value)
  {
  return InterlockedIncrement(value);
  }

int32_t interlocked_decrement(volatile int32_t *value)
  {
  return InterlockedDecrement(value);
  }


#include <Shlwapi.h>

result_t create_or_open_mmap(const char *filename, uint32_t size,
  uint8_t fill_byte, bool *created,
HANDLE *hndl, HANDLE *mm_hndl, uint8_t **buffer)
  {

  char dir[_MAX_PATH];
  GetModuleFileName(NULL, dir, _MAX_PATH);
  PathRemoveFileSpec(dir);

  char root_path[MAX_PATH];
  PathCombine(root_path, dir, filename);


  *hndl = CreateFile(root_path,
    GENERIC_WRITE | GENERIC_READ,
    FILE_SHARE_WRITE,
    NULL,
    OPEN_ALWAYS,
    0,
    NULL);

  // fill file to 128k
  if (GetFileSize(*hndl, NULL) == 0)
    {
    if(created != 0)
      *created = true;

    uint8_t blk[4096];
    memset(blk, fill_byte, 4096);

    uint32_t i;
    for (i = 0; i < size; i += 4096)
      WriteFile(*hndl, blk, 4096, NULL, NULL);
    }
  else if(created != 0)
    *created = false;

  // memory map the registry
  *mm_hndl =
    CreateFileMapping(reg_fd,
      NULL,
      PAGE_READWRITE,
      0,
      size,
      NULL);

  *buffer = MapViewOfFile(*mm_hndl, FILE_MAP_ALL_ACCESS, 0, 0, size);

  return s_ok;
  }

extern result_t start_fb(uint16_t x, uint16_t y);

extern const char *i2c_device_s;
extern const char *screen_x_s;
extern const char *screen_y_s;

extern result_t krypton_fs_init(const char *root_path, filesystem_t **handle);

result_t neutron_calloc(size_t size, size_t elem_size, void** mem)
  {
  if (mem == 0)
    return e_bad_parameter;

  *mem = calloc(size, elem_size);

  return s_ok;
  }

result_t neutron_malloc(size_t size, void **mem)
  {
  if (mem == 0)
    return e_bad_parameter;

  *mem = malloc(size);

  return s_ok;
  }

result_t neutron_free(void *mem)
  {
  if (mem != 0)
    free(mem);

  return s_ok;
  }

result_t neutron_realloc(size_t new_size, void **mem)
  {
  if (mem != 0)
    *mem = realloc(*mem, new_size);

  return s_ok;
  }

result_t neutron_strdup(const char *str, size_t len, char **mem)
  {
  if (mem == 0 || str == 0)
    return e_bad_parameter;

  if(len == 0)
    *mem = _strdup(str);
  else
  {
    char *out = malloc(len+1);
    *mem = out;
    while(*str != 0 && len-- > 0)
      *out++ = *str++;

    *out = 0;
  }

  return s_ok;
  }


const char *krypton_key = "krypton";

static const char *ffs_key_name = "flash";

result_t krypton_init(int argc, char **argv, krypton_parameters_t * init_params)
  {
  result_t result;

  semaphore_create(&mutex);
  semaphore_signal(mutex);

    uint32_t reg_size = init_params->config_blocks << 5;

  // open the registry
  create_or_open_mmap(init_params->config_path, reg_size, 0, 0, &reg_fd, &reg_mm, &reg_buffer);

  if (failed(result = bsp_reg_init(init_params->factory_reset, init_params->config_blocks, 128)) &&
    result != e_not_initialized)
    return result;

  memid_t key;
  if (failed(result = reg_open_key(0, krypton_key, &key)))
    {
    if (failed(reg_create_key(0, krypton_key, &key)))
      return result;
    }

  char filename[MAX_PATH + 1];
  _splitpath(argv[0], 0, 0, filename, 0);

  reg_set_string(key, "progname", filename);

  if (init_params->slcan_device != 0)
    {
    if (failed(result = reg_set_string(key, "device", init_params->slcan_device)))
      return printf("Cannot set slcan device name\n");

    if (failed(result = reg_set_string(key, "device-type", "slcan")))
      return printf("Cannot set the sclan device type");
    }
  else
    {
    reg_delete_value(key, "device");
    reg_delete_value(key, "device-type");
    }

  init_params->factory_reset = result == e_not_initialized;

  // start the can systems
  if (failed(result = canfly_init(&init_params->neutron_params, false)) &&
    result != e_path_not_found)
    return result;

#ifndef _TOOLS
  // see if we have a screen defined
  if (init_params->init_gdi)
    start_fb(init_params->width, init_params->height);
#endif

  return s_ok;
  }

#ifndef _TOOLS
result_t run_krypton(stream_t *console, const char *prompt, cli_node_t *cli_root)
  {
  cli_t parser;
  parser.cfg.root = cli_root;
  parser.cfg.ch_complete = '\t';
  parser.cfg.ch_erase = '\b';
  parser.cfg.ch_del = 127;
  parser.cfg.ch_help = '?';
  parser.cfg.flags = 0;
  parser.cfg.prompt = "canfly";

  parser.cfg.console_in = console;
  parser.cfg.console_out = console;
  parser.cfg.console_err = console;

  if (failed(cli_init(&parser.cfg, &parser)))
    {
    stream_printf(console, "Unable to start muon\r\n");
    return e_unexpected;
    }

  return cli_run(&parser);
  }
#endif

static handle_t driver;

result_t bsp_can_init(handle_t rx_queue, const neutron_parameters_t *params)
  {
  result_t result;
  memid_t key;
  if (failed(result = reg_open_key(0, krypton_key, &key)))
    return result;

  return slcan_create(key, rx_queue, &driver);
  }

result_t bsp_send_can(const canmsg_t * msg)
  {
  return slcan_send(driver, msg);
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

typedef struct _semaphore_t {
  base_t base;
  HANDLE handle;
  volatile LONG cnt;
  } semaphore_t;

static const typeid_t semaphore_type = { "semaphore_t" };

result_t semaphore_create(handle_t *handle)
  {
  // can't use neutron malloc as is a kernel function
  semaphore_t *semp = (semaphore_t *)LocalAlloc(LPTR, sizeof(semaphore_t));

  semp->base.type = &semaphore_type;
  semp->handle = CreateSemaphore(NULL, 0, INT_MAX, NULL);
  semp->cnt = 0;

  *handle = (handle_t) semp;

  return s_ok;
  }

result_t semaphore_close(handle_t hndl)
  {
  result_t result;
  semaphore_t *semp;
  if (failed(result = is_typeof(hndl, &semaphore_type, (void **)&semp)))
    return result;

  CloseHandle(semp->handle);
  LocalFree(semp);

  return s_ok;
  }

result_t semaphore_signal(handle_t hndl)
  {
  result_t result;
  semaphore_t *semp;
  if (failed(result = is_typeof(hndl, &semaphore_type, (void **)&semp)))
    return result;

  InterlockedDecrement(&semp->cnt);

  ReleaseSemaphore(semp->handle, 1, NULL);
  return s_ok;

  }

result_t semaphore_wait(handle_t hndl, uint32_t ticks)
  {
  result_t result;
  semaphore_t *semp;
  if (failed(result = is_typeof(hndl, &semaphore_type, (void **)&semp)))
    return result;

  InterlockedIncrement(&semp->cnt);

  DWORD dwResult = WaitForSingleObject(semp->handle, ticks);

  switch (dwResult)
    {
    case WAIT_TIMEOUT:
      return e_timeout;
    case WAIT_FAILED:
      return e_unexpected;
    }

  return s_ok;
  }

#if defined(_DEBUG) & !defined(_NO_TRACE)
static char buffer[256];
result_t platform_trace(uint16_t level, const char *msg, va_list va)
  {
  vsnprintf(buffer, 256, msg, va);

  OutputDebugString(buffer);
  return s_ok;
  }
#endif

result_t has_wait_tasks(handle_t hndl)
  {
  result_t result;
  semaphore_t *semp;
  if (failed(result = is_typeof(hndl, &semaphore_type, (void **)&semp)))
    return result;

  return semp->cnt > 0 ? s_ok : s_false;
  }

 result_t ticks(uint32_t *value)
  {
   if (value == 0)
     return e_bad_pointer;

  *value = GetTickCount();
  return s_ok;
  }


typedef struct _thread_t {
  base_t base;
  task_callback cb;
  void *parg;
  DWORD id;
  HANDLE handle;
  const char *name;
  } thread_t;

static DWORD WINAPI win_cb(void *parg)
  {
  thread_t *tw = (thread_t *)parg;

  (*tw->cb)(tw->parg);

  return 0;
  }

static const typeid_t thread_type;

static result_t close_task(handle_t hndl)
  {
  result_t result;
  thread_t *thread;
  if (failed(result = is_typeof(hndl, &thread_type, (void **)&thread)))
    return result;

  TerminateThread(thread->handle, 0);

  memset(thread, 0, sizeof(thread_t));
  neutron_free(thread);

  return s_ok;
  }


static const typeid_t thread_type =
  {
  .name = "thread_t",
  .etherealize = close_task
  };

result_t task_create_kernel(const char *name,
  uint16_t stack_size,
  task_callback pfn,
  void *param,
  uint8_t priority,
  handle_t *task)
  {
  return task_create(name, stack_size, pfn, param, priority, task);
  }


result_t task_create(const char *name,
  uint16_t stack_size,
  task_callback pfn,
  void *param,
  uint8_t priority,
  handle_t *task)
  {
  result_t result;
  thread_t *thread;
  
  if (failed(result = neutron_malloc(sizeof(thread_t), (void **)&thread)))
    return result;

  thread->base.type = &thread_type;
  thread->cb = pfn;
  thread->parg = param;
  thread->name = name;
  thread->handle = CreateThread(NULL, stack_size, win_cb, thread, 0, NULL);

  if (thread->handle == 0)
    {
    neutron_free(thread);
    return e_unexpected;
    }

  thread->id = GetThreadId(thread->handle);

  if (task != 0)
    *task = (handle_t)thread;

  SetThreadPriority(thread->handle, priority);

  return s_ok;
  }

result_t get_current_task(handle_t *hndl)
  {
  DebugBreak();
  return s_ok;
  }

result_t set_task_priority(handle_t hndl, uint8_t p)
  {
  result_t result;
  thread_t *thread;
  if (failed(result = is_typeof(hndl, &thread_type, (void **)&thread)))
    return result;

  SetThreadPriority(thread->handle, p);

  return s_ok;
  }

result_t get_task_priority(handle_t hndl, uint8_t *p)
  {
  result_t result;
  thread_t *thread;
  if (failed(result = is_typeof(hndl, &thread_type, (void **)&thread)))
    return result;

  *p = GetThreadPriority(thread->handle);

  return s_ok;
  }

result_t get_task_name(handle_t hndl, const char **name)
  {
  result_t result;
  thread_t *thread;
  if (failed(result = is_typeof(hndl, &thread_type, (void **)&thread)))
    return result;

  *name = thread->name;

  return s_ok;
  }

result_t task_suspend(handle_t hndl)
  {
  result_t result;
  thread_t *thread;
  if (failed(result = is_typeof(hndl, &thread_type, (void **)&thread)))
    return result;


  SuspendThread(thread->handle);

  return s_ok;
  }

result_t task_resume(handle_t hndl)
  {
  result_t result;
  thread_t *thread;
  if (failed(result = is_typeof(hndl, &thread_type, (void **)&thread)))
    return result;


  ResumeThread(thread->handle);

  return s_ok;
  }

result_t task_sleep(handle_t task, uint32_t n)
  {

  Sleep(n);

  return s_ok;
  }

result_t task_exit(uint32_t code)
  {
  exit(code);
  }

result_t yield()
  {
  Sleep(0);         // allow a context switch

  return s_ok;
  }

result_t bsp_random(uint32_t seed, uint32_t *value)
  {
  if (rand == 0)
    return e_bad_pointer;

  *value = rand();

  return s_ok;
  }

result_t bsp_cpu_id(uint32_t* id, uint32_t* revision, const char** image_name)
  {
  if (id == 0)
    return e_bad_pointer;

  *id = 0;

  if(revision != 0)
    *revision = 0;

  if(image_name != 0)
    *image_name = 0;

  return s_ok;
  }

result_t debug_break()
  {
  DebugBreak();

  return s_ok;
  }

result_t bsp_queue_reset(uint32_t delay)
  {
  return e_not_implemented;
  }
