//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::ValueTypeEquals___STATIC__BOOLEAN__SystemValueType__OBJECT(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  stack.SetResult_Boolean(CLR_RT_HeapBlock::ObjectsEqual(stack.Arg0(), stack.Arg1(), false));

  NANOCLR_NOCLEANUP_NOLABEL();
  }
