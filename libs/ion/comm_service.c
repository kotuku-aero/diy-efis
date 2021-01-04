#include "../neutron/neutron.h"

#include "../nano/PAL/Include/nanoPAL_COM.h"

bool GenericPort_Initialize(int comPortNum)
  {
  (void)comPortNum;

  NATIVE_PROFILE_PAL_COM();
  return true;
  }

bool GenericPort_Uninitialize(int comPortNum)
  {
  return true;
  }

uint32_t GenericPort_Write(int comPortNum, const char *data, size_t size)
  {
  return 0;
  }

int GenericPort_Read(int comPortNum, char *data, size_t size)
  {
  return 0;
  }

bool GenericPort_Flush(int comPortNum)
  {
  return true;
  }

bool GenericPort_IsSslSupported(int comPortNum)
  {
  return false;
  }

bool GenericPort_UpgradeToSsl(int comPortNum, unsigned int flags)
  {
  return false;
  }

bool GenericPort_IsUsingSsl(int comPortNum)
  {
  return false;
  }
