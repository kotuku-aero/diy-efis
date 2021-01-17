//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System;
using System.Collections;
using System.Diagnostics;

namespace System.Threading.Tasks
{
    /// <summary>
    /// Represents an asynchronous operation.
    /// </summary>
    public class Task
    {
        TaskAwaiter awaiter;

        ArrayList _continuations = new ArrayList();
        protected AutoResetEvent resultWaitHandle = new AutoResetEvent(false);

        bool isCompleted;
        /// <summary>
        /// Gets a value that indicates whether the task has completed.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the task has completed (that is, the task is in one of the three final states: RanToCompletion, Faulted, or Canceled); otherwise, <see langword="false"/>.
        /// </value>
        public bool IsCompleted
        {
            get => isCompleted;
            set
            {
                isCompleted = true;
                resultWaitHandle.Set();
            }
        }

        /// <summary>
        /// Gets the AggregateException that caused the <see cref="Task"/> to end prematurely. If the <see cref="Task"/> completed successfully or has not yet thrown any exceptions, this will return <see langword="null"/>.
        /// </summary>
        /// <value>
        /// The AggregateException that caused the <see cref="Task"/> to end prematurely.
        /// </value>
        public Exception Exception { get; protected set; }

        /// <summary>
        /// Gets whether the task ran to completion.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the task ran to completion; otherwise <see langword="false"/>.
        /// </value>
        public bool IsCompletedSuccessfully => IsCompleted && Exception == null;

        /// <summary>
        /// Control flags for a task
        /// </summary>
        protected enum Flags
        {
            /// <summary>
            /// Run the task continuations on the same thread that start the task
            /// </summary>
            ContinueOnSameContext = 1
        }
        /// <summary>
        /// Internal control paramters for this task
        /// </summary>
        protected Flags flags = Flags.ContinueOnSameContext;

        // TODO this is not in the .NET API
        public static Task<TResult> FromEvent<TResult>(EventHandler<TResult> callback)
        {
            var tcs = new TaskCompletionSource<TResult>();
            callback += (s, e) => tcs.SetResult(e);
            return tcs.Task;
        }

        /// <summary>
        /// Queues the specified work to run on the thread pool and returns a Task object that represents that work.
        /// </summary>
        /// <param name="action">The work to execute asynchronously</param>
        /// <returns>A task that represents the work queued to execute in the ThreadPool.</returns>
        public static Task Run(Action action)
        {
            return new Task(action);
        }

        /// <summary>
        /// Queues the specified work to run on the thread pool and returns a proxy for the task returned by <paramref name="function"/>.
        /// </summary>
        /// <typeparam name="TResult">The work to execute asynchronously</typeparam>
        /// <param name="function"></param>
        /// <returns></returns>
        // TODO check parameters and return DOES NOT match .NET 
        public static Task<TResult> Run<TResult>(Func<TResult> function)
        {
            return new Task<TResult>(function);
        }

        /// <summary>
        /// Creates a task that will complete after a time delay.
        /// </summary>
        /// <param name="millSeconds">Creates a task that completes after a specified number of milliseconds.</param>
        /// <returns>A task that represents the time delay.</returns>
        public static Task Delay(int millSeconds)
        {
            return new Task(() => Thread.Sleep(millSeconds));
        }

        /// <summary>
        /// Gets a task that has already completed successfully.
        /// </summary>
        /// <value>
        /// The successfully completed task.
        /// </value>
        /// <remarks>
        /// This property returns a task whose Status property is set to RanToCompletion. To create task that returns a value and runs to completion, call the FromResult method.
        /// Repeated attempts to retrieve this property value may not always return the same instance.
        /// </remarks>
        public static Task CompletedTask
        {
            get
            {
                var task = new Task();
                task.Complete();
                return task;
            }
        }

        /// <summary>
        /// Creates a Task that's completed successfully with the specified result.
        /// </summary>
        /// <typeparam name="TResult">The type of the result returned by the task.</typeparam>
        /// <param name="result">The result to store into the completed task.</param>
        /// <returns>The successfully completed task.</returns>
        public static Task<TResult> FromResult<TResult>(TResult result)
        {
            var task = new Task<TResult>();
            task.Complete(result);
            Debug.WriteLine("FromResult: Complete");
            return task;
        }

