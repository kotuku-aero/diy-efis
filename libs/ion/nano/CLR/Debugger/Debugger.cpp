//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "../Include/nanoCLR_Runtime.h"
//#include "../../HAL/Include/nanoHAL.h"
//#include "../CorLib/mscorlib/corlib_native.h"
//#include "../Core/Core.h"

#include "../../../ion_debugger.h"

#define __min(a, b) (((a) < (b)) ? (a) : (b))

//--//

static CLR_RT_Thread *get_thread_from_pid(int32_t pid)
  {
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
    if (th->m_pid == pid)
      return th;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
    if (th->m_pid == pid)
      return th;
    }
  NANOCLR_FOREACH_NODE_END();

  return NULL;
  }

result_t debug_create_list_of_threads(debugging_thread_list_t **reply)
  {
  uint32_t *pidDst;
  int num;

  num = g_CLR_RT_ExecutionEngine.m_threadsReady.NumOfNodes() + g_CLR_RT_ExecutionEngine.m_threadsWaiting.NumOfNodes();

  uint32_t totLen = sizeof(debugging_thread_list_t) + (num) * sizeof(uint32_t);
  *reply = (debugging_thread_list_t *)platform_malloc(totLen);
  
  (*reply)->m_num = num;

  pidDst = (*reply)->m_pids;

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, thSrc, g_CLR_RT_ExecutionEngine.m_threadsReady)
    {
    *pidDst++ = thSrc->m_pid;
    }
  NANOCLR_FOREACH_NODE_END();

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, thSrc, g_CLR_RT_ExecutionEngine.m_threadsWaiting)
    {
    *pidDst++ = thSrc->m_pid;
    }
  NANOCLR_FOREACH_NODE_END();

  return s_ok;
  }

result_t debug_create_list_of_calls(uint32_t pid, thread_stack_t **cmdReply)
  {
  CLR_RT_Thread *th = get_thread_from_pid(pid);
  if (th == 0)
    return e_bad_parameter;

  for (int pass = 0; pass < 2; pass++)
    {
    int num = 0;

    NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
      {
      if (pass == 1)
        {
        int tmp = num;

        if (call->m_inlineFrame)
          {
          call_t &dst = (*cmdReply)->m_data[tmp++];

          dst.m_index = call->m_inlineFrame->m_frame.m_call.m_index;
          dst.m_IP = (uint32_t)(call->m_inlineFrame->m_frame.m_IP - call->m_inlineFrame->m_frame.m_IPStart);
          }

        call_t &dst = (*cmdReply)->m_data[tmp];

        dst.m_index = call->m_call.m_index;
        dst.m_IP = (uint32_t)(call->m_IP - call->m_IPstart);

        if (dst.m_IP && call != th->CurrentFrame())
          {
          // With the exception of when the IP is 0, for a breakpoint on Push,
          // The call->m_IP is the next instruction to execute, not the currently executing one.
          // For non-leaf frames, this will return the IP within the call.
          dst.m_IP--;
          }
        }

      if (call->m_inlineFrame)
        {
        num++;
        }

      num++;
      }
    NANOCLR_FOREACH_NODE_END();

    if (pass == 0)
      {
      uint32_t totLen = sizeof(thread_stack_t) + (num) * sizeof(call_t);
      *cmdReply = (thread_stack_t *) platform_malloc(totLen);

      (*cmdReply)->m_num = num;
      (*cmdReply)->m_status = th->m_status;
      (*cmdReply)->m_flags = th->m_flags;
      }
    }

  return s_ok;
  }

result_t debug_restart(uint32_t flags)
  {
  g_CLR_RT_ExecutionEngine.m_iReboot_Options = flags;

  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions |= c_fDebugger_RebootPending;

  return s_ok;
  }

result_t debug_set_execution_flags(uint32_t flags_to_set, uint32_t flags_to_reset)
  {
  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions |= flags_to_set;
  g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions &= ~flags_to_reset;

  return s_ok;
  }

result_t debug_get_execution_flags(uint32_t *flags)
  {
  if (flags == 0)
    return e_bad_parameter;

  *flags = g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions;
  return s_ok;
  }


