//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#ifndef _CORLIB_NATIVE_H_
#define _CORLIB_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack Base64FormattingOptions
{
    Base64FormattingOptions_None = 0,
    Base64FormattingOptions_InsertLineBreaks = 1,
} Base64FormattingOptions;

typedef enum __nfpack EditorBrowsableState
{
    EditorBrowsableState_Always = 0,
    EditorBrowsableState_Never = 1,
    EditorBrowsableState_Advanced = 2,
} EditorBrowsableState;

typedef enum __nfpack DateTime_DateTimePart
{
    DateTime_DateTimePart_Year = 0,
    DateTime_DateTimePart_Month = 1,
    DateTime_DateTimePart_Day = 2,
    DateTime_DateTimePart_DayOfWeek = 3,
    DateTime_DateTimePart_DayOfYear = 4,
    DateTime_DateTimePart_Hour = 5,
    DateTime_DateTimePart_Minute = 6,
    DateTime_DateTimePart_Second = 7,
    DateTime_DateTimePart_Millisecond = 8,
} DateTime_DateTimePart;

typedef enum __nfpack DateTimeKind
{
    DateTimeKind_Utc = 1,
    DateTimeKind_Local = 2,
} DateTimeKind;

typedef enum __nfpack DayOfWeek
{
    DayOfWeek_Sunday = 0,
    DayOfWeek_Monday = 1,
    DayOfWeek_Tuesday = 2,
    DayOfWeek_Wednesday = 3,
    DayOfWeek_Thursday = 4,
    DayOfWeek_Friday = 5,
    DayOfWeek_Saturday = 6,
} DayOfWeek;

typedef enum __nfpack DebuggableAttribute_DebuggingModes
{
    DebuggableAttribute_DebuggingModes_None = 0,
    DebuggableAttribute_DebuggingModes_Default = 1,
    DebuggableAttribute_DebuggingModes_DisableOptimizations = 256,
    DebuggableAttribute_DebuggingModes_IgnoreSymbolStoreSequencePoints = 2,
    DebuggableAttribute_DebuggingModes_EnableEditAndContinue = 4,
} DebuggableAttribute_DebuggingModes;

typedef enum __nfpack DebuggerBrowsableState
{
    DebuggerBrowsableState_Never = 0,
    DebuggerBrowsableState_Collapsed = 2,
    DebuggerBrowsableState_RootHidden = 3,
} DebuggerBrowsableState;

typedef enum __nfpack IOException_IOExceptionErrorCode
{
    IOException_IOExceptionErrorCode_Others = -536870912,
    IOException_IOExceptionErrorCode_InvalidDriver = -520093696,
    IOException_IOExceptionErrorCode_FileNotFound = -503316480,
    IOException_IOExceptionErrorCode_DirectoryNotFound = -486539264,
    IOException_IOExceptionErrorCode_VolumeNotFound = -469762048,
    IOException_IOExceptionErrorCode_PathTooLong = -452984832,
    IOException_IOExceptionErrorCode_DirectoryNotEmpty = -436207616,
    IOException_IOExceptionErrorCode_UnauthorizedAccess = -419430400,
    IOException_IOExceptionErrorCode_PathAlreadyExists = -402653184,
    IOException_IOExceptionErrorCode_TooManyOpenHandles = -385875968,
} IOException_IOExceptionErrorCode;

typedef enum __nfpack SeekOrigin
{
    SeekOrigin_Begin = 0,
    SeekOrigin_Current = 1,
    SeekOrigin_End = 2,
} SeekOrigin;

typedef enum __nfpack ThreadPriority
{
    ThreadPriority_Lowest = 0,
    ThreadPriority_BelowNormal = 1,
    ThreadPriority_Normal = 2,
    ThreadPriority_AboveNormal = 3,
    ThreadPriority_Highest = 4,
} ThreadPriority;

typedef enum __nfpack ThreadState
{
    ThreadState_Running = 0,
    ThreadState_StopRequested = 1,
    ThreadState_SuspendRequested = 2,
    ThreadState_Background = 4,
    ThreadState_Unstarted = 8,
    ThreadState_Stopped = 16,
    ThreadState_WaitSleepJoin = 32,
    ThreadState_Suspended = 64,
    ThreadState_AbortRequested = 128,
    ThreadState_Aborted = 256,
} ThreadState;