        /// <summary>
        /// Creates a <see cref="Task"/> that has completed with a specified exception.
        /// </summary>
        /// <param name="e">The exception with which to complete the task.</param>
        /// <returns>The faulted task.</returns>
        public static Task FromException(Exception e)
        {
            var task = new Task();
            task.CompleteWithException(e);
            return task;
        }

        /// <summary>
        /// Creates a <see cref="Task"/> that's completed with a specified exception.
        /// </summary>
        /// <typeparam name="TResult">The type of the result returned by the task.</typeparam>
        /// <param name="e">The exception with which to complete the task.</param>
        /// <returns>The faulted task.</returns>
        public static Task<TResult> FromException<TResult>(Exception e)
        {
            var task = new Task<TResult>();
            task.CompleteWithException(e);
            return task;
        }

        // TODO check missing cancellation token
        public static Task FromCanceled()
        {
            var task = new Task();
            task.CompleteWithException(new TaskCanceledException());
            return task;
        }

        // TODO check missing cancellation token
        public static Task<TResult> FromCanceled<TResult>()
        {
            var task = new Task<TResult>();
            task.CompleteWithException(new TaskCanceledException());
            return task;
        }

        // TODO .NET doesn't have constructor
        public Task()
        {
            Debug.WriteLine("Task:ctor()");
            awaiter = new TaskAwaiter(this);
        }


        /// <summary>
        /// Initializes a new <see cref="Task"/> with the specified action.
        /// </summary>
        /// <param name="action">The delegate that represents the code to execute in the task.</param>
        public Task(Action action):this()
        {
            var syncContext = SynchronizationContext.Current;
            ThreadPool.QueueUserWorkItem((_) =>
            {
                try
                {
                    action();
                    Debug.WriteLine("Calling Complete");
                    if (flags.HasFlag(Flags.ContinueOnSameContext))
                    {
                        syncContext.Post((__) => Complete(), null);
                    }
                    else
                    {
                        Complete();
                    }
                    Debug.WriteLine("Called Complete");
                }
                catch (Exception e)
                {
                    syncContext.Post((ex) => CompleteWithException((Exception)ex), e);
                }
            });
        }

        // TODO check .NET
        protected void RunContinuations()
        {
            Debug.WriteLine("Continuations");
            foreach (var c in _continuations)
            {
                var _continue = (Action)c;
                Debug.WriteLine("Continue");
                _continue();
            }
            _continuations.Clear();
            Debug.WriteLine("Continuations OK");
        }

        /// <summary>
        /// Gets an awaiter used to await this <see cref="Task"/>.
        /// </summary>
        /// <returns>An awaiter instance.</returns>
        public TaskAwaiter GetAwaiter()
        {
            return awaiter;
        }

        /// <summary>
        /// Creates a continuation that executes asynchronously when the target Task completes.
        /// </summary>
        /// <param name="continuation">An action to run when the <see cref="Task"/> completes. When run, the delegate will be passed the completed task as an argument.</param>
        /// <returns>A new continuation <see cref="Task"/>.</returns>
        public Task ContinueWith(Action continuation)
        {
            Task task = new Task();

            task.OnCompleted(continuation);

            OnCompleted(() =>
            {
                task.Complete();
            });

            return task;
        }

        /// <summary>
        /// Make task continuation not neccessarily run on the same context where it is started. Gives better performance
        /// </summary>
        /// <param name="continueOnSameContext"></param>
        /// <returns>Same instance of task</returns>
        public Task ConfigureAwait(bool continueOnSameContext = true)
        {
            if (continueOnSameContext == false)
            {
                flags &= ~Flags.ContinueOnSameContext;
            }
            return this;
        }

        internal void OnCompleted(Action continuation)
        {
            if (IsCompleted)
                continuation();
            else
                _continuations.Add(continuation);
        }

        internal void Complete()
        {
            Debug.WriteLine("Completing");
            IsCompleted = true;
            RunContinuations();
        }

        protected internal void CompleteWithException(Exception e)
        {
            Exception = e;
            IsCompleted = true;
            RunContinuations();
        }

