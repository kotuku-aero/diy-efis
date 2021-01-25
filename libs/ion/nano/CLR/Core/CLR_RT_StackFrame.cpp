//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_StackFrame::Push(CLR_RT_Thread *th, const CLR_RT_MethodDef_Instance &callInst, int32_t extraBlocks)
  {
 
  HRESULT hr;

  CLR_RT_StackFrame *stack;
  CLR_RT_StackFrame *caller;
  CLR_RT_Assembly *assm;
  const CLR_RECORD_METHODDEF *md;
  const CLR_RT_MethodDef_Instance *callInstPtr = &callInst;
  uint32_t sizeLocals;
  uint32_t sizeEvalStack;

  assm = callInstPtr->m_assm;
  md = callInstPtr->m_target;

  sizeLocals = md->numLocals;
#ifndef CLR_NO_IL_INLINE
  sizeEvalStack = md->lengthEvalStack + CLR_RT_StackFrame::c_OverheadForNewObjOrInteropMethod + 1;
#else
  sizeEvalStack = md->lengthEvalStack + CLR_RT_StackFrame::c_OverheadForNewObjOrInteropMethod;
#endif

  //--//

  caller = th->CurrentFrame();

  //--//

  //
  // Allocate memory for the runtime state.
  //
  {
  uint32_t memorySize = sizeLocals + sizeEvalStack;

  if (extraBlocks > 0)
    memorySize += extraBlocks;
#ifndef CLR_NO_IL_INLINE
  if (memorySize < c_MinimumStack)
    {
    sizeEvalStack += c_MinimumStack - memorySize;
    memorySize = c_MinimumStack;
    }
#else
  if (memorySize < c_MinimumStack)
    memorySize = c_MinimumStack;
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

  memorySize += CONVERTFROMSIZETOHEAPBLOCKS(offsetof(CLR_RT_StackFrame, m_extension));

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

  stack = EVENTCACHE_EXTRACT_NODE_AS_BLOCKS(
    g_CLR_RT_EventCache,
    CLR_RT_StackFrame,
    DATATYPE_STACK_FRAME,
    0,
    memorySize);
  CHECK_ALLOCATION(stack);
  }

  //--//

  { //
  stack->m_owningSubThread =
    th->CurrentSubThread();   // CLR_RT_SubThread*         m_owningSubThread;  // EVENT HEAP - NO RELOCATION -
  stack->m_owningThread = th;   // CLR_RT_Thread*            m_owningThread;     // EVENT HEAP - NO RELOCATION -
                                // uint32_t                m_flags;
                                //
  stack->m_call = *callInstPtr; // CLR_RT_MethodDef_Instance m_call;
                                //
                                // CLR_RT_MethodHandler      m_nativeMethod;
                                // CLR_PMETADATA             m_IPstart;          // ANY   HEAP - DO RELOCATION -
                                // CLR_PMETADATA             m_IP;               // ANY   HEAP - DO RELOCATION -
                                //
  stack->m_locals =
    stack->m_extension; // CLR_RT_HeapBlock*         m_locals;           // EVENT HEAP - NO RELOCATION -
  stack->m_evalStack =
    stack->m_extension +
    sizeLocals; // CLR_RT_HeapBlock*         m_evalStack;        // EVENT HEAP - NO RELOCATION -
  stack->m_evalStackPos =
    stack->m_evalStack; // CLR_RT_HeapBlock*         m_evalStackPos;     // EVENT HEAP - NO RELOCATION -
  stack->m_evalStackEnd =
    stack->m_evalStack +
    sizeEvalStack;         // CLR_RT_HeapBlock*         m_evalStackEnd;     // EVENT HEAP - NO RELOCATION -
  stack->m_arguments = NULL; // CLR_RT_HeapBlock*         m_arguments;        // EVENT HEAP - NO RELOCATION -
                             //
                             // union
                             // {
  stack->m_customState = 0;  //    uint32_t             m_customState;
                             //    void*                  m_customPointer;
                             // };
                             //
#ifndef CLR_NO_IL_INLINE
  stack->m_inlineFrame = NULL;
#endif
                                         //
                                         // CLR_RT_HeapBlock          m_extension[1];
                                         //
#if defined(ENABLE_NATIVE_PROFILER)
  stack->m_fNativeProfiled = stack->m_owningThread->m_fNativeProfiled;
#endif
  CLR_RT_MethodHandler impl;

  if (md->flags & CLR_RECORD_METHODDEF::MD_DelegateInvoke) // Special case for delegate calls.
    {
    stack->m_nativeMethod = (CLR_RT_MethodHandler)CLR_RT_Thread::Execute_DelegateInvoke;

    stack->m_flags = CLR_RT_StackFrame::c_MethodKind_Native;
    stack->m_IPstart = NULL;
    }
  else if (assm->m_nativeCode && (impl = assm->m_nativeCode[get_method_index(stack->m_call.m_index)]) != NULL)
    {
    stack->m_nativeMethod = impl;

    stack->m_flags = CLR_RT_StackFrame::c_MethodKind_Native;
    stack->m_IPstart = NULL;
    stack->m_IP = NULL;
    }
  else
    {
    stack->m_nativeMethod = (CLR_RT_MethodHandler)CLR_RT_Thread::Execute_IL;

    if (md->RVA == CLR_EmptyIndex)
      NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    stack->m_flags = CLR_RT_StackFrame::c_MethodKind_Interpreted;
    stack->m_IPstart = assm->GetByteCode(md->RVA);
    stack->m_IP = stack->m_IPstart;
    }

#if defined(ENABLE_NATIVE_PROFILER)
  if (stack->m_owningThread->m_fNativeProfiled == false && md->flags & CLR_RECORD_METHODDEF::MD_NativeProfiled)
    {
    stack->m_flags |= CLR_RT_StackFrame::c_NativeProfiled;
    stack->m_owningThread->m_fNativeProfiled = true;
    }
#endif

  //--//

  th->m_stackFrames.LinkAtBack(stack);
  }

  if (md->numLocals)
    {
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitializeLocals(stack->m_locals, assm, md));
    }

  {
  uint32_t flags = md->flags & (md->MD_Synchronized | md->MD_GloballySynchronized);

  if (flags)
    {
    if (flags & md->MD_Synchronized)
      stack->m_flags |= c_NeedToSynchronize;
    if (flags & md->MD_GloballySynchronized)
      stack->m_flags |= c_NeedToSynchronizeGlobally;
    }
  }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  stack->m_depth = stack->Caller()->Prev() ? stack->Caller()->m_depth + 1 : 0;

  if (g_CLR_RT_ExecutionEngine.m_breakpointsNum)
    {
    if (stack->m_call.DebuggingInfo().HasBreakpoint())
      {
      stack->m_flags |= CLR_RT_StackFrame::c_HasBreakpoint;
      }

    if (stack->m_owningThread->m_fHasJMCStepper || (stack->m_flags & c_HasBreakpoint) ||
      (caller->Prev() != NULL && (caller->m_flags & c_HasBreakpoint)))
      {
      g_CLR_RT_ExecutionEngine.Breakpoint_StackFrame_Push(
        stack,
        c_DEPTH_STEP_CALL);
      }
    }
