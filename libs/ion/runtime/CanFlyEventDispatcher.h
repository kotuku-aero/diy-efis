#ifndef __HeapBlockDispatcher_h__
#define __HeapBlockDIspatcher_h__

#include "../canflylib/CanFly_CoreLibrary.h"

#include "../../neutron/neutron.h"

struct CanFlyMsgEvent;

struct CanFlyEventDispatcher : public CLR_RT_ObjectToEvent_Destination // EVENT HEAP - NO RELOCATION -
  {
  static void HandlerMethod_Initialize();
  static void HandlerMethod_RecoverFromGC();
  static void HandlerMethod_CleanUp();

  static CLR_RT_DblLinkedList eventList;

  struct CanFlyMsgEventData
    {
    CanFlyEventDispatcher *context;
    canmsg_t msg;
    };

  static HRESULT CreateInstance(CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& portRef);
  static HRESULT ExtractInstance(CLR_RT_HeapBlock& ref, CanFlyEventDispatcher*& port);

  HRESULT StartDispatch(CanFlyMsgEvent *message, CLR_RT_Thread* th);
  HRESULT RecoverManagedObject(CLR_RT_HeapBlock*& port);

  static void ThreadTerminationCallback(void* arg);
  void SaveToIonQueue(const canmsg_t* msg);
  void RemoveFromIonQueue();

  void RecoverFromGC();
  bool ReleaseWhenDeadEx();
  };

extern CanFlyEventDispatcher g_CLR_MessageDispatcher;

struct CanFlyMsgEvent : public CLR_RT_HeapBlock_Node
  {
  CanFlyEventDispatcher::CanFlyMsgEventData m_message;
  };

#endif
