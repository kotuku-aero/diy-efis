//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Represents the standard input, output, and error streams for console applications. This class cannot be inherited.
  /// </summary>
  [Serializable]
  public static class Console
  {
    /// <summary>
    /// Writes the specified string value to the standard output stream.
    /// </summary>
    /// <param name="value">The value to write.</param>
    [Obsolete("This method is going to be removed in a future version. Call Debug.Write instead.")]
    public static void Write(string value)
    {
      CanFly.Runtime.OutNative(value, false);
    }

    /// <summary>
    /// Writes the specified string value, followed by the current line terminator, to the standard output stream.
    /// </summary>
    /// <param name="value">The value to write.</param>
    [Obsolete("This method is going to be removed in a future version. Call Debug.Write instead.")]
    public static void WriteLine(string value)
    {
      CanFly.Runtime.OutNative(value, true);
    }
  }
}
