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
      _handle = Photon.CreatePen(color, width, (ushort) style);
    }

    public void Dispose()
    {
      Photon.DisposePen(_handle);
    }

    public uint Handle
    {
      get { return _handle; }
    }

    public ushort Width
    {
      get { return Photon.GetPenWidth(Handle); }
    }

    public uint Color
    {
      get { return Photon.GetPenColor(Handle); }
    }

    public PenStyle Style
    {
      get { return (PenStyle) Photon.GetPenStyle(Handle); }
    }
  };
}
