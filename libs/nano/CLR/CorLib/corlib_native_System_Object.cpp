//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


//--//

HRESULT Library_corlib_native_CanFly_Runtime::ObjectEquals___STATIC__BOOLEAN__OBJECT__OBJECT(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  stack.SetResult_Boolean(CLR_RT_HeapBlock::ObjectsEqual(stack.Arg0(), stack.Arg1(), true));

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::ObjectGetHashCode___STATIC__I4__OBJECT(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  stack.SetResult_I4(CLR_RT_HeapBlock::GetHashCode(stack.This(), true, 0));

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::ObjectMemberwiseClone___STATIC__OBJECT__OBJECT(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  NANOCLR_SET_AND_LEAVE(g_CLR_RT_ExecutionEngine.CloneObject(stack.PushValueAndClear(), stack.Arg0()));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::ObjectReferenceEquals___STATIC__BOOLEAN__OBJECT__OBJECT(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  stack.SetResult_Boolean(CLR_RT_HeapBlock::ObjectsEqual(stack.Arg0(), stack.Arg1(), true));

  NANOCLR_NOCLEANUP_NOLABEL();
  }
