//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Provides the base class for value types.
  /// </summary>
  [Serializable]
  public abstract class ValueType
  {

    /// <summary>
    /// Indicates whether this instance and a specified object are equal.
    /// </summary>
    /// <param name="obj">The object to compare with the current instance.</param>
    /// <returns>true if obj and this instance are the same type and represent the same value; otherwise, false.</returns>
    public override bool Equals(Object obj) { return CanFly.Runtime.ValueTypeEquals(this, obj); }
  }
}
