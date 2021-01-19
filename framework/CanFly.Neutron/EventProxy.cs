using System;

namespace CanFly
{
  /// <summary>
  /// Generic callback for a CanFly message
  /// </summary>
  /// <param name="msg"></param>
  public delegate void CanFlyMsgHandler(CanFlyMsg msg);
  /// <summary>
  /// Holder of a callback from the event system to the CLR
  /// </summary>
  public sealed class EventProxy : IDisposable
  {
    private ushort _canId;
    private CanFlyMsgHandler _callback;
    public EventProxy(ushort canId, CanFlyMsgHandler callback)
    {
      _canId = canId;
      Syscall.AddEvent(canId, OnMessage);
    }

    private void OnMessage(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7)
    {
      CanFlyMsg msg = new CanFlyMsg(flags, b0, b1, b2, b3, b4, b5, b6, b7);
      _callback(msg);
    }

    public void Dispose()
    {
      Syscall.RemoveEvent(_canId, OnMessage);
    }
  }
}
