#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"


HRESULT Library_corlib_native_CanFly_Syscall::StreamEof___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, stream_eof((stream_p)param0));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamRead___STATIC__I4__U4__U2__BYREF_SZARRAY_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint16_t param1;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (param1 == 0)
    {
    SetResult_INT32(stack, e_bad_parameter);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  // create the return value
  CLR_RT_HeapBlock &returnBlk = stack.PushValue();

  if(FAILED(hr = CLR_RT_HeapBlock_Array::CreateInstance(stack.Arg2(), param1, g_CLR_RT_WellKnownTypes.m_UInt8)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  CLR_RT_HeapBlock_Array *array = stack.Arg2().Dereference()->Array();

  uint8_t *buffer = array->GetElement(0);

  hr = stream_read((stream_p)param0, buffer, param1, 0);
  SetResult_INT32(stack, hr);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamWrite___STATIC__I4__U4__SZARRAY_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  CLR_RT_TypedArray_UINT8 param1;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
    FAILED(hr = Interop_Marshal_UINT8_ARRAY(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, stream_write((stream_p)param0, param1.GetBuffer(), (uint16_t)param1.GetSize()));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamGetPos___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint32_t retValue;
  hr = stream_getpos((stream_p)param0, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger((uint32_t)retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamSetPos___STATIC__I4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_UINT32(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, stream_setpos((stream_p)param0, param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamLength___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  uint32_t retValue;

  hr = stream_length((stream_p)param0, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamTruncate___STATIC__I4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_UINT32(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, stream_truncate((stream_p)param0, param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamCopy___STATIC__I4__U4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_UINT32(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  SetResult_INT32(stack, stream_copy((stream_p)param0, (stream_p)param1));
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamPath___STATIC__I4__U4__BOOLEAN__BYREF_STRING(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  bool param1;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_bool(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg2().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  char retValue[NAME_MAX + 1];

  hr = stream_path((stream_p)param0, param1, NAME_MAX, retValue);
  retValue[NAME_MAX] = 0;

  SetResult_INT32(stack, hr);

  if(succeeded(hr))
    CLR_RT_HeapBlock_String::CreateInstance(*stack.Arg2().Dereference(), retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamClose___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, stream_close((stream_p)param0));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamDelete___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, stream_delete((stream_p)param0));
  
  return S_OK;
  }
