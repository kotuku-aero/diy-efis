#include "CanFlyEventDispatcher.h"
#include "CLR_Message.h"

CLR_RT_DblLinkedList CanFlyEventDispatcher::eventList;

void CanFlyEventDispatcher::HandlerMethod_Initialize()
  {
  CanFlyEventDispatcher::eventList.DblLinkedList_Initialize();
  }

void CanFlyEventDispatcher::HandlerMethod_RecoverFromGC()
  {

  NANOCLR_FOREACH_NODE(CanFlyEventDispatcher, event, CanFlyEventDispatcher::eventList)
    {
    event->RecoverFromGC();
    }
  NANOCLR_FOREACH_NODE_END();
  }

void CanFlyEventDispatcher::HandlerMethod_CleanUp()
  {
  CanFlyEventDispatcher* event;

  while (NULL != (event = (CanFlyEventDispatcher*)CanFlyEventDispatcher::eventList.FirstValidNode()))
    {
    event->DetachAll();
    event->ReleaseWhenDeadEx();
    }
  }

HRESULT CanFlyEventDispatcher::CreateInstance(CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& eventRef)
  {
  NANOCLR_HEADER();

  CanFlyEventDispatcher* event = NULL;

  event = EVENTCACHE_EXTRACT_NODE(g_CLR_RT_EventCache, CanFlyEventDispatcher, DATATYPE_IO_PORT); CHECK_ALLOCATION(event);

  {

  CLR_RT_ProtectFromGC gc(*event);

  event->Initialize();

  eventList.LinkAtBack(event);

  NANOCLR_CHECK_HRESULT(CLR_RT_ObjectToEvent_Source::CreateInstance(event, owner, eventRef));

  }

  NANOCLR_CLEANUP();

  if (FAILED(hr))
    {
    if (event)
      event->ReleaseWhenDead();
    }

  NANOCLR_CLEANUP_END();
  }

HRESULT CanFlyEventDispatcher::ExtractInstance(CLR_RT_HeapBlock& ref, CanFlyEventDispatcher*& event)
  {
  NANOCLR_HEADER();

  CLR_RT_ObjectToEvent_Source* src = CLR_RT_ObjectToEvent_Source::ExtractInstance(ref);
  FAULT_ON_NULL(src);

  event = (CanFlyEventDispatcher*)src->m_eventPtr;

  NANOCLR_NOCLEANUP();
  }

void CanFlyEventDispatcher::RecoverFromGC()
  {
  CheckAll();

  ReleaseWhenDeadEx();
  }

bool CanFlyEventDispatcher::ReleaseWhenDeadEx()
  {
  if (!IsReadyForRelease()) 
    return false;

  //remove any queued messages
  NANOCLR_FOREACH_NODE(CanFlyMsgEvent, message, g_CLR_Message.m_MessageData.m_applicationQueue)
    {
    if (this == message->m_message.context)
      {
      message->Unlink();

      --g_CLR_Message.m_MessageData.m_queuedMessages;

      ThreadTerminationCallback(message);
      }
    }
  NANOCLR_FOREACH_NODE_END();

  return ReleaseWhenDead();
  }

void CanFlyEventDispatcher::RemoveFromIonQueue()
  {
  // Since we are going to analyze and update the queue we need to disable interrupts.
  // Interrupt service routines add records to this queue.
  CLR_UINT32 elemCount = 0;
  GLOBAL_LOCK();
  elemCount = g_CLR_Message.m_MessageData.m_HalQueue.NumberOfElements();
  GLOBAL_UNLOCK();

  // For all elements in the queue
  for (CLR_UINT32 curElem = 0; curElem < elemCount; curElem++)
    {
    // Retrieve the element ( actually remove it from the queue )
    CLR_Message::CLRMessage* testRec = NULL;
    GLOBAL_LOCK();
    testRec = g_CLR_Message.m_MessageData.m_HalQueue.Pop();
    GLOBAL_UNLOCK();

    // Check if context of this record points to the instance of CLR_RT_HeapBlock_CanFlyMsgDispatcher
    // If the "context" is the same as "this", then we skip the "Push" and record is removed.
    if (testRec->m_context != this)
      {
      // If it is different from this instance of CLR_RT_HeapBlock_CanFlyMsgDispatcher, thin push it back
      CLR_Message::CLRMessage* newRec = NULL;
      GLOBAL_LOCK();
      newRec = g_CLR_Message.m_MessageData.m_HalQueue.Push();
      GLOBAL_UNLOCK();

      newRec->AssignFrom(*testRec);
      }
    }
  }