static result_t fill_values(
  CLR_RT_HeapBlock *ptr,
  debugging_value_t *array,
  size_t num,
  CLR_RT_HeapBlock *reference,
  CLR_RT_TypeDef_Instance *pTD)
  {
  if (ptr == 0)
    return e_bad_pointer;

  if (array == 0 || num == 0)
    return e_bad_parameter;

  debugging_value_t *dst = array++;
  num--;
  CLR_RT_TypeDescriptor desc;

  memset(dst, 0, sizeof(*dst));

  dst->m_referenceID = (reference != NULL) ? reference : ptr;
  dst->m_dt = ptr->DataType();
  dst->m_flags = ptr->DataFlags();
  dst->m_size = ptr->DataSize();

  if (pTD != NULL)
    {
    dst->m_td = pTD->m_index;
    }
  else if (SUCCEEDED(desc.InitializeFromObject(*ptr)))
    {
    dst->m_td = desc.m_handlerCls.m_index;
    }

  switch (dst->m_dt)
    {
    case DATATYPE_BOOLEAN:
    case DATATYPE_I1:
    case DATATYPE_U1:

    case DATATYPE_CHAR:
    case DATATYPE_I2:
    case DATATYPE_U2:

    case DATATYPE_I4:
    case DATATYPE_U4:
    case DATATYPE_R4:

    case DATATYPE_I8:
    case DATATYPE_U8:
    case DATATYPE_R8:
    case DATATYPE_DATETIME:
    case DATATYPE_TIMESPAN:
    case DATATYPE_REFLECTION:
      //
      // Primitives or optimized value types.
      //
      memcpy(dst->m_builtinValue, (void *)&ptr->NumericByRefConst().u1, 8);
      break;

    case DATATYPE_STRING:
    {
    const char *text = ptr->StringText();

    if (text != NULL)
      {
      dst->m_charsInString = text;
      dst->m_bytesInString = strlen(text);

      if (dst->m_bytesInString > sizeof(dst->m_builtinValue))
        dst->m_bytesInString = sizeof(dst->m_builtinValue) - 1;

      strncpy((char *)dst->m_builtinValue, text, sizeof(dst->m_builtinValue));
      }
    else
      {
      dst->m_charsInString = NULL;
      dst->m_bytesInString = 0;
      dst->m_builtinValue[0] = 0;
      }
    }
    break;

    case DATATYPE_OBJECT:
    case DATATYPE_BYREF:
      return fill_values(ptr->Dereference(), array, num, NULL, pTD);

    case DATATYPE_CLASS:
    case DATATYPE_VALUETYPE:
      dst->m_td = ptr->ObjectCls();
      break;

    case DATATYPE_SZARRAY:
    {
    CLR_RT_HeapBlock_Array *ptr2 = (CLR_RT_HeapBlock_Array *)ptr;

    dst->m_array_numOfElements = ptr2->m_numOfElements;
    dst->m_array_depth = ptr2->ReflectionDataConst().m_levels;
    dst->m_array_typeIndex = ptr2->ReflectionDataConst().m_data.m_type;
    }
    break;

    ////////////////////////////////////////

    case DATATYPE_WEAKCLASS:
      break;

    case DATATYPE_ARRAY_BYREF:
      dst->m_arrayref_referenceID = ptr->Array();
      dst->m_arrayref_index = ptr->ArrayIndex();

      break;

    case DATATYPE_DELEGATE_HEAD:
      break;

    case DATATYPE_DELEGATELIST_HEAD:
      break;

    case DATATYPE_FREEBLOCK:
    case DATATYPE_CACHEDBLOCK:
    case DATATYPE_ASSEMBLY:
    case DATATYPE_OBJECT_TO_EVENT:
    case DATATYPE_BINARY_BLOB_HEAD:

    case DATATYPE_THREAD:
    case DATATYPE_SUBTHREAD:
    case DATATYPE_STACK_FRAME:
    case DATATYPE_TIMER_HEAD:
    case DATATYPE_LOCK_HEAD:
    case DATATYPE_LOCK_OWNER_HEAD:
    case DATATYPE_LOCK_REQUEST_HEAD:
    case DATATYPE_WAIT_FOR_OBJECT_HEAD:
    case DATATYPE_FINALIZER_HEAD:
    case DATATYPE_MEMORY_STREAM_HEAD:
    case DATATYPE_MEMORY_STREAM_DATA:

    case DATATYPE_SERIALIZER_HEAD:
    case DATATYPE_SERIALIZER_DUPLICATE:
    case DATATYPE_SERIALIZER_STATE:

    case DATATYPE_ENDPOINT_HEAD:


      break;
    }

  return s_ok;
  }

static result_t get_value(
  CLR_RT_HeapBlock *ptr,
  CLR_RT_HeapBlock *reference,
  CLR_RT_TypeDef_Instance *pTD,
  debugging_value_t *reply,
  size_t count  )
  {
  return fill_values(ptr, reply, count, reference, pTD);
  }

result_t set_breakpoints(const execution_breakpoint_t *breakpoints, size_t count)
  {
  g_CLR_RT_ExecutionEngine.InstallBreakpoints(breakpoints, count);
  return s_ok;
  }

static CLR_RT_Assembly *find_assembly(uint16_t idxAssm)
  {
  NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
    if (pASSM->m_idx == idxAssm)
      return pASSM;
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  return NULL;
  }

static bool check_type_def(uint32_t td, CLR_RT_TypeDef_Instance &inst)
  {
  
  CLR_RT_Assembly *assm = find_assembly(get_assembly_index(td));

  if (assm && get_type_index(td) < assm->m_pTablesSize[TBL_TypeDef])
    return inst.InitializeFromIndex(td);

  return false;
  }