        // TODO check .NET
        /// <summary>
        /// Get the result of the async operation. Block the calling thread until task completes
        /// </summary>
        public void GetResult()
        {
            resultWaitHandle.WaitOne();
            //use it as one shot
            resultWaitHandle.Set(); 
            if (Exception != null)
            {
                throw Exception;
            }
        }
    }

    /// <summary>
    /// Represents an asynchronous operation that can return a value.
    /// </summary>
    /// <typeparam name="TResult">The type of the result produced by this <see cref="Task"/>.</typeparam>
    public class Task<TResult> : Task
    {
        TaskAwaiter<TResult> awaiter;
        TResult result;

        /// <summary>
        /// Gets the result value of this <see cref="Task"/>.
        /// </summary>
        /// <value>
        /// The result value of this <see cref="Task"/>, which is of the same type as the task's type parameter.
        /// </value>
        public TResult Result
        {
            get
            {
                resultWaitHandle.WaitOne();
                resultWaitHandle.Set(); //use it as one shot
                //// TODO check loop
                //while (!IsCompleted);

                if (Exception != null)
                {
                    throw Exception;
                }

                return result;
            }

            private set
            {
                IsCompleted = true;
                result = value;
            }
        }

        // TODO
        public Task()
        {
            Debug.WriteLine("Task<T>:ctor()");
            awaiter = new TaskAwaiter<TResult>(this);
        }

        /// <summary>
        /// Initializes a new <see cref="Task"/> with the specified function.
        /// </summary>
        /// <param name="function">The delegate that represents the code to execute in the task. When the function has completed, the task's <see cref="Result"/> property will be set to return the result value of the function.</param>
        public Task(Func<TResult> function) :this()
        {
            Debug.WriteLine("Task<T>:ctor(action)");
            var syncContext = SynchronizationContext.Current;
            ThreadPool.QueueUserWorkItem((_) =>
            {
                Debug.WriteLine("Task<T>:Start");
                try
                {
                    var result = function();
                    if (flags.HasFlag(Flags.ContinueOnSameContext))
                    {
                        syncContext.Post((r) => Complete((TResult)r), result);
                    }
                    else
                    {
                        Complete(result);
                    }
                }
                catch (Exception e)
                {
                    syncContext.Post((ex) => CompleteWithException((Exception)ex), e);
                }
                Debug.WriteLine("Task<T>:End");
            });
            Debug.WriteLine("Task<T>:ctor(action):ends");
        }

        /// <summary>
        /// Creates a continuation that executes asynchronously when the target Task completes.
        /// </summary>
        /// <typeparam name="TNewResult">The type of the result produced by the continuation.</typeparam>
        /// <param name="continuationFunction">A function to run when the Task completes. When run, the delegate will be passed the completed task as an argument.</param>
        /// <returns>A new continuation Task.</returns>
        public Task<TNewResult> ContinueWith<TNewResult>(Func<TResult, TNewResult> continuationFunction)
        {
            Task<TNewResult> task = new Task<TNewResult>();
            OnCompleted((result) =>
            {
                task.Complete(continuationFunction(result));
            });
            return task;
        }

        /// <summary>
        /// Make task continuation not neccessarily run on the same context where it is started. Gives better performance
        /// </summary>
        /// <param name="continueOnSameContext"></param>
        /// <returns>same instance of task</returns>
        public new Task<TResult> ConfigureAwait(bool continueOnSameContext = true)
        {
            if (continueOnSameContext == false)
            {
                flags &= ~Flags.ContinueOnSameContext;
            }
            return this;
        }

        internal void OnCompleted(Action<TResult> continuation)
        {
            OnCompleted(() => continuation(GetResult()));
        }

        // TODO check .NET
        /// <summary>
        /// Get the result of the async operation. Block the calling thread until task completes
        /// </summary>
        public new TResult GetResult()
        {
            return Result;
        }

        internal void Complete(TResult result)
        {
            Result = result;
            RunContinuations();
            Debug.WriteLine("Complete Done");
        }

        /// <summary>
        /// Gets an awaiter used to await this Task.
        /// </summary>
        /// <returns>An awaiter instance.</returns>
        public new TaskAwaiter<TResult> GetAwaiter()
        {
            return awaiter;
        }
    }
}
