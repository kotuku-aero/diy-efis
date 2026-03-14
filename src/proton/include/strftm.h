#ifndef __strftm_h__
#define __strftm_h__

#include "../../include/sys_canfly.h"

extern size_t strftm(char *s, size_t maxsize, const char *format, const tm_t *timeptr);

#endif