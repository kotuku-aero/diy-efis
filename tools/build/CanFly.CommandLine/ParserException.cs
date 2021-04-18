using System;
using System.Runtime.Serialization;

namespace CanFly.CommandLine
{
    /// <summary>
    /// This exception is thrown when a generic parsing error occurs.
    /// </summary>
    [Serializable]
    public class ParserException : Exception
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ParserException"/> class. The exception is thrown
        /// when something unexpected occurs during the parsing process.
        /// </summary>
        public ParserException()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ParserException"/> class. The exception is thrown
        /// when something unexpected occurs during the parsing process.
        /// </summary>
        /// <param name="message">Error message string.</param>
        public ParserException(string message)
            : base(message)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ParserException"/> class. The exception is thrown
        /// when something unexpected occurs during the parsing process.
        /// </summary>
        /// <param name="message">Error message string.</param>
        /// <param name="innerException">Inner exception reference.</param>
        public ParserException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ParserException"/> class. The exception is thrown
        /// when something unexpected occurs during the parsing process.
        /// </summary>
        /// <param name="info">The object that holds the serialized object data.</param>
        /// <param name="context">The contextual information about the source or destination.</param>
        protected ParserException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}