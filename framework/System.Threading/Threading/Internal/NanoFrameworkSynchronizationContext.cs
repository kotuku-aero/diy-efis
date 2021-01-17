//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System;

namespace System.Threading.Internal
{
    public class NanoFrameworkSynchronizationContext:SynchronizationContext
    {
        int workerId;

        public NanoFrameworkSynchronizationContext(int workerId)
        {
            this.workerId = workerId;
        }

        public override void Post(SendOrPostCallback d, object state)
        {
            ThreadPool.QueueUserWorkItemOnSpecificWorker(workerId, (s) => d(s), state);
        }

        //public override void Send(SendOrPostCallback d, object state)
        //{
        //    ThreadPool.QueueUserWorkItemOnSpecificThread(threadId, (s) => d(s), state);
        //}
    }
}
