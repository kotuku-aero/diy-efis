#include "../canflylib/CanFly_CoreLibrary.h"

#include "../../neutron/neutron.h"
#include "../../photon/photon.h"

static void CopyCanMsg(const canmsg_t* msg, CLR_RT_HeapBlock* pThis)
  {
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___flags].NumericByRef().u2 = msg->flags;
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data0].NumericByRef().u1 = msg->raw[0];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data1].NumericByRef().u1 = msg->raw[1];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data2].NumericByRef().u1 = msg->raw[2];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data3].NumericByRef().u1 = msg->raw[3];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data4].NumericByRef().u1 = msg->raw[4];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data5].NumericByRef().u1 = msg->raw[5];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data6].NumericByRef().u1 = msg->raw[6];
  pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data7].NumericByRef().u1 = msg->raw[7];
  }

static void CopyCanMsg(const CLR_RT_HeapBlock* pThis, canmsg_t* msg)
  {
  msg->flags = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___flags].NumericByRefConst().u2;
  msg->raw[0] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data0].NumericByRefConst().u1;
  msg->raw[1] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data1].NumericByRefConst().u1;
  msg->raw[2] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data2].NumericByRefConst().u1;
  msg->raw[3] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data3].NumericByRefConst().u1;
  msg->raw[4] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data4].NumericByRefConst().u1;
  msg->raw[5] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data5].NumericByRefConst().u1;
  msg->raw[6] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data6].NumericByRefConst().u1;
  msg->raw[7] = pThis[Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::FIELD___data7].NumericByRefConst().u1;
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U2__BOOLEAN(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param1));

  canmsg_t msg;

  create_can_msg_int32(&msg, param0, 0, 0);
  msg.canas.data_type = param1;

  CopyCanMsg(&msg, stack.This());

  // NANOCLR_CHECK_HRESULT(hr);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint32(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int32(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint16(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__I2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  int16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT16(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int16(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U2__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 2, param1));

  uint16_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 3, param2));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint16_2(&msg, param0, 0, param1, param2), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__I2__I2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  int16_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT16(stack, 2, param1));

  int16_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT16(stack, 3, param2));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int16_2(&msg, param0, 0, param1, param2), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__R4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  float param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_float(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_float(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__CHAR(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  char param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int8(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U1__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint8_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 2, param1));

  uint8_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 3, param2));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint8_2(&msg, param0, 0, param1, param2), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__CHAR__CHAR(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  char param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 2, param1));

  char param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 3, param2));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int8_2(&msg, param0, 0, param1, param2), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U1__U1__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint8_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 2, param1));

  uint8_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 3, param2));

  uint8_t param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 4, param3));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint8_3(&msg, param0, 0, param1, param2, param3), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__CHAR__CHAR__CHAR(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  char param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 2, param1));

  char param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 3, param2));

  char param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 4, param3));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int8_3(&msg, param0, 0, param1, param2, param3), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U1__U1__U1__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint8_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 2, param1));

  uint8_t param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 3, param2));

  uint8_t param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 4, param3));

  uint8_t param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 5, param4));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint8_4(&msg, param0, 0, param1, param2, param3, param4), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__CHAR__CHAR__CHAR__CHAR(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  char param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 2, param1));

  char param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 3, param2));

  char param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 4, param3));

  char param4;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_CHAR(stack, 5, param4));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_int8_4(&msg, param0, 0, param1, param2, param3, param4), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::_ctor___VOID__U2__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  uint8_t param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 2, param1));

  canmsg_t msg;
  CopyCanMsg(create_can_msg_uint8(&msg, param0, 0, param1), stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::get_Length___U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  SetResult_UINT8(stack, get_can_len(&msg));

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetFlags___U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  SetResult_INT8(stack, msg.flags);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::SetFlags___VOID__U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint16_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT16(stack, 1, param0));

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  msg.flags = param0;

  CopyCanMsg(&msg, stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::get_Id___U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  uint16_t id = get_can_id(&msg);

  SetResult_UINT16(stack, id);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::get_NodeId___U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  SetResult_UINT8(stack, msg.canas.node_id);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::set_NodeId___VOID__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint8_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 1, param0));

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  msg.canas.node_id = param0;

  CopyCanMsg(&msg, stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetDataType___U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  SetResult_UINT16(stack, msg.canas.data_type);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::get_ServiceCode___U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  SetResult_UINT8(stack, msg.canas.service_code);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::set_ServiceCode___VOID__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint8_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 1, param0));

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  msg.canas.service_code = param0;

  CopyCanMsg(&msg, stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::get_MessageCode___U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  SetResult_UINT8(stack, msg.canas.message_code);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::set_MessageCode___VOID__U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  uint8_t param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT8(stack, 1, param0));

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  msg.canas.message_code = param0;

  CopyCanMsg(&msg, stack.This());

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetFloat___R4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  float result;
  hr = get_param_float(&msg, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_float(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetInt32___I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int32_t result;
  hr = get_param_int32(&msg, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_INT32(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetUInt32___U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  uint32_t result;
  hr = get_param_uint32(&msg, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetInt16___I2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int16_t result;
  hr = get_param_int16(&msg, 0, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_INT16(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetUInt16___U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int16_t result;
  hr = get_param_int16(&msg, 0, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_INT16(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetInt8___I1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int8_t result;
  hr = get_param_int8(&msg, 0, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_INT8(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetUInt8___U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  uint8_t result;
  hr = get_param_uint8(&msg, 0, &result);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT8(stack, result);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetInt16Array___SZARRAY_I2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();
  hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int len = 0;

  int16_t val[2];

  switch (msg.canas.data_type)
    {
    case CANAS_DATATYPE_SHORT:
      len = 1;
      hr = get_param_int16(&msg, 0, &val[0]);
      break;
    case CANAS_DATATYPE_SHORT2:
      len = 2;
      hr = get_param_int16(&msg, 0, &val[0]);
      NANOCLR_CHECK_HRESULT(hr);
      hr = get_param_int16(&msg, 1, &val[1]);
      break;
    default:
      hr = e_invalid_operation;
      break;
    }

  NANOCLR_CHECK_HRESULT(hr);

  CLR_RT_HeapBlock& ret = stack.PushValueAndClear();
  CLR_RT_HeapBlock_Array* arr;
  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(ret, (CLR_UINT32)len, g_CLR_RT_WellKnownTypes.m_Int16));
  arr = ret.DereferenceArray();

  int16_t* values = (int16_t*)arr->GetFirstElement();
  for (int i = 0; i < len; i++)
    values[i] = val[i];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetUInt16Array___SZARRAY_U2(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();
  hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int len = 0;

  uint16_t val[2];

  switch (msg.canas.data_type)
    {
    case CANAS_DATATYPE_USHORT2:
      len = 2;
      hr = get_param_uint16(&msg, 1, &val[1]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_USHORT:
      if (len == 0)
        len = 1;
      hr = get_param_uint16(&msg, 0, &val[0]);
      break;
    default:
      hr = e_invalid_operation;
      break;
    }

  NANOCLR_CHECK_HRESULT(hr);

  CLR_RT_HeapBlock& ret = stack.PushValueAndClear();
  CLR_RT_HeapBlock_Array* arr;
  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(ret, (CLR_UINT32)len, g_CLR_RT_WellKnownTypes.m_UInt16));
  arr = ret.DereferenceArray();

  uint16_t* values = (uint16_t*)arr->GetFirstElement();
  for (int i = 0; i < len; i++)
    values[i] = val[i];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetInt8Array___SZARRAY_I1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();
  hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int len = 0;

  int8_t val[4];

  switch (msg.canas.data_type)
    {
    case CANAS_DATATYPE_CHAR4:
      len = 4;
      hr = get_param_int8(&msg, 3, &val[3]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_CHAR3:
      if (len == 0)
        len = 3;
      hr = get_param_int8(&msg, 2, &val[2]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_CHAR2:
      if (len == 0)
        len = 2;
      hr = get_param_int8(&msg, 1, &val[1]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_CHAR:
      if (len == 0)
        len = 1;
      hr = get_param_int8(&msg, 0, &val[0]);
      break;
    default:
      hr = e_invalid_operation;
      break;
    }

  NANOCLR_CHECK_HRESULT(hr);

  CLR_RT_HeapBlock& ret = stack.PushValueAndClear();
  CLR_RT_HeapBlock_Array* arr;
  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(ret, (CLR_UINT32)len, g_CLR_RT_WellKnownTypes.m_Int8));
  arr = ret.DereferenceArray();

  int8_t* values = (int8_t*)arr->GetFirstElement();
  for (int i = 0; i < len; i++)
    values[i] = val[i];

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetUInt8Array___SZARRAY_U1(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();
  hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  int len = 0;

  uint8_t val[4];

  switch (msg.canas.data_type)
    {
    case CANAS_DATATYPE_CHAR4:
      len = 4;
      hr = get_param_uint8(&msg, 3, &val[3]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_CHAR3:
      if (len == 0)
        len = 3;
      hr = get_param_uint8(&msg, 2, &val[2]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_CHAR2:
      if (len == 0)
        len = 2;
      hr = get_param_uint8(&msg, 1, &val[1]);
      NANOCLR_CHECK_HRESULT(hr);
    case CANAS_DATATYPE_CHAR:
      if (len == 0)
        len = 1;
      hr = get_param_uint8(&msg, 0, &val[0]);
      break;
    default:
      hr = e_invalid_operation;
      break;
    }

  NANOCLR_CHECK_HRESULT(hr);

  CLR_RT_HeapBlock& ret = stack.PushValueAndClear();
  CLR_RT_HeapBlock_Array* arr;
  NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(ret, (CLR_UINT32)len, g_CLR_RT_WellKnownTypes.m_UInt8));
  arr = ret.DereferenceArray();

  uint8_t* values = (uint8_t*)arr->GetFirstElement();
  for (int i = 0; i < len; i++)
    values[i] = val[i];

  NANOCLR_NOCLEANUP();
  }


HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::ToString___STRING(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER();
  hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  uint16_t id = get_can_id(&msg);

  char retValue[256];

  const char* id_name = get_name_from_id(id);
  const char* data_type = get_datatype_name(msg.canas.data_type);

  if (id_name == 0)
    printf_s("%s(%d)", retValue, data_type, id);
  else
    printf_s("%s(%s)", retValue, data_type, id_name);

  SetResult_LPCSTR(stack, retValue);
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::SendMessage___VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param0));

  // param0 is the handle to a window (hwnd)
  handle_t hwnd = (handle_t)param0;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  hr = send_message(hwnd, &msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::PostMessage___STATIC__VOID__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 2, param1));

  // param0 is the handle to a window (hwnd)
  handle_t hwnd = (handle_t)param0;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  hr = post_message(hwnd, &msg, param1);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::Send___STATIC__VOID(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  hr = can_send(&msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::SendRaw___STATIC__VOID(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  hr = can_send_raw(&msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::SendReply___STATIC__VOID(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  hr = can_send_reply(&msg);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::GetMessage___STATIC__BOOLEAN__U4__BYREF_U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  handle_t hwnd;
  canmsg_t msg;

  bool retValue = get_message((handle_t)param0, &hwnd, &msg);

  stack.Arg1().NumericByRef().u4 = (uint32_t)hwnd;

  // copy it to the object
  CopyCanMsg(&msg, stack.This());

  SetResult_bool(stack, retValue);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_CanFlyMsg::DispatchMessage___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  canmsg_t msg;
  CopyCanMsg(stack.This(), &msg);

  dispatch_message((handle_t)param0, &msg);

  NANOCLR_NOCLEANUP();
  }
