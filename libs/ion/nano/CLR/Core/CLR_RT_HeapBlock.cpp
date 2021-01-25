//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"
#include "../../HAL/Include/nanoHAL.h"
#include "../../PAL/Include/nanoPAL_NativeDouble.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_HeapBlock::InitializeToZero()
  {
 
  CLR_RT_Memory::ZeroFill(&m_data, this->DataSize() * sizeof(*this) - offsetof(CLR_RT_HeapBlock, m_data));
  }


HRESULT CLR_RT_HeapBlock::EnsureObjectReference(CLR_RT_HeapBlock *&obj)
  {
 
  HRESULT hr;

  switch (this->DataType())
    {
    case DATATYPE_OBJECT:
    case DATATYPE_BYREF:
    {
    obj = Dereference();
    FAULT_ON_NULL(obj);

    switch (obj->DataType())
      {
      case DATATYPE_CLASS:
      case DATATYPE_VALUETYPE:
      case DATATYPE_DATETIME: // Special case.
      case DATATYPE_TIMESPAN: // Special case.
        NANOCLR_SET_AND_LEAVE(S_OK);

      default:
        // the remaining data types aren't to be handled
        break;
    }
    }
    break;

    case DATATYPE_DATETIME: // Special case.
    case DATATYPE_TIMESPAN: // Special case.
      obj = this;
      NANOCLR_SET_AND_LEAVE(S_OK);

    default:
      // the remaining data types aren't to be handled
      break;
  }

  NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_HeapBlock::SetReflection(const CLR_RT_ReflectionDef_Index &reflex)
  {
 
  HRESULT hr;

  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_REFLECTION, 0, 1);
  m_data.reflection = reflex;

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT CLR_RT_HeapBlock::SetReflectionAssembly(uint32_t assm)
  {
 
  HRESULT hr;

  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_REFLECTION, 0, 1);
  m_data.reflection.m_kind = REFLECTION_ASSEMBLY;
  m_data.reflection.m_levels = 0;
  m_data.reflection.m_data.m_assm = assm;

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT CLR_RT_HeapBlock::SetReflection(uint32_t sig)
  {
 
  HRESULT hr;

  CLR_RT_TypeDescriptor desc;

  NANOCLR_CHECK_HRESULT(desc.InitializeFromTypeSpec(sig));

  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_REFLECTION, 0, 1);
  m_data.reflection = desc.m_reflex;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::SetReflectionClass(uint32_t cls)
  {
 
  HRESULT hr;

  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_REFLECTION, 0, 1);
  m_data.reflection.m_kind = REFLECTION_TYPE;
  m_data.reflection.m_levels = 0;
  m_data.reflection.m_data.m_type = cls;

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT CLR_RT_HeapBlock::SetReflectionField(uint32_t fd)
  {
 
  HRESULT hr;

  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_REFLECTION, 0, 1);
  m_data.reflection.m_kind = REFLECTION_FIELD;
  m_data.reflection.m_levels = 0;
  m_data.reflection.m_data.m_field = fd;

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT CLR_RT_HeapBlock::SetReflectionMethod(uint32_t md)
  {
 
  HRESULT hr;

  CLR_RT_MethodDef_Instance inst;

  if (inst.InitializeFromIndex(md) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_REFLECTION, 0, 1);
  m_data.reflection.m_kind =
    (inst.m_target->flags & CLR_RECORD_METHODDEF::MD_Constructor) ? REFLECTION_CONSTRUCTOR : REFLECTION_METHOD;
  m_data.reflection.m_levels = 0;
  m_data.reflection.m_data.m_method = md;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::SetObjectCls(uint32_t cls)
  {
 
  HRESULT hr;

  CLR_RT_TypeDef_Instance inst;

  if (inst.InitializeFromIndex(cls) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

  m_data.objectHeader.cls = cls;
  m_data.objectHeader.lock = NULL;

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_HeapBlock::InitializeArrayReference(CLR_RT_HeapBlock &ref, int index)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_Array *array;

  if (ref.DataType() != DATATYPE_OBJECT)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  array = ref.DereferenceArray();
  FAULT_ON_NULL(array);

  if (array->DataType() != DATATYPE_SZARRAY)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  if (index < 0 || index >= (int32_t)array->m_numOfElements)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
    }

  InitializeArrayReferenceDirect(*array, index);

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_HeapBlock::InitializeArrayReferenceDirect(CLR_RT_HeapBlock_Array &array, int index)
  {
 
  m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_ARRAY_BYREF, 0, 1);
  m_data.arrayReference.array = &array;
  m_data.arrayReference.index = index;
  }

void CLR_RT_HeapBlock::FixArrayReferenceForValueTypes()
  {
 
  CLR_RT_HeapBlock_Array *array = m_data.arrayReference.array;

  //
  // ValueTypes are implemented as pointers to objects,
  // so getting an array reference to a ValueType has to be treated like assigning a pointer!
  //
  // DateTime and TimeSpan are optimized as primitive types,
  // so getting an array reference to them is like getting a reference to them.
  //
  switch (array->m_typeOfElement)
    {
    case DATATYPE_VALUETYPE:
      this->SetReference(*(CLR_RT_HeapBlock *)array->GetElement(m_data.arrayReference.index));
      break;

    case DATATYPE_DATETIME:
    case DATATYPE_TIMESPAN:
      m_id.raw = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_BYREF, 0, 1);
      m_data.objectReference.ptr = (CLR_RT_HeapBlock *)array->GetElement(m_data.arrayReference.index);
      break;
    }
  }

HRESULT CLR_RT_HeapBlock::LoadFromReference(CLR_RT_HeapBlock &ref)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock tmp;
  CLR_RT_HeapBlock *obj;
  CLR_DataType dt = ref.DataType();

  if (dt == DATATYPE_ARRAY_BYREF)
    {
    CLR_RT_HeapBlock_Array *array = ref.m_data.arrayReference.array;
    FAULT_ON_NULL(array);
    uint8_t *src = array->GetElement(ref.m_data.arrayReference.index);
    uint32_t size = array->m_sizeOfElement;

    if (!array->m_fReference)
      {
      uint32_t second = 0;
      uint32_t first;

      SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(array->m_typeOfElement, 0, 1));

      if (size == 4)
        {
        first = ((uint32_t *)src)[0];
        }
      else if (size == 8)
        {
        first = ((uint32_t *)src)[0];
        second = ((uint32_t *)src)[1];
        }
      else if (size == 1)
        {
        first = ((uint8_t *)src)[0];
        }
      else
        {
        first = ((uint16_t *)src)[0];
        }

      ((uint32_t *)&NumericByRef())[0] = first;
      ((uint32_t *)&NumericByRef())[1] = second;

      NANOCLR_SET_AND_LEAVE(S_OK);
      }

    //
    // It's a pointer to a full CLR_RT_HeapBlock.
    //
    obj = (CLR_RT_HeapBlock *)src;
    }
  else if (dt == DATATYPE_BYREF)
    {
    obj = ref.Dereference();
    FAULT_ON_NULL(obj);

    if (obj->DataType() == DATATYPE_VALUETYPE)
      {
      tmp.SetObjectReference(obj);

      obj = &tmp;
      }
    }
  else if (c_CLR_RT_DataTypeLookup[dt].m_flags & CLR_RT_DataTypeLookup::c_Direct)
    {
    obj = &ref;

    if (dt == DATATYPE_OBJECT)
      {
      CLR_RT_HeapBlock *objT = ref.Dereference();

      if (objT && objT->IsBoxed())
        {
        CLR_RT_TypeDef_Instance inst;
        if (objT->DataType() != DATATYPE_VALUETYPE)
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }

        if (!inst.InitializeFromIndex(objT->ObjectCls()))
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
          }

        if (inst.m_target->dataType != DATATYPE_VALUETYPE) // It's a boxed primitive/enum type.
          {
          obj = &objT[1];
          }
        }
      }
    }
  else
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  if (obj->IsAValueType())
    {
    NANOCLR_SET_AND_LEAVE(g_CLR_RT_ExecutionEngine.CloneObject(*this, *obj));
    }

  this->Assign(*obj);

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::StoreToReference(CLR_RT_HeapBlock &ref, int size)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock *obj;
  CLR_DataType dt = ref.DataType();

  if (dt == DATATYPE_ARRAY_BYREF)
    {
    CLR_RT_HeapBlock_Array *array = ref.m_data.arrayReference.array;
    FAULT_ON_NULL(array);
    uint8_t *dst = array->GetElement(ref.m_data.arrayReference.index);

    if (!array->m_fReference)
      {
      int32_t sizeArray = array->m_sizeOfElement;

      //
      // Cannot copy NULL reference to a primitive type array.
      //
      obj = FixBoxingReference();
      FAULT_ON_NULL(obj);

      if (size == -1)
        {
        // size == -1 tells StoreToReference to allow the value 'this' to have more precision than the dest
        // array.  This fixes the following bug.
        //  :  conv.u1
        //  :  stobj      [mscorlib]System.Byte
        // The conv.u1 will convert the top of the eval stack to a u1.  But since the eval stack is required
        // to contain at least 4 byte values, this heap block will be promoted to an I4.
        // stobj ignores the type token (System.Byte) and calls Reassign, which calls StoreToReference.

        if (c_CLR_RT_DataTypeLookup[this->DataType()].m_sizeInBytes < sizeArray)
          {
          // Not enough precision here.
          NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }

#if defined(_DEBUG)
        {
        CLR_DataType dtElem = (CLR_DataType)array->m_typeOfElement;
        CLR_RT_HeapBlock blk;
        blk.Assign(*this);

        NANOCLR_CHECK_HRESULT(blk.Convert(
          dtElem,
          false,
          (c_CLR_RT_DataTypeLookup[dtElem].m_flags & CLR_RT_DataTypeLookup::c_Signed) == 0));

        switch (sizeArray)
          {
          case 1:
            _ASSERTE(blk.NumericByRefConst().u1 == this->NumericByRefConst().u1);
            break;
          case 2:
            _ASSERTE(blk.NumericByRefConst().u2 == this->NumericByRefConst().u2);
            break;
          case 4:
            _ASSERTE(blk.NumericByRefConst().u4 == this->NumericByRefConst().u4);
            break;
          case 8:
            _ASSERTE(blk.NumericByRefConst().u8 == this->NumericByRefConst().u8);
            break;
          }
        }
#endif
        }
      else if (size == 0)
        {
        if (obj->DataType() != array->m_typeOfElement)
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }
        }
      else
        {
        if (size != sizeArray)
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }
        }

      uint32_t first = ((uint32_t *)&obj->NumericByRef())[0];
      uint32_t second = ((uint32_t *)&obj->NumericByRef())[1];

      if (sizeArray == 4)
        {
        ((uint32_t *)dst)[0] = (uint32_t)first;
        }
      else if (sizeArray == 8)
        {
        ((uint32_t *)dst)[0] = (uint32_t)first;
        ((uint32_t *)dst)[1] = (uint32_t)second;
        }
      else if (sizeArray == 1)
        {
        ((uint8_t *)dst)[0] = (uint8_t)first;
        }
      else
        {
        ((uint16_t *)dst)[0] = (uint16_t)first;
        }

      NANOCLR_SET_AND_LEAVE(S_OK);
      }
    else
      {
      //
      // If the source is not null, make sure the types are compatible.
      //
      if (this->DataType() == DATATYPE_OBJECT && this->Dereference())
        {
        CLR_RT_TypeDescriptor descSrc;
        CLR_RT_TypeDescriptor descDst;
        CLR_RT_TypeDescriptor descDstSub;

        NANOCLR_CHECK_HRESULT(descSrc.InitializeFromObject(*this));
        NANOCLR_CHECK_HRESULT(descDst.InitializeFromObject(*array));
        descDst.GetElementType(descDstSub);

        if (CLR_RT_ExecutionEngine::IsInstanceOf(descSrc, descDstSub, false) == false)
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }
        }

      obj = (CLR_RT_HeapBlock *)dst;

      NANOCLR_SET_AND_LEAVE(obj->Reassign(*this));
      }
    }
  else if (dt == DATATYPE_BYREF)
    {
    obj = ref.Dereference();
    FAULT_ON_NULL(obj);

    if (obj->DataType() == DATATYPE_VALUETYPE)
      {
      NANOCLR_SET_AND_LEAVE(ref.Reassign(*this));
      }
    }
  else if (c_CLR_RT_DataTypeLookup[dt].m_flags & CLR_RT_DataTypeLookup::c_Direct)
    {
    obj = &ref;
    }
  else
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  obj->Assign(*this);

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::Reassign(const CLR_RT_HeapBlock &value)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock *obj;
  CLR_RT_HeapBlock ref;

  if (this->DataType() == DATATYPE_BYREF)
    {
    obj = this->Dereference();
    FAULT_ON_NULL(obj);

    //
    // Real value types can be passed as references.
    //
    if (obj->DataType() == DATATYPE_VALUETYPE)
      {
      ref.SetObjectReference(obj);

      obj = &ref;
      }

    NANOCLR_SET_AND_LEAVE(obj->Reassign(value));
    }
  else if (value.DataType() == DATATYPE_BYREF)
    {
    obj = value.Dereference();
    FAULT_ON_NULL(obj);

    //
    // Real value types can be passed as references.
    //
    if (obj->DataType() == DATATYPE_VALUETYPE)
      {
      ref.SetObjectReference(obj);

      obj = &ref;
      }

    NANOCLR_SET_AND_LEAVE(this->Reassign(*obj));
    }
  else if (this->DataType() == DATATYPE_ARRAY_BYREF)
    {
    NANOCLR_CHECK_HRESULT(ref.LoadFromReference(*this));
    NANOCLR_CHECK_HRESULT(ref.Reassign(value));
    NANOCLR_SET_AND_LEAVE(ref.StoreToReference(*this, -1));
    }
  else if (value.DataType() == DATATYPE_ARRAY_BYREF)
    {
    _ASSERTE(false); // not tested

    CLR_RT_HeapBlock valueT;
    valueT.Assign(value);

    NANOCLR_CHECK_HRESULT(ref.LoadFromReference(valueT));
    NANOCLR_SET_AND_LEAVE(this->Reassign(ref));
    }
  else
    {
    bool fDestination = this->IsAValueType();
    bool fSource = value.IsAValueType();

    if (fSource != fDestination)
      {
      // For value type objects we don't care if the source item is boxed because
      // CopyValueType will take care of the boxing/unboxing
      if (fDestination != value.IsAReferenceOfThisType(DATATYPE_VALUETYPE))
        {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }
      }

    if (fDestination)
      {
      NANOCLR_SET_AND_LEAVE(g_CLR_RT_ExecutionEngine.CopyValueType(this->Dereference(), value.Dereference()));
      }

    this->Assign(value);
    }

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_HeapBlock::AssignAndPinReferencedObject(const CLR_RT_HeapBlock &value)
  {
  // This is very special case that we have local variable with pinned attribute in metadata.
  // This code is called only if "fixed" keyword is present in the managed code. Executed on assignment to "fixed"
  // pointer. First check if there is object referenced by the local var. We unpin it, since the reference is
  // replaced.
  if ((m_data.objectReference.ptr != NULL && m_id.type.dataType == DATATYPE_ARRAY_BYREF) ||
    m_id.type.dataType == DATATYPE_BYREF)
    {
    // Unpin the object that has been pointed by local variable.
    m_data.objectReference.ptr->Unpin();
    }

  // Move the data.
  m_data = value.m_data;

  // Leave the same logic as in AssignAndPreserveType
  if (DataType() > DATATYPE_LAST_PRIMITIVE_TO_PRESERVE)
    {
    m_id.type.dataType = value.m_id.type.dataType;
    m_id.type.size = value.m_id.type.size;
    // We take new flags, but preserve "pinned" attribute
    m_id.type.flags = m_id.type.flags | HB_Pinned;
    }

  // Pin the object referenced by local variable.
  if ((m_data.objectReference.ptr != NULL && m_id.type.dataType == DATATYPE_ARRAY_BYREF) ||
    m_id.type.dataType == DATATYPE_BYREF)
    {
    m_data.objectReference.ptr->Pin();
    }
  }

