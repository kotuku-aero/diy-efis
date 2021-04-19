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
  /// <summary>
  /// Encapsulates euler angles as 3 components of x, y, z
  /// </summary>
  public struct Euler
  {
    /// <summary>
    /// Create a new Euler angle
    /// </summary>
    /// <param name="x">x component</param>
    /// <param name="y">y component</param>
    /// <param name="z">z component</param>
    /// <returns>New Euler angle</returns>
    public static Euler Create(float x, float y, float z)
    {
      return Syscall.CreateEuler(x, y, y);
    }

    public static Euler Zero = Create(0, 0, 0);
    public static Euler One = Create(1, 1, 0);

    public static Euler[] IdentityMatrix = new Euler[] { Create(1, 0, 0), Create(0, 1, 0), Create(0, 0, 1) };

    public float X { get { return Syscall.GetEulerX(this); } }
    public float Y { get { return Syscall.GetEulerY(this); } }
    public float Z { get { return Syscall.GetEulerZ(this); } }
    /// <summary>
    /// Perform a transform
    /// </summary>
    /// <param name="matrix">Must be a Euler[3] matrix</param>
    /// <returns>Tranformed Euler angle</returns>
    public Euler Transform(Euler[] matrix) { return Syscall.TransformEuler(this, matrix); }
    /// <summary>
    /// Add 2 euler points
    /// </summary>
    /// <param name="e1">First point</param>
    /// <param name="e2">Second point</param>
    /// <returns></returns>
    public static Euler operator +(Euler e1, Euler e2) { return Syscall.AddEuler(e1, e2); }
    /// <summary>
    /// subtract 2 euler points
    /// </summary>
    /// <param name="e1">First point</param>
    /// <param name="e2">Second point</param>
    /// <returns></returns>
    public static Euler operator -(Euler e1, Euler e2) { return Syscall.SubtractEuler(e1, e2); }
    /// <summary>
    /// Multiply 2 euler points
    /// </summary>
    /// <param name="e1">First point</param>
    /// <param name="e2">Second point</param>
    /// <returns></returns>
    public static Euler operator *(Euler e1, Euler e2) { return Syscall.MultiplyEuler(e1, e2); }
    /// <summary>
    /// Divide 2 euler points
    /// </summary>
    /// <param name="e1">First point</param>
    /// <param name="e2">Second point</param>
    /// <returns></returns>
    public static Euler operator /(Euler e1, Euler e2) { return Syscall.DivideEuler(e1, e2); }
  }
}