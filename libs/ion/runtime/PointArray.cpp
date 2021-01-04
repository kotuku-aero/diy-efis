#include "../canflylib/CanFly_CoreLibrary.h"

#include "../../../neutron/neutron.h"
#include "../../../photon/photon.h"

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayCreate___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  vector_p hndl;
  hr = vector_create(sizeof(point_t), &hndl);
  NANOCLR_CHECK_HRESULT(hr);
  hr = vector_expand(hndl, (uint16_t)param0, 0, 0);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, (uint32_t) hndl);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayRelease___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  hr = vector_close((vector_p)param0);
   
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArraySetPoint___STATIC__VOID__U4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  vector_p hndl = (vector_p)param0;
  point_t pt = { param2, param3 };

  hr = vector_set(hndl, (uint16_t)param1, &pt);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayClear___STATIC__VOID__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  vector_p hndl = (vector_p)param0;
  hr = vector_clear(hndl);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArraySize___STATIC__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;
  {

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  vector_p hndl = (vector_p)param0;
  uint16_t count;
  hr = vector_count(hndl, &count);

  NANOCLR_CHECK_HRESULT(hr);
  SetResult_UINT32(stack, count);
  }
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayResize___STATIC__VOID__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  vector_p hndl = (vector_p)param0;
  hr = vector_expand(hndl, (uint16_t)param1, 0, 0);

  NANOCLR_CHECK_HRESULT(hr);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayAppend___STATIC__U4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  vector_p hndl = (vector_p)param0;
  point_t pt = { param1, param2 };

  hr = vector_push_back(hndl, &pt);  
  NANOCLR_CHECK_HRESULT(hr);

  uint16_t retValue;
  hr = vector_count(hndl, &retValue);
  NANOCLR_CHECK_HRESULT(hr);

  SetResult_UINT32(stack, retValue);
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayInsertAt___STATIC__VOID__U4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  signed int param3;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 3, param3));

  vector_p hndl = (vector_p)param0;
  point_t pt = { param2, param3 };

  hr = vector_insert(hndl, (uint16_t)param1, &pt);
  NANOCLR_CHECK_HRESULT(hr);

  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayGetPoint___STATIC__VOID__U4__U4__BYREF_I4__BYREF_I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  vector_p hndl = (vector_p)param0;
  point_t pt;

  hr = vector_at(hndl, (uint16_t)param1, &pt);
  NANOCLR_CHECK_HRESULT(hr);

  stack.Arg2().NumericByRef().s4 = pt.x;
  stack.Arg3().NumericByRef().s4 = pt.y;
  
  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayIndexOf___STATIC__I4__U4__I4__I4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  signed int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 1, param1));

  signed int param2;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_INT32(stack, 2, param2));

  vector_p hndl = (vector_p)param0;
  point_t pt = { param1, param2 };

  const point_t* begin;
  const point_t* end;

  hr = vector_begin(hndl, (void**)&begin);
  NANOCLR_CHECK_HRESULT(hr);
  hr = vector_end(hndl, (void**)&end);
  NANOCLR_CHECK_HRESULT(hr);

  int32_t retValue = 0;
  for (; begin < end; begin++, retValue++)
    {
    if (begin->x == pt.x && begin->y == pt.y)
      break;
    }

  if (begin == end)
    retValue = -1;

  SetResult_INT32(stack, retValue);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_CanFly_CoreLibrary_CanFly_PointArray::PointArrayRemoveAt___STATIC__VOID__U4__U4(CLR_RT_StackFrame& stack)
  {
  NANOCLR_HEADER(); hr = S_OK;

  unsigned int param0;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 0, param0));

  unsigned int param1;
  NANOCLR_CHECK_HRESULT(Interop_Marshal_UINT32(stack, 1, param1));

  vector_p hndl = (vector_p)param0;

  hr = vector_erase(hndl, (uint16_t)param1);

  NANOCLR_NOCLEANUP();
  }
