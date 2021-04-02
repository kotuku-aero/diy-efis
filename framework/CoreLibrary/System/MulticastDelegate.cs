//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Represents a multicast delegate; that is, a delegate that can have more than one element in its invocation list.
  /// </summary>
  [Serializable]
  public abstract class MulticastDelegate : Delegate
  {

    /// <summary>
    /// Determines whether two MulticastDelegate objects are equal.
    /// </summary>
    /// <param name="d1">The left operand.</param>
    /// <param name="d2">The right operand.</param>
    /// <returns>true if d1 and d2 have the same invocation lists; otherwise, false.</returns>
    public static bool operator ==(MulticastDelegate d1, MulticastDelegate d2) { return CanFly.Runtime.Equals(d1, d2); }

    /// <summary>
    /// Determines whether two MulticastDelegate objects are not equal.
    /// </summary>
    /// <param name="d1">The left operand.</param>
    /// <param name="d2">The right operand.</param>
    /// <returns>rue if d1 and d2 do not have the same invocation lists; otherwise, false.</returns>
    public static bool operator !=(MulticastDelegate d1, MulticastDelegate d2) { return CanFly.Runtime.NotEquals(d1, d2); }

    /// <summary>
    /// Returns the hash code for this instance.
    /// </summary>
    /// <param name="obj">A 32-bit signed integer hash code.</param>
    /// <returns></returns>
    public override bool Equals(object obj)
    {
      return base.Equals(obj);
    }
  }
}
