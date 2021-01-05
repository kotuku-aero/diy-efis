#ifndef __HeapBlockDispatcher_h__
#define __HeapBlockDIspatcher_h__

#include "../canflylib/CanFly_CoreLibrary.h"

#include "../../neutron/neutron.h"

struct CLR_RT_CanFlyMsgEvent;

struct CLR_RT_HeapBlock_CanFlyMsgDispatcher : public CLR_RT_ObjectToEvent_Destination // EVENT HEAP - NO RELOCATION -
  {
  static void HandlerMethod_Initialize();
  static void HandlerMethod_RecoverFromGC();
  static void HandlerMethod_CleanUp();

  static CLR_RT_DblLinkedList eventList;

  struct CanFlyMsgEvent
    {
    CLR_RT_HeapBlock_CanFlyMsgDispatcher* context;
    canmsg_t msg;
    };

  static HRESULT CreateInstance(CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& portRef);
  static HRESULT ExtractInstance(CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_CanFlyMsgDispatcher*& port);

  HRESULT StartDispatch(CLR_RT_CanFlyMsgEvent* message, CLR_RT_Thread* th);
  HRESULT RecoverManagedObject(CLR_RT_HeapBlock*& port);

  static void ThreadTerminationCallback(void* arg);
  void SaveToProtonQueue(const canmsg_t* msg);
  void RemoveFromProtonQueue();

  void RecoverFromGC();
  bool ReleaseWhenDeadEx();
  };

extern CLR_RT_HeapBlock_CanFlyMsgDispatcher g_CLR_MessageDispatcher;

struct CLR_RT_CanFlyMsgEvent : public CLR_RT_HeapBlock_Node
  {
  CLR_RT_HeapBlock_CanFlyMsgDispatcher::CanFlyMsgEvent m_message;
  };

#endif
