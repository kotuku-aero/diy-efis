#include "../nano/CLR/Include/nanoCLR_Headers.h"
#include "../nano/HAL/Include/nanoHAL_v2.h"

#include "targetHAL.h"

void HAL_AssertEx()
  {
  NanoDebugBreak();
  }

#ifdef _WIN32
void NanoDebugBreak()
  {
  // DebugBreak();

  while (true)
    {
    }
  }
#endif
