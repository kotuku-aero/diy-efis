//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"
#include "corhdr_private.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ITERATE_THROUGH_RECORDS(assm, i, tblName, tblNameUC)                                                           \
    const CLR_RECORD_##tblNameUC *src = (const CLR_RECORD_##tblNameUC *)assm->GetTable(TBL_##tblName);                 \
    CLR_RT_##tblName##_CrossReference *dst = assm->m_pCrossReference_##tblName;                                        \
    for (i = 0; i < assm->m_pTablesSize[TBL_##tblName]; i++, src++, dst++)

////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#define NANOCLR_TRACE_DEFAULT(win, arm) (win)
#else
#define NANOCLR_TRACE_DEFAULT(win, arm) (arm)
#endif

#if defined(NANOCLR_TRACE_ERRORS)
int s_CLR_RT_fTrace_Errors = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(NANOCLR_TRACE_EXCEPTIONS)
int s_CLR_RT_fTrace_Exceptions = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(NANOCLR_TRACE_INSTRUCTIONS)
int s_CLR_RT_fTrace_Instructions = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_None, c_CLR_RT_Trace_None);
#endif

#if defined(NANOCLR_GC_VERBOSE)
int s_CLR_RT_fTrace_Memory = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_None, c_CLR_RT_Trace_None);
#endif

#if defined(NANOCLR_TRACE_MEMORY_STATS)
int s_CLR_RT_fTrace_MemoryStats = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(NANOCLR_GC_VERBOSE)
int s_CLR_RT_fTrace_GC = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_None, c_CLR_RT_Trace_None);
#endif

#if defined(WIN32)
int s_CLR_RT_fTrace_SimulateSpeed = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_None);
#endif

#if !defined(BUILD_RTM)
int s_CLR_RT_fTrace_AssemblyOverhead = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_Info, c_CLR_RT_Trace_Info);
#endif

#if defined(WIN32)
int s_CLR_RT_fTrace_StopOnFAILED = NANOCLR_TRACE_DEFAULT(c_CLR_RT_Trace_None, c_CLR_RT_Trace_None);
#endif

#if defined(WIN32)
int s_CLR_RT_fTrace_ARM_Execution = 0;

int s_CLR_RT_fTrace_RedirectLinesPerFile;
std::wstring s_CLR_RT_fTrace_RedirectOutput;
std::wstring s_CLR_RT_fTrace_RedirectCallChain;

std::wstring s_CLR_RT_fTrace_HeapDump_FilePrefix;
bool s_CLR_RT_fTrace_HeapDump_IncludeCreators = false;

bool s_CLR_RT_fTimeWarp = false;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_ReflectionDef_Index::Clear()
  {
  m_kind = REFLECTION_INVALID;
  m_levels = 0;
  m_data.m_raw = 0;
  }

uint32_t CLR_RT_ReflectionDef_Index::GetTypeHash() const
  {

  switch (m_kind)
    {
    case REFLECTION_TYPE:
    {
    CLR_RT_TypeDef_Instance inst;

    if (m_levels != 0)
      return 0;

    if (!inst.InitializeFromIndex(m_data.m_type))
      return 0;

    return inst.CrossReference().m_hash;
    }

    case REFLECTION_TYPE_DELAYED:
      return m_data.m_raw;
    }

  return 0;
  }

void CLR_RT_ReflectionDef_Index::InitializeFromHash(uint32_t hash)
  {

  m_kind = REFLECTION_TYPE_DELAYED;
  m_levels = 0;
  m_data.m_raw = hash;
  }

uint64_t CLR_RT_ReflectionDef_Index::GetRawData() const
  {

  uint64_t data;
  _ASSERTE(sizeof(data) == sizeof(*this));

  memcpy(&data, this, sizeof(data));

  return data;
  }

void CLR_RT_ReflectionDef_Index::SetRawData(uint64_t data)
  {

  _ASSERTE(sizeof(data) == sizeof(*this));

  memcpy(this, &data, sizeof(data));
  }

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_Assembly_Instance &inst)
  {

  if (ref.DataType() == DATATYPE_REFLECTION)
    {
    return inst.InitializeFromIndex(ref.ReflectionDataConst().m_data.m_assm);
    }

  return false;
  }

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_TypeDef_Instance &inst, uint32_t *levels)
  {

  if (ref.DataType() == DATATYPE_REFLECTION)
    {
    return inst.InitializeFromReflection(ref.ReflectionDataConst(), levels);
    }

  return false;
  }

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_MethodDef_Instance &inst)
  {

  if (ref.DataType() == DATATYPE_REFLECTION)
    {
    switch (ref.ReflectionData().m_kind)
      {
      case REFLECTION_CONSTRUCTOR:
      case REFLECTION_METHOD:
        return inst.InitializeFromIndex(ref.ReflectionDataConst().m_data.m_method);
      }
    }

  return false;
  }

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, CLR_RT_FieldDef_Instance &inst)
  {

  if (ref.DataType() == DATATYPE_REFLECTION && ref.ReflectionData().m_kind == REFLECTION_FIELD)
    {
    return inst.InitializeFromIndex(ref.ReflectionDataConst().m_data.m_field);
    }

  return false;
  }

bool CLR_RT_ReflectionDef_Index::Convert(CLR_RT_HeapBlock &ref, uint32_t &hash)
  {

  if (ref.DataType() != DATATYPE_REFLECTION)
    return false;

  hash = ref.ReflectionData().GetTypeHash();

  return hash != 0;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_SignatureParser::Initialize_TypeSpec(CLR_RT_Assembly *assm, const CLR_RECORD_TYPESPEC *ts)
  {

  Initialize_TypeSpec(assm, assm->GetSignature(ts->sig));
  }

void CLR_RT_SignatureParser::Initialize_TypeSpec(CLR_RT_Assembly *assm, CLR_PMETADATA ts)
  {

  m_assm = assm;
  m_sig = ts;

  m_type = CLR_RT_SignatureParser::c_TypeSpec;
  m_flags = 0;
  m_count = 1;
  }

//--//

void CLR_RT_SignatureParser::Initialize_Interfaces(CLR_RT_Assembly *assm, const CLR_RECORD_TYPEDEF *td)
  {

  if (td->interfaces != CLR_EmptyIndex)
    {
    CLR_PMETADATA sig = assm->GetSignature(td->interfaces);

    m_count = (*sig++);
    m_sig = sig;
    }
  else
    {
    m_count = 0;
    m_sig = NULL;
    }

  m_type = CLR_RT_SignatureParser::c_Interfaces;
  m_flags = 0;

  m_assm = assm;
  }

//--//

void CLR_RT_SignatureParser::Initialize_FieldDef(CLR_RT_Assembly *assm, const CLR_RECORD_FIELDDEF *fd)
  {

  Initialize_FieldDef(assm, assm->GetSignature(fd->sig));
  }

void CLR_RT_SignatureParser::Initialize_FieldDef(CLR_RT_Assembly *assm, CLR_PMETADATA fd)
  {

  m_type = CLR_RT_SignatureParser::c_Field;
  m_flags = (*fd++);
  m_count = 1;

  m_assm = assm;
  m_sig = fd;
  }

void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_Assembly *assm, const CLR_RECORD_METHODDEF *md)
  {

  Initialize_MethodSignature(assm, assm->GetSignature(md->sig));
  }

void CLR_RT_SignatureParser::Initialize_MethodSignature(CLR_RT_Assembly *assm, CLR_PMETADATA md)
  {

  m_type = CLR_RT_SignatureParser::c_Method;
  m_flags = (*md++);
  m_count = (*md++) + 1;

  m_assm = assm;
  m_sig = md;
  }

//--//

void CLR_RT_SignatureParser::Initialize_MethodLocals(CLR_RT_Assembly *assm, const CLR_RECORD_METHODDEF *md)
  {

  //
  // WARNING!!!
  //
  // If you change this method, change "CLR_RT_ExecutionEngine::InitializeLocals" too.
  //

  m_assm = assm;
  m_sig = assm->GetSignature(md->locals);

  m_type = CLR_RT_SignatureParser::c_Locals;
  m_flags = 0;
  m_count = md->numLocals;
  }

//--//

void CLR_RT_SignatureParser::Initialize_Objects(CLR_RT_HeapBlock *lst, int count, bool fTypes)
  {

  m_lst = lst;

  m_type = CLR_RT_SignatureParser::c_Object;
  m_flags = fTypes ? 1 : 0;
  m_count = count;
  }

//--//

HRESULT CLR_RT_SignatureParser::Advance(Element &res)
  {

  //
  // WARNING!!!
  //
  // If you change this method, change "CLR_RT_ExecutionEngine::InitializeLocals" too.
  //

  HRESULT hr;

  _ASSERTE(m_count > 0);

  m_count--;

  res.m_fByRef = false;
  res.m_levels = 0;

  switch (m_type)
    {
    case c_Interfaces:
    {
    CLR_RT_TypeDef_Instance cls;

    res.m_dt = DATATYPE_CLASS;

    if (cls.ResolveToken(CLR_TkFromStream(m_sig), m_assm) == false)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }

    res.m_cls = cls.m_index;
    }
    break;

    case c_Object:
    {
    CLR_RT_TypeDescriptor desc;
    CLR_RT_HeapBlock *ptr = m_lst++;

    if (m_flags)
      {
      // Reflection types are now boxed, so unbox first
      if (ptr->DataType() == DATATYPE_OBJECT)
        {
        ptr = ptr->Dereference();
        }
      if (ptr->DataType() != DATATYPE_REFLECTION)
        {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

      NANOCLR_CHECK_HRESULT(desc.InitializeFromReflection(ptr->ReflectionDataConst()));
      }
    else
      {
      switch (ptr->DataType())
        {
        case DATATYPE_BYREF:
        case DATATYPE_ARRAY_BYREF:
          res.m_fByRef = true;
          break;

        default:
          // the remaining data types aren't to be handled
          break;
        }

      NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(*ptr));
      }

    desc.m_handlerCls.InitializeFromIndex(desc.m_reflex.m_data.m_type);

    res.m_levels = desc.m_reflex.m_levels;
    res.m_dt = (CLR_DataType)desc.m_handlerCls.m_target->dataType;
    res.m_cls = desc.m_reflex.m_data.m_type;

    //
    // Special case for Object types.
    //
    if (res.m_cls == g_CLR_RT_WellKnownTypes.m_Object)
      {
      res.m_dt = DATATYPE_OBJECT;
      }
    }
    break;

    default:
      while (true)
        {
        res.m_dt = CLR_UncompressElementType(m_sig);

        switch (res.m_dt)
          {
          case DATATYPE_BYREF:
            if (res.m_fByRef)
              {
              NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
              }

            res.m_fByRef = true;
            break;

          case DATATYPE_SZARRAY:
            res.m_levels++;
            break;

          case DATATYPE_CLASS:
          case DATATYPE_VALUETYPE:
          {
          uint32_t tk = CLR_TkFromStream(m_sig);

          if (CLR_TypeFromTk(tk) == TBL_TypeSpec)
            {
            CLR_RT_SignatureParser sub;
            sub.Initialize_TypeSpec(m_assm, m_assm->GetTypeSpec(CLR_DataFromTk(tk)));
            int extraLevels = res.m_levels;

            NANOCLR_CHECK_HRESULT(sub.Advance(res));

            res.m_levels += extraLevels;
            }
          else
            {
            CLR_RT_TypeDef_Instance cls;

            if (cls.ResolveToken(tk, m_assm) == false)
              {
              NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
              }

            res.m_cls = cls.m_index;
            }

          NANOCLR_SET_AND_LEAVE(S_OK);
          }

          case DATATYPE_OBJECT:
            res.m_cls = g_CLR_RT_WellKnownTypes.m_Object;

            NANOCLR_SET_AND_LEAVE(S_OK);

          case DATATYPE_VOID:
            res.m_cls = g_CLR_RT_WellKnownTypes.m_Void;

            NANOCLR_SET_AND_LEAVE(S_OK);

          default:
          {
          const uint32_t *cls = c_CLR_RT_DataTypeLookup[res.m_dt].m_cls;

          if (cls)
            {
            res.m_cls = *cls;
            NANOCLR_SET_AND_LEAVE(S_OK);
            }
          else
            {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }
          }
          }
        }
      break;
    }

  NANOCLR_NOCLEANUP();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_Assembly_Instance::InitializeFromIndex(uint32_t idx)
  {

  if (NANOCLR_INDEX_IS_VALID(idx))
    {
    m_index = idx;
    m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];

    return true;
    }

  m_index = 0;
  m_assm = NULL;

  return false;
  }

void CLR_RT_Assembly_Instance::Clear()
  {
  m_index = 0;

  m_assm = NULL;
  }

//////////////////////////////

bool CLR_RT_TypeSpec_Instance::InitializeFromIndex(uint32_t idx)
  {

  if (NANOCLR_INDEX_IS_VALID(idx))
    {
    m_index = idx;
    m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
    m_target = m_assm->GetSignature(m_assm->GetTypeSpec(get_type_index(m_index))->sig);

    return true;
    }

  m_index = 0;
  m_assm = NULL;
  m_target = NULL;

  return false;
  }

void CLR_RT_TypeSpec_Instance::Clear()
  {
  m_index = 0;
  m_assm = NULL;
  m_target = NULL;
  }

bool CLR_RT_TypeSpec_Instance::ResolveToken(uint32_t tk, CLR_RT_Assembly *assm)
  {

  if (assm && CLR_TypeFromTk(tk) == TBL_TypeSpec)
    {
    uint32_t idx = CLR_DataFromTk(tk);

    m_index = create_index(assm->m_idx, idx);

    m_assm = assm;
    m_target = assm->GetSignature(assm->GetTypeSpec(idx)->sig);

    return true;
    }

  Clear();

  return false;
  }

//////////////////////////////

bool CLR_RT_TypeDef_Instance::InitializeFromReflection(const CLR_RT_ReflectionDef_Index &reflex, uint32_t *levels)
  {

  uint32_t cls;
  const uint32_t *ptr = NULL;

  if (levels)
    *levels = reflex.m_levels;

  switch (reflex.m_kind)
    {
    case REFLECTION_TYPE:
      if (reflex.m_levels > 0 && levels == NULL)
        {
        ptr = &g_CLR_RT_WellKnownTypes.m_Array;
        }
      else
        {
        ptr = &reflex.m_data.m_type;
        }
      break;

    case REFLECTION_TYPE_DELAYED:
      if (g_CLR_RT_TypeSystem.FindTypeDef(reflex.m_data.m_raw, cls))
        {
        ptr = &cls;
        }
      break;
    }

  return ptr ? InitializeFromIndex(*ptr) : false;
  }

