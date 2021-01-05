#ifndef __CLR_Message_h__
#define __CLR_Message_h__

#include "../../nano/CLR/Include/nanoCLR_Types.h"
#include "../../nano/CLR/Include/nanoCLR_Runtime.h"

#include "../../neutron/neutron.h"


struct CLR_Message
  {
  struct CLRMessage
    {
    CLR_RT_HeapBlock_Node* m_context;

    canmsg_t msg;


    void AssignFrom(const CLRMessage& recFrom)
      {
      m_context = recFrom.m_context;
      memcpy(&msg, &recFrom.msg, sizeof(canmsg_t));
      }
    };

  struct MessageData
    {
    Hal_Queue_UnknownSize<CLRMessage> m_HalQueue;
    CLR_RT_DblLinkedList m_applicationQueue;
    CLR_UINT32 m_queuedMessages;
    };

  //--//

  bool m_fInitialized;

  MessageData m_MessageData;

  CLR_UINT32 m_DebuggerEventsMask;

  CLR_UINT32 m_wakeupEvents;

  //--//

  static HRESULT CreateInstance();

  HRESULT Message_Initialize();

  static HRESULT DeleteInstance();

  void Message_Cleanup();

  void PrepareForGC();
  void ProcessActivity();

  HRESULT ManagedMessage_Initialize();
  HRESULT ProcessInterrupts();
  HRESULT SpawnDispatcher();
  HRESULT TransferAllInterruptsToApplicationQueue();
  };

// the global CLR CanFly Message handler
extern CLR_Message g_CLR_Message;

#endif
