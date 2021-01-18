#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"


HRESULT Library_corlib_native_CanFly_Syscall::GetDirectoryEntry___STATIC__BOOLEAN__U4__BYREF_STRING__BYREF_STRING(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  char *retValue = (char *)neutron_malloc(NAME_MAX + 1);
  dir_entry_type et;

  hr = read_directory((handle_t)param0, &et, retValue, NAME_MAX);
  retValue[NAME_MAX] = 0;

  if (failed(hr))
    neutron_free(retValue);

  NANOCLR_CHECK_HRESULT(hr);

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
  CLR_RT_HeapBlock_String::CreateInstance(stack.Arg2(), retValue + pathEnd, len - pathEnd);

  // terminate the string at the path
  retValue[pathEnd] = 0;
  len = pathEnd;

  if(len > 0)
    CLR_RT_HeapBlock_String::CreateInstance(stack.Arg1(), retValue, len);

  SetResult_bool(stack, et == et_file);

  neutron_free(retValue);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::RewindDirectoryEntry___STATIC__VOID__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = rewind_directory((handle_t)param0);
  
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }
