using System;

namespace System.Threading
{
    /// <summary>
    /// Propagates notification that operation should be cancelled.
    /// </summary>
    public struct CancellationToken
    {
        CancellationTokenSource source;
        /// <summary>
        /// Initializes the System.Threading.CancellationToken.
        /// </summary>
        /// <param name="source">The canceled state for the token.</param>
        internal CancellationToken(CancellationTokenSource source)
        {
            this.source = source;
        }

        /// <summary>
        /// Initializes the System.Threading.CancellationToken.
        /// </summary>
        /// <param name="canceled">The canceled state for the token.</param>
        public CancellationToken(bool canceled)
        {
            this.source = null;
        }

        static readonly CancellationToken none = new CancellationToken();
       
        /// <summary>
        ///  Returns an empty System.Threading.CancellationToken value.
        /// </summary>
        /// <returns>An empty cancellation token.</returns>
        public static CancellationToken None => none;

        /// <summary>
        /// Gets whether cancellation has been requested for this token.
        /// </summary>
        /// <returns>true if cancellation has been requested for this token; otherwise, false.</returns>
        public bool IsCancellationRequested => source.IsCancellationRequested;

        /// <summary>
        /// Gets whether this token is capable of being in the canceled state.
        /// </summary>
        /// <returns> true if this token is capable of being in the canceled state; otherwise, false.</returns>
        public bool CanBeCanceled => !source.IsCancellationRequested;

        /// <summary>
        /// Gets a System.Threading.WaitHandle that is signaled when the token is canceled.
        /// </summary>
        /// <returns>A System.Threading.WaitHandle that is signaled when the token is canceled.</returns>
        public WaitHandle WaitHandle => source.WaitHandle;

        /// <summary>
        /// Determines whether the current System.Threading.CancellationToken instance is equal to the specified token.
        /// </summary>
        /// <param name="other"></param>
        /// <returns> 
        ///  true if the instances are equal; otherwise, false. Two tokens are equal if they
        ///  are associated with the same System.Threading.CancellationTokenSource or if they
        ///  were both constructed from public System.Threading.CancellationToken constructors
        ///  and their System.Threading.CancellationToken.IsCancellationRequested values are
        ///  equal.
        /// </returns>
        /// <exception cref="System.ObjectDisposedException">An associated System.Threading.CancellationTokenSource has been disposed.</exception>
        public bool Equals(CancellationToken other)
        {
            if (this.source == other.source)
                return true;
            return false;
        }
        /// <summary>
        ///  Determines whether the current System.Threading.CancellationToken instance is equal to the specified System.Object.
        /// </summary>
        /// <param name="other">The other object to which to compare this instance.</param>
        /// <returns>
        ///  true if other is a System.Threading.CancellationToken and if the two instances
        ///  are equal; otherwise, false. Two tokens are equal if they are associated with
        ///  the same System.Threading.CancellationTokenSource or if they were both constructed
        ///  from public System.Threading.CancellationToken constructors and their System.Threading.CancellationToken.IsCancellationRequested
        ///  values are equal.
        /// </returns>
        /// <exception cref="System.ObjectDisposedException">An associated System.Threading.CancellationTokenSource has been disposed.</exception>
        public override bool Equals(object other)
        {
            if (other is CancellationToken token)
            {
                return Equals(token);
            }
            return false;
        }

        /// <summary>
        /// Serves as a hash function for a System.Threading.CancellationToken.
        /// </summary>
        /// <returns>A hash code for the current System.Threading.CancellationToken instance.</returns>
        public override int GetHashCode()
        {
            return source.GetHashCode();
        }
        
        /// <summary>
        /// Registers a delegate that will be called when this System.Threading.CancellationToken  is canceled.
        /// </summary>
        /// <param name="callback">The delegate to be executed when the System.Threading.CancellationToken is canceled.</param>
        /// <returns>The System.Threading.CancellationTokenRegistration instance that can be used to deregister the callback.</returns>
        /// <exception cref="System.ArgumentNullException">callback is null</exception>
        public CancellationTokenRegistration Register(Action callback)
        {
            return Register(callback, true);
        }

        /// <summary>
        /// Registers a delegate that will be called when this System.Threading.CancellationToken  is canceled.
        /// </summary>
        /// <param name="callback">The delegate to be executed when the System.Threading.CancellationToken is canceled.</param>
        /// <param name="useSynchronizationContext">A value that indicates whether to capture the current System.Threading.SynchronizationContext and use it when invoking the callback.</param>
        /// <returns>The System.Threading.CancellationTokenRegistration instance that can be used to deregister the callback.</returns>
        /// <exception cref="System.ArgumentNullException">callback is null</exception>
        public CancellationTokenRegistration Register(Action callback, bool useSynchronizationContext)
        {
            callback = callback ?? throw new ArgumentNullException("callback");
            if (source.IsCancellationRequested)
            {
                callback();
            }
            else if (source != null)
            {
                return source.NotifyOnCancelled(callback, useSynchronizationContext);
            }
            return new CancellationTokenRegistration();
        }
        /// <summary>
        /// Registers a delegate that will be called when this System.Threading.CancellationToken  is canceled.
        /// </summary>
        /// <param name="callback">The delegate to be executed when the System.Threading.CancellationToken is canceled.</param>
        /// <param name="state">The state to pass to the callback when the delegate is invoked. This may be null.</param>
        /// <returns>The System.Threading.CancellationTokenRegistration instance that can be used to deregister the callback.</returns>
        /// <exception cref="System.ArgumentNullException">callback is null</exception>
        public CancellationTokenRegistration Register(Action<object> callback, object state)
        {
            return Register(() => callback(state), true);
        }
        /// <summary>
        /// Registers a delegate that will be called when this System.Threading.CancellationToken  is canceled.
        /// </summary>
        /// <param name="callback">The delegate to be executed when the System.Threading.CancellationToken is canceled.</param>
        /// <param name="state">The state to pass to the callback when the delegate is invoked. This may be null.</param>
        /// <param name="useSynchronizationContext">A Boolean value that indicates whether to capture the current System.Threading.SynchronizationContext and use it when invoking the callback.</param>
        /// <returns>The System.Threading.CancellationTokenRegistration instance that can be used to deregister the callback.</returns>
        /// <exception cref="System.ArgumentNullException">callback is null</exception>
        public CancellationTokenRegistration Register(Action<object> callback, object state, bool useSynchronizationContext)
        {
            return Register(() => callback(state), useSynchronizationContext);
        }

        /// <summary>
        /// Throws a System.OperationCanceledException if this token has had cancellation requested
        /// </summary>
        public void ThrowIfCancellationRequested()
        {
            if (source.IsCancellationRequested)
                throw new SystemException();//TODO: throw OperationCanceledException, not defined yet
        }

        /// <summary>
        /// Determines whether two System.Threading.CancellationToken instances are equal.
        /// </summary>
        /// <param name="left">The first instance.</param>
        /// <param name="right">The second instance.</param>
        /// <returns> true if the instances are equal; otherwise, false.</returns>
        public static bool operator ==(CancellationToken left, CancellationToken right)
        {
            return left.Equals(right);
        }

        /// <summary>
        /// Determines whether two System.Threading.CancellationToken instances are not equal.
        /// </summary>
        /// <param name="left">The first instance.</param>
        /// <param name="right">The second instance.</param>
        /// <returns>true if the instances are not equal; otherwise, false.</returns>
        public static bool operator !=(CancellationToken left, CancellationToken right)
        {
            return !left.Equals(right);
        }
    }
}
