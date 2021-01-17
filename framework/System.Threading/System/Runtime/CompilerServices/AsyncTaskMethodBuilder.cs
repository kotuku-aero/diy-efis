//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Diagnostics;
using System.Threading.Tasks;

namespace System.Runtime.CompilerServices
{
  /// <summary>
  /// Represents a builder for asynchronous methods that return a task.
  /// </summary>
  public struct AsyncTaskMethodBuilder
  {
    /// <summary>
    /// Creates an instance of the <see cref="AsyncTaskMethodBuilder"/> class.
    /// </summary>
    /// <returns>A new instance of the builder.</returns>
    public static AsyncTaskMethodBuilder Create()
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:Create");
      return new AsyncTaskMethodBuilder();
    }

    Task task;
    /// <summary>
    /// Gets the task for this builder
    /// </summary>
    /// <value>
    /// The task for this builder.
    /// </value>
    public Task Task => task;

    /// <summary>
    ///  Schedules the state machine to proceed to the next action when the specified awaiter completes.
    /// </summary>
    /// <typeparam name="TAwaiter"></typeparam>
    /// <typeparam name="TStateMachine"></typeparam>
    /// <param name="awaiter"></param>
    /// <param name="stateMachine"></param>
    public void AwaitOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
        where TAwaiter : INotifyCompletion
        where TStateMachine : IAsyncStateMachine
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:AwaitOnCompleted");
      var _stateMachine = stateMachine;
      awaiter.OnCompleted(() =>
      {
        Debug.WriteLine($"AsyncTaskMethodBuilder:OnCompleted");
        _stateMachine.MoveNext();
      });
    }

    /// <summary>
    /// Schedules the state machine to proceed to the next action when the specified awaiter completes. This method can be called from partially trusted code.
    /// </summary>
    /// <typeparam name="TAwaiter"></typeparam>
    /// <typeparam name="TStateMachine"></typeparam>
    /// <param name="awaiter"></param>
    /// <param name="stateMachine"></param>
    public void AwaitUnsafeOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
        where TAwaiter : ICriticalNotifyCompletion
        where TStateMachine : IAsyncStateMachine
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:AwaitUnsafeOnCompleted");
      var _stateMachine = stateMachine;
      awaiter.OnCompleted(() =>
      {
        Debug.WriteLine($"AsyncTaskMethodBuilder:OnCompleted");
        _stateMachine.MoveNext();
      });
    }

    /// <summary>
    ///  Marks the task as failed and binds the specified exception to the task.
    /// </summary>
    /// <param name="exception"></param>
    public void SetException(Exception exception)
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:SetException");
      task.CompleteWithException(exception);
    }
    /// <summary>
    /// Marks the task as successfully completed.
    /// </summary>
    public void SetResult()
    {
      task.Complete();
      Debug.WriteLine($"AsyncTaskMethodBuilder:SetResult");
    }
    /// <summary>
    /// Associates the builder with the specified state machine.
    /// </summary>
    /// <param name="stateMachine"></param>
    public void SetStateMachine(IAsyncStateMachine stateMachine)
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:SetStateMachine");
    }

    /// <summary>
    /// Begins running the builder with the associated state machine.
    /// </summary>
    /// <typeparam name="TStateMachine"></typeparam>
    /// <param name="stateMachine"></param>
    public void Start<TStateMachine>(ref TStateMachine stateMachine) where TStateMachine : IAsyncStateMachine
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:Start");
      task = new Task();
      stateMachine.MoveNext();
    }
  }

  /// <summary>
  /// Represents a builder for asynchronous methods that return a task.
  /// </summary>
  /// <typeparam name="TResult"></typeparam>
  public struct AsyncTaskMethodBuilder<TResult>
  {
    Task<TResult> task;

    /// <summary>
    /// Gets the task for this builder
    /// </summary>
    /// <value>
    /// The task for this builder.
    /// </value>
    public Task<TResult> Task => task;

    /// <summary>
    /// Creates an instance of the <see cref="AsyncTaskMethodBuilder"/> class.
    /// </summary>
    /// <returns>A new instance of the builder.</returns>
    public static AsyncTaskMethodBuilder<TResult> Create()
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:Create");
      return new AsyncTaskMethodBuilder<TResult>();
    }

    /// <summary>
    /// Schedules the state machine to proceed to the next action when the specified awaiter completes.
    /// </summary>
    /// <typeparam name="TAwaiter">The type of the awaiter.</typeparam>
    /// <typeparam name="TStateMachine">The type of the state machine.</typeparam>
    /// <param name="awaiter">The awaiter.</param>
    /// <param name="stateMachine">The state machine.</param>
    public void AwaitOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
        where TAwaiter : INotifyCompletion
        where TStateMachine : IAsyncStateMachine
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:AwaitOnCompleted");
      var _stateMachine = stateMachine;
      awaiter.OnCompleted(() =>
      {
        Debug.WriteLine($"AsyncTaskMethodBuilder:OnCompleted");
        _stateMachine.MoveNext();
      });
    }

    /// <summary>
    /// Schedules the state machine to proceed to the next action when the specified awaiter completes. This method can be called from partially trusted code.
    /// </summary>
    /// <typeparam name="TAwaiter">The type of the awaiter.</typeparam>
    /// <typeparam name="TStateMachine">The type of the state machine.</typeparam>
    /// <param name="awaiter">The awaiter.</param>
    /// <param name="stateMachine">The state machine.</param>
    public void AwaitUnsafeOnCompleted<TAwaiter, TStateMachine>(ref TAwaiter awaiter, ref TStateMachine stateMachine)
        where TAwaiter : ICriticalNotifyCompletion
        where TStateMachine : IAsyncStateMachine
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:AwaitUnsafeOnCompleted");
      var _stateMachine = stateMachine;
      awaiter.OnCompleted(() =>
      {
        Debug.WriteLine($"AsyncTaskMethodBuilder:OnCompleted");
        _stateMachine.MoveNext();
      });
    }

    /// <summary>
    /// Marks the task as failed and binds the specified exception to the task.
    /// </summary>
    /// <param name="exception">The exception to bind to the task.</param>
    public void SetException(Exception exception)
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:SetException");
      task?.CompleteWithException(exception);
    }

    /// <summary>
    /// Marks the task as successfully completed.
    /// </summary>
    /// <param name="result"></param>
    /// <remarks>
    /// This type and its members are intended for use by the compiler.
    /// </remarks>
    public void SetResult(TResult result)
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:SetResult");
      task?.Complete(result);
    }

    /// <summary>
    /// Associates the builder with the specified state machine.
    /// </summary>
    /// <param name="stateMachine">The state machine instance to associate with the builder</param>
    public void SetStateMachine(IAsyncStateMachine stateMachine)
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:SetStateMachine");

    }

    /// <summary>
    /// Begins running the builder with the associated state machine.
    /// </summary>
    /// <typeparam name="TStateMachine">The type of the state machine.</typeparam>
    /// <param name="stateMachine">The state machine instance, passed by reference.</param>
    public void Start<TStateMachine>(ref TStateMachine stateMachine) where TStateMachine : IAsyncStateMachine
    {
      Debug.WriteLine($"AsyncTaskMethodBuilder:Start");
      task = new Task<TResult>();
      stateMachine.MoveNext();
    }
  }
}