bool CLR_RT_TypeDef_Instance::InitializeFromIndex(uint32_t idx)
  {

  if (NANOCLR_INDEX_IS_VALID(idx))
    {
    m_index = idx;
    m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
    m_target = m_assm->GetTypeDef(get_type_index(m_index));

    return true;
    }

  m_index = 0;
  m_assm = NULL;
  m_target = NULL;

  return false;
  }

bool CLR_RT_TypeDef_Instance::InitializeFromMethod(const CLR_RT_MethodDef_Instance &md)
  {

  if (NANOCLR_INDEX_IS_VALID(md.m_index))
    {
    uint16_t idxAssm = get_assembly_index(md.m_index);
    uint16_t idxType = md.CrossReference().GetOwner();

    m_index = create_index(idxAssm, idxType);

    m_assm = g_CLR_RT_TypeSystem.m_assemblies[idxAssm - 1];
    m_target = m_assm->GetTypeDef(idxType);

    return true;
    }

  Clear();

  return false;
  }

bool CLR_RT_TypeDef_Instance::InitializeFromField(const CLR_RT_FieldDef_Instance &fd)
  {

  if (NANOCLR_INDEX_IS_VALID(fd.m_index))
    {
    CLR_RT_Assembly *assm = fd.m_assm;
    const CLR_RECORD_TYPEDEF *td = (const CLR_RECORD_TYPEDEF *)assm->GetTable(TBL_TypeDef);
    uint16_t idxField = get_field_index(fd.m_index);
    int i = assm->m_pTablesSize[TBL_TypeDef];

    if (fd.m_target->flags & CLR_RECORD_FIELDDEF::FD_Static)
      {
      for (; i; i--, td++)
        {
        if (td->sFields_First <= idxField && idxField < td->sFields_First + td->sFields_Num)
          {
          break;
          }
        }
      }
    else
      {
      for (; i; i--, td++)
        {
        if (td->iFields_First <= idxField && idxField < td->iFields_First + td->iFields_Num)
          {
          break;
          }
        }
      }

    if (i)
      {
      uint16_t idxAssm = get_assembly_index(fd.m_index);
      uint16_t idxType = assm->m_pTablesSize[TBL_TypeDef] - i;

      m_index = create_index(idxAssm, idxType);

      m_assm = g_CLR_RT_TypeSystem.m_assemblies[idxAssm - 1];
      m_target = m_assm->GetTypeDef(idxType);

      return true;
      }
    }

  Clear();

  return false;
  }

bool CLR_RT_TypeDef_Instance::IsATypeHandler()
  {

  return (m_index == g_CLR_RT_WellKnownTypes.m_Type || m_index == g_CLR_RT_WellKnownTypes.m_TypeStatic);
  }

void CLR_RT_TypeDef_Instance::Clear()
  {

  m_index = 0;

  m_assm = NULL;
  m_target = NULL;
  }

bool CLR_RT_TypeDef_Instance::ResolveToken(uint32_t tk, CLR_RT_Assembly *assm)
  {

  if (assm)
    {
    uint32_t idx = CLR_DataFromTk(tk);

    switch (CLR_TypeFromTk(tk))
      {
      case TBL_TypeRef:
        m_index = assm->m_pCrossReference_TypeRef[idx].m_target;
        m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
        m_target = m_assm->GetTypeDef(get_type_index(m_index));
        return true;

      case TBL_TypeDef:
        m_index = create_index(assm->m_idx, idx);

        m_assm = assm;
        m_target = assm->GetTypeDef(idx);
        return true;

      default:
        // the remaining data types aren't to be handled
        break;
      }
    }

  Clear();

  return false;
  }

//--//

bool CLR_RT_TypeDef_Instance::SwitchToParent()
  {

  if (NANOCLR_INDEX_IS_VALID(m_index))
    {
    uint16_t extends = m_target->extends;

    if (extends != CLR_EmptyIndex)
      {
      uint32_t tmp;
      const uint32_t *cls;

      if (extends & 0x8000) // TypeRef
        {
        cls = &m_assm->m_pCrossReference_TypeRef[extends & 0x7FFF].m_target;
        }
      else
        {
        tmp = create_index(get_assembly_index(m_index), extends);

        cls = &tmp;
        }

      return InitializeFromIndex(*cls);
      }
    }

  Clear();

  return false;
  }

bool CLR_RT_TypeDef_Instance::HasFinalizer() const
  {

  return NANOCLR_INDEX_IS_VALID(m_index) &&
    (CrossReference().m_flags & CLR_RT_TypeDef_CrossReference::TD_CR_HasFinalizer);
  }

//////////////////////////////

bool CLR_RT_FieldDef_Instance::InitializeFromIndex(uint32_t idx)
  {

  if (NANOCLR_INDEX_IS_VALID(idx))
    {
    m_index = idx;
    m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
    m_target = m_assm->GetFieldDef(get_field_index(m_index));

    return true;
    }

  m_index = 0;
  m_assm = NULL;
  m_target = NULL;

  return false;
  }

void CLR_RT_FieldDef_Instance::Clear()
  {

  m_index = 0;

  m_assm = NULL;
  m_target = NULL;
  }

bool CLR_RT_FieldDef_Instance::ResolveToken(uint32_t tk, CLR_RT_Assembly *assm)
  {

  if (assm)
    {
    uint32_t idx = CLR_DataFromTk(tk);

    switch (CLR_TypeFromTk(tk))
      {
      case TBL_FieldRef:
        m_index = assm->m_pCrossReference_FieldRef[idx].m_target;
        m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
        m_target = m_assm->GetFieldDef(get_field_index(m_index));
        return true;

      case TBL_FieldDef:
        m_index = create_index(assm->m_idx, idx);

        m_assm = assm;
        m_target = m_assm->GetFieldDef(idx);
        return true;

      default:
        // the remaining data types aren't to be handled
        break;
      }
    }

  Clear();

  return false;
  }

//////////////////////////////

bool CLR_RT_MethodDef_Instance::InitializeFromIndex(uint32_t idx)
  {

  if (NANOCLR_INDEX_IS_VALID(idx))
    {
    m_index = idx;
    m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
    m_target = m_assm->GetMethodDef(get_method_index(m_index));

    return true;
    }

  m_index = 0;
  m_assm = NULL;
  m_target = NULL;

  return false;
  }

void CLR_RT_MethodDef_Instance::Clear()
  {
  m_index = 0;
  m_assm = NULL;
  m_target = NULL;
  }

