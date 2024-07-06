#ifndef __GETDELIM_H__
#define __GETDELIM_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _POSIX_SOURCE
  typedef long ssize_t;
#define SSIZE_MAX LONG_MAX
#endif

  extern ssize_t getdelim(char ** lineptr, size_t * n, int delimiter, FILE *ci, FILE *co);
  extern ssize_t getline(char ** lineptr, size_t * n, FILE * ci, FILE *co);

#ifdef __cplusplus
  }
#endif

#endif
