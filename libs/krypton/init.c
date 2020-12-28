#include "krypton.h"

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
"  -f <path>      Set the framebuffer path to <path>\n"
"  -x <x-pixels>  Set the screen width to x-pixels\n"
"  -y <y-pixels>  Set the screen height to y-pixels\n"
"  -d <device>    Open the i2c-devicet <device>\n"
"  -h             Print this help message\n"
" Values for CM3:\n"
" diy-efis -c 32768 -f /dev/fb1 -x 320 -y 240 -d /dev/i2c-1 diy-efis.reg\n"
" Values for PI-TFT:\n"
" diy-efis -c 32768 -f /dev/fb1 -x 480 -y 320 -d /dev/i2c-1 diy-efis.reg\n";

static result_t print_error(const char *msg, result_t result)
  {
  fprintf(stderr, "%s %d\n", msg, result);
  return result;
  }

extern const char *i2c_device_s;
extern const char *screen_x_s;
extern const char *screen_y_s;

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

  int opt;
  while ((opt = getopt(argc, argv, "hc:f:x:y:d:")) != -1)
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
      default:
        return print_error(help, s_false);
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

  // see if we have a screen defined
  memid_t proton_key;
  if (succeeded(reg_open_key(0, "proton", &proton_key)))
    {
    uint16_t x;
    uint16_t y;
    if (succeeded(reg_get_uint16(proton_key, "screen-x", &x)) &&
      succeeded(reg_get_uint16(proton_key, "screen-y", &y)))
      {
      uint32_t size = x * y * sizeof(color_t);
      // this creates the emulator
      fb_buffer = (uint8_t *)malloc(size);

      memset(fb_buffer, 0, size);

      start_fb(x, y, fb_buffer);
      }
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