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