HRESULT CLR_RT_HeapBlock::PerformBoxingIfNeeded()
  {
 
  HRESULT hr;

  // we need to box the optimized value types...
  bool fBox = (c_CLR_RT_DataTypeLookup[this->DataType()].m_flags & CLR_RT_DataTypeLookup::c_OptimizedValueType) != 0;

  // ... but also the value types under object types
  if (!fBox && this->DataType() == DATATYPE_OBJECT)
    {
    CLR_RT_HeapBlock *src = this->Dereference();

    if (src && src->DataType() == DATATYPE_VALUETYPE && !src->IsBoxed())
      {
      fBox = true;
      }
    }

  if (fBox)
    {
    CLR_RT_TypeDescriptor desc;

    NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(*this));

    NANOCLR_CHECK_HRESULT(PerformBoxing(desc.m_handlerCls));
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::PerformBoxing(const CLR_RT_TypeDef_Instance &cls)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock tmp;
  CLR_RT_HeapBlock *obj = this;
  CLR_DataType dt = obj->DataType();

  //
  // System.DateTime and System.TimeSpan are real value types, so sometimes they are passed by reference.
  //
  if (dt == DATATYPE_BYREF)
    {
    obj = obj->Dereference();
    FAULT_ON_NULL(obj);
    dt = obj->DataType();

    //
    // Real value types can be passed as references.
    //
    if (dt == DATATYPE_VALUETYPE)
      {
      tmp.SetObjectReference(obj);
      obj = &tmp;

      dt = DATATYPE_OBJECT;
      }
    }

  {
  CLR_DataType dataType = (CLR_DataType)cls.m_target->dataType;
  const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dataType];

  if (dtl.m_flags & CLR_RT_DataTypeLookup::c_OptimizedValueType)
    {
    CLR_RT_HeapBlock *ptr =
      g_CLR_RT_ExecutionEngine.ExtractHeapBlocksForClassOrValueTypes(DATATYPE_VALUETYPE, HB_Boxed, cls, 2);
    FAULT_ON_NULL(ptr);

    switch (dataType)
      {
      case DATATYPE_DATETIME: // Special case.
      case DATATYPE_TIMESPAN: // Special case.
        dataType = DATATYPE_I8;
        break;

      default:
        // the remaining data types aren't to be handled
        break;
      }

    ptr[1].SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dataType, 0, 1));
    ptr[1].AssignData(*this);

    this->SetObjectReference(ptr);
    }
  else if (dt == DATATYPE_OBJECT)
    {
    CLR_RT_HeapBlock *ptr = this->Dereference();

    if (ptr->IsBoxed() || ptr->DataType() != DATATYPE_VALUETYPE)
      {
      NANOCLR_SET_AND_LEAVE(S_FALSE); // Don't box twice...
      }

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.CloneObject(*this, *ptr));

    this->Dereference()->Box();
    }
  else
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
  }

  NANOCLR_NOCLEANUP();
  }

