//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

#include "../../../ion_debugger.h"

static const int64_t c_MaximumTimeToActive = (TIME_CONVERSION__ONEMINUTE * TIME_CONVERSION__TO_SECONDS);
extern HRESULT ExceptionCreateInstance(CLR_RT_HeapBlock &ref, uint32_t cls, HRESULT hr, CLR_RT_StackFrame *stack);
//--//

CLR_RT_ExecutionEngine::ExecutionConstraintCompensation CLR_RT_ExecutionEngine::s_compensation = { 0, 0, 0 };

//--//

HRESULT CLR_RT_ExecutionEngine::CreateInstance()
  {
 
  HRESULT hr;

  NANOCLR_CLEAR(g_CLR_RT_ExecutionEngine);

  NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.ExecutionEngine_Initialize());

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::ExecutionEngine_Initialize()
  {
 
  HRESULT hr;

  m_maximumTimeToActive = c_MaximumTimeToActive; // int64_t                           m_maximumTimeToActive
                                                 // int                                 m_iDebugger_Conditions;
                                                 //

  // int64_t                           m_currentNextActivityTime;
  m_timerCache = false;                           // bool                                m_timerCache;
                                                  // int64_t                           m_timerCacheNextTimeout;
                                                  //
  m_heap.DblLinkedList_Initialize();              // CLR_RT_DblLinkedList                m_heap;
                                                  // CLR_RT_HeapCluster*                 m_lastHcUsed;
  m_heapState = c_HeapState_Normal;               // int                                 m_heapState;
                                                  //
  m_weakReferences.DblLinkedList_Initialize();    // CLR_RT_DblLinkedList                m_weakReferences;
                                                  //
  m_timers.DblLinkedList_Initialize();            // CLR_RT_DblLinkedList                m_timers;
  m_raisedEvents = 0;                             // uint32_t                          m_raisedEvents;
                                                  //
  m_threadsReady.DblLinkedList_Initialize();      // CLR_RT_DblLinkedList                m_threadsReady;
  m_threadsWaiting.DblLinkedList_Initialize();    // CLR_RT_DblLinkedList                m_threadsWaiting;
  m_threadsZombie.DblLinkedList_Initialize();     // CLR_RT_DblLinkedList                m_threadsZombie;
                                                  // int                                 m_lastPid;
                                                  //
  m_finalizersAlive.DblLinkedList_Initialize();   // CLR_RT_DblLinkedList                m_finalizersAlive;
  m_finalizersPending.DblLinkedList_Initialize(); // CLR_RT_DblLinkedList                m_finalizersPending;
                                                  // CLR_RT_Thread*                      m_finalizerThread;
                                                  // CLR_RT_Thread*                      m_cctorThread;
                                                  //
  m_globalLock = NULL;           // CLR_RT_HeapBlock*                   m_globalLock;
  m_outOfMemoryException = NULL; // CLR_RT_HeapBlock*                   m_outOfMemoryException;

  m_currentUICulture = NULL; // CLR_RT_HeapBlock*                   m_currentUICulture;

  // CLR_RT_HeapBlock_EndPoint::HandlerMethod_Initialize();
  // CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_Initialize();

  m_canFlyEventThread = NULL; // CLR_RT_Thread                       m_interruptThread;

  m_scratchPadArray = NULL; // CLR_RT_HeapBlock_Array*             m_scratchPadArray;

  m_currentThread = NULL;

  m_GlobalExecutionCounter = 0;

#if !defined(BUILD_RTM)
  m_fShuttingDown = false; // bool                                m_fShuttingDown;
#endif

  NANOCLR_CHECK_HRESULT(AllocateHeaps());

  g_CLR_RT_TypeSystem.TypeSystem_Initialize();
  g_CLR_RT_EventCache.EventCache_Initialize();

  m_startTime = HAL_Time_CurrentTime();

  CLR_RT_HeapBlock_WeakReference::RecoverObjects(m_heap);

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::AllocateHeaps()
  {
 
  HRESULT hr;

  const uint32_t c_HeapClusterSize = sizeof(CLR_RT_HeapBlock) * CLR_RT_HeapBlock::HB_MaxSize;

  uint8_t *heapFirstFree = s_CLR_RT_Heap.m_location;
  uint32_t heapFree = s_CLR_RT_Heap.m_size;
  int32_t i = 0;
  uint32_t blockSize = 1;

  if (heapFree <= sizeof(CLR_RT_HeapCluster))
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

  while (heapFree > sizeof(CLR_RT_HeapCluster))
    {
    CLR_RT_HeapCluster *hc = (CLR_RT_HeapCluster *)heapFirstFree;
    uint32_t size = (heapFree < c_HeapClusterSize) ? heapFree : c_HeapClusterSize;

#if NANOCLR_VALIDATE_HEAP >= NANOCLR_VALIDATE_HEAP_1_HeapBlocksAndUnlink

    trace_debug("Heap Cluster information\r\n");
    trace_debug("Start:       %08x\r\n", (size_t)heapFirstFree);
    trace_debug("Free:        %08x\r\n", (size_t)heapFree);
    trace_debug("Block size:  %d\r\n", sizeof(CLR_RT_HeapBlock));

#endif
    ///
    /// Speed up heap initialization for devices with very large heaps > 1MB
    /// Exponentially increase the size of a default heap block
    ///
    if (i > 100 * 1024 * 1024)
      {
      blockSize = CLR_RT_HeapBlock::HB_MaxSize;
      }
    else if (i > 10 * 1024 * 1024)
      {
      blockSize = 10 * 1024;
      }
    else if (i > 1024 * 1024)
      {
      blockSize = 1 * 1024;
      }

    hc->HeapCluster_Initialize(size, blockSize);

    m_heap.LinkAtBack(hc);

    heapFirstFree += size;
    heapFree -= size;
    i += size;
    }

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::DeleteInstance()
  {
 
  HRESULT hr;

  g_CLR_RT_ExecutionEngine.ExecutionEngine_Cleanup();

  NANOCLR_NOCLEANUP_NOLABEL();
  }

void CLR_RT_ExecutionEngine::ExecutionEngine_Cleanup()
  {
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // developer notes:
  // Most of the following calls are just for pure ceremony and gracefully terminating stuff,
  // cleaning collections and such.
  // In particular the previous existing calls to Abort threads and ReleaseAllThreads is completely irrelevant
  // because the execution engine wasn't running anymore. Whatever code that is on those threads
  // there to be executed wouldn't never be executed anyways.
  /////////////////////////////////////////////////////////////////////////////////////////////////

 
  m_fShuttingDown = true;

  m_scratchPadArray = NULL;
  m_breakpointsNum = 0;

  m_finalizersAlive.DblLinkedList_PushToCache();
  m_finalizersPending.DblLinkedList_PushToCache();
  m_finalizerThread = NULL;
  m_cctorThread = NULL;
  m_timerThread = NULL;

  g_CLR_RT_TypeSystem.TypeSystem_Cleanup();
  g_CLR_RT_EventCache.EventCache_Cleanup();

  m_globalLock = NULL;

  // CLR_RT_HeapBlock_EndPoint::HandlerMethod_CleanUp();
  // CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_CleanUp();

  m_canFlyEventThread = NULL;

  m_heap.DblLinkedList_Initialize();
  }

void CLR_RT_ExecutionEngine::Reboot(bool fHard)
  {
  if (fHard)
    {
    debug_break();
    }
  else
    {
    CLR_EE_DBG_SET(RebootPending);
    }
  }

int64_t CLR_RT_ExecutionEngine::GetUptime()
  {
  return HAL_Time_CurrentTime() - g_CLR_RT_ExecutionEngine.m_startTime;
  }

void CLR_RT_ExecutionEngine::JoinAllThreadsAndExecuteFinalizer()
  {
  }

void CLR_RT_ExecutionEngine::LoadDownloadedAssemblies()
  {
 
  PerformGarbageCollection();
  PerformHeapCompaction();

  //
  // Load any patch or similar!
  //
  NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_WeakReference, weak, m_weakReferences)
    {
    if ((weak->m_identity.m_flags & CLR_RT_HeapBlock_WeakReference::WR_ArrayOfBytes) != 0 &&
      weak->m_targetSerialized)
      {
      CLR_RECORD_ASSEMBLY *header;

      header = (CLR_RECORD_ASSEMBLY *)weak->m_targetSerialized->GetFirstElement();

      if (header->GoodAssembly())
        {
        CLR_RT_Assembly *assm = NULL;

        if (SUCCEEDED(CLR_RT_Assembly::CreateInstance(header, assm)))
          {
          assm->m_pFile = weak->m_targetSerialized;

          g_CLR_RT_TypeSystem.Link(assm);
          }
        }
      }
    }
  NANOCLR_FOREACH_NODE_END();

  (void)g_CLR_RT_TypeSystem.ResolveAll();

  NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
    if (pASSM->m_pFile)
      {
      //
      // For those assemblies that failed to load (missing dependency?), clean up.
      //
      if ((pASSM->m_flags & CLR_RT_Assembly::ResolutionCompleted) == 0)
        {
        pASSM->m_pFile = NULL;

        pASSM->DestroyInstance();
        }
      }
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  g_CLR_RT_TypeSystem.PrepareForExecution();
  }

//--//

void CLR_RT_ExecutionEngine::ExecutionConstraint_Suspend()
  {
 
  s_compensation.Suspend();
  }

void CLR_RT_ExecutionEngine::ExecutionConstraint_Resume()
  {
 
  s_compensation.Resume();
  }

//--//

uint32_t CLR_RT_ExecutionEngine::PerformGarbageCollection()
  {
 
  m_heapState = c_HeapState_UnderGC;

  uint32_t freeMem = g_CLR_RT_GarbageCollector.ExecuteGarbageCollection();

  m_heapState = c_HeapState_Normal;

  m_lastHcUsed = NULL;

#if !defined(BUILD_RTM) || defined(_WIN32)
  if (m_fPerformHeapCompaction)
    CLR_EE_SET(Compaction_Pending);
#endif

  g_CLR_RT_ExecutionEngine.SpawnFinalizer();

  return freeMem;
  }

void CLR_RT_ExecutionEngine::PerformHeapCompaction()
  {
 
  if (CLR_EE_DBG_IS(NoCompaction))
    return;

  g_CLR_RT_GarbageCollector.ExecuteCompaction();

  CLR_EE_CLR(Compaction_Pending);

  m_lastHcUsed = NULL;
  }

void CLR_RT_ExecutionEngine::Relocate()
  {
 
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_scratchPadArray);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_globalLock);
  // CLR_RT_GarbageCollector::Heap_Relocate( (void**)&m_outOfMemoryException );

  CLR_RT_GarbageCollector::Heap_Relocate((void **)&m_currentUICulture);

  m_weakReferences.Relocate();
  }

//--//
HRESULT CLR_RT_ExecutionEngine::WaitForDebugger()
  {
 
  HRESULT hr;

  while (CLR_EE_DBG_IS(Stopped) && !CLR_EE_DBG_IS(RebootPending) && !CLR_EE_DBG_IS(ExitPending))
    {
    // this will fail if there is no debugger attached
    hr = debug_break();
    }

#if defined(WIN32)
  NANOCLR_NOCLEANUP();
#else
  NANOCLR_NOCLEANUP_NOLABEL();
#endif
  }

