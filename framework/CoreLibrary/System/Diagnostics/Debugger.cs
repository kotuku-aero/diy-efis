//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System.Diagnostics
{
  /// <summary>
  /// Enables communication with a debugger. This class cannot be inherited.
  /// </summary>
  public class Debugger
  {
    /// <summary>
    /// Gets a value that indicates whether a debugger is attached to the process.
    /// </summary>
    /// <value>true if a debugger is attached; otherwise, false.</value>
    public static bool IsAttached
    {
      get { return CanFly.Runtime.DebuggerIsAttached(); }
    }

    /// <summary>
    /// Signals a breakpoint to an attached debugger.
    /// </summary>
    public static void Break()
    {
      CanFly.Runtime.DebuggerBreak();
    }
  }
}