static bool check_field_def(uint32_t fd, CLR_RT_FieldDef_Instance &inst)
  {
  CLR_RT_Assembly *assm = find_assembly(get_assembly_index(fd));

  if (assm != 0 && get_field_index(fd) < assm->m_pTablesSize[TBL_FieldDef])
    return inst.InitializeFromIndex(fd);

  return false;
  }

static bool check_method_def(uint32_t md, CLR_RT_MethodDef_Instance &inst)
  {
  CLR_RT_Assembly *assm = find_assembly(get_assembly_index(md));

  if (assm != 0 && get_method_index(md) < assm->m_pTablesSize[TBL_MethodDef])
    {
    return inst.InitializeFromIndex(md);
    }

  return false;
  }

static CLR_RT_StackFrame *check_stack_frame(int32_t pid, uint32_t depth, bool &isInline)
  {
  CLR_RT_Thread *th = get_thread_from_pid(pid);

  isInline = false;

  if (th)
    {
    NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
      {
      if (call->m_inlineFrame)
        {
        if (depth-- == 0)
          {
          isInline = true;
          return call;
          }
        }

      if (depth-- == 0)
        return call;
      }
    NANOCLR_FOREACH_NODE_END();
    }

  return NULL;
  }

result_t debug_thread_pause(uint32_t pid)
  {
  

  CLR_RT_Thread *th = get_thread_from_pid(pid);

  if (th == 0)
    return e_bad_parameter;

  return th->Suspend();
  }

result_t debug_thread_resume(uint32_t pid)
  {
  CLR_RT_Thread *th = get_thread_from_pid(pid);

  if (th == 0)
    return e_bad_parameter;

  return th->Resume();
  }

result_t debug_thread_get_values(uint32_t pid, debugging_value_t *values, size_t count) 
  {
  CLR_RT_Thread *th = get_thread_from_pid(pid);

  if (th == 0)
    return e_bad_parameter;

  CLR_RT_HeapBlock *pThread = 0;
  bool fFound = false;

  // If we are a thread spawned by the debugger to perform evaluations,
  // return the thread object that correspond to thread that has focus in debugger.
  th = th->m_realThread;

  // Find an existing managed thread, if it exists
  // making sure to only return the managed object association with the current appdomain
  // to prevent leaking of managed Thread objects across AD boundaries.

  NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, src, th->m_references)
    {
    CLR_RT_HeapBlock *pManagedThread = src->m_objectPtr;
    _ASSERTE(pManagedThread != NULL);

    fFound = true;

    if (fFound)
      {
      pThread = pManagedThread;

      break;
      }
    }
  NANOCLR_FOREACH_NODE_END();

  if (!fFound)
    {
    pThread = (CLR_RT_HeapBlock *)platform_malloc(sizeof(CLR_RT_HeapBlock));

    // Create the managed thread.
    // This implies that there is no state in the managed object.  This is not exactly true, as the managed thread
    // contains the priority as well as the delegate to start.  However, that state is really just used as a
    // placeholder for the data before the thread is started.  Once the thread is started, they are copied over to
    // the unmanaged thread object and no longer used.  The managed object is then used simply as a wrapper for the
    // unmanaged thread.  Therefore, it is safe to simply make another managed thread here.
    if (SUCCEEDED(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*pThread, g_CLR_RT_WellKnownTypes.m_Thread)))
      {
      CLR_RT_HeapBlock *pRes = pThread->Dereference();

      int pri = th->GetThreadPriority();
      /*
      pRes[Library_corlib_native_System_Threading_Thread::FIELD___priority].NumericByRef().s4 = pri;

      if (SUCCEEDED(CLR_RT_ObjectToEvent_Source::CreateInstance(
        th,
        *pRes,
        pRes[Library_corlib_native_System_Threading_Thread::FIELD___thread])))
        {
        fFound = true;
        }
        */
      }
    }

  if (!fFound)
    return false;

  return get_value(pThread, NULL, NULL, values, count);
  }

result_t debug_exception_get_values(uint32_t pid, debugging_value_t *values, size_t count)
  {
  CLR_RT_Thread *th = get_thread_from_pid(pid);

  if (th == 0)
    return e_bad_parameter;
  CLR_RT_HeapBlock *blk = NULL;

  blk = &th->m_currentException;

  return get_value(blk, NULL, NULL, values, count);
  }

