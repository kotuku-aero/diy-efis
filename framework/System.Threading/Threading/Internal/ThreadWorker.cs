//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

using System;
using System.Diagnostics;

namespace System.Threading.Internal
{
    internal class ThreadWorker
    {
        Thread thread;
        WaitCallback callback;
        object state;
        internal bool IsFree => thread == null || callback == null || thread.ThreadState == ThreadState.Suspended;
        internal int Id => thread != null ? (int) thread.ManagedThreadId : -1;
        internal SynchronizationContext SynchronizationContext { get; private set; }
        AutoResetEvent run = new AutoResetEvent(false);

        void Start()
        {
            thread = new Thread(() =>
            {
                while (true)
                {
                    Debug.WriteLine($"Thread {Id} started");
                    try
                    {
                        callback(state);
                        callback = null;
                        state = null;
                    }
                    catch { }
                    ThreadPool.RunPendingWorkItems(this);
                    //if more job was posted to this pool as a result of call to RunPendingWorkItems, continue the work immediately
                    if (callback != null)
                        continue;
                    Debug.WriteLine($"Thread {Id} exited");
                    run.WaitOne();
                }
            });
            SynchronizationContext = new NanoFrameworkSynchronizationContext((int) thread.ManagedThreadId);
            thread.Start();
        }

        public void Post(WaitCallback callback, object state)
        {
            this.state = state;
            this.callback = callback;
            if (thread == null)
            {
                Start();
            }
            else
            {
                run.Set();
            }
        }
    }

}
