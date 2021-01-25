//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::DebuggerIsAttached___STATIC__BOOLEAN(CLR_RT_StackFrame &stack)
  {
  stack.SetResult_Boolean(true);
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Runtime::DebuggerBreak___STATIC__VOID(CLR_RT_StackFrame &stack)
  {
  debugger_event_args_t args;
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_BREAK;
  args.pid = stack.m_owningThread->m_pid;

  return debug_breakpoint(&args);
  }