#if defined(WIN32)
HRESULT CLR_RT_ExecutionEngine::CreateEntryPointArgs(CLR_RT_HeapBlock &argsBlk, wchar_t *szCommandLineArgs)
  {
 
  HRESULT hr;

  std::list<std::wstring> args;

  wchar_t *szArgNext = NULL;
  wchar_t *szArg = szCommandLineArgs;
  const wchar_t *sep = L" ";
  wchar_t *context = NULL;

  szArg = wcstok_s(szArg, sep, &context);

  while (szArg != NULL)
    {
    std::wstring arg = szArg;
    args.insert(args.end(), arg);

    szArg = wcstok_s(NULL, sep, &context);
    }

  if (FAILED(hr = CLR_RT_HeapBlock_Array::CreateInstance(argsBlk, (uint32_t)args.size(), g_CLR_RT_WellKnownTypes.m_String)))
    return hr;

  CLR_RT_HeapBlock_Array *array = argsBlk.Array();
  uint32_t iArg = 0;

  for (std::list<std::wstring>::iterator it = args.begin(); it != args.end(); it++, iArg++)
    {
    std::string arg;

    CLR_RT_HeapBlock *blk = (CLR_RT_HeapBlock *)array->GetElement(iArg);
    CLR_RT_UnicodeHelper::ConvertToUTF8((*it).c_str(), arg);

    if (FAILED(hr = CLR_RT_HeapBlock_String::CreateInstance(*blk, arg.c_str())))
      return hr;
    }

  NANOCLR_NOCLEANUP();
  }

#endif

HRESULT CLR_RT_ExecutionEngine::Execute(wchar_t *entryPointArgs, int maxContextSwitch)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock ref;
  CLR_RT_Thread *thMain = NULL;

  if (NANOCLR_INDEX_IS_INVALID(g_CLR_RT_TypeSystem.m_entryPoint))
    {
#if !defined(BUILD_RTM) || defined(_WIN32)
    trace_debug("Cannot find any entrypoint!\r\n");
#endif
    NANOCLR_SET_AND_LEAVE(CLR_E_ENTRYPOINT_NOT_FOUND);
    }

  NANOCLR_CHECK_HRESULT(WaitForDebugger());

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
  CLR_EE_DBG_SET_MASK(StateProgramRunning, StateMask);
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Delegate::CreateInstance(ref, g_CLR_RT_TypeSystem.m_entryPoint, NULL));

  {
  CLR_RT_ProtectFromGC gc(ref);

  NANOCLR_CHECK_HRESULT(NewThread(thMain, ref.DereferenceDelegate(), ThreadPriority_Normal, -1));
  }

  // To debug static constructors, the thread should be created after the entrypoint thread.
  NANOCLR_CHECK_HRESULT(WaitForDebugger());

  // m_cctorThread is NULL before call and inialized by the SpawnStaticConstructor
  SpawnStaticConstructor(m_cctorThread);

  while (true)
    {
    HRESULT hr2 = ScheduleThreads(maxContextSwitch);
    NANOCLR_CHECK_HRESULT(hr2);

    if (CLR_EE_DBG_IS(RebootPending) || CLR_EE_DBG_IS(ExitPending))
      {
      NANOCLR_SET_AND_LEAVE(S_FALSE);
      }

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    if (CLR_EE_DBG_IS(Stopped))
      {
      CLR_RT_ExecutionEngine::ExecutionConstraint_Suspend();

      NANOCLR_CHECK_HRESULT(WaitForDebugger());

      CLR_RT_ExecutionEngine::ExecutionConstraint_Resume();
      }
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    if (CLR_EE_IS(Compaction_Pending))
      {
      PerformHeapCompaction();
      _ASSERTE(FIMPLIES(CLR_EE_DBG_IS_NOT(NoCompaction), CLR_EE_IS_NOT(Compaction_Pending)));
      }

    if (hr2 == CLR_S_NO_READY_THREADS)
      {
      WaitForActivity();
      }
    else if (hr2 == CLR_S_QUANTUM_EXPIRED)
      {
#if !defined(BUILD_RTM) || defined(_WIN32)
      if (m_fPerformGarbageCollection)
        {
#if defined(NANOCLR_GC_VERBOSE)
        if (s_CLR_RT_fTrace_GC >= c_CLR_RT_Trace_Info)
          {
          trace_debug("    Memory: Forcing GC.\r\n");
          }
#endif
        PerformGarbageCollection();

#if defined(NANOCLR_GC_VERBOSE)
        if (s_CLR_RT_fTrace_Memory > c_CLR_RT_Trace_Info)
          {
          uint32_t inUse = g_CLR_RT_GarbageCollector.m_totalBytes - g_CLR_RT_GarbageCollector.m_freeBytes;

          trace_debug("    Memory: INUSE: %d\r\n", (int)inUse);
          }
#endif
        }
#endif
      }
    else
      {
      break;
      }
    }

  NANOCLR_CLEANUP();

#if defined(WIN32)

  // By skipping the whole CLRStartup routine, the Monitor_Program_Exit message never gets sent to clients.
  debug_program_exit(0);

  WaitForDebugger();
#endif

  NANOCLR_CLEANUP_END();
  }

bool CLR_RT_ExecutionEngine::EnsureSystemThread(CLR_RT_Thread *&thread, int priority)
  {
 

  if (thread == NULL)
    {
    return SUCCEEDED(NewThread(thread, NULL, priority, -1, CLR_RT_Thread::TH_F_System));
    }
  else
    {
    return thread->CanThreadBeReused();
    }
  }

void CLR_RT_ExecutionEngine::SpawnTimer()
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Timer, timer, m_timers)
    {
    if (timer->m_flags & CLR_RT_HeapBlock_Timer::c_Triggered)
      {
      if (EnsureSystemThread(m_timerThread, ThreadPriority_Normal))
        {
        // only fire one timer at a time

        timer->SpawnTimer(m_timerThread);

        // put at the back of the queue to allow for fairness of the timers.
        m_timers.LinkAtBack(timer);
        }
      break;
      }
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CLR_RT_ExecutionEngine::StaticConstructorTerminationCallback(void *arg)
  {
  (void)arg;

 
  g_CLR_RT_ExecutionEngine.SpawnStaticConstructor(g_CLR_RT_ExecutionEngine.m_cctorThread);
  }

bool CLR_RT_ExecutionEngine::SpawnStaticConstructorHelper(CLR_RT_Assembly *assembly, uint32_t idx)
  {
 
  uint32_t idxNext;

  _ASSERTE(m_cctorThread != NULL);
  _ASSERTE(m_cctorThread->CanThreadBeReused());

  idxNext = idx;

  _ASSERTE(assembly != NULL);

  // find next method with static constructor
  if (assembly->FindNextStaticConstructor(idxNext))
    {
    CLR_RT_HeapBlock_Delegate *dlg;
    CLR_RT_HeapBlock refDlg;
    refDlg.SetObjectReference(NULL);
    CLR_RT_ProtectFromGC gc(refDlg);

    if (SUCCEEDED(CLR_RT_HeapBlock_Delegate::CreateInstance(refDlg, idxNext, NULL)))
      {
      dlg = refDlg.DereferenceDelegate();

      if (SUCCEEDED(m_cctorThread->PushThreadProcDelegate(dlg)))
        {
        m_cctorThread->m_terminationCallback = CLR_RT_ExecutionEngine::StaticConstructorTerminationCallback;

        return true;
        }
      }
    }

  assembly->m_flags |= CLR_RT_Assembly::StaticConstructorsExecuted;
  return false;
  }

void CLR_RT_ExecutionEngine::SpawnStaticConstructor(CLR_RT_Thread *&pCctorThread)
  {
 
  CLR_RT_HeapBlock_Delegate *dlg = NULL;

  if (!EnsureSystemThread(pCctorThread, (ThreadPriority)ThreadPriority_Highest +1))
    return;

  dlg = pCctorThread->m_dlg;

  if (dlg != NULL)
    {
    uint32_t idx = dlg->DelegateFtn();
    CLR_RT_MethodDef_Instance inst;

    // Find next static constructor for given idx
    _ASSERTE(NANOCLR_INDEX_IS_VALID(idx));
    _SIDE_ASSERTE(inst.InitializeFromIndex(idx));

    // This is ok if idx is no longer valid.  SpawnStaticConstructorHelper will call FindNextStaticConstructor
    // which will fail
    idx++;

    if (SpawnStaticConstructorHelper(inst.m_assm, idx))
      return;
    }

  // first, find the AppDomainAssembly to run. (what about appdomains!!!)
  NANOCLR_FOREACH_ASSEMBLY(g_CLR_RT_TypeSystem)
    {
    // Find an AppDomainAssembly that does not have it's static constructor bit set...
    if ((pASSM->m_flags & CLR_RT_Assembly::StaticConstructorsExecuted) == 0)
      {
      uint32_t idx;
      idx = create_index(pASSM->m_idx, 0);
      bool fDepedenciesRun = true;

      // Check that all dependent assemblies have had static constructors run.
      CLR_RT_AssemblyRef_CrossReference *ar = pASSM->m_pCrossReference_AssemblyRef;
      for (int i = 0; i < pASSM->m_pTablesSize[TBL_AssemblyRef]; i++, ar++)
        {
        if ((ar->m_target->m_flags & CLR_RT_Assembly::StaticConstructorsExecuted) == 0)
          {
          fDepedenciesRun = true;
          break;
          }
        }

      if (fDepedenciesRun && SpawnStaticConstructorHelper(pASSM, idx))
        return;
      }
    }
  NANOCLR_FOREACH_ASSEMBLY_END();

  // no more static constructors needed...
  pCctorThread->DestroyInstance();
  }

void CLR_RT_ExecutionEngine::FinalizerTerminationCallback(void *arg)
  {
  (void)arg;

 
  g_CLR_RT_ExecutionEngine.SpawnFinalizer();
  }

void CLR_RT_ExecutionEngine::SpawnFinalizer()
  {
 

  CLR_RT_HeapBlock_Finalizer *fin = (CLR_RT_HeapBlock_Finalizer *)m_finalizersPending.FirstNode();
  if (fin->Next() != NULL)
    {
    CLR_RT_HeapBlock delegate;
    delegate.SetObjectReference(NULL);
    CLR_RT_ProtectFromGC gc(delegate);

    if (EnsureSystemThread(m_finalizerThread, ThreadPriority_BelowNormal))
      {
      if (SUCCEEDED(CLR_RT_HeapBlock_Delegate::CreateInstance(delegate, fin->m_md, NULL)))
        {
        CLR_RT_HeapBlock_Delegate *dlg = delegate.DereferenceDelegate();

        dlg->m_object.SetObjectReference(fin->m_object);

        if (SUCCEEDED(m_finalizerThread->PushThreadProcDelegate(dlg)))
          {
          g_CLR_RT_EventCache.Append_Node(fin);
          m_finalizerThread->m_terminationCallback = CLR_RT_ExecutionEngine::FinalizerTerminationCallback;
          }
        }
      }
    }
  }

void CLR_RT_ExecutionEngine::AdjustExecutionCounter(CLR_RT_DblLinkedList &threadList, int iUpdateValue)

  { // Iterate over threads in increase executioin counter by iUpdateValue
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, pThread, threadList)
    {
    pThread->m_executionCounter += iUpdateValue;
    // Update m_executionCounter if thread is too behind of m_GlobalExecutionCounter
    pThread->BringExecCounterToDate(m_GlobalExecutionCounter);
    }
  NANOCLR_FOREACH_NODE_END()
  }

