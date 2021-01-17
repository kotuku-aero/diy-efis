//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading
{
    /// <summary>
    /// Provides the basic functionality for propagating a synchronization context in various synchronization models.
    /// </summary>
    public class SynchronizationContext
    {
        /// <summary>
        /// Creates a new instance of the <see cref="SynchronizationContext"/> class.
        /// </summary>
        public SynchronizationContext()
        {

        }

        static readonly SynchronizationContext defaultSyncronizationContext = new SynchronizationContext();

        /// <summary>
        /// Gets the synchronization context for the current thread.
        /// </summary>
        /// <value>
        ///  A <see cref="SynchronizationContext"/> object representing the current synchronization context.
        /// </value>
        public static SynchronizationContext Current
        {
            get
            {
                var context = ThreadPool.Current?.SynchronizationContext;
                if (context == null)
                    context = defaultSyncronizationContext;
                return context;
            }
        }

        /// <summary>
        /// Sets the current synchronization context.
        /// </summary>
        /// <param name="syncContext">The <see cref="SynchronizationContext"/> object to be set.</param>
        public static void SetSynchronizationContext(SynchronizationContext syncContext)
        {
            throw new NotSupportedException();
        }

        /// <summary>
        /// When overridden in a derived class, creates a copy of the synchronization context.
        /// </summary>
        /// <returns>A new System.Threading.SynchronizationContext object.</returns>
        public virtual SynchronizationContext CreateCopy()
        { 
            return new SynchronizationContext(); 
        }
        
        bool waitNotification;

        /// <summary>
        /// Determines if wait notification is required.
        /// </summary>
        /// <returns><see langword="true"/> if wait notification is required; otherwise, <see langword="false"/>.</returns>
        public bool IsWaitNotificationRequired() 
        { 
            return waitNotification; 
        }

        /// <summary>
        ///  When overridden in a derived class, responds to the notification that an operation has completed.
        /// </summary>
        public virtual void OperationCompleted() { }

        /// <summary>
        /// When overridden in a derived class, responds to the notification that an operation has started.
        /// </summary>
        public virtual void OperationStarted() { }

        /// <summary>
        ///  When overridden in a derived class, dispatches an asynchronous message to a synchronization context.
        /// </summary>
        /// <param name="d">The <see cref="SendOrPostCallback"/> delegate to call.</param>
        /// <param name="state">The object passed to the delegate.</param>
        public virtual void Post(SendOrPostCallback d, object state)
        {
            ThreadPool.QueueUserWorkItem(new WaitCallback(d), state);
        }

        /// <summary>
        /// When overridden in a derived class, dispatches a synchronous message to a synchronization context.
        /// </summary>
        /// <param name="d">The <see cref="SendOrPostCallback"/> delegate to call.</param>
        /// <param name="state">The object passed to the delegate.</param>
        public virtual void Send(SendOrPostCallback d, object state)
        { 
            d(state); 
        }

        /// <summary>
        /// Sets notification that wait notification is required and prepares the callback method so it can be called more reliably when a wait occurs.
        /// </summary>
        protected void SetWaitNotificationRequired() { waitNotification = true; }
    }
}
