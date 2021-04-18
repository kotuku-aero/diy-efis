/*
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
  public struct Rect
  {
     /// <summary>
    /// 
    /// </summary>
    /// <param name="left"></param>
    /// <param name="right"></param>
    /// <param name="top"></param>
    /// <param name="bottom"></param>
   public static Rect Create(int left, int top, int right, int bottom)
    {
      return Syscall.CreateRect((short)left, (short)top, (short)right, (short)bottom);
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="pt"></param>
    /// <param name="extent"></param>
    public static Rect Create(Point pt, Extent extent)
    {
      return Syscall.CreateRect((short)pt.X, (short)pt.Y, (short)(pt.X + extent.Dx), (short)(pt.Y + extent.Dy));
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="top_left"></param>
    /// <param name="bottom_right"></param>
    public static Rect Create(Point top_left, Point bottom_right)
    {
      return Syscall.CreateRect((short)top_left.X, (short)top_left.Y, (short)bottom_right.X, (short)bottom_right.Y);
    }
    /// <summary>
    /// Width of the rectangle
    /// </summary>
    /// <value></value>
    public short Width
    {
      get { return (short) Math.Abs(Right - Left); }
    }
    /// <summary>
    /// Height of the rectangle
    /// </summary>
    /// <value></value>
    public int Height
    {
      get { return Math.Abs(Bottom - Top); }
    }
    /// <summary>
    /// Left position of the rectangle
    /// </summary>
    /// <value></value>
    public int Left
    {
      get { return Syscall.GetRectLeft(this); }
    }
    /// <summary>
    /// Top position of the rectangle
    /// </summary>
    /// <value></value>
    public int Top
    {
      get { return Syscall.GetRectTop(this); }
    }
    /// <summary>
    /// Rightmost rectangle point
    /// </summary>
    /// <value></value>
    public int Right
    {
      get { return Syscall.GetRectRight(this); }
    }
    /// <summary>
    /// Bottom position of the rectangle
    /// </summary>
    /// <value></value>
    public int Bottom
    {
      get { return Syscall.GetRectBottom(this); }
    }

    public Extent Extent
    {
      get { return Extent.Create(Math.Abs(Right - Left), Math.Abs(Bottom - Top)); }
    }

    public Point BottomRight
    {
      get { return Point.Create(Right, Bottom); }
    }

    public Point TopLeft
    {
      get { return Point.Create(Left, Top); }
    }

    public Point BottomLeft
    {
      get { return Point.Create(Left, Bottom); }
    }

    public Point TopRight
    {
      get { return Point.Create(Right, Top); }
    }

    public Rect Add(Rect value)
    {
      return CanFly.Syscall.CreateRect((short)(Left + value.Left), (short)(Top + value.Top), (short)(Right + value.Right), (short)(Bottom + value.Bottom));
    }

    public Rect Add(int dl, int dt, int dr, int db)
    {
      return CanFly.Syscall.CreateRect((short)(Left + dl), (short)(Top + dt), (short)(Right + dr), (short)(Bottom + db));
    }

    public Rect Expand(Extent value)
    {
      return CanFly.Syscall.CreateRect((short)(Left), (short)(Top), (short)(Right + value.Dx), (short)(Bottom + value.Dy));
    }

    public Rect Expand(int dx, int dy)
    {
      return CanFly.Syscall.CreateRect((short)(Left), (short)(Top), (short)(Right + dx), (short)(Bottom + dy));
    }

    public Rect MoveTo(Point pt)
    {
      return CanFly.Syscall.CreateRect((short)(pt.X), (short)(pt.Y), (short)(pt.X + Width), (short)(pt.Y + Height));
    }

    public Rect MoveTo(int x, int y)
    {
      return CanFly.Syscall.CreateRect((short)(x), (short)(y), (short)(x + Width), (short)(y + Height));
    }
  };
}
