//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System.Diagnostics;
using System.Threading;

namespace System.Runtime.CompilerServices
{
    /// <summary>
    /// Represents a builder for asynchronous methods that do not return a value.
    /// </summary>
    public struct AsyncVoidMethodBuilder
    {
        /// <summary>
        /// Creates an instance of the AsyncVoidMethodBuilder class.
        /// </summary>
        /// <returns>A new instance of the builder.</returns>
        public static AsyncVoidMethodBuilder Create()
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:Create");
            return new AsyncVoidMethodBuilder();
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
            Debug.WriteLine($"AsyncVoidMethodBuilder:AwaitOnCompleted");
            var _stateMachine = stateMachine;
            awaiter.OnCompleted(() =>
            {
                Debug.WriteLine($"AsyncVoidMethodBuilder:OnCompleted");
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
            Debug.WriteLine($"AsyncVoidMethodBuilder:AwaitUnsafeOnCompleted");
            var _stateMachine = stateMachine;
            awaiter.OnCompleted(() =>
            {
                Debug.WriteLine($"AsyncVoidMethodBuilder:OnCompleted");
                _stateMachine.MoveNext();
            });
        }

        /// <summary>
        /// Binds an exception to the method builder.
        /// </summary>
        /// <param name="exception">The exception to bind.</param>
        public void SetException(Exception exception)
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:SetException");
        }

        /// <summary>
        /// Marks the method builder as successfully completed.
        /// </summary>
        /// <remarks>
        /// This type and its members are intended for use by the compiler.
        /// </remarks>
        public void SetResult()
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:SetResult");
        }

        /// <summary>
        /// Associates the builder with the specified state machine.
        /// </summary>
        /// <param name="stateMachine">The state machine instance to associate with the builder.</param>
        public void SetStateMachine(IAsyncStateMachine stateMachine)
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:SetStateMachine");
        }

        /// <summary>
        /// Begins running the builder with the associated state machine.
        /// </summary>
        /// <typeparam name="TStateMachine">The type of the state machine.</typeparam>
        /// <param name="stateMachine">The state machine instance, passed by reference.</param>
        public void Start<TStateMachine>(ref TStateMachine stateMachine) where TStateMachine : IAsyncStateMachine
        {
            Debug.WriteLine($"AsyncVoidMethodBuilder:Start");
            stateMachine.MoveNext();
        }
    }
}
