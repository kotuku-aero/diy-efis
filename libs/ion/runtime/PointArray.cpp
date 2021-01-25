#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayCreate___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  uint32_t param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg1().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  vector_p hndl;
  if (failed(hr = vector_create(sizeof(point_t), &hndl)) ||
    failed(hr = vector_expand(hndl, (uint16_t)param0, 0, 0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.Arg1().Dereference()->SetInteger((uint32_t)hndl);

  SetResult_INT32(stack, s_ok);

 return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayRelease___STATIC__I4__U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  uint32_t param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

    SetResult_INT32(stack, vector_close((vector_p)param0));
   
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArraySetPoint___STATIC__I4__U4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  uint32_t param0;
  uint32_t param1;
  int32_t param2;
  int32_t param3;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_UINT32(stack, 1, param1)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 2, param2)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 3, param3)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  vector_p hndl = (vector_p)param0;
  point_t pt = { param2, param3 };

  SetResult_INT32(stack, vector_set(hndl, (uint16_t)param1, &pt));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayClear___STATIC__I4__U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  uint32_t param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  vector_p hndl = (vector_p)param0;
  SetResult_INT32(stack, vector_clear(hndl));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArraySize___STATIC__I4__U4__BYREF_U4(CLR_RT_StackFrame& stack)
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

  vector_p hndl = (vector_p)param0;
  uint16_t count;
  hr = vector_count(hndl, &count);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg1().Dereference()->SetInteger(count);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayResize___STATIC__I4__U4__U4(CLR_RT_StackFrame& stack)
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

  vector_p hndl = (vector_p)param0;
  SetResult_INT32(stack, vector_expand(hndl, (uint16_t)param1, 0, 0));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayAppend___STATIC__I4__U4__I4__I4__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 1, param1)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg3().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  vector_p hndl = (vector_p)param0;
  point_t pt = { param1, param2 };

  hr = vector_push_back(hndl, &pt);
  if (failed(hr))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  uint16_t retValue;
  hr = vector_count(hndl, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg3().Dereference()->SetInteger(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayInsertAt___STATIC__I4__U4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  uint32_t param0;
  uint32_t param1;
  int32_t param2;
  int32_t param3;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_UINT32(stack, 1, param1)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 2, param2)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 3, param3)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  vector_p hndl = (vector_p)param0;
  point_t pt = { param2, param3 };

  SetResult_INT32(stack, vector_insert(hndl, (uint16_t)param1, &pt));
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayGetPoint___STATIC__I4__U4__U4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
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

  if (stack.Arg2().Dereference() == 0 ||
    stack.Arg3().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  vector_p hndl = (vector_p)param0;
  point_t pt;

  hr = vector_at(hndl, (uint16_t)param1, &pt);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    {
    stack.Arg2().Dereference()->SetInteger(pt.x);
    stack.Arg3().Dereference()->SetInteger(pt.y);
    }
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayIndexOf___STATIC__I4__U4__I4__I4__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  uint32_t param0;
  int32_t param1;
  int32_t param2;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 1, param1)) ||
     FAILED(hr =Interop_Marshal_INT32(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  if (stack.Arg3().Dereference() == 0)
    {
    SetResult_INT32(stack, e_bad_pointer);
    return S_OK;
    }

  vector_p hndl = (vector_p)param0;
  point_t pt = { param1, param2 };

  const point_t* begin;
  const point_t* end;

  if (failed(hr = vector_begin(hndl, (void **)&begin)) ||
    failed(hr = vector_end(hndl, (void **)&end)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  uint32_t retValue = 0;
  for (; begin < end; begin++, retValue++)
    {
    if (begin->x == pt.x && begin->y == pt.y)
      break;
    }

  if (begin == end)
    SetResult_INT32(stack, e_not_found);
  else
    {
    SetResult_INT32(stack, s_ok);
    stack.Arg3().Dereference()->SetInteger(retValue);
    }

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PointArrayRemoveAt___STATIC__I4__U4__U4(CLR_RT_StackFrame& stack)
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

  vector_p hndl = (vector_p)param0;

  SetResult_INT32(stack, vector_erase(hndl, (uint16_t)param1));

  return S_OK;
  }
