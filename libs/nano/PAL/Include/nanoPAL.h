//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOPAL_H_
#define _NANOPAL_H_ 1

//#include <nanoHAL_Types.h>

#include "../../HAL/Include/nanoHAL.h"
#include "../Include/nanoPAL_Time.h"
#include "../../HAL/Include/nanoHAL_v2.h"
#include "../Include/nanoPAL_AsyncProcCalls_decl.h"
#include "../Include/nanoPAL_PerformanceCounters.h"
#include "../Include/nanoPAL_COM.h"

#ifdef __cplusplus
  extern "C" {
#endif

  //
  //#include <Display_decl.h>
  //#include <Power_decl.h> (must be before events_decl.h)
  //#include <events_decl.h>
  //#include <palevent_decl.h>
  //#include <heap_decl.h>
  // This function returns location of the CLR heap.
  void HeapLocation(unsigned char *&BaseAddress, unsigned int &SizeInBytes);

  //
  // !!! KEEP IN SYNC WITH Microsoft.SPOT.Hardware.PowerLevel !!!
  //
  enum POWER_LEVEL
    {
    POWER_LEVEL__HIGH_POWER = 0x10,
    POWER_LEVEL__MID_POWER = 0x20,
    POWER_LEVEL__LOW_POWER = 0x30,
    };

  int hal_vprintf(const char *format, va_list arg);
  int hal_vfprintf(COM_HANDLE stream, const char *format, va_list arg);
  int hal_snprintf(char *buffer, size_t len, const char *format, ...);
  int hal_vsnprintf(char *buffer, size_t len, const char *format, va_list arg);

#define hal_strcpy_s(strDst, sizeInBytes, strSrc) strcpy_s(strDst, sizeInBytes, strSrc)
#define hal_strncpy_s(strDst, sizeInBytes, strSrc, count) strncpy_s(strDst, sizeInBytes, strSrc, count)
#define hal_strlen_s(str) strlen(str)
#define hal_strncmp_s(str1, str2, num) strncmp(str1, str2, num)

#ifdef __cplusplus
  }
#endif

//#include <SD_decl.h>

//#include "..\pal\Diagnostics\Native_Profiler.h"
// native profiler
#define NATIVE_PROFILE_PAL_HEAP()
#define NATIVE_PROFILE_PAL_CRT()

#endif // _NANOPAL_H_
