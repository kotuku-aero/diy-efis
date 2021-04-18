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
  public struct Colors
  {
    public static uint FromRGBA(byte r, byte g, byte b, byte a)
    {
      return ((uint)((((uint)(r)) << 24) | (((uint)(g)) << 16) | (((uint)(b)) << 8) | ((uint)(a))));
    }

    public static uint FromRGB(byte r, byte g, byte b)
    {
      return FromRGBA(r, g, b, 0xff);
    }
    public static readonly uint White = FromRGB(255, 255, 255);
    public static readonly uint Black = FromRGB(0, 0, 0);
    public static readonly uint Gray = FromRGB(128, 128, 128);
    public static readonly uint LightGray = FromRGB(192, 192, 192);
    public static readonly uint DarkGray = FromRGB(64, 64, 64);
    public static readonly uint Red = FromRGB(255, 0, 0);
    public static readonly uint Pink = FromRGB(255, 128, 128);
    public static readonly uint Blue = FromRGB(0, 0, 255);
    public static readonly uint Green = FromRGB(0, 255, 0);
    public static readonly uint LightGreen = FromRGB(0, 192, 0);
    public static readonly uint Yellow = FromRGB(255, 255, 64);
    public static readonly uint Magenta = FromRGB(255, 0, 255);
    public static readonly uint Cyan = FromRGB(0, 255, 255);
    public static readonly uint PaleYellow = FromRGB(255, 255, 208);
    public static readonly uint LightYellow = FromRGB(255, 255, 128);
    public static readonly uint LimeGreen = FromRGB(192, 220, 192);
    public static readonly uint Teal = FromRGB(64, 128, 128);
    public static readonly uint DarkGreen = FromRGB(0, 128, 0);
    public static readonly uint Maroon = FromRGB(128, 0, 0);
    public static readonly uint Purple = FromRGB(128, 0, 128);
    public static readonly uint Orange = FromRGB(255, 192, 64);
    public static readonly uint Khaki = FromRGB(167, 151, 107);
    public static readonly uint Olive = FromRGB(128, 128, 0);
    public static readonly uint Brown = FromRGB(192, 128, 32);
    public static readonly uint Navy = FromRGB(0, 64, 128);
    public static readonly uint LightBlue = FromRGB(128, 128, 255);
    public static readonly uint FadedBlue = FromRGB(192, 192, 255);
    public static readonly uint LightGrey = FromRGB(192, 192, 192);
    public static readonly uint DarkGrey = FromRGB(64, 64, 64);
    public static readonly uint Hollow = FromRGBA(0, 0, 0, 0);
  }
}
