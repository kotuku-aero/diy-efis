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

bool Target_ConfigUpdateRequiresErase()
  {
  return false;
  }

bool Target_HasNanoBooter()
  {
  return false;
  };

bool Target_IFUCapable()
  {
  return false;
  };

bool Target_HasProprietaryBooter()
  {
  return false;
  };

uint32_t GetPlatformCapabilities()
  {
  return 0;
  };

uint32_t GetTargetCapabilities()
  {
  return 0;
  };

bool RequestToLaunchProprietaryBootloader()
  {
  return false;
  };

bool RequestToLaunchNanoBooter()
  {
  return false;
  };