void CLR_RT_ExecutionEngine::UpdateToLowestExecutionCounter(CLR_RT_Thread *pThread) const
  {
  // Set the m_executionCounter in thread to lowest value among ready threads.
  // Thus it will be executed last.
  pThread->m_executionCounter = m_GlobalExecutionCounter - 1;
  }

void CLR_RT_ExecutionEngine::RetrieveCurrentMethod(uint32_t &assmIdx, uint32_t &methodIdx)
  {
  assmIdx = 0;
  methodIdx = 0;

  if (m_currentThread != NULL)
    {
    CLR_RT_StackFrame *stack = m_currentThread->CurrentFrame();

    if (stack)
      {
      assmIdx = get_assembly_index(stack->m_call.m_index);
      methodIdx = get_method_index(stack->m_call.m_index);
      }
    }
  }

void CLR_RetrieveCurrentMethod(uint32_t &assmIdx, uint32_t &methodIdx)
  {
  g_CLR_RT_ExecutionEngine.RetrieveCurrentMethod(assmIdx, methodIdx);
  }

void CLR_SoftReboot()
  {
  CLR_EE_DBG_SET(RebootPending);
  }

void CLR_DebuggerBreak()
  {
  if (g_CLR_RT_ExecutionEngine.m_currentThread != NULL)
    {
    CLR_RT_HeapBlock *obj = g_CLR_RT_ExecutionEngine.m_currentThread->m_currentException.Dereference();

    ///
    /// Only inject the exception once -- if the dereference is not null then the exception is already set on the
    /// current thread
    ///
    if (obj == NULL)
      {
      ExceptionCreateInstance(        g_CLR_RT_ExecutionEngine.m_currentThread->m_currentException,
        g_CLR_RT_WellKnownTypes.m_WatchdogException,
        CLR_E_WATCHDOG_TIMEOUT,
        g_CLR_RT_ExecutionEngine.m_currentThread->CurrentFrame());
      }
    }
  }

#define EXECUTION_COUNTER_MAXIMUM 0x40000000
#define EXECUTION_COUNTER_ADJUSTMENT 0x00000001

HRESULT CLR_RT_ExecutionEngine::ScheduleThreads(int maxContextSwitch)
  {
   HRESULT hr;

  // We run threads based on the m_executionCounter.
  // The thread with highest m_executionCounter is chosen for execution.
  // The highest value of m_executionCounter of any thread is stored in m_GlobalExecutionCounter
  // We need to check that m_GlobalExecutionCounter does not underflow ( runs below - -2147483647 ) This would be very
  // rare condition, but it may happen. We put threshold at - 0x40000000
  if (m_GlobalExecutionCounter < -EXECUTION_COUNTER_MAXIMUM)
    { // Iterate over threads in all queues and bump the execution counter by MAX_EXECUTION_COUNTER_ADJUSTMENT

    m_GlobalExecutionCounter += EXECUTION_COUNTER_ADJUSTMENT;

    // For each list of threads runs over it and updates execution counter.
    // AdjustExecutionCounter gets const & to list of threads.
    // List of threads is not modified, but m_executionCounter is bumped up in each thread.

    AdjustExecutionCounter(m_threadsReady, EXECUTION_COUNTER_ADJUSTMENT);
    AdjustExecutionCounter(m_threadsWaiting, EXECUTION_COUNTER_ADJUSTMENT);
    AdjustExecutionCounter(m_threadsZombie, EXECUTION_COUNTER_ADJUSTMENT);
    }

  while (maxContextSwitch-- > 0)
    {
    if (CLR_EE_DBG_IS(Stopped))
      {
      NANOCLR_SET_AND_LEAVE(CLR_S_NO_READY_THREADS);
      }

    CLR_RT_Thread *th = NULL;

    //  If a static constructor thread exists, we should be running it.
    //  What about func-eval?
    if (m_cctorThread == NULL)
      {
      // This is normal case execution. Looks for first ready thread.
      th = (CLR_RT_Thread *)m_threadsReady.FirstNode();
      }
    else // If a static constructor thread exists, we should be running it.
      {
      //  This is special case executed during initialization of static constructors.
      if (m_cctorThread->m_status == CLR_RT_Thread::TH_S_Ready &&
        !(m_cctorThread->m_flags & CLR_RT_Thread::TH_F_Suspended))
        {
        th = m_cctorThread;
        }
      else
        {
        // The m_cctorThread is exists, but not ready - means entered blocking call.
        // We do not want to preempt constructor thread, so stay idle.
        NANOCLR_SET_AND_LEAVE(CLR_S_NO_READY_THREADS);
        }
      }

    // If th->Next() is NULL, then there are no Ready to run threads in the system.
    // In this case we spawn finalizer and make finalizer thread as ready one.
    if (th->Next() == NULL)
      {
      g_CLR_RT_ExecutionEngine.SpawnFinalizer();

      // Now finalizer thread might be in ready state if there are object that need call to finalizer.
      // th might point to finilizer thread.
      th = (CLR_RT_Thread *)m_threadsReady.FirstNode();

      // Thread create can cause stopping debugging event
#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
      if (CLR_EE_DBG_IS(Stopped))
        {
        NANOCLR_SET_AND_LEAVE(CLR_S_NO_READY_THREADS);
        }
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
      }

    // If there is ready thread - decrease m_executionCounter for this (th) thread.
    if (th->Next() != NULL)
      {
      // The value to update m_executionCounter for each run. See comment for GetQuantumDebit for possible values
      int debitForEachRun = th->GetQuantumDebit();

      // If thread is way too much behind on its execution, we cut off extra credit.
      // We guarantee the thread will not be scheduled more than 4 consecutive times.
      th->BringExecCounterToDate(m_GlobalExecutionCounter);

      // Subtract the execution counter by debit value ( for executing thread )
      th->m_executionCounter -= debitForEachRun;

      // Keep the track of lowest execution counter.
      if (th->m_executionCounter < m_GlobalExecutionCounter)
        {
        m_GlobalExecutionCounter = th->m_executionCounter;
        }
      }
    else
      {
      if (m_threadsWaiting.IsEmpty())
        {
        NANOCLR_SET_AND_LEAVE(CLR_S_NO_THREADS);
        }

      NANOCLR_SET_AND_LEAVE(CLR_S_NO_READY_THREADS);
      }

    Watchdog_Reset();

    {
    // Runs the tread until expiration of its quantum or until thread is blocked.
    hr = th->Execute();
    }

    if (FAILED(hr))
      {
      switch (hr)
        {
        case CLR_E_RESCHEDULE:
          break;

        case CLR_E_THREAD_WAITING:
          th->m_status = CLR_RT_Thread::TH_S_Waiting;
          break;

        default:
          th->m_status = CLR_RT_Thread::TH_S_Terminated;
          break;
        }
      }

    Watchdog_Reset();

    PutInProperList(th);

    (void)ProcessTimer();
    }

  NANOCLR_SET_AND_LEAVE(CLR_S_QUANTUM_EXPIRED);

  NANOCLR_CLEANUP();

  NANOCLR_CLEANUP_END();
  }

uint32_t CLR_RT_ExecutionEngine::WaitForActivity()
  {
 

  int64_t timeoutMin = ProcessTimer();

  if (m_threadsReady.IsEmpty() == false)
    return 0; // Someone woke up...

  if (timeoutMin > 0LL)
    {
    NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, m_threadsWaiting)
      {
      if ((th->m_waitForEvents & Event_IdleCPU) != 0 && th->m_waitForEvents_IdleTimeWorkItem < timeoutMin)
        {
        th->m_waitForEvents_IdleTimeWorkItem = TIMEOUT_ZERO;

        th->Restart(true);

        return SYSTEM_EVENT_FLAG_ALL; // Someone woke up...
        }
      }
    NANOCLR_FOREACH_NODE_END();

    // return WaitForActivity(SLEEP_LEVEL__SLEEP, g_CLR_HW_Hardware.m_wakeupEvents, timeoutMin);
    }

  return 0;
  }

uint32_t CLR_RT_ExecutionEngine::WaitForActivity(uint32_t powerLevel, uint32_t events, int64_t timeout_ms)
  {
 

  return WaitSystemEvents(powerLevel, events, timeout_ms);

  /*
  if (powerLevel != PowerLevel__Active)
    {
    return WaitSystemEvents(powerLevel, events, timeout_ms);
    }

  return 0;
  */
  }

//--//

void CLR_RT_ExecutionEngine::PutInProperList(CLR_RT_Thread *th)
  {
 
  switch (th->m_status)
    {
    case CLR_RT_Thread::TH_S_Ready:
      if ((th->m_flags & CLR_RT_Thread::TH_F_Suspended) == 0)
        {
        InsertThreadRoundRobin(m_threadsReady, th);
        break;
        }
      //
      // Fall-through...
      //
    case CLR_RT_Thread::TH_S_Waiting:
      m_threadsWaiting.LinkAtBack(th);
      break;

    case CLR_RT_Thread::TH_S_Terminated:
      th->Passivate();
      break;

    case CLR_RT_Thread::TH_S_Unstarted:
      m_threadsZombie.LinkAtFront(th);
      break;
    }
  }

