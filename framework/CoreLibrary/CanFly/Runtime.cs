using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  /// <summary>
  /// Class holding all system calls.  The order of this class never changes.
  /// Methods can be added to the end of it, but never removed.
  /// </summary>
  internal class Runtime
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
    internal static extern byte[] GetBytes(uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(ulong value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetBytes(ushort value);
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
    internal static extern string ToBase64String(byte[] inArray, int offset, int length, bool insertLineBreaks);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] FromBase64String(string inString);
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
    internal static extern Object DelegateTarget(Delegate dlg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetCapacity(Object[] list, int capacity);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern object ArrayListGet(Object[] list, int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListSet(Object[] list, int index, object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int ArrayListAdd(Object[] list, Object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListClear(Object[] list);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListInsert(Object[] list, int index, Object value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ArrayListRemoveAt(Object[] list, int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool DebuggerIsAttached();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DebuggerBreak();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DebuggerPrint(string message, bool newLine);

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
    internal static extern double Abs(double val);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Min(double val1, double val2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Max(double val1, double val2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Acos(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Asin(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Atan(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Atan2(double x, double y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Ceiling(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Cos(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Cosh(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Floor(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double IEEERemainder(double x, double y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Exp(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Log(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Log10(double value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Pow(double x, double y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Round(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Sign(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Sin(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Sinh(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Sqrt(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Tan(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Tanh(double d);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double Truncate(double d);
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
    internal static extern char StringCharAt(String obj, int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char[] StringToCharArray(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char[] StringToCharArray(String obj, int startIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringLength(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String[] StringSplit(String obj, char[] separator, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringSubstring(String obj, int startIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringTrim(String obj, char[] trimChars, bool trimStart, bool trimEnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StringCtor(String obj, char[] value, int startIndex, int length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StringCtor(String obj, char[] value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StringCtor(String obj, char c, int count);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringCompare(String strA, String strB);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, char value, int startIndex, int count, bool reverse);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOfAny(String obj, char[] anyOf, int startIndex, int count, bool reverse);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StringIndexOf(String obj, String value, int startIndex, int count, bool reverse);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringToLower(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringToUpper(String obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern String StringConcat(String left, String right);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ValueTypeEquals(ValueType obj, Object obj1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool WeakReferenceIsAlive(WeakReference obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Object GetWeakReferenceTarget(WeakReference obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWeakReferenceTarget(WeakReference obj, Object target);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool ToBoolean(Boolean obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetBoolean(bool value, ref Boolean obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern sbyte ToInt8(SByte obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetInt8(sbyte value, ref SByte obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte ToUInt8(Byte obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetUInt8(byte value, ref Byte obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short ToInt16(Int16 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetInt16(short value, ref Int16 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort ToUInt16(UInt16 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetUInt16(ushort value, ref UInt16 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int ToInt32(Int32 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetInt32(int value, ref Int32 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ToUInt32(UInt32 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetUInt32(uint value, ref UInt32 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern long ToInt64(Int64 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetInt64(long value, ref Int64 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ulong ToUInt64(UInt64 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetUInt64(ulong value, ref UInt64 obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float ToFloat(Single obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetFloat(float value, ref Single obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern double ToDouble(Double obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetDouble(double value, ref Double obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern char ToChar(Char obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetChar(char value, ref Char obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ulong ToDateTime(DateTime obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetDateTime(ulong value, ref DateTime obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern long ToTimeSpan(TimeSpan obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetTimeSpan(long value, ref TimeSpan obj);
  }
}
