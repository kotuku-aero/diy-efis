//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

static HRESULT Clear(CLR_RT_StackFrame &stack, CLR_RT_HeapBlock &arg, int index, int length)
  {
  (void)stack;

  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_RT_HeapBlock_Array *array;
  int                     numElem;

  NANOCLR_CHECK_HRESULT(CLR_Checks::VerifyArrayReference(arg));

  array = arg.DereferenceArray();
  numElem = array->m_numOfElements;

  if (length == -1) length = numElem;

  if (length < 0 ||
    index          < 0 ||
    length + index > numElem)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

  NANOCLR_SET_AND_LEAVE(array->ClearElements(index, length));

  NANOCLR_NOCLEANUP();
  }

static HRESULT Copy(CLR_RT_StackFrame &stack, CLR_RT_HeapBlock &argSrc, int indexSrc, CLR_RT_HeapBlock &argDst, int indexDst, int length)
  {
  (void)stack;

  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  if (length)
    {
    NANOCLR_CHECK_HRESULT(CLR_Checks::VerifyArrayReference(argSrc));
    NANOCLR_CHECK_HRESULT(CLR_Checks::VerifyArrayReference(argDst));

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Array::Copy(argSrc.DereferenceArray(), indexSrc, argDst.DereferenceArray(), indexDst, length));
    }

  NANOCLR_NOCLEANUP();
  }

static HRESULT TrySzIndexOf(CLR_RT_StackFrame &stack, CLR_RT_HeapBlock &argSrc, CLR_RT_HeapBlock &match, int start, int stop, bool fForward, CLR_RT_HeapBlock &retValue)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  int index;
  bool fRet;
  CLR_RT_HeapBlock_Array *array = argSrc.DereferenceArray();

  NANOCLR_CHECK_HRESULT(CLR_Checks::VerifyArrayReference(argSrc));

  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::IndexOf(array, match, start, stop, fForward, index));

  _ASSERTE(retValue.DataType() == DATATYPE_BYREF);

  retValue.Dereference()->SetInteger(index);

  fRet = (index >= 0);

  if (array->m_typeOfElement <= DATATYPE_LAST_PRIMITIVE)
    {
    //Primitives do not override Equals, so this check is good enough.  
    //If match is not found, index == -1, so the managed Array.IndexOf will
    //skip the more expensive Object.Equals call.

    _ASSERTE(FIMPLIES(!fRet, index == -1));
    fRet = true;
    }

  stack.SetResult_Boolean(fRet);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::GetArrayAt___STATIC__OBJECT__SystemArray__I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_RT_HeapBlock &thisRef = stack.ThisRef();
  int               index = stack.Arg1().NumericByRef().s4;

  CLR_RT_HeapBlock  ref; NANOCLR_CHECK_HRESULT(ref.InitializeArrayReference(thisRef, index));

  {
  CLR_RT_HeapBlock &top = stack.PushValueAndClear();

  NANOCLR_CHECK_HRESULT(top.LoadFromReference(ref));
  NANOCLR_CHECK_HRESULT(top.PerformBoxingIfNeeded());
  }

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::SetArrayAt___STATIC__VOID__SystemArray__I4__OBJECT(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_RT_HeapBlock &thisRef = stack.ThisRef();
  int               index = stack.Arg1().NumericByRef().s4;

  CLR_RT_HeapBlock  ref; NANOCLR_CHECK_HRESULT(ref.InitializeArrayReference(thisRef, index));

  NANOCLR_SET_AND_LEAVE(stack.Arg2().StoreToReference(ref, 0));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::GetArrayLength___STATIC__I4__SystemArray(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  NANOCLR_CHECK_HRESULT(CLR_Checks::VerifyArrayReference(stack.Arg0()));

  stack.SetResult_I4(stack.Arg0().DereferenceArray()->m_numOfElements);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::Copy___STATIC__VOID__SystemArray__I4__SystemArray__I4__I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  NANOCLR_SET_AND_LEAVE(Copy(stack, stack.Arg0(), stack.Arg1().NumericByRef().s4, stack.Arg2(), stack.Arg3().NumericByRef().s4, stack.Arg4().NumericByRef().s4));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::Clear___STATIC__VOID__SystemArray__I4__I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  NANOCLR_SET_AND_LEAVE(Clear(stack, stack.Arg0(), stack.Arg1().NumericByRef().s4, stack.Arg2().NumericByRef().s4));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::TrySzIndexOf___STATIC__BOOLEAN__SystemArray__I4__I4__OBJECT__BYREF_I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_INT32 start = stack.Arg1().NumericByRefConst().s4;
  CLR_INT32 count = stack.Arg2().NumericByRefConst().s4;
  CLR_INT32 stop = start + count;

  NANOCLR_SET_AND_LEAVE(TrySzIndexOf(stack, stack.Arg0(), stack.Arg3(), start, stop, true, stack.Arg4()));

  NANOCLR_NOCLEANUP();
  }

