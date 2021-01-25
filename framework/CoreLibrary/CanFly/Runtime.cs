using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  /// <summary>
  /// Class holding all system calls.  The order of this class never changes.
  /// Methods can be added to the end of it, but never removed.
  /// </summary>
  internal static class Runtime
  {
    /**************************************************************************************/
    // mscorlib routines
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Copy(Array sourceArray, int sourceIndex, Array destinationArray, int destinationIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Clear(Array array, int index, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetArrayLength(Array array);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern object GetArrayAt(Array array, int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetArrayAt(Array array, int index, object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool TrySzIndexOf(Array sourceArray, int sourceIndex, int count, Object value, out int retVal);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsLittleEndian();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern long DoubleToInt64Bits(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(char value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(long value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
#pragma warning disable CS3001 // Argument type 'uint' is not CLS-compliant
    internal static extern byte[] GetBytes(uint value);
#pragma warning restore CS3001 // Argument type 'uint' is not CLS-compliant
    [MethodImpl(MethodImplOptions.InternalCall)]
#pragma warning disable CS3001 // Argument type 'ulong' is not CLS-compliant
    internal static extern byte[] GetBytes(ulong value);
#pragma warning restore CS3001 // Argument type 'ulong' is not CLS-compliant
    [MethodImpl(MethodImplOptions.InternalCall)]
#pragma warning disable CS3001 // Argument type 'ushort' is not CLS-compliant
    internal static extern byte[] GetBytes(ushort value);
#pragma warning restore CS3001 // Argument type 'ushort' is not CLS-compliant
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Int64BitsToDouble(long value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ToBoolean(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char ToChar(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double ToDouble(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short ToInt16(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int ToInt32(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern long ToInt64(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float ToSingle(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string ToString(byte[] value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string ToString(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string ToString(byte[] value, int startIndex, int length);
    [CLSCompliant(false)]
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort ToUInt16(byte[] value, int startIndex);
    [CLSCompliant(false)]
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ToUInt32(byte[] value, int startIndex);
    [CLSCompliant(false)]
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ulong ToUInt64(byte[] value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern long NativeToInt64(string value, bool signed, long min, long max, int fromBase);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double NativeToDouble(string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CompareTo(double d, double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsInfinity(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsNaN(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsNegativeInfinity(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsPositiveInfinity(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool HasFlag(Enum value, Enum flag);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String GetStackTrace(Exception ex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool AnyPendingFinalizers();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SuppressFinalize(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ReRegisterForFinalize(Object obj);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern System.Globalization.CultureInfo GetCurrentUICultureInternal();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetCurrentUICultureInternal(System.Globalization.CultureInfo value);


    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string ToBase64String(byte[] inArray, int offset, int length, bool insertLineBreaks);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] FromBase64CharArray(char[] inArray, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ulong ToTicks(int year, int month, int day, int hour, int minute, int second, int millisecond);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int DaysInMonth(int year, int month);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ulong UtcNow();
    /*
    private enum DateTimePart
    {
      Year,
      Month,
      Day,
      DayOfWeek,
      DayOfYear,
      Hour,
      Minute,
      Second,
      Millisecond
    }     */
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetDateTimePart(ulong ticks, int part);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool DelegateEquals(Delegate del, Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Delegate DelegateCombine(Delegate a, Delegate b);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Delegate DelegateRemove(Delegate source, Delegate value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetCapacity(System.Collections.ArrayList list, int capacity);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern object ArrayListGet(System.Collections.ArrayList list, int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListSet(System.Collections.ArrayList list, int index, object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int ArrayListAdd(System.Collections.ArrayList list, Object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListClear(System.Collections.ArrayList list);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListInsert(System.Collections.ArrayList list, int index, Object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListRemoveAt(System.Collections.ArrayList list, int index);
    // Debugging methods
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal extern static void OutNative(string text, bool addLineFeed);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool DebuggerIsAttached();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DebuggerBreak();

    ////////////////////////////////////////////////////////////////////////////
    //
    // Format the positive integer value to a string and perfix with assigned
    // length of leading zero.
    //
    // Parameters:
    //  value: The value to format
    //  len: The maximum length for leading zero.
    //  If the digits of the value is greater than len, no leading zero is added.
    //  (If len > 2, we'll treat it as 2)
    //
    // Notes:
    //  The function can format to Int32.MaxValue.
    //
    ////////////////////////////////////////////////////////////////////////////
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String FormatDigits(int value, int len);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GenerateNewGuid();

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Abs(int val);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Min(int val1, int val2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Max(int val1, int val2);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool Equals(MulticastDelegate d1, MulticastDelegate d2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool NotEquals(MulticastDelegate d1, MulticastDelegate d2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String FormatNative(Object value, char format, int precision);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ObjectEquals(Object obj, Object other);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ObjectReferenceEquals(Object objA, Object objB);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int ObjectGetHashCode(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Object ObjectMemberwiseClone(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RandomCtor(Random obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RandomCtor(Random obj, int seed);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RandomNext(Random obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RandomNext(Random obj, int maxValue);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double RandomNextDouble(Random obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RandomNextBytes(Random obj, byte[] buffer);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InitializeArray(Array array, RuntimeFieldHandle fldHandle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Object GetObjectValue(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RunClassConstructor(RuntimeTypeHandle type);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int OffsetToStringData();

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsTransparentProxy(Object proxy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool Equals(String a, String b);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char StringCharAt(String obj, int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char[] StringToCharArray(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char[] StringToCharArray(String obj, int startIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLength(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String[] StringSplit(String obj, char[] separator);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String[] StringSplit(String obj, char[] separator, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringSubstring(String obj, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringSubstring(String obj, int startIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringTrim(String obj, char[] trimChars);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringTrimStart(String obj, char[] trimChars);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringTrimEnd(String obj, char[] trimChars);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StringCtor(String obj, char[] value, int startIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StringCtor(String obj, char[] value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StringCtor(String obj, char c, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringCompare(String strA, String strB);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringCompareTo(String obj, Object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringCompareTo(String obj, String strB);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, char value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, char value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, char value, int startIndex, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOfAny(String obj, char[] anyOf);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOfAny(String obj, char[] anyOf, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOfAny(String obj, char[] anyOf, int startIndex, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, String value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, String value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, String value, int startIndex, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOf(String obj, char value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOf(String obj, char value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOf(String obj, char value, int startIndex, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOfAny(char[] anyOf);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOfAny(String obj, char[] anyOf);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOfAny(String obj, char[] anyOf, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOfAny(String obj, char[] anyOf, int startIndex, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOf(String obj, String value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOf(String obj, String value, int startIndex);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLastIndexOf(String obj, String value, int startIndex, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringToLower(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringToUpper(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringTrim(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringConcat(String str0, String str1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringConcat(String str0, String str1, String str2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringConcat(String str0, String str1, String str2, String str3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringConcat(String[] values);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void AutoResetEventCtor(System.Threading.AutoResetEvent obj, bool initialState);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool AutoResetEventReset(System.Threading.AutoResetEvent obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool AutoResetEventSet(System.Threading.AutoResetEvent obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Increment(ref int location);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Decrement(ref int location);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Exchange(ref int location1, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CompareExchange(ref int location1, int value, int comparand);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ManualResetEventCtor(System.Threading.ManualResetEvent obj, bool initialState);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ManualResetEventReset(System.Threading.ManualResetEvent obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ManualResetEventSet(System.Threading.ManualResetEvent obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void MonitorEnter(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void MonitorExit(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadCtor(byte priority, string name, System.Threading.ThreadStart start, object arg, out uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadGetArg(uint id, out object arg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadGetCurrentId(out uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadStart(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadAbort(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadSuspend(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadResume(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetThreadPriority(uint id, out byte priority);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint SetThreadPriority(uint id, byte priority);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadIsAlive(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadSleep(int millisecondsTimeout);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetThreadState(uint id, out uint state);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadWaitTillFinished(uint id, int waitWilliseconds);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void TimerCtor(System.Threading.Timer obj, System.Threading.TimerCallback callback, Object state, int dueTime, int period);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool TimerChange(System.Threading.Timer obj, int dueTime, int period);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void TimerDispose(System.Threading.Timer obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool WaitHandleWaitOne(System.Threading.WaitHandle obj, int millisecondsTimeout, bool exitContext);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int WaitMultiple(System.Threading.WaitHandle[] waitHandles, int millisecondsTimeout, bool exitContext, bool waitAll);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ValueTypeEquals(ValueType obj, Object obj1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void WeakReferenceCtor(WeakReference obj, Object target);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool WeakReferenceIsAlive(WeakReference obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Object GetWeakReferenceTarget(WeakReference obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWeakReferenceTarget(WeakReference obj, Object target);
  }
}
