//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
  // After much discussion, we decided the Interlocked class doesn't need
  // any HPA's for synchronization or external threading.  They hurt C#'s
  // codegen for the yield keyword, and arguably they didn't protect much.
  // Instead, they penalized people (and compilers) for writing threadsafe
  // code.
  /// <summary>
  /// Provides atomic operations for variables that are shared by multiple threads.
  /// </summary>
  public static class Interlocked
  {
    /// <summary>
    /// Increments a specified variable and stores the result, as an atomic operation.
    /// </summary>
    /// <param name="location">The variable whose value is to be incremented.</param>
    /// <returns>The incremented value.</returns>
    public static int Increment(ref int location)
    {
      int old_value;
      if (CanFly.Syscall.InterlockedIncrement(ref location, out old_value) < 0)
        throw new ApplicationException();

      return old_value;
    }

    /// <summary>
    /// Decrements a specified variable and stores the result, as an atomic operation.
    /// </summary>
    /// <param name="location">The variable whose value is to be decremented.</param>
    /// <returns>The decremented value.</returns>
    public static int Decrement(ref int location)
    {
      int old_value;
      if (CanFly.Syscall.InterlockedDecrement(ref location, out old_value) < 0)
        throw new ApplicationException();

      return old_value;
    }

    /// <summary>
    /// Sets a 32-bit signed integer to a specified value and returns the original value, as an atomic operation.
    /// </summary>
    /// <param name="location1">The variable to set to the specified value.</param>
    /// <param name="value">The value to which the location1 parameter is set.</param>
    /// <returns>The original value of location1.</returns>
    public static int Exchange(ref int location, int value)
    {
      int old_value;
      if (CanFly.Syscall.InterlockedExchange(ref location, value, out old_value) < 0)
        throw new ApplicationException();

      return old_value;
    }


    /// <summary>
    /// Compares two 32-bit signed integers for equality and, if they are equal, replaces the first value.
    /// </summary>
    /// <param name="location1">The destination, whose value is compared with comparand and possibly replaced.</param>
    /// <param name="value">The value that replaces the destination value if the comparison results in equality.</param>
    /// <param name="comparand">The value that is compared to the value at location1.</param>
    /// <returns>The original value in location1.</returns>
    public static int CompareExchange(ref int location, int value, int comparand)
    {
      if (CanFly.Syscall.InterlockedCompareExchange(ref location, value, comparand) < 0)
        throw new ApplicationException();

      return comparand;
    }
  }
}
