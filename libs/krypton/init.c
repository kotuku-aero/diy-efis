#include "../../libs/electron/electron.h"

#include <Windows.h>

static HANDLE reg_fd;
static HANDLE reg_mm;
static uint8_t *reg_buffer;

uint8_t *fb_buffer;

static handle_t mutex;

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
  handle_t tls_mutex = TlsGetValue(0);
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
extern const bitmap_t  splash_bitmap;     // 320x240

extern result_t electron_init(const char *reg_path, bool factory_reset)
  {
  result_t result;
  semaphore_create(&mutex);
  semaphore_signal(mutex);

  uint32_t reg_size = 4096 * 32;
  // open the registry
  create_or_open_mmap(reg_path, reg_size, &reg_fd, &reg_mm, &reg_buffer);

  reg_size >>= 5;         // make number of blocks

  // and init the registry
  if (failed(result = bsp_reg_init(factory_reset, (uint16_t)reg_size, 128)))
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

      if (x == 320 && y == 240)
        {
        memcpy(fb_buffer, splash_bitmap.pixels, size);
        }
      else
       memset(fb_buffer, 0, size);

      start_fb(x, y, fb_buffer);
      }
    }

  return s_ok;
  }

#define BLOCK_SHIFT 5

result_t bsp_reg_read_block(uint32_t offset, uint16_t bytes_to_read,
  void *buffer, handle_t task_callback)
  {
  memcpy(buffer, reg_buffer + offset, bytes_to_read);

  if (task_callback != 0)
    semaphore_signal(task_callback);
  return s_ok;
  }

result_t bsp_reg_write_block(uint32_t offset, uint16_t bytes_to_write,
  const void *buffer, handle_t task_callback)
  {
  memcpy(reg_buffer + offset, buffer, bytes_to_write);
  FlushViewOfFile(reg_buffer + offset, bytes_to_write);
  if (task_callback != 0)
    semaphore_signal(task_callback);

  return s_ok;
  }

extern result_t create_root_screen(gdi_dim_t x, gdi_dim_t y, const char *device, int display_mode)
  {
  return e_not_implemented;
  }


void *kmalloc(size_t size)
  {
  return malloc(size);
  }

void kfree(void *mem)
  {
  free(mem);
  }

void *krealloc(void *mem, size_t new_size)
  {
  return realloc(mem, new_size);
  }

char *kstrdup(const char *str)
  {
  size_t len = strlen(str);
  char *buf = (char *)kmalloc(len + 1);

  memcpy(buf, str, len + 1);
  return len;
  }

typedef struct _semaphore_t {
  HANDLE handle;
  volatile LONG cnt;
  } semaphore_t;

result_t semaphore_create(handle_t *handle)
  {
  semaphore_t *semp = (semaphore_t *)kmalloc(sizeof(semaphore_t));

  semp->handle = CreateSemaphore(NULL, 0, INT_MAX, NULL);
  semp->cnt = 0;

  *handle = semp;

  return s_ok;
  }

result_t semaphore_close(handle_t hndl)
  {
  semaphore_t *semp = (semaphore_t *)hndl;
  CloseHandle(semp->handle);
  kfree(semp);

  return s_ok;
  }

result_t semaphore_signal(handle_t hndl)
  {
  semaphore_t *semp = (semaphore_t *)hndl;
  InterlockedDecrement(&semp->cnt);

  ReleaseSemaphore(semp->handle, 1, NULL);
  return s_ok;

  }

result_t semaphore_wait(handle_t hndl, uint32_t ticks)
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

bool has_wait_tasks(handle_t hndl)
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
  handle_t *task)
  {
  tw_t *tw = (tw_t *)kmalloc(sizeof(tw_t));
  tw->cb = pfn;
  tw->parg = param;

  task = CreateThread(NULL, stack_size, win_cb, tw, 0, NULL);

  SetThreadPriority(task, priority);

  return s_ok;
  }

result_t set_task_priority(handle_t h, uint8_t p)
  {
  SetThreadPriority(h, p);

  return s_ok;
  }

result_t get_task_priority(handle_t h, uint8_t *p)
  {
  *p = GetThreadPriority(h);

  return s_ok;
  }

result_t close_task(handle_t h)
  {
  TerminateThread(h, 0);

  return s_ok;
  }

result_t task_suspend(handle_t h)
  {
  SuspendThread(h);

  return s_ok;
  }

result_t task_resume(handle_t h)
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