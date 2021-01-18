#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"

HRESULT Library_corlib_native_CanFly_Syscall::RegStreamOpen___STATIC__U4__U4__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char *param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  stream_p retValue;
  hr = reg_stream_open(param0, param1, &retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (uint32_t) retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegStreamCreate___STATIC__U4__U4__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  const char *param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 1, param1));

  stream_p retValue;
  hr = reg_stream_create(param0, param1, &retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (uint32_t) retValue);
  }
  NANOCLR_NOCLEANUP();
  }