/*******************************************************************************************
 *  Thefunction CLR_RT_HeapBlock::PerformUnboxing is used during processing of unbox.any IL instruction.
 *  Example
 *  unbox.any  [mscorlib]System.Int32
 *  unbox.any takes the value at the top of evaluation stack and performs unboxing into the type
 *  specified after the instruction. In this case the type is [mscorlib]System.Int32.
 *  Function parameters:
 *  1. this - Heap block at the top of evaluation stack.
 *  2. cls  - Runtime Type Definition of the type specified after instruction.
 *  The functoin takes the object pointed by top of ev. stack. Then it does 3 operatioins:
 *  1. Dereferences the object
 *  2. Validates the type of data kept by object corresponds to type in cls.
 *  3. Moves de-referenced date to top of evaluation stack.
 *******************************************************************************************/

HRESULT CLR_RT_HeapBlock::PerformUnboxing(const CLR_RT_TypeDef_Instance &cls)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock *src;

  if (this->DataType() != DATATYPE_OBJECT)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  // Finds the object that keeps the boxed type.
  src = this->Dereference();
  FAULT_ON_NULL(src);

  // Validates that src keeps something boxed and the boxed value is VALUE type.
  if (src->IsBoxed() == false || src->DataType() != DATATYPE_VALUETYPE)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  // Validates the type of data kept by object corresponds to type in cls.
  // If typedef indexes are the same, then skip and go to assigment of objects.
  if (src->ObjectCls().m_data != cls.m_data)
    {
    // The typedef indexes are different, but src and cls may have identical basic data type.
    // Need to check it. If identical - the unboxing is allowed.
    // This "if" compares underlying type in object and cls. Should be equal in order to continue.
    if (!(src->DataSize() > 1 && (src[1].DataType() == cls.m_target->dataType)))
      {
      // No luck. The types in src object and specified by cls are different. Need to throw exceptioin.
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_CAST);
      }
    }

  if (cls.m_target->dataType == DATATYPE_VALUETYPE)
    {
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.CloneObject(*this, *this));

    this->Dereference()->Unbox();
    }
  else // It's a boxed primitive/enum type.
    {
    this->Assign(src[1]);

    this->ChangeDataType(cls.m_target->dataType);
    }

  NANOCLR_NOCLEANUP();
  }

