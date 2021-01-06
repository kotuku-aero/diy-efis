/*
#include "../../nano/CLR/Include/nanoCLR_Interop.h"
#include "../../nano/CLR/Include/nanoCLR_Runtime.h"
#include "../../nano/CLR/Include/nanoPackStruct.h"
*/
#include "../canflylib/CanFly_CoreLibrary.h"

#include "../../neutron/neutron.h"


HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegCreateKey___STATIC__U4__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  memid_t key;
  hr = reg_create_key((memid_t)param0, param1, &key);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, key);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegOpenKey___STATIC__U4__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  memid_t key;
  hr = reg_open_key((memid_t)param0, param1, &key);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, key);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetInt8___STATIC__I1__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  int8_t retValue;
  hr = reg_get_int8((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_INT8(stack, retValue);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetInt8___STATIC__VOID__U4__STRING__I1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  int8_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT8(stack, 2, param2));

  hr = reg_set_int8((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetUint8___STATIC__U1__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  uint8_t retValue;
  hr = reg_get_uint8((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT8(stack, retValue);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetUint8___STATIC__VOID__U4__STRING__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  uint8_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 2, param2));

  hr = reg_set_uint8((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetInt16___STATIC__I2__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  int16_t retValue;
  hr = reg_get_int16((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_INT16(stack, retValue);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetInt16___STATIC__VOID__U4__STRING__I2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  int16_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT16(stack, 2, param2));

  hr = reg_set_int16((memid_t)param0, param1, param2);

  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetUint16___STATIC__U2__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  uint16_t retValue;
  hr = reg_get_uint16((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT16(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetUint16___STATIC__VOID__U4__STRING__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  uint16_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param2));

  hr = reg_set_uint16((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetInt32___STATIC__I4__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  signed int retValue;
  hr = reg_get_int32((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_INT32(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetInt32___STATIC__VOID__U4__STRING__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  hr = reg_set_int32((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetUint32___STATIC__U4__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  unsigned int retValue;
  hr = reg_get_uint32((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetUint32___STATIC__VOID__U4__STRING__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  unsigned int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 2, param2));

  hr = reg_set_uint32((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetString___STATIC__STRING__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  char retValue[REG_STRING_MAX+1];
  uint16_t len = REG_STRING_MAX;

  hr = reg_get_string((memid_t)param0, param1, retValue, &len);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_LPCSTR(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetString___STATIC__VOID__U4__STRING__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  const char* param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 2, param2));

  hr = reg_set_string((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetFloat___STATIC__R4__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  float retValue;
  hr = reg_get_float((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_float(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetFloat___STATIC__VOID__U4__STRING__R4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  float param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_float(stack, 2, param2));

  hr = reg_set_float((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegGetBool___STATIC__BOOLEAN__U4__STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  bool retValue;
  hr = reg_get_bool((memid_t)param0, param1, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_bool(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Neutron::RegSetBool___STATIC__VOID__U4__STRING__BOOLEAN(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char* param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  bool param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_bool(stack, 2, param2));

  hr = reg_set_bool((memid_t)param0, param1, param2);
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }