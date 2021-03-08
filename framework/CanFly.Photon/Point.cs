using System;

namespace CanFly
{
  // we store values as an int as the compiler naturally promotes maths
  // to an int, however the API uses shorts.  That is handled with the api call's
  public class Point
  {
    private int _x;
    private int _y;

    public Point()
    {
      X = 0;
      Y = 0;
    }

    public Point(int x, int y)
    {
      X = x;
      Y = y;
    }

    public int X
    {
      get { return _x; }
      set { _x = value; }
    }

    public int Y
    {
      get { return _y; }
      set { _y = value; }
    }
  };
}
