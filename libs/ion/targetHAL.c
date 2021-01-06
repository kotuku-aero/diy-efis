#include "../nano/CLR/Include/nanoCLR_Headers.h"
#include "../nano/HAL/Include/nanoHAL_v2.h"

void HAL_AssertEx()
  {
  NonoDebugBreak();

  while (true)
    {
    }
  }

#ifdef _WIN32
void NanoDebugBreak()
  {
  return DebugBreak();
  }
#endif
