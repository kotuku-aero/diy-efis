//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock_Delegate::CreateInstance(CLR_RT_HeapBlock &reference, const uint32_t &ftn, CLR_RT_StackFrame *call)
  {
 
  HRESULT hr;

  reference.SetObjectReference(NULL);

  uint32_t length = 0;

#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)    
  if (call)
    {
    NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(CLR_RT_StackFrame, ptr, call)
      {
      length++;
      }
    NANOCLR_FOREACH_NODE_BACKWARD_END();
    }

  //
  // Limit depth to three callers.
  //
  if (length > 3) length = 3;
#else
  (void)call;
#endif

  uint32_t totLength = (uint32_t)(sizeof(CLR_RT_HeapBlock_Delegate) + length * sizeof(uint32_t));

  CLR_RT_HeapBlock_Delegate *dlg = (CLR_RT_HeapBlock_Delegate *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(DATATYPE_DELEGATE_HEAD, 0, totLength); CHECK_ALLOCATION(dlg);

  reference.SetObjectReference(dlg);

  dlg->ClearData();
  dlg->m_cls.Clear();
  dlg->m_ftn = ftn;
#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
  dlg->m_numOfStackFrames = length;
#endif

  dlg->m_object.SetObjectReference(NULL);

#if defined(NANOCLR_DELEGATE_PRESERVE_STACK)
  if (call)
    {
    uint32_t *callStack = dlg->GetStackFrames();

    NANOCLR_FOREACH_NODE_BACKWARD__DIRECT(CLR_RT_StackFrame, ptr, call)
      {
      if (length-- == 0) break;

      *callStack++ = ptr->m_call.m_index;
      }
    NANOCLR_FOREACH_NODE_BACKWARD_END();
    }
#endif

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_HeapBlock_Delegate::Relocate()
  {
 
  m_object.Relocate__HeapBlock();
  }

