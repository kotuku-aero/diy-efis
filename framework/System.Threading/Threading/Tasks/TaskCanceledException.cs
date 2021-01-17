//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Threading.Tasks
{
    /// <summary>
    /// Represents an exception used to communicate task cancellation.
    /// </summary>
    public class TaskCanceledException : Exception
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="TaskCanceledException"/> class with a system-supplied message that describes the error.
        /// </summary>
        public TaskCanceledException()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="TaskCanceledException"/> class with a system-supplied message that describes the error.
        /// </summary>
        /// <param name="message">The message that describes the exception. The caller of this constructor is required to ensure that this string has been localized for the current system culture.</param>
        public TaskCanceledException(string message) : base(message)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="TaskCanceledException"/> class with a system-supplied message that describes the error.
        /// </summary>
        /// <param name="message">The message that describes the exception. The caller of this constructor is required to ensure that this string has been localized for the current system culture.</param>
        /// <param name="innerException">The exception that is the cause of the current exception. If the <paramref name="innerException"/> parameter is not <see langword="null"/>, the current exception is raised in a catch block that handles the inner exception.</param>
        public TaskCanceledException(string message, Exception innerException) : base(message, innerException)
        {
        }
    }
}
