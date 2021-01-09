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
      _right = _left + extent.Dx;
      _bottom = _left + extent.Dy;
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
      get { return Math.Abs(_right - _left); }
    }

    public int Height
    {
      get { return Math.Abs(_bottom - _top); }
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
        return new Extent(Math.Abs(_right - _left), Math.Abs(_bottom - _top));
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
