//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Runtime.CompilerServices
{
    /// <summary>
    /// Represents an operation that schedules continuations when it completes.
    /// </summary>
    public interface INotifyCompletion
    {
        /// <summary>
        /// Schedules the continuation action that's invoked when the instance completes.
        /// </summary>
        /// <param name="continuation">The action to invoke when the operation completes.</param>
        void OnCompleted(Action continuation);
    }
}
