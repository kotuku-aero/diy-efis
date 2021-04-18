//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

namespace System
{
  /// <summary>
  /// Represents a weak reference, which references an object while still allowing that object to be reclaimed by garbage collection.
  /// </summary>
  [Serializable]
  public class WeakReference
  {
    /// <summary>
    /// Initializes a new instance of the WeakReference class, referencing the specified object.
    /// </summary>
    /// <param name="target">The object to track or null.</param>
    public WeakReference(Object target)
    {
      CanFly.Runtime.SetWeakReferenceTarget(this, target);
    }

    /// <summary>
    /// Gets an indication whether the object referenced by the current WeakReference object has been garbage collected.
    /// </summary>
    /// <value>true if the object referenced by the current WeakReference object has not been garbage collected and is still accessible; otherwise, false.</value>
    public virtual bool IsAlive
    {
      get { return CanFly.Runtime.WeakReferenceIsAlive(this); }
    }

    /// <summary>
    /// Gets or sets the object (the target) referenced by the current WeakReference object.
    /// </summary>
    /// <value>null if the object referenced by the current WeakReference object has been garbage collected; otherwise, a reference to the object referenced by the current WeakReference object.</value>
    public virtual Object Target
    {
      get { return CanFly.Runtime.GetWeakReferenceTarget(this); }
      set { CanFly.Runtime.SetWeakReferenceTarget(this, value); }
    }
  }
}
