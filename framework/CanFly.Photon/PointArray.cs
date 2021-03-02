using System;
using System.Collections;

namespace CanFly
{
  /// <summary>
  /// Wrapper around an Atom array of points
  /// </summary>
  public class PointArray : IDisposable, IEnumerable
  {
    private uint _handle;
    /// <summary>
    /// Create a point array, will be initialzied with Point(0, 0)
    /// </summary>
    /// <param name="numPoints"></param>
    public PointArray(ushort numPoints = 0)
    {
      Syscall.PointArrayCreate(numPoints, out _handle);
    }
    /// <summary>
    /// Create a point array from a list of points
    /// </summary>
    /// <param name="points"></param>
    public PointArray(params Point[] points)
    {
      if (points == null)
        throw new ArgumentNullException();

      ushort len = (ushort)points.Length;
      Syscall.PointArrayCreate(len, out _handle);
      for (ushort i = 0; i < len; i++)
        this[i] = points[i];
    }

    internal uint Handle
    {
      get { return _handle; }
    }

    public Point this[ushort index]
    {
      get
      {
        int x;
        int y;

        Syscall.PointArrayGetPoint(_handle, index, out x, out y);

        return new Point(x, y);
      }
      set
      {
        Point pt = (Point)value;
        Syscall.PointArraySetPoint(_handle, index, pt.X, pt.Y);
      }
    }

    public ushort Count
    {
      get 
      {
        ushort value;
        Syscall.PointArraySize(_handle, out value);
        return value;
      }
      set { Syscall.PointArrayResize(_handle, value); }
    }

    public uint Add(Point value)
    {
      ushort size;
      Syscall.PointArrayAppend(_handle, value.X, value.Y, out size);

      return size;
    }

    public void Clear()
    {
      Syscall.PointArrayClear(_handle);
    }

    public bool Contains(Point value)
    {
      int index;
      Syscall.PointArrayIndexOf(_handle, value.X, value.Y, out index);

      return index >= 0;
    }
    public void Dispose()
    {
      Syscall.PointArrayRelease(_handle);
      _handle = 0;
    }

    private class PointEnumerator : IEnumerator
    {
      private PointArray _outer;
      private ushort _index;

      public PointEnumerator(PointArray outer)
      {
        _outer = outer;
        _index = 0;
      }

      public object Current
      {
        get { return _outer[_index]; }
      }

      public bool MoveNext()
      {
        if (_index >= _outer.Count)
          return false;

        _index++;
        return true;
      }

      public void Reset()
      {
        _index = 0;
      }
    }

    public IEnumerator GetEnumerator()
    {
      return new PointEnumerator(this);
    }

    public int IndexOf(Point value)
    {
      int index;
      Syscall.PointArrayIndexOf(_handle, value.X, value.Y, out index);

      return index;
    }

    public void Insert(ushort index, Point value)
    {
      Syscall.PointArrayInsertAt(_handle, index, value.X, value.Y);
    }

    public void Remove(Point value)
    {
      int index;
      Syscall.PointArrayIndexOf(_handle, value.X, value.Y, out index);

      if (index >= 0)
        Syscall.PointArrayRemoveAt(_handle, (ushort) index);
    }

    public void RemoveAt(ushort index)
    {
      Syscall.PointArrayRemoveAt(_handle, index);
    }
  }
}
