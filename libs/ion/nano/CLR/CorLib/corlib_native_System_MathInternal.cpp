//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

HRESULT Library_corlib_native_CanFly_Runtime::Abs___STATIC__I4__I4(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  int32_t d = stack.Arg0().NumericByRefConst().s4;
  int32_t res = abs(d);

  stack.SetResult_I4(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::Max___STATIC__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  int32_t x = stack.Arg0().NumericByRefConst().s4;
  int32_t y = stack.Arg1().NumericByRefConst().s4;
  int32_t res = x >= y ? x : y;

  stack.SetResult_I4(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::Min___STATIC__I4__I4__I4(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  int32_t x = stack.Arg0().NumericByRefConst().s4;
  int32_t y = stack.Arg1().NumericByRefConst().s4;
  int32_t res = x <= y ? x : y;

  stack.SetResult_I4(res);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

