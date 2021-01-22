#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"

HRESULT Library_corlib_native_CanFly_Syscall::GetDirectoryEntry___STATIC__I4__U4__BYREF_STRING__BYREF_STRING__BYREF_BOOLEAN(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned int param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  char retValue[NAME_MAX+1];
  dir_entry_type et;

  hr = read_directory((handle_t)param0, &et, retValue, NAME_MAX);
  retValue[NAME_MAX] = 0;

  if (failed(hr))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }


  // split the retValue into a path and a name
  size_t len = strlen(retValue);
  size_t pathEnd = len -1;

  while (pathEnd > 0)
    {
    if (retValue[pathEnd] == '/')
      break;

    pathEnd--;
    }

  // everything after pathEnd is the name
  CLR_RT_HeapBlock_String::CreateInstance(*stack.Arg2().Dereference(), retValue + pathEnd, len - pathEnd);

  // terminate the string at the path
  retValue[pathEnd] = 0;
  len = pathEnd;

  if(len > 0)
    CLR_RT_HeapBlock_String::CreateInstance(*stack.Arg1().Dereference(), retValue, len);

  stack.Arg3().Dereference()->SetBoolean(et == et_file);

  SetResult_INT32(stack, s_ok);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RewindDirectoryEntry___STATIC__I4__U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(failed(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  SetResult_INT32(stack, rewind_directory((handle_t)param0));
  
  return S_OK;
  }
