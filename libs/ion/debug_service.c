#include "../neutron/neutron.h"

#include "../nano/PAL/Include/nanoPAL_COM.h"

bool DebuggerPort_Initialize(COM_HANDLE comPortNum)
  {
  return true;
  }

bool DebuggerPort_Uninitialize(COM_HANDLE comPortNum)
  {
  return true;
  }

int DebuggerPort_Write(COM_HANDLE comPortNum, const char *data, size_t size, int maxRetries)
  {
  return 0;
  }

int DebuggerPort_Read(COM_HANDLE comPortNum, char *data, size_t size)
  {
  return 0;
  }

bool DebuggerPort_Flush(COM_HANDLE comPortNum)
  {
  return true;
  }

bool DebuggerPort_IsSslSupported(COM_HANDLE comPortNum)
  {
  return false;
  }

bool DebuggerPort_UpgradeToSsl(COM_HANDLE comPortNum, unsigned int flags)
  {
  return false;
  }

bool DebuggerPort_IsUsingSsl(COM_HANDLE comPortNum)
  {
  return false;
  }
