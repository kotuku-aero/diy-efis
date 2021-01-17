using System;

namespace System.Threading
{
    /// <summary>
    /// Represents a callback delegate that has been registered with a System.Threading.CancellationToken.
    /// </summary>
    public struct CancellationTokenRegistration : IDisposable
    {
        CancellationTokenSource source;
        Action onDisposed;
        internal CancellationTokenRegistration(CancellationTokenSource source, Action onDisposed)
        {
            this.source = source;
            this.onDisposed = onDisposed;
        }

        /// <summary>
        /// Releases all resources used by the current instance of the System.Threading.CancellationTokenRegistration
        /// </summary>
        public void Dispose()
        {
            onDisposed?.Invoke();
        }

        /// <summary>
        /// Determines whether the current System.Threading.CancellationTokenRegistration instance is equal to the specified System.Threading.CancellationTokenRegistration.
        /// </summary>
        /// <param name="obj">The other System.Threading.CancellationTokenRegistration to which to compare  this instance.</param>
        /// <returns>
        ///     True, if both this and other are equal. False, otherwise. Two System.Threading.CancellationTokenRegistration
        ///     instances are equal if they both refer to the output of a single call to the
        ///     same Register method of a System.Threading.CancellationToken.
        ///</returns>
        public override bool Equals(object obj)
        {
            if (obj is CancellationTokenRegistration other)
            {
                return source == other.source;
            }
            return false;
        }

        /// <summary>
        /// Determines whether the current System.Threading.CancellationTokenRegistration instance is equal to the specified System.Threading.CancellationTokenRegistration.
        /// </summary>
        /// <param name="other">The other System.Threading.CancellationTokenRegistration to which to compare  this instance.</param>
        /// <returns>
        ///     True, if both this and other are equal. False, otherwise. Two System.Threading.CancellationTokenRegistration
        ///     instances are equal if they both refer to the output of a single call to the
        ///     same Register method of a System.Threading.CancellationToken.
        ///</returns>
        public bool Equals(CancellationTokenRegistration other)
        {
            return source == other.source;
        }

        /// <summary>
        /// Serves as a hash function for a System.Threading.CancellationTokenRegistration.
        /// </summary>
        /// <returns>A hash code for the current System.Threading.CancellationTokenRegistration instance.</returns>
        public override int GetHashCode()
        {
            return source.GetHashCode();
        }

        /// <summary>
        /// Determines whether two System.Threading.CancellationTokenRegistration instances are equal.
        /// </summary>
        /// <param name="left">The first instance.</param>
        /// <param name="right">The second instance.</param>
        /// <returns>True if the instances are not equal; otherwise, false.</returns>
        public static bool operator ==(CancellationTokenRegistration left, CancellationTokenRegistration right)
        {
            return left.Equals(right);
        }

        /// <summary>
        /// Determines whether two System.Threading.CancellationTokenRegistration instances are not equal.
        /// </summary>
        /// <param name="left">The first instance.</param>
        /// <param name="right">The second instance.</param>
        /// <returns>True if the instances are not equal; otherwise, false.</returns>
        public static bool operator !=(CancellationTokenRegistration left, CancellationTokenRegistration right)
        {
            return !left.Equals(right);
        }
    }
}
