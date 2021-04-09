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
    public static readonly Pen BlackPen = new Pen(Colors.Black, 1, PenStyle.Solid);
    public static readonly Pen GrayPen = new Pen(Colors.Gray, 1, PenStyle.Solid);
    public static readonly Pen FadedBluePen = new Pen(Colors.FadedBlue, 1, PenStyle.Solid);
    public static readonly Pen BrownPen = new Pen(Colors.Brown, 1, PenStyle.Solid);
    public static readonly Pen RedPen = new Pen(Colors.Red, 1, PenStyle.Solid);
    public static readonly Pen GreenPen = new Pen(Colors.Green, 1, PenStyle.Solid);
    public static readonly Pen OrangePen = new Pen(Colors.Orange, 1, PenStyle.Solid);
    public static readonly Pen LightbluePen = new Pen(Colors.LightBlue, 1, PenStyle.Solid);
    public static readonly Pen MagentaPen = new Pen(Colors.Magenta, 1, PenStyle.Solid);
    public static readonly Pen BluePen = new Pen(Colors.Blue, 1, PenStyle.Solid);
    public static readonly Pen YellowPen = new Pen(Colors.Yellow, 1, PenStyle.Solid);
    public static readonly Pen WhitePen = new Pen(Colors.White, 1, PenStyle.Solid);
    public static readonly Pen LightGrayPen = new Pen(Colors.LightGray, 1, PenStyle.Solid);
    public static readonly Pen LightGrayPen3 = new Pen(Colors.LightGray, 3, PenStyle.Solid);
    public static readonly Pen WhitePen2 = new Pen(Colors.White, 2, PenStyle.Solid);
    public static readonly Pen WhitePen3 = new Pen(Colors.White, 3, PenStyle.Solid);
    public static readonly Pen GreenPen3 = new Pen(Colors.Green, 3, PenStyle.Solid);
    public static readonly Pen RedPen3 = new Pen(Colors.Red, 3, PenStyle.Solid);
    public static readonly Pen YellowPen3 = new Pen(Colors.Yellow, 3, PenStyle.Solid);
    public static readonly Pen LightbluePen3 = new Pen(Colors.LightBlue, 3, PenStyle.Solid);
    public static readonly Pen OrangePen3 = new Pen(Colors.Orange, 3, PenStyle.Solid);
    public static readonly Pen LightbluePen5 = new Pen(Colors.LightBlue, 5, PenStyle.Solid);
    public static readonly Pen RedPen5 = new Pen(Colors.Red, 5, PenStyle.Solid);
    public static readonly Pen OrangePen5 = new Pen(Colors.Orange, 5, PenStyle.Solid);
    public static readonly Pen WhitePen5 = new Pen(Colors.White, 5, PenStyle.Solid);
    public static readonly Pen GreenPen5 = new Pen(Colors.Green, 5, PenStyle.Solid);
  }
}
