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
using System.Collections;

namespace CanFly.Proton
{
  public class VerticalBarWidget : GaugeWidget
  {
    public enum Style
    {
      Point, // simple-pointer
      PointerMinMax, // a pointer with min-max markers
      PointerMax, // simple-pointer with max markers
      PointerMin,
    };

    private ushort _numValues;

    private ushort _resetLabel;

    private Point _center;
    private int _gaugeRadii;

    private ushort _arcBegin;
    private ushort _arcRange;
    private float _resetValue;

    private Style _style;
    private ushort _width;          // pointer or sweep width

    private bool _drawValue;         // draw the value
    private bool _valueBoxVisible;
    private ValueOutStyle _valueAlign;    // draw the value centered in the box, otherwise right
    private Font _valueFont;       // what font to draw a value in
    private Rect _valueRect;

    private ArrayList _steps;
    private ArrayList _ticks;

    private float _scale;
    private float _offset;

    private ushort[] _labels;
    // a gauge supports up to 6 values
    // this is updated on each window message
    private float[] _values;
    private float[] _minValues;
    private float[] _maxValues;
    /// <summary>
    /// Construct a gauge widget with no settings
    /// </summary>
    /// <param name="parent">Parent widget</param>
    /// <param name="bounds">Position of the widget relative to the parent</param>
    /// <param name="id">Id of the widget</param>
    public VerticalBarWidget(Widget parent, Rect bounds, ushort id, ushort resetLabel, params ushort[] labels)
      : base(parent, bounds, id)
    {
      _valueAlign = ValueOutStyle.RightAlign;
      _valueBoxVisible = true;

      _numValues = (ushort)labels.Length;
      _labels = labels;

      _values = new float[_numValues];
      _minValues = new float[_numValues];
      _maxValues = new float[_numValues];
    }

    /// <summary>
    /// Called after all of the gauge properties are set.
    /// </summary>
    /// <param name="resetLabel"></param>
    /// <param name="labels"></param>
    public override void Initialize()
    {
      base.Initialize();

      float resetValue = ResetValue;
      for (ushort i = 0; i < NumValues; i++)
      {
        _values[i] = resetValue;
        _minValues[i] = resetValue;
        _maxValues[i] = resetValue;
      }

      if (_resetLabel != 0)
        AddCanFlyEvent(_resetLabel, OnResetLabel);

      for (int i = 0; i < NumValues; i++)
        AddCanFlyEvent(Labels[i], OnValueLabel);

      InvalidateRect();
    }

    private void OnValueLabel(CanFlyMsg msg)
    {
      try
      {
        float float_value = msg.GetFloat();

        for (int i = 0; i < NumValues; i++)
        {
          if (msg.CanID == Labels[i])
          {
            float_value *= Scale;
            float_value += Offset;

            if (_values[i] != float_value)
            {
              _values[i] = float_value;

              _minValues[i] = (float)Math.Min(_minValues[i], _values[i]);
              _maxValues[i] = (float)Math.Max(_maxValues[i], _values[i]);

              InvalidateRect();
            }
            break;
          }
        }
      }
      catch
      {

      }
    }

    private void OnResetLabel(CanFlyMsg e)
    {
      for (int i = 0; i < _numValues; i++)
      {
        _minValues[i] = ResetValue;
        _maxValues[i] = ResetValue;
      }

      InvalidateRect();
    }

    // each bar is drawn with the following dimensions:
    //
    //  *****************
    //                5 pixels
    //        ********
    //        *      *
    //        *      *
    //   +    *      *
    //   |\   *      *
    //   | \  *      *
    //   |  * *      *
    //   | /  *      *
    //   |/   *      *
    //   +    *      *
    //        ********
    //                5 pixels
    //  *****************
    //  ^ ^  ^    ^    ^
    //  | |  |    |    +- 2 Pixels
    //  | |  |    +------ 6 Pixels
    //  | |  +----------- 1 Pixel
    //  | +-------------- 5 Pixels
    //  +---------------- 1 Pixel
    //
    //

