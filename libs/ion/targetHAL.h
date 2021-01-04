//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_HAL_H_
#define _TARGET_HAL_H_

// #include <nanoHAL_Power.h>
#include "../nano/HAL/Include/nanoHAL_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

// set min possible number of sockets
#define PLATFORM_DEPENDENT__SOCKETS_MAX_COUNT 1

#if defined(_WIN32)
#define NANOCLR_STOP() ::DebugBreak()
#pragma warning(error : 4706) // error C4706: assignment within conditional expression
#endif

inline void __cdecl HARD_Breakpoint()
{
    if (::IsDebuggerPresent())
    {
        ::DebugBreak();
    }
}

#define HARD_BREAKPOINT() HARD_Breakpoint()

inline bool Target_ConfigUpdateRequiresErase()
{
    return true;
}

inline bool Target_HasNanoBooter()
{
    return false;
};

inline bool Target_IFUCapable()
{
    return false;
};

inline bool Target_HasProprietaryBooter()
{
    return false;
};

inline uint32_t GetPlatformCapabilities()
{
    return 0;
};

inline uint32_t GetTargetCapabilities()
{
    return 0;
};

inline bool RequestToLaunchProprietaryBootloader()
{
    return false;
};

inline bool RequestToLaunchNanoBooter()
{
    return false;
};

//	CPU_Sleep((SLEEP_LEVEL_type)sleepLevel, wakeEvents);

  extern void CPU_Sleep();
  extern void CPU_Reset();
  extern void Watchdog_Reset();
  extern bool CPU_IsSoftRebootSupported();

#ifdef __cplusplus
  }
#endif

#endif //_TARGET_HAL_H_