#endif

  //--//

  if (caller->Prev() != NULL && caller->m_nativeMethod == stack->m_nativeMethod)
    {
    if (stack->m_flags & CLR_RT_StackFrame::c_ProcessSynchronize)
      {
      stack->m_flags |= CLR_RT_StackFrame::c_CallerIsCompatibleForRet;
      }
    else
      {
      stack->m_flags |=
        CLR_RT_StackFrame::c_CallerIsCompatibleForCall | CLR_RT_StackFrame::c_CallerIsCompatibleForRet;
      }
    }

  //
  // If the arguments are in the caller's stack frame (var == 0), let's link the two.
  //
  if (extraBlocks < 0)
    {
#if defined(_WIN32)
    if (caller->m_evalStackPos > caller->m_evalStackEnd)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_STACK_OVERFLOW);
      }
#endif

    //
    // Everything is set up correctly, pop the operands.
    //
    stack->m_arguments = &caller->m_evalStackPos[-md->numArgs];

    caller->m_evalStackPos = stack->m_arguments;

#if defined(_WIN32)
    if (stack->m_arguments < caller->m_evalStack)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_STACK_UNDERFLOW);
      }
#endif
    }
  else
    {
    stack->m_arguments = stack->m_evalStackEnd;
    }

  NANOCLR_CLEANUP();

  NANOCLR_CLEANUP_END();
  }

