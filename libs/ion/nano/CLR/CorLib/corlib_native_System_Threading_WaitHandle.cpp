//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

void EventSet(CLR_RT_StackFrame &stack)
  {
 
  CLR_RT_HeapBlock *pThis = stack.Arg0().Dereference();

  CLR_RT_HeapBlock_WaitForObject::SignalObject(*pThis);

  stack.SetResult_Boolean(true);
  }

void EventReset(CLR_RT_StackFrame &stack)
  {
 
  CLR_RT_HeapBlock *pThis = stack.Arg0().Dereference();

  pThis->ResetFlags(CLR_RT_HeapBlock::HB_Signaled);

  stack.SetResult_Boolean(true);
  }

HRESULT EventWait(CLR_RT_StackFrame &stack,
  CLR_RT_HeapBlock &blkTimeout, 
  CLR_RT_HeapBlock &blkExitContext, 
  CLR_RT_HeapBlock *objects,
  int cObjects,
  bool fWaitAll)
  {
  (void)blkExitContext;

 
  HRESULT hr;

  int64_t timeExpire;

  NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitTimeout(timeExpire, blkTimeout.NumericByRef().s4));

  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_WaitForObject::WaitForSignal(stack, timeExpire, objects, cObjects, fWaitAll));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::WaitHandleWaitOne___STATIC__BOOLEAN__SystemThreadingWaitHandle__I4__BOOLEAN(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  NANOCLR_CHECK_HRESULT(EventWait(stack, stack.Arg1(), stack.Arg2(), &stack.ThisRef(), 1, false));

  stack.SetResult_Boolean(stack.m_owningThread->m_waitForObject_Result != CLR_RT_Thread::TH_WAIT_RESULT_TIMEOUT);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::WaitMultiple___STATIC__I4__SZARRAY_SystemThreadingWaitHandle__I4__BOOLEAN__BOOLEAN(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_Array* arr = stack.Arg0().DereferenceArray();  FAULT_ON_NULL(arr);

  NANOCLR_CHECK_HRESULT(EventWait(stack, stack.Arg1(), stack.Arg2(), (CLR_RT_HeapBlock*)arr->GetFirstElement(), arr->m_numOfElements, stack.Arg3().NumericByRef().s1 == 1));

  stack.SetResult_I4(stack.m_owningThread->m_waitForObject_Result);

  NANOCLR_NOCLEANUP();
  }

//--//
