//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
  using System;

  /// <summary>
  /// Represents the method that handles calls from a Timer.
  /// </summary>
  /// <param name="state"></param>
  public delegate void TimerCallback(Object state);

  /// <summary>
  /// Provides a mechanism for executing a method on a thread pool thread at specified intervals. This class cannot be inherited.
  /// </summary>
  public sealed class Timer : MarshalByRefObject, IDisposable
  {
    // these fields are required in the native end
    private uint _semaphore;
    private uint _thread;
    private int _dueTime;
    private int _period;
    private object _state;
    private TimerCallback _callback;

    /// <summary>
    /// Initializes a new instance of the Timer class, using a 32-bit signed integer to specify the time interval.
    /// </summary>
    /// <param name="callback">A TimerCallback delegate representing a method to be executed.</param>
    /// <param name="state">An object containing information to be used by the callback method, or null.</param>
    /// <param name="dueTime">The amount of time to delay before callback is invoked, in milliseconds. 
    /// Specify Timeout.Infinite to prevent the timer from starting. Specify zero (0) to start the timer immediately.</param>
    /// <param name="period">The time interval between invocations of callback, in milliseconds. Specify Timeout.Infinite to disable periodic signaling.</param>
    public Timer(TimerCallback callback, Object state, int dueTime, int period)
    {
      CanFly.Syscall.SemaphoreCreate(out _semaphore);

      _dueTime = dueTime;
      _period = period;

      CanFly.Syscall.CreateThread((byte)ThreadPriority.Normal, null, Run, this, out _thread);
    }

    private void Run()
    {
      while(_period > 0 || _dueTime > 0)
      {
        CanFly.Syscall.SemaphoreWait(_semaphore, (uint)(_dueTime > 0 ? _dueTime : _period));
        _dueTime = 0;

        if(_period > 0)
          _callback(_state);
      }

      // signal so the dispose handler does not wait
      CanFly.Syscall.SemaphoreSignal(_semaphore);
    }

    /// <summary>
    /// Initializes a new instance of the Timer class, using TimeSpan values to measure time intervals.
    /// </summary>
    /// <param name="callback">A delegate representing a method to be executed.</param>
    /// <param name="state">An object containing information to be used by the callback method, or null.</param>
    /// <param name="dueTime">The amount of time to delay before the callback parameter invokes its methods. Specify negative one (-1) milliseconds to prevent the timer from starting. Specify zero (0) to start the timer immediately.</param>
    /// <param name="period">The time interval between invocations of the methods referenced by callback. Specify negative one (-1) milliseconds to disable periodic signaling.</param>
    public Timer(TimerCallback callback, Object state, TimeSpan dueTime, TimeSpan period)
      : this(callback, state, dueTime.Milliseconds, period.Milliseconds)
    {
    }

    /// <summary>
    /// Changes the start time and the interval between method invocations for a timer, using 32-bit signed integers to measure time intervals.
    /// </summary>
    /// <param name="dueTime">The amount of time to delay before the invoking the callback method specified when the Timer was constructed, in milliseconds. 
    /// Specify Timeout.Infinite to prevent the timer from restarting. Specify zero (0) to restart the timer immediately.</param>
    /// <param name="period">The time interval between invocations of the callback method specified when the Timer was constructed, in milliseconds. 
    /// Specify Timeout.Infinite to disable periodic signaling.</param>
    /// <returns>true if the timer was successfully updated; otherwise, false.</returns>
    public bool Change(int dueTime, int period)
    {
      _dueTime = dueTime;
      _period = period;

      // force update
      CanFly.Syscall.SemaphoreSignal(_semaphore);

      return true;
    }

    /// <summary>
    /// Changes the start time and the interval between method invocations for a timer, using TimeSpan values to measure time intervals.
    /// </summary>
    /// <param name="dueTime">A TimeSpan representing the amount of time to delay before invoking the callback method specified when the Timer was constructed. 
    /// Specify negative one (-1) milliseconds to prevent the timer from restarting. Specify zero (0) to restart the timer immediately.</param>
    /// <param name="period">The time interval between invocations of the callback method specified when the Timer was constructed. Specify negative one (-1) milliseconds to disable periodic signaling.</param>
    /// <returns>true if the timer was successfully updated; otherwise, false.</returns>
    public bool Change(TimeSpan dueTime, TimeSpan period) 
    { 
      return Change(dueTime.Milliseconds, period.Milliseconds);
    }

    /// <summary>
    /// Releases all resources used by the current instance of Timer.
    /// </summary>
    public void Dispose() 
    {
      _dueTime = 0;
      _period = 0;
      CanFly.Syscall.SemaphoreSignal(_semaphore);
      // the thread signals on exit
      CanFly.Syscall.SemaphoreWait(_semaphore, uint.MaxValue);

      CanFly.Syscall.SemaphoreClose(_semaphore);
    }
  }
}


