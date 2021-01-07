#include "CLR_Message.h"
#include "CanFlyEventDispatcher.h"

#define NUMBER_MESSAGES 32

size_t InterruptRecords() { return NUMBER_MESSAGES; }

static CLR_Message::CLRMessage canMsgStorage[(NUMBER_MESSAGES * NUMBER_MESSAGES)];

HRESULT CLR_Message::Message_Initialize()
  {
  NANOCLR_HEADER();

  m_MessageData.m_HalQueue.Initialize((CLR_Message::CLRMessage *)&canMsgStorage, NUMBER_MESSAGES);

  m_MessageData.m_applicationQueue.DblLinkedList_Initialize();

  m_MessageData.m_queuedMessages = 0;

  NANOCLR_NOCLEANUP_NOLABEL();
  }

HRESULT CLR_Message::SpawnDispatcher()
  {
  NATIVE_PROFILE_CLR_HARDWARE();

  NANOCLR_HEADER();

  CanFlyMsgEvent *canFlyEvent;
  CanFlyEventDispatcher * canFlyEventDispatcher;
  CanFlyEventDispatcher::CanFlyMsgEventData * canFlyEventData;

  // if reboot is in progress, just bail out
  if (CLR_EE_DBG_IS(RebootPending))
    {
    return S_OK;
    }

  canFlyEvent = (CanFlyMsgEvent *)m_MessageData.m_applicationQueue.FirstValidNode();

  if ((canFlyEvent == NULL) || !g_CLR_RT_ExecutionEngine.EnsureSystemThread(g_CLR_RT_ExecutionEngine.m_canFlyEventThread, ThreadPriority::System_Highest))
    {
    return S_OK;
    }

  canFlyEvent->Unlink();

  canFlyEventData = &canFlyEvent->m_message;
  canFlyEventDispatcher = canFlyEventData->context;

  CLR_RT_ProtectFromGC gc1(*canFlyEventDispatcher);

  NANOCLR_SET_AND_LEAVE(canFlyEventDispatcher->StartDispatch(canFlyEvent, g_CLR_RT_ExecutionEngine.m_canFlyEventThread));

  NANOCLR_CLEANUP();

  if (FAILED(hr))
    {
    canFlyEventDispatcher->ThreadTerminationCallback(canFlyEvent);
    }

  --m_MessageData.m_queuedMessages;

  NANOCLR_CLEANUP_END();
  }

HRESULT CLR_Message::TransferAllMessagesToApplicationQueue()
  {
  NATIVE_PROFILE_CLR_HARDWARE();

  NANOCLR_HEADER();

  while (true)
    {
    CLR_Message::CLRMessage* rec;

    {
    GLOBAL_LOCK();

    rec = m_MessageData.m_HalQueue.Peek();

    GLOBAL_UNLOCK();
    }

    if (rec == NULL) 
      break;

    CanFlyMsgEvent *queueRec = (CanFlyMsgEvent *)CLR_RT_Memory::Allocate_And_Erase(sizeof(CanFlyMsgEvent), CLR_RT_HeapBlock::HB_CompactOnFailure);  
    CHECK_ALLOCATION(queueRec);

    queueRec->m_message.msg.flags = rec->msg.flags;
    queueRec->m_message.msg.raw[0] = rec->msg.raw[0];
    queueRec->m_message.msg.raw[1] = rec->msg.raw[1];
    queueRec->m_message.msg.raw[2] = rec->msg.raw[2];
    queueRec->m_message.msg.raw[3] = rec->msg.raw[3];
    queueRec->m_message.msg.raw[4] = rec->msg.raw[4];
    queueRec->m_message.msg.raw[5] = rec->msg.raw[5];
    queueRec->m_message.msg.raw[6] = rec->msg.raw[6];
    queueRec->m_message.msg.raw[7] = rec->msg.raw[7];

    queueRec->m_message.context = (CanFlyEventDispatcher *)rec->m_context;

    m_MessageData.m_applicationQueue.LinkAtBack(queueRec); ++m_MessageData.m_queuedMessages;

    {
    GLOBAL_LOCK();

    m_MessageData.m_HalQueue.Pop();

    GLOBAL_UNLOCK();
    }
    }

  if (m_MessageData.m_queuedMessages == 0)
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_NO_INTERRUPT);
    }

  NANOCLR_CLEANUP();

  if (CLR_E_OUT_OF_MEMORY == hr)
    {
    // if there is no memory left discard all interrupts to avoid getting into a death spiral of OOM exceptions
        {
        GLOBAL_LOCK();

        while (!m_MessageData.m_HalQueue.IsEmpty())
          {
          m_MessageData.m_HalQueue.Pop();
          }

        GLOBAL_UNLOCK();
        }
    }

  NANOCLR_CLEANUP_END();
  }

HRESULT CLR_Message::ProcessInterrupts()
  {
  NATIVE_PROFILE_CLR_HARDWARE();

  NANOCLR_HEADER();

  NANOCLR_CHECK_HRESULT(TransferAllMessagesToApplicationQueue());

  SpawnDispatcher();

  NANOCLR_NOCLEANUP();
  }
