using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public enum PenStyle
  {
    Solid,
    Dash,
    Dot,
    DashDot,
    DashDotDot,
    Null
  };

  public class Pen : IDisposable
  {
    private uint _handle;

    public Pen(uint color, ushort width, PenStyle style)
    {
      _handle = Syscall.CreatePen(color, width, (ushort) style);
    }

    public void Dispose()
    {
      Syscall.DisposePen(_handle);
    }

    public uint Handle
    {
      get { return _handle; }
    }

    public ushort Width
    {
      get { return Syscall.GetPenWidth(Handle); }
    }

    public uint Color
    {
      get { return Syscall.GetPenColor(Handle); }
    }

    public PenStyle Style
    {
      get { return (PenStyle) Syscall.GetPenStyle(Handle); }
    }
  };
}
