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
using CanFly;

namespace CanFly.Proton
{
  public class Pens
  {
    public static readonly Pen BlackPen = Pen.Create(Colors.Black, 1, PenStyle.Solid);
    public static readonly Pen GrayPen = Pen.Create(Colors.Gray, 1, PenStyle.Solid);
    public static readonly Pen FadedBluePen = Pen.Create(Colors.FadedBlue, 1, PenStyle.Solid);
    public static readonly Pen BrownPen = Pen.Create(Colors.Brown, 1, PenStyle.Solid);
    public static readonly Pen RedPen = Pen.Create(Colors.Red, 1, PenStyle.Solid);
    public static readonly Pen LightRedPen = Pen.Create(Colors.LightRed, 1, PenStyle.Solid);
    public static readonly Pen GreenPen = Pen.Create(Colors.Green, 1, PenStyle.Solid);
    public static readonly Pen LightGreenPen = Pen.Create(Colors.LightGreen, 1, PenStyle.Solid);
    public static readonly Pen OrangePen = Pen.Create(Colors.Orange, 1, PenStyle.Solid);
    public static readonly Pen LightbluePen = Pen.Create(Colors.LightBlue, 1, PenStyle.Solid);
    public static readonly Pen MagentaPen = Pen.Create(Colors.Magenta, 1, PenStyle.Solid);
    public static readonly Pen BluePen = Pen.Create(Colors.Blue, 1, PenStyle.Solid);
    public static readonly Pen YellowPen = Pen.Create(Colors.Yellow, 1, PenStyle.Solid);
    public static readonly Pen WhitePen = Pen.Create(Colors.White, 1, PenStyle.Solid);
    public static readonly Pen LightGrayPen = Pen.Create(Colors.LightGray, 1, PenStyle.Solid);
    public static readonly Pen LightGrayPen3 = Pen.Create(Colors.LightGray, 3, PenStyle.Solid);
    public static readonly Pen WhitePen2 = Pen.Create(Colors.White, 2, PenStyle.Solid);
    public static readonly Pen BlackPen2 = Pen.Create(Colors.Black, 2, PenStyle.Solid);
    public static readonly Pen WhitePen3 = Pen.Create(Colors.White, 3, PenStyle.Solid);
    public static readonly Pen GreenPen3 = Pen.Create(Colors.Green, 3, PenStyle.Solid);
    public static readonly Pen RedPen3 = Pen.Create(Colors.Red, 3, PenStyle.Solid);
    public static readonly Pen YellowPen3 = Pen.Create(Colors.Yellow, 3, PenStyle.Solid);
    public static readonly Pen LightbluePen3 = Pen.Create(Colors.LightBlue, 3, PenStyle.Solid);
    public static readonly Pen OrangePen3 = Pen.Create(Colors.Orange, 3, PenStyle.Solid);
    public static readonly Pen BlackPen3 = Pen.Create(Colors.Black, 3, PenStyle.Solid);
    public static readonly Pen LightbluePen5 = Pen.Create(Colors.LightBlue, 5, PenStyle.Solid);
    public static readonly Pen RedPen5 = Pen.Create(Colors.Red, 5, PenStyle.Solid);
    public static readonly Pen LightRedPen5 = Pen.Create(Colors.LightRed, 5, PenStyle.Solid);
    public static readonly Pen LightGreenPen5 = Pen.Create(Colors.LightGreen, 5, PenStyle.Solid);
    public static readonly Pen OrangePen5 = Pen.Create(Colors.Orange, 5, PenStyle.Solid);
    public static readonly Pen WhitePen5 = Pen.Create(Colors.White, 5, PenStyle.Solid);
    public static readonly Pen GreenPen5 = Pen.Create(Colors.Green, 5, PenStyle.Solid);
    public static readonly Pen Hollow = Pen.Create(Colors.Hollow, 0, PenStyle.Null);
  }
}
