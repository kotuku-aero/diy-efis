//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Converts base data types to an array of bytes, and an array of bytes to base data types.
  /// </summary>
  public static class BitConverter
  {
    /// <summary>
    /// Indicates the byte order ("endianess") in which data is stored in this computer architecture.
    /// </summary>
    public static bool IsLittleEndian
    {
      get { return CanFly.Runtime.IsLittleEndian(); }
    }

    /// <summary>
    /// Converts the specified double-precision floating point number to a 64-bit signed integer.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns></returns>
    public static long DoubleToInt64Bits(double value) { return CanFly.Runtime.DoubleToInt64Bits(value); }

    /// <summary>
    /// Returns the specified Boolean value as an array of bytes.
    /// </summary>
    /// <param name="value">A Boolean value.</param>
    /// <returns>An array of bytes with length 1.</returns>
    public static byte[] GetBytes(bool value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified Unicode character value as an array of bytes.
    /// </summary>
    /// <param name="value">A character to convert.</param>
    /// <returns>An array of bytes with length 2.</returns>
    public static byte[] GetBytes(char value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified double-precision floating point value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 8.</returns>
    public static byte[] GetBytes(double value) { return CanFly.Runtime.GetBytes(value); }

  /// <summary>
  /// Returns the specified single-precision floating point value as an array of bytes.
  /// </summary>
  /// <param name="value">The number to convert.</param>
  /// <returns>An array of bytes with length 4.</returns>
    public static byte[] GetBytes(float value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified 32-bit signed integer value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 4.</returns>
    public static byte[] GetBytes(int value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified 64-bit signed integer value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 8.</returns>
    public static byte[] GetBytes(long value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified 16-bit signed integer value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 2.</returns>
    public static byte[] GetBytes(short value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified 32-bit unsigned integer value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 4.</returns>
    public static byte[] GetBytes(uint value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified 64-bit unsigned integer value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 8.</returns>
    public static byte[] GetBytes(ulong value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Returns the specified 16-bit unsigned integer value as an array of bytes.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>An array of bytes with length 2.</returns>
    public static byte[] GetBytes(ushort value) { return CanFly.Runtime.GetBytes(value); }

    /// <summary>
    /// Converts the specified 64-bit signed integer to a double-precision floating point number.
    /// </summary>
    /// <param name="value">The number to convert.</param>
    /// <returns>A double-precision floating point number whose value is equivalent to value.</returns>
    public static double Int64BitsToDouble(long value) { return CanFly.Runtime.Int64BitsToDouble(value); }

    /// <summary>
    /// Returns a Boolean value converted from one byte at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>true if the byte at startIndex in value is nonzero; otherwise, false.</returns>
    public static bool ToBoolean(byte[] value, int startIndex) { return CanFly.Runtime.ToBoolean(value, startIndex); }

    /// <summary>
    /// Returns a Unicode character converted from two bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A character formed by two bytes beginning at startIndex.</returns>
    public static char ToChar(byte[] value, int startIndex) { return CanFly.Runtime.ToChar(value, startIndex); }

    /// <summary>
    /// Returns a double-precision floating point number converted from eight bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A double precision floating point number formed by eight bytes beginning at startIndex.</returns>
    public static double ToDouble(byte[] value, int startIndex) { return CanFly.Runtime.ToDouble(value, startIndex); }

    /// <summary>
    /// Returns a 16-bit signed integer converted from two bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A 16-bit signed integer formed by two bytes beginning at startIndex.</returns>
    public static short ToInt16(byte[] value, int startIndex) { return CanFly.Runtime.ToInt16(value, startIndex); }

    /// <summary>
    /// Returns a 32-bit signed integer converted from four bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A 32-bit signed integer formed by four bytes beginning at startIndex.</returns>
    public static int ToInt32(byte[] value, int startIndex) { return CanFly.Runtime.ToInt32(value, startIndex); }

    /// <summary>
    /// Returns a 64-bit signed integer converted from eight bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A 64-bit signed integer formed by eight bytes beginning at startIndex.</returns>
    public static long ToInt64(byte[] value, int startIndex) { return CanFly.Runtime.ToInt64(value, startIndex); }

    /// <summary>
    /// Returns a single-precision floating point number converted from four bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A single-precision floating point number formed by four bytes beginning at startIndex.</returns>
    public static float ToSingle(byte[] value, int startIndex) { return CanFly.Runtime.ToSingle(value, startIndex); }

    /// <summary>
    /// Converts the numeric value of each element of a specified array of bytes to its equivalent hexadecimal string representation.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <returns>A String of hexadecimal pairs separated by hyphens, where each pair represents the corresponding element in value; for example, "7F-2C-4A".</returns>
    public static string ToString(byte[] value) { return CanFly.Runtime.ToString(value); }

    /// <summary>
    /// Converts the numeric value of each element of a specified subarray of bytes to its equivalent hexadecimal string representation.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A String of hexadecimal pairs separated by hyphens, where each pair represents the corresponding element in a subarray of value; for example, "7F-2C-4A".</returns>
    public static string ToString(byte[] value, int startIndex) { return CanFly.Runtime.ToString(value, startIndex); }

    /// <summary>
    /// Converts the numeric value of each element of a specified subarray of bytes to its equivalent hexadecimal string representation.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <param name="length">The number of array elements in value to convert.</param>
    /// <returns>A String of hexadecimal pairs separated by hyphens, where each pair represents the corresponding element in a subarray of value; for example, "7F-2C-4A".</returns>
    public static string ToString(byte[] value, int startIndex, int length) { return CanFly.Runtime.ToString(value, startIndex, length); }

    /// <summary>
    /// Returns a 16-bit unsigned integer converted from two bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">An array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A 16-bit unsigned integer formed by two bytes beginning at startIndex.</returns>
    public static ushort ToUInt16(byte[] value, int startIndex) { return CanFly.Runtime.ToUInt16(value, startIndex); }

    /// <summary>
    /// Returns a 32-bit unsigned integer converted from two bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">The array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A 32-bit unsigned integer formed by two bytes beginning at startIndex.</returns>
    public static uint ToUInt32(byte[] value, int startIndex) { return CanFly.Runtime.ToUInt32(value, startIndex); }

    /// <summary>
    /// Returns a 64-bit unsigned integer converted from two bytes at a specified position in a byte array.
    /// </summary>
    /// <param name="value">The array of bytes.</param>
    /// <param name="startIndex">The starting position within value.</param>
    /// <returns>A 64-bit unsigned integer formed by two bytes beginning at startIndex.</returns>
    public static ulong ToUInt64(byte[] value, int startIndex) { return CanFly.Runtime.ToUInt64(value, startIndex); }
  }
}
