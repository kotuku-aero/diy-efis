/*
 * Newlib System Call Stubs for DIY-EFIS
 *
 * OVERVIEW:
 * Newlib (the C standard library) expects certain POSIX-like system calls
 * to be provided by the platform. Since PIC32 is bare-metal (no OS), we
 * provide minimal stub implementations.
 *
 * These stubs allow programs to link but may return errors for operations
 * that don't make sense in a bare-metal environment (like file I/O).
 *
 * INTEGRATION:
 * Add this file to a library that gets linked AFTER the standard libraries:
 *   target_link_libraries(your_app PRIVATE c m gcc your_stubs_lib)
 *
 * CUSTOMIZATION NOTES:
 * - sbrk(): Currently calls panic() - customize if you need malloc support
 * - write(): Silently succeeds - connect to UART for printf output
 * - read(): Returns EOF - connect to UART for scanf/getchar input
 * - _exit(): Infinite loop - could trigger watchdog or jump to bootloader
 */

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "../../include/canfly.h"

/*
 * errno implementation
 * Newlib expects a per-thread errno, but in bare metal we use a simple global.
 */
static int _errno = 0;
int *__errno(void)
  {
  return &_errno;
  }

/*
 * sbrk - Increase program data space
 *
 * HEAP ALLOCATION:
 * This is called by malloc() to expand the heap.
 *
 */
void *sbrk(int incr)
{
  static char *heap_end = 0;
  extern char _end;           // Defined by linker (end of BSS)
  extern char _stack;         // Defined by linker (top of stack)

  if (heap_end == 0)
    heap_end = &_end;

  char *prev_heap_end = heap_end;

  // Check for collision with stack
  if (heap_end + incr > &_stack)
    {
        errno = ENOMEM;
        return (void *)-1;
    }

  heap_end += incr;
  return (void *)prev_heap_end;
}

/*
 * _exit - Terminate program execution
 * Called when program exits or abort() is called.
 *
 */
void _exit(int status)
  {
  sys_exit(status);
  }

/*
 * close - Close a file descriptor
 */
int _close(int file)
  {
  errno = close_handle((handle_t)file);
  return failed(errno) ? -1 : 0;
  }

/*
 * fstat - Get file status
 *
 */
int _fstat(int file, struct stat *st)
  {
  stat_t stbuf;
  if (failed(errno = stream_stat((handle_t)file, &stbuf, nullptr)))
    return -1;

  // Make it look like a character device (for stdout/stderr)
  st->st_mode = S_IFCHR;
  st->st_size = stbuf.size;
  return 0;
  }

/*
 * isatty - Check if file descriptor is a terminal
 * Return 1 for stdin/stdout/stderr, 0 otherwise.
 */
int isatty(int file)
  {
   return 0;
  }

/*
 * lseek - Reposition file offset
 */
int _lseek(int file, int ptr, int dir)
  {
  if (failed(errno = stream_setpos((handle_t)file, ptr, (uint32_t)dir)))
    return -1;
  return 0;
  }

/*
 * read - Read from a file descriptor
 *
 */
int _read(int file, char *ptr, int len)
  {
  uint32_t bytes_read;
  if (failed(errno = stream_read((handle_t)file, ptr, len, &bytes_read, nullptr)))
    return -1;

  return (int)bytes_read;
  }

/*
 * write - Write to a file descriptor
 *
 */
int _write(int file, char *ptr, int len)
  {
  if (failed(errno = stream_write((handle_t)file, ptr, (uint32_t)len, nullptr)))
    return -1;

  return len;
  }

int _open(const char *name, int flags, int mode)
  {
  // combine the flags and mode
  uint32_t _flags = 0;

  if (flags & O_RDONLY)
    _flags |= STREAM_O_RD;

  if (flags & O_WRONLY)
    _flags |= STREAM_O_WR;

  if (flags & O_RDWR)
    _flags |= STREAM_O_RDWR;

  if (flags & O_CREAT)
    _flags |= STREAM_O_CREAT;

  if (flags & O_EXCL)
    _flags |= STREAM_O_EXCL;

  if (flags & O_TRUNC)
    _flags |= STREAM_O_TRUNC;

  if (flags & O_APPEND)
    _flags |= STREAM_O_APPEND;

  // TODO: MKDIR?

  handle_t fd;
  if (failed(errno = stream_open(name, _flags, &fd, nullptr)))
    return -1;

  return (int) fd;
  }
/*
 * kill - Send signal to a process
 * Not supported in bare metal (no processes).
 */
int kill(int pid, int sig)
  {
  (void)pid;
  (void)sig;
  errno = EINVAL;
  return -1;
  }

/*
 * getpid - Get process ID
 * Not supported in bare metal (no processes).
 * Returns a dummy PID to satisfy newlib.
 */
int getpid(void)
  {
  return 1;  // Return a dummy PID
  }