void CLR_RT_ExecutionEngine::AbortAllThreads(CLR_RT_DblLinkedList &threads)
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, threads)
    {
    if ((th->m_flags & CLR_RT_Thread::TH_F_Aborted) == 0)
      {
      th->Abort();

      NANOCLR_FOREACH_NODE_RESTART(CLR_RT_Thread, th, threads);
      }
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CLR_RT_ExecutionEngine::ReleaseAllThreads(CLR_RT_DblLinkedList &threads)
  {
 
  while (true)
    {
    CLR_RT_Thread *th = (CLR_RT_Thread *)threads.ExtractFirstNode();
    if (!th)
      break;

    th->DestroyInstance();
    }
  }

void CLR_RT_ExecutionEngine::InsertThreadRoundRobin(CLR_RT_DblLinkedList &threads, CLR_RT_Thread *thTarget)
  {
 
  CLR_RT_Thread *th;

  thTarget->Unlink();

  if (threads.IsEmpty())
    {
    th = (CLR_RT_Thread *)threads.Tail();
    }
  else
    {
    int priTarget = thTarget->GetExecutionCounter();

    NANOCLR_FOREACH_NODE__NODECL(CLR_RT_Thread, th, threads)
      {
      if (th->GetExecutionCounter() < priTarget)
        break;
      }
    NANOCLR_FOREACH_NODE_END();
    }

  thTarget->m_waitForEvents = 0;
  thTarget->m_waitForEvents_Timeout = TIMEOUT_INFINITE;

  if (thTarget->m_waitForObject != NULL)
    {
    g_CLR_RT_EventCache.Append_Node(thTarget->m_waitForObject);

    thTarget->m_waitForObject = NULL;
    }

  threads.InsertBeforeNode(th, thTarget);
  }

//--//

HRESULT CLR_RT_ExecutionEngine::NewThread(
  CLR_RT_Thread *&thRes,
  CLR_RT_HeapBlock_Delegate *pDelegate,
  int priority,
  int32_t id,
  uint32_t flags)
  {
 
  HRESULT hr;

  NANOCLR_CHECK_HRESULT(
    CLR_RT_Thread::CreateInstance(id != -1 ? id : ++m_lastPid, pDelegate, priority, thRes, flags));

  PutInProperList(thRes);

  NANOCLR_CLEANUP();

  if (FAILED(hr))
    {
    if (thRes)
      {
      thRes->DestroyInstance();

      thRes = NULL;
      }
    }

  NANOCLR_CLEANUP_END();
  }

int32_t CLR_RT_ExecutionEngine::GetNextThreadId()
  {
  return ++m_lastPid;
  }

//--//

CLR_RT_HeapBlock *CLR_RT_ExecutionEngine::ExtractHeapBlocksForArray(
  CLR_RT_TypeDef_Instance &inst,
  uint32_t length,
  const CLR_RT_ReflectionDef_Index &reflex)
  {
 
  CLR_DataType dt = (CLR_DataType)inst.m_target->dataType;
  const CLR_RT_DataTypeLookup &dtl = c_CLR_RT_DataTypeLookup[dt];

  uint32_t totLength = (uint32_t)(sizeof(CLR_RT_HeapBlock_Array) + length * dtl.m_sizeInBytes);
  uint32_t lengthHB = CONVERTFROMSIZETOHEAPBLOCKS(totLength);

  if (lengthHB > CLR_RT_HeapBlock::HB_MaxSize)
    return NULL;

  CLR_RT_HeapBlock_Array *pArray = (CLR_RT_HeapBlock_Array *)ExtractHeapBlocks(m_heap, DATATYPE_SZARRAY, 0, lengthHB);

  if (pArray)
    {
    pArray->ReflectionData() = reflex;
    pArray->m_numOfElements = length;

    pArray->m_typeOfElement = dt;
    pArray->m_sizeOfElement = dtl.m_sizeInBytes;
    pArray->m_fReference = (dtl.m_flags & CLR_RT_DataTypeLookup::c_Numeric) == 0;
    }

  return pArray;
  }

CLR_RT_HeapBlock *CLR_RT_ExecutionEngine::ExtractHeapBlocksForClassOrValueTypes(
  uint32_t dataType,
  uint32_t flags,
  uint32_t cls,
  uint32_t length)
  {
 
  if (length > CLR_RT_HeapBlock::HB_MaxSize)
    return NULL;

  _ASSERTE(dataType == DATATYPE_CLASS || dataType == DATATYPE_VALUETYPE);

  flags = flags | CLR_RT_HeapBlock::HB_InitializeToZero;
  CLR_RT_HeapBlock *hb = ExtractHeapBlocks(m_heap, dataType, flags, length);

  if (hb)
    {
    hb->SetObjectCls(cls);
    }

  return hb;
  }

CLR_RT_HeapBlock *CLR_RT_ExecutionEngine::ExtractHeapBytesForObjects(
  uint32_t dataType,
  uint32_t flags,
  uint32_t length)
  {
 
  return ExtractHeapBlocksForObjects(dataType, flags, CONVERTFROMSIZETOHEAPBLOCKS(length));
  }

CLR_RT_HeapBlock *CLR_RT_ExecutionEngine::ExtractHeapBlocksForObjects(
  uint32_t dataType,
  uint32_t flags,
  uint32_t length)
  {
 
  if (length > CLR_RT_HeapBlock::HB_MaxSize)
    return NULL;

  _ASSERTE(dataType != DATATYPE_CLASS && dataType != DATATYPE_VALUETYPE && dataType != DATATYPE_SZARRAY);

  flags &= ~CLR_RT_HeapBlock::HB_Alive;

  CLR_RT_HeapBlock *hb = ExtractHeapBlocks(m_heap, dataType, flags, length);

  return hb;
  }

//--//

CLR_RT_HeapBlock_Node *CLR_RT_ExecutionEngine::ExtractHeapBlocksForEvents(
  uint32_t dataType,
  uint32_t flags,
  uint32_t length)
  {
 
  if (length > CLR_RT_HeapBlock::HB_MaxSize)
    return NULL;

  flags |= CLR_RT_HeapBlock::HB_Alive | CLR_RT_HeapBlock::HB_Event;

  CLR_RT_HeapBlock_Node *hb = (CLR_RT_HeapBlock_Node *)ExtractHeapBlocks(m_heap, dataType, flags, length);

  if (hb)
    {
    hb->GenericNode_Initialize();
    }

  return hb;
  }

CLR_RT_HeapBlock *CLR_RT_ExecutionEngine::ExtractHeapBlocks(
  CLR_RT_DblLinkedList &heap,
  uint32_t dataType,
  uint32_t flags,
  uint32_t length)
  {
 
#if !defined(BUILD_RTM)
  if (m_heapState == c_HeapState_UnderGC && ((flags & CLR_RT_HeapBlock::HB_SpecialGCAllocation) == 0))
    {
    trace_debug("Internal error: call to memory allocation during garbage collection!!!\r\n");

    // Getting here during a GC is possible, since the watchdog ISR may now require
    // dynamic memory allocation for logging.  Returning NULL means the watchdog log will
    // be lost, but without major restructuring there is not much we can do.
    return NULL;
    }
#endif

#if defined(NANOCLR_FORCE_GC_BEFORE_EVERY_ALLOCATION)
  if (m_heapState != c_HeapState_UnderGC)
    {
    g_CLR_RT_EventCache.EventCache_Cleanup();
    PerformGarbageCollection();
    }
#endif

  for (int phase = 0;; phase++)
    {
        {
        CLR_RT_HeapBlock *hb;

        if (flags & CLR_RT_HeapBlock::HB_Event)
          {
          NANOCLR_FOREACH_NODE_BACKWARD(CLR_RT_HeapCluster, hc, heap)
            {
            hb = hc->ExtractBlocks(dataType, flags, length);
            if (hb)
              {
              return hb;
              }
            }
          NANOCLR_FOREACH_NODE_BACKWARD_END();
          }
        else
          {
          if (m_lastHcUsed != NULL)
            {
            hb = m_lastHcUsed->ExtractBlocks(dataType, flags, length);
            if (hb)
              {
              return hb;
              }
            }

          NANOCLR_FOREACH_NODE(CLR_RT_HeapCluster, hc, heap)
            {
            hb = hc->ExtractBlocks(dataType, flags, length);
            if (hb)
              {
#if defined(NANOCLR_GC_VERBOSE)
              if (s_CLR_RT_fTrace_Memory >= c_CLR_RT_Trace_Info)
                {
                if (phase != 0)
                  {
                  trace_debug("ExtractHeapBlocks succeeded at phase %d\r\n", phase);
                  }
                }
#endif
              m_lastHcUsed = hc;
              return hb;
              }
            }
          NANOCLR_FOREACH_NODE_END();
          }

        m_lastHcUsed = NULL;
        }

        if (flags & CLR_RT_HeapBlock::HB_NoGcOnFailedAllocation)
          {
          return NULL;
          }

        switch (phase)
          {
          case 0:
#if defined(NANOCLR_GC_VERBOSE)
            if (s_CLR_RT_fTrace_Memory >= c_CLR_RT_Trace_Info)
              {
              trace_debug(
                "    Memory: ExtractHeapBlocks: %d bytes needed.\r\n",
                length * sizeof(CLR_RT_HeapBlock));
              }
#endif

            PerformGarbageCollection();

            break;

          default: // Total failure...
#if !defined(BUILD_RTM)
            trace_debug(
              "Failed allocation for %d blocks, %d bytes\r\n\r\n",
              length,
              length * sizeof(CLR_RT_HeapBlock));
#endif
            if (g_CLR_RT_GarbageCollector.m_freeBytes >= (length * sizeof(CLR_RT_HeapBlock)))
              {

              // A compaction probably would have saved this OOM
              // Compaction will occur for Bitmaps, Arrays, etc. if this function returns NULL, so lets not
              // through an assert here

              // Throw the OOM, and schedule a compaction at a safe point
              CLR_EE_SET(Compaction_Pending);
              }

            return NULL;
          }
    }
  }

CLR_RT_HeapBlock *CLR_RT_ExecutionEngine::AccessStaticField(uint32_t fd)
  {
 
  CLR_RT_FieldDef_Instance inst;

  if (inst.InitializeFromIndex(fd) && inst.m_target->flags & CLR_RECORD_FIELDDEF::FD_Static)
    {
    return &inst.m_assm->m_pStaticFields[inst.CrossReference().m_offset];
    }

  return NULL;
  }

HRESULT CLR_RT_ExecutionEngine::InitializeReference(CLR_RT_HeapBlock &ref, CLR_RT_SignatureParser &parser)
  {
 
  //
  // WARNING!!!
  //
  // If you change this method, change "CLR_RT_ExecutionEngine::InitializeLocals" too.
  //

  HRESULT hr;

  CLR_RT_SignatureParser::Element res;
  CLR_DataType dt;

  NANOCLR_CHECK_HRESULT(parser.Advance(res));

  dt = res.m_dt;

  if (res.m_levels > 0) // Array
    {
    dt = DATATYPE_OBJECT;
    }
  else
    {
    if (dt == DATATYPE_VALUETYPE)
      {
      CLR_RT_TypeDef_Instance inst;
      inst.InitializeFromIndex(res.m_cls);

      if ((inst.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask) == CLR_RECORD_TYPEDEF::TD_Semantics_Enum)
        {
        dt = (CLR_DataType)inst.m_target->dataType;
        }
      else
        {
        NANOCLR_SET_AND_LEAVE(NewObject(ref, inst));
        }
      }
    else
      {
      if (c_CLR_RT_DataTypeLookup[dt].m_flags & CLR_RT_DataTypeLookup::c_Reference)
        {
        dt = DATATYPE_OBJECT;
        }
      }
    }

  ref.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, CLR_RT_HeapBlock::HB_Alive, 1));
  ref.ClearData();

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::InitializeReference(
  CLR_RT_HeapBlock &ref,
  const CLR_RECORD_FIELDDEF *target,
  CLR_RT_Assembly *assm)
  {
 
  HRESULT hr;

  CLR_RT_SignatureParser parser;
  parser.Initialize_FieldDef(assm, target);

  NANOCLR_SET_AND_LEAVE(InitializeReference(ref, parser));

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::InitializeLocals(
  CLR_RT_HeapBlock *locals,
  CLR_RT_Assembly *assm,
  const CLR_RECORD_METHODDEF *md)
  {
 
  //
  // WARNING!!!
  //
  // This method is a shortcut for the following code:
  //

  HRESULT hr;

  CLR_PMETADATA sig = assm->GetSignature(md->locals);
  uint32_t count = md->numLocals;
  bool fZeroed = false;

  while (count)
    {
    CLR_DataType dt = DATATYPE_VOID;
    uint32_t cls;
    uint32_t levels = 0;
    CLR_DataType dtModifier = DATATYPE_VOID;

    while (true)
      {
      dt = CLR_UncompressElementType(sig);

      switch (dt)
        {
        case DATATYPE_TYPE_PINNED:
          dtModifier = DATATYPE_TYPE_PINNED;
          break;

          // Array declared on stack        .locals init [0] int16[] foo,
        case DATATYPE_SZARRAY:
          // Reference declared on stack - .locals init [1] int16& pinned pShort,
        case DATATYPE_BYREF:
          levels++;
          break;

        case DATATYPE_CLASS:
        case DATATYPE_VALUETYPE:
        {
        uint32_t tk = CLR_TkFromStream(sig);
        uint32_t idx = CLR_DataFromTk(tk);

        switch (CLR_TypeFromTk(tk))
          {
          case TBL_TypeSpec:
          {
          CLR_RT_SignatureParser sub;
          sub.Initialize_TypeSpec(assm, assm->GetTypeSpec(idx));
          CLR_RT_SignatureParser::Element res;

          NANOCLR_CHECK_HRESULT(sub.Advance(res));

          cls = res.m_cls;
          levels += res.m_levels;
          }
          break;

          case TBL_TypeRef:
            cls = assm->m_pCrossReference_TypeRef[idx].m_target;
            break;

          case TBL_TypeDef:
            cls = create_index(assm->m_idx, idx);
            break;

          default:
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }
        }
        goto done;

        default:
        {
        const uint32_t *cls2 = c_CLR_RT_DataTypeLookup[dt].m_cls;

        if (cls2 == NULL)
          {
          NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
          }

        cls = *cls2;
        }
        goto done;
        }
      }

  done:
    if (levels > 0) // Array or reference
      {
      locals->SetObjectReference(NULL);

      // If local varialb has DATATYPE_TYPE_PINNED, we mark heap block as
      if (dtModifier == DATATYPE_TYPE_PINNED)
        {
        locals->Pin();
        }
      }
    else
      {
      if (dt == DATATYPE_VALUETYPE)
        {
        CLR_RT_TypeDef_Instance inst;
        inst.InitializeFromIndex(cls);

        if (inst.m_target->dataType != DATATYPE_VALUETYPE)
          {
          locals->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(inst.m_target->dataType, CLR_RT_HeapBlock::HB_Alive, 1));
          locals->ClearData();
          }
        else
          {
          //
          // Before we allocate anything, we need to make sure the rest of the local variables are in a
          // consistent state.
          //
          if (fZeroed == false)
            {
            fZeroed = true;

            CLR_RT_HeapBlock *ptr = locals;
            CLR_RT_HeapBlock *ptrEnd = &locals[count];

            do
              {
              ptr->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(DATATYPE_I4, 0, 1));

              } while (++ptr < ptrEnd);
            }

          NANOCLR_CHECK_HRESULT(NewObject(*locals, inst));
          }
        }
      else
        {
        if (c_CLR_RT_DataTypeLookup[dt].m_flags & CLR_RT_DataTypeLookup::c_Reference)
          {
          dt = DATATYPE_OBJECT;
          }

        locals->SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, CLR_RT_HeapBlock::HB_Alive, 1));
        locals->ClearData();
        }
      }

    locals++;
    count--;
    }

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::NewObjectFromIndex(CLR_RT_HeapBlock &reference, uint32_t cls)
  {
 
  HRESULT hr;

  CLR_RT_TypeDef_Instance inst;

  if (inst.InitializeFromIndex(cls) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  NANOCLR_SET_AND_LEAVE(NewObject(reference, inst));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::NewObject(CLR_RT_HeapBlock &reference, const CLR_RT_TypeDef_Instance &inst)
  {
 
  HRESULT hr;

  reference.SetObjectReference(NULL);

  CLR_DataType dt = (CLR_DataType)inst.m_target->dataType;

  //
  // You cannot create an array this way.
  //
  if (inst.m_index == g_CLR_RT_WellKnownTypes.m_Array)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  if ((c_CLR_RT_DataTypeLookup[dt].m_flags & CLR_RT_DataTypeLookup::c_Reference) == 0)
    {
    reference.SetDataId(CLR_RT_HEAPBLOCK_RAW_ID(dt, 0, 1));
    }
  else
    {
    switch (dt)
      {
      case DATATYPE_STRING:
        //
        // Special case for strings.
        //
        break;

      case DATATYPE_WEAKCLASS:
      {
      CLR_RT_HeapBlock_WeakReference *weakref;

      // this used to be a call to CLR_RT_HeapBlock_WeakReference::CreateInstance
      weakref = (CLR_RT_HeapBlock_WeakReference *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(
        DATATYPE_WEAKCLASS,
        CLR_RT_HeapBlock::HB_InitializeToZero,
        sizeof(*weakref));
      CHECK_ALLOCATION(weakref);

      reference.SetObjectReference(weakref);
      }
      break;

      case DATATYPE_CLASS:
      case DATATYPE_VALUETYPE:
      {
      int clsFields = inst.m_target->iFields_Num;
      int totFields = inst.CrossReference().m_totalFields + CLR_RT_HeapBlock::HB_Object_Fields_Offset;
      CLR_RT_HeapBlock *obj = ExtractHeapBlocksForClassOrValueTypes(dt, 0, inst.m_index, totFields);
      CHECK_ALLOCATION(obj);

      reference.SetObjectReference(obj);

      {
      const CLR_RECORD_FIELDDEF *target = NULL;
      CLR_RT_Assembly *assm = NULL;
      CLR_RT_TypeDef_Instance instSub = inst;

      NANOCLR_CHECK_HRESULT(obj->SetObjectCls(inst.m_index));

      //
      // Initialize field types, from last to first.
      //
      // We do the decrement BEFORE the comparison because we want to stop short of the first field, the
      // object descriptor (already initialized).
      //
      obj += totFields;
      while (--totFields > 0)
        {
        while (clsFields == 0)
          {
          if (instSub.SwitchToParent() == false)
            NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);

          clsFields = instSub.m_target->iFields_Num;
          target = NULL;
          }

        if (target == NULL)
          {
          assm = instSub.m_assm;
          target = assm->GetFieldDef(instSub.m_target->iFields_First + clsFields);
          }

        obj--;
        target--;
        clsFields--;

        NANOCLR_CHECK_HRESULT(InitializeReference(*obj, target, assm));
        }
      }

      if (inst.HasFinalizer())
        {
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Finalizer::CreateInstance(reference.Dereference(), inst));
        }
      }
      break;

      default:
        NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
      }
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::NewObject(CLR_RT_HeapBlock &reference, uint32_t tk, CLR_RT_Assembly *assm)
  {
 
  HRESULT hr;

  CLR_RT_TypeDef_Instance res;

  if (res.ResolveToken(tk, assm) == false)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  NANOCLR_CHECK_HRESULT(NewObjectFromIndex(reference, res.m_index));

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::CloneObject(CLR_RT_HeapBlock &reference, const CLR_RT_HeapBlock &source)
  {
 
  HRESULT hr;

  const CLR_RT_HeapBlock *obj = &source;
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

  switch (dt)
    {

    case DATATYPE_VALUETYPE:
    case DATATYPE_CLASS:
    {
    //
    // Save the pointer to the object to clone, in case 'reference' and 'source' point to the same block.
    //
    CLR_RT_HeapBlock safeSource;
    safeSource.SetObjectReference(obj);
    CLR_RT_ProtectFromGC gc(safeSource);

    NANOCLR_CHECK_HRESULT(NewObjectFromIndex(reference, obj->ObjectCls()));
    NANOCLR_CHECK_HRESULT(CopyValueType(reference.Dereference(), obj));
    }
    break;

    default:
      if ((c_CLR_RT_DataTypeLookup[dt].m_flags & CLR_RT_DataTypeLookup::c_OptimizedValueType) == 0)
        {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

      //
      // Non-reference type, straight copy.
      //
      reference.Assign(source);
      break;
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::CopyValueType(CLR_RT_HeapBlock *destination, const CLR_RT_HeapBlock *source)
  {
 
  HRESULT hr;

  if (destination != source)
    {
    uint32_t cls = source->ObjectCls();
    if (cls == destination->ObjectCls())
      {
      CLR_RT_TypeDef_Instance inst;
      inst.InitializeFromIndex(cls);
      uint32_t totFields = inst.CrossReference().m_totalFields;

      if (source->IsBoxed())
        destination->Box();

      while (true)
        {
        if (totFields-- == 0)
          break;

        //
        // We increment the two pointers to skip the header of the objects.
        //
        source++;
        destination++;

        NANOCLR_CHECK_HRESULT(destination->Reassign(*source));
        }

      NANOCLR_SET_AND_LEAVE(S_OK);
      }

    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::NewArrayList(CLR_RT_HeapBlock &ref, int size, CLR_RT_HeapBlock_Array *&array)
  {
 
  HRESULT hr;

  const int minCapacity = CLR_RT_ArrayListHelper::c_defaultCapacity;
  int count = size;
  int capacity = size < minCapacity ? minCapacity : size;

  NANOCLR_CHECK_HRESULT(NewObjectFromIndex(ref, g_CLR_RT_WellKnownTypes.m_ArrayList));

  NANOCLR_CHECK_HRESULT(CLR_RT_ArrayListHelper::PrepareArrayList(ref, count, capacity));
  NANOCLR_CHECK_HRESULT(CLR_RT_ArrayListHelper::ExtractArrayFromArrayList(ref, array, count, capacity));

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::FindFieldDef(
  CLR_RT_TypeDef_Instance &inst,
  const char *szText,
  uint32_t &res)
  {
 
  HRESULT hr;

  CLR_RT_TypeDef_Instance local = inst;

  do
    {
    if (local.m_assm->FindFieldDef(local.m_target, szText, NULL, 0, res))
      NANOCLR_SET_AND_LEAVE(S_OK);
    } while (local.SwitchToParent());

    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

    NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::FindFieldDef(
  CLR_RT_HeapBlock &reference,
  const char *szText,
  uint32_t &res)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock *obj;
  CLR_RT_TypeDef_Instance inst;

  if (reference.DataType() != DATATYPE_OBJECT)
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);

  obj = reference.Dereference();
  FAULT_ON_NULL(obj);

  if (inst.InitializeFromIndex(obj->ObjectCls()) == false)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }

  NANOCLR_SET_AND_LEAVE(FindFieldDef(inst, szText, res));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::FindField(CLR_RT_HeapBlock &reference, const char *szText, CLR_RT_HeapBlock *&field)
  {
 
  HRESULT hr;

  CLR_RT_FieldDef_Instance inst;
  uint32_t idx;
  CLR_RT_HeapBlock *res;

  field = NULL;

  NANOCLR_CHECK_HRESULT(FindFieldDef(reference, szText, idx));

  inst.InitializeFromIndex(idx);

  if (inst.m_target->flags & CLR_RECORD_FIELDDEF::FD_Static)
    {
    res = CLR_RT_ExecutionEngine::AccessStaticField(idx);
    if (res == NULL)
      NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
    }
  else
    {
    res = reference.Dereference();
    FAULT_ON_NULL(res);

    res += inst.CrossReference().m_offset;
    }

  field = res;

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::SetField(CLR_RT_HeapBlock &reference, const char *szText, CLR_RT_HeapBlock &value)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock *field;

  NANOCLR_CHECK_HRESULT(FindField(reference, szText, field));

  field->Assign(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::GetField(CLR_RT_HeapBlock &reference, const char *szText, CLR_RT_HeapBlock &value)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock *field;

  NANOCLR_CHECK_HRESULT(FindField(reference, szText, field));

  value.Assign(*field);

  NANOCLR_NOCLEANUP();
  }

//--//

CLR_RT_HeapBlock_Lock *CLR_RT_ExecutionEngine::FindLockObject(CLR_RT_DblLinkedList &threads, CLR_RT_HeapBlock &object)
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, threads)
    {
    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Lock, lock, th->m_locks)
      {
      CLR_RT_HeapBlock &res = lock->m_resource;

      if (CLR_RT_HeapBlock::ObjectsEqual(res, object, true))
        {
        return lock;
        }
      }
    NANOCLR_FOREACH_NODE_END();
    }
  NANOCLR_FOREACH_NODE_END();

  return NULL;
  }

CLR_RT_HeapBlock_Lock *CLR_RT_ExecutionEngine::FindLockObject(CLR_RT_HeapBlock &object)
  {
 
  CLR_RT_HeapBlock_Lock *lock;

  if (object.DataType() == DATATYPE_OBJECT)
    {
    CLR_RT_HeapBlock *ptr = object.Dereference();

    if (ptr)
      {
      switch (ptr->DataType())
        {
        case DATATYPE_VALUETYPE:
        case DATATYPE_CLASS:
          return ptr->ObjectLock();

        default:
          // the remaining data types aren't to be handled
          break;
        }
      }
    }

  lock = FindLockObject(m_threadsReady, object);
  if (lock)
    return lock;
  lock = FindLockObject(m_threadsWaiting, object);
  return lock;
  }

//--//

void CLR_RT_ExecutionEngine::DeleteLockRequests(CLR_RT_Thread *thTarget, CLR_RT_SubThread *sthTarget)
  {
 
  if ((thTarget && thTarget->m_lockRequestsCount) || (sthTarget && sthTarget->m_lockRequestsCount))
    {
    DeleteLockRequests(thTarget, sthTarget, m_threadsReady);
    DeleteLockRequests(thTarget, sthTarget, m_threadsWaiting);
    }
  }

void CLR_RT_ExecutionEngine::DeleteLockRequests(
  CLR_RT_Thread *thTarget,
  CLR_RT_SubThread *sthTarget,
  CLR_RT_DblLinkedList &threads)
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, threads)
    {
    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Lock, lock, th->m_locks)
      {
      NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_LockRequest, req, lock->m_requests)
        {
        CLR_RT_SubThread *sth = req->m_subthreadWaiting;

        if (sth == sthTarget || sth->m_owningThread == thTarget)
          {
          g_CLR_RT_EventCache.Append_Node(req);

          if (sth->ChangeLockRequestCount(-1))
            return;
          }
        }
      NANOCLR_FOREACH_NODE_END();
      }
    NANOCLR_FOREACH_NODE_END();
    }
  NANOCLR_FOREACH_NODE_END();
  }


int64_t CLR_RT_ExecutionEngine::ProcessTimer()
  {
 

  int64_t timeoutMin;

  timeoutMin = m_maximumTimeToActive; // max sleep.

  ////////////////////////////////////////////////
  // WARNING
  //
  // The check on the 'Stopped' condition, and the 'else'
  // condition below cause a race condition when the application is running under debugger
  // and there are no ready threads in the app.  teh desktop side debugger in facts is prodigal
  // of Stopped commands and so is the runtime itself.  Since the commands come asynchronously
  // though and there is no co-ordination it is possible that
  // a) a 'Stopped' condition reset get lost or
  // b) waiting threads whose timers are expired are never moved to the ready queue
  //

  if (CLR_EE_DBG_IS(Stopped) && m_threadsWaiting.IsEmpty())
    {
    // Don't process events while the debugger is stopped and no thread was waiting.
    // if some thread was waiting we might need to transfer it to the ready queue
    }
  else
    {
    if (m_timerCache && m_timerCacheNextTimeout > HAL_Time_CurrentTime())
      {
      timeoutMin = m_timerCacheNextTimeout - HAL_Time_CurrentTime();
      }
    // else
    {
    CheckTimers(timeoutMin);

    CheckThreads(timeoutMin, m_threadsReady);
    CheckThreads(timeoutMin, m_threadsWaiting);

    m_timerCacheNextTimeout = timeoutMin + HAL_Time_CurrentTime();
    m_timerCache = (m_timerCacheNextTimeout > HAL_Time_CurrentTime());
    }
    }

  /*
  // if the system timer is not set as one of the wakeup events then just return the max time to active
  if (0 == (g_CLR_HW_Hardware.m_wakeupEvents & SYSTEM_EVENT_FLAG_SYSTEM_TIMER))
    {
    timeoutMin = m_maximumTimeToActive;
    }
    */

  return timeoutMin;
  }

void CLR_RT_ExecutionEngine::ProcessTimeEvent(uint32_t event)
  {
 
  SYSTEMTIME systemTime;

  HAL_Time_ToSystemTime(HAL_Time_CurrentTime(), &systemTime);

  NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Timer, timer, m_timers)
    {
    if (timer->m_flags & CLR_RT_HeapBlock_Timer::c_EnabledTimer)
      {
      uint32_t val = timer->m_flags & CLR_RT_HeapBlock_Timer::c_AnyChange;

      if (val)
        {
        timer->AdjustNextFixedExpire(systemTime, true);

        if (val == event && (timer->m_flags & CLR_RT_HeapBlock_Timer::c_Recurring) == 0)
          {
          timer->Trigger();
          }
        }
      }
    }
  NANOCLR_FOREACH_NODE_END();

  SpawnTimer();
  }

void CLR_RT_ExecutionEngine::InvalidateTimerCache()
  {
 
  g_CLR_RT_ExecutionEngine.m_timerCache = false;
  }

//--//--//

bool CLR_RT_ExecutionEngine::IsTimeExpired(const int64_t &timeExpire, int64_t &timeoutMin)
  {
 

  if (timeExpire <= (int64_t)HAL_Time_CurrentTime())
    return true;

  int64_t diff = timeExpire - HAL_Time_CurrentTime();

  if (diff < timeoutMin)
    {
    timeoutMin = diff;
    }

  return false;
  }

bool CLR_RT_ExecutionEngine::IsThereEnoughIdleTime(uint32_t expectedMsec)
  {
 
  /*
  if (::Events_MaskedRead(g_CLR_HW_Hardware.m_wakeupEvents))
    return false;
    */

  int64_t now = HAL_Time_CurrentTime();

  if (now + expectedMsec * TIME_CONVERSION__TO_MILLISECONDS >= m_currentNextActivityTime)
    return false;

  return true;
  }

//--//

void CLR_RT_ExecutionEngine::CheckTimers(int64_t &timeoutMin)
  {
 
  bool fAnyTimersExpired = false;

  NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Timer, timer, m_timers)
    {
    if (timer->m_flags & CLR_RT_HeapBlock_Timer::c_EnabledTimer)
      {
      int64_t expire = timer->m_timeExpire;
      if (IsTimeExpired(expire, timeoutMin))
        {

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
        if (CLR_EE_DBG_IS_NOT(PauseTimers))
#endif
          {
          timer->Trigger();
          fAnyTimersExpired = true;
          }
        }
      }
    }
  NANOCLR_FOREACH_NODE_END();

  if (fAnyTimersExpired)
    {
    SpawnTimer();
    }
  }

void CLR_RT_ExecutionEngine::CheckThreads(int64_t &timeoutMin, CLR_RT_DblLinkedList &threads)
  {
 

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, threads)
    {
    int64_t expire;

#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)
    if (th->m_flags & CLR_RT_Thread::TH_F_Suspended)
      {
      continue;
      }
#endif //#if defined(NANOCLR_ENABLE_SOURCELEVELDEBUGGING)

    //
    // Check events.
    //
    expire = th->m_waitForEvents_Timeout;
    if (IsTimeExpired(expire, timeoutMin))
      {
      th->m_waitForEvents_Timeout = TIMEOUT_INFINITE;

      th->Restart(false);
      }

    //
    // Check wait for object.
    //

    {
    CLR_RT_HeapBlock_WaitForObject *wait = th->m_waitForObject;

    if (wait)
      {
      if (IsTimeExpired(wait->m_timeExpire, timeoutMin))
        {
        th->m_waitForObject_Result = CLR_RT_Thread::TH_WAIT_RESULT_TIMEOUT;

        th->Restart(true);
        }
      }
    }

    //
    // Check lock requests.
    //
    NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_Lock, lock, th->m_locks)
      {
      NANOCLR_FOREACH_NODE(CLR_RT_HeapBlock_LockRequest, req, lock->m_requests)
        {
        if (IsTimeExpired(req->m_timeExpire, timeoutMin))
          {
          CLR_RT_SubThread *sth = req->m_subthreadWaiting;

          sth->ChangeLockRequestCount(-1);

          g_CLR_RT_EventCache.Append_Node(req);
          }
        }
      NANOCLR_FOREACH_NODE_END();
      }
    NANOCLR_FOREACH_NODE_END();

    //
    // Check constraints.
    //
    NANOCLR_FOREACH_NODE_BACKWARD(CLR_RT_SubThread, sth, th->m_subThreads)
      {
      if (sth->m_timeConstraint != TIMEOUT_INFINITE)
        {
        if (IsTimeExpired(s_compensation.Adjust(sth->m_timeConstraint), timeoutMin))
          {
          ExceptionCreateInstance(th->m_currentException,
            g_CLR_RT_WellKnownTypes.m_ConstraintException,
            S_OK,
            th->CurrentFrame());

          if ((sth->m_status & CLR_RT_SubThread::STATUS_Triggered) == 0)
            {
            sth->m_status |= CLR_RT_SubThread::STATUS_Triggered;

            //
            // This is the first time, give it 500msec to clean before killing it.
            //
            sth->m_timeConstraint += TIME_CONVERSION__TO_MILLISECONDS * 500;
            CLR_RT_ExecutionEngine::InvalidateTimerCache();
            }
          else
            {
            CLR_RT_SubThread::DestroyInstance(th, sth, CLR_RT_SubThread::MODE_CheckLocks);

            //
            // So it doesn't fire again...
            //
            sth->m_timeConstraint = TIMEOUT_INFINITE;
            }

          th->Restart(true);
          }
        }
      }
    NANOCLR_FOREACH_NODE_END();
    }
  NANOCLR_FOREACH_NODE_END();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::LockObject(
  CLR_RT_HeapBlock &reference,
  CLR_RT_SubThread *sth,
  const int64_t &timeExpire,
  bool fForce)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_Lock *lock;

  lock = FindLockObject(reference);

  if (lock == NULL)
    {
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Lock::CreateInstance(lock, sth->m_owningThread, reference));
    }

  NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Lock::IncrementOwnership(lock, sth, timeExpire, fForce));

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::UnlockObject(CLR_RT_HeapBlock &reference, CLR_RT_SubThread *sth)
  {
 
  HRESULT hr;

  CLR_RT_HeapBlock_Lock *lock;

  lock = FindLockObject(reference);

  NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_Lock::DecrementOwnership(lock, sth));

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::Sleep(CLR_RT_Thread *caller, const int64_t &timeExpire)
  {
 
  HRESULT hr;

  caller->m_waitForEvents_Timeout = timeExpire;
  CLR_RT_ExecutionEngine::InvalidateTimerCache();
  caller->m_status = CLR_RT_Thread::TH_S_Waiting;

  NANOCLR_SET_AND_LEAVE(CLR_E_THREAD_WAITING);

  NANOCLR_NOCLEANUP();
  }

