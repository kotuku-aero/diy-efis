//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

// #include <nanoHAL_Power.h>
#include "nano/HAL/Include/nanoHAL_v2.h"

#ifdef __cplusplus
extern "C" {
#endif
  extern void NanoDebugBreak();
#define NANOCLR_STOP() ::NanoDebugBreak()
  extern uint32_t GetPlatformCapabilities();
  extern void CPU_Sleep();
  extern void CPU_Reset();
  extern void Watchdog_Reset();
  extern bool CPU_IsSoftRebootSupported();

#ifdef __cplusplus
  }
#endif

#endif //_TARGET_HAL_H_
