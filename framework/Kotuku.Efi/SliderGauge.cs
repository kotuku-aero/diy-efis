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
using CanFly;
using CanFly.Proton;

namespace Kotuku
{
  public class SliderGauge : GaugeWidget
  {
    private float _scale;
    private float _offset;
    private short _value;
    public SliderGauge(Widget parent, Rect bounds, ushort id, ushort canID, float scale, float offset)
    : base(parent, bounds, id)
    {
      _scale = scale;
      _offset = offset;
      BackgroundColor = Colors.Black;
      BorderPen = Pens.LightGrayPen;

      AddCanFlyEvent(canID, OnValue);
    }

    public float Scale
    {
      get { return _scale; }
      set { _scale = value; }
    }

    public float Offset
    {
      get { return _offset; }
      set { _offset = value; }
    }

    public short Value
    {
      get { return _value; }
      set { _value = value; }
    }

    protected override void PaintBackground(Canvas backgroundCanvas)
    {
      Rect wndRect = WindowRect;
      int numSteps = NumSteps;

      int barTop = 5;
      int barLeft = 2;

      Step first_step = GetStep(0);
      Step last_step = GetStep(numSteps - 1);

      long range = last_step.Value - first_step.Value;

      float pixels_per_unit = ((float)wndRect.Width - 16) / ((float)range);

      // so we now have the increment, just start at the bottom
      Rect drawing_rect = Rect.Create(
        wndRect.Left + 8,
        wndRect.Top + 15,
        wndRect.Left + 8,
        wndRect.Top + 20);

      for (int step = 1; step < numSteps; step++)
      {
        last_step = GetStep(step);

        float relative_value = last_step.Value - first_step.Value;
        first_step = last_step;

        int pixels = (int)(relative_value * pixels_per_unit);

        drawing_rect = drawing_rect.Add(0, 0, pixels, 0);

        backgroundCanvas.Rectangle(Pens.Hollow, last_step.Pen.Color, drawing_rect);

        drawing_rect = drawing_rect.Add(pixels, 0, 0, 0);
      }
    }

    protected override void PaintWidget()
    {
      int numSteps = NumSteps;
      Step first_step = GetStep(0);
      Step last_step = GetStep(numSteps - 1);

      int drawingWidth = WindowRect.Width - 16;
      int range = last_step.Value - first_step.Value;
      float pixels_per_unit = ((float)drawingWidth) / ((float)range);

      float min_range = first_step.Value;

      float value = _value;
      value = (float)Math.Max(value, min_range);
      value = (float)Math.Min(value, min_range + range);

      float relative_value = value - min_range;
      int position = (int)(-(relative_value * pixels_per_unit) + 8);

      position = Math.Max(position, 2);

        Polygon(Pens.Hollow, Colors.White,
          Point.Create(position + 2, 12),
          Point.Create(position, 19),
          Point.Create(position -2, 12),
          Point.Create(position + 2, 12));
    }

    private void OnValue(CanFlyMsg msg)
    {
      short value = msg.GetInt16();

      float scaledValue = value * _scale;
      scaledValue += _offset;

      value = (short)scaledValue;

      if (_value != value)
      {
        _value = value;
        InvalidateRect();
      }

    }
  }
}