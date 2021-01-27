//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
  /// <summary>
  /// Notifies a waiting thread that an event has occurred. This class cannot be inherited.
  /// </summary>
  public sealed class AutoResetEvent : WaitHandle
  {
    /// <summary>
    /// Initializes a new instance of the AutoResetEvent class with a Boolean value indicating whether to set the initial state to signaled.
    /// </summary>
    /// <param name="initialState">true to set the initial state to signaled; false to set the initial state to non-signaled.</param>
    public AutoResetEvent(bool initialState)
    {
      if (initialState)
        CanFly.Syscall.SemaphoreSignal(handle);
    }

    /// <summary>
    /// Sets the state of the event to nonsignaled, causing threads to block.
    /// </summary>
    /// <returns>true if the operation succeeds; otherwise, false.</returns>
    public bool Reset()
    {
      // remove the counter
      return CanFly.Syscall.SemaphoreWait(handle, 0) == 0;
    }

    /// <summary>
    /// Sets the state of the event to signaled, allowing one or more waiting threads to proceed.
    /// </summary>
    /// <returns>true if the operation succeeds; otherwise, false.</returns>
    public bool Set()
    { 
      return CanFly.Syscall.SemaphoreSignal(handle) == 0;
    }

    public override bool WaitOne(int millisecondsTimeout, bool exitContext)
    {
      if (base.WaitOne(millisecondsTimeout, exitContext))
        return Set();

      return false;
    }
  }
}
