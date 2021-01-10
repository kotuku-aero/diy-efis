/*
#include "../../nano/CLR/Include/nanoCLR_Interop.h"
#include "../../nano/CLR/Include/nanoCLR_Runtime.h"
#include "../../nano/CLR/Include/nanoPackStruct.h"
*/

#include "../canflylib/CanFly_CoreLibrary.h"

#include "../../photon/photon.h"
#include "../../photon/window.h"

static CLR_RT_Thread *canFlyEventThread;
static semaphore_p mutex = 0;

static void thread_termination_callback(void *arg)
  {
  // release the next message to the delegate thread
  if (mutex != 0)
    semaphore_signal(mutex);
  }

static result_t photon_event(handle_t hwnd, struct _event_proxy_t *proxy, const canmsg_t *msg)
  {
  result_t result;
  // find the hwnd wnd data
  void *wndData;
  if(failed(result = get_wnddata(hwnd, &wndData)))
    return result;

  // block till thread is available.
  semaphore_wait(mutex, INDEFINITE_WAIT);

  CLR_RT_StackFrame *stackTop;
  CLR_RT_HeapBlock *args;
  CLR_RT_HeapBlock *widgetObject = (CLR_RT_HeapBlock *)wndData;
  CLR_RT_HeapBlock_Delegate *dlg;
  CLR_RT_MethodDef_Instance inst;
  dlg = widgetObject[Library_CanFly_CoreLibrary_CanFly_Widget::FIELD__threadSpawn].DereferenceDelegate();
  if (dlg == 0)
    return s_false;

  if (canFlyEventThread->PushThreadProcDelegate(dlg) != S_OK)
    return s_false;

  stackTop = canFlyEventThread->CurrentFrame();

  args = stackTop->m_arguments;

  // if a static call then skip the this parameter.
  if ((stackTop->m_call.m_target->flags & CLR_RECORD_METHODDEF::MD_Static) == 0)
    ++args;

  //
  // set values for delegate arguments
  //
  args[0].SetInteger(get_can_id(msg));
  args[1].SetInteger(msg->flags);
  args[2].SetInteger(msg->raw[0]);
  args[3].SetInteger(msg->raw[1]);
  args[4].SetInteger(msg->raw[2]);
  args[5].SetInteger(msg->raw[3]);
  args[6].SetInteger(msg->raw[4]);
  args[7].SetInteger(msg->raw[5]);
  args[8].SetInteger(msg->raw[6]);
  args[9].SetInteger(msg->raw[7]);

  canFlyEventThread->m_terminationCallback = thread_termination_callback;
  canFlyEventThread->m_terminationParameter = 0;

  return s_ok;
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Widget::_cctor___STATIC__VOID(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  // create the message worker thread
  if (!g_CLR_RT_ExecutionEngine.EnsureSystemThread(canFlyEventThread, ThreadPriority::System_Highest))
    return S_OK;

  // stop more than 1 use of the thread.
  semaphore_create(&mutex);
  semaphore_signal(mutex);

  NANOCLR_CHECK_HRESULT(hr);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_Widget::AddEvent___VOID__U4__U2(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  uint32_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param1));

  CLR_RT_HeapBlock *widgetObject = stack.This();

  add_event((handle_t)param0, param1, widgetObject, photon_event);
 
  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }
