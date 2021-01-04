using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  /// <summary>
  /// A multicast (combinable) delegate that defines the event handler for a canfly.
  /// </summary>
  /// <param name="flags">flags for the message</param>
  /// <param name="b0">byte 0</param>
  /// <param name="b1">byte 1</param>
  /// <param name="b2">byte 2</param>
  /// <param name="b3">byte 3</param>
  /// <param name="b4">byte 4</param>
  /// <param name="b5">byte 5</param>
  /// <param name="b6">byte 6</param>
  /// <param name="b7">byte 7</param>
  public delegate void CanFlyEventHandler(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);

  /// <summary>
  /// Initializes a new instance of the <see cref="CanFlyEventDispatcher"/> class.
  /// </summary>
  public class CanFlyEventDispatcher : IDisposable
  {
    /// <summary>
    /// The event handler for this <see cref="CanFlyEventDispatcher"/> object (a multicast combinable delegate).
    /// </summary>
    protected CanFlyEventHandler threadSpawn = null;

    /// <summary>
    /// The event handler for this <see cref="CanFlyEventDispatcher"/> object (a multicast combinable delegate).
    /// </summary>
    protected CanFlyEventHandler callbacks = null;

    /// <summary>
    /// Set to <code>true</code> if this <see cref="CanFlyEventDispatcher"/> object has been disposed, or <code>false</code> if not.
    /// </summary>
    protected bool disposed = false;

    // this field is required in native so need to be kept here
#pragma warning disable 169
    private object _CanFlyEventDispatcher;

    /// <summary>
    /// Initializes a new instance of the <see cref="CanFlyEventDispatcher"/> class.
    /// </summary>
    /// <param name="driverName">The name of a registered driver.</param>
    /// <param name="driverData">Driver data to be used when initializing the driver.</param>
    [MethodImpl(MethodImplOptions.InternalCall)]
    extern public CanFlyEventDispatcher();

    /// <summary>
    /// Releases resources used by this <see cref="CanFlyEventDispatcher"/> object.
    /// </summary>
    /// <param name="disposing"><code>true</code> if in the process of disposing, or <code>false</code> otherwise.</param>
    [MethodImpl(MethodImplOptions.InternalCall)]
    extern protected virtual void Dispose(bool disposing);

#pragma warning disable 1591
    ~CanFlyEventDispatcher()
    {
      Dispose(false);
    }

    /// <summary>
    /// Releases resources used by this <see cref="CanFlyEventDispatcher"/> object.
    /// </summary>
    [MethodImpl(MethodImplOptions.Synchronized)]
    public void Dispose()
    {
      if (!disposed)
      {
        Dispose(true);

        GC.SuppressFinalize(this);

        disposed = true;
      }
    }

    /// <summary>
    /// Adds or removes callback methods for native events.
    /// </summary>
    /// <remarks>Applications can subscribe to this event to be notified when an interrupt event occurs.</remarks>
    public event CanFlyEventHandler OnMessage
    {
      [MethodImpl(MethodImplOptions.Synchronized)]
      add
      {
        if (disposed)
        {
          throw new ObjectDisposedException();
        }

        CanFlyEventHandler callbacksOld = callbacks;
        CanFlyEventHandler callbacksNew = (CanFlyEventHandler)Delegate.Combine(callbacksOld, value);

        try
        {
          callbacks = callbacksNew;

          if (callbacksNew != null)
          {
            if (callbacksNew.Equals(value) == false)
              callbacksNew = new CanFlyEventHandler(this.MultiCastCase);
          }

          threadSpawn = callbacksNew;
        }
        catch
        {
          callbacks = callbacksOld;
          throw;
        }
      }

      [MethodImpl(MethodImplOptions.Synchronized)]
      remove
      {
        if (disposed)
          throw new ObjectDisposedException();

        CanFlyEventHandler callbacksOld = callbacks;
        CanFlyEventHandler callbacksNew = (CanFlyEventHandler)Delegate.Remove(callbacksOld, value);

        try
        {
          callbacks = callbacksNew;
        }
        catch
        {
          callbacks = callbacksOld;

          throw;
        }
      }
    }

    private void MultiCastCase(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7)
    {
      callbacks?.Invoke(flags, b0, b1, b2, b3, b4, b5, b6, b7);
    }
  }
}