result_t debug_thread_unwind(uint32_t pid, uint32_t depth)
  {
  CLR_RT_StackFrame *call;
  CLR_RT_Thread *th;
  bool isInline = false;

  if ((call = check_stack_frame(pid, depth, isInline)) != NULL)
    {
    _ASSERTE((call->m_flags & CLR_RT_StackFrame::c_MethodKind_Native) == 0);

    th = call->m_owningThread;
    _ASSERTE(th->m_nestedExceptionsPos);

    CLR_RT_Thread::UnwindStack &us = th->m_nestedExceptions[th->m_nestedExceptionsPos - 1];
    _ASSERTE(th->m_currentException.Dereference() == us.m_exception);
    _ASSERTE(us.m_flags & CLR_RT_Thread::UnwindStack::c_ContinueExceptionHandler);

    us.m_handlerStack = call;
    us.m_flags |= CLR_RT_Thread::UnwindStack::c_MagicCatchForInteceptedException;

    us.SetPhase(CLR_RT_Thread::UnwindStack::p_2_RunningFinallys_0);
    }

  return s_ok;
  }

result_t debug_stack_info(uint32_t pid, uint32_t depth, stack_info_t *info)
  {
  CLR_RT_StackFrame *call;
  bool isInline = false;

  if ((call = check_stack_frame(pid, depth, isInline)) != NULL)
    {
    if (isInline)
      {
      info->m_md = call->m_inlineFrame->m_frame.m_call.m_index;
      info->m_IP = (uint32_t)(call->m_inlineFrame->m_frame.m_IP - call->m_inlineFrame->m_frame.m_IPStart);
      info->m_numOfArguments = call->m_inlineFrame->m_frame.m_call.m_target->numArgs;
      info->m_numOfLocals = call->m_inlineFrame->m_frame.m_call.m_target->numLocals;
      info->m_depthOfEvalStack = (uint32_t)(call->m_evalStack - call->m_inlineFrame->m_frame.m_evalStack);
      }
    else
      {
      info->m_md = call->m_call.m_index;
      info->m_IP = (uint32_t)(call->m_IP - call->m_IPstart);
      info->m_numOfArguments = call->m_call.m_target->numArgs;
      info->m_numOfLocals = call->m_call.m_target->numLocals;
      info->m_depthOfEvalStack = (uint32_t)call->TopValuePosition();
      }

    return s_ok;
    }

  return e_not_found;
  }

result_t debug_set_ip(uint32_t pid, uint32_t depth, uint32_t ip, uint32_t depthOfEvalStack)
  {
  CLR_RT_StackFrame *call;
  bool isInline = false;

  if ((call = check_stack_frame(pid, depth, isInline)) != NULL)
    {
    if (isInline)
      return e_unexpected;
    else
      {
      call->m_IP = call->m_IPstart + ip;
      call->m_evalStackPos = call->m_evalStack + depthOfEvalStack;
      }

    call->m_flags &= ~CLR_RT_StackFrame::c_InvalidIP;

    return s_ok;
    }

  return e_bad_parameter;
  }

static bool IsBlockEnumMaybe(void *obj)
  {
  CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)obj;
  const uint32_t c_MaskForPrimitive = CLR_RT_DataTypeLookup::c_Integer | CLR_RT_DataTypeLookup::c_Numeric;

  CLR_RT_TypeDescriptor desc;

  if (FAILED(desc.InitializeFromObject(*blk)))
    return false;

  const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[desc.m_handlerCls.m_target->dataType];

  return (dtl.m_flags & c_MaskForPrimitive) == c_MaskForPrimitive;
  }

static bool SetBlockHelper(CLR_RT_HeapBlock *blk, CLR_DataType dt, uint8_t *builtinValue)
  {
  
  bool fCanAssign = false;

  if (blk)
    {
    CLR_DataType dtDst;
    CLR_RT_HeapBlock src;

    dtDst = blk->DataType();

    src.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, 0, 1));
    memcpy((void *)&src.NumericByRef().u1, builtinValue, sizeof(uint64_t));

    if (dtDst == dt)
      {
      fCanAssign = true;
      }
    else
      {
      if (dt == DATATYPE_REFLECTION)
        {
        fCanAssign = (dtDst == DATATYPE_OBJECT && blk->Dereference() == NULL);
        }
      else if (dt == DATATYPE_OBJECT)
        {
        fCanAssign = (src.Dereference() == NULL && dtDst == DATATYPE_REFLECTION);
        }
      else
        {
        _ASSERTE(c_CLR_RT_DataTypeLookup[dtDst].m_flags & CLR_RT_DataTypeLookup::c_Numeric);

        if (c_CLR_RT_DataTypeLookup[dtDst].m_sizeInBytes == sizeof(int32_t) &&
          c_CLR_RT_DataTypeLookup[dt].m_sizeInBytes < sizeof(int32_t))
          {
          dt = dtDst;
          fCanAssign = true;
          }
        }
      }

    if (fCanAssign)
      {
      blk->ChangeDataType(dt);
      memcpy((void *)&blk->NumericByRef().u1, builtinValue, sizeof(uint64_t));
      }
    }

  return fCanAssign;
  }

static CLR_RT_HeapBlock *GetScratchPad_Helper(int idx)
  {
  
  CLR_RT_HeapBlock_Array *array = g_CLR_RT_ExecutionEngine.m_scratchPadArray;
  CLR_RT_HeapBlock tmp;
  CLR_RT_HeapBlock ref;

  tmp.SetObjectReference(array);

  if (SUCCEEDED(ref.InitializeArrayReference(tmp, idx)))
    {
    return (CLR_RT_HeapBlock *)array->GetElement(idx);
    }

  return NULL;
  }

