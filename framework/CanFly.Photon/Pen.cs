﻿/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;

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

    /// <summary>
    /// Return the handle or 0 if null
    /// </summary>
    /// <param name="pen"></param>
    /// <returns></returns>
    static public uint SafeHandle(Pen pen)
    {
      return pen == null ? 0 : pen.Handle;
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
