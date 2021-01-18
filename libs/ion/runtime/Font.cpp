#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

HRESULT Library_corlib_native_CanFly_Syscall::GetFont___STATIC__U4__STRING__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  const char* param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_LPCSTR(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  handle_t font;
  hr = open_font(param0, (uint16_t)param1, &font);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, (uint32_t)font);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::ReleaseFont___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  // fonts are never closed (yet)
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::LoadFont___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = load_font((handle_t)param0);
  
  NANOCLR_NOCLEANUP();
  }