//--//

result_t debug_set_scratchpad_size(uint32_t size)
  {
  CLR_RT_HeapBlock ref;

  if (size == 0)
    {
    g_CLR_RT_ExecutionEngine.m_scratchPadArray = NULL;
    }
  else
    {
    if (SUCCEEDED(CLR_RT_HeapBlock_Array::CreateInstance(ref, size, g_CLR_RT_WellKnownTypes.m_Object)))
      {
      CLR_RT_HeapBlock_Array *pOld = g_CLR_RT_ExecutionEngine.m_scratchPadArray;
      CLR_RT_HeapBlock_Array *pNew = ref.DereferenceArray();

      if (pOld)
        {
        memcpy(
          pNew->GetFirstElement(),
          pOld->GetFirstElement(),
          sizeof(CLR_RT_HeapBlock) * __min(pNew->m_numOfElements, pOld->m_numOfElements));
        }

      g_CLR_RT_ExecutionEngine.m_scratchPadArray = pNew;
      }
    }

  return s_ok;
  }

result_t debug_get_value_stack(uint32_t pid, uint32_t depth, uint32_t kind, uint32_t index, debugging_value_t *values, size_t count)
  {
  CLR_RT_StackFrame *call;
  bool isInline = false;

  if ((call = check_stack_frame(pid, depth, isInline)) != NULL)
    {
    CLR_RT_HeapBlock *array;
    uint32_t num;
    CLR_RT_MethodDef_Instance &md = isInline ? call->m_inlineFrame->m_frame.m_call : call->m_call;

    switch (kind)
      {
      case c_Argument:
        array = isInline ? call->m_inlineFrame->m_frame.m_args : call->m_arguments;
        num = isInline ? md.m_target->numArgs : md.m_target->numArgs;
        break;

      case c_Local:
        array = isInline ? call->m_inlineFrame->m_frame.m_locals : call->m_locals;
        num = isInline ? md.m_target->numLocals : md.m_target->numLocals;
        break;

      case c_EvalStack:
        array = isInline ? call->m_inlineFrame->m_frame.m_evalStack : call->m_evalStack;
        num = isInline ? (uint32_t)(call->m_evalStack - call->m_inlineFrame->m_frame.m_evalStack)
          : (uint32_t)call->TopValuePosition();
        break;

      default:
        return false;
      }

    if (index >= num)
      return false;

    CLR_RT_HeapBlock *blk = &array[index];
    CLR_RT_HeapBlock *reference = NULL;
    CLR_RT_HeapBlock tmp;
    CLR_RT_TypeDef_Instance *pTD = NULL;
    CLR_RT_TypeDef_Instance td;

    if (kind != c_EvalStack && IsBlockEnumMaybe(blk))
      {
      uint32_t iElement = index;
      CLR_RT_SignatureParser parser;
      CLR_RT_SignatureParser::Element res;
      CLR_RT_TypeDescriptor desc;

      if (kind == c_Argument)
        {
        parser.Initialize_MethodSignature(md.m_assm, md.m_target);

        iElement++; // Skip the return value, always at the head of the signature.

        if (parser.m_flags & PIMAGE_CEE_CS_CALLCONV_HASTHIS)
          {
          if (iElement == 0)
            return false; // The requested argument is the "this" argument, it can never be a primitive.

          iElement--;
          }
        }
      else
        {
        parser.Initialize_MethodLocals(md.m_assm, md.m_target);
        }

      do
        {
        parser.Advance(res);
        } while (iElement--);

        //
        // Arguments to a methods come from the eval stack and we don't fix up the eval stack for each call.
        // So some arguments have the wrong datatype, since an eval stack push always promotes to 32 bits.
        //
        if (c_CLR_RT_DataTypeLookup[blk->DataType()].m_sizeInBytes == sizeof(int32_t) &&
          c_CLR_RT_DataTypeLookup[res.m_dt].m_sizeInBytes < sizeof(int32_t))
          {
          tmp.Assign(*blk);
          tmp.ChangeDataType(res.m_dt);

          reference = blk;
          blk = &tmp;
          }

        //
        // Check for enum.
        //
        desc.InitializeFromType(res.m_cls);

        if (desc.m_handlerCls.m_target->IsEnum())
          {
          td = desc.m_handlerCls;
          pTD = &td;
          }
      }

    return get_value( blk, reference, pTD, values, count);
    }

  return false;
  }

  result_t debug_get_value_field(void *obj, uint32_t offset, uint32_t fd, debugging_value_t *values, size_t count)
  {
    CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)obj;

  CLR_RT_HeapBlock *reference = NULL;
  CLR_RT_HeapBlock tmp;
  CLR_RT_TypeDescriptor desc;
  CLR_RT_TypeDef_Instance td;
  CLR_RT_TypeDef_Instance *pTD = NULL;
  CLR_RT_FieldDef_Instance inst;

  if (blk != NULL && offset > 0)
    {
    if (FAILED(desc.InitializeFromObject(*blk)))
      return e_unexpected;

    td = desc.m_handlerCls;
    offset = offset - 1;

    while (true)
      {
      uint32_t iFields = td.m_target->iFields_Num;
      uint32_t totalFields = td.CrossReference().m_totalFields;
      uint32_t dFields = totalFields - iFields;

      if (offset >= dFields)
        {
        offset -= dFields;
        break;
        }

      if (!td.SwitchToParent())
        return e_unexpected;
      }

    fd = create_index(get_assembly_index(td.m_index), td.m_target->iFields_First + offset);
    }

  if (!check_field_def(fd, inst))
    return e_unexpected;

  if (blk == NULL)
    {
    blk = CLR_RT_ExecutionEngine::AccessStaticField(fd);
    }
  else
    {
    if (offset == 0)
      {
      offset = inst.CrossReference().m_offset;
      }

    if (offset == 0)
      return false;

    switch (blk->DataType())
      {
      case DATATYPE_CLASS:
      case DATATYPE_VALUETYPE:
        break;

      default:
        if (FAILED(blk->EnsureObjectReference(blk)))
          return false;
        break;
      }

    switch (blk->DataType())
      {
      case DATATYPE_DATETIME: // Special case.
      case DATATYPE_TIMESPAN: // Special case.
        tmp.SetInteger((int64_t)blk->NumericByRefConst().s8);
        reference = blk;
        blk = &tmp;
        break;

      default:
        blk = &blk[offset];
        break;
      }
    }

  if (IsBlockEnumMaybe(blk))
    {
    if (SUCCEEDED(desc.InitializeFromFieldDefinition(inst)))
      {
      if (desc.m_handlerCls.m_target->IsEnum())
        {
        pTD = &desc.m_handlerCls;
        }
      }
    }

  return get_value(blk, reference, pTD, values, count);
  }

  result_t debug_get_value_array(void *obj, uint32_t index, debugging_value_t *values, size_t count)
  {
    CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)obj;

  CLR_RT_HeapBlock *reference = NULL;
  CLR_RT_HeapBlock tmp;
  CLR_RT_HeapBlock ref;
  CLR_RT_TypeDef_Instance *pTD = NULL;
  CLR_RT_TypeDef_Instance td;

  tmp.SetObjectReference(blk);

  if (SUCCEEDED(ref.InitializeArrayReference(tmp, index)))
    {
    CLR_RT_HeapBlock_Array *array = ref.Array();

    if (array->m_fReference)
      {
      blk = (CLR_RT_HeapBlock *)array->GetElement(index);
      }
    else
      {
      if (FAILED(tmp.LoadFromReference(ref)))
        return false;

      blk = &tmp;
      reference = (CLR_RT_HeapBlock *)-1;
      }

    if (IsBlockEnumMaybe(blk))
      {
      if (td.InitializeFromIndex(array->ReflectionDataConst().m_data.m_type))
        {
        if (td.m_target->IsEnum())
          {
          pTD = &td;
          }
        }
      }
    }

  return get_value(blk, reference, pTD, values, count);
  }