CLR_RT_HeapBlock *CLR_RT_HeapBlock::FixBoxingReference()
  {
 
  //
  // Not boxed, nothing to do.
  //
  if (this->DataType() == DATATYPE_OBJECT)
    {
    CLR_RT_HeapBlock *src = this->Dereference();
    if (!src)
      return src;

    if (src->DataType() == DATATYPE_VALUETYPE && src->IsBoxed())
      {
      CLR_RT_TypeDef_Instance inst;

      if (!inst.InitializeFromIndex(src->ObjectCls()))
        return NULL;

      if (inst.m_target->dataType != DATATYPE_VALUETYPE) // It's a boxed primitive/enum type.
        {
        return &src[1];
        }
      }
    }

  return this;
  }

//--//

bool CLR_RT_HeapBlock::IsZero() const
  {
 
  switch (DataType())
    {
    case DATATYPE_OBJECT:
      return (m_data.objectReference.ptr == NULL);

    case DATATYPE_I8:
    case DATATYPE_U8:
      return (m_data.numeric.u8 == 0);

    case DATATYPE_R4:
      return (m_data.numeric.r4 == 0);
    case DATATYPE_R8:
      return (m_data.numeric.r8 == 0);

    default:
      return (m_data.numeric.u4 == 0);
    }
  }

//--//

void CLR_RT_HeapBlock::Promote()
  {
 

  switch (DataType())
    {
    case DATATYPE_I1:
      m_id.type.dataType = DATATYPE_I4;
      m_data.numeric.s4 = (int32_t)m_data.numeric.s1;
      break;

    case DATATYPE_I2:
      m_id.type.dataType = DATATYPE_I4;
      m_data.numeric.s4 = (int32_t)m_data.numeric.s2;
      break;

    case DATATYPE_BOOLEAN:
    case DATATYPE_U1:
      m_id.type.dataType = DATATYPE_I4;
      m_data.numeric.u4 = (uint32_t)m_data.numeric.u1;
      break;

    case DATATYPE_CHAR:
    case DATATYPE_U2:
      m_id.type.dataType = DATATYPE_I4;
      m_data.numeric.u4 = (uint32_t)m_data.numeric.u2;
      break;

    default:
      // this data type is not to be promoted
      break;
    }
  }

//--//

uint32_t CLR_RT_HeapBlock::GetHashCode(CLR_RT_HeapBlock *ptr, bool fRecurse, uint32_t crc = 0)
  {
 
  if (!ptr)
    return 0;

  switch (ptr->DataType())
    {
    case DATATYPE_OBJECT:
      crc = GetHashCode(ptr->Dereference(), fRecurse, crc);
      break;

    case DATATYPE_STRING:
    {
    const char *szText = ptr->StringText();

    crc = SUPPORT_ComputeCRC(szText, (int)hal_strlen_s(szText), crc);
    }
    break;

    case DATATYPE_CLASS:
    case DATATYPE_VALUETYPE:
      if (fRecurse)
        {
        CLR_RT_TypeDef_Instance cls;
        cls.InitializeFromIndex(ptr->ObjectCls());
        int totFields = cls.CrossReference().m_totalFields;

        if (totFields > 0)
          {
          while (totFields-- > 0)
            {
            crc = GetHashCode(++ptr, false, crc);
            }
          }
        else
          {
          crc = SUPPORT_ComputeCRC(&ptr, sizeof(ptr), crc);
          }
        }
      break;

    case DATATYPE_DELEGATE_HEAD:
    {
    CLR_RT_HeapBlock_Delegate *dlg = (CLR_RT_HeapBlock_Delegate *)ptr;
    const uint32_t &ftn = dlg->DelegateFtn();

    crc = GetHashCode(&dlg->m_object, false, crc);

    crc = SUPPORT_ComputeCRC(&ftn, sizeof(ftn), crc);
    }
    break;

    case DATATYPE_OBJECT_TO_EVENT:
    {
    CLR_RT_ObjectToEvent_Source *evtSrc = (CLR_RT_ObjectToEvent_Source *)ptr;

    crc = GetHashCode(evtSrc->m_eventPtr, false, crc);
    crc = GetHashCode(evtSrc->m_objectPtr, false, crc);
    }
    break;

    default:
      crc = SUPPORT_ComputeCRC(&ptr->DataByRefConst(), ptr->GetAtomicDataUsedBytes(), crc);
      break;
    }

  return crc;
  }

