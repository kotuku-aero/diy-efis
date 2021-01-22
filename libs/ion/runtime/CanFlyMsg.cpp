#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"

#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

HRESULT Library_corlib_native_CanFly_Syscall::SendMessage___STATIC__I4__U4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned int param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  unsigned short param1;
  if (FAILED(hr = Interop_Marshal_UINT16(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  canmsg_t msg;
  msg.flags = param1;
  msg.raw[0] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[1] = stack.ArgN(3).NumericByRef().u1;
  msg.raw[2] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[3] = stack.ArgN(5).NumericByRef().u1;
  msg.raw[4] = stack.ArgN(6).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(7).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(8).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(9).NumericByRef().u1;

  SetResult_INT32(stack, send_message((handle_t)param0, &msg));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PostMessage___STATIC__I4__U4__U4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  unsigned int param0;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 0, param0)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  unsigned int param1;
  if (FAILED(hr = Interop_Marshal_UINT32(stack, 1, param1)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  unsigned short param2;
  if (FAILED(hr = Interop_Marshal_UINT16(stack, 2, param2)))
    {
    SetResult_INT32(stack, hr);
    return S_OK;
    }

  canmsg_t msg;
  msg.flags = param2;
  msg.raw[0] = stack.ArgN(3).NumericByRef().u1;
  msg.raw[1] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[2] = stack.ArgN(5).NumericByRef().u1;
  msg.raw[3] = stack.ArgN(6).NumericByRef().u1;
  msg.raw[4] = stack.ArgN(7).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(8).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(9).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(10).NumericByRef().u1;

  SetResult_INT32(stack, post_message((handle_t)param0, &msg, param1));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::Send___STATIC__I4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  canmsg_t msg;
  msg.flags = stack.ArgN(0).NumericByRef().u2;
  msg.raw[0] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[1] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[2] = stack.ArgN(3).NumericByRef().u1;
  msg.raw[3] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[4] = stack.ArgN(5).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(6).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(7).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(8).NumericByRef().u1;

  SetResult_INT32(stack, can_send(&msg));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SendRaw___STATIC__I4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;


  canmsg_t msg;
  msg.flags = stack.ArgN(0).NumericByRef().u2;
  msg.raw[0] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[1] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[2] = stack.ArgN(3).NumericByRef().u1;
  msg.raw[3] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[4] = stack.ArgN(5).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(6).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(7).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(8).NumericByRef().u1;

  SetResult_INT32(stack, can_send_raw(&msg));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::SendReply___STATIC__I4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;


  canmsg_t msg;
  msg.flags = stack.ArgN(0).NumericByRef().u2;
  msg.raw[0] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[1] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[2] = stack.ArgN(3).NumericByRef().u1;
  msg.raw[3] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[4] = stack.ArgN(5).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(6).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(7).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(8).NumericByRef().u1;

  SetResult_INT32(stack, can_send_reply(&msg));

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetFloat___STATIC__I4__U1__U1__U1__U1__BYREF_R4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(3).NumericByRef().u1;

  float value;
  hr = get_param_float(&msg, &value);
  SetResult_INT32(stack, hr);
  if(succeeded(hr))
    stack.Arg4().Dereference()->SetFloat(value);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetInt32___STATIC__I4__U1__U1__U1__U1__BYREF_I4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(3).NumericByRef().u1;

  int32_t value;
  hr = get_param_int32(&msg, &value);
  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg4().Dereference()->SetInteger(value);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetUInt32___STATIC__I4__U1__U1__U1__U1__BYREF_U4(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(3).NumericByRef().u1;

  uint32_t value;
  hr = get_param_uint32(&msg, &value);
  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg4().Dereference()->SetInteger(value);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetInt16___STATIC__I4__U1__U1__BYREF_I2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;

  int16_t value;
  hr = get_param_int16(&msg, 0, &value);
  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg4().Dereference()->SetInteger(value);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetUInt16___STATIC__I4__U1__U1__BYREF_U2(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;

  uint16_t value;
  hr = get_param_uint16(&msg, 0, &value);
  SetResult_INT32(stack, hr);
  if (succeeded(hr))
    stack.Arg4().Dereference()->SetInteger(value);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackFloat___STATIC__I4__R4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  float value = stack.Arg0().NumericByRef().r4;
  canmsg_t msg;
  create_can_msg_float(&msg, 0, 0, value);
  
  SetResult_INT32(stack, 0);
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];
  stack.Arg3().NumericByRef().u1 = msg.raw[6];
  stack.Arg4().NumericByRef().u1 = msg.raw[7];

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackInt32___STATIC__I4__I4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  int32_t value = stack.Arg0().NumericByRef().s4;
  canmsg_t msg;
  create_can_msg_int32(&msg, 0, 0, value);

  SetResult_INT32(stack, 0);
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];
  stack.Arg3().NumericByRef().u1 = msg.raw[6];
  stack.Arg4().NumericByRef().u1 = msg.raw[7];

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackUInt32___STATIC__I4__U4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint32_t value = stack.Arg0().NumericByRef().u4;
  canmsg_t msg;
  create_can_msg_uint32(&msg, 0, 0, value);

  SetResult_INT32(stack, 0);
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];
  stack.Arg3().NumericByRef().u1 = msg.raw[6];
  stack.Arg4().NumericByRef().u1 = msg.raw[7];

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackInt16___STATIC__I4__I2__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  int16_t value = stack.Arg0().NumericByRef().s2;
  canmsg_t msg;
  create_can_msg_int16(&msg, 0, 0, value);

  SetResult_INT32(stack, 0);
  stack.Arg1().Dereference()->SetInteger(msg.raw[4]);
  stack.Arg2().Dereference()->SetInteger(msg.raw[5]);

  return S_OK;
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackUInt16___STATIC__I4__U2__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  HRESULT hr;

  uint16_t value = stack.Arg0().NumericByRef().u2;
  canmsg_t msg;
  create_can_msg_uint16(&msg, 0, 0, value);

  SetResult_INT32(stack, s_ok);
  stack.Arg1().Dereference()->SetInteger(msg.raw[4]);
  stack.Arg2().Dereference()->SetInteger(msg.raw[5]);

  return S_OK;
  }
