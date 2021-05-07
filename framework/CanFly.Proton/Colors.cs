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

namespace CanFly.Proton
{
  public struct Colors {
    public static readonly Color White = Color.Create(255, 255, 255);
    public static readonly Color Black = Color.Create(0, 0, 0);
    public static readonly Color Gray = Color.Create(128, 128, 128);
    public static readonly Color LightGray = Color.Create(192, 192, 192);
    public static readonly Color DarkGray = Color.Create(64, 64, 64);
    public static readonly Color Red = Color.Create(255, 0, 0);
    public static readonly Color LightRed = Color.Create(128, 0, 0);
    public static readonly Color Pink = Color.Create(255, 128, 128);
    public static readonly Color Blue = Color.Create(0, 0, 255);
    public static readonly Color Green = Color.Create(0, 255, 0);
    public static readonly Color LightGreen = Color.Create(0, 192, 0);
    public static readonly Color Yellow = Color.Create(255, 255, 64);
    public static readonly Color Magenta = Color.Create(255, 0, 255);
    public static readonly Color Cyan = Color.Create(0, 255, 255);
    public static readonly Color PaleYellow = Color.Create(255, 255, 208);
    public static readonly Color LightYellow = Color.Create(255, 255, 128);
    public static readonly Color LimeGreen = Color.Create(192, 220, 192);
    public static readonly Color Teal = Color.Create(64, 128, 128);
    public static readonly Color DarkGreen = Color.Create(0, 128, 0);
    public static readonly Color Maroon = Color.Create(128, 0, 0);
    public static readonly Color Purple = Color.Create(128, 0, 128);
    public static readonly Color Orange = Color.Create(255, 192, 64);
    public static readonly Color Khaki = Color.Create(167, 151, 107);
    public static readonly Color Olive = Color.Create(128, 128, 0);
    public static readonly Color Brown = Color.Create(192, 128, 32);
    public static readonly Color Navy = Color.Create(0, 64, 128);
    public static readonly Color LightBlue = Color.Create(128, 128, 255);
    public static readonly Color FadedBlue = Color.Create(192, 192, 255);
    public static readonly Color LightGrey = Color.Create(192, 192, 192);
    public static readonly Color DarkGrey = Color.Create(64, 64, 64);
    public static readonly Color Hollow = Color.Create(0, 0, 0, 0);

  }
}