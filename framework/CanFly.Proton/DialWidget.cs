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

namespace CanFly.Proton
{
  public class DialWidget : GaugeWidget
  {
    public enum Style
    {
      Pointer,         // a simple line
      Sweep,           // a pie sweep
      Bar,             // line sweep
      Point,           // simple-pointer
      PointerMinMax,  // a pointer with min-max markers
      PointMinMax,    // simple-pointer with min-max markers
    };

    private Point _center;
    private int _gaugeRadii;

    private ushort _arcBegin;
    private ushort _arcRange;
    private float _resetValue;

    private Style _style;
    private ushort _width;          // pointer or sweep width

    private float _scale;
    private float _offset;
    private float _value;
    private float _minValue;
    private float _maxValue;
    /// <summary>
    /// Construct a gauge widget with no settings
    /// </summary>
    /// <param name="parent">Parent widget</param>
    /// <param name="bounds">Position of the widget relative to the parent</param>
    /// <param name="id">Id of the widget</param>
    public DialWidget(Widget parent, Rect bounds, ushort id, ushort resetLabel, ushort label)
      : base(parent, bounds, id)
    {
      ValueAlign = ValueOutStyle.RightAlign;
      ValueBoxVisible = true;
      if (resetLabel != 0)
        AddCanFlyEvent(resetLabel, OnResetLabel);

      if (label != 0)
        AddCanFlyEvent(label, OnValueLabel);
    }
    /// <summary>
    /// Called after all of the gauge properties are set.
    /// </summary>
    /// <param name="resetLabel"></param>
    /// <param name="labels"></param>
    public override void Initialize()
    {
      base.Initialize();

      _value = ResetValue;

      InvalidateRect();
    }

    private void OnValueLabel(CanFlyMsg msg)
    {
      try
      {
        float float_value = msg.GetFloat();

        float_value *= Scale;
        float_value += Offset;

        if (_value != float_value)
        {
          _value = float_value;

          _minValue = (float)Math.Min(_minValue, _value);
          _maxValue = (float)Math.Max(_maxValue, _value);

          InvalidateRect();
        }
      }
      catch
      {

      }
    }
    /// <summary>
    /// Called when a reset event is received.
    /// </summary>
    /// <param name="e"></param>
    private void OnResetLabel(CanFlyMsg e)
    {
      _minValue = ResetValue;
      _maxValue = ResetValue;

      InvalidateRect();
    }
    /// <summary>
    /// Draw the background canvas
    /// </summary>
    /// <param name="backgroundCanvas"></param>
    protected override void PaintBackground(Canvas backgroundCanvas)
    {
      Rect wnd_rect = WindowRect;

      short min_range = 0;
      short max_range = 0;
      float degrees_per_unit = 1.0f;

      Step[] steps = Steps;

      // we now draw an arc.  The range is the last value in the arc
      // minus the first value.  Note that the values can be -
      // the range of the gauge is gauge_y - 360 degrees
      if (steps != null)
      {
        int num_steps = steps.Length;

        Step first_step = steps[0];
        Step last_step = steps[num_steps - 1];

        min_range = first_step.Value;
        max_range = last_step.Value;

        int steps_range = max_range - min_range;

        degrees_per_unit = ((float)_arcRange) / steps_range;

        float arc_start = ArcBegin;
        ushort i;

        //    font(_font);
        //    background_color(Colors.Black);
        //    text_color(Colors.White);
        //
        for (i = 1; i < num_steps; i++)
        {
          last_step = steps[i];

          // the arc angle is the step end
          float arc_end = (last_step.Value - min_range) * degrees_per_unit;
          arc_end += ArcBegin;

          // the arc starts a point(0, -1) which is 0 degrees
          // so ranges from 0..270 are 90.360 and needs to
          // be split to draw 270..360
          // draw arc
          int[] arc_angles = { (int)arc_start, (int)arc_end, -1 };

          if (arc_angles[0] > 360)
          {
            arc_angles[0] -= 360;
            arc_angles[1] -= 360;
          }
          else if (arc_angles[1] > 360)
          {
            arc_angles[2] = arc_angles[1] - 360;
            arc_angles[1] = 360;
          }

          CanFly.Pen pen = last_step.Pen;

          backgroundCanvas.Arc(pen, Center, GaugeRadii, arc_angles[0], arc_angles[1]);

          if (arc_angles[2] >= 0)
            backgroundCanvas.Arc(last_step.Pen, Center, GaugeRadii, 0, arc_angles[2]);

          arc_start = arc_end;
        }
      }

      TickMark[] ticks = Ticks;
      if (ticks != null)
      {
        int num_ticks = ticks.Length;

        // we now draw the tick marks.  They are a line from the point 5 pixels long.
        // we create a white pen for this
        //pen(&white_pen_2);

        int line_start = GaugeRadii - 5;
        // end of the mark is the edge of the gauge
        int line_end = GaugeRadii + (Width >> 1);
        float arc_start;

        for (int i = 0; i < num_ticks; i++)
        {
          TickMark tick = ticks[i];

          arc_start = (tick.Value - min_range) * degrees_per_unit;
          arc_start += ArcBegin;
          // make it point to correct spot....

          backgroundCanvas.Polyline(Pens.WhitePen,
            RotatePoint(Center, Point.Create(Center.X + line_start, Center.Y), (int)arc_start),
          RotatePoint(Center, Point.Create(Center.X + line_end, Center.Y), (int)arc_start));

          if (tick.Text != null)
          {
            // write the text at the point
            Extent size = backgroundCanvas.TextExtent(Font, tick.Text);

            // the text is below the tick marks
            Point top_left = RotatePoint(Center, Point.Create(Center.X, Center.Y - line_end + (size.Dy >> 1)), (int)arc_start);

            top_left.Add(-(size.Dx >> 1), -(size.Dy >> 1));

            backgroundCanvas.DrawText(Font, Colors.White, BackgroundColor, tick.Text, top_left);
          }
        }
      }
    }
    public virtual float Value
    {
      get { return _value; }
    }

