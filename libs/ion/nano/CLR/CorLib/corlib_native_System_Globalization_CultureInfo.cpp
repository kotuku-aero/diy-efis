//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::GetCurrentUICultureInternal___STATIC__SystemGlobalizationCultureInfo(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  stack.PushValue().SetObjectReference(g_CLR_RT_ExecutionEngine.m_currentUICulture);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::SetCurrentUICultureInternal___STATIC__VOID__SystemGlobalizationCultureInfo(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  g_CLR_RT_ExecutionEngine.m_currentUICulture = stack.Arg0().Dereference();

  NANOCLR_NOCLEANUP_NOLABEL();
  }
