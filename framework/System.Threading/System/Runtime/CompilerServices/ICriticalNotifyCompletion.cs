//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Runtime.CompilerServices
{
    /// <summary>
    /// Represents an awaiter that schedules continuations when an await operation completes.
    /// </summary>
    public interface ICriticalNotifyCompletion : INotifyCompletion
    {
        /// <summary>
        /// Schedules the continuation action that's invoked when the instance completes.
        /// </summary>
        /// <param name="continuation">The action to invoke when the operation completes.</param>
        /// <remarks>
        /// Unlike <see cref="INotifyCompletion.OnCompleted"/>, <see cref="UnsafeOnCompleted"/> doesn't have to propagate ExecutionContext information.
        /// </remarks>
        void UnsafeOnCompleted(Action continuation);
    }
}
