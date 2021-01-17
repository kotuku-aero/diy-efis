//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System;

namespace System.Threading.Internal
{
    internal readonly struct WorkItem
    {
        public readonly WaitCallback callBack;
        public readonly object state;
        public readonly int workerId;
        public WorkItem(WaitCallback callBack, object state, int threadId = -1)
        {
            this.callBack = callBack;
            this.state = state;
            this.workerId = threadId;
        }

        public bool IsValid => callBack != null;
    }
}