result_t debug_get_value_block(void *obj, debugging_value_t *values, size_t count)
  {
  CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)obj;

  return get_value(blk, NULL, NULL, values, count);
  }

result_t debug_get_value_scratchpad(uint32_t index, debugging_value_t *values, size_t count)
  {
  CLR_RT_HeapBlock *blk = GetScratchPad_Helper(index);

  return get_value(blk, NULL, NULL, values, count);
  }

result_t debug_set_value_scratchpad(void *obj, CLR_DataType datatype, uint8_t *value, size_t value_len)
  {
  CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)obj;

  return SetBlockHelper(blk, datatype, value);
  }

result_t debug_set_value_scratchpad_array(void *obj, uint32_t index, uint8_t *value, size_t value_len)
  {
  CLR_RT_HeapBlock_Array *array = (CLR_RT_HeapBlock_Array *)obj;

  CLR_RT_HeapBlock tmp;
  bool fSuccess = false;

  tmp.SetObjectReference(array);

  //
  // We can only set values in arrays of primitive types.
  //
  if (array != NULL && !array->m_fReference)
    {
    CLR_RT_HeapBlock ref;

    if (SUCCEEDED(ref.InitializeArrayReference(tmp, index)))
      {
      if (SUCCEEDED(tmp.LoadFromReference(ref)))
        {
        if (SetBlockHelper(&tmp, tmp.DataType(), value))
          {
          return tmp.StoreToReference(ref, 0);
          }
        }
      }
    }

  return e_bad_parameter;
  }

