using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public abstract class Stream : IDisposable
  {
    private uint _handle;

    protected Stream(uint handle)
    {
      _handle = handle;
    }
    
    public void Dispose()
    {
      OnDispose();
    }

    /// <summary>
    /// Underlying OS handle
    /// </summary>
    internal uint Handle
    {
      get { return _handle; }
    }

    protected abstract void OnDispose();
  }

  public class RegistryStream : Stream
  {
    private RegistryStream(uint handle)
      : base(handle)
    {

    }

    public static RegistryStream Open(uint parent, string path)
    {
      return new RegistryStream(RegStreamOpen(parent, path));
    }

    protected override void OnDispose()
    {
      RegStreamClose(Handle);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamOpen(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamCreate(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamDelete(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool RegStreamEof(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern byte[] RegStreamRead(uint stream, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamGetPos(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamLength(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string RegStreamPath(uint stream, bool full_path);
  }
}