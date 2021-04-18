//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Provides the base class for enumerations.
  /// </summary>
  [Serializable]
  public abstract class Enum : ValueType
  {
    /// <summary>
    /// Determines whether one or more bit fields are set in the current instance.
    /// </summary>
    /// <param name="flag">An enumeration value.</param>
    /// <returns><see langword="true"/> if the bit field or bit fields that are set in flag are also set in the current instance; otherwise, <see langword="false"/>.</returns>
    /// <exception cref="ArgumentException"><paramref name="flag"/> is a different type than the current instance.</exception>
    public bool HasFlag(Enum flag) { return CanFly.Runtime.HasFlag(this, flag); }

    /// <summary>
    /// Converts the value of this instance to its equivalent string representation.
    /// </summary>
    /// <returns>The string representation of the value of this instance.</returns>
    /// <remarks>Available only in mscorlib build with support for System.Reflection.</remarks>
    public override String ToString()
    {
      throw new NotImplementedException();
    }

  }
}
