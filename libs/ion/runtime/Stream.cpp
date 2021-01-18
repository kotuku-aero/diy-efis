#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"


HRESULT Library_corlib_native_CanFly_Syscall::StreamEof___STATIC__BOOLEAN__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = stream_eof((stream_p)param0);
  bool retValue = hr == s_ok;

  if (hr != s_ok && hr != s_false)
    {
    NANOCLR_CHECK_HRESULT(hr);
    }
  else
    hr = s_ok;

  SetResult_bool(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamRead___STATIC__SZARRAY_U1__U4__U2(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  if (param1 == 0)
    return e_bad_parameter;

  // create the return value
  CLR_RT_HeapBlock &returnBlk = stack.PushValue();

  NANOCLR_CHECK_HRESULT(
    CLR_RT_HeapBlock_Array::CreateInstance(returnBlk, param1, g_CLR_RT_WellKnownTypes.m_UInt8));

  CLR_RT_HeapBlock_Array *array = returnBlk.Array();

  uint8_t *buffer = array->GetElement(0);

  hr = stream_read((stream_p)param0, buffer, param1, 0);

  if (failed(hr))
    stack.Pop();  // remove the failing result

  NANOCLR_CHECK_HRESULT(hr);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamWrite___STATIC__VOID__U4__SZARRAY_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  CLR_RT_TypedArray_UINT8 param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8_ARRAY(stack, 1, param1));

  hr = stream_write((stream_p)param0, param1.GetBuffer(), (uint16_t)param1.GetSize());

  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamGetPos___STATIC__U4__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int retValue;
  hr = stream_getpos((stream_p)param0, &retValue);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamSetPos___STATIC__VOID__U4__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  hr = stream_setpos((stream_p)param0, param1);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamLength___STATIC__U4__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int retValue;

  hr = stream_length((stream_p)param0, &retValue);
  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, retValue);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamTruncate___STATIC__VOID__U4__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  hr = stream_truncate((stream_p)param0, param1);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamCopy___STATIC__VOID__U4__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  hr = stream_copy((stream_p)param0, (stream_p)param1);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamPath___STATIC__STRING__U4__BOOLEAN(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  bool param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_bool(stack, 1, param1));

  char *retValue = (char *)neutron_malloc(NAME_MAX + 1);

  hr = stream_path((stream_p)param0, param1, NAME_MAX, retValue);
  retValue[NAME_MAX] = 0;

  if (failed(hr))
    neutron_free(retValue);

  NANOCLR_CHECK_HRESULT(hr);

  SetResult_LPCSTR(stack, retValue);

  neutron_free(retValue);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamClose___STATIC__VOID__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = stream_close((stream_p)param0);

  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::StreamDelete___STATIC__VOID__U4(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = stream_delete((stream_p)param0);
  NANOCLR_CHECK_HRESULT(hr);

  }
  NANOCLR_NOCLEANUP();
  }
