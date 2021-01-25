//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::DelegateEquals___STATIC__BOOLEAN__SystemDelegate__OBJECT(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  stack.SetResult_Boolean(CLR_RT_HeapBlock::Compare_Unsigned_Values(stack.Arg0(), stack.Arg1()) == 0);

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT Library_corlib_native_CanFly_Runtime::DelegateCombine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Delegate_List::Combine(stack.PushValue(), stack.Arg0(), stack.Arg1(), false));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::DelegateRemove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate(CLR_RT_StackFrame &stack)
  {
 
  HRESULT hr;

  NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Delegate_List::Remove(stack.PushValue(), stack.Arg0(), stack.Arg1()));

  NANOCLR_NOCLEANUP();
  }

//--//
/*
CLR_RT_HeapBlock_Delegate *GetLastDelegate(CLR_RT_HeapBlock_Delegate *dlg)
  {
 
  if (dlg)
    {
    if (dlg->DataType() == DATATYPE_DELEGATELIST_HEAD)
      {
      CLR_RT_HeapBlock_Delegate_List *lst = (CLR_RT_HeapBlock_Delegate_List *)dlg;

      if (lst->m_length == 0)
        {
        dlg = NULL;
        }
      else
        {
        CLR_RT_HeapBlock *ptr = lst->GetDelegates();

        dlg = ptr[lst->m_length - 1].DereferenceDelegate();
        }
      }
    }

  return dlg;
  }
  */