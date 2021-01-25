//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"
#include "../../PAL/Include/nanoPAL_NativeDouble.h"

HRESULT Library_corlib_native_CanFly_Runtime::CompareTo___STATIC__I4__R8__R8(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  double d = stack.Arg0().NumericByRefConst().r8;
  double val = stack.Arg1().NumericByRefConst().r8;
  int32_t res = System::Double::CompareTo(d, val);

  stack.PushValue().SetInteger(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::IsInfinity___STATIC__BOOLEAN__R8(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  double d = stack.Arg0().NumericByRefConst().r8;
  bool res = System::Double::IsInfinity(d);

  stack.SetResult_Boolean(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::IsNaN___STATIC__BOOLEAN__R8(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  double d = stack.Arg0().NumericByRefConst().r8;
  bool res = System::Double::IsNaN(d);

  stack.SetResult_Boolean(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::IsNegativeInfinity___STATIC__BOOLEAN__R8(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  double d = stack.Arg0().NumericByRefConst().r8;
  bool res = System::Double::IsNegativeInfinity(d);

  stack.SetResult_Boolean(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::IsPositiveInfinity___STATIC__BOOLEAN__R8(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  double d = stack.Arg0().NumericByRefConst().r8;
  bool res = System::Double::IsPositiveInfinity(d);

  stack.SetResult_Boolean(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