void CanFlyEventDispatcher::SaveToIonQueue(const canmsg_t *msg)
  {
  NATIVE_PROFILE_CLR_IOPORT();

  CLR_Message::CLRMessage* rec = g_CLR_Message.m_MessageData.m_HalQueue.Push();

  if (rec == NULL)
    {
    // remove the oldest message to make room for the newest
    g_CLR_Message.m_MessageData.m_HalQueue.Pop();

    rec = g_CLR_Message.m_MessageData.m_HalQueue.Push();
    }

  if (rec)
    {
    memcpy(&rec->msg, msg, sizeof(canmsg_t));
    }

  //::Events_Set(SYSTEM_EVENT_HW_INTERRUPT);
  }

void SaveNativeEventToHALQueue(CanFlyEventDispatcher* pContext, const canmsg_t *msg)
  {
  pContext->SaveToIonQueue(msg);
  }

void CleanupNativeEventsFromHALQueue(CanFlyEventDispatcher* pContext)
  {
  pContext->RemoveFromIonQueue();
  }

HRESULT CanFlyEventDispatcher::StartDispatch(CanFlyMsgEvent *eventMessage, CLR_RT_Thread* th)
  {
  NANOCLR_HEADER();

  CLR_RT_StackFrame* stackTop;
  CLR_RT_HeapBlock* args;
  CLR_RT_HeapBlock_Delegate* dlg;
  CLR_RT_HeapBlock* event;
  const CLR_UINT64 c_UTCMask = 0x8000000000000000ULL;

  CanFlyMsgEventData & message = eventMessage->m_message;

  NANOCLR_CHECK_HRESULT(RecoverManagedObject(event));
  dlg = event[Library_CanFly_CoreLibrary_CanFly_CanFlyEventDispatcher::FIELD__threadSpawn].DereferenceDelegate(); 
  FAULT_ON_NULL(dlg);

  NANOCLR_CHECK_HRESULT(th->PushThreadProcDelegate(dlg));

  stackTop = th->CurrentFrame();

  args = stackTop->m_arguments;

  if ((stackTop->m_call.m_target->flags & CLR_RECORD_METHODDEF::MD_Static) == 0)
    {
    ++args;
    }

  //
  // set values for delegate arguments
  //
  args[0].SetInteger(message.msg.flags);
  args[1].SetInteger(message.msg.raw[0]);
  args[2].SetInteger(message.msg.raw[1]);
  args[3].SetInteger(message.msg.raw[2]);
  args[4].SetInteger(message.msg.raw[3]);
  args[5].SetInteger(message.msg.raw[4]);
  args[6].SetInteger(message.msg.raw[5]);
  args[7].SetInteger(message.msg.raw[6]);
  args[8].SetInteger(message.msg.raw[7]);

  th->m_terminationCallback = CanFlyEventDispatcher::ThreadTerminationCallback;
  th->m_terminationParameter = eventMessage;

  NANOCLR_NOCLEANUP();
  }

void CanFlyEventDispatcher::ThreadTerminationCallback(void* arg)
  {
  CanFlyMsgEvent *eventMessage = (CanFlyMsgEvent *)arg;

  CLR_RT_Memory::Release(eventMessage);

  g_CLR_Message.SpawnDispatcher();
  }

HRESULT CanFlyEventDispatcher::RecoverManagedObject(CLR_RT_HeapBlock*& event)
  {
  NATIVE_PROFILE_CLR_IOPORT();

  NANOCLR_HEADER();

  // recover the managed object
  NANOCLR_FOREACH_NODE(CLR_RT_ObjectToEvent_Source, ref, this->m_references)
    {
    if (ref->m_objectPtr)
      {
      event = ref->m_objectPtr;
      NANOCLR_SET_AND_LEAVE(S_OK);
      }
    }
  NANOCLR_FOREACH_NODE_END();

  event = NULL;

  NANOCLR_SET_AND_LEAVE(CLR_E_PIN_DEAD);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyEventDispatcher::Dispose___VOID__BOOLEAN(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  bool param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_bool(stack, 1, param0));

  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyEventDispatcher::_ctor___VOID(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }
