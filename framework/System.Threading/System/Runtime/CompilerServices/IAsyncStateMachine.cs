//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Runtime.CompilerServices
{
    /// <summary>
    /// Represents state machines that are generated for asynchronous methods. This type is intended for compiler use only.
    /// </summary>
    public interface IAsyncStateMachine
    {
        /// <summary>
        /// Moves the state machine to its next state.
        /// </summary>
        void MoveNext();

        /// <summary>
        /// Configures the state machine with a heap-allocated replica.
        /// </summary>
        /// <param name="stateMachine">The heap-allocated replica.</param>
        void SetStateMachine(IAsyncStateMachine stateMachine);
    }
}
