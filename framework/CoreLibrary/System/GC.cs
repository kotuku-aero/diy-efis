//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Controls the system garbage collector, a service that automatically reclaims unused memory.
  /// </summary>
  public static class GC
  {
    /// <summary>
    /// Suspends the current thread until the thread that is processing the queue of finalizers has emptied that queue.
    /// </summary>
    public static void WaitForPendingFinalizers()
    {
      while (CanFly.Runtime.AnyPendingFinalizers()) Threading.Thread.Sleep(10);
    }

    /// <summary>
    /// Requests that the system not call the finalizer for the specified object.
    /// </summary>
    /// <param name="obj">The object that a finalizer must not be called for. </param>
    public static void SuppressFinalize(Object obj) { CanFly.Runtime.SuppressFinalize(obj); }

    /// <summary>
    /// Requests that the system call the finalizer for the specified object for which SuppressFinalize has previously been called.
    /// </summary>
    /// <param name="obj">The object that a finalizer must be called for. </param>
    public static void ReRegisterForFinalize(Object obj) { CanFly.Runtime.ReRegisterForFinalize(obj); }

  }
}
