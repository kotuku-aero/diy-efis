//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"
#include "../Include/nanoCLR_Interop.h"

///////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH DateTime.DateTimePart (in managed code) !!! //
///////////////////////////////////////////////////////////////////////
enum DateTimePart
  {
  DateTimePart_Year,
  DateTimePart_Month,
  DateTimePart_Day,
  DateTimePart_DayOfWeek,
  DateTimePart_DayOfYear,
  DateTimePart_Hour,
  DateTimePart_Minute,
  DateTimePart_Second,
  DateTimePart_Millisecond
  };

////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH DateTime._UTCMask (in managed code)  !!! //
// !!! KEEP IN SYNC WITH DateTime._tickMask (in managed code) !!! //
////////////////////////////////////////////////////////////////////
CLR_INT64 s_UTCMask = ULONGLONGCONSTANT(0x8000000000000000);
CLR_INT64 s_TickMask = ULONGLONGCONSTANT(0x7FFFFFFFFFFFFFFF);

HRESULT Library_corlib_native_CanFly_Runtime::ToTicks___STATIC__U8__I4__I4__I4__I4__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_RT_HeapBlock *pArg = &(stack.Arg0());
  SYSTEMTIME        st;
  CLR_INT64 value;

  NANOCLR_CLEAR(st);
  st.wYear = pArg[0].NumericByRef().s4;
  st.wMonth = pArg[1].NumericByRef().s4;
  st.wDay = pArg[2].NumericByRef().s4;
  st.wHour = pArg[3].NumericByRef().s4;
  st.wMinute = pArg[4].NumericByRef().s4;
  st.wSecond = pArg[5].NumericByRef().s4;
  st.wMilliseconds = pArg[6].NumericByRef().s4;

  /// Our current supported range is between 1601 and 3000. Years before 1582 requires different calculation (see explanation
  /// in time_decl.h), same way years after 3000 will not hold the simple arithmetic which we are using. More complex calculations
  /// outside these range are not worth the CPU cycle and codesize.
  // some checks below have to use the parameter because the st struct uses unsigned types
  if (
    (st.wYear < 1601) ||
    (st.wYear > 3000) ||
    (st.wMonth < 1) ||
    (st.wMonth > 12) ||
    (st.wDay < 1) ||
    (st.wDay > 31) ||
    (pArg[3].NumericByRef().s4 < 0) ||
    (st.wHour > 23) ||
    (pArg[4].NumericByRef().s4 < 0) ||
    (st.wMinute > 59) ||
    (pArg[5].NumericByRef().s4 < 0) ||
    (st.wSecond > 59) ||
    (st.wMilliseconds < pArg[6].NumericByRef().s4) ||
    (st.wMilliseconds > 1000))
    {
    NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }


  value = (CLR_INT64)HAL_Time_ConvertFromSystemTime(&st) | s_UTCMask;
  stack.SetResult_I8(value);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::GetDateTimePart___STATIC__I4__U8__I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  signed int days;
  SYSTEMTIME st;

  CLR_INT64 value = stack.Arg0().NumericByRef().s8;
  DateTimePart dateTimePart = (DateTimePart)stack.Arg1().NumericByRef().s4;

  HAL_Time_ToSystemTime(value, &st);

  switch (dateTimePart)
    {
    case DateTimePart_Year:
      stack.SetResult_I4(st.wYear);
      break;

    case DateTimePart_Month:
      stack.SetResult_I4(st.wMonth);
      break;

    case DateTimePart_Day:
      stack.SetResult_I4(st.wDay);
      break;

    case DateTimePart_DayOfWeek:
      stack.SetResult_I4(st.wDayOfWeek);
      break;

    case DateTimePart_DayOfYear:
      NANOCLR_CHECK_HRESULT(HAL_Time_AccDaysInMonth(st.wYear, st.wMonth, &days));
      days += st.wDay;
      stack.SetResult_I4(days);
      break;

    case DateTimePart_Hour:
      stack.SetResult_I4(st.wHour);
      break;

    case DateTimePart_Minute:
      stack.SetResult_I4(st.wMinute);
      break;

    case DateTimePart_Second:
      stack.SetResult_I4(st.wSecond);
      break;

    case DateTimePart_Millisecond:
      stack.SetResult_I4(st.wMilliseconds);
      break;

    default:
      NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
      break;
    }

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::DaysInMonth___STATIC__I4__I4__I4(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_INT32 year = stack.Arg0().NumericByRef().s4;
  CLR_INT32 month = stack.Arg1().NumericByRef().s4;
  CLR_INT32 days = 0;

  NANOCLR_CHECK_HRESULT(HAL_Time_DaysInMonth(year, month, &days));

  stack.SetResult_I4(days);

  NANOCLR_NOCLEANUP();
  }

HRESULT Library_corlib_native_CanFly_Runtime::UtcNow___STATIC__U8(CLR_RT_StackFrame &stack)
  {
  NATIVE_PROFILE_CLR_CORE();
  NANOCLR_HEADER();

  CLR_INT64 val;

  val = HAL_Time_CurrentDateTime(false) | s_UTCMask;
  stack.SetResult_I8(val);

  NANOCLR_NOCLEANUP();
  }