uint32_t CLR_RT_HeapBlock::GetAtomicDataUsedBytes() const

  {
  switch (DataType())
    {
    case DATATYPE_BOOLEAN: // Fall through, hashDataSize = 1
    case DATATYPE_U1:      // Fall through, hashDataSize = 1
    case DATATYPE_CHAR:
      return 1;

    case DATATYPE_I2: // Fall through, hashDataSize = 2
    case DATATYPE_U2:
      return 2;
      break;

    case DATATYPE_I4:
    case DATATYPE_U4:
    case DATATYPE_R4:
      return 4;
      break;

    case DATATYPE_I8:       // Fall through, hashDataSize = 8
    case DATATYPE_U8:       // Fall through, hashDataSize = 8
    case DATATYPE_R8:       // Fall through, hashDataSize = 8
    case DATATYPE_DATETIME: // Fall through, hashDataSize = 8
    case DATATYPE_TIMESPAN:
      return 8;
      break;

      // Default full size of CLR_RT_HeapBlock_AtomicData
    default:
      return sizeof(CLR_RT_HeapBlock_AtomicData);
    }
  // The same as default. This is never reached, but I put it to avoid potential compiler warning.
  return sizeof(CLR_RT_HeapBlock_AtomicData);
  }

bool CLR_RT_HeapBlock::ObjectsEqual(
  const CLR_RT_HeapBlock &pArgLeft,
  const CLR_RT_HeapBlock &pArgRight,
  bool fSameReference)
  {
 
  if (&pArgLeft == &pArgRight)
    return true;

  if (pArgLeft.DataType() == pArgRight.DataType())
    {
    switch (pArgLeft.DataType())
      {
      case DATATYPE_VALUETYPE:
        if (pArgLeft.ObjectCls().m_data == pArgRight.ObjectCls().m_data)
          {
          const CLR_RT_HeapBlock *objLeft = &pArgLeft;
          const CLR_RT_HeapBlock *objRight = &pArgRight;
          uint32_t num = pArgLeft.DataSize();

          while (--num)
            {
            if (ObjectsEqual(*++objLeft, *++objRight, false) == false)
              return false;
            }

          return true;
          }
        break;
      case DATATYPE_OBJECT:
        {
        CLR_RT_HeapBlock *objLeft = pArgLeft.Dereference();
        CLR_RT_HeapBlock *objRight = pArgRight.Dereference();
        if (objLeft == objRight)
          return true;

        if (objLeft && objRight)
          {
          if (!fSameReference || (objLeft->DataType() == DATATYPE_REFLECTION))
            return ObjectsEqual(*objLeft, *objRight, false);
          }
        }
        break;

      case DATATYPE_SZARRAY:
        if (fSameReference == false)
          {
          _ASSERTE(false); // can this code path ever be executed?

          CLR_RT_HeapBlock_Array *objLeft = (CLR_RT_HeapBlock_Array *)&pArgLeft;
          CLR_RT_HeapBlock_Array *objRight = (CLR_RT_HeapBlock_Array *)&pArgRight;

          if (objLeft->m_numOfElements == objRight->m_numOfElements &&
            objLeft->m_sizeOfElement == objRight->m_sizeOfElement &&
            objLeft->m_typeOfElement == objRight->m_typeOfElement)
            {
            if (!objLeft->m_fReference)
              {
              if (memcmp(
                objLeft->GetFirstElement(),
                objRight->GetFirstElement(),
                objLeft->m_numOfElements * objLeft->m_sizeOfElement) == 0)
                {
                return true;
                }
              }
            }
          }
        break;
      case DATATYPE_REFLECTION:
        if (pArgLeft.SameHeader(pArgRight))
          return true;
        break;
      default:
        if (fSameReference == false)
          {
          const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[pArgLeft.DataType()];

          if ((dtl.m_flags & CLR_RT_DataTypeLookup::c_Reference) == 0)
            {
            uint32_t size = (dtl.m_sizeInBits + 7) / 8;

            if (memcmp(&pArgLeft.DataByRefConst(), &pArgRight.DataByRefConst(), size) == 0)
              {
              return true;
              }
            }
          }
        break;
      }
    }

  return false;
  }

//--//

static const CLR_RT_HeapBlock *FixReflectionForType(const CLR_RT_HeapBlock &src, CLR_RT_HeapBlock &tmp)
  {
 
  const CLR_RT_ReflectionDef_Index &rd = src.ReflectionDataConst();

  if (rd.m_kind == REFLECTION_TYPE)
    {
    CLR_RT_TypeDef_Instance inst;
    uint32_t levels;

    if (inst.InitializeFromReflection(rd, &levels) && levels == 0)
      {
      tmp.Assign(src);

      CLR_RT_ReflectionDef_Index &rd2 = tmp.ReflectionData();

      rd2.InitializeFromHash(inst.CrossReference().m_hash);

      return &tmp;
      }
    }

  return &src;
  }

//--//

static inline int CompareValues_Numeric(int32_t left, int32_t right)
  {
 
  if (left > right)
    return 1;
  if (left < right)
    return -1;
  /**************/ return 0;
  }

static inline int CompareValues_Numeric(uint32_t left, uint32_t right)
  {
 
  if (left > right)
    return 1;
  if (left < right)
    return -1;
  /**************/ return 0;
  }

static int CompareValues_Numeric(const int64_t left, const int64_t right)
  {
 

  if (left > right)
    return 1;
  if (left < right)
    return -1;
  /**************/ return 0;
  }

static int CompareValues_Numeric(const uint64_t left, const uint64_t right)
  {
 
  if (left > right)
    return 1;
  if (left < right)
    return -1;
  /**************/ return 0;
  }

static int CompareValues_Numeric(const CLR_RT_HeapBlock &left, const CLR_RT_HeapBlock &right, bool fSigned, int bytes)
  {
 

  switch (bytes)
    {
    case 4:
      if (fSigned)
        return CompareValues_Numeric(
          (int32_t)left.NumericByRefConst().s4,
          (int32_t)right.NumericByRefConst().s4);
      else
        return CompareValues_Numeric(
          (uint32_t)left.NumericByRefConst().u4,
          (uint32_t)right.NumericByRefConst().u4);

    case 8:
      if (fSigned)
        return CompareValues_Numeric(
          (int64_t)left.NumericByRefConst().s8,
          (int64_t)right.NumericByRefConst().s8);
      else
        return CompareValues_Numeric(
          (uint64_t)left.NumericByRefConst().u8,
          (uint64_t)right.NumericByRefConst().u8);

    case 2:
      if (fSigned)
        return CompareValues_Numeric(
          (int32_t)left.NumericByRefConst().s2,
          (int32_t)right.NumericByRefConst().s2);
      else
        return CompareValues_Numeric(
          (uint32_t)left.NumericByRefConst().u2,
          (uint32_t)right.NumericByRefConst().u2);

    case 1:
      if (fSigned)
        return CompareValues_Numeric(
          (int32_t)left.NumericByRefConst().s1,
          (int32_t)right.NumericByRefConst().s1);
      else
        return CompareValues_Numeric(
          (uint32_t)left.NumericByRefConst().u1,
          (uint32_t)right.NumericByRefConst().u1);

    default:
      return -1;
    }
  }

