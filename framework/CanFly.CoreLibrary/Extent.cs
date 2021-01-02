using System;

namespace CanFly
{
  public class Extent
  {
    private int _dx;
    private int _dy;

    public Extent()
    {
      _dx = 0;
      _dy = 0;
    }

    public Extent(int dx, int dy)
    {
      _dx = dx;
      _dy = dy;
    }

    public int Dx
    {
      get { return _dx; }
      set { _dx = value; }
    }

    public int Dy
    {
      get { return _dy; }
      set { _dy = value; }
    }
  };
}