bool CLR_RT_MethodDef_Instance::ResolveToken(uint32_t tk, CLR_RT_Assembly *assm)
  {

  if (assm)
    {
    uint32_t idx = CLR_DataFromTk(tk);

    switch (CLR_TypeFromTk(tk))
      {
      case TBL_MethodRef:
        m_index = assm->m_pCrossReference_MethodRef[idx].m_target;
        m_assm = g_CLR_RT_TypeSystem.m_assemblies[get_assembly_index(m_index) - 1];
        m_target = m_assm->GetMethodDef(get_method_index(m_index));
        return true;

      case TBL_MethodDef:
        m_index = create_index(assm->m_idx, idx);

        m_assm = assm;
        m_target = m_assm->GetMethodDef(idx);
        return true;

      default:
        // the remaining data types aren't to be handled
        break;
      }
    }

  Clear();

  return false;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_TypeDescriptor::TypeDescriptor_Initialize()
  {

  m_flags = 0;          // uint32_t                 m_flags;
  m_handlerCls.Clear(); // CLR_RT_TypeDef_Instance    m_handlerCls;
                        //
  m_reflex.Clear();     // CLR_RT_ReflectionDef_Index m_reflex;
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromDataType(CLR_DataType dt)
  {

  HRESULT hr;

  if (dt >= DATATYPE_FIRST_INVALID)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
  else
    {
    const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dt];

    m_flags = dtl.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask2;

    if (dtl.m_cls)
      {
      if (m_handlerCls.InitializeFromIndex(*dtl.m_cls) == false)
        {
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

      m_reflex.m_kind = REFLECTION_TYPE;
      m_reflex.m_levels = 0;
      m_reflex.m_data.m_type = *dtl.m_cls;
      }
    else
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromReflection(const CLR_RT_ReflectionDef_Index &reflex)
  {

  HRESULT hr;

  CLR_RT_TypeDef_Instance inst;
  uint32_t levels;

  if (inst.InitializeFromReflection(reflex, &levels) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_CHECK_HRESULT(InitializeFromType(inst.m_index));

  if (levels)
    {
    m_reflex.m_levels = levels;

    ConvertToArray();
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromTypeSpec(uint32_t sig)
  {

  HRESULT hr;

  CLR_RT_TypeSpec_Instance inst;
  CLR_RT_SignatureParser parser;

  if (inst.InitializeFromIndex(sig) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  parser.Initialize_TypeSpec(inst.m_assm, inst.m_target);

  NANOCLR_SET_AND_LEAVE(InitializeFromSignatureParser(parser));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromType(uint32_t cls)
  {

  HRESULT hr;

  if (m_handlerCls.InitializeFromIndex(cls) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
  else
    {
    const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[m_handlerCls.m_target->dataType];

    m_flags = dtl.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask;

    m_reflex.m_kind = REFLECTION_TYPE;
    m_reflex.m_levels = 0;
    m_reflex.m_data.m_type = m_handlerCls.m_index;

    if (m_flags == CLR_RT_DataTypeLookup::c_Primitive)
      {
      if ((m_handlerCls.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) ==
        CLR_RECORD_TYPEDEF::TD_Semantics_Enum)
        {
        m_flags = CLR_RT_DataTypeLookup::c_Enum;
        }
      }
    else
      {
      switch (m_handlerCls.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask)
        {
        case CLR_RECORD_TYPEDEF::TD_Semantics_ValueType:
          m_flags = CLR_RT_DataTypeLookup::c_ValueType;
          break;
        case CLR_RECORD_TYPEDEF::TD_Semantics_Class:
          m_flags = CLR_RT_DataTypeLookup::c_Class;
          break;
        case CLR_RECORD_TYPEDEF::TD_Semantics_Interface:
          m_flags = CLR_RT_DataTypeLookup::c_Interface;
          break;
        case CLR_RECORD_TYPEDEF::TD_Semantics_Enum:
          m_flags = CLR_RT_DataTypeLookup::c_Enum;
          break;
        }
      }

    if (m_handlerCls.m_index == g_CLR_RT_WellKnownTypes.m_Array)
      {
      m_flags |= CLR_RT_DataTypeLookup::c_Array;
      }

    if (m_handlerCls.m_index == g_CLR_RT_WellKnownTypes.m_ArrayList)
      {
      m_flags |= CLR_RT_DataTypeLookup::c_ArrayList;
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromFieldDefinition(const CLR_RT_FieldDef_Instance &fd)
  {

  HRESULT hr;

  CLR_RT_SignatureParser parser;
  parser.Initialize_FieldDef(fd.m_assm, fd.m_target);

  NANOCLR_SET_AND_LEAVE(InitializeFromSignatureParser(parser));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromSignatureParser(CLR_RT_SignatureParser &parser)
  {

  HRESULT hr;

  CLR_RT_SignatureParser::Element res;

  if (parser.Available() <= 0)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

  NANOCLR_CHECK_HRESULT(parser.Advance(res));

  NANOCLR_CHECK_HRESULT(InitializeFromType(res.m_cls));

  if (res.m_levels)
    {
    m_reflex.m_levels = res.m_levels;

    ConvertToArray();
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::InitializeFromObject(const CLR_RT_HeapBlock &ref)
  {

  HRESULT hr;

  const CLR_RT_HeapBlock *obj = &ref;
  CLR_DataType dt;

  while (true)
    {
    dt = (CLR_DataType)obj->DataType();

    if (dt == DATATYPE_BYREF || dt == DATATYPE_OBJECT)
      {
      obj = obj->Dereference();
      FAULT_ON_NULL(obj);
      }
    else
      {
      break;
      }
    }

  {
  uint32_t cls = 0;
  const CLR_RT_ReflectionDef_Index *reflex = NULL;

  switch (dt)
    {
    case DATATYPE_SZARRAY:
      reflex = &obj->ReflectionDataConst();
      cls = reflex->m_data.m_type;
      break;

    case DATATYPE_VALUETYPE:
    case DATATYPE_CLASS:
      cls = obj->ObjectCls();
      break;

    case DATATYPE_DELEGATE_HEAD:
      {
      CLR_RT_HeapBlock_Delegate *dlg = (CLR_RT_HeapBlock_Delegate *)obj;

      cls = NANOCLR_INDEX_IS_VALID(dlg->m_cls) ? dlg->m_cls : g_CLR_RT_WellKnownTypes.m_Delegate;
      }
      break;

    case DATATYPE_DELEGATELIST_HEAD:
    {
    CLR_RT_HeapBlock_Delegate_List *dlgLst = (CLR_RT_HeapBlock_Delegate_List *)obj;

    cls = NANOCLR_INDEX_IS_VALID(dlgLst->m_cls) ? dlgLst->m_cls
      : g_CLR_RT_WellKnownTypes.m_MulticastDelegate;
    }
    break;
    case DATATYPE_WEAKCLASS:
      cls = g_CLR_RT_WellKnownTypes.m_WeakReference;
    break;
    case DATATYPE_REFLECTION:
      reflex = &(obj->ReflectionDataConst());

      switch (reflex->m_kind)
        {
        case REFLECTION_ASSEMBLY:
          cls = g_CLR_RT_WellKnownTypes.m_Assembly;
          break;
        case REFLECTION_TYPE:
          cls = g_CLR_RT_WellKnownTypes.m_Type;
          break;
        case REFLECTION_TYPE_DELAYED:
          cls = g_CLR_RT_WellKnownTypes.m_Type;
          break;
        case REFLECTION_CONSTRUCTOR:
          cls = g_CLR_RT_WellKnownTypes.m_ConstructorInfo;
          break;
        case REFLECTION_METHOD:
          cls = g_CLR_RT_WellKnownTypes.m_MethodInfo;
          break;
        case REFLECTION_FIELD:
          cls = g_CLR_RT_WellKnownTypes.m_FieldInfo;
          break;
        }

      break;

      //--//

    case DATATYPE_ARRAY_BYREF:
      {
      CLR_RT_HeapBlock_Array *array = obj->Array();
      FAULT_ON_NULL(array);

      if (array->m_fReference)
        {
        obj = (CLR_RT_HeapBlock *)array->GetElement(obj->ArrayIndex());

        NANOCLR_SET_AND_LEAVE(InitializeFromObject(*obj));
        }

      reflex = &array->ReflectionDataConst();
      cls = reflex->m_data.m_type;
      }
      break;

    default:
      NANOCLR_SET_AND_LEAVE(InitializeFromDataType(dt));
    }

  if (cls)
    {
    NANOCLR_CHECK_HRESULT(InitializeFromType(cls));
    }

  if (reflex)
    {
    m_reflex = *reflex;
    }

  if (dt == DATATYPE_SZARRAY)
    {
    ConvertToArray();
    }
  }

  NANOCLR_NOCLEANUP();
  }

////////////////////////////////////////

void CLR_RT_TypeDescriptor::ConvertToArray()
  {

  m_flags &= CLR_RT_DataTypeLookup::c_SemanticMask;
  m_flags |= CLR_RT_DataTypeLookup::c_Array;

  m_handlerCls.InitializeFromIndex(g_CLR_RT_WellKnownTypes.m_Array);
  }

bool CLR_RT_TypeDescriptor::ShouldEmitHash()
  {

  if (m_flags & (CLR_RT_DataTypeLookup::c_Array | CLR_RT_DataTypeLookup::c_ArrayList))
    {
    return true;
    }

  if (m_flags &
    (CLR_RT_DataTypeLookup::c_Primitive | CLR_RT_DataTypeLookup::c_ValueType | CLR_RT_DataTypeLookup::c_Enum))
    {
    return false;
    }

  if (m_handlerCls.CrossReference().m_hash != 0)
    {
    return true;
    }

  return false;
  }

bool CLR_RT_TypeDescriptor::GetElementType(CLR_RT_TypeDescriptor &sub)
  {

  switch (m_reflex.m_levels)
    {
    case 0:
      return false;

    case 1:
      sub.InitializeFromType(m_reflex.m_data.m_type);
      break;

    default:
      sub = *this;
      sub.m_reflex.m_levels--;
      break;
    }

  return true;
  }

////////////////////////////////////////

HRESULT CLR_RT_TypeDescriptor::ExtractObjectAndDataType(CLR_RT_HeapBlock *&ref, CLR_DataType &dt)
  {
  HRESULT hr;

  while (true)
    {
    dt = (CLR_DataType)ref->DataType();

    if (dt == DATATYPE_BYREF || dt == DATATYPE_OBJECT)
      {
      ref = ref->Dereference();
      FAULT_ON_NULL(ref);
      }
    else
      {
      break;
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeDescriptor::ExtractTypeIndexFromObject(const CLR_RT_HeapBlock &ref, uint32_t &res)
  {
  HRESULT hr;

  CLR_RT_HeapBlock *obj = (CLR_RT_HeapBlock *)&ref;
  CLR_DataType dt;

  NANOCLR_CHECK_HRESULT(CLR_RT_TypeDescriptor::ExtractObjectAndDataType(obj, dt));

  if (dt == DATATYPE_VALUETYPE || dt == DATATYPE_CLASS)
    {
    res = obj->ObjectCls();
    }
  else
    {
    const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dt];

    if (dtl.m_cls)
      {
      res = *dtl.m_cls;
      }
    else
      {
      res = 0;
      }
    }

  if (NANOCLR_INDEX_IS_INVALID(res))
    {
    CLR_RT_TypeDescriptor desc;

    NANOCLR_CHECK_HRESULT(desc.InitializeFromObject(ref))

      // If desc.InitializeFromObject( ref ) call succeded, then we use m_handlerCls for res
      res = desc.m_handlerCls.m_index;

    if (NANOCLR_INDEX_IS_INVALID(res))
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }
    }

  NANOCLR_NOCLEANUP();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Keep this string less than 8-character long (including terminator) because it's stuffed into an 8-byte structure.
//
static const char c_MARKER_ASSEMBLY_V1[] = "NFMRK1";

bool CLR_RECORD_ASSEMBLY::GoodHeader() const
  {

  CLR_RECORD_ASSEMBLY header = *this;
  header.headerCRC = 0;

  if (SUPPORT_ComputeCRC(&header, sizeof(header), 0) != this->headerCRC)
    return false;

  if (this->stringTableVersion != c_CLR_StringTable_Version)
    return false;

  return memcmp(marker, c_MARKER_ASSEMBLY_V1, sizeof(c_MARKER_ASSEMBLY_V1)) == 0;
  }

bool CLR_RECORD_ASSEMBLY::GoodAssembly() const
  {
  if (!GoodHeader())
    return false;

  return SUPPORT_ComputeCRC(&this[1], this->TotalSize() - sizeof(*this), 0) == this->assemblyCRC;
  }

#if defined(WIN32)

void CLR_RECORD_ASSEMBLY::ComputeCRC()
  {

  memcpy(marker, c_MARKER_ASSEMBLY_V1, sizeof(marker));

  headerCRC = 0;
  assemblyCRC = SUPPORT_ComputeCRC(&this[1], this->TotalSize() - sizeof(*this), 0);
  headerCRC = SUPPORT_ComputeCRC(this, sizeof(*this), 0);
  }

#endif

uint32_t CLR_RECORD_ASSEMBLY::ComputeAssemblyHash(const char *name, const CLR_RECORD_VERSION &ver)
  {

  uint32_t assemblyHASH;

  assemblyHASH = SUPPORT_ComputeCRC(name, (int)hal_strlen_s(name), 0);
  assemblyHASH = SUPPORT_ComputeCRC(&ver, sizeof(ver), assemblyHASH);

  return assemblyHASH;
  }

CLR_PMETADATA CLR_RECORD_EH::ExtractEhFromByteCode(CLR_PMETADATA ipEnd, const CLR_RECORD_EH *&ptrEh, uint32_t &numEh)
  {

  uint32_t num = *(--ipEnd);
  ipEnd -= sizeof(CLR_RECORD_EH) * num;

  numEh = num;
  ptrEh = (const CLR_RECORD_EH *)ipEnd;

  return ipEnd;
  }

uint32_t CLR_RECORD_EH::GetToken() const
  {

  if (classToken & 0x8000)
    {
    return CLR_TkFromType(TBL_TypeRef, classToken & 0x7FFF);
    }
  else
    {
    return CLR_TkFromType(TBL_TypeDef, classToken);
    }
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_ExceptionHandler::ConvertFromEH(
  const CLR_RT_MethodDef_Instance &owner,
  CLR_PMETADATA ipStart,
  const CLR_RECORD_EH *ehPtr)
  {
  CLR_RECORD_EH eh;
  memcpy(&eh, ehPtr, sizeof(eh));

  switch (eh.mode)
    {
    case CLR_RECORD_EH::EH_Finally:
      m_typeFilter = 0;
      break;

    case CLR_RECORD_EH::EH_Filter:
      m_userFilterStart = ipStart + eh.filterStart;
      break;

    case CLR_RECORD_EH::EH_CatchAll:
      m_typeFilter = g_CLR_RT_WellKnownTypes.m_Object;
      break;

    case CLR_RECORD_EH::EH_Catch:
    {
    CLR_RT_TypeDef_Instance cls;
    if (cls.ResolveToken(eh.GetToken(), owner.m_assm) == false)
      return false;
    m_typeFilter = cls.m_index;
    }
    break;

    default:
      return false;
    }

  if (owner.m_target->RVA == CLR_EmptyIndex)
    return false;

  m_ehType = eh.mode;
  m_tryStart = ipStart + eh.tryStart;
  m_tryEnd = ipStart + eh.tryEnd;
  m_handlerStart = ipStart + eh.handlerStart;
  m_handlerEnd = ipStart + eh.handlerEnd;

  return true;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_Assembly::IsSameAssembly(const CLR_RT_Assembly &assm) const
  {
  if (m_header->headerCRC == assm.m_header->headerCRC && m_header->assemblyCRC == assm.m_header->assemblyCRC)
    {
    return true;
    }

  return false;
  }

void CLR_RT_Assembly::Assembly_Initialize(CLR_RT_Assembly::Offsets &offsets)
  {

  uint8_t *buffer = (uint8_t *)this;
  int i;

  m_szName = GetString(m_header->assemblyName);

  //--//
  buffer += offsets.iBase;
  m_pCrossReference_AssemblyRef = (CLR_RT_AssemblyRef_CrossReference *)buffer;
  buffer += offsets.iAssemblyRef;
  m_pCrossReference_TypeRef = (CLR_RT_TypeRef_CrossReference *)buffer;
  buffer += offsets.iTypeRef;
  m_pCrossReference_FieldRef = (CLR_RT_FieldRef_CrossReference *)buffer;
  buffer += offsets.iFieldRef;
  m_pCrossReference_MethodRef = (CLR_RT_MethodRef_CrossReference *)buffer;
  buffer += offsets.iMethodRef;
  m_pCrossReference_TypeDef = (CLR_RT_TypeDef_CrossReference *)buffer;
  buffer += offsets.iTypeDef;
  m_pCrossReference_FieldDef = (CLR_RT_FieldDef_CrossReference *)buffer;
  buffer += offsets.iFieldDef;
  m_pCrossReference_MethodDef = (CLR_RT_MethodDef_CrossReference *)buffer;
  buffer += offsets.iMethodDef;

  m_pStaticFields = (CLR_RT_HeapBlock *)buffer;
  buffer += offsets.iStaticFields;

  memset(m_pStaticFields, 0, offsets.iStaticFields);

  {ITERATE_THROUGH_RECORDS(this, i, TypeDef, TYPEDEF) {
    dst->m_flags = 0;
    dst->m_totalFields = 0;
    dst->m_hash = 0;
    }
  }

  {ITERATE_THROUGH_RECORDS(this, i, FieldDef, FIELDDEF) {
    dst->m_offset = CLR_EmptyIndex;
    }
  }

  {
  ITERATE_THROUGH_RECORDS(this, i, MethodDef, METHODDEF)
    {
    dst->m_data = CLR_EmptyIndex;
    }
  }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  {
  m_pDebuggingInfo_MethodDef = (CLR_RT_MethodDef_DebuggingInfo *)buffer;
  buffer += offsets.iDebuggingInfoMethods;

  memset(m_pDebuggingInfo_MethodDef, 0, offsets.iDebuggingInfoMethods);
  }
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  }

HRESULT CLR_RT_Assembly::CreateInstance(const CLR_RECORD_ASSEMBLY *header, CLR_RT_Assembly *&assm)
  {

  //
  // We have to use this trick, otherwise the C++ compiler will try to all the constructor for Assembly.
  //
  HRESULT hr;

  uint8_t buf[sizeof(CLR_RT_Assembly)];
  CLR_RT_Assembly *skeleton = (CLR_RT_Assembly *)buf;

  NANOCLR_CLEAR(*skeleton);

  if (header->GoodAssembly() == false)
    NANOCLR_MSG_SET_AND_LEAVE(CLR_E_FAIL, L"Failed in type system: assembly is not good.\n");

  skeleton->m_header = header;

  //
  // Compute overall size for assembly data structure.
  //
  {
  for (uint32_t i = 0; i < ARRAYSIZE(skeleton->m_pTablesSize) - 1; i++)
    {
    skeleton->m_pTablesSize[i] = header->SizeOfTable((CLR_TABLESENUM)i);
    }

  skeleton->m_pTablesSize[TBL_AssemblyRef] /= sizeof(CLR_RECORD_ASSEMBLYREF);
  skeleton->m_pTablesSize[TBL_TypeRef] /= sizeof(CLR_RECORD_TYPEREF);
  skeleton->m_pTablesSize[TBL_FieldRef] /= sizeof(CLR_RECORD_FIELDREF);
  skeleton->m_pTablesSize[TBL_MethodRef] /= sizeof(CLR_RECORD_METHODREF);
  skeleton->m_pTablesSize[TBL_TypeDef] /= sizeof(CLR_RECORD_TYPEDEF);
  skeleton->m_pTablesSize[TBL_FieldDef] /= sizeof(CLR_RECORD_FIELDDEF);
  skeleton->m_pTablesSize[TBL_MethodDef] /= sizeof(CLR_RECORD_METHODDEF);
  skeleton->m_pTablesSize[TBL_Attributes] /= sizeof(CLR_RECORD_ATTRIBUTE);
  skeleton->m_pTablesSize[TBL_TypeSpec] /= sizeof(CLR_RECORD_TYPESPEC);
  skeleton->m_pTablesSize[TBL_Resources] /= sizeof(CLR_RECORD_RESOURCE);
  skeleton->m_pTablesSize[TBL_ResourcesFiles] /= sizeof(CLR_RECORD_RESOURCE_FILE);
  }

  //--//

  //
  // Count static fields.
  //
  {
  const CLR_RECORD_TYPEDEF *src = (const CLR_RECORD_TYPEDEF *)skeleton->GetTable(TBL_TypeDef);

  for (int i = 0; i < skeleton->m_pTablesSize[TBL_TypeDef]; i++, src++)
    {
    skeleton->m_iStaticFields += src->sFields_Num;
    }
  }

  //--//

  {
  CLR_RT_Assembly::Offsets offsets;

  offsets.iBase = ROUNDTOMULTIPLE(sizeof(CLR_RT_Assembly), uint32_t);
  offsets.iAssemblyRef = ROUNDTOMULTIPLE(
    skeleton->m_pTablesSize[TBL_AssemblyRef] * sizeof(CLR_RT_AssemblyRef_CrossReference),
    uint32_t);
  offsets.iTypeRef =
    ROUNDTOMULTIPLE(skeleton->m_pTablesSize[TBL_TypeRef] * sizeof(CLR_RT_TypeRef_CrossReference), uint32_t);
  offsets.iFieldRef =
    ROUNDTOMULTIPLE(skeleton->m_pTablesSize[TBL_FieldRef] * sizeof(CLR_RT_FieldRef_CrossReference), uint32_t);
  offsets.iMethodRef = ROUNDTOMULTIPLE(
    skeleton->m_pTablesSize[TBL_MethodRef] * sizeof(CLR_RT_MethodRef_CrossReference),
    uint32_t);
  offsets.iTypeDef =
    ROUNDTOMULTIPLE(skeleton->m_pTablesSize[TBL_TypeDef] * sizeof(CLR_RT_TypeDef_CrossReference), uint32_t);
  offsets.iFieldDef =
    ROUNDTOMULTIPLE(skeleton->m_pTablesSize[TBL_FieldDef] * sizeof(CLR_RT_FieldDef_CrossReference), uint32_t);
  offsets.iMethodDef = ROUNDTOMULTIPLE(
    skeleton->m_pTablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_CrossReference),
    uint32_t);

  if (skeleton->m_header->numOfPatchedMethods > 0)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_ASSM_PATCHING_NOT_SUPPORTED);
    }

#if !defined(NANOCLR_APPDOMAINS)
  offsets.iStaticFields = ROUNDTOMULTIPLE(skeleton->m_iStaticFields * sizeof(CLR_RT_HeapBlock), uint32_t);
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  offsets.iDebuggingInfoMethods = ROUNDTOMULTIPLE(
    skeleton->m_pTablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_DebuggingInfo),
    uint32_t);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

  size_t iTotalRamSize = offsets.iBase + offsets.iAssemblyRef + offsets.iTypeRef + offsets.iFieldRef +
    offsets.iMethodRef + offsets.iTypeDef + offsets.iFieldDef + offsets.iMethodDef;

#if !defined(NANOCLR_APPDOMAINS)
  iTotalRamSize += offsets.iStaticFields;
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  iTotalRamSize += offsets.iDebuggingInfoMethods;
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

  //--//

  assm = EVENTCACHE_EXTRACT_NODE_AS_BYTES(
    g_CLR_RT_EventCache,
    CLR_RT_Assembly,
    DATATYPE_ASSEMBLY,
    0,
    (uint32_t)iTotalRamSize);
  CHECK_ALLOCATION(assm);

  {
  //
  // We don't want to blow away the block header...
  //
  CLR_RT_HeapBlock *src = skeleton;
  CLR_RT_HeapBlock *dst = assm;

  memset(&dst[1], 0, iTotalRamSize - sizeof(CLR_RT_HeapBlock));
  memcpy(&dst[1], &src[1], sizeof(*assm) - sizeof(CLR_RT_HeapBlock));
  }

  assm->Assembly_Initialize(offsets);

#if !defined(BUILD_RTM)
  trace_debug(
    "   Assembly: %s (%d.%d.%d.%d)  ",
    assm->m_szName,
    header->version.iMajorVersion,
    header->version.iMinorVersion,
    header->version.iBuildNumber,
    header->version.iRevisionNumber);

  if (s_CLR_RT_fTrace_AssemblyOverhead >= c_CLR_RT_Trace_Info)
    {
    size_t iMetaData = header->SizeOfTable(TBL_AssemblyRef) + header->SizeOfTable(TBL_TypeRef) +
      header->SizeOfTable(TBL_FieldRef) + header->SizeOfTable(TBL_MethodRef) +
      header->SizeOfTable(TBL_TypeDef) + header->SizeOfTable(TBL_FieldDef) +
      header->SizeOfTable(TBL_MethodDef) + header->SizeOfTable(TBL_Attributes) +
      header->SizeOfTable(TBL_TypeSpec) + header->SizeOfTable(TBL_Signatures);

    trace_debug(
      " (%d RAM - %d ROM - %d METADATA)\r\n\r\n",
      iTotalRamSize,
      header->TotalSize(),
      iMetaData);

    trace_debug(
      "   AssemblyRef    = %8d bytes (%8d elements)\r\n",
      offsets.iAssemblyRef,
      skeleton->m_pTablesSize[TBL_AssemblyRef]);
    trace_debug(
      "   TypeRef        = %8d bytes (%8d elements)\r\n",
      offsets.iTypeRef,
      skeleton->m_pTablesSize[TBL_TypeRef]);
    trace_debug(
      "   FieldRef       = %8d bytes (%8d elements)\r\n",
      offsets.iFieldRef,
      skeleton->m_pTablesSize[TBL_FieldRef]);
    trace_debug(
      "   MethodRef      = %8d bytes (%8d elements)\r\n",
      offsets.iMethodRef,
      skeleton->m_pTablesSize[TBL_MethodRef]);
    trace_debug(
      "   TypeDef        = %8d bytes (%8d elements)\r\n",
      offsets.iTypeDef,
      skeleton->m_pTablesSize[TBL_TypeDef]);
    trace_debug(
      "   FieldDef       = %8d bytes (%8d elements)\r\n",
      offsets.iFieldDef,
      skeleton->m_pTablesSize[TBL_FieldDef]);
    trace_debug(
      "   MethodDef      = %8d bytes (%8d elements)\r\n",
      offsets.iMethodDef,
      skeleton->m_pTablesSize[TBL_MethodDef]);
#if !defined(NANOCLR_APPDOMAINS)
    trace_debug(
      "   StaticFields   = %8d bytes (%8d elements)\r\n",
      offsets.iStaticFields,
      skeleton->m_iStaticFields);
#endif
    trace_debug("\r\n");

    trace_debug(
      "   Attributes      = %8d bytes (%8d elements)\r\n",
      skeleton->m_pTablesSize[TBL_Attributes] * sizeof(CLR_RECORD_ATTRIBUTE),
      skeleton->m_pTablesSize[TBL_Attributes]);
    trace_debug(
      "   TypeSpec        = %8d bytes (%8d elements)\r\n",
      skeleton->m_pTablesSize[TBL_TypeSpec] * sizeof(CLR_RECORD_TYPESPEC),
      skeleton->m_pTablesSize[TBL_TypeSpec]);
    trace_debug(
      "   Resources       = %8d bytes (%8d elements)\r\n",
      skeleton->m_pTablesSize[TBL_Resources] * sizeof(CLR_RECORD_RESOURCE),
      skeleton->m_pTablesSize[TBL_Resources]);
    trace_debug(
      "   Resources Files = %8d bytes (%8d elements)\r\n",
      skeleton->m_pTablesSize[TBL_ResourcesFiles] * sizeof(CLR_RECORD_RESOURCE),
      skeleton->m_pTablesSize[TBL_ResourcesFiles]);
    trace_debug("   Resources Data  = %8d bytes\r\n", skeleton->m_pTablesSize[TBL_ResourcesData]);
    trace_debug("   Strings         = %8d bytes\r\n", skeleton->m_pTablesSize[TBL_Strings]);
    trace_debug("   Signatures      = %8d bytes\r\n", skeleton->m_pTablesSize[TBL_Signatures]);
    trace_debug("   ByteCode        = %8d bytes\r\n", skeleton->m_pTablesSize[TBL_ByteCode]);
    trace_debug("\r\n\r\n");
    }
#endif
  }

  NANOCLR_NOCLEANUP();
  }

#if defined(WIN32)
HRESULT CLR_RT_Assembly::CreateInstance(
  const CLR_RECORD_ASSEMBLY *header,
  CLR_RT_Assembly *&assm,
  const wchar_t *szName)
  {

  HRESULT hr;

  std::string strPath;

  NANOCLR_CHECK_HRESULT(CLR_RT_Assembly::CreateInstance(header, assm));

  if (szName != NULL)
    {
    CLR_RT_UnicodeHelper::ConvertToUTF8(szName, strPath);

    assm->m_strPath = new std::string(strPath);
    }

  NANOCLR_NOCLEANUP();
  }
#endif

bool CLR_RT_Assembly::Resolve_AssemblyRef(bool fOutput)
  {

  bool fGot = true;
  int i;

  ITERATE_THROUGH_RECORDS(this, i, AssemblyRef, ASSEMBLYREF)
    {
    const char *szName = GetString(src->name);

    if (dst->m_target == NULL)
      {
      CLR_RT_Assembly *target = g_CLR_RT_TypeSystem.FindAssembly(szName, &src->version, false);

      if (target == NULL || (target->m_flags & CLR_RT_Assembly::Resolved) == 0)
        {
#if !defined(BUILD_RTM)
        if (fOutput)
          {
          trace_debug(
            "Assembly: %s (%d.%d.%d.%d)",
            m_szName,
            m_header->version.iMajorVersion,
            m_header->version.iMinorVersion,
            m_header->version.iBuildNumber,
            m_header->version.iRevisionNumber);

          trace_debug(
            " needs assembly '%s' (%d.%d.%d.%d)\r\n",
            szName,
            src->version.iMajorVersion,
            src->version.iMinorVersion,
            src->version.iBuildNumber,
            src->version.iRevisionNumber);
          }
#endif

        fGot = false;
        }
      else
        {
        dst->m_target = target;
        }
      }
    }

  return fGot;
  }

void CLR_RT_Assembly::DestroyInstance()
  {

  if (m_idx)
    {
    g_CLR_RT_TypeSystem.m_assemblies[m_idx - 1] = NULL;
    }

#if defined(WIN32)
  if (this->m_strPath != NULL)
    {
    delete this->m_strPath;
    this->m_strPath = NULL;
    }
#endif

  //--//

  g_CLR_RT_EventCache.Append_Node(this);
  }

//--//
HRESULT CLR_RT_Assembly::Resolve_TypeRef()
  {

  HRESULT hr;

  int i;

  ITERATE_THROUGH_RECORDS(this, i, TypeRef, TYPEREF)
    {
    if (src->scope & 0x8000) // Flag for TypeRef
      {
      CLR_RT_TypeDef_Instance inst;

      if (inst.InitializeFromIndex(m_pCrossReference_TypeRef[src->scope & 0x7FFF].m_target) == false)
        {
#if !defined(BUILD_RTM)
        trace_debug("Resolve: unknown scope: %08x\r\n", src->scope);
#endif

#if defined(_WIN32)
        NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown scope: %08x\r\n", src->scope);
#else
        NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown scope: %08x\r\n", src->scope);
#endif
        }

      const char *szName = GetString(src->name);
      if (inst.m_assm->FindTypeDef(szName, get_type_index(inst.m_index), dst->m_target) == false)
        {
#if !defined(BUILD_RTM)
        trace_debug("Resolve: unknown type: %s\r\n", szName);
#endif

#if defined(_WIN32)
        NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown type: %s\r\n", szName);
#else
        NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown type: %s\r\n", szName);
#endif
        }
      }
    else
      {
      CLR_RT_Assembly *assm = m_pCrossReference_AssemblyRef[src->scope].m_target;
      if (assm == NULL)
        {
        NANOCLR_MSG_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: assm is null\n");
        }

      const char *szNameSpace = GetString(src->nameSpace);
      const char *szName = GetString(src->name);
      if (assm->FindTypeDef(szName, szNameSpace, dst->m_target) == false)
        {
#if !defined(BUILD_RTM)
        trace_debug("Resolve: unknown type: %s.%s\r\n", szNameSpace, szName);
#endif

#if defined(_WIN32)
        NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown type: %s.%s\r\n", szNameSpace, szName);
#else
        NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown type: %s\r\n", szName);
#endif
        }
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_Assembly::Resolve_FieldRef()
  {

  HRESULT hr;

  int i;

  ITERATE_THROUGH_RECORDS(this, i, FieldRef, FIELDREF)
    {
    CLR_RT_TypeDef_Instance inst;

    if (inst.InitializeFromIndex(m_pCrossReference_TypeRef[src->container].m_target) == false)
      {
#if !defined(BUILD_RTM)
      trace_debug("Resolve Field: unknown scope: %08x\r\n", src->container);
#endif

#if defined(_WIN32)
      NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve Field: unknown scope: %08x\r\n", src->container);
#else
      NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve Field: unknown scope: %08x\r\n", src->container);
#endif
      }

    const char *szName = GetString(src->name);

    if (inst.m_assm->FindFieldDef(inst.m_target, szName, this, src->sig, dst->m_target) == false)
      {
#if !defined(BUILD_RTM)
      trace_debug("Resolve: unknown field: %s\r\n", szName);
#endif

#if defined(_WIN32)
      NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown field: %s\r\n", szName);
#else
      NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown field: %s\r\n", szName);
#endif
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_Assembly::Resolve_MethodRef()
  {

  HRESULT hr;

  int i;

  ITERATE_THROUGH_RECORDS(this, i, MethodRef, METHODREF)
    {
    CLR_RT_TypeDef_Instance inst;

    if (inst.InitializeFromIndex(m_pCrossReference_TypeRef[src->container].m_target) == false)
      {
#if !defined(BUILD_RTM)
      trace_debug("Resolve Field: unknown scope: %08x\r\n", src->container);
#endif

#if defined(_WIN32)
      NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve Field: unknown scope: %08x\r\n", src->container);
#else
      NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve Field: unknown scope: %08x\r\n", src->container);
#endif
      }

    const char *name = GetString(src->name);
    bool fGot = false;

    while (NANOCLR_INDEX_IS_VALID(inst.m_index))
      {
      if (inst.m_assm->FindMethodDef(inst.m_target, name, this, src->sig, dst->m_target))
        {
        fGot = true;
        break;
        }

      inst.SwitchToParent();
      }

    if (fGot == false)
      {
      inst.InitializeFromIndex(m_pCrossReference_TypeRef[src->container].m_target);

      const CLR_RECORD_TYPEDEF *qTD = inst.m_target;
      CLR_RT_Assembly *qASSM = inst.m_assm;

#if !defined(BUILD_RTM)
      trace_debug(
        "Resolve: unknown method: %s.%s.%s\r\n",
        qASSM->GetString(qTD->nameSpace),
        qASSM->GetString(qTD->name),
        name);
#endif

#if defined(_WIN32)
      NANOCLR_CHARMSG_SET_AND_LEAVE(CLR_E_FAIL, "Resolve: unknown method: %s\r\n", name);
#else
      NANOCLR_MSG1_SET_AND_LEAVE(CLR_E_FAIL, L"Resolve: unknown method: %s\r\n", name);
#endif
      }
    }

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_Assembly::Resolve_Link()
  {

  int iStaticFields = 0;
  int idxType;

  ITERATE_THROUGH_RECORDS(this, idxType, TypeDef, TYPEDEF)
    {
    int num;
    int i;

    //
    // Link static fields.
    //
    {
    CLR_RT_FieldDef_CrossReference *fd = &m_pCrossReference_FieldDef[src->sFields_First];

    num = src->sFields_Num;

    for (; num; num--, fd++)
      {
      fd->m_offset = iStaticFields++;
      }
    }

    //
    // Link instance fields.
    //
    {
    uint32_t idx;
    idx = create_index(m_idx, idxType);
    CLR_RT_TypeDef_Instance inst;
    inst.InitializeFromIndex(idx);
    uint16_t tot = 0;

    do
      {
      if (inst.m_target->flags & CLR_RECORD_TYPEDEF::TD_HasFinalizer)
        {
        dst->m_flags |= CLR_RT_TypeDef_CrossReference::TD_CR_HasFinalizer;
        }

      tot += inst.m_target->iFields_Num;
      } while (inst.SwitchToParent());

      dst->m_totalFields = tot;

      //--//

      CLR_RT_FieldDef_CrossReference *fd = &m_pCrossReference_FieldDef[src->iFields_First];

      num = src->iFields_Num;
      i = tot - num + CLR_RT_HeapBlock::HB_Object_Fields_Offset; // Take into account the offset from the
                                                                 // beginning of the object.

      for (; num; num--, i++, fd++)
        {
        fd->m_offset = i;
        }
    }

    //
    // Link methods.
    //
    {
    CLR_RT_MethodDef_CrossReference *md = &m_pCrossReference_MethodDef[src->methods_First];

    int num = src->vMethods_Num + src->iMethods_Num + src->sMethods_Num;

    for (; num; num--, md++)
      {
      md->m_data = idxType;
      }
    }
    }
  }

struct TypeIndexLookup
  {
  const char *nameSpace;
  const char *name;
  uint32_t *ptr;
  };

static const TypeIndexLookup c_TypeIndexLookup[] = {
#define TIL(ns, nm, fld)                                                                                               \
    {                                                                                                                  \
        ns, nm, &g_CLR_RT_WellKnownTypes.fld                                                                           \
    }
    TIL("System", "Boolean", m_Boolean),
    TIL("System", "Char", m_Char),
    TIL("System", "SByte", m_Int8),
    TIL("System", "Byte", m_UInt8),
    TIL("System", "Int16", m_Int16),
    TIL("System", "UInt16", m_UInt16),
    TIL("System", "Int32", m_Int32),
    TIL("System", "UInt32", m_UInt32),
    TIL("System", "Int64", m_Int64),
    TIL("System", "UInt64", m_UInt64),
    TIL("System", "Single", m_Single),
    TIL("System", "Double", m_Double),
    TIL("System", "DateTime", m_DateTime),
    TIL("System", "TimeSpan", m_TimeSpan),
    TIL("System", "String", m_String),

    TIL("System", "Void", m_Void),
    TIL("System", "Object", m_Object),
    TIL("System", "ValueType", m_ValueType),
    TIL("System", "Enum", m_Enum),

    TIL("System", "AppDomainUnloadedException", m_AppDomainUnloadedException),
    TIL("System", "ArgumentNullException", m_ArgumentNullException),
    TIL("System", "ArgumentException", m_ArgumentException),
    TIL("System", "ArgumentOutOfRangeException", m_ArgumentOutOfRangeException),
    TIL("System", "Exception", m_Exception),
    TIL("System", "IndexOutOfRangeException", m_IndexOutOfRangeException),
    TIL("System", "InvalidCastException", m_InvalidCastException),
    TIL("System", "InvalidOperationException", m_InvalidOperationException),
    TIL("System", "NotSupportedException", m_NotSupportedException),
    TIL("System", "NotImplementedException", m_NotImplementedException),
    TIL("System", "NullReferenceException", m_NullReferenceException),
    TIL("System", "OutOfMemoryException", m_OutOfMemoryException),
    TIL("System", "ObjectDisposedException", m_ObjectDisposedException),
    TIL("System.Threading", "ThreadAbortException", m_ThreadAbortException),
    TIL("nanoFramework.Runtime.Native", "ConstraintException", m_ConstraintException),

    TIL("System", "Delegate", m_Delegate),
    TIL("System", "MulticastDelegate", m_MulticastDelegate),

    TIL("System", "Array", m_Array),
    TIL("System.Collections", "ArrayList", m_ArrayList),
    TIL("System", "ICloneable", m_ICloneable),
    TIL("System.Collections", "IList", m_IList),

    TIL("System.Reflection", "Assembly", m_Assembly),
    TIL("System", "Type", m_TypeStatic),
    TIL("System", "RuntimeType", m_Type),
    TIL("System.Reflection", "RuntimeConstructorInfo", m_ConstructorInfo),
    TIL("System.Reflection", "RuntimeMethodInfo", m_MethodInfo),
    TIL("System.Reflection", "RuntimeFieldInfo", m_FieldInfo),

    TIL("System", "WeakReference", m_WeakReference),

    TIL("Microsoft.SPOT", "SerializationHintsAttribute", m_SerializationHintsAttribute),
    TIL("Microsoft.SPOT.Hardware", "WatchdogException", m_WatchdogException),

    TIL("nanoFramework.UI", "Bitmap", m_Bitmap),
    TIL("nanoFramework.UI", "Font", m_Font),
    TIL("nanoFramework.Touch", "TouchEvent", m_TouchEvent),
    TIL("nanoFramework.Touch", "TouchInput", m_TouchInput),

    TIL("System.Threading", "Thread", m_Thread),

#undef TIL
  };

struct MethodIndexLookup
  {
  const char *name;
  uint32_t *type;
  uint32_t *method;
  };

static const MethodIndexLookup c_MethodIndexLookup[] = {
#define MIL(nm, type, method)                                                                                          \
    {                                                                                                                  \
        nm, &g_CLR_RT_WellKnownTypes.type, &g_CLR_RT_WellKnownMethods.method                                           \
    }

    MIL("GetObjectFromId", m_ResourceManager, m_ResourceManager_GetObjectFromId),
    MIL("GetObjectChunkFromId", m_ResourceManager, m_ResourceManager_GetObjectChunkFromId),

#undef MIL
  };

void CLR_RT_Assembly::Resolve_TypeDef()
  {

  const TypeIndexLookup *tilOuterClass = NULL;
  const TypeIndexLookup *til = c_TypeIndexLookup;

  for (size_t i = 0; i < ARRAYSIZE(c_TypeIndexLookup); i++, til++)
    {
    uint32_t &dst = *til->ptr;

    if (NANOCLR_INDEX_IS_INVALID(dst))
      {
      if (til->nameSpace == NULL)
        {
        if (tilOuterClass)
          {
          FindTypeDef(til->name, get_type_index(*tilOuterClass->ptr), dst);
          }
        }
      else
        {
        FindTypeDef(til->name, til->nameSpace, dst);
        }
      }

    if (til->nameSpace != NULL)
      {
      tilOuterClass = til;
      }
    }
  }

void CLR_RT_Assembly::Resolve_MethodDef()
  {

  const CLR_RECORD_METHODDEF *md = GetMethodDef(0);

  for (int i = 0; i < m_pTablesSize[TBL_MethodDef]; i++, md++)
    {
    const MethodIndexLookup *mil = c_MethodIndexLookup;

    uint32_t idx = create_index(m_idx, i);

    // Check for wellKnownMethods
    for (size_t i = 0; i < ARRAYSIZE(c_MethodIndexLookup); i++, mil++)
      {
      uint32_t &idxType = *mil->type;
      uint32_t &idxMethod = *mil->method;

      if (NANOCLR_INDEX_IS_VALID(idxType) && NANOCLR_INDEX_IS_INVALID(idxMethod))
        {
        CLR_RT_TypeDef_Instance instType;

        _SIDE_ASSERTE(instType.InitializeFromIndex(idxType));

        if (instType.m_assm == this)
          {
          if (!strcmp(GetString(md->name), mil->name))
            {
            idxMethod = idx;
            }
          }
        }
      }

    if (md->flags & CLR_RECORD_METHODDEF::MD_EntryPoint)
      {
      g_CLR_RT_TypeSystem.m_entryPoint = idx;
      }
    }
  }

HRESULT CLR_RT_Assembly::Resolve_AllocateStaticFields(CLR_RT_HeapBlock *pStaticFields)
  {

  HRESULT hr;

  const CLR_RECORD_FIELDDEF *fd = GetFieldDef(0);

  for (int i = 0; i < m_pTablesSize[TBL_FieldDef]; i++, fd++)
    {
    if (fd->flags & CLR_RECORD_FIELDDEF::FD_Static)
      {
      CLR_RT_FieldDef_CrossReference &res = m_pCrossReference_FieldDef[i];

      NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitializeReference(pStaticFields[res.m_offset], fd, this));
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_Assembly::PrepareForExecution()
  {
  HRESULT hr;

  if ((m_flags & CLR_RT_Assembly::PreparingForExecution) != 0)
    {
    // Circular dependency
    _ASSERTE(false);

    NANOCLR_MSG_SET_AND_LEAVE(CLR_E_FAIL, L"Failed to prepare type system for execution\n");
    }

  if ((m_flags & CLR_RT_Assembly::PreparedForExecution) == 0)
    {
    int i;

    m_flags |= CLR_RT_Assembly::PreparingForExecution;

    ITERATE_THROUGH_RECORDS(this, i, AssemblyRef, ASSEMBLYREF)
      {
      _ASSERTE(dst->m_target != NULL);

      if (dst->m_target != NULL)
        {
        NANOCLR_CHECK_HRESULT(dst->m_target->PrepareForExecution());
        }
      }
//
//    if (m_header->patchEntryOffset != 0xFFFFFFFF)
//      {
//      CLR_PMETADATA ptr = GetResourceData(m_header->patchEntryOffset);
//
//#if defined(WIN32)
//      trace_debug("Simulating jump into patch code...\r\n");
//#else
//      ((void (*)())ptr)();
//#endif
//      }
    }

  NANOCLR_CLEANUP();

  // Only try once.  If this fails, then what?
  m_flags |= CLR_RT_Assembly::PreparedForExecution;
  m_flags &= ~CLR_RT_Assembly::PreparingForExecution;

  NANOCLR_CLEANUP_END();
  }

//--//

uint32_t CLR_RT_Assembly::ComputeAssemblyHash()
  {

  return m_header->ComputeAssemblyHash(m_szName, m_header->version);
  }

uint32_t CLR_RT_Assembly::ComputeAssemblyHash(const CLR_RECORD_ASSEMBLYREF *ar)
  {

  return m_header->ComputeAssemblyHash(GetString(ar->name), ar->version);
  }

//--//

bool CLR_RT_Assembly::FindTypeDef(const char *name, const char *nameSpace, uint32_t &idx)
  {

  const CLR_RECORD_TYPEDEF *target = GetTypeDef(0);
  int tblSize = m_pTablesSize[TBL_TypeDef];

  for (int i = 0; i < tblSize; i++, target++)
    {
    if (target->enclosingType == CLR_EmptyIndex)
      {
      const char *szNameSpace = GetString(target->nameSpace);
      const char *szName = GetString(target->name);

      if (!strcmp(szName, name) && !strcmp(szNameSpace, nameSpace))
        {
        idx = create_index(m_idx, i);

        return true;
        }
      }
    }

  idx = 0;

  return false;
  }

bool CLR_RT_Assembly::FindTypeDef(const char *name, uint16_t scope, uint32_t &idx)
  {

  const CLR_RECORD_TYPEDEF *target = GetTypeDef(0);
  int tblSize = m_pTablesSize[TBL_TypeDef];

  for (int i = 0; i < tblSize; i++, target++)
    {
    if (target->enclosingType == scope)
      {
      const char *szName = GetString(target->name);

      if (!strcmp(szName, name))
        {
        idx = create_index(m_idx, i);

        return true;
        }
      }
    }

  idx = 0;

  return false;
  }

bool CLR_RT_Assembly::FindTypeDef(uint32_t hash, uint32_t &idx)
  {

  CLR_RT_TypeDef_CrossReference *p = m_pCrossReference_TypeDef;
  uint32_t tblSize = m_pTablesSize[TBL_TypeDef];
  uint32_t i;

  for (i = 0; i < tblSize; i++, p++)
    {
    if (p->m_hash == hash)
      break;
    }

  if (i != tblSize)
    {
    idx = create_index(m_idx, i);

    return true;
    }
  
  idx = 0;

  return false;
  }

static bool local_FindFieldDef(
  CLR_RT_Assembly *assm,
  uint32_t first,
  uint32_t num,
  const char *szText,
  CLR_RT_Assembly *base,
  uint16_t sig,
  uint32_t &res)
  {

  const CLR_RECORD_FIELDDEF *fd = assm->GetFieldDef(first);

  for (uint32_t i = 0; i < num; i++, fd++)
    {
    const char *fieldName = assm->GetString(fd->name);

    if (!strcmp(fieldName, szText))
      {
      if (base)
        {
        CLR_RT_SignatureParser parserLeft;
        parserLeft.Initialize_FieldDef(assm, fd);
        CLR_RT_SignatureParser parserRight;
        parserRight.Initialize_FieldDef(base, base->GetSignature(sig));

        if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight) == false)
          continue;
        }

      res = create_index(assm->m_idx, first + i);

      return true;
      }
    }

  res = 0;

  return false;
  }

bool CLR_RT_Assembly::FindFieldDef(
  const CLR_RECORD_TYPEDEF *td,
  const char *name,
  CLR_RT_Assembly *base,
  uint16_t sig,
  uint32_t &idx)
  {
  // look class fields
  if (local_FindFieldDef(this, td->iFields_First, td->iFields_Num, name, base, sig, idx))
    return true;
  // look static fields
  if (local_FindFieldDef(this, td->sFields_First, td->sFields_Num, name, base, sig, idx))
    return true;

  idx = 0;

  return false;
  }

bool CLR_RT_Assembly::FindMethodDef(
  const CLR_RECORD_TYPEDEF *td,
  const char *name,
  CLR_RT_Assembly *base,
  uint16_t sig,
  uint32_t &idx)
  {

  int i;
  int num = td->vMethods_Num + td->iMethods_Num + td->sMethods_Num;
  const CLR_RECORD_METHODDEF *md = GetMethodDef(td->methods_First);

  for (i = 0; i < num; i++, md++)
    {
    const char *methodName = GetString(md->name);

    if (!strcmp(methodName, name))
      {
      bool fMatch = true;

      if (CLR_SIG_INVALID != sig)
        {
        CLR_RT_SignatureParser parserLeft;
        parserLeft.Initialize_MethodSignature(this, md);

        CLR_RT_SignatureParser parserRight;
        parserRight.Initialize_MethodSignature(base, base->GetSignature(sig));

        fMatch = CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight);
        }

      if (fMatch)
        {
        idx = create_index(m_idx, i + td->methods_First);

        return true;
        }
      }
    }

  idx = 0;

  return false;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CLR_RT_Assembly::FindMethodBoundaries(uint16_t i, uint16_t &start, uint16_t &end)
  {

  const CLR_RECORD_METHODDEF *p = GetMethodDef(i);

  if (p->RVA == CLR_EmptyIndex)
    return false;

  start = p->RVA;

  while (true)
    {
    p++;
    i++;

    if (i == m_pTablesSize[TBL_MethodDef])
      {
      end = m_pTablesSize[TBL_ByteCode];
      break;
      }

    if (p->RVA != CLR_EmptyIndex)
      {
      end = p->RVA;
      break;
      }
    }

  return true;
  }

bool CLR_RT_Assembly::FindNextStaticConstructor(uint32_t &idx)
  {

  _ASSERTE(m_idx == get_assembly_index(idx));

  for (int i = get_method_index(idx); i < m_pTablesSize[TBL_MethodDef]; i++)
    {
    const CLR_RECORD_METHODDEF *md = GetMethodDef(i);

    idx = create_index(m_idx, i);

    if (md->flags & CLR_RECORD_METHODDEF::MD_StaticConstructor)
      {
      return true;
      }
    }

  idx = 0;
  return false;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_Assembly::Resolve_ComputeHashes()
  {
  HRESULT hr;

  const CLR_RECORD_TYPEDEF *src = GetTypeDef(0);
  CLR_RT_TypeDef_CrossReference *dst = m_pCrossReference_TypeDef;

  for (int i = 0; i < m_pTablesSize[TBL_TypeDef]; i++, src++, dst++)
    {
    uint32_t idx;
    idx = create_index(m_idx, i);
    CLR_RT_TypeDef_Instance inst;
    inst.InitializeFromIndex(idx);
    uint32_t hash = ComputeHashForName(idx, 0);

    while (NANOCLR_INDEX_IS_VALID(inst.m_index))
      {
      const CLR_RECORD_TYPEDEF *target = inst.m_target;
      const CLR_RECORD_FIELDDEF *fd = inst.m_assm->GetFieldDef(target->iFields_First);

      for (int j = 0; j < target->iFields_Num; j++, fd++)
        {
        if ((fd->flags & CLR_RECORD_FIELDDEF::FD_NotSerialized) == 0)
          {
          CLR_RT_SignatureParser parser;
          parser.Initialize_FieldDef(inst.m_assm, fd);
          CLR_RT_SignatureParser::Element res;

          NANOCLR_CHECK_HRESULT(parser.Advance(res));

          while (res.m_levels-- > 0)
            {
            hash = ComputeHashForType(DATATYPE_SZARRAY, hash);
            }

          hash = ComputeHashForType(res.m_dt, hash);

          if ((res.m_dt == DATATYPE_VALUETYPE) || (res.m_dt == DATATYPE_CLASS))
            {
            hash = ComputeHashForName(res.m_cls, hash);
            }

          const char *fieldName = inst.m_assm->GetString(fd->name);

          hash = SUPPORT_ComputeCRC(fieldName, (uint32_t)hal_strlen_s(fieldName), hash);
          }
        }

      inst.SwitchToParent();
      }

    dst->m_hash = hash ? hash : 0xFFFFFFFF; // Don't allow zero as an hash value!!
    }

  NANOCLR_NOCLEANUP();
  }

uint32_t CLR_RT_Assembly::ComputeHashForName(uint32_t td, uint32_t hash)
  {

  char rgBuffer[512];
  char *szBuffer = rgBuffer;
  size_t iBuffer = MAXSTRLEN(rgBuffer);

  g_CLR_RT_TypeSystem.BuildTypeName(td, szBuffer, iBuffer);

  uint32_t hashPost = SUPPORT_ComputeCRC(rgBuffer, (int)(MAXSTRLEN(rgBuffer) - iBuffer), hash);

  return hashPost;
  }

uint32_t CLR_RT_Assembly::ComputeHashForType(CLR_DataType et, uint32_t hash)
  {

  uint8_t val = (uint8_t)CLR_RT_TypeSystem::MapDataTypeToElementType(et);

  uint32_t hashPost = SUPPORT_ComputeCRC(&val, sizeof(val), hash);

  return hashPost;
  }

//--//

CLR_RT_HeapBlock *CLR_RT_Assembly::GetStaticField(const int index)
  {
  return &m_pStaticFields[index];
  }

void CLR_RT_Assembly::Relocate()
  {
  CLR_RT_GarbageCollector::Heap_Relocate(m_pStaticFields, m_iStaticFields);
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_header);
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_szName);
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_pFile);
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_nativeCode);
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

void CLR_RT_TypeSystem::TypeSystem_Initialize()
  {
  NANOCLR_CLEAR(g_CLR_RT_TypeSystem);
  NANOCLR_CLEAR(g_CLR_RT_WellKnownTypes);
  }

void CLR_RT_TypeSystem::TypeSystem_Cleanup()
  {
  NANOCLR_FOREACH_ASSEMBLY(*this)
    {
    pASSM->DestroyInstance();

    *ppASSM = NULL;
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  m_assembliesMax = 0;
  }

void CLR_RT_TypeSystem::Link(CLR_RT_Assembly *assm)
  {

  NANOCLR_FOREACH_ASSEMBLY_NULL(*this)
    {
    *ppASSM = assm;

    assm->m_idx = idx;

    PostLinkageProcessing(assm);

    if (m_assembliesMax < idx)
      m_assembliesMax = idx;

    return;
    }
  NANOCLR_FOREACH_ASSEMBLY_NULL_END();
  }

void CLR_RT_TypeSystem::PostLinkageProcessing(CLR_RT_Assembly *assm)
  {

  if (!strcmp(assm->m_szName, "mscorlib"))
    m_assemblyMscorlib = assm;
  }

CLR_RT_Assembly *CLR_RT_TypeSystem::FindAssembly(const char *szName, const CLR_RECORD_VERSION *ver, bool fExact)
  {
  NANOCLR_FOREACH_ASSEMBLY(*this)
    {
    if (!strcmp(pASSM->m_szName, szName))
      {
      // if there is no version information, anything goes
      if (NULL == ver)
        {
        return pASSM;
        }
      // exact match requested: must take into accoutn all numbers in the version
      else if (fExact)
        {
        if (0 == memcmp(&pASSM->m_header->version, ver, sizeof(*ver)))
          {
          return pASSM;
          }
        }
      // exact match was NOT required but still there version information,
      // we will enforce only the first two number because (by convention)
      // only the minor field is required to be bumped when native assemblies change CRC
      else if (
        ver->iMajorVersion == pASSM->m_header->version.iMajorVersion &&
        ver->iMinorVersion == pASSM->m_header->version.iMinorVersion)
        {
        return pASSM;
        }
      }
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  return NULL;
  }

bool CLR_RT_TypeSystem::FindTypeDef(
  const char *name,
  const char *nameSpace,
  CLR_RT_Assembly *assm,
  uint32_t &res)
  {


  if (assm)
    {
    NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(*this)
      {
      if (pASSM->IsSameAssembly(*assm) && pASSM->FindTypeDef(name, nameSpace, res))
        return true;
      }
    NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END();

    res = 0;

    return false;
    }

  return FindTypeDef(name, nameSpace, res);
  }

bool CLR_RT_TypeSystem::FindTypeDef(const char *name, const char *nameSpace, uint32_t &res)
  {

  NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(*this)
    {
    if (pASSM->FindTypeDef(name, nameSpace, res))
      return true;
    }
  NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END();

  res = 0;
  return false;
  }

bool CLR_RT_TypeSystem::FindTypeDef(uint32_t hash, uint32_t &res)
  {

  NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN(*this)
    {
    if (pASSM->FindTypeDef(hash, res))
      return true;
    }
  NANOCLR_FOREACH_ASSEMBLY_IN_CURRENT_APPDOMAIN_END();

  res = 0;
  return false;
  }

bool CLR_RT_TypeSystem::FindTypeDef(const char *szClass, CLR_RT_Assembly *assm, uint32_t &res)
  {
  char rgName[MAXTYPENAMELEN];
  char rgNamespace[MAXTYPENAMELEN];

  if (hal_strlen_s(szClass) < ARRAYSIZE(rgNamespace))
    {
    const char *szPtr = szClass;
    const char *szPtr_LastDot = NULL;
    const char *szPtr_FirstSubType = NULL;
    char c;
    size_t len;

    while (true)
      {
      c = szPtr[0];
      if (!c)
        break;

      if (c == '.')
        {
        szPtr_LastDot = szPtr;
        }
      else if (c == '+')
        {
        szPtr_FirstSubType = szPtr;
        break;
        }

      szPtr++;
      }

    if (szPtr_LastDot)
      {
      len = szPtr_LastDot++ - szClass;
      hal_strncpy_s(rgNamespace, ARRAYSIZE(rgNamespace), szClass, len);
      len = szPtr - szPtr_LastDot;
      hal_strncpy_s(rgName, ARRAYSIZE(rgName), szPtr_LastDot, len);
      }
    else
      {
      rgNamespace[0] = 0;
      hal_strcpy_s(rgName, ARRAYSIZE(rgName), szClass);
      }

    if (FindTypeDef(rgName, rgNamespace, assm, res))
      {
      //
      // Found the containing type, let's look for the nested type.
      //
      if (szPtr_FirstSubType)
        {
        CLR_RT_TypeDef_Instance inst;

        do
          {
          szPtr = ++szPtr_FirstSubType;

          while (true)
            {
            c = szPtr_FirstSubType[0];
            if (!c)
              break;

            if (c == '+')
              break;

            szPtr_FirstSubType++;
            }

          len = szPtr_FirstSubType - szPtr;
          hal_strncpy_s(rgName, ARRAYSIZE(rgName), szPtr, len);

          inst.InitializeFromIndex(res);

          if (inst.m_assm->FindTypeDef(rgName, get_type_index(res), res) == false)
            {
            return false;
            }

          } while (c == '+');
        }

      return true;
      }
    }

  res = 0;

  return false;
  }

bool CLR_RT_TypeSystem::FindTypeDef(const char *szClass, CLR_RT_Assembly *assm, CLR_RT_ReflectionDef_Index &reflex)
  {
  return false;
  }

int CompareResource(const void *p1, const void *p2)
  {

  const CLR_RECORD_RESOURCE *resource1 = (const CLR_RECORD_RESOURCE *)p1;
  const CLR_RECORD_RESOURCE *resource2 = (const CLR_RECORD_RESOURCE *)p2;

  return (int)resource1->id - (int)resource2->id;
  }

HRESULT CLR_RT_TypeSystem::LocateResourceFile(
  CLR_RT_Assembly_Instance assm,
  const char *name,
  int32_t &idxResourceFile)
  {

  HRESULT hr;

  CLR_RT_Assembly *pAssm = assm.m_assm;

  for (idxResourceFile = 0; idxResourceFile < pAssm->m_pTablesSize[TBL_ResourcesFiles]; idxResourceFile++)
    {
    const CLR_RECORD_RESOURCE_FILE *resourceFile = pAssm->GetResourceFile(idxResourceFile);

    if (!strcmp(pAssm->GetString(resourceFile->name), name))
      {
      NANOCLR_SET_AND_LEAVE(S_OK);
      }
    }

  idxResourceFile = -1;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::LocateResource(
  CLR_RT_Assembly_Instance assm,
  int32_t idxResourceFile,
  int16_t id,
  const CLR_RECORD_RESOURCE *&res,
  uint32_t &size)
  {

  HRESULT hr;

  CLR_RT_Assembly *pAssm = assm.m_assm;
  const CLR_RECORD_RESOURCE_FILE *resourceFile;
  CLR_RECORD_RESOURCE resourceT;
  const CLR_RECORD_RESOURCE *resNext;
  const CLR_RECORD_RESOURCE *resZero;

  res = NULL;
  size = 0;

  if (idxResourceFile < 0 || idxResourceFile >= pAssm->m_pTablesSize[TBL_ResourcesFiles])
    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

  resourceFile = pAssm->GetResourceFile(idxResourceFile);

  _ASSERTE(resourceFile->numberOfResources > 0);

  resZero = pAssm->GetResource(resourceFile->offset);

  resourceT.id = id;

  res = (const CLR_RECORD_RESOURCE *)
    bsearch(&resourceT, resZero, resourceFile->numberOfResources, sizeof(CLR_RECORD_RESOURCE), CompareResource);

  if (res != NULL)
    {
    // compute size here...
    // assert not the last resource
    _ASSERTE(res + 1 <= pAssm->GetResource(pAssm->m_pTablesSize[TBL_Resources] - 1));
    resNext = res + 1;

    size = resNext->offset - res->offset;

    // deal with alignment.
    size -= (resNext->flags & CLR_RECORD_RESOURCE::FLAGS_PaddingMask);
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::ResolveAll()
  {

  HRESULT hr;

  bool fOutput = false;

  while (true)
    {
    bool fGot = false;
    bool fNeedResolution = false;

    NANOCLR_FOREACH_ASSEMBLY(*this)
      {
      if ((pASSM->m_flags & CLR_RT_Assembly::Resolved) == 0)
        {
        fNeedResolution = true;

        if (pASSM->Resolve_AssemblyRef(fOutput))
          {
          fGot = true;

          pASSM->m_flags |= CLR_RT_Assembly::Resolved;

          NANOCLR_CHECK_HRESULT(pASSM->Resolve_TypeRef());
          NANOCLR_CHECK_HRESULT(pASSM->Resolve_FieldRef());
          NANOCLR_CHECK_HRESULT(pASSM->Resolve_MethodRef());
          /********************/ pASSM->Resolve_TypeDef();
          /********************/ pASSM->Resolve_MethodDef();
          /********************/ pASSM->Resolve_Link();
          NANOCLR_CHECK_HRESULT(pASSM->Resolve_ComputeHashes());

          NANOCLR_CHECK_HRESULT(pASSM->Resolve_AllocateStaticFields(pASSM->m_pStaticFields));

          pASSM->m_flags |= CLR_RT_Assembly::ResolutionCompleted;
          }
        }
      }
    NANOCLR_FOREACH_ASSEMBLY_END();

    if (fOutput == true)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_TYPE_UNAVAILABLE);
      }

    if (fGot == false)
      {
      if (fNeedResolution)
        {
#if !defined(BUILD_RTM)
        trace_debug("Link failure: some assembly references cannot be resolved!!\r\n\r\n");
#endif

        fOutput = true;
        }
      else
        {
        break;
        }
      }
    }

#if !defined(BUILD_RTM)

  if (s_CLR_RT_fTrace_AssemblyOverhead >= c_CLR_RT_Trace_Info)
    {
        {
        int pTablesSize[TBL_Max];
        memset(pTablesSize, 0, sizeof(pTablesSize));
        CLR_RT_Assembly::Offsets offsets;
        memset(&offsets, 0, sizeof(offsets));

        size_t iStaticFields = 0;
        size_t iTotalRamSize = 0;
        size_t iTotalRomSize = 0;
        size_t iMetaData = 0;

        NANOCLR_FOREACH_ASSEMBLY(*this)
          {
          offsets.iBase += ROUNDTOMULTIPLE(sizeof(CLR_RT_Assembly), uint32_t);
          offsets.iAssemblyRef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_AssemblyRef] * sizeof(CLR_RT_AssemblyRef_CrossReference),
            uint32_t);
          offsets.iTypeRef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_TypeRef] * sizeof(CLR_RT_TypeRef_CrossReference),
            uint32_t);
          offsets.iFieldRef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_FieldRef] * sizeof(CLR_RT_FieldRef_CrossReference),
            uint32_t);
          offsets.iMethodRef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_MethodRef] * sizeof(CLR_RT_MethodRef_CrossReference),
            uint32_t);
          offsets.iTypeDef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_TypeDef] * sizeof(CLR_RT_TypeDef_CrossReference),
            uint32_t);
          offsets.iFieldDef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_FieldDef] * sizeof(CLR_RT_FieldDef_CrossReference),
            uint32_t);
          offsets.iMethodDef += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_CrossReference),
            uint32_t);

#if !defined(NANOCLR_APPDOMAINS)
          offsets.iStaticFields += ROUNDTOMULTIPLE(pASSM->m_iStaticFields * sizeof(CLR_RT_HeapBlock), uint32_t);
#endif

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
          offsets.iDebuggingInfoMethods += ROUNDTOMULTIPLE(
            pASSM->m_pTablesSize[TBL_MethodDef] * sizeof(CLR_RT_MethodDef_DebuggingInfo),
            uint32_t);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

          iMetaData += pASSM->m_header->SizeOfTable(TBL_AssemblyRef) + pASSM->m_header->SizeOfTable(TBL_TypeRef) +
            pASSM->m_header->SizeOfTable(TBL_FieldRef) + pASSM->m_header->SizeOfTable(TBL_MethodRef) +
            pASSM->m_header->SizeOfTable(TBL_TypeDef) + pASSM->m_header->SizeOfTable(TBL_FieldDef) +
            pASSM->m_header->SizeOfTable(TBL_MethodDef) +
            pASSM->m_header->SizeOfTable(TBL_Attributes) + pASSM->m_header->SizeOfTable(TBL_TypeSpec) +
            pASSM->m_header->SizeOfTable(TBL_Signatures);

          for (int tbl = 0; tbl < TBL_Max; tbl++)
            {
            pTablesSize[tbl] += pASSM->m_pTablesSize[tbl];
            }

          iTotalRomSize += pASSM->m_header->TotalSize();

          iStaticFields += pASSM->m_iStaticFields;
          }
        NANOCLR_FOREACH_ASSEMBLY_END();

        iTotalRamSize = offsets.iBase + offsets.iAssemblyRef + offsets.iTypeRef + offsets.iFieldRef +
          offsets.iMethodRef + offsets.iTypeDef + offsets.iFieldDef + offsets.iMethodDef;

#if !defined(NANOCLR_APPDOMAINS)
        iTotalRamSize += offsets.iStaticFields;
#endif

        trace_debug(
          "\r\nTotal: (%d RAM - %d ROM - %d METADATA)\r\n\r\n",
          iTotalRamSize,
          iTotalRomSize,
          iMetaData);

        trace_debug(
          "   AssemblyRef    = %8d bytes (%8d elements)\r\n",
          offsets.iAssemblyRef,
          pTablesSize[TBL_AssemblyRef]);
        trace_debug(
          "   TypeRef        = %8d bytes (%8d elements)\r\n",
          offsets.iTypeRef,
          pTablesSize[TBL_TypeRef]);
        trace_debug(
          "   FieldRef       = %8d bytes (%8d elements)\r\n",
          offsets.iFieldRef,
          pTablesSize[TBL_FieldRef]);
        trace_debug(
          "   MethodRef      = %8d bytes (%8d elements)\r\n",
          offsets.iMethodRef,
          pTablesSize[TBL_MethodRef]);
        trace_debug(
          "   TypeDef        = %8d bytes (%8d elements)\r\n",
          offsets.iTypeDef,
          pTablesSize[TBL_TypeDef]);
        trace_debug(
          "   FieldDef       = %8d bytes (%8d elements)\r\n",
          offsets.iFieldDef,
          pTablesSize[TBL_FieldDef]);
        trace_debug(
          "   MethodDef      = %8d bytes (%8d elements)\r\n",
          offsets.iMethodDef,
          pTablesSize[TBL_MethodDef]);

#if !defined(NANOCLR_APPDOMAINS)
        trace_debug("   StaticFields   = %8d bytes (%8d elements)\r\n", offsets.iStaticFields, iStaticFields);
#endif

        trace_debug("\r\n");

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        trace_debug("   DebuggingInfo  = %8d bytes\r\n", offsets.iDebuggingInfoMethods);
        trace_debug("\r\n");
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

        trace_debug(
          "   Attributes      = %8d bytes (%8d elements)\r\n",
          pTablesSize[TBL_Attributes] * sizeof(CLR_RECORD_ATTRIBUTE),
          pTablesSize[TBL_Attributes]);
        trace_debug(
          "   TypeSpec        = %8d bytes (%8d elements)\r\n",
          pTablesSize[TBL_TypeSpec] * sizeof(CLR_RECORD_TYPESPEC),
          pTablesSize[TBL_TypeSpec]);
        trace_debug(
          "   Resources Files = %8d bytes (%8d elements)\r\n",
          pTablesSize[TBL_ResourcesFiles] * sizeof(CLR_RECORD_RESOURCE_FILE),
          pTablesSize[TBL_ResourcesFiles]);
        trace_debug(
          "   Resources       = %8d bytes (%8d elements)\r\n",
          pTablesSize[TBL_Resources] * sizeof(CLR_RECORD_RESOURCE),
          pTablesSize[TBL_Resources]);
        trace_debug("   Resources Data  = %8d bytes\r\n", pTablesSize[TBL_ResourcesData]);
        trace_debug("   Strings         = %8d bytes\r\n", pTablesSize[TBL_Strings]);
        trace_debug("   Signatures      = %8d bytes\r\n", pTablesSize[TBL_Signatures]);
        trace_debug("   ByteCode        = %8d bytes\r\n", pTablesSize[TBL_ByteCode]);
        trace_debug("\r\n\r\n");
        }
    }

#endif

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::PrepareForExecutionHelper(const char *szAssembly)
  {

  HRESULT hr;

  NANOCLR_FOREACH_ASSEMBLY(*this)
    {
    if (!strcmp(szAssembly, pASSM->m_szName))
      {
      NANOCLR_CHECK_HRESULT(pASSM->PrepareForExecution());
      }
    }

  NANOCLR_FOREACH_ASSEMBLY_END();

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::PrepareForExecution()
  {
  HRESULT hr;

  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions |= c_fDebugger_BreakpointsDisabled;

  if (g_CLR_RT_ExecutionEngine.m_outOfMemoryException == NULL)
    {
    CLR_RT_HeapBlock exception;

    NANOCLR_CHECK_HRESULT(
      g_CLR_RT_ExecutionEngine.NewObjectFromIndex(exception, g_CLR_RT_WellKnownTypes.m_OutOfMemoryException));

    g_CLR_RT_ExecutionEngine.m_outOfMemoryException = exception.Dereference();
    }

  NANOCLR_FOREACH_ASSEMBLY(*this)
    {
    NANOCLR_CHECK_HRESULT(pASSM->PrepareForExecution());
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  NANOCLR_CLEANUP();

  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions |= c_fDebugger_BreakpointsDisabled;

  g_CLR_RT_ExecutionEngine.Breakpoint_Assemblies_Loaded();
  NANOCLR_CLEANUP_END();
  }

bool CLR_RT_TypeSystem::MatchSignature(CLR_RT_SignatureParser &parserLeft, CLR_RT_SignatureParser &parserRight)
  {

  if (parserLeft.m_type != parserRight.m_type)
    return false;
  if (parserLeft.m_flags != parserRight.m_flags)
    return false;

  return MatchSignatureDirect(parserLeft, parserRight, false);
  }

bool CLR_RT_TypeSystem::MatchSignatureDirect(
  CLR_RT_SignatureParser &parserLeft,
  CLR_RT_SignatureParser &parserRight,
  bool fIsInstanceOfOK)
  {

  while (true)
    {
    int iAvailLeft = parserLeft.Available();
    int iAvailRight = parserRight.Available();

    if (iAvailLeft != iAvailRight)
      return false;

    if (!iAvailLeft)
      return true;

    CLR_RT_SignatureParser::Element resLeft;
    if (FAILED(parserLeft.Advance(resLeft)))
      return false;
    CLR_RT_SignatureParser::Element resRight;
    if (FAILED(parserRight.Advance(resRight)))
      return false;

    if (!MatchSignatureElement(resLeft, resRight, fIsInstanceOfOK))
      return false;
    }

  return true;
  }

bool CLR_RT_TypeSystem::MatchSignatureElement(
  CLR_RT_SignatureParser::Element &resLeft,
  CLR_RT_SignatureParser::Element &resRight,
  bool fIsInstanceOfOK)
  {

  if (fIsInstanceOfOK)
    {
    CLR_RT_ReflectionDef_Index idxLeft;
    CLR_RT_TypeDescriptor descLeft;
    CLR_RT_ReflectionDef_Index idxRight;
    CLR_RT_TypeDescriptor descRight;

    idxLeft.m_kind = REFLECTION_TYPE;
    idxLeft.m_levels = resLeft.m_levels;
    idxLeft.m_data.m_type = resLeft.m_cls;

    idxRight.m_kind = REFLECTION_TYPE;
    idxRight.m_levels = resRight.m_levels;
    idxRight.m_data.m_type = resRight.m_cls;

    if (FAILED(descLeft.InitializeFromReflection(idxLeft)))
      return false;
    if (FAILED(descRight.InitializeFromReflection(idxRight)))
      return false;

    if (!CLR_RT_ExecutionEngine::IsInstanceOf(descRight, descLeft, false))
      return false;
    }
  else
    {
    if (resLeft.m_fByRef != resRight.m_fByRef)
      return false;
    if (resLeft.m_levels != resRight.m_levels)
      return false;
    if (resLeft.m_dt != resRight.m_dt)
      return false;
    if (resLeft.m_cls != resRight.m_cls)
      return false;
    }

  return true;
  }

//--//

HRESULT CLR_RT_TypeSystem::QueueStringToBuffer(char *szBuffer, size_t iBuffer, const char *szText)
  {

  HRESULT hr;

  if (szText)
    {
    if (CLR_SafeSprintf(szBuffer, iBuffer, "%s", szText) == false)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
      }
    }
  else
    {
    szBuffer[0] = 0;
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::BuildTypeName(uint32_t cls, char *szBuffer, size_t iBuffer)
  {

  return BuildTypeName(cls, szBuffer, iBuffer, CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL, 0);
  }

HRESULT CLR_RT_TypeSystem::BuildTypeName(
  uint32_t cls,
  char *szBuffer,
  size_t iBuffer,
  uint32_t flags,
  uint32_t levels)
  {

  HRESULT hr;

  CLR_RT_TypeDef_Instance inst;
  CLR_RT_Assembly *assm;
  const CLR_RECORD_TYPEDEF *td;
  bool fFullName;

  if (inst.InitializeFromIndex(cls) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  assm = inst.m_assm;
  td = inst.m_target;
  fFullName = flags & CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL;

  if (fFullName && td->enclosingType != CLR_EmptyIndex)
    {
    uint32_t clsSub = create_index(get_assembly_index(inst.m_index), td->enclosingType);

    NANOCLR_CHECK_HRESULT(BuildTypeName(clsSub, szBuffer, iBuffer, flags, 0));


    NANOCLR_CHECK_HRESULT(QueueStringToBuffer(
      szBuffer,
      iBuffer,
      (flags & CLR_RT_TypeSystem::TYPENAME_NESTED_SEPARATOR_DOT) ? "." : "+"));
    }

  if (fFullName && td->nameSpace != CLR_EmptyIndex)
    {
    const char *szNameSpace = assm->GetString(td->nameSpace);

    if (szNameSpace[0])
      {
      NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, szNameSpace));
      NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "."));
      }
    }

  NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, assm->GetString(td->name)));

  while (levels-- > 0)
    {
    NANOCLR_CHECK_HRESULT(QueueStringToBuffer(szBuffer, iBuffer, "[]"));
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::BuildMethodName(uint32_t md, char *szBuffer, size_t iBuffer)
  {

  HRESULT hr;

  CLR_RT_MethodDef_Instance inst;
  CLR_RT_TypeDef_Instance instOwner;

  if (inst.InitializeFromIndex(md) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
  if (instOwner.InitializeFromMethod(inst) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner.m_index, szBuffer, iBuffer));

  CLR_SafeSprintf(szBuffer, iBuffer, "::%s", inst.m_assm->GetString(inst.m_target->name));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_TypeSystem::BuildFieldName(uint32_t fd, char *szBuffer, size_t iBuffer)
  {

  HRESULT hr;

  CLR_RT_FieldDef_Instance inst;
  CLR_RT_TypeDef_Instance instOwner;

  if (inst.InitializeFromIndex(fd) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
  if (instOwner.InitializeFromField(inst) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  NANOCLR_CHECK_HRESULT(BuildTypeName(instOwner.m_index, szBuffer, iBuffer));

  CLR_SafeSprintf(szBuffer, iBuffer, "::%s", inst.m_assm->GetString(inst.m_target->name));

  NANOCLR_NOCLEANUP();
  }

//--//

bool CLR_RT_TypeSystem::FindVirtualMethodDef(
  uint32_t cls,
  uint32_t calleeMD,
  uint32_t &idx)
  {

  CLR_RT_MethodDef_Instance calleeInst;

  if (calleeInst.InitializeFromIndex(calleeMD))
    {
    const char *calleeName = calleeInst.m_assm->GetString(calleeInst.m_target->name);

    CLR_RT_TypeDef_Instance inst;
    inst.InitializeFromMethod(calleeInst);

    if ((inst.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) ==
      CLR_RECORD_TYPEDEF::TD_Semantics_Interface)
      {
      //
      // It's an interface method, it could be that the class is implementing explicitly the method.
      // Prepend the Interface name to the method name and try again.
      //
      char rgBuffer[512];
      char *szBuffer = rgBuffer;
      size_t iBuffer = MAXSTRLEN(rgBuffer);

      BuildTypeName(
        inst.m_index,
        szBuffer,
        iBuffer,
        CLR_RT_TypeSystem::TYPENAME_FLAGS_FULL | CLR_RT_TypeSystem::TYPENAME_NESTED_SEPARATOR_DOT,
        0);
      QueueStringToBuffer(szBuffer, iBuffer, ".");
      QueueStringToBuffer(szBuffer, iBuffer, calleeName);

      if (FindVirtualMethodDef(cls, calleeMD, rgBuffer, idx))
        return true;
      }

    if (FindVirtualMethodDef(cls, calleeMD, calleeName, idx))
      return true;
    }

  idx = 0;

  return false;
  }

bool CLR_RT_TypeSystem::FindVirtualMethodDef(
  uint32_t cls,
  uint32_t calleeMD,
  const char *calleeName,
  uint32_t &idx)
  {

  CLR_RT_TypeDef_Instance clsInst;
  clsInst.InitializeFromIndex(cls);
  CLR_RT_MethodDef_Instance calleeInst;
  calleeInst.InitializeFromIndex(calleeMD);

  CLR_RT_Assembly *calleeAssm = calleeInst.m_assm;
  const CLR_RECORD_METHODDEF *calleeMDR = calleeInst.m_target;
  uint8_t calleeNumArgs = calleeMDR->numArgs;

  while (NANOCLR_INDEX_IS_VALID(clsInst.m_index))
    {
    CLR_RT_Assembly *targetAssm = clsInst.m_assm;
    const CLR_RECORD_TYPEDEF *targetTDR = clsInst.m_target;
    const CLR_RECORD_METHODDEF *targetMDR = targetAssm->GetMethodDef(targetTDR->methods_First);
    int num = targetTDR->vMethods_Num + targetTDR->iMethods_Num;

    for (int i = 0; i < num; i++, targetMDR++)
      {
      if (targetMDR == calleeMDR)
        {
        // Shortcut for identity.
        idx = calleeInst.m_index;
        return true;
        }

      if (targetMDR->numArgs == calleeNumArgs && (targetMDR->flags & CLR_RECORD_METHODDEF::MD_Abstract) == 0)
        {
        const char *targetName = targetAssm->GetString(targetMDR->name);

        if (!strcmp(targetName, calleeName))
          {
          CLR_RT_SignatureParser parserLeft;
          parserLeft.Initialize_MethodSignature(calleeAssm, calleeMDR);
          CLR_RT_SignatureParser parserRight;
          parserRight.Initialize_MethodSignature(targetAssm, targetMDR);

          if (CLR_RT_TypeSystem::MatchSignature(parserLeft, parserRight))
            {
            idx = create_index(targetAssm->m_idx, i + targetTDR->methods_First);

            return true;
            }
          }
        }
      }

    clsInst.SwitchToParent();
    }

  idx = 0;

  return false;
  }

CLR_DataType CLR_RT_TypeSystem::MapElementTypeToDataType(uint32_t et)
  {

  const CLR_RT_DataTypeLookup *ptr = c_CLR_RT_DataTypeLookup;

  for (uint32_t num = 0; num < DATATYPE_FIRST_INVALID; num++, ptr++)
    {
    if (ptr->m_convertToElementType == et)
      return (CLR_DataType)num;
    }

  if (et == ELEMENT_TYPE_I)
    return DATATYPE_I4;
  if (et == ELEMENT_TYPE_U)
    return DATATYPE_U4;

  return DATATYPE_FIRST_INVALID;
  }

uint32_t CLR_RT_TypeSystem::MapDataTypeToElementType(CLR_DataType dt)
  {

  return c_CLR_RT_DataTypeLookup[dt].m_convertToElementType;
  }

//--//

void CLR_RT_AttributeEnumerator::Initialize(CLR_RT_Assembly *assm)
  {
  m_assm = assm;   // CLR_RT_Assembly*            m_assm;
  m_ptr = NULL;    // const CLR_RECORD_ATTRIBUTE* m_ptr;
  m_num = 0;       // int                         m_num;
                   // CLR_RECORD_ATTRIBUTE        m_td;
  m_match = 0;; // uint32_t      m_match;
  }

void CLR_RT_AttributeEnumerator::Initialize(const CLR_RT_TypeDef_Instance &inst)
  {
  m_data.ownerType = TBL_TypeDef;
  m_data.ownerIdx = get_type_index(inst.m_index);

  Initialize(inst.m_assm);
  }

void CLR_RT_AttributeEnumerator::Initialize(const CLR_RT_FieldDef_Instance &inst)
  {
  m_data.ownerType = TBL_FieldDef;
  m_data.ownerIdx = get_field_index(inst.m_index);

  Initialize(inst.m_assm);
  }

void CLR_RT_AttributeEnumerator::Initialize(const CLR_RT_MethodDef_Instance &inst)
  {
  m_data.ownerType = TBL_MethodDef;
  m_data.ownerIdx = get_method_index(inst.m_index);

  Initialize(inst.m_assm);
  }

bool CLR_RT_AttributeEnumerator::Advance()
  {

  const CLR_RECORD_ATTRIBUTE *ptr = m_ptr;
  int num = m_num;
  uint32_t key = m_data.Key();
  bool fRes = false;

  if (ptr == NULL)
    {
    ptr = m_assm->GetAttribute(0) - 1;
    num = m_assm->m_pTablesSize[TBL_Attributes];
    }

  while (num)
    {
    ptr++;
    num--;

    if (ptr->Key() == key)
      {
      uint16_t tk = ptr->constructor;
      if (tk & 0x8000)
        {
        m_match = m_assm->m_pCrossReference_MethodRef[tk & 0x7FFF].m_target;
        }
      else
        {
        m_match = create_index(m_assm->m_idx, tk);
        }

      m_blob = m_assm->GetSignature(ptr->data);

      fRes = true;
      break;
      }
    }

  m_ptr = ptr;
  m_num = num;

  return fRes;
  }

void CLR_RT_AttributeEnumerator::GetCurrent(CLR_RT_TypeDef_Instance *instTD)
  {
  CLR_RT_MethodDef_Instance md;

  md.InitializeFromIndex(m_match);
  instTD->InitializeFromMethod(md);
  }

bool CLR_RT_AttributeEnumerator::MatchNext(
  const CLR_RT_TypeDef_Instance *instTD,
  const CLR_RT_MethodDef_Instance *instMD)
  {

  while (Advance())
    {
    if (instMD)
      {
      if (m_match != instMD->m_index)
        continue;
      }

    if (instTD)
      {
      CLR_RT_MethodDef_Instance md;
      CLR_RT_TypeDef_Instance td;

      md.InitializeFromIndex(m_match);
      td.InitializeFromMethod(md);

      if (td.m_index != instTD->m_index)
        continue;
      }

    return true;
    }

  return false;
  }

//--//

HRESULT CLR_RT_AttributeParser::Initialize(const CLR_RT_AttributeEnumerator &en)
  {

  HRESULT hr;

  if (m_methodDefInstance.InitializeFromIndex(en.m_match) == false || m_typeDefInstance.InitializeFromMethod(m_methodDefInstance) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  m_parser.Initialize_MethodSignature(m_methodDefInstance.m_assm, m_methodDefInstance.m_target);
  m_parser.Advance(m_res); // Skip return value.

  m_assm = en.m_assm;
  m_blob = en.m_blob;

  m_currentPos = 0;
  m_fixed_Count = m_methodDefInstance.m_target->numArgs - 1;
  m_named_Count = -1;
  m_constructorParsed = false;
  m_methodIndex = en.m_match;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_AttributeParser::Next(Value *&res)
  {

  HRESULT hr;

  if (m_currentPos == m_fixed_Count)
    {
    NANOCLR_READ_UNALIGNED_UINT16(m_named_Count, m_blob);
    }

  if (m_fixed_Count == 0 && m_named_Count == 0 && !m_constructorParsed)
    {
    // Attribute class has no fields, no properties and only default constructor

    m_lastValue.m_mode = Value::c_DefaultConstructor;
    m_lastValue.m_name = NULL;

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(m_lastValue.m_value, m_typeDefInstance));

    res = &m_lastValue;

    m_constructorParsed = true;

    NANOCLR_SET_AND_LEAVE(S_OK);
    }
  else if ((m_currentPos < m_fixed_Count) && !m_constructorParsed)
    {
    // Attribute class has a constructor

    m_lastValue.m_mode = Value::c_ConstructorArgument;
    m_lastValue.m_name = NULL;

    ////////////////////////////////////////////////
    // need to read the arguments from the blob

    NANOCLR_CHECK_HRESULT(m_parser.Advance(m_res));
    //
    // Skip value info.
    //
    m_blob += sizeof(uint8_t);

    const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[m_res.m_dt];

    if (dtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric)
      {
      // size of value
      uint32_t size = dtl.m_sizeInBytes;

      NANOCLR_CHECK_HRESULT(
        g_CLR_RT_ExecutionEngine.NewObjectFromIndex(m_lastValue.m_value, g_CLR_RT_WellKnownTypes.m_TypeStatic));

      // need to setup reflection and data type Id to properly setup the object
      m_lastValue.m_value.SetReflection(*dtl.m_cls);

      m_lastValue.m_value.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(m_res.m_dt, 0, 1));

      // because this is a numeric object, performa a raw copy of the numeric value data from the blob to the
      // return value
      memcpy((uint8_t *)&m_lastValue.m_value.NumericByRef(), m_blob, size);
      m_blob += size;
      }
    else if (m_res.m_dt == DATATYPE_STRING)
      {
      uint32_t tk;
      NANOCLR_READ_UNALIGNED_UINT16(tk, m_blob);

      CLR_RT_HeapBlock_String::CreateInstance(m_lastValue.m_value, "System.String");
      }
    else
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }

    res = &m_lastValue;

    m_constructorParsed = true;

    NANOCLR_SET_AND_LEAVE(S_OK);
    }
  else if (m_currentPos < m_fixed_Count + m_named_Count && !m_constructorParsed)
    {
    // Attribute class has named fields

    uint32_t kind;
    NANOCLR_READ_UNALIGNED_UINT8(kind, m_blob);

    m_lastValue.m_name = GetString();

    if (kind == SERIALIZATION_TYPE_FIELD)
      {
      uint32_t fd;
      CLR_RT_FieldDef_Instance inst;

      m_lastValue.m_mode = Value::c_NamedField;

      NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.FindFieldDef(m_typeDefInstance, m_lastValue.m_name, fd));

      inst.InitializeFromIndex(fd);

      m_parser.Initialize_FieldDef(inst.m_assm, inst.m_target);
      }
    else
      {
      m_lastValue.m_mode = Value::c_NamedProperty;

      //
      // it's supposed to reach here when there is an attribute contructor
      // but that is already handled upwards
      // leaving this here waiting for a special case that hits here (if there is one...)
      //
      NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
      }
    }
  else
    {
    res = NULL;
    NANOCLR_SET_AND_LEAVE(S_OK);
    }

  NANOCLR_CHECK_HRESULT(m_parser.Advance(m_res));

  res = &m_lastValue;

  //
  // Check for Enums.
  //
  if (m_res.m_dt == DATATYPE_VALUETYPE)
    {
    CLR_RT_TypeDef_Instance td;
    td.InitializeFromIndex(m_res.m_cls);

    if ((td.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) == CLR_RECORD_TYPEDEF::TD_Semantics_Enum)
      {
      m_res.m_dt = (CLR_DataType)td.m_target->dataType;
      }
    }

  //
  // Skip value info.
  //
  m_blob += sizeof(uint8_t);

  {
  const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[m_res.m_dt];

  if (dtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric)
    {
    // need to setup reflection and data type Id to properly setup the object
    m_lastValue.m_value.SetReflection(m_res.m_cls);

    m_lastValue.m_value.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(m_res.m_dt, 0, 1));

    uint32_t size = dtl.m_sizeInBytes;

    memcpy(&m_lastValue.m_value.NumericByRef(), m_blob, size);
    m_blob += size;
    }
  else if (m_res.m_dt == DATATYPE_STRING)
    {
    uint32_t tk;
    NANOCLR_READ_UNALIGNED_UINT16(tk, m_blob);

    CLR_RT_HeapBlock_String::CreateInstance(m_lastValue.m_value, "System.String");
    }
  else
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
  }

  m_lastValue.m_pos = m_currentPos++;

  NANOCLR_NOCLEANUP();
  }

const char *CLR_RT_AttributeParser::GetString()
  {

  uint32_t tk;
  NANOCLR_READ_UNALIGNED_UINT16(tk, m_blob);

  return m_assm->GetString(tk);
  }