    protected override void PaintBackground(Canvas backgroundCanvas)
    {
      int numSteps = NumSteps;
      if (numSteps < 2)
        return;

      Rect wnd_rect = WindowRect;
      Rect graph = Rect.Create(wnd_rect.Left, wnd_rect.Top, wnd_rect.Left + 14, wnd_rect.Height);

      for (uint i = 0; i < _numValues; i++)
      {
        Step first_step = GetStep(0);
        Step last_step = GetStep(numSteps - 1);

        long range = last_step.Value - first_step.Value;

        float pixels_per_unit = ((float)graph.Height - 12) / ((float)range);

        // so we now have the increment, just start at the bottom
        Rect drawing_rect = Rect.Create(
          graph.Left + 8,
          graph.Top + 3,
          graph.Right - 2,
          graph.Bottom - 7);

        for (int step = 1; step < numSteps; step++)
        {
          last_step = GetStep(step);

          float relative_value = last_step.Value - first_step.Value;
          first_step = last_step;

          float pixels = relative_value * pixels_per_unit;

          drawing_rect = drawing_rect.Add(0, 0, 0, (int)-pixels);

          backgroundCanvas.Rectangle(Pens.Hollow, last_step.Pen.Color, drawing_rect);

          drawing_rect = Rect.Create(drawing_rect.Top, drawing_rect.Top,
            drawing_rect.Right, drawing_rect.Top);
        }

        graph = graph.Add(14, 0, 14, 0);
      }

      int height = graph.Height - 12;

      int numTicks = NumTicks;

      if (numTicks > 0)
      {
        TickMark first_tick = GetTick(0);
        TickMark last_tick = GetTick(numTicks - 1);

        int range = last_tick.Value - first_tick.Value;
        float pixels_per_unit = ((float)height) / ((float)range);

        for (int i = 0; i < numTicks; i++)
        {
          last_tick = GetTick(i);

          if (last_tick.Text == null)
            continue;

          short value = last_tick.Value;

          Extent sz = backgroundCanvas.TextExtent(Font, last_tick.Text);

          float relative_value = value - first_tick.Value;
          float pixels = relative_value * pixels_per_unit;
          Point pt = Point.Create(graph.Left + 2, (graph.Bottom - (int)pixels) - 7 - (sz.Dy >> 1));

          backgroundCanvas.DrawText(Font, NameColor, BackgroundColor, last_tick.Text, pt);
        }
      }
    }

    protected override void PaintWidget()
    {
      int numSteps = NumSteps;
      int offset = 1;

      for (int i = 0; i < _numValues; i++)
      {
        Step first_step = GetStep(0);
        Step last_step = GetStep(numSteps - 1);

        int drawingHeight = WindowRect.Height - 12;
        int range = last_step.Value - first_step.Value;
        float pixels_per_unit = ((float)drawingHeight) / ((float)range);

        float min_range = first_step.Value;

        float value = _values[i];
        value = (float)Math.Max(value, min_range);
        value = (float)Math.Min(value, min_range + range);

        float relative_value = value - min_range;
        int position = (int)(-(relative_value * pixels_per_unit) - 8);

        position = Math.Max(position, 5);

        Polygon(Pens.LightbluePen, Colors.LightBlue,
          Point.Create(offset, position + 5),
          Point.Create(offset + 5, position),
          Point.Create(offset, position - 5),
          Point.Create(offset, position + 5));

        if (_style == Style.PointerMinMax || _style == Style.PointerMax)
        {
          relative_value = _maxValues[i] - min_range;
          if (relative_value >= 0.0)
          {
            position = (int)(drawingHeight - (relative_value * pixels_per_unit) - 8);
            position = Math.Max(position, 5);

            Polygon(Pens.LightbluePen, Colors.LightBlue,
              Point.Create(offset, position + 5),
              Point.Create(offset + 5, position),
              Point.Create(offset, position - 5),
              Point.Create(offset, position + 5));

          }
        }

        if (DrawStyle == Style.PointerMinMax || DrawStyle == Style.PointerMin)
        {
          relative_value = _minValues[i] - min_range;
          if (relative_value >= 0.0)
          {
            position = (int)(drawingHeight - (relative_value * pixels_per_unit) - 8);
            position = Math.Max(position, 5);

            Polygon(Pens.LightbluePen, Colors.Hollow,
              Point.Create(offset, position + 5),
              Point.Create(offset + 5, position),
              Point.Create(offset, position - 5),
              Point.Create(offset, position + 5));
          }
        }
        offset += 14;
      }
    }
    public ushort NumValues
    {
      get { return _numValues; }
    }

    public ushort ResetLabel
    {
      get { return _resetLabel; }
      set { _resetLabel = value; }
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

    public ushort[] Labels
    {
      get { return _labels; }
    }

    public void SetLabel(ushort offset, ushort value)
    {
      if (offset >= _numValues)
        throw new IndexOutOfRangeException();

      _labels[offset] = value;
    }
  }
}
