//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::AutoResetEventCtor___STATIC__VOID__SystemThreadingAutoResetEvent__BOOLEAN(CLR_RT_StackFrame& stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_RT_HeapBlock* pThis = stack.Arg0().Dereference();

  pThis->SetFlags(CLR_RT_HeapBlock::HB_SignalAutoReset);

  if (stack.Arg1().NumericByRef().s4)
    {
    pThis->SetFlags(CLR_RT_HeapBlock::HB_Signaled);
    }

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::AutoResetEventReset___STATIC__BOOLEAN__SystemThreadingAutoResetEvent(CLR_RT_StackFrame& stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  EventReset(stack);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::AutoResetEventSet___STATIC__BOOLEAN__SystemThreadingAutoResetEvent(CLR_RT_StackFrame& stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  EventSet(stack);

  NANOCLR_NOCLEANUP_NOLABEL();
  }