//--//

HRESULT CLR_RT_ExecutionEngine::WaitEvents(
  CLR_RT_Thread *caller,
  const int64_t &timeExpire,
  uint32_t events,
  bool &fSuccess)
  {
 
  HRESULT hr;

  if (m_raisedEvents & events)
    {
    m_raisedEvents &= ~events;

    fSuccess = true;
    }
  else
    {
    fSuccess = false;

    if ((int64_t)HAL_Time_CurrentTime() < timeExpire)
      {
      caller->m_waitForEvents = events;
      caller->m_waitForEvents_Timeout = timeExpire;
      CLR_RT_ExecutionEngine::InvalidateTimerCache();
      caller->m_status = CLR_RT_Thread::TH_S_Waiting;

      NANOCLR_SET_AND_LEAVE(CLR_E_THREAD_WAITING);
      }
    }

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_ExecutionEngine::SignalEvents(CLR_RT_DblLinkedList &threads, uint32_t events)
  {
 
  m_raisedEvents |= events;

  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, threads)
    {
    if ((th->m_waitForEvents & events) != 0)
      {
      _ASSERTE(&threads == &m_threadsWaiting);
      _ASSERTE(th->m_status == CLR_RT_Thread::TH_S_Waiting);

      th->Restart(true);
      }
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CLR_RT_ExecutionEngine::SignalEvents(uint32_t events)
  {
 
  // Why does the ready queue need to be checked.
  SignalEvents(m_threadsReady, events);
  SignalEvents(m_threadsWaiting, events);
  }

//--//

bool CLR_RT_ExecutionEngine::IsInstanceOf(
  CLR_RT_TypeDescriptor &desc,
  CLR_RT_TypeDescriptor &descTarget,
  bool isInstInstruction)
  {
 
  CLR_RT_TypeDef_Instance &inst = desc.m_handlerCls;
  CLR_RT_TypeDef_Instance &instTarget = descTarget.m_handlerCls;
  bool fArray = false;

  while (desc.m_reflex.m_levels > 0 && descTarget.m_reflex.m_levels > 0)
    {
    desc.GetElementType(desc);
    descTarget.GetElementType(descTarget);

    fArray = true;
    }

  if (desc.m_reflex.m_levels < descTarget.m_reflex.m_levels)
    return false;

  if (desc.m_reflex.m_levels > descTarget.m_reflex.m_levels)
    {
    if (descTarget.m_reflex.m_levels == 0 && !isInstInstruction)
      {
      //
      // Casting from <type>[] to System.Array or System.Object is always allowed.
      //
      if (inst.m_index == g_CLR_RT_WellKnownTypes.m_Array ||
        inst.m_index == g_CLR_RT_WellKnownTypes.m_Object ||
        inst.m_index == g_CLR_RT_WellKnownTypes.m_IList ||
        inst.m_index == g_CLR_RT_WellKnownTypes.m_ICloneable)
        {
        return true;
        }
      }

    if (inst.m_target->dataType != instTarget.m_target->dataType)
      {
      return false;
      }
    }

  uint32_t semantic = (inst.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask);
  uint32_t semanticTarget = (instTarget.m_target->flags & CLR_RECORD_TYPEDEF::TD_Semantics_Mask);

  if (fArray)
    {
    if (semantic != semanticTarget)
      {
      return false;
      }
    }

  do
    {
    if (inst.m_index == instTarget.m_index)
      {
      return true;
      }

    //
    // Scan the list of interfaces.
    //
    if (semanticTarget == CLR_RECORD_TYPEDEF::TD_Semantics_Interface && inst.m_target->interfaces != CLR_EmptyIndex)
      {
      CLR_RT_SignatureParser parser;
      parser.Initialize_Interfaces(inst.m_assm, inst.m_target);
      CLR_RT_SignatureParser::Element res;

      while (parser.Available() > 0)
        {
        if (FAILED(parser.Advance(res)))
          break;

        if (res.m_cls == instTarget.m_index)
          {
          return true;
          }
        }
      }
    } while (inst.SwitchToParent());

    return false;
  }

bool CLR_RT_ExecutionEngine::IsInstanceOf(uint32_t cls, uint32_t clsTarget)
  {
 
  CLR_RT_TypeDescriptor desc;
  CLR_RT_TypeDescriptor descTarget;

  if (FAILED(desc.InitializeFromType(cls)))
    return false;
  if (FAILED(descTarget.InitializeFromType(clsTarget)))
    return false;

  return IsInstanceOf(desc, descTarget, false);
  }

bool CLR_RT_ExecutionEngine::IsInstanceOf(CLR_RT_HeapBlock &ref, uint32_t clsTarget)
  {
 
  CLR_RT_TypeDescriptor desc;
  CLR_RT_TypeDescriptor descTarget;

  if (FAILED(desc.InitializeFromObject(ref)))
    return false;
  if (FAILED(descTarget.InitializeFromType(clsTarget)))
    return false;

  return IsInstanceOf(desc, descTarget, false);
  }

bool CLR_RT_ExecutionEngine::IsInstanceOf(
  CLR_RT_HeapBlock &obj,
  CLR_RT_Assembly *assm,
  uint32_t token,
  bool isInstInstruction)
  {
 
  CLR_RT_TypeDescriptor desc;
  CLR_RT_TypeDescriptor descTarget;
  CLR_RT_TypeDef_Instance clsTarget;
  CLR_RT_TypeSpec_Instance defTarget;

  if (FAILED(desc.InitializeFromObject(obj)))
    return false;

  if (clsTarget.ResolveToken(token, assm))
    {
    //
    // Shortcut for identity.
    //
    if (desc.m_handlerCls.m_index == clsTarget.m_index)
      return true;

    if (FAILED(descTarget.InitializeFromType(clsTarget.m_index)))
      return false;
    }
  else if (defTarget.ResolveToken(token, assm))
    {
    if (FAILED(descTarget.InitializeFromTypeSpec(defTarget.m_index)))
      return false;
    }
  else
    {
    return false;
    }

  return IsInstanceOf(desc, descTarget, isInstInstruction);
  }

HRESULT CLR_RT_ExecutionEngine::CastToType(
  CLR_RT_HeapBlock &ref,
  uint32_t tk,
  CLR_RT_Assembly *assm,
  bool isInstInstruction)
  {
 
  HRESULT hr;

  if (ref.DataType() == DATATYPE_OBJECT && ref.Dereference() == NULL)
    {
    ;
    }
  else if (g_CLR_RT_ExecutionEngine.IsInstanceOf(ref, assm, tk, isInstInstruction) == true)
    {
    ;
    }
  else
    {
    if (isInstInstruction == false)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_CAST);
      }

    ref.SetObjectReference(NULL);
    }

  NANOCLR_NOCLEANUP();
  }

