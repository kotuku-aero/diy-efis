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
  public class Rect
  {
    private int _left;
    private int _top;
    private int _right;
    private int _bottom;

    public Rect()
    {
      _left = 0;
      _top = 0;
      _right = 0;
      _bottom = 0;
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="left"></param>
    /// <param name="right"></param>
    /// <param name="top"></param>
    /// <param name="bottom"></param>
    public Rect(int left, int top, int right, int bottom)
    {
      _left = left;
      _top = top;
      _right = right;
      _bottom = bottom;
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="pt"></param>
    /// <param name="extent"></param>
    public Rect(Point pt, Extent extent)
    {
      _left = pt.X;
      _top = pt.Y;
      _right = (int)(_left + extent.Dx);
      _bottom = (int)(_left + extent.Dy);
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="top_left"></param>
    /// <param name="bottom_right"></param>
    public Rect(Point top_left, Point bottom_right)
    {
      _left = top_left.X;
      _top = top_left.Y;
      _right = bottom_right.X;
      _bottom = bottom_right.Y;
    }

    public int Width
    {
      get { return (int) Math.Abs(_right - _left); }
    }

    public int Height
    {
      get { return (int) Math.Abs(_bottom - _top); }
    }

    public int Left
    {
      get { return _left; }
      set { _left = value; }
    }

    public int Top
    {
      get { return _top; }
      set { _top = value; }
    }

    public int Right
    {
      get { return _right; }
      set { _right = value; }
    }

    public int Bottom
    {
      get { return _bottom; }
      set { _bottom = value; }
    }

    public Extent Extent
    {
      get
      {
        return new Extent((int) Math.Abs(_right - _left), (int) Math.Abs(_bottom - _top));
      }
    }

    public Point BottomRight
    {
      get { return new Point(_right, _bottom); }
    }

    public Point TopLeft
    {
      get { return new Point(_right, _top); }
    }
  };
}
