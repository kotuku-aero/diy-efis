#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

HRESULT Library_corlib_native_CanFly_Syscall::GetFont___STATIC__I4__STRING__U4__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  const char* param0;
  unsigned int param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 0, param0)) ||
    FAILED(hr =Interop_Marshal_UINT32(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t font;
  hr = open_font(param0, (uint16_t)param1, &font);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((CLR_INT32)font);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::ReleaseFont___STATIC__I4__U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  // fonts are never closed (yet)
  stack.SetResult_I4(s_ok);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::LoadFont___STATIC__I4__U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(load_font((handle_t)param0));
  
  return S_OK;
  }