////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_RT_ExecutionEngine::InitTimeout(int64_t &timeExpire, const int64_t &timeout)
  {
 
  HRESULT hr;

  if (timeout < 0)
    {
    // because we are expecting the timeout value to be in ticks
    // need to check for two possible infinite timeouts:
    // 1. when coding in native it's supposed to use -1L as a timeout infinite
    // 2. in managed code the constant System.Threading.Timeout.Infinite is -1 milliseconds, therefore needs to be
    // converted to ticks
    if ((timeout != -1L) && (timeout != -1L * TIME_CONVERSION__TO_MILLISECONDS))
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
      }

    timeExpire = TIMEOUT_INFINITE;
    }
  else
    {
    timeExpire = timeout + HAL_Time_CurrentTime();
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT CLR_RT_ExecutionEngine::InitTimeout(int64_t &timeExpire, int32_t timeout)
  {
 
  HRESULT hr;

  if (timeout < 0)
    {
    if (timeout != -1)
      {
      NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
      }

    timeExpire = TIMEOUT_INFINITE;
    }
  else
    {
    timeExpire = timeout;
    timeExpire *= TIME_CONVERSION__TO_MILLISECONDS;
    timeExpire += HAL_Time_CurrentTime();
    }

  NANOCLR_NOCLEANUP();
  }

void CLR_RT_ExecutionEngine::SetDebuggingInfoBreakpoints(bool fSet)
  {
 
  for (size_t pos = 0; pos < m_breakpointsNum; pos++)
    {
    const execution_breakpoint_t &def = m_breakpoints[pos];

    if (def.m_flags & c_HARD)
      {
      CLR_RT_MethodDef_Instance inst;

      if (inst.InitializeFromIndex(def.m_method_index))
        {
        inst.DebuggingInfo().SetBreakpoint(fSet);
        }
      }
    }
  }

void CLR_RT_ExecutionEngine::InstallBreakpoints(const execution_breakpoint_t *data, size_t num)
  {
 
  SetDebuggingInfoBreakpoints(false);

  if (m_breakpoints)
    {
    CLR_RT_Memory::Release((void *) m_breakpoints);

    m_breakpoints = NULL;
    m_breakpointsNum = 0;
    }

  if (num)
    {
    size_t len = num * sizeof(execution_breakpoint_t);

    m_breakpoints = (execution_breakpoint_t *)CLR_RT_Memory::Allocate(
      len,
      CLR_RT_HeapBlock::HB_CompactOnFailure);
    if (m_breakpoints)
      {
      memcpy(m_breakpoints, data, len);

      m_breakpointsNum = num;
      }
    }

  SetDebuggingInfoBreakpoints(true);

  Breakpoint_Threads_Prepare(m_threadsReady);
  Breakpoint_Threads_Prepare(m_threadsWaiting);
  }

void CLR_RT_ExecutionEngine::StopOnBreakpoint(execution_breakpoint_t *def, CLR_RT_Thread *th)
  {
  if (CLR_EE_DBG_IS_NOT(BreakpointsDisabled))
    {
    if (m_breakpointsActiveNum < ARRAYSIZE(m_breakpointsActive))
      {
      execution_breakpoint_t &bp = m_breakpointsActive[m_breakpointsActiveNum++];

      bp = *def;

      CLR_EE_DBG_SET(Stopped);

      if (th)
        {
        bp.m_pid = th->m_pid;
        th->m_timeQuantumExpired = true;
        }

      if (m_breakpointsActiveNum == 1)
        {
        debugger_event_args_t args;
        args.version = sizeof(debugger_event_args_t);
        args.reason = &bp;
        debug_breakpoint(&args);
        }
      }
    else
      {
      _ASSERTE(false);
      }
    }
  }

void CLR_RT_ExecutionEngine::StopOnBreakpoint(
  execution_breakpoint_t &def,
  CLR_RT_StackFrame *stack,
  CLR_PMETADATA ip)
  {
 
  if (ip == NULL)
    ip = stack->m_IP;

  def.m_depth = stack->m_depth;
  def.m_method_index = stack->m_call;
  def.m_IP = (uint32_t)(ip - stack->m_IPstart);

  // Don't fail for special cases regarding messages dealing with exception handling.
  _ASSERTE(def.m_IP == 0xffffffff || ip >= stack->m_IPstart);
  // we don't actually know the end of the method.

  StopOnBreakpoint(def, stack->m_owningThread);
  }

bool CLR_RT_ExecutionEngine::DequeueActiveBreakpoint(execution_breakpoint_t &def)
  {
 
  if (m_breakpointsActiveNum)
    {
    def = m_breakpointsActive[0];

    if (--m_breakpointsActiveNum == 0)
      {
      def.m_flags |= c_LAST_BREAKPOINT;
      }

    memmove(
      &m_breakpointsActive[0],
      &m_breakpointsActive[1],
      sizeof(m_breakpointsActive[0]) * m_breakpointsActiveNum);

    return true;
    }

  return false;
  }

//--//

void CLR_RT_ExecutionEngine::Breakpoint_System_Event(
  execution_breakpoint_t &hit,
  uint16_t event,
  CLR_RT_Thread *th,
  CLR_RT_StackFrame *stack,
  CLR_PMETADATA ip)
  {
 
  for (size_t pos = 0; pos < m_breakpointsNum; pos++)
    {
    execution_breakpoint_t &def = m_breakpoints[pos];

    if (stack != NULL)
      {
      _ASSERTE(FIMPLIES(th != NULL, th == stack->m_owningThread));

      th = stack->m_owningThread;
      }

    if (th == NULL || (def.m_pid == th->m_pid) || def.m_pid == c_PID_ANY)
      {
      if (def.m_flags & event)
        {
        hit.m_id = def.m_id;
        hit.m_flags = event;

        if (stack != NULL)
          {
          StopOnBreakpoint(hit, stack, ip);
          }
        else
          {
          StopOnBreakpoint(hit, th);
          }
        }
      }
    }
  }

void CLR_RT_ExecutionEngine::Breakpoint_Assemblies_Loaded()
  {
  debugger_event_args_t args;
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_ASSEMBLIES_LOADED;
  args.pid = m_currentThread->m_pid;
  debug_breakpoint(&args);
  }

void CLR_RT_ExecutionEngine::Breakpoint_Threads_Prepare(CLR_RT_DblLinkedList &threads)
  {
 
  NANOCLR_FOREACH_NODE(CLR_RT_Thread, th, threads)
    {
    th->m_fHasJMCStepper = false;

    for (size_t pos = 0; pos < m_breakpointsNum; pos++)
      {
      execution_breakpoint_t &def = m_breakpoints[pos];

      if (def.m_flags & c_STEP_JMC)
        {
        if (def.m_pid == th->m_pid)
          {
          th->m_fHasJMCStepper = true;
          break;
          }
        }
      }

    NANOCLR_FOREACH_NODE(CLR_RT_StackFrame, call, th->m_stackFrames)
      {
      call->m_flags &= ~CLR_RT_StackFrame::c_HasBreakpoint;

      if (call->m_call.DebuggingInfo().HasBreakpoint())
        {
        call->m_flags |= CLR_RT_StackFrame::c_HasBreakpoint;
        }
      else
        {
        for (size_t pos = 0; pos < m_breakpointsNum; pos++)
          {
          execution_breakpoint_t &def = m_breakpoints[pos];

          if (def.m_flags & c_STEP)
            {
            if (def.m_pid == th->m_pid && def.m_depth == call->m_depth)
              {
              call->m_flags |= CLR_RT_StackFrame::c_HasBreakpoint;
              break;
              }
            }
          }
        }
#ifndef CLR_NO_IL_INLINE
      if (call->m_inlineFrame)
        {
        if (call->m_inlineFrame->m_frame.m_call.DebuggingInfo().HasBreakpoint())
          {
          call->m_flags |= CLR_RT_StackFrame::c_HasBreakpoint;
          }
        else
          {
          for (size_t pos = 0; pos < m_breakpointsNum; pos++)
            {
            execution_breakpoint_t &def = m_breakpoints[pos];

            if (def.m_flags & c_STEP)
              {
              if (def.m_pid == th->m_pid && def.m_depth == (call->m_depth - 1))
                {
                call->m_flags |= CLR_RT_StackFrame::c_HasBreakpoint;
                break;
                }
              }
            }
          }
        }
#endif
      }
    NANOCLR_FOREACH_NODE_END();
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CLR_RT_ExecutionEngine::Breakpoint_Thread_Terminated(CLR_RT_Thread *th)
  {
  debugger_event_args_t args;
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_THREAD_TERMINATED;
  if (th->m_scratchPad > 0)
    args.flags |= c_EVAL_COMPLETE;
  args.pid = th->m_pid;
  args.depth = 0xFFFFFFFF;

  debug_breakpoint(&args);
  }

void CLR_RT_ExecutionEngine::Breakpoint_Thread_Created(CLR_RT_Thread *th)
  {
  debugger_event_args_t args;
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_THREAD_CREATED;
  args.pid = th->m_pid;

  debug_breakpoint(&args);
  }

void CLR_RT_ExecutionEngine::Breakpoint_StackFrame_Push(CLR_RT_StackFrame *stack, uint32_t reason)
  {
  CLR_RT_Thread *th = stack->m_owningThread;
  int pid = th->m_pid;
  uint32_t depthMax = stack->m_depth - 1;
  uint16_t flags = c_STEP_IN;
  uint32_t depthMin = depthMax;

  if (stack->m_flags & CLR_RT_StackFrame::c_PseudoStackFrameForFilter)
    {
    // If a filter frame is being pushed on (assuming the InterceptMask for filters is set), we want to intercept
    // the frame in certain special cases.
    _ASSERTE(th->m_nestedExceptionsPos);
    CLR_RT_Thread::UnwindStack &us = th->m_nestedExceptions[th->m_nestedExceptionsPos - 1];
    _ASSERTE(us.m_stack == stack);
    depthMin = us.m_handlerStack->m_depth;
    // If we popped off frames from AppDomain transitions that had steppers, we want to break there as well.
    depthMax = 0xffffffff;
    flags |= c_STEP_OVER | c_STEP_OUT;
    }

  for (size_t pos = 0; pos < m_breakpointsNum; pos++)
    {
    execution_breakpoint_t &def = m_breakpoints[pos];

    if (def.m_flags & flags)
      {
      if (def.m_pid == pid)
        {
        bool fStop;

        if (def.m_flags & c_STEP_JMC)
          {
          fStop = stack->m_call.DebuggingInfo().IsJMC();
          }
        else
          {
          fStop = (def.m_depth >= depthMin && def.m_depth <= depthMax);
          }

        if (def.m_depth == depthMin &&
          (def.m_flags & c_STEP) == c_STEP_OUT)
          {
          // In the case a user did a step out in a frame, we don't want to break if a filter gets pushed from
          // that frame. However, if there is a step in or over on that frame, we should break.
          fStop = false;
          }

        if (fStop)
          {
          execution_breakpoint_t hit = def;

          hit.m_flags = c_STEP_IN;
          hit.m_depthExceptionHandler = reason;

          StopOnBreakpoint(hit, stack, NULL);
          }
        }
      }
    }

  Breakpoint_StackFrame_Hard(stack, stack->m_IP);
  }

void CLR_RT_ExecutionEngine::Breakpoint_StackFrame_Pop(CLR_RT_StackFrame *stack, bool stepEH)
  {
 
  int pid = stack->m_owningThread->m_pid;
  uint32_t depth = stack->m_depth;
  CLR_RT_StackFrame *caller = stack->Caller();

  if (caller->Prev() || stepEH)
    {
    for (size_t pos = 0; pos < m_breakpointsNum; pos++)
      {
      execution_breakpoint_t &def = m_breakpoints[pos];

      if (def.m_flags & c_STEP_OUT)
        {
        if (def.m_pid == pid)
          {
          bool fStop;

          if (def.m_flags & c_STEP_JMC)
            {
            fStop = (depth <= def.m_depth) && caller->m_call.DebuggingInfo().IsJMC();
            }
          else
            {
            fStop = (depth <= def.m_depth);
            }

          if (fStop)
            {
            execution_breakpoint_t hit = def;

            hit.m_flags = c_STEP_OUT;

            if (stepEH)
              {
              hit.m_depthExceptionHandler =
                c_DEPTH_STEP_EXCEPTION_HANDLER;
              }
            else
              {
              hit.m_depthExceptionHandler =
                c_DEPTH_STEP_RETURN;
              }

            StopOnBreakpoint(hit, (stepEH) ? stack : caller, NULL);
            }
          }
        }
      }
    }
  }

void CLR_RT_ExecutionEngine::Breakpoint_StackFrame_Step(CLR_RT_StackFrame *stack, CLR_PMETADATA ip)
  {
  int pid = stack->m_owningThread->m_pid;
  uint32_t depth = stack->m_depth;
  uint32_t IPoffset = (uint32_t)(ip - stack->m_IPstart);

  for (size_t pos = 0; pos < m_breakpointsNum; pos++)
    {
    execution_breakpoint_t &def = m_breakpoints[pos];

    if (def.m_flags & c_STEP_OVER)
      {
      if (def.m_pid == pid && def.m_depth == depth && (IPoffset < def.m_IPStart || IPoffset >= def.m_IPEnd))
        {
        if (def.m_flags & c_STEP_JMC)
          {
          if (!stack->m_call.DebuggingInfo().IsJMC())
            {
            continue;
            }
          }

        execution_breakpoint_t hit = def;

        hit.m_flags = c_STEP_OVER;

        StopOnBreakpoint(hit, stack, ip);
        }
      }
    }

  Breakpoint_StackFrame_Hard(stack, ip);
  }

void CLR_RT_ExecutionEngine::Breakpoint_StackFrame_Hard(CLR_RT_StackFrame *stack, CLR_PMETADATA ip)
  {
 
  if (stack->Prev() != NULL && ip != NULL)
    {
    uint32_t IPoffset = (uint32_t)(ip - stack->m_IPstart);

    for (size_t pos = 0; pos < m_breakpointsNum; pos++)
      {
      execution_breakpoint_t &def = m_breakpoints[pos];

      if (def.m_flags & c_HARD)
        {
        if (def.m_pid == stack->m_owningThread->m_pid ||
          def.m_pid == c_PID_ANY)
          {
          if (def.m_method_index == stack->m_call.m_index && def.m_IP == IPoffset)
            {
            debugger_event_args_t hit;
            memset(&hit, 0, sizeof(debugger_event_args_t));
            hit.version = sizeof(debugger_event_args_t);
            hit.ip = ip;
            hit.stack = stack;
            hit.reason = &m_breakpoints[pos];
            hit.flags = c_HARD;

            debug_breakpoint(&hit);
            }
          }
        }
      }
    }
  }

void CLR_RT_ExecutionEngine::Breakpoint_Exception(CLR_RT_StackFrame *stack, uint32_t reason, CLR_PMETADATA ip)
  {
  debugger_event_args_t args;
  memset(&args, 0, sizeof(debugger_event_args_t));
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_EXCEPTION_THROWN;
  args.stack = stack;
  args.ip = ip;
  args.depth = reason;
  args.pid = stack->m_owningThread->m_pid;

  debug_breakpoint(&args);
  }

void CLR_RT_ExecutionEngine::Breakpoint_Exception_Uncaught(CLR_RT_Thread *th)
  {
  debugger_event_args_t args;
  memset(&args, 0, sizeof(debugger_event_args_t));
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_EXCEPTION_THROWN;
  args.depth = c_DEPTH_UNCAUGHT;
  args.exception_handler_depth = c_DEPTH_UNCAUGHT;
  args.pid = th->m_pid;

  debug_breakpoint(&args);
  }

void CLR_RT_ExecutionEngine::Breakpoint_Exception_Intercepted(CLR_RT_StackFrame *stack)
  {
  debugger_event_args_t args;
  memset(&args, 0, sizeof(debugger_event_args_t));
  args.version = sizeof(debugger_event_args_t);
  args.flags = c_EXCEPTION_CAUGHT | c_EXCEPTION_UNWIND;
  args.stack = stack;
  args.exception_handler_depth = stack->m_depth;
  args.pid = stack->m_owningThread.m_pid;

  debug_breakpoint(&args);
  }

uint32_t CLR_RT_ExecutionEngine::WaitSystemEvents(uint32_t powerLevel, uint32_t events, int64_t timeExpire)
  {
 
  int32_t timeout;

  uint32_t res = 0;

  m_currentNextActivityTime = timeExpire + HAL_Time_CurrentTime();

  timeout = (int32_t)timeExpire / TIME_CONVERSION__TO_MILLISECONDS;

  if (timeout == 0)
    timeout = 1;

   // wait for a wakeup call (some sort of event).
  CPU_Sleep();
  //res = ::Events_WaitForEvents(powerLevel, events, timeout);
  // UNDONE: FIXME
  // ::Watchdog_GetSetEnabled( bool, bool );
  // TODO check if the watchdog needs to be feed here... don't think so
  Watchdog_Reset();

  m_currentNextActivityTime = 0;

  return res;
  }
