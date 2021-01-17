//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace System.Threading.Tasks
{
    /// <summary>
    /// Provides an object that waits for the completion of an asynchronous task.
    /// This API supports the product infrastructure and is not intended to be used directly from your code.
    /// </summary>
    public readonly struct TaskAwaiter : INotifyCompletion
    {
        //readonly bool _isCompleted;
        //readonly Exception _exception;
        //readonly Action _continuation;
        readonly Task _task;

        // TODO check .NET
        public TaskAwaiter(Task task)
        {
            _task = task;
        }

        /// <summary>
        /// Gets a value that indicates whether the asynchronous task has completed.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the task has completed; otherwise, <see langword="false"/>.
        /// </value>
        public bool IsCompleted => _task.IsCompleted;

        /// <summary>
        /// Ends the wait for the completion of the asynchronous task.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        public void GetResult()
        {
            _task.GetResult();
        }

        /// <summary>
        /// Sets the action to perform when the TaskAwaiter object stops waiting for the asynchronous task to complete.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <param name="continuation">The action to perform when the wait operation completes.</param>
        public void OnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        /// <summary>
        /// Schedules the continuation action for the asynchronous task that is associated with this awaiter.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <param name="continuation">The action to invoke when the await operation completes.</param>
        public void UnsafeOnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        //public void CompleteWithException(Exception e)
        //{
        //    _exception = e;
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}

        //public void Complete()
        //{
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}
    }

    /// <summary>
    /// Represents an object that waits for the completion of an asynchronous task and provides a parameter for the result.
    /// This API supports the product infrastructure and is not intended to be used directly from your code.
    /// </summary>
    /// <typeparam name="TResult">The result for the task.</typeparam>
    public readonly struct TaskAwaiter<TResult> : ICriticalNotifyCompletion, INotifyCompletion
    {
        readonly Task<TResult> _task;

        // TODO cehck .NET
        public TaskAwaiter(Task<TResult> task)
        {
            Debug.WriteLine("TaskAwaiter<T>:ctor(task)");
            _task = task;

        }

        /// <summary>
        /// Gets a value that indicates whether the asynchronous task has completed.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the task has completed; otherwise, <see langword="false"/>.
        /// </value>
        public bool IsCompleted => _task.IsCompleted;

        /// <summary>
        /// Ends the wait for the completion of the asynchronous task.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <returns>The result of the completed task.</returns>
        public TResult GetResult()
        {
            return _task.GetResult();
        }

        /// <summary>
        /// Sets the action to perform when the <see cref="TaskAwaiter"/> object stops waiting for the asynchronous task to complete.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <param name="continuation">The action to perform when the wait operation completes.</param>
        public void OnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        /// <summary>
        /// Schedules the continuation action for the asynchronous task associated with this awaiter.
        /// This API supports the product infrastructure and is not intended to be used directly from your code.
        /// </summary>
        /// <param name="continuation">The action to invoke when the await operation completes.</param>
        public void UnsafeOnCompleted(Action continuation)
        {
            _task.OnCompleted(continuation);
        }

        //public void CompleteWithException(Exception e)
        //{
        //    _exception = e;
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}

        //public void Complete(TResult result)
        //{
        //    _result = result;
        //    _isCompleted = true;
        //    _continuation?.Invoke();
        //}
    }
}
