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
    public PointArray()
    {
      Syscall.PointArrayCreate(0, out _handle);
    }
     public PointArray(ushort numPoints)
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
