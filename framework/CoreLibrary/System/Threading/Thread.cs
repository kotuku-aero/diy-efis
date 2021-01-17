//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
  using System;

  /// <summary>
  /// Creates and controls a thread, sets its priority, and gets its status.
  /// </summary>
  public sealed class Thread
  {
    // these fields are required in the native end
#pragma warning disable 0169
    private Delegate _delegate;
    private int _priority;
    [Reflection.FieldNoReflection]
    private object _thread;
    [Reflection.FieldNoReflection]
    private object _appDomain;
    private int _id;
#pragma warning restore 0169

    /// <summary>
    /// Initializes a new instance of the Thread class.
    /// </summary>
    /// <param name="start">A ThreadStart delegate that represents the methods to be invoked when this thread begins executing.</param>
    public Thread(ThreadStart start)
    {
      CanFly.Runtime.ThreadCtor(this, start);
    }

    /// <summary>
    /// Causes the operating system to change the state of the current instance to ThreadState.Running.
    /// </summary>
    public void Start() { CanFly.Runtime.ThreadStart(this); }

    /// <summary>
    /// Raises a ThreadAbortException in the thread on which it is invoked, to begin the process of terminating the thread. Calling this method usually terminates the thread.
    /// </summary>
    public void Abort() { CanFly.Runtime.ThreadAbort(this); }

    /// <summary>
    /// Either suspends the thread, or if the thread is already suspended, has no effect.
    /// </summary>
    public void Suspend() { CanFly.Runtime.ThreadSuspend(this); }

    /// <summary>
    /// Obsolete : Resumes a thread that has been suspended.
    /// </summary>
    public void Resume() { CanFly.Runtime.ThreadResume(this); }

    /// <summary>
    /// Gets or sets a value indicating the scheduling priority of a thread.
    /// </summary>
    /// <value>One of the ThreadPriority values. The default value is ThreadPriority.Normal.</value>
    public ThreadPriority Priority
    {
      get { return CanFly.Runtime.GetThreadPriority(this); }
      set { CanFly.Runtime.SetThreadPriority(this, value); }
    }

    /// <summary>
    /// Gets a unique identifier for the current managed thread.
    /// </summary>
    /// <value>An integer that represents a unique identifier for this managed thread.</value>
    public int ManagedThreadId
    {
      get { return CanFly.Runtime.GetManagedThreadId(this); }
    }

    /// <summary>
    /// Gets a value indicating the execution status of the current thread.
    /// </summary>
    /// <value>true if this thread has been started and has not terminated normally or aborted; otherwise, false.</value>
    public bool IsAlive
    {
      get { return CanFly.Runtime.ThreadIsAlive(this); }
    }

    /// <summary>
    /// Blocks the calling thread until the thread represented by this instance terminates, while continuing to perform standard COM and SendMessage pumping.
    /// </summary>
    public void Join() { CanFly.Runtime.ThreadJoin(this); }

    /// <summary>
    /// Blocks the calling thread until the thread represented by this instance terminates or the specified time elapses, while continuing to perform standard COM and SendMessage pumping.
    /// </summary>
    /// <param name="millisecondsTimeout">The number of milliseconds to wait for the thread to terminate.</param>
    /// <returns>true if the thread has terminated; false if the thread has not terminated after the amount of time specified by the millisecondsTimeout parameter has elapsed.</returns>
    public bool Join(int millisecondsTimeout) { return CanFly.Runtime.ThreadJoin(this, millisecondsTimeout); }

    /// <summary>
    /// Blocks the calling thread until the thread represented by this instance terminates or the specified time elapses, while continuing to perform standard COM and SendMessage pumping.
    /// </summary>
    /// <param name="timeout">A TimeSpan set to the amount of time to wait for the thread to terminate.</param>
    /// <returns>true if the thread terminated; false if the thread has not terminated after the amount of time specified by the timeout parameter has elapsed.</returns>
    public bool Join(TimeSpan timeout) { return CanFly.Runtime.ThreadJoin(this, timeout.Milliseconds); }

    /// <summary>
    /// Suspends the current thread for the specified number of milliseconds.
    /// </summary>
    /// <param name="millisecondsTimeout">The number of milliseconds for which the thread is suspended. 
    /// If the value of the <paramref name="millisecondsTimeout"/> argument is zero, the thread relinquishes the remainder 
    /// of its time slice to any thread of equal priority that is ready to run. If there are no other threads 
    /// of equal priority that are ready to run, execution of the current thread is not suspended.</param>
    /// <remarks>
    /// The thread will not be scheduled for execution by the operating system for the amount of time specified. 
    /// You can specify Timeout.Infinite for the <paramref name="millisecondsTimeout"/> parameter to suspend the thread indefinitely. However, we recommend that you use other <see cref="System.Threading"/> classes such as <see cref="AutoResetEvent"/>, <see cref="ManualResetEvent"/>, <see cref="Monitor"/> or <see cref="WaitHandle"/> instead to synchronize threads or manage resources.
    /// The system clock ticks at a specific rate called the clock resolution. The actual timeout might not be exactly the specified timeout, because the specified timeout will be adjusted to coincide with clock ticks. 
    /// </remarks>
    public static void Sleep(int millisecondsTimeout) { CanFly.Runtime.ThreadSleep(millisecondsTimeout); }

    /// <summary>
    /// Suspends the current thread for the specified amount of time.
    /// </summary>
    /// <param name="timeout">The amount of time for which the thread is suspended. 
    /// If the value of the <paramref name="timeout"/> argument is Zero, the thread relinquishes the remainder 
    /// of its time slice to any thread of equal priority that is ready to run. If there are no other threads 
    /// of equal priority that are ready to run, execution of the current thread is not suspended.</param>
    /// <remarks>
    /// The thread will not be scheduled for execution by the operating system for the amount of time specified. 
    /// You can specify <see cref="Timeout.Infinite"/> for the <paramref name="timeout"/> parameter to suspend the thread indefinitely. However, we recommend that you use other <see cref="System.Threading"/> classes such as <see cref="AutoResetEvent"/>, <see cref="ManualResetEvent"/>, <see cref="Monitor"/> or <see cref="WaitHandle"/> instead to synchronize threads or manage resources.
    /// The system clock ticks at a specific rate called the clock resolution. The actual timeout might not be exactly the specified timeout, because the specified timeout will be adjusted to coincide with clock ticks. 
    /// </remarks>
    public static void Sleep(TimeSpan timeout)
    {
      long tm = timeout.Ticks / TimeSpan.TicksPerMillisecond;
      if (tm < -1 || tm > Int32.MaxValue)
      {
#pragma warning disable S3928 // Parameter names used into ArgumentException constructors should match an existing one 
        throw new ArgumentOutOfRangeException();
#pragma warning restore S3928 // Parameter names used into ArgumentException constructors should match an existing one 
      }
      Sleep((int)tm);
    }

    /// <summary>
    /// Gets the currently running thread.
    /// </summary>
    /// <value>A Thread that is the representation of the currently running thread.</value>
    public static Thread CurrentThread
    {
      get { return CanFly.Runtime.GetCurrentThread(); }
    }

    /// <summary>
    /// Gets a value containing the states of the current thread.
    /// </summary>
    /// <value>One of the ThreadState values indicating the state of the current thread. The initial value is Unstarted.</value>
    public ThreadState ThreadState
    {
      get { return CanFly.Runtime.GetThreadState(this); }
    }
  }
}
