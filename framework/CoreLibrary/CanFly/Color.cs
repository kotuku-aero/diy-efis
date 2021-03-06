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
  public struct Color
  {
    /// <summary>
    /// Create a color
    /// </summary>
    /// <param name="red">Red intensity</param>
    /// <param name="green">Green inensity</param>
    /// <param name="blue">Blue intensity</param>
    /// <param name="alpha">Saturation</param>
    /// <returns>New Color</returns>
    public static Color Create(byte red, byte green, byte blue, byte alpha)
    {
      return Syscall.CreateColor(red, green, blue, alpha);
    }
    /// <summary>
    /// Create a color, with 100% saturation
    /// </summary>
    /// <param name="red">Red intensity</param>
    /// <param name="green">Green inensity</param>
    /// <param name="blue">Blue intensity</param>
    /// <returns>New Color</returns>
    public static Color Create(byte red, byte green, byte blue) { return Syscall.CreateColor(red, green, blue, 0xff); }
    /// <summary>
    /// Create a color from an ecoded rgba value
    /// </summary>
    /// <param name="encoded">RGBA encoded value</param>
    /// <returns>New color</returns>
    public static Color Create(uint encoded) { return Syscall.CreateEncodedColor(encoded); }

    public byte Red { get { return Syscall.GetRed(this); } }
    public byte Green { get { return Syscall.GetGreen(this); } }
    public byte Blue { get { return Syscall.GetBlue(this); } }
    public byte Alpha { get { return Syscall.GetAlpha(this); } }
  }
}