result_t debbug_allocate_scratchpad_object(int index, uint32_t td, debugging_value_t *values, size_t count)
  {
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(index);

  if (ptr)
    {
    if (SUCCEEDED(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*ptr, td)))
      {
      blk = ptr;
      }
    }

  return get_value(blk, NULL, NULL, values, count);
  }

result_t debug_allocate_scratchpad_string(int index, uint32_t size, debugging_value_t *values, size_t count)
  {
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(index);

  if (ptr)
    {
    CLR_RT_HeapBlock_String *str = CLR_RT_HeapBlock_String::CreateInstance(*ptr, size);

    if (str)
      {
      char *dst = (char *)str->StringText();

      //
      // Fill the string with spaces, it will be set at a later stage.
      //
      memset(dst, ' ', size);
      dst[size] = 0;

      blk = ptr;
      }
    }

  return get_value(blk, NULL, NULL, values, count);
  }

result_t debug_allocate_scratchpad_array(int index, uint32_t td, uint32_t depth, uint32_t num_elem, debugging_value_t *values, size_t count)
  {
  CLR_RT_HeapBlock *blk = NULL;
  CLR_RT_HeapBlock *ptr = GetScratchPad_Helper(index);

  if (ptr)
    {
    CLR_RT_ReflectionDef_Index reflex;

    reflex.m_kind = REFLECTION_TYPE;
    reflex.m_levels = depth;
    reflex.m_data.m_type = td;

    if (SUCCEEDED(CLR_RT_HeapBlock_Array::CreateInstance(*ptr, num_elem, reflex)))
      {
      blk = ptr;
      }
    }

  return get_value(blk, NULL, NULL, values, count);
  }

struct AnalyzeObject
  {
  CLR_RT_HeapBlock *m_ptr;
  bool m_fNull;
  bool m_fBoxed;
  bool m_fCanBeNull;
  CLR_RT_TypeDescriptor m_desc;
  CLR_RT_HeapBlock m_value;
  };

static HRESULT AnalyzeObject_Helper(CLR_RT_HeapBlock *ptr, AnalyzeObject &ao)
  {
  
  HRESULT hr;

  if (ptr && ptr->DataType() == DATATYPE_BYREF)
    ptr = ptr->Dereference();

  ao.m_ptr = ptr;

  if (ptr == NULL || (ptr->DataType() == DATATYPE_OBJECT && ptr->Dereference() == NULL))
    {
    ao.m_fNull = true;
    ao.m_fBoxed = false;
    ao.m_fCanBeNull = true;
    }
  else
    {
    NANOCLR_CHECK_HRESULT(ao.m_desc.InitializeFromObject(*ptr));

    ao.m_fNull = false;
    ao.m_fBoxed = (ptr->DataType() == DATATYPE_OBJECT && ptr->Dereference()->IsBoxed());

    switch (ao.m_desc.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask2 & ~CLR_RT_DataTypeLookup::c_SemanticMask)
      {
      case CLR_RT_DataTypeLookup::c_Array:
      case CLR_RT_DataTypeLookup::c_ArrayList:
        ao.m_fCanBeNull = true;
        break;
      default:
      {
      switch (ao.m_desc.m_flags & CLR_RT_DataTypeLookup::c_SemanticMask)
        {
        case CLR_RT_DataTypeLookup::c_Primitive:
        case CLR_RT_DataTypeLookup::c_ValueType:
        case CLR_RT_DataTypeLookup::c_Enum:
          ao.m_fCanBeNull =
            ao.m_fBoxed || (ao.m_desc.m_handlerCls.m_index == g_CLR_RT_WellKnownTypes.m_String);
          break;

        default:
          ao.m_fCanBeNull = true;
          break;
        }

      break;
      }
      }
    }

  NANOCLR_NOCLEANUP();
  }

static HRESULT Assign_Helper(CLR_RT_HeapBlock *blkDst, CLR_RT_HeapBlock *blkSrc)
  {
  
  HRESULT hr;

  AnalyzeObject aoDst;
  AnalyzeObject aoSrc;
  CLR_RT_HeapBlock srcVal;
  srcVal.SetObjectReference(NULL);
  CLR_RT_ProtectFromGC gc(srcVal);

  NANOCLR_CHECK_HRESULT(AnalyzeObject_Helper(blkDst, aoDst));
  NANOCLR_CHECK_HRESULT(AnalyzeObject_Helper(blkSrc, aoSrc));

  if (aoSrc.m_fNull)
    {
    if (aoDst.m_fCanBeNull == false)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }

    NANOCLR_CHECK_HRESULT(srcVal.StoreToReference(*blkDst, 0));
    }
  else
    {
    NANOCLR_CHECK_HRESULT(srcVal.LoadFromReference(*blkSrc));

    if (aoDst.m_fNull)
      {
      if (aoSrc.m_fCanBeNull == false)
        {
        NANOCLR_CHECK_HRESULT(srcVal.PerformBoxing(aoSrc.m_desc.m_handlerCls));
        }

      blkDst->Assign(srcVal);
      }
    else
      {
      if (srcVal.IsAValueType())
        {
        if (blkDst->IsAValueType() == false)
          {
          NANOCLR_CHECK_HRESULT(srcVal.PerformBoxing(aoSrc.m_desc.m_handlerCls));
          }
        }
      else
        {
        if (blkDst->IsAValueType() == true)
          {
          NANOCLR_CHECK_HRESULT(srcVal.PerformUnboxing(aoSrc.m_desc.m_handlerCls));
          }
        }

      NANOCLR_CHECK_HRESULT(blkDst->Reassign(srcVal));
      }
    }

  NANOCLR_NOCLEANUP();
  }

