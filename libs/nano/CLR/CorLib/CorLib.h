//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_CORLIB_NATIVE_H_
#define _NANOCLR_CORLIB_NATIVE_H_

#include "corlib_native.h"
#include "../Include/nanoCLR_Runtime.h"
#include "../Include/nanoCLR_Checks.h"
//#include <nanoCLR_Diagnostics.h>
//#include <nanoCLR_Hardware.h>

#include "../../PAL/Include/nanoCRT.h"

#if !defined(_WIN32)
#include <printf.h>
#endif // !defined(_WIN32)

extern void EventSet(CLR_RT_StackFrame &stack);
extern void EventReset(CLR_RT_StackFrame &stack);
extern HRESULT EventWait(CLR_RT_StackFrame &stack,
  CLR_RT_HeapBlock &blkTimeout,
  CLR_RT_HeapBlock &blkExitContext,
  CLR_RT_HeapBlock *objects,
  int cObjects,
  bool fWaitAll);
extern HRESULT StringConcat(CLR_RT_StackFrame &stack, CLR_RT_HeapBlock *array, int num);

// used for tracking exceptions
struct StackTrace
  {
  CLR_RT_MethodDef_Index m_md;
  CLR_UINT32 m_IP;
  };


#endif
