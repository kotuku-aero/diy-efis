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

    NANOCLR_NATIVE_DECLARE(get_CurrentUICultureInternal___STATIC__SystemGlobalizationCultureInfo);
    NANOCLR_NATIVE_DECLARE(set_CurrentUICultureInternal___STATIC__VOID__SystemGlobalizationCultureInfo);

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

struct Library_corlib_native_System_Object
{
    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(GetHashCode___I4);
    NANOCLR_NATIVE_DECLARE(MemberwiseClone___OBJECT);
    NANOCLR_NATIVE_DECLARE(ReferenceEquals___STATIC__BOOLEAN__OBJECT__OBJECT);

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

    NANOCLR_NATIVE_DECLARE(CompareTo___I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(get_Chars___CHAR__I4);
    NANOCLR_NATIVE_DECLARE(ToCharArray___SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(ToCharArray___SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_Length___I4);
    NANOCLR_NATIVE_DECLARE(Split___SZARRAY_STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(Split___SZARRAY_STRING__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(Substring___STRING__I4);
    NANOCLR_NATIVE_DECLARE(Substring___STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(Trim___STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(TrimStart___STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(TrimEnd___STRING__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(CompareTo___I4__STRING);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__CHAR);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(IndexOfAny___I4__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(IndexOfAny___I4__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(IndexOfAny___I4__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__STRING);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__STRING__I4);
    NANOCLR_NATIVE_DECLARE(IndexOf___I4__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__CHAR);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__CHAR__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOfAny___I4__SZARRAY_CHAR);
    NANOCLR_NATIVE_DECLARE(LastIndexOfAny___I4__SZARRAY_CHAR__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOfAny___I4__SZARRAY_CHAR__I4__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__STRING);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__STRING__I4);
    NANOCLR_NATIVE_DECLARE(LastIndexOf___I4__STRING__I4__I4);
    NANOCLR_NATIVE_DECLARE(ToLower___STRING);
    NANOCLR_NATIVE_DECLARE(ToUpper___STRING);
    NANOCLR_NATIVE_DECLARE(Trim___STRING);
    NANOCLR_NATIVE_DECLARE(Equals___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(op_Equality___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(op_Inequality___STATIC__BOOLEAN__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Compare___STATIC__I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__STRING__STRING__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(Concat___STATIC__STRING__SZARRAY_STRING);

    //--//

};

struct Library_corlib_native_System_Exception
{
    static const int FIELD___message = 1;
    static const int FIELD___innerException = 2;
    static const int FIELD___stackTrace = 3;
    static const int FIELD__HResult = 4;

    NANOCLR_NATIVE_DECLARE(get_StackTrace___STRING);

    //--//

};

struct Library_corlib_native_System_ArgumentException
{
    static const int FIELD___paramName = 5;

    //--//

};

struct Library_corlib_native_System_Array
{
    NANOCLR_NATIVE_DECLARE(System_Collections_IList_get_Item___OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(System_Collections_IList_set_Item___VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(get_Length___I4);
    NANOCLR_NATIVE_DECLARE(Copy___STATIC__VOID__SystemArray__I4__SystemArray__I4__I4);
    NANOCLR_NATIVE_DECLARE(Clear___STATIC__VOID__SystemArray__I4__I4);
    NANOCLR_NATIVE_DECLARE(TrySzIndexOf___STATIC__BOOLEAN__SystemArray__I4__I4__OBJECT__BYREF_I4);

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

struct Library_corlib_native_System_BitConverter
{
    NANOCLR_NATIVE_DECLARE(get_IsLittleEndian___STATIC__BOOLEAN);
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

    //--//

};

struct Library_corlib_native_System_Byte
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Collections_ArrayList
{
    static const int FIELD___items = 1;
    static const int FIELD___size = 2;

    NANOCLR_NATIVE_DECLARE(get_Item___OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(set_Item___VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(Add___I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(Clear___VOID);
    NANOCLR_NATIVE_DECLARE(Insert___VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(RemoveAt___VOID__I4);
    NANOCLR_NATIVE_DECLARE(SetCapacity___VOID__I4);

    //--//

};

struct Library_corlib_native_System_Console
{
    NANOCLR_NATIVE_DECLARE(OutNative___STATIC__VOID__STRING__BOOLEAN);

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

    NANOCLR_NATIVE_DECLARE(CompareTo___STATIC__I4__R8__R8);
    NANOCLR_NATIVE_DECLARE(IsInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsNaN___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsNegativeInfinity___STATIC__BOOLEAN__R8);
    NANOCLR_NATIVE_DECLARE(IsPositiveInfinity___STATIC__BOOLEAN__R8);

    //--//

};

struct Library_corlib_native_System_Convert
{
    NANOCLR_NATIVE_DECLARE(NativeToInt64___STATIC__I8__STRING__BOOLEAN__I8__I8__I4);
    NANOCLR_NATIVE_DECLARE(NativeToDouble___STATIC__R8__STRING);
    NANOCLR_NATIVE_DECLARE(ToBase64String___STATIC__STRING__SZARRAY_U1__I4__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(FromBase64CharArray___STATIC__SZARRAY_U1__SZARRAY_CHAR__I4);

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

    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(ToString___STRING);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(CompareTo___I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(Compare___STATIC__I4__SystemTimeSpan__SystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(Equals___STATIC__BOOLEAN__SystemTimeSpan__SystemTimeSpan);

    //--//

};

struct Library_corlib_native_System_DateTime
{
    static const int FIELD_STATIC__MinValue = 6;
    static const int FIELD_STATIC__MaxValue = 7;
    static const int FIELD_STATIC__UnixEpoch = 8;

    static const int FIELD___ticks = 1;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetDateTimePart___I4__SystemDateTimeDateTimePart);
    NANOCLR_NATIVE_DECLARE(DaysInMonth___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(get_UtcNow___STATIC__SystemDateTime);
    NANOCLR_NATIVE_DECLARE(get_Today___STATIC__SystemDateTime);

    //--//

};

struct Library_corlib_native_System_Delegate
{
    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);
    NANOCLR_NATIVE_DECLARE(Combine___STATIC__SystemDelegate__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(Remove___STATIC__SystemDelegate__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(op_Equality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate);
    NANOCLR_NATIVE_DECLARE(op_Inequality___STATIC__BOOLEAN__SystemDelegate__SystemDelegate);

    //--//

};

struct Library_corlib_native_System_Diagnostics_Debug
{
    NANOCLR_NATIVE_DECLARE(WriteLineNative___STATIC__VOID__STRING__BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Diagnostics_Debugger
{
    NANOCLR_NATIVE_DECLARE(get_IsAttached___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Break___STATIC__VOID);

    //--//

};

struct Library_corlib_native_System_Diagnostics_DebuggerBrowsableAttribute
{
    static const int FIELD___state = 1;

    //--//

};

struct Library_corlib_native_System_Enum
{
    NANOCLR_NATIVE_DECLARE(HasFlag___BOOLEAN__SystemEnum);

    //--//

};

struct Library_corlib_native_System_EventArgs
{
    static const int FIELD_STATIC__Empty = 9;

    //--//

};

struct Library_corlib_native_System_GC
{
    NANOCLR_NATIVE_DECLARE(AnyPendingFinalizers___STATIC__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(SuppressFinalize___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(ReRegisterForFinalize___STATIC__VOID__OBJECT);

    //--//

};

struct Library_corlib_native_System_Globalization_DateTimeFormat
{
    NANOCLR_NATIVE_DECLARE(FormatDigits___STATIC__STRING__I4__I4);

    //--//

};

struct Library_corlib_native_System_UInt32
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_Int16
{
    static const int FIELD___value = 1;

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

    NANOCLR_NATIVE_DECLARE(GenerateNewGuid___STATIC__SZARRAY_U1);

    //--//

};

struct Library_corlib_native_System_MathInternal
{
    NANOCLR_NATIVE_DECLARE(Abs___STATIC__I4__I4);
    NANOCLR_NATIVE_DECLARE(Min___STATIC__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Max___STATIC__I4__I4__I4);

    //--//

};

struct Library_corlib_native_System_MulticastDelegate
{
    NANOCLR_NATIVE_DECLARE(op_Equality___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate);
    NANOCLR_NATIVE_DECLARE(op_Inequality___STATIC__BOOLEAN__SystemMulticastDelegate__SystemMulticastDelegate);

    //--//

};

struct Library_corlib_native_System_Number
{
    NANOCLR_NATIVE_DECLARE(FormatNative___STATIC__STRING__OBJECT__CHAR__I4);

    //--//

};

struct Library_corlib_native_System_Random
{
    static const int FIELD___random = 1;

    NANOCLR_NATIVE_DECLARE(Next___I4);
    NANOCLR_NATIVE_DECLARE(Next___I4__I4);
    NANOCLR_NATIVE_DECLARE(NextDouble___R8);
    NANOCLR_NATIVE_DECLARE(NextBytes___VOID__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__I4);

    //--//

};

struct Library_corlib_native_System_Runtime_CompilerServices_AccessedThroughPropertyAttribute
{
    static const int FIELD___propertyName = 1;

    //--//

};

struct Library_corlib_native_System_Runtime_CompilerServices_RuntimeHelpers
{
    NANOCLR_NATIVE_DECLARE(InitializeArray___STATIC__VOID__SystemArray__SystemRuntimeFieldHandle);
    NANOCLR_NATIVE_DECLARE(GetObjectValue___STATIC__OBJECT__OBJECT);
    NANOCLR_NATIVE_DECLARE(RunClassConstructor___STATIC__VOID__SystemRuntimeTypeHandle);
    NANOCLR_NATIVE_DECLARE(get_OffsetToStringData___STATIC__I4);

    //--//

};

struct Library_corlib_native_System_Runtime_Remoting_RemotingServices
{
    NANOCLR_NATIVE_DECLARE(IsTransparentProxy___STATIC__BOOLEAN__OBJECT);

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

struct Library_corlib_native_System_Single
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

struct Library_corlib_native_System_Threading_AutoResetEvent
{
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Reset___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Set___BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Threading_Interlocked
{
    NANOCLR_NATIVE_DECLARE(Increment___STATIC__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(Decrement___STATIC__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(Exchange___STATIC__I4__BYREF_I4__I4);
    NANOCLR_NATIVE_DECLARE(CompareExchange___STATIC__I4__BYREF_I4__I4__I4);

    //--//

};

struct Library_corlib_native_System_Threading_ManualResetEvent
{
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Reset___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Set___BOOLEAN);

    //--//

};

struct Library_corlib_native_System_Threading_Monitor
{
    NANOCLR_NATIVE_DECLARE(Enter___STATIC__VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(Exit___STATIC__VOID__OBJECT);

    //--//

};

struct Library_corlib_native_System_Threading_Thread
{
    static const int FIELD___delegate = 1;
    static const int FIELD___priority = 2;
    static const int FIELD___thread = 3;
    static const int FIELD___appDomain = 4;
    static const int FIELD___id = 5;

    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SystemThreadingThreadStart);
    NANOCLR_NATIVE_DECLARE(Start___VOID);
    NANOCLR_NATIVE_DECLARE(Abort___VOID);
    NANOCLR_NATIVE_DECLARE(Suspend___VOID);
    NANOCLR_NATIVE_DECLARE(Resume___VOID);
    NANOCLR_NATIVE_DECLARE(get_Priority___SystemThreadingThreadPriority);
    NANOCLR_NATIVE_DECLARE(set_Priority___VOID__SystemThreadingThreadPriority);
    NANOCLR_NATIVE_DECLARE(get_ManagedThreadId___I4);
    NANOCLR_NATIVE_DECLARE(get_IsAlive___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Join___VOID);
    NANOCLR_NATIVE_DECLARE(Join___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(Join___BOOLEAN__SystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(get_ThreadState___SystemThreadingThreadState);
    NANOCLR_NATIVE_DECLARE(Sleep___STATIC__VOID__I4);
    NANOCLR_NATIVE_DECLARE(get_CurrentThread___STATIC__SystemThreadingThread);

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

    NANOCLR_NATIVE_DECLARE(Dispose___VOID);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SystemThreadingTimerCallback__OBJECT__I4__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__SystemThreadingTimerCallback__OBJECT__SystemTimeSpan__SystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(Change___BOOLEAN__I4__I4);
    NANOCLR_NATIVE_DECLARE(Change___BOOLEAN__SystemTimeSpan__SystemTimeSpan);

    //--//

};

struct Library_corlib_native_System_Threading_WaitHandle
{
    NANOCLR_NATIVE_DECLARE(WaitOne___BOOLEAN__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(WaitMultiple___STATIC__I4__SZARRAY_SystemThreadingWaitHandle__I4__BOOLEAN__BOOLEAN);

    //--//

};

struct Library_corlib_native_System_UInt16
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_UInt64
{
    static const int FIELD___value = 1;

    //--//

};

struct Library_corlib_native_System_ValueType
{
    NANOCLR_NATIVE_DECLARE(Equals___BOOLEAN__OBJECT);

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

struct Library_corlib_native_System_WeakReference
{
    NANOCLR_NATIVE_DECLARE(get_IsAlive___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(get_Target___OBJECT);
    NANOCLR_NATIVE_DECLARE(set_Target___VOID__OBJECT);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__OBJECT);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;

#endif  //_CORLIB_NATIVE_H_
