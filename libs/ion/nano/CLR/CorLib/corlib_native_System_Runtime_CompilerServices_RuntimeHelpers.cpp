//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_CanFly_Runtime::InitializeArray___STATIC__VOID__SystemArray__SystemRuntimeFieldHandle(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  CLR_RT_FieldDef_Instance inst;
  CLR_RT_HeapBlock_Array* array = stack.Arg0().DereferenceArray(); FAULT_ON_NULL(array);

  if (array->m_fReference)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

  if (CLR_RT_ReflectionDef_Index::Convert(stack.Arg1(), inst) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  if ((inst.m_target->flags & CLR_RECORD_FIELDDEF::FD_HasFieldRVA) == 0 || inst.m_target->defaultValue == CLR_EmptyIndex)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  {
  CLR_PMETADATA ptrSrc = inst.m_assm->GetSignature(inst.m_target->defaultValue);
  uint32_t    lenSrc; NANOCLR_READ_UNALIGNED_UINT16(lenSrc, ptrSrc);

  uint8_t* ptrDst = array->GetFirstElement();
  uint32_t    lenDst = array->m_numOfElements;
  uint32_t    sizeDst = array->m_sizeOfElement;

  lenSrc /= sizeDst; if (lenSrc > lenDst) lenSrc = lenDst;
#if !defined(NETMF_TARGET_BIG_ENDIAN)
  memcpy(ptrDst, ptrSrc, lenSrc * sizeDst);

#else
  // FIXME GJS - WOuld it be possible to move the endian swap to pe compile time to get rid of this?
  // If this is a numeric dataype of datatype size other than a byte then byteswap the entries
  // Unaligned reads handle endianess, just use them. FIXME GJS - this could be the subject of much optimization
  switch (array->m_typeOfElement)
    {
    case DATATYPE_CHAR:
    case DATATYPE_I2:
    case DATATYPE_U2:
    {
    uint32_t  count = 0;
    uint16_t  d;
    uint16_t* p16 = (uint16_t*)ptrDst;
    while (count < lenDst)
      {
      NANOCLR_READ_UNALIGNED_UINT16(d, ptrSrc);
      *p16++ = SwapEndian(d);
      count++;
      }
    }
    break;

    case DATATYPE_I4:
    case DATATYPE_U4:
    case DATATYPE_R4:
    {
    uint32_t  count = 0;
    uint32_t  d;
    uint32_t* p32 = (uint32_t*)ptrDst;
    while (count < lenDst)
      {
      NANOCLR_READ_UNALIGNED_UINT32(d, ptrSrc);
      *p32++ = SwapEndian(d);
      count++;
      }
    }
    break;
    case DATATYPE_I8:
    case DATATYPE_U8:
    case DATATYPE_R8:
    {
    uint32_t  count = 0;
    uint64_t  d;
    uint64_t* p64 = (uint64_t*)ptrDst;

    while (count < lenDst)
      {
      NANOCLR_READ_UNALIGNED_UINT64(d, ptrSrc);
      *p64++ = SwapEndian(d);
      count++;
      }
    }
    break;
    default:
      memcpy(ptrDst, ptrSrc, lenSrc * sizeDst);
      break;
    }



#if defined(NANOCLR_EMULATED_FLOATINGPOINT)
  switch (array->m_typeOfElement)
    {
    case DATATYPE_R4:
    {
    CLR_RT_HeapBlock tmp;
    uint32_t* ptr = (uint32_t*)ptrDst;

    for (; lenSrc; lenSrc--, ptr++)
      {
      NANOCLR_CHECK_HRESULT(tmp.SetFloatIEEE754(*ptr));

      *ptr = tmp.NumericByRef().u4;
      }
    }
    break;

    case DATATYPE_R8:
    {
    CLR_RT_HeapBlock tmp;
    uint64_t* ptr = (uint64_t*)ptrDst;

    for (; lenSrc; lenSrc--, ptr++)
      {
      NANOCLR_CHECK_HRESULT(tmp.SetDoubleIEEE754(*ptr));

      *ptr = tmp.NumericByRef().u8;
      }
    }
    break;
    }
#endif        
#endif //NETMF_TARGET_BIG_ENDIAN
  }

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::GetObjectValue___STATIC__OBJECT__OBJECT(CLR_RT_StackFrame& stack)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock& top = stack.PushValueAndClear();
  CLR_RT_HeapBlock* src = stack.Arg0().Dereference();

  if (src && src->DataType() == DATATYPE_VALUETYPE && src->IsBoxed())
    {
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.CloneObject(top, *src));
    }
  else
    {
    top.SetObjectReference(src);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::RunClassConstructor___STATIC__VOID__SystemRuntimeTypeHandle(CLR_RT_StackFrame& stack)
  {
  (void)stack;

 
  HRESULT hr;

  NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::OffsetToStringData___STATIC__I4(CLR_RT_StackFrame& stack)
  {
  (void)stack;

 
  HRESULT hr;

  NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

  NANOCLR_NOCLEANUP();
  }