    public virtual float MinValue
    {
      get { return _minValue; }
    }

    public virtual float MaxValue
    {
      get { return _maxValue; }
    }

    protected override void PaintWidget()
    {
      // we now calculate the range of the segments.  There are 8 segments
      // around a 240 degree arc.  We draw the nearest based on the range
      // of the gauge.
      int rotation;

      float value = Value;

      Step valueStep = CalculateStep(value);

      switch (_style)
      {
        case Style.PointerMinMax:
          {
            float maxValue = MaxValue;
            float minValue = MinValue;

            DrawPoint(CalculateStep(minValue), false, CalculateRotation(minValue));
            DrawPoint(CalculateStep(maxValue), false, CalculateRotation(maxValue));
          }
          break;
        case Style.Pointer:
          rotation = CalculateRotation(value);
          Polyline(valueStep.IndicatorPen,
            RotatePoint(Center, Point.Create(Center.X, Center.Y - 5), rotation),
            RotatePoint(Center, Point.Create(Center.X, Center.Y - GaugeRadii + 5), rotation));
          break;
        case Style.Sweep:
          Pie(valueStep.IndicatorPen,
            valueStep.IndicatorColor, Center, ArcBegin + 90,
            CalculateRotation(value), Center.Y - GaugeRadii + 5, 5);
          break;
        case Style.Bar:
          Pen outlinePen = valueStep.IndicatorPen;
          rotation = CalculateRotation(value);

          int[] arc_angles = new int[3] { _arcBegin, rotation, -1 };

          if (arc_angles[0] > 360)
          {
            arc_angles[0] -= 360;
            arc_angles[1] -= 360;
          }
          else if (arc_angles[1] > 360)
          {
            arc_angles[2] = arc_angles[1] - 360;
            arc_angles[1] = 360;
          }

          int offset = _width >> 1;
          int radii = _gaugeRadii - offset;
          radii -= outlinePen.Width >> 1;

          if (arc_angles[0] == arc_angles[1] && arc_angles[2] == -1)
          {
            Point pt = RotatePoint(Center, Point.Create(Center.X + radii, Center.Y), arc_angles[0]);
            // draw a dot
            Rect ellipse = Rect.Create(pt.X - offset, pt.Y - offset, pt.X + offset, pt.Y + offset);

            Ellipse(Pens.Hollow, outlinePen.Color, ellipse);
          }
          else
          {
            Arc(outlinePen, Center, radii, arc_angles[0], arc_angles[1]);

            if (arc_angles[2] > 0)
              Arc(outlinePen, Center, radii, 0, arc_angles[2]);
          }
          break;
        case Style.PointMinMax:
          {
            float maxValue = MaxValue;
            float minValue = MinValue;

            DrawPoint(CalculateStep(minValue), false, CalculateRotation(minValue));
            DrawPoint(CalculateStep(maxValue), false, CalculateRotation(maxValue));
            DrawPoint(valueStep, true, CalculateRotation(value));
          }
          break;
        case Style.Point:
          DrawPoint(valueStep, true, CalculateRotation(value));
          break;
      }
    }

    private void DrawPoint(Step selectedStep, bool fillPoint, int rotation)
    {
      Polygon(selectedStep.IndicatorPen,
        fillPoint ? selectedStep.IndicatorColor : Colors.Hollow,
        RotatePoint(_center, Point.Create(_center.X + _gaugeRadii, _center.Y), rotation),
        RotatePoint(_center, Point.Create(_center.X + _gaugeRadii - 7, _center.Y - 4), rotation),
        RotatePoint(_center, Point.Create(_center.X + _gaugeRadii - 7, _center.Y + 4), rotation),
        RotatePoint(_center, Point.Create(_center.X + _gaugeRadii, _center.Y), rotation));
    }

    private int CalculateRotation(double value)
    {
      int numSteps = NumSteps;
      if (numSteps < 2)
        return 0;

      Step first_step = GetStep(0);
      Step last_step = GetStep(numSteps - 1);

      double min_range = first_step.Value;
      double max_range = last_step.Value;

      // get the percent that the gauge is displaying
      double percent = (Math.Max(Math.Min(value, max_range), min_range) - min_range) / (max_range - min_range);

      return (int)(_arcBegin + (_arcRange * percent));
    }

    public Point Center
    {
      get { return _center; }
      set { _center = value; }
    }

    public int GaugeRadii
    {
      get { return _gaugeRadii; }
      set { _gaugeRadii = value; }
    }

    public ushort ArcBegin
    {
      get { return _arcBegin; }
      set { _arcBegin = value; }
    }

    public ushort ArcRange
    {
      get { return _arcRange; }
      set { _arcRange = value; }
    }

    public float ResetValue
    {
      get { return _resetValue; }
      set { _resetValue = value; }
    }

    public Style DrawStyle
    {
      get { return _style; }
      set { _style = value; }
    }

    public ushort Width
    {
      get { return _width; }
      set { _width = value; }
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
  }
}
