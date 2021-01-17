using System;
using System.Collections;

namespace System.Threading
{
    /// <summary>
    /// Signals to a System.Threading.CancellationToken that it should be canceled.
    /// </summary>
    public class CancellationTokenSource : IDisposable
    {
        /// <summary>
        ///  Initializes a new instance of the System.Threading.CancellationTokenSource class.
        /// </summary>
        public CancellationTokenSource()
        {
        }

        /// <summary>
        ///  Initializes a new instance of the System.Threading.CancellationTokenSource class
        //   that will be canceled after the specified time span.
        /// </summary>
        /// <param name="delay">The time interval to wait before canceling this System.Threading.CancellationTokenSource.</param>
        public CancellationTokenSource(TimeSpan delay)
        {
            CancelAfter((int)delay.TotalMilliseconds);
        }

        /// <summary>
        /// Initializes a new instance of the System.Threading.CancellationTokenSource class
        //  that will be canceled after the specified delay in milliseconds.
        /// </summary>
        /// <param name="millisecondsDelay"> The time interval in milliseconds to wait before canceling this System.Threading.CancellationTokenSource.</param>
        public CancellationTokenSource(int millisecondsDelay)
        {
            CancelAfter(millisecondsDelay);
        }

        /// <summary>
        ///  Gets whether cancellation has been requested for this System.Threading.CancellationTokenSource.
        /// </summary>
        /// <returns>
        /// true if cancellation has been requested for this System.Threading.CancellationTokenSource;
        /// otherwise, false.
        ///</returns>
        public bool IsCancellationRequested { get; private set; }

        /// <summary>
        /// Gets the System.Threading.CancellationToken associated with this System.Threading.CancellationTokenSource.
        /// </summary>
        /// <returns>
        /// The System.Threading.CancellationToken associated with this System.Threading.CancellationTokenSource.
        /// </returns>
        public CancellationToken Token => new CancellationToken(this);

        /// <summary>
        ///  Communicates a request for cancellation.
        /// </summary>
        /// <exception cref="System.ObjectDisposedException">This System.Threading.CancellationTokenSource has been disposed</exception>
        public void Cancel()
        {
            DoCancellation();
        }
        /// <summary>
        /// true if exceptions should immediately propagate; otherwise, false.
        /// </summary>
        /// <param name="throwOnFirstException"></param>
        /// <exception cref="System.ObjectDisposedException">This System.Threading.CancellationTokenSource has been disposed.</exception>
        public void Cancel(bool throwOnFirstException)
        {
            DoCancellation();
        }
        /// <summary>
        /// Schedules a cancel operation on this System.Threading.CancellationTokenSource
        /// after the specified time span.
        /// </summary>
        /// <param name="delay">The time span to wait before canceling this System.Threading.CancellationTokenSource.</param>
        /// <exception cref="System.ObjectDisposedException">This System.Threading.CancellationTokenSource has been disposed.</exception>
        public void CancelAfter(TimeSpan delay)
        {
            CancelAfter((int)delay.TotalMilliseconds);
        }

        /// <summary>
        /// Schedules a cancel operation on this System.Threading.CancellationTokenSource
        ///  after the specified number of milliseconds.
        /// </summary>
        /// <param name="millisecondsDelay">The time span to wait before canceling this System.Threading.CancellationTokenSource.</param>
        /// <exception cref="System.ObjectDisposedException">This System.Threading.CancellationTokenSource has been disposed.</exception>
        public void CancelAfter(int millisecondsDelay)
        {
            if (isDisposed)
                throw new ObjectDisposedException();
            new Thread(() =>
            {
                Thread.Sleep(millisecondsDelay);
                DoCancellation();
            }).Start();
        }

        struct CancellationNotificationInfo
        {
            public Action Action;
            public SynchronizationContext SynchronizationContext;
        }

        AutoResetEvent cancelSignal;
        internal WaitHandle WaitHandle
        {
            get
            {
                if (cancelSignal == null)
                    cancelSignal = new AutoResetEvent(false);
                return cancelSignal;
            }
        }

        ArrayList cancelNotifications = new ArrayList();
        void DoCancellation()
        {
            if (isDisposed)
                throw new ObjectDisposedException();
            if (IsCancellationRequested)
            {
                throw new Exception("Already cancelled");
            }
            IsCancellationRequested = true;
            if (cancelSignal != null)
            {
                cancelSignal.Set();
            }
            foreach(var item in cancelNotifications)
            {
                var kv = (DictionaryEntry)item;
                var notification = (CancellationNotificationInfo)kv.Value;
                if (notification.SynchronizationContext != null)
                {
                    notification.SynchronizationContext.Post((_) => notification.Action(), null);
                }
                else
                {
                    notification.Action();
                }
            }
            cancelNotifications.Clear();
        }

        internal CancellationTokenRegistration NotifyOnCancelled(Action action, bool useSynchronizationContext)
        {
            int id = action.GetHashCode();
            var entry = new DictionaryEntry(id, new CancellationNotificationInfo()
            {
                Action = action,
                SynchronizationContext = useSynchronizationContext ? SynchronizationContext.Current : null
            });
            CancellationTokenRegistration registration = new CancellationTokenRegistration(this, () =>
            {
                cancelNotifications.Remove(id);
            });
            cancelNotifications.Add(entry);
            return registration;
        }

        bool isDisposed;
        /// <summary>
        /// Releases all resources used by the current instance of the System.Threading.CancellationTokenSource
        /// class.
        /// </summary>
        public void Dispose()
        {
            isDisposed = true;
        }
    }
}
