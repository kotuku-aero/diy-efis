#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"


HRESULT Library_corlib_native_CanFly_Syscall::FileStreamOpen___STATIC__U4__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  const char *param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  stream_p retValue;
  hr = stream_open(param0, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (uint32_t) retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::FileStreamCreate___STATIC__U4__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {
  const char *param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  stream_p retValue;
  hr = stream_create(param0, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (uint32_t)retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreateDirectory___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  const char *param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  hr = create_directory(param0);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::RemoveDirectory___STATIC__VOID__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  const char *param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  hr = remove_directory(param0);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetDirectoryEnumerator___STATIC__U4__STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  const char *param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  handle_t dirp;
  hr = open_directory(param0, &dirp);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, (uint32_t)dirp);
  }
  NANOCLR_NOCLEANUP();
  }