#ifndef CLR_NO_IL_INLINE
bool CLR_RT_StackFrame::PushInline(
  CLR_PMETADATA &ip,
  CLR_RT_Assembly *&assm,
  CLR_RT_HeapBlock *&evalPos,
  CLR_RT_MethodDef_Instance &calleeInst,
  CLR_RT_HeapBlock *pThis)
  {
  const CLR_RECORD_METHODDEF *md = calleeInst.m_target;

  if ((m_inlineFrame != NULL) || // We can only support one inline at a time per stack call
    (m_evalStackEnd - evalPos) <= (md->numArgs + md->numLocals + md->lengthEvalStack +
      2) || // We must have enough space on the current stack for the inline method
    (m_nativeMethod != (CLR_RT_MethodHandler)CLR_RT_Thread::Execute_IL) || // We only support IL inlining
    (md->flags & ~CLR_RECORD_METHODDEF::MD_HasExceptionHandlers) >=
    CLR_RECORD_METHODDEF::MD_Constructor || // Do not try to inline constructors, etc because they require
                                            // special processing
    (0 != (md->flags & CLR_RECORD_METHODDEF::MD_Static)) || // Static methods also requires special processing
    (calleeInst.m_assm->m_nativeCode != NULL && (calleeInst.m_assm->m_nativeCode[calleeInst.Method()] !=
      NULL)) || // Make sure the callee is not an internal method
    (md->RVA == CLR_EmptyIndex) ||                         // Make sure we have a valid IP address for the method
    !g_CLR_RT_EventCache.GetInlineFrameBuffer(
      &m_inlineFrame)) // Make sure we have an extra slot in the inline cache
    {
    return false;
    }

  CLR_PMETADATA ipTmp = calleeInst.m_assm->GetByteCode(md->RVA);

  // make backup
  m_inlineFrame->m_frame.m_IP = ip;
  m_inlineFrame->m_frame.m_IPStart = m_IPstart;
  m_inlineFrame->m_frame.m_locals = m_locals;
  m_inlineFrame->m_frame.m_args = m_arguments;
  m_inlineFrame->m_frame.m_call = m_call;
  m_inlineFrame->m_frame.m_evalStack = m_evalStack;
  m_inlineFrame->m_frame.m_evalPos = pThis;

  // increment the evalPos pointer so that we don't corrupt the real stack
  evalPos++;
  assm = calleeInst.m_assm;
  ip = ipTmp;

  m_arguments = pThis;
  m_locals = &m_evalStackEnd[-md->numLocals];
  m_call = calleeInst;
  m_evalStackEnd = m_locals;
  m_evalStack = evalPos;
  m_evalStackPos = evalPos + 1;
  m_IPstart = ip;
  m_IP = ip;

  if (md->numLocals)
    {
    g_CLR_RT_ExecutionEngine.InitializeLocals(m_locals, calleeInst.m_assm, md);
    }

  m_flags |= CLR_RT_StackFrame::c_MethodKind_Inlined;

  if (md->retVal != DATATYPE_VOID)
    {
    m_flags |= CLR_RT_StackFrame::c_InlineMethodHasReturnValue;
    }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  m_depth++;

  if (g_CLR_RT_ExecutionEngine.m_breakpointsNum)
    {
    if (m_call.DebuggingInfo().HasBreakpoint())
      {
      m_flags |= CLR_RT_StackFrame::c_HasBreakpoint;
      }

    if (m_owningThread->m_fHasJMCStepper || (m_flags & CLR_RT_StackFrame::c_HasBreakpoint))
      {
      g_CLR_RT_ExecutionEngine.Breakpoint_StackFrame_Push(
        this,
        c_DEPTH_STEP_CALL);
      }
    }
#endif

  return true;
  }

