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

      CanFly.Syscall.InterlockedIncrement(ref location, out old_value);

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

      CanFly.Syscall.InterlockedDecrement(ref location, out old_value);

      return old_value;
    }
  }
}
