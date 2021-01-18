//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

static HRESULT GetRandom(CLR_RT_StackFrame &stack, CLR_RT_Random *&rand, bool create)
  {
  NANOCLR_HEADER();

  CLR_RT_HeapBlock *pThis = stack.Arg0().Dereference();
  FAULT_ON_NULL(pThis);

  if (create)
    {
    NANOCLR_CHECK_HRESULT(
      CLR_RT_HeapBlock_BinaryBlob::CreateInstance(pThis[Library_corlib_native_System_Random::FIELD___random], sizeof(CLR_RT_Random), NULL, NULL, 0));
    }

  rand = (CLR_RT_Random *)pThis[Library_corlib_native_System_Random::FIELD___random].DereferenceBinaryBlob()->GetData();

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::RandomNext___STATIC__I4__SystemRandom(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();

  CLR_RT_Random* rand;

  NANOCLR_CHECK_HRESULT(GetRandom(stack, rand, false));

  stack.SetResult_I4(rand->Next() & 0x7FFFFFFF);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::RandomNext___STATIC__I4__SystemRandom__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();

  CLR_RT_Random* rand;
  CLR_INT32      maxValue;

  NANOCLR_CHECK_HRESULT(GetRandom(stack, rand, false));

  maxValue = stack.Arg1().NumericByRef().s4;

  if (maxValue <= 0) NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);

  stack.SetResult_I4(rand->Next() % maxValue);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::RandomNextDouble___STATIC__R8__SystemRandom(CLR_RT_StackFrame& stack)
  {


  NANOCLR_HEADER();

  CLR_RT_Random* rand;

#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)

  double     nextDouble;
#else
  CLR_INT64  nextDouble;

#endif

  NANOCLR_CHECK_HRESULT(GetRandom(stack, rand, false));

  nextDouble = rand->NextDouble();

  stack.SetResult_R8(nextDouble);

  NANOCLR_NOCLEANUP();


  }

HRESULT Library_corlib_native_CanFly_Runtime::RandomNextBytes___STATIC__VOID__SystemRandom__SZARRAY_U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();

  CLR_RT_Random* rand;
  CLR_RT_HeapBlock_Array* buffer;

  NANOCLR_CHECK_HRESULT(GetRandom(stack, rand, false));

  buffer = stack.Arg1().DereferenceArray(); FAULT_ON_NULL(buffer);

  rand->NextBytes(buffer->GetFirstElement(), buffer->m_numOfElements);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::RandomCtor___STATIC__VOID__SystemRandom(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();

  CLR_RT_Random* rand;

  NANOCLR_CHECK_HRESULT(GetRandom(stack, rand, true));

  rand->Initialize();

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::RandomCtor___STATIC__VOID__SystemRandom__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();

  CLR_RT_Random* rand;

  NANOCLR_CHECK_HRESULT(GetRandom(stack, rand, true));

  rand->Initialize(stack.Arg1().NumericByRef().s4);

  NANOCLR_NOCLEANUP();
  }

//--//
