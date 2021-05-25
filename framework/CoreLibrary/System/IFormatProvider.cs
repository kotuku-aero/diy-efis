namespace System
{
    /// <summary>
    /// Provides a mechanism for retrieving an object to control formatting.
    /// </summary>
    /// <remarks>Available only in mscorlib build with support for System.Reflection.</remarks>
    public interface IFormatProvider
    {
        /// <summary>
        /// Returns an object that provides formatting services for the specified type.
        /// </summary>
        /// <returns>An instance of the object that can format the supplied object</returns>
        Object GetFormat(Object formatType);
    }
}

