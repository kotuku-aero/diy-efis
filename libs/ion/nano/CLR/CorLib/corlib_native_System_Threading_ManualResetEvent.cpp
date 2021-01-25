//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::ManualResetEventCtor___STATIC__VOID__SystemThreadingManualResetEvent__BOOLEAN(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock* pThis = stack.Arg0().Dereference();

  if (stack.Arg1().NumericByRef().s4)
    {
    pThis->SetFlags(CLR_RT_HeapBlock::HB_Signaled);
    }

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::ManualResetEventReset___STATIC__BOOLEAN__SystemThreadingManualResetEvent(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  EventReset(stack);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::ManualResetEventSet___STATIC__BOOLEAN__SystemThreadingManualResetEvent(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  EventSet(stack);

  NANOCLR_NOCLEANUP_NOLABEL();
  }
