#include "../nano/CLR/CorLib/mscorlib/corlib_native.h"

#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

HRESULT Library_corlib_native_CanFly_Syscall::GetMessage___STATIC__BOOLEAN__U4__BYREF_U4__BYREF_U2__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hwnd;
  canmsg_t msg;
  if (!get_message((handle_t) param0, &hwnd, &msg) != s_ok)
    {
    SetResult_bool(stack, false);
    return S_OK;
    }

  stack.Arg1().NumericByRef().u4 = (CLR_UINT32) hwnd;
  stack.Arg1().NumericByRef().u2 = msg.flags;
  stack.Arg2().NumericByRef().u1 = msg.raw[0];
  stack.Arg3().NumericByRef().u1 = msg.raw[1];
  stack.Arg4().NumericByRef().u1 = msg.raw[2];
  stack.Arg5().NumericByRef().u1 = msg.raw[3];
  stack.Arg6().NumericByRef().u1 = msg.raw[4];
  stack.Arg7().NumericByRef().u1 = msg.raw[5];
  stack.ArgN(8).NumericByRef().u1 = msg.raw[6];
  stack.ArgN(9).NumericByRef().u1 = msg.raw[7];

  hr = S_OK;
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::DispatchMessage___STATIC__VOID__U4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned short param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

  canmsg_t msg;
  msg.flags = param1;
  msg.raw[0] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[1] = stack.ArgN(3).NumericByRef().u1;
  msg.raw[2] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[3] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[4] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(4).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(4).NumericByRef().u1;

  hr = send_message((handle_t)param0, &msg);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::SendMessage___STATIC__VOID__U4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned short param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param1));

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

  hr = send_message((handle_t)param0, &msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::PostMessage___STATIC__VOID__U4__U4__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  unsigned short param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param2));

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

  hr = post_message((handle_t)param0, &msg, param1);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::Send___STATIC__VOID__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

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

  hr = can_send(&msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::SendRaw___STATIC__VOID__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();


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

  hr = can_send_raw(&msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::SendReply___STATIC__VOID__U2__U1__U1__U1__U1__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();


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

  hr = can_send_reply(&msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetFloat___STATIC__R4__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(3).NumericByRef().u1;

  float value;
  hr = get_param_float(&msg, &value);
  NANOCLR_CHECK_HRESULT(hr);
  
  stack.SetResult_R8(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetInt32___STATIC__I4__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(3).NumericByRef().u1;

  int32_t value;
  hr = get_param_int32(&msg, &value);
  NANOCLR_CHECK_HRESULT(hr);

  stack.SetResult_I4(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetUInt32___STATIC__U4__U1__U1__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;
  msg.raw[6] = stack.ArgN(2).NumericByRef().u1;
  msg.raw[7] = stack.ArgN(3).NumericByRef().u1;

  uint32_t value;
  hr = get_param_uint32(&msg, &value);
  NANOCLR_CHECK_HRESULT(hr);

  stack.SetResult_U4(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetInt16___STATIC__I2__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;

  int16_t value;
  hr = get_param_int16(&msg, 0, &value);
  NANOCLR_CHECK_HRESULT(hr);

  stack.SetResult_I2(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::GetUInt16___STATIC__U2__U1__U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  canmsg_t msg;
  msg.raw[4] = stack.ArgN(0).NumericByRef().u1;
  msg.raw[5] = stack.ArgN(1).NumericByRef().u1;

  uint16_t value;
  hr = get_param_uint16(&msg, 0, &value);
  NANOCLR_CHECK_HRESULT(hr);

  stack.SetResult_U2(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackFloat___STATIC__VOID__R4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  float value = stack.Arg0().NumericByRef().r4;
  canmsg_t msg;
  create_can_msg_float(&msg, 0, 0, value);
  
  hr = S_OK;
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];
  stack.Arg3().NumericByRef().u1 = msg.raw[6];
  stack.Arg4().NumericByRef().u1 = msg.raw[7];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackInt32___STATIC__VOID__I4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  int32_t value = stack.Arg0().NumericByRef().s4;
  canmsg_t msg;
  create_can_msg_int32(&msg, 0, 0, value);

  hr = S_OK;
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];
  stack.Arg3().NumericByRef().u1 = msg.raw[6];
  stack.Arg4().NumericByRef().u1 = msg.raw[7];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackUInt32___STATIC__VOID__U4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  uint32_t value = stack.Arg0().NumericByRef().u4;
  canmsg_t msg;
  create_can_msg_uint32(&msg, 0, 0, value);

  hr = S_OK;
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];
  stack.Arg3().NumericByRef().u1 = msg.raw[6];
  stack.Arg4().NumericByRef().u1 = msg.raw[7];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackInt16___STATIC__VOID__I2__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  int16_t value = stack.Arg0().NumericByRef().s2;
  canmsg_t msg;
  create_can_msg_int16(&msg, 0, 0, value);

  hr = S_OK;
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Syscall::PackUInt16___STATIC__VOID__U2__BYREF_U1__BYREF_U1(CLR_RT_StackFrame &stack)
  {
  NANOCLR_HEADER();

  uint16_t value = stack.Arg0().NumericByRef().u2;
  canmsg_t msg;
  create_can_msg_uint16(&msg, 0, 0, value);

  hr = S_OK;
  stack.Arg1().NumericByRef().u1 = msg.raw[4];
  stack.Arg2().NumericByRef().u1 = msg.raw[5];

  NANOCLR_NOCLEANUP();
  }