static inline int CompareValues_Pointers(const CLR_RT_HeapBlock *left, const CLR_RT_HeapBlock *right)
  {
 
  if (left > right)
    return 1;
  if (left < right)
    return -1;
  /**************/ return 0;
  }

int32_t CLR_RT_HeapBlock::Compare_Values(const CLR_RT_HeapBlock &left, const CLR_RT_HeapBlock &right, bool fSigned)
  {
 
  CLR_DataType leftDataType = left.DataType();
  CLR_DataType rightDataType = right.DataType();

  if (leftDataType == rightDataType)
    {
    switch (leftDataType)
      {
      case DATATYPE_OBJECT:
      case DATATYPE_BYREF:
      {
      CLR_RT_HeapBlock *leftObj = left.Dereference();
      CLR_RT_HeapBlock *rightObj = right.Dereference();

      if (!leftObj)
        {
        return !rightObj ? 0 : -1; // NULL references always compare smaller than non-NULL ones.
        }
      else if (!rightObj)
        {
        return 1; // NULL references always compare smaller than non-NULL ones.
        }

      return Compare_Values(*leftObj, *rightObj, fSigned);
        }

      case DATATYPE_STRING:
      {
      CLR_RT_HeapBlock_String *leftStr = (CLR_RT_HeapBlock_String *)&left;
      CLR_RT_HeapBlock_String *rightStr = (CLR_RT_HeapBlock_String *)&right;

      return strcmp(leftStr->StringText(), rightStr->StringText());
      }

      case DATATYPE_DELEGATELIST_HEAD:
      {
      CLR_RT_HeapBlock_Delegate_List *leftDlg = (CLR_RT_HeapBlock_Delegate_List *)&left;
      CLR_RT_HeapBlock_Delegate_List *rightDlg = (CLR_RT_HeapBlock_Delegate_List *)&right;
      CLR_RT_HeapBlock *leftPtr = leftDlg->GetDelegates();
      CLR_RT_HeapBlock *rightPtr = rightDlg->GetDelegates();
      uint32_t leftLen = leftDlg->m_length;
      uint32_t rightLen = rightDlg->m_length;

      while (leftLen > 0 && rightLen > 0)
        {
        int res = CLR_RT_HeapBlock::Compare_Values(*leftPtr++, *rightPtr++, fSigned);
        if (res)
          return res;

        leftLen--;
        rightLen--;
        }

      if (!leftLen)
        {
        return !rightLen ? 0 : -1; // NULL references always compare smaller than non-NULL ones.
        }
      else // rightLen != 0 for sure.
        {
        return 1; // NULL references always compare smaller than non-NULL ones.
        }
      }

      case DATATYPE_DELEGATE_HEAD:
      {
      CLR_RT_HeapBlock_Delegate *leftDlg = (CLR_RT_HeapBlock_Delegate *)&left;
      CLR_RT_HeapBlock_Delegate *rightDlg = (CLR_RT_HeapBlock_Delegate *)&right;
      uint32_t leftData = leftDlg->DelegateFtn().m_data;
      uint32_t rightData = rightDlg->DelegateFtn().m_data;

      if (leftData > rightData)
        return 1;
      if (leftData < rightData)
        return -1;

      return Compare_Values(leftDlg->m_object, rightDlg->m_object, fSigned);
      }

      case DATATYPE_CLASS:
      case DATATYPE_VALUETYPE:
      case DATATYPE_SZARRAY:
      case DATATYPE_WEAKCLASS:
        return CompareValues_Pointers(&left, &right);

      case DATATYPE_REFLECTION:
      {
      const CLR_RT_HeapBlock *ptrLeft;
      const CLR_RT_HeapBlock *ptrRight;
      CLR_RT_HeapBlock hbLeft;
      CLR_RT_HeapBlock hbRight;

      if (left.ReflectionDataConst().m_kind != right.ReflectionDataConst().m_kind)
        {
        ptrLeft = FixReflectionForType(left, hbLeft);
        ptrRight = FixReflectionForType(right, hbRight);
        }
      else
        {
        ptrLeft = &left;
        ptrRight = &right;
        }

      return CompareValues_Numeric(*ptrLeft, *ptrRight, false, 8);
      }

      //--//
#if !defined(NANOCLR_EMULATED_FLOATINGPOINT)

      case DATATYPE_R4:

        // deal with special cases:
        // return 0 if the numbers are unordered (either or both are NaN)
        // this is post processed in interpreter so '1' will turn into '0'
        if (__isnand(left.NumericByRefConst().r4) || __isnand(right.NumericByRefConst().r4))
          {
          return 1;
          }

        // The infinite values are equal to themselves.
        // this is post processed in interpreter so '0' will turn into '1'
        else if (__isinfd(left.NumericByRefConst().r4) && __isinfd(right.NumericByRefConst().r4))
          {
          return 0;
          }
        // all the rest now
        else
          {
          if (isgreater(left.NumericByRefConst().r4, right.NumericByRefConst().r4))
            {
            return 1;
            }
          else if (isless(left.NumericByRefConst().r4, right.NumericByRefConst().r4))
            {
            return -1;
            }
          else
            {
            return 0;
            }
          }

      case DATATYPE_R8:

        // deal with special cases:
        // return 0 if the numbers are unordered (either or both are NaN)
        // this is post processed in interpreter so '1' will turn into '0'
        if (__isnand((double)left.NumericByRefConst().r8) || __isnand((double)right.NumericByRefConst().r8))
          {
          return 1;
          }

        // The infinite values are equal to themselves.
        // this is post processed in interpreter so '0' will turn into '1'
        else if (
          __isinfd((double)left.NumericByRefConst().r8) && __isinfd((double)right.NumericByRefConst().r8))
          {
          return 0;
          }
        // all the rest now
        else
          {
          if (isgreater((double)left.NumericByRefConst().r8, (double)right.NumericByRefConst().r8))
            {
            return 1;
            }
          else if (isless((double)left.NumericByRefConst().r8, (double)right.NumericByRefConst().r8))
            {
            return -1;
            }
          else
            {
            return 0;
            }
          }

#else
      case DATATYPE_R4:
      case DATATYPE_R8:
        fSigned = true;
#endif

      case DATATYPE_BOOLEAN:
      case DATATYPE_I1:
      case DATATYPE_U1:

      case DATATYPE_CHAR:
      case DATATYPE_I2:
      case DATATYPE_U2:

      case DATATYPE_I4:
      case DATATYPE_U4:

      case DATATYPE_I8:
      case DATATYPE_U8:
      case DATATYPE_DATETIME:
      case DATATYPE_TIMESPAN:
        return CompareValues_Numeric(left, right, fSigned, c_CLR_RT_DataTypeLookup[leftDataType].m_sizeInBytes);

      default:
        // the remaining data types aren't to be handled
        break;
      }
      }
  else
    {
    if (leftDataType == DATATYPE_STRING && rightDataType == DATATYPE_OBJECT)
      {
      CLR_RT_HeapBlock *rightObj = right.Dereference();

      if (!rightObj)
        {
        return 1; // NULL references always compare smaller than non-NULL ones.
        }

      return Compare_Values(left, *rightObj, fSigned);
      }

    if (leftDataType == DATATYPE_OBJECT && rightDataType == DATATYPE_STRING)
      {
      CLR_RT_HeapBlock *leftObj = left.Dereference();

      if (!leftObj)
        {
        return -1; // NULL references always compare smaller than non-NULL ones.
        }

      return Compare_Values(*leftObj, right, fSigned);
      }

    //--//

    const CLR_RT_DataTypeLookup &leftDtl = c_CLR_RT_DataTypeLookup[leftDataType];
    const CLR_RT_DataTypeLookup &rightDtl = c_CLR_RT_DataTypeLookup[rightDataType];

    if ((leftDtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric) &&
      (rightDtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric))
      {
      if (leftDtl.m_sizeInBytes == rightDtl.m_sizeInBytes)
        {
        return CompareValues_Numeric(left, right, fSigned, leftDtl.m_sizeInBytes);
        }
      else
        {
        trace_debug(
          "RUNTIME ERROR: comparing two values of different size: %d vs. %d!!!\r\n",
          leftDataType,
          rightDataType);
        }
      }
    }

  return -1; // Not comparable...
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_HeapBlock::NumericAdd(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_I4:
      m_data.numeric.s4 += right.m_data.numeric.s4;
      break;

    case DATATYPE_U4:
      m_data.numeric.u4 += right.m_data.numeric.u4;
      break;

    case DATATYPE_I8:
      m_data.numeric.s8 += right.m_data.numeric.s8;
      break;

    case DATATYPE_U8:
      m_data.numeric.u8 += right.m_data.numeric.u8;
      break;

    case DATATYPE_R4:
    m_data.numeric.r4 += right.m_data.numeric.r4;
    break;

    case DATATYPE_R8:
    m_data.numeric.r8 += right.m_data.numeric.r8;
    break;

    // Adding of value to array reference is like advancing the index in array.
    case DATATYPE_ARRAY_BYREF:
    {
    // Retrieve refernced array. Test if it is not NULL
    CLR_RT_HeapBlock_Array *array = m_data.arrayReference.array;
    FAULT_ON_NULL(array);
    // Advance current index. C# on pointer operations multiplies the offset by object size. We need to reverse
    // it.
    m_data.arrayReference.index += right.m_data.numeric.s4 / array->m_sizeOfElement;
  }
    break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
  }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericSub(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_U4:
      m_data.numeric.u4 -= right.m_data.numeric.u4;
      break;

    case DATATYPE_I4:
      m_data.numeric.s4 -= right.m_data.numeric.s4;
      break;

    case DATATYPE_U8:
      m_data.numeric.u8 -= right.m_data.numeric.u8;
      break;

    case DATATYPE_I8:
      m_data.numeric.s8 -= right.m_data.numeric.s8;
      break;

    case DATATYPE_R4:
    m_data.numeric.r4 -= right.m_data.numeric.r4;
    break;

    case DATATYPE_R8:
    m_data.numeric.r8 -= right.m_data.numeric.r8;
    break;

    // Substructing of value to array reference is like decreasing the index in array.
    case DATATYPE_ARRAY_BYREF:
    {
    // Retrieve refernced array. Test if it is not NULL
    CLR_RT_HeapBlock_Array *array = m_data.arrayReference.array;
    FAULT_ON_NULL(array);
    // Advance current index. C# on pointer operations multiplies the offset by object size. We need to reverse
    // it.
    m_data.arrayReference.index -= right.m_data.numeric.s4 / array->m_sizeOfElement;
  }
    break;
    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericMul(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_U4:
      m_data.numeric.u4 = m_data.numeric.u4 * right.m_data.numeric.u4;
      break;

    case DATATYPE_I4:
      m_data.numeric.s4 = m_data.numeric.s4 * right.m_data.numeric.s4;
      break;

    case DATATYPE_U8:
      m_data.numeric.u8 = m_data.numeric.u8 * right.m_data.numeric.u8;
      break;

    case DATATYPE_I8:
      m_data.numeric.s8 = m_data.numeric.s8 * right.m_data.numeric.s8;
      break;

    case DATATYPE_R4:
    m_data.numeric.r4 = m_data.numeric.r4 * right.m_data.numeric.r4;
    break;

    case DATATYPE_R8:
    m_data.numeric.r8 = m_data.numeric.r8 * right.m_data.numeric.r8;
    break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericDiv(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  if (right.IsZero())
    NANOCLR_SET_AND_LEAVE(CLR_E_DIVIDE_BY_ZERO);

  switch (DataType())
    {
    case DATATYPE_U4:
    case DATATYPE_I4:
      m_data.numeric.s4 = m_data.numeric.s4 / right.m_data.numeric.s4;
      break;

    case DATATYPE_U8:
    case DATATYPE_I8:
      m_data.numeric.s8 = m_data.numeric.s8 / right.m_data.numeric.s8;
      break;

    case DATATYPE_R4:
      m_data.numeric.r4 = m_data.numeric.r4 / right.m_data.numeric.r4;
      break;

    case DATATYPE_R8:
      m_data.numeric.r8 = m_data.numeric.r8 / right.m_data.numeric.r8;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
      }

HRESULT CLR_RT_HeapBlock::NumericDivUn(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  if (right.IsZero())
    NANOCLR_SET_AND_LEAVE(CLR_E_DIVIDE_BY_ZERO);

  switch (DataType())
    {
    case DATATYPE_I4:
    case DATATYPE_U4:
      m_data.numeric.u4 = m_data.numeric.u4 / right.m_data.numeric.u4;
      break;

    case DATATYPE_I8:
    case DATATYPE_U8:
      m_data.numeric.u8 = m_data.numeric.u8 / right.m_data.numeric.u8;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericRem(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  if (right.IsZero())
    NANOCLR_SET_AND_LEAVE(CLR_E_DIVIDE_BY_ZERO);

  switch (DataType())
    {
    case DATATYPE_U4:
      m_data.numeric.u4 %= right.m_data.numeric.u4;
      break;

    case DATATYPE_I4:
      m_data.numeric.s4 %= right.m_data.numeric.s4;
      break;

    case DATATYPE_U8:
      m_data.numeric.u8 %= right.m_data.numeric.u8;
      break;

    case DATATYPE_I8:
      m_data.numeric.s8 %= right.m_data.numeric.s8;
      break;
    case DATATYPE_R4:
      m_data.numeric.r4 = fmod(m_data.numeric.r4, right.m_data.numeric.r4);
      break;

    case DATATYPE_R8:
      m_data.numeric.r8 =
        fmod((CLR_DOUBLE_TEMP_CAST)m_data.numeric.r8, (CLR_DOUBLE_TEMP_CAST)right.m_data.numeric.r8);
      break;
    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericRemUn(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  if (right.IsZero())
    NANOCLR_SET_AND_LEAVE(CLR_E_DIVIDE_BY_ZERO);

  switch (DataType())
    {
    case DATATYPE_I4:
    case DATATYPE_U4:
      m_data.numeric.u4 %= right.m_data.numeric.u4;
      break;

    case DATATYPE_I8:
    case DATATYPE_U8:
      m_data.numeric.u8 %= right.m_data.numeric.u8;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
    }

HRESULT CLR_RT_HeapBlock::NumericShl(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_I4:
    case DATATYPE_U4:
      m_data.numeric.u4 <<= right.m_data.numeric.u4;
      break;

    case DATATYPE_I8:
    case DATATYPE_U8:
      m_data.numeric.u8 <<= right.m_data.numeric.u4;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericShr(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_U4:
      m_data.numeric.u4 >>= right.m_data.numeric.u4;
      break;

    case DATATYPE_I4:
      m_data.numeric.s4 >>= right.m_data.numeric.u4;
      break;

    case DATATYPE_U8:
      m_data.numeric.u8 >>= right.m_data.numeric.u4;
      break;

    case DATATYPE_I8:
      m_data.numeric.s8 >>= right.m_data.numeric.u4;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericShrUn(const CLR_RT_HeapBlock &right)
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_I4:
    case DATATYPE_U4:
      m_data.numeric.u4 >>= right.m_data.numeric.u4;
      break;

    case DATATYPE_I8:
    case DATATYPE_U8:
      m_data.numeric.u8 >>= right.m_data.numeric.u4;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_HeapBlock::NumericNeg()
  {
 
  HRESULT hr;

  switch (DataType())
    {
    case DATATYPE_U4:
    case DATATYPE_I4:
      m_data.numeric.s4 = -m_data.numeric.s4;
      break;

    case DATATYPE_U8:
    case DATATYPE_I8:
      m_data.numeric.s8 = -m_data.numeric.s8;
      break;

    case DATATYPE_R4:
      m_data.numeric.r4 = -m_data.numeric.r4;
      break;

    case DATATYPE_R8:
      m_data.numeric.r8 = -m_data.numeric.r8;
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_RT_HeapBlock *CLR_RT_HeapBlock::ExtractValueBlock(int offset)
  {
 
  CLR_RT_HeapBlock *ptr = Dereference();

  if (ptr)
    {
    ptr = &ptr[offset];
    }

  return ptr;
  }

void CLR_RT_HeapBlock::ReadValue(int64_t &val, int offset)
  {
 
  CLR_RT_HeapBlock *ptr = ExtractValueBlock(offset);

  if (ptr)
    {
    val = ptr->NumericByRefConst().s8;
    }
  else
    {
    int32_t val2 = 0;

    val = val2;
    }
  }

void CLR_RT_HeapBlock::WriteValue(const int64_t &val, int offset)
  {
 
  CLR_RT_HeapBlock *ptr = ExtractValueBlock(offset);

  if (ptr)
    ptr->NumericByRef().s8 = val;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_HeapBlock::Relocate__HeapBlock()
  {
 
  CLR_RT_HEAPBLOCK_RELOCATE(this);
  }

void CLR_RT_HeapBlock::Relocate_String()
  {
 
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_data.string.m_text);
    }

void CLR_RT_HeapBlock::Relocate_Obj()
  {
 
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_data.objectReference.ptr);
  }

void CLR_RT_HeapBlock::Relocate_Cls()
  {
 
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_data.objectHeader.lock);

  CLR_RT_GarbageCollector::Heap_Relocate(this + 1, DataSize() - 1);
  }

void CLR_RT_HeapBlock::Relocate_Ref()
  {
 
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_data.objectReference.ptr);
  }

void CLR_RT_HeapBlock::Relocate_ArrayRef()
  {
 
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_data.arrayReference.array);
  }

//--//

#if defined(NANOCLR_FILL_MEMORY_WITH_DIRTY_PATTERN)

void CLR_RT_HeapBlock::Debug_CheckPointer() const
  {
 
  if (m_id.type.dataType == DATATYPE_OBJECT)
    {
    Debug_CheckPointer(Dereference());
  }
  }

void CLR_RT_HeapBlock::Debug_CheckPointer(void *ptr)
  {
 
  switch ((size_t)ptr)
    {
    case 0xCFCFCFCF:
    case 0xCBCBCBCB:
    case 0xABABABAB:
    case 0xADADADAD:
    case 0xDFDFDFDF:
      NANOCLR_STOP();
      break;
    }
  }

void CLR_RT_HeapBlock::Debug_ClearBlock(int data)
  {
 
  uint32_t size = DataSize();

  if (size > 1)
    {
    CLR_RT_HeapBlock_Raw *ptr = (CLR_RT_HeapBlock_Raw *)this;
    uint32_t raw1 = CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_OBJECT, 0, 1);
    uint32_t raw2;

    raw2 = data & 0xFF;
    raw2 = raw2 | (raw2 << 8);
    raw2 = raw2 | (raw2 << 16);

    while (--size)
      {
      ptr++;

      ptr->data[0] = raw1;
      ptr->data[1] = raw2;
      ptr->data[2] = raw2;
      }
    }
  }

#endif
