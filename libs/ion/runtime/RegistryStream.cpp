#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"

HRESULT Library_corlib_native_CanFly_Syscall::RegStreamOpen___STATIC__I4__U4__STRING__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  const char *param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  stream_p retValue;
  hr = reg_stream_open(param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegStreamCreate___STATIC__I4__U4__STRING__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  const char *param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  stream_p retValue;
  hr = reg_stream_create(param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)retValue);

  return S_OK;
  }