typedef enum __nfpack TypeCode
{
    TypeCode_Empty = 0,
    TypeCode_Object = 1,
    TypeCode_DBNull = 2,
    TypeCode_Boolean = 3,
    TypeCode_Char = 4,
    TypeCode_SByte = 5,
    TypeCode_Byte = 6,
    TypeCode_Int16 = 7,
    TypeCode_UInt16 = 8,
    TypeCode_Int32 = 9,
    TypeCode_UInt32 = 10,
    TypeCode_Int64 = 11,
    TypeCode_UInt64 = 12,
    TypeCode_Single = 13,
    TypeCode_Double = 14,
    TypeCode_Decimal = 15,
    TypeCode_DateTime = 16,
    TypeCode_String = 18,
} TypeCode;

struct Library_corlib_native_CanFly_Runtime
{
    NANOCLR_NATIVE_DECLARE(Copy___STATIC__VOID__SystemArray__I4__SystemArray__I4__I4);
    NANOCLR_NATIVE_DECLARE(Clear___STATIC__VOID__SystemArray__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetArrayLength___STATIC__I4__SystemArray);
    NANOCLR_NATIVE_DECLARE(GetArrayAt___STATIC__OBJECT__SystemArray__I4);
    NANOCLR_NATIVE_DECLARE(SetArrayAt___STATIC__VOID__SystemArray__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(TrySzIndexOf___STATIC__BOOLEAN__SystemArray__I4__I4__OBJECT__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(IsLittleEndian___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(DoubleToInt64Bits___STATIC__I8__R8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__CHAR);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__R8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__R4);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__I8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__I2);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__U4);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__U8);
    NANOCLR_NATIVE_DECLARE(GetBytes___STATIC__SZARRAY_U1__U2);
    NANOCLR_NATIVE_DECLARE(Int64BitsToDouble___STATIC__R8__I8);
    NANOCLR_NATIVE_DECLARE(ToBoolean___STATIC__BOOLEAN__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToChar___STATIC__CHAR__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToDouble___STATIC__R8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToInt16___STATIC__I2__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToInt32___STATIC__I4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToInt64___STATIC__I8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToSingle___STATIC__R4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToString___STATIC__STRING__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(ToString___STATIC__STRING__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToString___STATIC__STRING__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(ToUInt16___STATIC__U2__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToUInt32___STATIC__U4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(ToUInt64___STATIC__U8__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8__I4);
    NANOCLR_NATIVE_DECLARE(NativeToDouble___STATIC__R8__STRING);
    NANOCLR_NATIVE_DECLARE(CompareTo___STATIC__I4__R8__R8);
    NANOCLR_NATIVE_DECLARE(IsInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsNaN___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsNegativeInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsPositiveInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(HasFlag___STATIC__BOOLEAN__SystemEnum__SystemEnum);
    NANOCLR_NATIVE_DECLARE(AnyPendingFinalizers___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(SuppressFinalize___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(ReRegisterForFinalize___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(GetCurrentUICultureInternal___STATIC__SystemGlobalizationCultureInfo);
    NANOCLR_NATIVE_DECLARE(SetCurrentUICultureInternal___STATIC__VOID__SystemGlobalizationCultureInfo);
    NANOCLR_NATIVE_DECLARE(ToBase64String___STATIC__STRING__SZARRAY_U1__I4__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(FromBase64CharArray___STATIC__SZARRAY_U1__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(ToTicks___STATIC__U8__I4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(DaysInMonth___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(UtcNow___STATIC__U8);
    NANOCLR_NATIVE_DECLARE(GetDateTimePart___STATIC__I4__U8__I4);
    NANOCLR_NATIVE_DECLARE(DelegateEquals___STATIC__BOOLEAN__SystemDelegate__OBJECT);
    NANOCLR_NATIVE_DECLARE(DelegateCombine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(DelegateRemove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(DelegateEquals___STATIC__BOOLEAN__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(DelegateNotEquals___STATIC__BOOLEAN__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(SetCapacity___STATIC__VOID__SystemCollectionsArrayList__I4);
    NANOCLR_NATIVE_DECLARE(ArrayListGet___STATIC__OBJECT__SystemCollectionsArrayList__I4);
    NANOCLR_NATIVE_DECLARE(ArrayListSet___STATIC__VOID__SystemCollectionsArrayList__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(ArrayListAdd___STATIC__I4__SystemCollectionsArrayList__OBJECT);
    NANOCLR_NATIVE_DECLARE(ArrayListClear___STATIC__VOID__SystemCollectionsArrayList);
    NANOCLR_NATIVE_DECLARE(ArrayListInsert___STATIC__VOID__SystemCollectionsArrayList__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(ArrayListRemoveAt___STATIC__VOID__SystemCollectionsArrayList__I4);
    NANOCLR_NATIVE_DECLARE(OutNative___STATIC__VOID__STRING__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(DebuggerIsAttached___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(DebuggerBreak___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(FormatDigits___STATIC__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(GenerateNewGuid___STATIC__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(Abs___STATIC__I4__I4);
    NANOCLR_NATIVE_DECLARE(Min___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Max___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Equals___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate);
    NANOCLR_NATIVE_DECLARE(NotEquals___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate);
    NANOCLR_NATIVE_DECLARE(FormatNative___STATIC__STRING__OBJECT__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(ObjectEquals___STATIC__BOOLEAN__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(ObjectReferenceEquals___STATIC__BOOLEAN__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(ObjectGetHashCode___STATIC__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(ObjectMemberwiseClone___STATIC__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(RandomCtor___STATIC__VOID__SystemRandom);
    NANOCLR_NATIVE_DECLARE(RandomCtor___STATIC__VOID__SystemRandom__I4);
    NANOCLR_NATIVE_DECLARE(RandomNext___STATIC__I4__SystemRandom);
    NANOCLR_NATIVE_DECLARE(RandomNext___STATIC__I4__SystemRandom__I4);
    NANOCLR_NATIVE_DECLARE(RandomNextDouble___STATIC__R8__SystemRandom);
    NANOCLR_NATIVE_DECLARE(RandomNextBytes___STATIC__VOID__SystemRandom__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(InitializeArray___STATIC__VOID__SystemArray__SystemRuntimeFieldHandle);
    NANOCLR_NATIVE_DECLARE(GetObjectValue___STATIC__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(RunClassConstructor___STATIC__VOID__SystemRuntimeTypeHandle);
    NANOCLR_NATIVE_DECLARE(OffsetToStringData___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(IsTransparentProxy___STATIC__BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(Equals___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringCharAt___STATIC__CHAR__STRING__I4);
    NANOCLR_NATIVE_DECLARE(StringToCharArray___STATIC__SZARRAY_CHAR__STRING);
    NANOCLR_NATIVE_DECLARE(StringToCharArray___STATIC__SZARRAY_CHAR__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringLength___STATIC__I4__STRING);
    NANOCLR_NATIVE_DECLARE(StringSplit___STATIC__SZARRAY_STRING__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringSplit___STATIC__SZARRAY_STRING__STRING__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(StringSubstring___STATIC__STRING__STRING__I4);
    NANOCLR_NATIVE_DECLARE(StringSubstring___STATIC__STRING__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringTrim___STATIC__STRING__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringTrimStart___STATIC__STRING__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringTrimEnd___STATIC__STRING__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringCtor___STATIC__VOID__STRING__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringCtor___STATIC__VOID__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringCtor___STATIC__VOID__STRING__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(StringCompare___STATIC__I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringCompareTo___STATIC__I4__STRING__OBJECT);
    NANOCLR_NATIVE_DECLARE(StringCompareTo___STATIC__I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringIndexOf___STATIC__I4__STRING__CHAR);
    NANOCLR_NATIVE_DECLARE(StringIndexOf___STATIC__I4__STRING__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(StringIndexOf___STATIC__I4__STRING__CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringIndexOfAny___STATIC__I4__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringIndexOfAny___STATIC__I4__STRING__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(StringIndexOfAny___STATIC__I4__STRING__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringIndexOf___STATIC__I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringIndexOf___STATIC__I4__STRING__STRING__I4);
    NANOCLR_NATIVE_DECLARE(StringIndexOf___STATIC__I4__STRING__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOf___STATIC__I4__STRING__CHAR);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOf___STATIC__I4__STRING__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOf___STATIC__I4__STRING__CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOfAny___STATIC__I4__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOfAny___STATIC__I4__STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOfAny___STATIC__I4__STRING__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOfAny___STATIC__I4__STRING__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOf___STATIC__I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOf___STATIC__I4__STRING__STRING__I4);
    NANOCLR_NATIVE_DECLARE(StringLastIndexOf___STATIC__I4__STRING__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(StringToLower___STATIC__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringToUpper___STATIC__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringTrim___STATIC__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringConcat___STATIC__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringConcat___STATIC__STRING__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringConcat___STATIC__STRING__STRING__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(StringConcat___STATIC__STRING__SZARRAY_STRING);
    NANOCLR_NATIVE_DECLARE(AutoResetEventCtor___STATIC__VOID__SystemThreadingAutoResetEvent__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(AutoResetEventReset___STATIC__BOOLEAN__SystemThreadingAutoResetEvent);
    NANOCLR_NATIVE_DECLARE(AutoResetEventSet___STATIC__BOOLEAN__SystemThreadingAutoResetEvent);
    NANOCLR_NATIVE_DECLARE(Increment___STATIC__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(Decrement___STATIC__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(Exchange___STATIC__I4__BYREF_I4__I4);
    NANOCLR_NATIVE_DECLARE(CompareExchange___STATIC__I4__BYREF_I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(ManualResetEventCtor___STATIC__VOID__SystemThreadingManualResetEvent__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(ManualResetEventReset___STATIC__BOOLEAN__SystemThreadingManualResetEvent);
    NANOCLR_NATIVE_DECLARE(ManualResetEventSet___STATIC__BOOLEAN__SystemThreadingManualResetEvent);
    NANOCLR_NATIVE_DECLARE(MonitorEnter___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(MonitorExit___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(ThreadCtor___STATIC__VOID__SystemThreadingThread__SystemThreadingThreadStart);
    NANOCLR_NATIVE_DECLARE(ThreadStart___STATIC__VOID__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(ThreadAbort___STATIC__VOID__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(ThreadSuspend___STATIC__VOID__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(ThreadResume___STATIC__VOID__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(GetThreadPriority___STATIC__SystemThreadingThreadPriority__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(SetThreadPriority___STATIC__VOID__SystemThreadingThread__SystemThreadingThreadPriority);
    NANOCLR_NATIVE_DECLARE(GetManagedThreadId___STATIC__I4__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(ThreadIsAlive___STATIC__BOOLEAN__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(ThreadJoin___STATIC__VOID__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(ThreadJoin___STATIC__BOOLEAN__SystemThreadingThread__I4);
    NANOCLR_NATIVE_DECLARE(ThreadSleep___STATIC__VOID__I4);
    NANOCLR_NATIVE_DECLARE(GetCurrentThread___STATIC__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(GetThreadState___STATIC__SystemThreadingThreadState__SystemThreadingThread);
    NANOCLR_NATIVE_DECLARE(TimerCtor___STATIC__VOID__SystemThreadingTimer__SystemThreadingTimerCallback__OBJECT__I4__I4);
    NANOCLR_NATIVE_DECLARE(TimerChange___STATIC__BOOLEAN__SystemThreadingTimer__I4__I4);
    NANOCLR_NATIVE_DECLARE(TimerDispose___STATIC__VOID__SystemThreadingTimer);
    NANOCLR_NATIVE_DECLARE(WaitHandleWaitOne___STATIC__BOOLEAN__SystemThreadingWaitHandle__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(WaitMultiple___STATIC__I4__SZARRAY_SystemThreadingWaitHandle__I4__BOOLEAN__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(ValueTypeEquals___STATIC__BOOLEAN__SystemValueType__OBJECT);
    NANOCLR_NATIVE_DECLARE(WeakReferenceCtor___STATIC__VOID__WEAKCLASS__OBJECT);
    NANOCLR_NATIVE_DECLARE(WeakReferenceIsAlive___STATIC__BOOLEAN__WEAKCLASS);
    NANOCLR_NATIVE_DECLARE(GetWeakReferenceTarget___STATIC__OBJECT__WEAKCLASS);
    NANOCLR_NATIVE_DECLARE(SetWeakReferenceTarget___STATIC__VOID__WEAKCLASS__OBJECT);

    //--//

};

struct Library_corlib_native_CanFly_Syscall
{
    NANOCLR_NATIVE_DECLARE(RegCreateKey___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegOpenKey___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegGetInt8___STATIC__I1__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetInt8___STATIC__VOID__U4__STRING__I1);
    NANOCLR_NATIVE_DECLARE(RegGetUint8___STATIC__U1__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetUint8___STATIC__VOID__U4__STRING__U1);
    NANOCLR_NATIVE_DECLARE(RegGetInt16___STATIC__I2__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetInt16___STATIC__VOID__U4__STRING__I2);
    NANOCLR_NATIVE_DECLARE(RegGetUint16___STATIC__U2__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetUint16___STATIC__VOID__U4__STRING__U2);
    NANOCLR_NATIVE_DECLARE(RegGetInt32___STATIC__I4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetInt32___STATIC__VOID__U4__STRING__I4);
    NANOCLR_NATIVE_DECLARE(RegGetUint32___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetUint32___STATIC__VOID__U4__STRING__U4);
    NANOCLR_NATIVE_DECLARE(RegGetString___STATIC__STRING__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetString___STATIC__VOID__U4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(RegGetFloat___STATIC__R4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetFloat___STATIC__VOID__U4__STRING__R4);
    NANOCLR_NATIVE_DECLARE(RegGetBool___STATIC__BOOLEAN__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetBool___STATIC__VOID__U4__STRING__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(RegEnumKey___STATIC__STRING__U4__BYREF_U4);
    NANOCLR_NATIVE_DECLARE(OpenScreen___STATIC__U4__U2__U2);
    NANOCLR_NATIVE_DECLARE(CreateWindow___STATIC__U4__U4__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(CreateChildWindow___STATIC__U4__U4__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(CloseWindow___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(SendMessage___STATIC__VOID__U4__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(PostMessage___STATIC__VOID__U4__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(GetWindowRect___STATIC__VOID__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(GetWindowPos___STATIC__VOID__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(SetWindowPos___STATIC__VOID__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetWindowData___STATIC__OBJECT__U4);
    NANOCLR_NATIVE_DECLARE(SetWindowData___STATIC__VOID__U4__OBJECT);
    NANOCLR_NATIVE_DECLARE(GetParent___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetWindowById___STATIC__U4__U4__U2);
    NANOCLR_NATIVE_DECLARE(GetFirstChild___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetNextSibling___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetPreviousSibling___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(InsertBefore___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(InsertAfter___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetZOrder___STATIC__U1__U4);
    NANOCLR_NATIVE_DECLARE(SetZOrder___STATIC__VOID__U4__U1);
    NANOCLR_NATIVE_DECLARE(CanvasClose___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(CreateRectCanvas___STATIC__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(CreatePngCanvas___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(LoadPng___STATIC__VOID__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetCanvasExtents___STATIC__VOID__U4__BYREF_U2__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(GetOrientation___STATIC__U2__U4);
    NANOCLR_NATIVE_DECLARE(SetOrientation___STATIC__VOID__U4__U2);
    NANOCLR_NATIVE_DECLARE(CreatePen___STATIC__U4__U4__U2__U2);
    NANOCLR_NATIVE_DECLARE(GetPenColor___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetPenWidth___STATIC__U2__U4);
    NANOCLR_NATIVE_DECLARE(GetPenStyle___STATIC__U2__U4);
    NANOCLR_NATIVE_DECLARE(DisposePen___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(Polyline___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4);
    NANOCLR_NATIVE_DECLARE(Ellipse___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Polygon___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__U4);
    NANOCLR_NATIVE_DECLARE(Rectangle___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(RoundRect___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(BitBlt___STATIC__VOID__U4__I4__I4__I4__I4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetPixel___STATIC__U4__U4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(SetPixel___STATIC__U4__U4__I4__I4__I4__I4__I4__I4__U4);
    NANOCLR_NATIVE_DECLARE(Arc___STATIC__VOID__U4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Pie___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(OpenFont___STATIC__U4__STRING__U2);
    NANOCLR_NATIVE_DECLARE(DrawText___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__U4__STRING__I4__I4__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(TextExtent___STATIC__VOID__U4__U4__STRING__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(InvalidateRect___STATIC__VOID__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(IsValid___STATIC__BOOLEAN__U4);
    NANOCLR_NATIVE_DECLARE(BeginPaint___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(EndPaint___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(GetMessage___STATIC__BOOLEAN__U4__BYREF_U4__BYREF_U2__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1);
    NANOCLR_NATIVE_DECLARE(DispatchMessage___STATIC__VOID__U4__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(PostMessage___STATIC__VOID__U4__U4__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(Send___STATIC__VOID__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(SendRaw___STATIC__VOID__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(SendReply___STATIC__VOID__U2__U2__U1__U1__U1__U1__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(GetFloat___STATIC__R4__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(GetInt32___STATIC__I4__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(GetUInt32___STATIC__U4__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(GetInt16___STATIC__I2__U1__U1);
    NANOCLR_NATIVE_DECLARE(GetUInt16___STATIC__U2__U1__U1);
    NANOCLR_NATIVE_DECLARE(PackFloat___STATIC__VOID__R4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1);
    NANOCLR_NATIVE_DECLARE(PackInt32___STATIC__VOID__I4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1);
    NANOCLR_NATIVE_DECLARE(PackUInt32___STATIC__VOID__U4__BYREF_U1__BYREF_U1__BYREF_U1__BYREF_U1);
    NANOCLR_NATIVE_DECLARE(PackInt16___STATIC__VOID__I2__BYREF_U1__BYREF_U1);
    NANOCLR_NATIVE_DECLARE(PackUInt16___STATIC__VOID__U2__BYREF_U1__BYREF_U1);
    NANOCLR_NATIVE_DECLARE(StreamEof___STATIC__BOOLEAN__U4);
    NANOCLR_NATIVE_DECLARE(StreamRead___STATIC__SZARRAY_U1__U4__U2);
    NANOCLR_NATIVE_DECLARE(StreamWrite___STATIC__VOID__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(StreamGetPos___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamSetPos___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamLength___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamTruncate___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamCopy___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamPath___STATIC__STRING__U4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(StreamClose___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(StreamDelete___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(CreateDirectory___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(RemoveDirectory___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetDirectoryEnumerator___STATIC__U4__STRING);
    NANOCLR_NATIVE_DECLARE(FileStreamOpen___STATIC__U4__STRING);
    NANOCLR_NATIVE_DECLARE(FileStreamCreate___STATIC__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegStreamOpen___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegStreamCreate___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(GetDirectoryEntry___STATIC__BOOLEAN__U4__BYREF_STRING__BYREF_STRING);
    NANOCLR_NATIVE_DECLARE(RewindDirectoryEntry___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(GetFont___STATIC__U4__STRING__U4);
    NANOCLR_NATIVE_DECLARE(ReleaseFont___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(LoadFont___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayCreate___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayRelease___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(PointArraySetPoint___STATIC__VOID__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayClear___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(PointArraySize___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayResize___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayAppend___STATIC__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayInsertAt___STATIC__VOID__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayGetPoint___STATIC__VOID__U4__U4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(PointArrayIndexOf___STATIC__I4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayRemoveAt___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(IncarnatePhoton___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(AddEvent___STATIC__VOID__U4__U2);

    //--//

};

struct Library_corlib_native_System_Boolean
{
    static const int FIELD_STATIC__FalseString = 0;
    static const int FIELD_STATIC__TrueString = 1;

    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Globalization_CultureInfo
{
    static const int FIELD___numInfo = 1;
    static const int FIELD___dateTimeInfo = 2;
    static const int FIELD___cultureInfoName = 3;
    static const int FIELD___name = 4;
    static const int FIELD___parent = 5;

    //--//

};

struct Library_corlib_native_System_Globalization_NumberFormatInfo
{
    static const int FIELD___numberGroupSizes = 1;
    static const int FIELD___positiveSign = 2;
    static const int FIELD___negativeSign = 3;
    static const int FIELD___numberDecimalSeparator = 4;
    static const int FIELD___numberGroupSeparator = 5;
    static const int FIELD___cultureInfo = 6;

    //--//

};

struct Library_corlib_native_System_Int32
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Char
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_String
{
    static const int FIELD_STATIC__Empty = 2;

    //--//

};

struct Library_corlib_native_System_Exception
{
    static const int FIELD___message = 1;
    static const int FIELD___innerException = 2;
    static const int FIELD___stackTrace = 3;
    static const int FIELD__HResult = 4;

    //--//

};

struct Library_corlib_native_System_ArgumentException
{
    static const int FIELD___paramName = 5;

    //--//

};

struct Library_corlib_native_System_Array__SzArrayEnumerator
{
    static const int FIELD___array = 1;
    static const int FIELD___index = 2;
    static const int FIELD___endIndex = 3;
    static const int FIELD___startIndex = 4;
    static const int FIELD___arrayLength = 5;

    //--//

};

struct Library_corlib_native_System_Int64
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Double
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Byte
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Single
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Int16
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_UInt32
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_UInt64
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_UInt16
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Collections_ArrayList
{
    static const int FIELD___items = 1;
    static const int FIELD___size = 2;

    //--//

};

struct Library_corlib_native_System_Globalization_DateTimeFormatInfo
{
    static const int FIELD___generalShortTimePattern = 1;
    static const int FIELD___generalLongTimePattern = 2;
    static const int FIELD___fullDateTimePattern = 3;

    //--//

};

struct Library_corlib_native_System_TimeSpan
{
    static const int FIELD_STATIC__Zero = 3;
    static const int FIELD_STATIC__MaxValue = 4;
    static const int FIELD_STATIC__MinValue = 5;

    static const int FIELD___ticks = 1;

    //--//

};

struct Library_corlib_native_System_DateTime
{
    static const int FIELD_STATIC__MinValue = 6;
    static const int FIELD_STATIC__MaxValue = 7;
    static const int FIELD_STATIC__UnixEpoch = 8;

    static const int FIELD___ticks = 1;

    //--//

};

struct Library_corlib_native_System_Diagnostics_DebuggerBrowsableAttribute
{
    static const int FIELD___state = 1;

    //--//

};

struct Library_corlib_native_System_EventArgs
{
    static const int FIELD_STATIC__Empty = 9;

    //--//

};

struct Library_corlib_native_System_Guid
{
    static const int FIELD_STATIC__Empty = 10;

    static const int FIELD___a = 1;
    static const int FIELD___b = 2;
    static const int FIELD___c = 3;
    static const int FIELD___d = 4;
    static const int FIELD___e = 5;
    static const int FIELD___f = 6;
    static const int FIELD___g = 7;
    static const int FIELD___h = 8;
    static const int FIELD___i = 9;
    static const int FIELD___j = 10;
    static const int FIELD___k = 11;

    //--//

};

struct Library_corlib_native_System_Random
{
    static const int FIELD___random = 1;

    //--//

};

struct Library_corlib_native_System_Runtime_CompilerServices_AccessedThroughPropertyAttribute
{
    static const int FIELD___propertyName = 1;

    //--//

};

struct Library_corlib_native_System_Runtime_Versioning_TargetFrameworkAttribute
{
    static const int FIELD___frameworkName = 1;
    static const int FIELD___frameworkDisplayName = 2;

    //--//

};

struct Library_corlib_native_System_SByte
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_SpanByte
{
    static const int FIELD___array = 1;
    static const int FIELD___start = 2;
    static const int FIELD___length = 3;

    //--//

};

struct Library_corlib_native_System_Threading_Thread
{
    static const int FIELD___delegate = 1;
    static const int FIELD___priority = 2;
    static const int FIELD___thread = 3;
    static const int FIELD___appDomain = 4;
    static const int FIELD___id = 5;

    //--//

};

struct Library_corlib_native_System_Threading_Timeout
{
    static const int FIELD_STATIC__InfiniteTimeSpan = 11;

    //--//

};

struct Library_corlib_native_System_Threading_Timer
{
    static const int FIELD___timer = 1;
    static const int FIELD___state = 2;
    static const int FIELD___callback = 3;

    //--//

};

struct Library_corlib_native_System_Version
{
    static const int FIELD___Major = 1;
    static const int FIELD___Minor = 2;
    static const int FIELD___Build = 3;
    static const int FIELD___Revision = 4;

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;

#endif  //_CORLIB_NATIVE_H_
