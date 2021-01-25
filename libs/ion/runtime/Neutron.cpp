#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"
#include "../../neutron/neutron.h"


HRESULT Library_corlib_native_CanFly_Syscall::RegCreateKey___STATIC__I4__U4__STRING__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  memid_t key;
  hr = reg_create_key((memid_t)param0, param1, &key);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)key);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegOpenKey___STATIC__I4__U4__STRING__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  memid_t key;
  hr = reg_open_key((memid_t)param0, param1, &key);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger((uint32_t)key);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetInt8___STATIC__I4__U4__STRING__BYREF_I1(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  int8_t retValue;
  hr = reg_get_int8((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetInt8___STATIC__I4__U4__STRING__I1(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  int8_t param2;
  if(FAILED(hr =Interop_Marshal_INT8(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_int8((memid_t)param0, param1, param2));
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetUint8___STATIC__I4__U4__STRING__BYREF_U1(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  uint8_t retValue;
  hr = reg_get_uint8((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetUint8___STATIC__I4__U4__STRING__U1(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;
  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  uint8_t param2;
  if(FAILED(hr =Interop_Marshal_UINT8(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_uint8((memid_t)param0, param1, param2));
  

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetInt16___STATIC__I4__U4__STRING__BYREF_I2(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  int16_t retValue;
  hr = reg_get_int16((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetInt16___STATIC__I4__U4__STRING__I2(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  int16_t param2;
  if(FAILED(hr =Interop_Marshal_INT16(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_int16((memid_t)param0, param1, param2));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetUint16___STATIC__I4__U4__STRING__BYREF_U2(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  uint16_t retValue;
  hr = reg_get_uint16((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetUint16___STATIC__I4__U4__STRING__U2(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  uint16_t param2;
  if(FAILED(hr =Interop_Marshal_UINT16(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
      return S_OK;
    }

  stack.SetResult_I4(reg_set_uint16((memid_t)param0, param1, param2));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetInt32___STATIC__I4__U4__STRING__BYREF_I4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  signed int retValue;
  hr = reg_get_int32((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetInt32___STATIC__I4__U4__STRING__I4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  signed int param2;
  if(FAILED(hr =Interop_Marshal_INT32(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_int32((memid_t)param0, param1, param2));
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetUint32___STATIC__I4__U4__STRING__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  unsigned int retValue;
  hr = reg_get_uint32((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetInteger(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetUint32___STATIC__I4__U4__STRING__U4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  unsigned int param2;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_uint32((memid_t)param0, param1, param2));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetString___STATIC__I4__U4__STRING__BYREF_STRING(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  char retValue[REG_STRING_MAX+1];
  uint16_t len = REG_STRING_MAX;

  hr = reg_get_string((memid_t)param0, param1, retValue, &len);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    CLR_RT_HeapBlock_String::CreateInstance(*stack.Arg2().Dereference(), retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetString___STATIC__I4__U4__STRING__STRING(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param2;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_string((memid_t)param0, param1, param2));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetFloat___STATIC__I4__U4__STRING__BYREF_R4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  float retValue;
  hr = reg_get_float((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetFloat(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetFloat___STATIC__I4__U4__STRING__R4(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  float param2;
  if(FAILED(hr =Interop_Marshal_float(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_float((memid_t)param0, param1, param2));
  

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegGetBool___STATIC__I4__U4__STRING__BYREF_BOOLEAN(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  bool retValue;
  hr = reg_get_bool((memid_t)param0, param1, &retValue);
  SetResult_INT32(stack, hr);

  if (succeeded(hr))
    stack.Arg2().Dereference()->SetBoolean(retValue);
  
  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RegSetBool___STATIC__I4__U4__STRING__BOOLEAN(CLR_RT_StackFrame& stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  const char* param1;
  if(FAILED(hr =Interop_Marshal_LPCSTR(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  bool param2;
  if(FAILED(hr =Interop_Marshal_bool(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  stack.SetResult_I4(reg_set_bool((memid_t)param0, param1, param2));

  return S_OK;
  }


HRESULT Library_corlib_native_CanFly_Syscall::RegEnumKey___STATIC__I4__U4__BYREF_U4__BYREF_STRING(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned int param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  char retValue[REG_NAME_MAX + 1];
  uint16_t child = stack.Arg2().NumericByRef().u2;
  field_datatype dt = field_datatype::field_key;

  hr = reg_enum_key(param0, &dt, 0, 0, REG_NAME_MAX, retValue, &child);

  if (succeeded(hr))
    {
    stack.Arg2().NumericByRef().u2 = child;
    CLR_RT_HeapBlock_String::CreateInstance(stack.Arg3(), retValue);
    }

  SetResult_INT32(stack, hr);

  return S_OK;
  }

// note we don't support delegate event types here. Is in the c# code
struct registered_event_t {
  uint16_t canID;
  CLR_RT_HeapBlock_Delegate *callback;
  };

struct queued_event_t {
  canmsg_t msg;
  CLR_RT_HeapBlock_Delegate *callback;
  };

// initially just stored in a vector, will be a map or something one day
static vector_p events = 0;
static semaphore_p events_mutex = 0;

static CLR_RT_Thread *canFlyEventThread;
static semaphore_p clr_thread_mutex = 0;

static task_p msg_worker_task;
static deque_p pending_calls;

static msg_hook_t ion_hook;

static void thread_termination_callback(void *arg)
  {
  // release the next message to the delegate thread
  if (clr_thread_mutex != 0)
    semaphore_signal(clr_thread_mutex);
  }

result_t call_delegate(queued_event_t *msg)
  {
  // block till thread is available.
  semaphore_wait(clr_thread_mutex, INDEFINITE_WAIT);
  if (canFlyEventThread->PushThreadProcDelegate(msg->callback) != S_OK)
    return s_false;

  CLR_RT_StackFrame *stackTop = canFlyEventThread->CurrentFrame();

  CLR_RT_HeapBlock *args = stackTop->m_arguments;

  // if a static call then skip the this parameter.
  if ((stackTop->m_call.m_target->flags & CLR_RECORD_METHODDEF::MD_Static) == 0)
    ++args;

  // set values for delegate arguments
  args[0].SetInteger(msg->msg.flags);
  args[1].SetInteger(msg->msg.raw[0]);
  args[2].SetInteger(msg->msg.raw[1]);
  args[3].SetInteger(msg->msg.raw[2]);
  args[4].SetInteger(msg->msg.raw[3]);
  args[5].SetInteger(msg->msg.raw[4]);
  args[6].SetInteger(msg->msg.raw[5]);
  args[7].SetInteger(msg->msg.raw[6]);
  args[8].SetInteger(msg->msg.raw[7]);

  canFlyEventThread->m_terminationCallback = thread_termination_callback;
  canFlyEventThread->m_terminationParameter = 0;

  return s_ok;
  }

static void msg_worker(void *)
  {
  queued_event_t cb_event;
  while (succeeded(pop_front(pending_calls, &cb_event, INDEFINITE_WAIT)))
    {
    call_delegate(&cb_event);
    }
  }

// called by photon to queue a message to the CLR runtime.
// the delegate is a widget specific function delegate
result_t queue_callback(CLR_RT_HeapBlock_Delegate *dlg, const canmsg_t *msg)
  {
  queued_event_t cb_event;
  cb_event.callback = dlg;
  memcpy(&cb_event.msg, msg, sizeof(canas_msg_t));

  // we don't wait for the queue to empty.  If it is full then
  // this is an overrun condition
  return push_back(pending_calls, &cb_event, 0);
  }

extern "C" {
  bool ion_hook_fn(const canmsg_t *msg, void *parg)
    {
    // if someone is adding to or removing a handler then we don't post
    // any events while that is happening.
    if (events != 0 && succeeded(semaphore_wait(events_mutex, 0)))
      {
      uint16_t len;
      if (failed(vector_count(events, &len)))
        return false;

      uint16_t id = get_can_id(msg);

      for (uint16_t index = 0; index < len; index++)
        {
        registered_event_t ev;
        if (failed(vector_at(events, index, &ev)))
          continue;

        if (ev.canID == id)
          {
          // call the delegate on a worker thread so that
          // it can spin waiting for the CLR code to
          // process the message.
          queue_callback(ev.callback, msg);
          }
        }

      semaphore_signal(events_mutex);
      }

    return true;
    }
  }

result_t neutron_clr_init()
  {
  result_t result;
  // create the message worker thread
  if (!g_CLR_RT_ExecutionEngine.EnsureSystemThread(canFlyEventThread, ThreadPriority_Highest + 1))
    return e_unexpected;

  // stop more than 1 use of the thread.
  if (failed(result = semaphore_create(&clr_thread_mutex)) ||
    failed(result = semaphore_signal(clr_thread_mutex)))
    return result;

  // create a vector to hols all events being listened for.
  if (failed(result = vector_create(sizeof(registered_event_t), &events)) ||
    failed(result = semaphore_create(&events_mutex)) ||
    failed(result = semaphore_signal(events_mutex)))
    return result;

  if (failed(result = deque_create(sizeof(queued_event_t), 64, &pending_calls)))
    {
    trace_error("Cannot create the neutron_clr event queue");
    return result;
    }

  if (failed(result = task_create("CLREVENT", DEFAULT_STACK_SIZE, msg_worker, 0, HIGH_PRIORITY, &msg_worker_task)))
    {
    trace_error("Cannot create the neutron_clr event callback worker");
    return result;
    }

  // hook the message queue.
  ion_hook.callback = ion_hook_fn;
  ion_hook.parg = 0;

  if (failed(result = subscribe(&ion_hook)))
    return result;


  return s_ok;
  }

HRESULT Library_corlib_native_CanFly_Syscall::AddEvent___STATIC__I4__U2__CanFlyCanFlyEventHandler(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned short param0;
  if(FAILED(hr =Interop_Marshal_UINT16(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  CLR_RT_HeapBlock_Delegate *callback = stack.Arg1().DereferenceDelegate();

  semaphore_wait(events_mutex, INDEFINITE_WAIT);

  uint16_t len;
  if (failed(vector_count(events, &len)))
    {
    stack.SetResult_I4(e_bad_handle);
    return S_OK;
    }

  bool found = false;
  registered_event_t ev;
  for (uint16_t index = 0; index < len; index++)
    {
    if (failed(vector_at(events, index, &ev)))
      break;

    if (ev.canID == param0)
      {
      found = true;
      break;
      }
    }

  if (found)
    {
    // link the delegate into the delegate chain
    // TODO : is this correct????
    hr = CLR_RT_HeapBlock_Delegate_List::Combine(stack.PushValue(), *ev.callback, stack.Arg1(), false);
    }
  else
    {
    ev.canID = param0;
    ev.callback = callback;
    }

  stack.SetResult_I4(semaphore_signal(events_mutex));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::RemoveEvent___STATIC__I4__U2__CanFlyCanFlyEventHandler(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned int param0;
  if(FAILED(hr =Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  CLR_RT_HeapBlock_Delegate *callback = stack.Arg1().DereferenceDelegate();

  semaphore_wait(events_mutex, INDEFINITE_WAIT);

  uint16_t len;
  if (failed(vector_count(events, &len)))
    {
    stack.SetResult_I4(e_invalid_handle);
    return S_OK;
    }

  bool found = false;
  registered_event_t ev;
  for (uint16_t index = 0; index < len; index++)
    {
    if (failed(vector_at(events, index, &ev)))
      break;

    if (ev.canID == param0)
      {
      // unlink the delegate
      hr = CLR_RT_HeapBlock_Delegate_List::Remove(stack.PushValue(), *callback, stack.Arg1());
      break;
      }
    }

  stack.SetResult_I4(semaphore_signal(events_mutex));

  return S_OK;
  }