void CLR_RT_StackFrame::PopInline()
  {

  CLR_RT_HeapBlock &src = m_evalStackPos[0];

  RestoreFromInlineStack();

  if (m_flags & CLR_RT_StackFrame::c_InlineMethodHasReturnValue)
    {
    if (m_owningThread->m_currentException.Dereference() == NULL)
      {
      CLR_RT_HeapBlock &dst = PushValueAndAssign(src);

      dst.Promote();
      }
    }

  g_CLR_RT_EventCache.FreeInlineBuffer(m_inlineFrame);
  m_inlineFrame = NULL;
  m_flags &= ~(CLR_RT_StackFrame::c_MethodKind_Inlined | CLR_RT_StackFrame::c_InlineMethodHasReturnValue);

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  if (m_owningThread->m_fHasJMCStepper || (m_flags & CLR_RT_StackFrame::c_HasBreakpoint))
    {
    g_CLR_RT_ExecutionEngine.Breakpoint_StackFrame_Pop(this, false);
    }
  m_depth--;
#endif
  }

void CLR_RT_StackFrame::RestoreFromInlineStack()
  {
  m_arguments = m_inlineFrame->m_frame.m_args;
  m_locals = m_inlineFrame->m_frame.m_locals;
  m_evalStackEnd += m_call.m_target->numLocals;
  m_call = m_inlineFrame->m_frame.m_call;
  m_IP = m_inlineFrame->m_frame.m_IP;
  m_IPstart = m_inlineFrame->m_frame.m_IPStart;
  m_evalStack = m_inlineFrame->m_frame.m_evalStack;
  m_evalStackPos = m_inlineFrame->m_frame.m_evalPos;
  }

void CLR_RT_StackFrame::RestoreStack(CLR_RT_InlineFrame &frame)
  {
  m_arguments = frame.m_args;
  m_locals = frame.m_locals;
  m_call = frame.m_call;
  m_IP = frame.m_IP;
  m_IPstart = frame.m_IPStart;
  m_evalStack = frame.m_evalStack;
  m_evalStackPos = frame.m_evalPos;
  m_evalStackEnd -= m_call.m_target->numLocals;
  }

void CLR_RT_StackFrame::SaveStack(CLR_RT_InlineFrame &frame)
  {
  frame.m_args = m_arguments;
  frame.m_locals = m_locals;
  frame.m_call = m_call;
  frame.m_IP = m_IP;
  frame.m_IPStart = m_IPstart;
  frame.m_evalPos = m_evalStackPos;
  frame.m_evalStack = m_evalStack;
  }
#endif

HRESULT CLR_RT_StackFrame::MakeCall(
  CLR_RT_MethodDef_Instance md,
  CLR_RT_HeapBlock *obj,
  CLR_RT_HeapBlock *args,
  int nArgs)
  {
 
  HRESULT hr;

  const CLR_RECORD_METHODDEF *mdR = md.m_target;
  bool fStatic = (mdR->flags & CLR_RECORD_METHODDEF::MD_Static) != 0;
  int numArgs = mdR->numArgs;
  int argsOffset = 0;
  CLR_RT_StackFrame *stackSub;
  CLR_RT_HeapBlock tmp;
  tmp.SetObjectReference(NULL);
  CLR_RT_ProtectFromGC gc(tmp);

  if (mdR->flags & CLR_RECORD_METHODDEF::MD_Constructor)
    {
    CLR_RT_TypeDef_Instance owner;
    owner.InitializeFromMethod(md);

    _ASSERTE(obj == NULL);

    _SIDE_ASSERTE(owner.InitializeFromMethod(md));

    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObject(tmp, owner));

    obj = &tmp;

    //
    // Make a copy of the object pointer.
    //
    PushValueAndAssign(tmp);
    }

  if (!fStatic)
    {
    FAULT_ON_NULL(obj);
    numArgs--;
    argsOffset = 1;
    }

  if (numArgs != nArgs)
    NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

  //
  // In case the invoked method is abstract or virtual, resolve it to the correct method implementation.
  //
  if (mdR->flags & (CLR_RECORD_METHODDEF::MD_Abstract | CLR_RECORD_METHODDEF::MD_Virtual))
    {
    uint32_t cls;
    uint32_t mdReal;

    _ASSERTE(obj);
    _ASSERTE(!fStatic);

    NANOCLR_CHECK_HRESULT(CLR_RT_TypeDescriptor::ExtractTypeIndexFromObject(*obj, cls));

    if (g_CLR_RT_EventCache.FindVirtualMethod(cls, md, mdReal) == false)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }

    md.InitializeFromIndex(mdReal);

    mdR = md.m_target;
    }


  NANOCLR_CHECK_HRESULT(CLR_RT_StackFrame::Push(m_owningThread, md, md.m_target->numArgs));

  stackSub = m_owningThread->CurrentFrame();

  if (!fStatic)
    {
    stackSub->m_arguments[0].Assign(*obj);
    }

  if (numArgs)
    {
    memcpy(&stackSub->m_arguments[argsOffset], args, sizeof(CLR_RT_HeapBlock) * numArgs);
    }


  NANOCLR_CHECK_HRESULT(stackSub->FixCall());

  NANOCLR_SET_AND_LEAVE(CLR_E_RESTART_EXECUTION);

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_StackFrame::FixCall()
  {
 
  HRESULT hr;

  const CLR_RECORD_METHODDEF *target = m_call.m_target;
  uint8_t numArgs = target->numArgs;

  //
  // The copy of ValueTypes is delayed as much as possible.
  //
  // If an argument is a ValueType, now it's a good time to clone it.
  //
  if (numArgs)
    {
    CLR_RT_SignatureParser parser;
    parser.Initialize_MethodSignature(m_call.m_assm, target);
    CLR_RT_SignatureParser::Element res;
    CLR_RT_HeapBlock *args = m_arguments;

    if (parser.m_flags & PIMAGE_CEE_CS_CALLCONV_HASTHIS)
      {
      args++;
      }

    //
    // Skip return value.
    //
    NANOCLR_CHECK_HRESULT(parser.Advance(res));

    for (; parser.Available() > 0; args++)
      {
      NANOCLR_CHECK_HRESULT(parser.Advance(res));

      if (res.m_levels > 0)
        continue; // Array, no need to fix.

      if (args->DataType() == DATATYPE_OBJECT)
        {
        CLR_RT_TypeDef_Instance inst;
        inst.InitializeFromIndex(res.m_cls);
        CLR_DataType dtT = (CLR_DataType)inst.m_target->dataType;
        const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dtT];

        if (dtl.m_flags & (CLR_RT_DataTypeLookup::c_OptimizedValueType | CLR_RT_DataTypeLookup::c_ValueType))
          {
          CLR_RT_HeapBlock *value = args->FixBoxingReference();
          FAULT_ON_NULL(value);

          if (value->DataType() == dtT)
            {
            // It's a boxed primitive/enum type.
            args->Assign(*value);
            }
          else if (args->Dereference()->ObjectCls().m_data == res.m_cls.m_data)
            {
            NANOCLR_CHECK_HRESULT(args->PerformUnboxing(inst));
            }
          else
            {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }
          }
        }

      if (res.m_dt == DATATYPE_VALUETYPE && res.m_fByRef == false)
        {
        if (args->IsAReferenceOfThisType(DATATYPE_VALUETYPE))
          {
          NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.CloneObject(*args, *args));
          }
        }
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_StackFrame::HandleSynchronized(bool fAcquire, bool fGlobal)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock refType;
  CLR_RT_HeapBlock *obj;
  CLR_RT_HeapBlock ref;
  CLR_RT_HeapBlock **ppGlobalLock;
  CLR_RT_HeapBlock *pGlobalLock;

  if (fGlobal)
    {
    obj = &ref;

    ppGlobalLock = &g_CLR_RT_ExecutionEngine.m_globalLock;

    pGlobalLock = *ppGlobalLock;

    if (pGlobalLock)
      {
      obj->SetObjectReference(pGlobalLock);
      }
    else
      {
      //
      // Create an private object to implement global locks.
      //
      NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(*obj, g_CLR_RT_WellKnownTypes.m_Object));

      *ppGlobalLock = obj->Dereference();
      }
    }
  else if (m_call.m_target->flags & CLR_RECORD_METHODDEF::MD_Static)
    {
    uint32_t idx = create_index(get_assembly_index(m_call.m_index), m_call.CrossReference().GetOwner());

    refType.SetReflection(idx);

    obj = &refType;
    }
  else
    {
    obj = &Arg0();
    }

  if (fAcquire)
    {
    NANOCLR_SET_AND_LEAVE(g_CLR_RT_ExecutionEngine.LockObject(*obj, m_owningSubThread, TIMEOUT_INFINITE, false));
    }
  else
    {
    NANOCLR_SET_AND_LEAVE(g_CLR_RT_ExecutionEngine.UnlockObject(*obj, m_owningSubThread));
    }

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_StackFrame::Pop()
  {
 

  if (m_owningThread->m_fHasJMCStepper || (m_flags & c_HasBreakpoint))
    {
    g_CLR_RT_ExecutionEngine.Breakpoint_StackFrame_Pop(this, false);
    }

  const uint32_t c_flagsToCheck = CLR_RT_StackFrame::c_CallOnPop | CLR_RT_StackFrame::c_Synchronized |
    CLR_RT_StackFrame::c_SynchronizedGlobally | CLR_RT_StackFrame::c_NativeProfiled;

  if (m_flags & c_flagsToCheck)
    {
    if (m_flags & CLR_RT_StackFrame::c_CallOnPop)
      {
      m_flags |= CLR_RT_StackFrame::c_CalledOnPop;

      if (m_nativeMethod)
        {
        (void)m_nativeMethod(*this);
        }
      }

    if (m_flags & CLR_RT_StackFrame::c_Synchronized)
      {
      m_flags &= ~CLR_RT_StackFrame::c_Synchronized;

      (void)HandleSynchronized(false, false);
      }

    if (m_flags & CLR_RT_StackFrame::c_SynchronizedGlobally)
      {
      m_flags &= ~CLR_RT_StackFrame::c_SynchronizedGlobally;

      (void)HandleSynchronized(false, true);
      }

#if defined(ENABLE_NATIVE_PROFILER)
    if (m_flags & CLR_RT_StackFrame::c_NativeProfiled)
      {
      m_owningThread->m_fNativeProfiled = false;
      m_flags &= ~CLR_RT_StackFrame::c_NativeProfiled;
      Native_Profiler_Stop();
      }
#endif
    }

  CLR_RT_StackFrame *caller = Caller();

  if (caller->Prev() != NULL)
    {
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    if (caller->m_flags & CLR_RT_StackFrame::c_HasBreakpoint)
      {
      g_CLR_RT_ExecutionEngine.Breakpoint_StackFrame_Step(caller, caller->m_IP);
      }
#endif

    //
    // Constructors are slightly different, they push the 'this' pointer back into the caller stack.
    //
    // This is to enable the special case for strings, where the object can be recreated by the constructor...
    //
    if (caller->m_flags & CLR_RT_StackFrame::c_ExecutingConstructor)
      {
      CLR_RT_HeapBlock &src = this->Arg0();
      CLR_RT_HeapBlock &dst = caller->PushValueAndAssign(src);

      dst.Promote();

      //
      // Undo the special "object -> reference" hack done by CEE_NEWOBJ.
      //
      if (dst.DataType() == DATATYPE_BYREF)
        {
        dst.ChangeDataType(DATATYPE_OBJECT);
        }

      caller->m_flags &= ~CLR_RT_StackFrame::c_ExecutingConstructor;

      _ASSERTE((m_flags & CLR_RT_StackFrame::c_AppDomainTransition) == 0);
      }
    else
      { // Note that ExecutingConstructor is checked on 'caller', whereas the other two flags are checked on 'this'
      const uint32_t c_moreFlagsToCheck =
        CLR_RT_StackFrame::c_PseudoStackFrameForFilter | CLR_RT_StackFrame::c_AppDomainTransition;

      if (m_flags & c_moreFlagsToCheck)
        {
        if (m_flags & CLR_RT_StackFrame::c_PseudoStackFrameForFilter)
          {
          // Do nothing here. Pushing return values onto stack frames that don't expect them are a bad idea.
          }
        }
      else //! c_moreFlagsToCheck
        {
        //
        // Push the return, if any.
        //
        if (m_call.m_target->retVal != DATATYPE_VOID)
          {
          if (m_owningThread->m_currentException.Dereference() == NULL)
            {
            CLR_RT_HeapBlock &src = this->TopValue();
            CLR_RT_HeapBlock &dst = caller->PushValueAndAssign(src);

            dst.Promote();
            }
          }
        }
      }
    }
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  else
    {
    int idx = m_owningThread->m_scratchPad;

    if (idx >= 0)
      {
      CLR_RT_HeapBlock_Array *array = g_CLR_RT_ExecutionEngine.m_scratchPadArray;

      if (array && array->m_numOfElements > (uint32_t)idx)
        {
        CLR_RT_HeapBlock *dst = (CLR_RT_HeapBlock *)array->GetElement((uint32_t)idx);
        CLR_RT_HeapBlock *exception = m_owningThread->m_currentException.Dereference();

        dst->SetObjectReference(NULL);

        if (exception != NULL)
          {
          dst->SetObjectReference(exception);
          }
        else if (m_call.m_target->retVal != DATATYPE_VOID)
          {
          CLR_RT_SignatureParser sig;
          sig.Initialize_MethodSignature(this->m_call.m_assm, this->m_call.m_target);
          CLR_RT_SignatureParser::Element res;
          CLR_RT_TypeDescriptor desc;

          dst->Assign(this->TopValue());

          // Perform boxing, if needed.

          // Box to the return value type
          _SIDE_ASSERTE(SUCCEEDED(sig.Advance(res)));
          _SIDE_ASSERTE(SUCCEEDED(desc.InitializeFromType(res.m_cls)));

          if (c_CLR_RT_DataTypeLookup[this->DataType()].m_flags &
            CLR_RT_DataTypeLookup::c_OptimizedValueType ||
            desc.m_handlerCls.m_target->IsEnum())
            {
            if (FAILED(dst->PerformBoxing(desc.m_handlerCls)))
              {
              dst->SetObjectReference(NULL);
              }
            }
          }
        }
      }
    }
#endif

  //
  // We could be jumping outside of a nested exception handler.
  //

  m_owningThread->PopEH(this, NULL);

  //
  // If this StackFrame owns a SubThread, kill it.
  //
  {
  CLR_RT_SubThread *sth = (CLR_RT_SubThread *)m_owningSubThread->Next();

  if (sth->Next() && sth->m_owningStackFrame == this)
    {
    CLR_RT_SubThread::DestroyInstance(sth->m_owningThread, sth, CLR_RT_SubThread::MODE_IncludeSelf);
    }
  }

  g_CLR_RT_EventCache.Append_Node(this);
  }

