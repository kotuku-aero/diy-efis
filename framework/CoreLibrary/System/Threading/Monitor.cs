//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
  using System;
  using Runtime.CompilerServices;

  /// <summary>
  /// Provides a mechanism that synchronizes access to objects.
  /// </summary>
  public class Monitor
  {
    /// <summary>
    /// Acquires an exclusive lock on the specified object.
    /// </summary>
    /// <param name="obj">The object on which to acquire the monitor lock.</param>
    public static void Enter(Object obj) { CanFly.Syscall.Enter(obj); }

    /// <summary>
    /// Releases an exclusive lock on the specified object.
    /// </summary>
    /// <param name="obj">The object on which to release the lock.</param>
    public static void Exit(Object obj) { CanFly.Syscall.Exit(obj); }
  }
}
