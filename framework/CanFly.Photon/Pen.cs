﻿using System;
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
      Syscall.CreatePen(color, width, (ushort) style, out _handle);
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
      get
      {
        ushort value;
        Syscall.GetPenWidth(Handle, out value);
        return value;
      }
    }

    public uint Color
    {
      get
      {
        uint color;
        Syscall.GetPenColor(Handle, out color);
        return color;
      }
    }

    public PenStyle Style
    {
      get 
      {
        ushort style;
        Syscall.GetPenStyle(Handle, out style);
        return (PenStyle)style;
      }
    }
  };
}