//--//

void CLR_RT_StackFrame::SetResult(int32_t val, CLR_DataType dataType)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val, dataType);
  }

void CLR_RT_StackFrame::SetResult_R4(float val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetFloat(val);
  }

void CLR_RT_StackFrame::SetResult_R8(double val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetDouble(val);
  }

void CLR_RT_StackFrame::SetResult_I1(uint8_t val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_I2(int16_t val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_I4(int32_t val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_I8(int64_t &val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_U1(int8_t val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_U2(uint16_t val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_U4(uint32_t val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_U8(uint64_t &val)

  {
  CLR_RT_HeapBlock &top = PushValue();

  top.SetInteger(val);
  }

void CLR_RT_StackFrame::SetResult_Boolean(bool val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetBoolean(val);
  }

void CLR_RT_StackFrame::SetResult_Object(CLR_RT_HeapBlock *val)
  {
 
  CLR_RT_HeapBlock &top = PushValue();

  top.SetObjectReference(val);
  }

HRESULT CLR_RT_StackFrame::SetResult_String(const char *val)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock &top = PushValue();

  NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(top, val));

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_StackFrame::ConvertResultToBoolean()
  {
 
  CLR_RT_HeapBlock &top = TopValue();

  top.SetBoolean(top.NumericByRef().s4 == 0);
  }

void CLR_RT_StackFrame::NegateResult()
  {
 
  CLR_RT_HeapBlock &top = TopValue();

  top.NumericByRef().s4 = top.NumericByRef().s4 ? 0 : 1;
  }

//--//

// input HeapBlock has timeout value **IN TICKS**
// sometimes you have to force a cast to (int64_t) otherwise the set operations will fail because of the var size
// mismatch
HRESULT CLR_RT_StackFrame::SetupTimeoutFromTicks(CLR_RT_HeapBlock &input, int64_t *&output)
  {
 
  HRESULT hr;

  if (m_customState == 0)
    {
    CLR_RT_HeapBlock &ref = PushValueAndClear();
    int64_t timeExpire;

    //
    // Initialize timeout and save it on the stack.
    //
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.InitTimeout(timeExpire, input.NumericByRef().s8));

    ref.SetInteger(timeExpire);

    m_customState = 1;
    }

  output = (int64_t *)&m_evalStack[0].NumericByRef().s8;

  NANOCLR_NOCLEANUP();
  }

//--//

void CLR_RT_StackFrame::Relocate()
  {
 

#ifndef CLR_NO_IL_INLINE
  if (m_inlineFrame)
    {
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_inlineFrame->m_frame.m_call.m_assm);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_inlineFrame->m_frame.m_call.m_target);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_inlineFrame->m_frame.m_IPStart);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_inlineFrame->m_frame.m_IP);

    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_call.m_assm);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_call.m_target);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_nativeMethod);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_IPstart);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_IP);

    CLR_RT_GarbageCollector::Heap_Relocate(
      m_inlineFrame->m_frame.m_args,
      m_inlineFrame->m_frame.m_call.m_target->numArgs);
    CLR_RT_GarbageCollector::Heap_Relocate(
      m_inlineFrame->m_frame.m_locals,
      m_inlineFrame->m_frame.m_call.m_target->numLocals);
    CLR_RT_GarbageCollector::Heap_Relocate(
      m_inlineFrame->m_frame.m_evalStack,
      (int)(m_evalStackPos - m_inlineFrame->m_frame.m_evalStack));
    CLR_RT_GarbageCollector::Heap_Relocate(m_locals, m_call.m_target->numLocals);
    }
  else
#endif
    {
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_call.m_assm);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_call.m_target);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_nativeMethod);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_IPstart);
    CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_IP);

    CLR_RT_GarbageCollector::Heap_Relocate(m_arguments, m_call.m_target->numArgs);
    CLR_RT_GarbageCollector::Heap_Relocate(m_locals, m_call.m_target->numLocals);
    CLR_RT_GarbageCollector::Heap_Relocate(m_evalStack, TopValuePosition());
    }
  }

//--//

HRESULT CLR_RT_StackFrame::NotImplementedStub()
  {
 
  HRESULT hr;

  if (m_call.m_target->retVal != DATATYPE_VOID)
    {
    SetResult_I4(0);
    }

  NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);

  NANOCLR_NOCLEANUP();
  }
