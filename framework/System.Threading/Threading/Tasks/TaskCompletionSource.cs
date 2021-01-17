//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading.Tasks
{
    /// <summary>
    /// Represents the producer side of a Task unbound to a delegate, providing access to the consumer side through the Task property.
    /// </summary>
    /// <typeparam name="TResult"></typeparam>
    public class TaskCompletionSource<TResult>
    {

        /// <summary>
        /// Gets the <see cref="Task"/> created by this <see cref="TaskCompletionSource"/>.
        /// </summary>
        /// <value>
        /// Returns the Task created by this TaskCompletionSource.
        /// </value>
        public Task<TResult> Task { get; }

        /// <summary>
        /// Creates a <see cref="TaskCompletionSource"/>.
        /// </summary>
        public TaskCompletionSource()
        {
            Task = new Task<TResult>();
        }

        /// <summary>
        /// Transitions the underlying Task into the RanToCompletion state.
        /// </summary>
        /// <param name="result"></param>
        public void SetResult(TResult result)
        {
            Task.Complete(result);
        }

        /// <summary>
        /// Transitions the underlying Task into the Canceled state.
        /// </summary>
        public void SetCanceled()
        {
            Task.CompleteWithException(new TaskCanceledException());
        }

        /// <summary>
        /// Transitions the underlying Task into the Faulted state and binds it to a specified exception.
        /// </summary>
        /// <param name="exception">The exception to bind to this Task.</param>
        public void SetException(Exception exception)
        {
            Task.CompleteWithException(exception);
        }

    }
}
