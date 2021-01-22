#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"


HRESULT Library_corlib_native_CanFly_Syscall::FileStreamOpen___STATIC__I4__STRING__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  const char *param0;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  stream_p retValue;
  hr = stream_open(param0, &retValue);
  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((CLR_UINT32)retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::FileStreamCreate___STATIC__I4__STRING__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  const char *param0;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  stream_p retValue;
  hr = stream_create(param0, &retValue);

  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((CLR_UINT32)retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::CreateDirectory___STATIC__I4__STRING(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  const char *param0;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  hr = create_directory(param0);
  SetResult_INT32(stack, hr);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RemoveDirectory___STATIC__I4__STRING(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  const char *param0;
  if (FAILED(hr = Interop_Marshal_LPCSTR(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  hr = remove_directory(param0);
  SetResult_INT32(stack, hr);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetDirectoryEnumerator___STATIC__I4__STRING__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;
  const char *param0;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  handle_t dirp;
  hr = open_directory(param0, &dirp);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((CLR_UINT32)dirp);

  return S_OK;
  }