result_t debug_value_assign(void *src, void *dst, debugging_value_t *values, size_t count)
  {
  CLR_RT_HeapBlock *dstBlk = (CLR_RT_HeapBlock *)dst;
  CLR_RT_HeapBlock *srcBlk = (CLR_RT_HeapBlock *)src;

  if (dstBlk && FAILED(Assign_Helper(dstBlk, srcBlk)))
    {
    dstBlk = NULL;
    }

  return get_value(dstBlk, NULL, NULL, values, count);
  }

result_t debug_get_assembly(uint32_t index, CLR_RT_Assembly **assembly)
  {
  if (index >= g_CLR_RT_TypeSystem.m_assembliesMax)
    return e_no_more_information;

  if (assembly == 0)
    return e_bad_pointer;

  // TODO: implement this....

  return s_ok;
  }

result_t debug_resolve_type(uint32_t type_id, char *buffer, size_t len)
  {
  CLR_RT_TypeDef_Instance inst;

  if (check_type_def(type_id, inst))
    return g_CLR_RT_TypeSystem.BuildTypeName(inst.m_index, buffer, len);

  return e_not_found;
  }

result_t debug_resolve_field(uint32_t fd, uint32_t *td, uint32_t *index, char *name, size_t len)
  {
  CLR_RT_FieldDef_Instance inst;

  if (check_field_def(fd, inst))
    {

    if (succeeded(g_CLR_RT_TypeSystem.BuildFieldName(inst.m_index, name, len)))
      {
      CLR_RT_TypeDef_Instance instClass;
      instClass.InitializeFromField(inst);

      *td = instClass.m_index;
      *index = inst.CrossReference().m_offset;

      return s_ok;
      }
    }

  return e_not_found;
  }

result_t debug_resolve_method(uint32_t md, uint32_t *td, char *name, size_t len)
  {
  CLR_RT_MethodDef_Instance inst;
  CLR_RT_TypeDef_Instance instOwner;

  if (check_method_def(md, inst) && instOwner.InitializeFromMethod(inst))
    {
    *td = instOwner.m_index;

    CLR_SafeSprintf(name, len, "%s", inst.m_assm->GetString(inst.m_target->name));

    return s_ok;
    }

  return s_ok;
  }

result_t debug_resolve_virtual_method(uint32_t vmd, void *obj, uint32_t *td, char *name, size_t len, uint32_t *md)
  {
  uint32_t cls;

  CLR_RT_HeapBlock *object = (CLR_RT_HeapBlock *)obj;

  if (succeeded(CLR_RT_TypeDescriptor::ExtractTypeIndexFromObject(*object, cls)) &&
    g_CLR_RT_EventCache.FindVirtualMethod(cls, vmd, md))
      return s_ok;

  return e_not_found;
  }

result_t debug_set_just_my_code_method(uint32_t md, bool is_it)
  {
  CLR_RT_MethodDef_Instance inst;

  if (!check_method_def(md, inst))
    return e_not_found;

  if (inst.m_target->RVA == CLR_EmptyIndex)
    return e_unexpected;

  inst.DebuggingInfo().SetJMC(is_it);

  return true;
  }

result_t debug_set_just_my_code_type(uint32_t index, bool is_it)
  {
  const CLR_RECORD_TYPEDEF *td;
  CLR_RT_TypeDef_Instance inst;
  int totMethods;
  uint32_t md;

  if (!check_type_def(index, inst))
    return e_bad_parameter;

  td = inst.m_target;
  totMethods = td->vMethods_Num + td->iMethods_Num + td->sMethods_Num;

  for (int i = 0; i < totMethods; i++)
    {
    md = create_index(get_assembly_index(index), td->methods_First + i);

    debug_set_just_my_code_method(md, is_it);
    }

  return true;
  }

result_t debug_set_just_my_code_assembly(uint32_t index, bool is_it)
  {
  CLR_RT_Assembly *assm = find_assembly(get_assembly_index(index));

  if (assm == 0)
    return e_not_found;

  for (int i = 0; i < assm->m_pTablesSize[TBL_TypeDef]; i++)
    debug_set_just_my_code_type(create_index(get_assembly_index(index), i), is_it);

  return s_ok;
  }
