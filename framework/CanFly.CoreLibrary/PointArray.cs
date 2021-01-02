using System;
using System.Collections;
using System.Runtime.CompilerServices;

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
    public PointArray(uint numPoints = 0)
    {
      _handle = PointArrayCreate(numPoints);
    }
    /// <summary>
    /// Create a point array from a list of points
    /// </summary>
    /// <param name="points"></param>
    public PointArray(params Point[] points)
    {
      if (points == null)
        throw new ArgumentNullException();

      uint len = (uint)points.Length;
      _handle = PointArrayCreate(len);
      for (uint i = 0; i < len; i++)
        this[i] = points[i];
    }

    internal uint Handle
    {
      get { return _handle; }
    }

    public Point this[uint index]
    {
      get
      {
        int x;
        int y;

        PointArrayGetPoint(_handle, index, out x, out y);

        return new Point(x, y);
      }
      set
      {
        Point pt = (Point)value;
        PointArraySetPoint(_handle, index, pt.X, pt.Y);
      }
    }

    public uint Count
    {
      get { return PointArraySize(_handle); }
      set { PointArrayResize(_handle, value); }
    }

    public uint Add(Point value)
    {
      return PointArrayAppend(_handle, value.X, value.Y);
    }

    public void Clear()
    {
      PointArrayClear(_handle);
    }

    public bool Contains(Point value)
    {
      return PointArrayIndexOf(_handle, value.X, value.Y) >= 0;
    }
    public void Dispose()
    {
      PointArrayRelease(_handle);
      _handle = 0;
    }

    private class PointEnumerator : IEnumerator
    {
      private PointArray _outer;
      private uint _index;

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

    public uint IndexOf(Point value)
    {
      return PointArrayIndexOf(_handle, value.X, value.Y);
    }

    public void Insert(uint index, Point value)
    {
      PointArrayInsertAt(_handle, index, value.X, value.Y);
    }

    public void Remove(Point value)
    {
      uint index = PointArrayIndexOf(_handle, value.X, value.Y);

      if (index >= 0)
        PointArrayRemoveAt(_handle, index);
    }

    public void RemoveAt(uint index)
    {
      PointArrayRemoveAt(_handle, index);
    }

    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint PointArrayCreate(uint numPoints);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArrayRelease(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArraySetPoint(uint handle, uint index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArrayClear(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint PointArraySize(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArrayResize(uint handle, uint size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint PointArrayAppend(uint handle, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArrayInsertAt(uint handle, uint index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArrayGetPoint(uint handle, uint index, out int x, out int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern uint PointArrayIndexOf(uint handle, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    private static extern void PointArrayRemoveAt(uint handle, uint index);
  }
}
