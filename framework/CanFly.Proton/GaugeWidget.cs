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
    public class Step
    {
      public short value;
      public Pen pen;
      public Color gauge_color;
    };

    public class TickMark
    {
      public short value;
      public string text;
    };

  public class GaugeWidget : Widget
  {
    public enum GaugeStyle
    {
      gs_pointer,         // a simple line
      gs_sweep,           // a pie sweep
      gs_bar,             // line sweep
      gs_point,           // simple-pointer
      gs_pointer_minmax,  // a pointer with min-max markers
      gs_point_minmax,    // simple-pointer with min-max markers
      bgvs_point, // simple-pointer
      bgvs_pointer_minmax, // a pointer with min-max markers
      bgvs_pointer_max, // simple-pointer with max markers
      bgvs_pointer_min,
      bghs_point,            // small horizonatal bar
      bghs_pointer_minmax, // a pointer with min-max markers
      bghs_pointer_max, // simple-pointer with max markers
      bghs_pointer_min,
    };

    // needs to be same order as above
    internal string[] GaugeStyleValues =
    {
      "gs_pointer",
      "gs_sweep",
      "gs_bar",
      "gs_point",
      "gs_pointer_minmax",
      "gs_point_minmax",
      "bgs_point",
      "bgs_pointer_minmax",
      "bgs_pointer_max",
      "bgs_pointer_min",
      "bghs_point",
      "bghs_pointer_minmax",
      "bghs_pointer_max",
      "bghs_pointer_min"
    };

    private string _name;
    private Color _nameColor;
    private Font _nameFont;
    private Point _namePt;
    private bool _drawName;
    private Color _backgroundColor;
    private Pen _borderPen;
    private bool _drawBorder;
    private Font _font;

    private ushort _numValues;

    private ushort _resetLabel;
    private ushort _label;

    private Point _center;
    private int _gaugeRadii;

    private ushort _arcBegin;
    private ushort _arcRange;
    private float _resetValue;

    private GaugeStyle _style;
    private ushort _width;          // pointer or sweep width

    private bool _drawValue;         // draw the value
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

  public GaugeWidget(Widget parent, Rect bounds, ushort id, ushort key)
    : base(parent, bounds, id)
    {
      Rect rect_wnd = WindowRect;

      if (!TryRegGetFloat(key, "scale", out _scale))
        _scale = 1.0f;

      if (!TryRegGetFloat(key, "offset", out _offset))
        _offset = 0;

      int enumValue;
      if (!LookupEnum(key, "style", GaugeStyleValues, out enumValue))
        Style = GaugeStyle.gs_pointer;
      else
        Style = (GaugeStyle)enumValue;

      // get the ID's of the reset can msg
      TryRegGetUint16(key, "reset-id", out _resetLabel);
      TryRegGetFloat(key, "reset-value", out _resetValue);

      if (!LookupFont(key, "font", out _font))
        // we always have the neo font.
        OpenFont("neo", 9, out _font);

      short x;
      if (!TryRegGetInt16(key, "center-x", out x))
        x = (short) (rect_wnd.Width >> 1);

      short y;
      if (!TryRegGetInt16(key, "center-y", out y))
        y = (short)(rect_wnd.Height >> 1);

      Center = Point.Create(x, y);

      // get the details for the name
      if (!TryRegGetBool(key, "draw-name", out _drawName))
        DrawName = true;

      if (DrawName && TryRegGetString(key, "name", out _name))
      {
        if (!LookupColor(key, "name-color", out _nameColor))
          NameColor = Colors.White;

        if (!LookupFont(key, "name-font", out _nameFont))
          // we always have the neo font.
          OpenFont("neo", 9, out _nameFont);

        if (!TryRegGetInt16(key, "name-x", out x))
          x = (short)Center.X;

        if (!TryRegGetInt16(key, "name-y", out y))
          y = (short)Center.Y;

        NamePt = Point.Create(x, y);
      }

      if (!TryRegGetUint16(key, "arc-begin", out _arcBegin))
        ArcBegin = 120;

      if (!TryRegGetUint16(key, "arc-range", out _arcRange))
        ArcRange = 270;

      if (!TryRegGetUint16(key, "width", out _width))
        Width = 7;

      if (!TryRegGetBool(key, "draw-value", out _drawValue))
        DrawValue = true;

      ushort uint_value;
      short dx;
      short dy;
      if (DrawValue)
      {
        if (!LookupFont(key, "value-font", out _valueFont))
          OpenFont("neo", 9, out _valueFont);

        if (TryRegGetInt16(key, "value-x", out x))
          x = (short)(Center.X + 2);

        if (TryRegGetInt16(key, "value-y", out y))
          y = (short)(ValueRect.Bottom - 25);

        if (TryRegGetInt16(key, "value-w", out dx))
          dx = (short)(rect_wnd.Right - x);

        if (TryRegGetInt16(key, "value-h", out dy))
          dy = 25;

        ValueRect = Rect.Create(x, y, x + dx, y + dy);
      }

      if (!TryRegGetUint16(key, "can-id", out uint_value))
      {
        // could be can value 0..3
        for (_numValues = 0; _numValues < 6; _numValues++)
        {
          string temp_name = string.Format("can-id-{0}", _numValues);

          if (!TryRegGetUint16(key, temp_name, out uint_value))
            break;

          Labels[_numValues] = uint_value;
         }
      }
      else
      {
        Labels[0] = uint_value;
        _numValues = 1;
      }

      if (!TryRegGetUint16(key, "radii", out uint_value))
        uint_value = (ushort)((rect_wnd.Width >> 1) - 5);

      GaugeRadii = uint_value;

      // open a step key
      ushort step_key;

      if (TryRegOpenKey(key, "step", out step_key))
      {
        _steps = new ArrayList();

        int i;
        // only support 99 ticks/steps
        for (i = 0; i < 99; i++)
        {
          // this stops as soon as the first key is not found
          ushort child_key;

          if (TryRegOpenKey(step_key, i.ToString(), out child_key))
          {
            Step new_step = new Step();

            TryRegGetInt16(child_key, "value", out new_step.value);
            LookupColor(child_key, "color", out new_step.gauge_color);
            if (!LookupPen(child_key, "pen", out new_step.pen))
              new_step.pen = Pens.LightbluePen;

            _steps.Add(new_step);
          }
          else
            break;
        }
      }

      ushort ticks_key;
      if (TryRegOpenKey(key, "tick", out ticks_key))
      {
        _ticks = new ArrayList();

        for (int i = 0; i < 99; i++)
        {
          ushort child_key;
          if (TryRegOpenKey(ticks_key, i.ToString(), out child_key))
          {
            // tick-0=650, 650
            // param1 . tick point
            // param2 . tick label
            TickMark new_tick = new TickMark();

            if (!TryRegGetInt16(child_key, "value", out new_tick.value))
              new_tick.value = 0;

            TryRegGetString(child_key, "text", out new_tick.text);

            _ticks.Add(new_tick);
          }
          else
            break;
        }
      }

      if (!LookupColor(key, "background-color", out _backgroundColor))
        _backgroundColor = Colors.Black;

      TryRegGetBool(key, "draw-border", out _drawBorder);

      if (!LookupPen(key, "border-pen", out _borderPen))
        _borderPen = Pens.GrayPen;

      Initialize(0);
    }
    /// <summary>
    /// Construct a gauge widget with no settings
    /// </summary>
    /// <param name="parent">Parent widget</param>
    /// <param name="bounds">Position of the widget relative to the parent</param>
    /// <param name="id">Id of the widget</param>
    public GaugeWidget(Widget parent, Rect bounds, ushort id)
      : base(parent, bounds, id)
    {
    }
    /// <summary>
    /// After all of the widget parameters are set, initialize the constructed widget
    /// </summary>
    /// <param name="numValues"></param>
    public void Initialize(ushort numValues)
    {
      if (numValues > 0)
      {
        _numValues = numValues;
        _labels = new ushort[numValues];
      }

      _values = new float[_numValues];
      _minValues = new float[_numValues];
      _maxValues = new float[_numValues];

      float resetValue = ResetValue;
      for (ushort i = 0; i < NumValues; i++)
      {
        _values[i] = resetValue;
        _minValues[i] = resetValue;
        _maxValues[i] = resetValue;
      }

      if (ResetLabel != 0)
        AddCanFlyEvent(ResetLabel, OnResetLabel);

      for (int i = 0; i < NumValues; i++)
        AddCanFlyEvent(Labels[i], OnValueLabel);

      InvalidateRect();
    }

    private bool LookupEnum(ushort key, string name, string[] values, out int index)
    {
      index = -1;
      string value;
      if(!TryRegGetString(key, name, out value))
        return false;

      for (index = 0; index < values.Length; index++)
        if (values[index] == value)
          return true;

      return false;
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

              _minValues[i] = (float) Math.Min(_minValues[i], _values[i]);
              _maxValues[i] = (float) Math.Max(_maxValues[i], _values[i]);

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

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wnd_rect = WindowRect;

      // fill without a border
      Rectangle(Pens.Hollow, BackgroundColor, wnd_rect);

      if (DrawBorder)
        RoundRect(BorderPen, Colors.Hollow, wnd_rect, 12);

      if (IsVerticalBarStyle)
        UpdateVerticalBarGauge();
      if(IsHorizontalBarStyle)
        UpdateHorizontalBarGauge();
      else
        UpdateDialGauge();

      EndPaint();
    }

    private void UpdateHorizontalBarGauge()
    {
      
    }

    private void UpdateVerticalBarGauge()
    {
      // we support up to 4 ID's.

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
      Step[] steps = Steps;

      if(steps == null || steps.Length < 2)
        return;

      Rect wnd_rect = WindowRect;
      Rect graph = Rect.Create(wnd_rect.Left, wnd_rect.Top, wnd_rect.Left + 14, wnd_rect.Height);

      for (uint i = 0; i < _numValues; i++)
      {
        Step first_step = steps[0];
        Step last_step = steps[steps.Length - 1];

        long range = last_step.value - first_step.value;

        float pixels_per_unit = ((float)graph.Height - 12) / ((float)range);

        // so we now have the increment, just start at the bottom
        Rect drawing_rect = Rect.Create(
          graph.Left + 8,
          graph.Top + 3,
          graph.Right - 2,
          graph.Bottom - 7);

        for (int step = 1; step < steps.Length; step++)
        {
          last_step = steps[step];

          float relative_value = last_step.value - first_step.value;
          first_step = last_step;

          float pixels = relative_value * pixels_per_unit;

          drawing_rect = drawing_rect.Add(0, 0, 0, (int) -pixels);

          Rectangle(Pens.Hollow, last_step.pen.Color, drawing_rect);

          drawing_rect = Rect.Create(drawing_rect.Top, drawing_rect.Top, 
            drawing_rect.Right, drawing_rect.Top);
        }

        graph = graph.Add(14, 0, 14, 0);
      }

      if (DrawName)
      {
        Extent sz = TextExtent(_font, _name);

        DrawText(Font, NameColor, BackgroundColor, Name, 
          wnd_rect.BottomRight.Add(-(4 + sz.Dx), -sz.Dy));
      }

      int height = graph.Height - 12;

      TickMark[] ticks = Ticks;

      if (ticks != null)
      {
        int num_ticks = ticks.Length;

        TickMark first_tick = ticks[0];
        TickMark last_tick = ticks[num_ticks - 1];

        int range = last_tick.value - first_tick.value;
        float pixels_per_unit = ((float)height) / ((float)range);

        for (int i = 0; i < num_ticks; i++)
        {
          last_tick = ticks[i];

          if (last_tick.text == null)
            continue;

          short value = last_tick.value;

          Extent sz = TextExtent(Font, last_tick.text);

          float relative_value = value - first_tick.value;
          float pixels = relative_value * pixels_per_unit;
          Point pt = Point.Create(graph.Left + 2, (graph.Bottom - (int)pixels) - 7 - (sz.Dy >> 1));

          DrawText(Font, NameColor, BackgroundColor, last_tick.text, pt);
        }
      }

      int offset = 1;

      for (int i = 0; i < _numValues; i++)
      {
        Step first_step = steps[0];
        Step last_step = steps[steps.Length - 1];

        int drawingHeight = wnd_rect.Height - 12;
        int range = last_step.value - first_step.value;
        float pixels_per_unit = ((float)drawingHeight) / ((float)range);

        float min_range = first_step.value;

        float value = _values[i];
        value = (float) Math.Max(value, min_range);
        value = (float) Math.Min(value, min_range + range);

        float relative_value = value - min_range;
        int position = (int)(-(relative_value * pixels_per_unit) - 8);

        position = Math.Max(position, 5);

        Polygon(Pens.LightbluePen, Colors.LightBlue,
          Point.Create(offset, position + 5),
          Point.Create(offset + 5, position),
          Point.Create(offset, position - 5),
          Point.Create(offset, position + 5));

        if (_style == GaugeStyle.bgvs_pointer_minmax || _style == GaugeStyle.bgvs_pointer_max)
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

        if (Style == GaugeStyle.bgvs_pointer_minmax || Style == GaugeStyle.bgvs_pointer_min)
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

    private void UpdateDialGauge()
    {
      Rect wnd_rect = WindowRect;
      if (DrawName)
      {
        Extent sz = TextExtent(NameFont, Name);

        DrawText(NameFont, NameColor, BackgroundColor, Name, 
          NamePt.Add(-(sz.Dx >> 1), -(sz.Dy >> 1)));
      }

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

        min_range = first_step.value;
        max_range = last_step.value;

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
          float arc_end = (last_step.value - min_range) * degrees_per_unit;
          arc_end += ArcBegin;

          // the arc starts a point(0, -1) which is 0 degrees
          // so ranges from 0..270 are 90.360 and needs to
          // be split to draw 270..360
          // draw arc
          int[] arc_angles = { (int)arc_start + 90, (int)arc_end + 90, -1 };

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

          CanFly.Pen pen = last_step.pen;

          Arc(pen, Center, GaugeRadii, arc_angles[0], arc_angles[1]);

          if (arc_angles[2] >= 0)
            Arc(last_step.pen, Center, GaugeRadii, 0, arc_angles[2]);

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

        int line_start = GaugeRadii;
        line_start += Width >> 1;              // add the line half width

        int line_end = line_start - Width - 5;
        float arc_start;

        for (int i = 0; i < num_ticks; i++)
        {
          TickMark tick = ticks[i];

          arc_start = (tick.value - min_range) * degrees_per_unit;
          arc_start += ArcBegin;
          // make it point to correct spot....
          arc_start += 180;

          Polyline(Pens.WhitePen,
            RotatePoint(Center, Point.Create(Center.X, Center.Y - line_start), (int)arc_start),
          RotatePoint(Center, Point.Create(Center.X, Center.Y - line_end), (int)arc_start));

          if (tick.text != null)
          {
            // write the text at the point
            Extent size = TextExtent(Font, tick.text);

            // the text is below the tick marks
            Point top_left = RotatePoint(Center, Point.Create(Center.X, Center.Y - line_end + (size.Dy >> 1)), (int)arc_start);

            top_left.Add(-(size.Dx >> 1), -(size.Dy >> 1));

            DrawText(Font, Colors.White, BackgroundColor, tick.text, top_left);

          }
        }
      }

      // we now calculate the range of the segments.  There are 8 segments
      // around a 240 degree arc.  We draw the nearest based on the range
      // of the gauge.
      int rotation;

      switch (_style)
      {
        case GaugeStyle.gs_pointer_minmax:
          DrawPoint(wnd_rect, CalculatePen(Width, _minValues[0]), CalculateRotation(_minValues[0]));
          DrawPoint(wnd_rect, CalculatePen(1, _maxValues[0]), CalculateRotation(_maxValues[0]));
          break;
        case GaugeStyle.gs_pointer:
          rotation = CalculateRotation(_values[0]);
          Polyline(CalculatePen(Width, _values[0]),
            RotatePoint(Center, Point.Create(Center.X, Center.Y - 5), rotation),
            RotatePoint(Center, Point.Create(Center.X, Center.Y - GaugeRadii + 5), rotation));
          break;
        case GaugeStyle.gs_sweep:
          Pie(CalculatePen(Width, _values[0]),
            CalculateColor(_values[0]), Center, ArcBegin + 90,
            CalculateRotation(_values[0]) + 90, Center.Y - GaugeRadii + 5, 5);
          break;
        case GaugeStyle.gs_bar:
          Pen outlinePen = CalculatePen(Width, _values[0]);
          rotation = CalculateRotation(_values[0]);

          int[] arc_angles = new int[3] { _arcBegin + 90, rotation + 90, -1 };

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

          if(arc_angles[0] == arc_angles[1] && arc_angles[2] == -1)
          {
            Point pt = RotatePoint(Center, Point.Create(Center.X + radii, Center.Y), arc_angles[0]);
            // draw a dot
            Rect ellipse = Rect.Create(pt.X - offset,  pt.Y - offset, pt.X + offset, pt.Y + offset);
            
            Ellipse(Pens.Hollow, outlinePen.Color, ellipse);
          }
          else
          {
            Arc(outlinePen, Center, radii, arc_angles[0], arc_angles[1]);

            if (arc_angles[2] > 0)
              Arc(outlinePen, Center, radii, 0, arc_angles[2]);
          }
          break;
        case GaugeStyle.gs_point_minmax:
          DrawPoint(wnd_rect, CalculatePen(1, _minValues[0]), CalculateRotation(_minValues[0]));
          DrawPoint(wnd_rect, CalculatePen(1, _maxValues[0]), CalculateRotation(_maxValues[0]));
          DrawPoint(wnd_rect, CalculatePen(1, _values[0]), CalculateRotation(_values[0]));
          break;
        case GaugeStyle.gs_point:
          DrawPoint(wnd_rect, CalculatePen(1, _values[0]), CalculateRotation(_values[0]));
          break;
      }

      // draw the value of the gauge as a text string
      if (_drawValue)
      {
        string str = ((int)_values[0]).ToString();
        Extent size = TextExtent(ValueFont, str);

        // draw a rectangle around the text
        Rectangle(Pens.GrayPen, Colors.Black, ValueRect);

        DrawText(ValueFont, Colors.LightBlue, Colors.Black, str,
          Point.Create(ValueRect.Right - (size.Dx + 2), ValueRect.Top + 2), ValueRect, TextOutStyle.Clipped);
      }
    }

    private bool IsVerticalBarStyle
    {
      get 
      {
        int value = (int)Style;

        return value >= (int)GaugeStyle.bgvs_point && value < (int)GaugeStyle.bghs_point; 
      }
    }

    private bool IsHorizontalBarStyle
    {
      get 
      {
        int value = (int)Style;

        return value >= (int)GaugeStyle.bghs_point; 
      }
    }


    private void DrawPoint(Rect rect, Pen outLine, int rotation)
    {
      Polygon(outLine, outLine.Color,
        RotatePoint(_center, Point.Create(_center.X, _center.Y + _gaugeRadii - 5), rotation),
        RotatePoint(_center, Point.Create(_center.X - 6, _center.Y + _gaugeRadii - 11), rotation),
        RotatePoint(_center, Point.Create(_center.X + 6, _center.Y + _gaugeRadii - 11), rotation),
        RotatePoint(_center, Point.Create(_center.X, _center.Y + _gaugeRadii - 5), rotation));
    }

    private int CalculateRotation(double value)
    {
      if (_steps == null)
        return 0;

      int count = _steps.Count;

      Step first_step = (Step)_steps[0];
      Step last_step = (Step)_steps[count - 1];

      double min_range = first_step.value;
      double max_range = last_step.value;

      // get the percent that the gauge is displaying
      double percent = (Math.Max(Math.Min(value, max_range), min_range) - min_range) / (max_range - min_range);

      return (int)(_arcBegin + (_arcRange * percent));
    }

    private Pen CalculatePen(ushort width, float value)
    {
      if (_steps == null || _steps.Count == 0)
        return Pen.Create(Colors.Hollow, width, PenStyle.Solid);

      Color color = Colors.Hollow;
      // step 0 is only used to set the minimum value for the gauge
      // so it is ignored
      for (int i = 1; i < _steps.Count; i++)
      {
        // get the next one, and see if we are done.
        Step step = (Step)_steps[i];

        // assume this is our color.
        color = step.gauge_color;
        if (value <= step.value)
          break;
      }

      return Pen.Create(color, width, PenStyle.Solid);
    }

    private Color CalculateColor(float value)
    {
      Color fill_color = Colors.LightBlue;

      if (_steps == null || _steps.Count == 0)
        return fill_color;

      Step step = (Step)_steps[0];

      for (int i = 0; i < _steps.Count; i++)
      {
        fill_color = step.gauge_color;

        step = (Step)_steps[i];

        if (value >= step.value)
          break;
      }

      return fill_color;
    }

    public ushort NumValues
    {
      get { return _numValues; }
    }

    public string Name
    {
      get { return _name; }
      set { _name = value; }
    }

    public Color NameColor
    {
      get { return _nameColor; }
      set { _nameColor = value; }
    }
    public Font NameFont
    {
      get { return _nameFont; }
      set { _nameFont = value; }
    }
    public Point NamePt
    {
      get { return _namePt; }
      set { _namePt = value; }
    }
    public bool DrawName
    {
      get { return _drawName; }
      set { _drawName = value; }
    }
    public Color BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
    }
    public Pen BorderPen
    {
      get { return _borderPen; }
      set { _borderPen = value; }
    }
    public bool DrawBorder
    {
      get { return _drawBorder; }
      set { _drawBorder = value; }
    }
    public Font Font
    {
      get { return _font; }
      set { _font = value; }
    }
    public ushort ResetLabel
    {
      get { return _resetLabel; }
      set { _resetLabel = value; }
    }
    public ushort Label
    {
      get { return _label; }
      set { _label = value; }
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
    public GaugeStyle Style
    {
      get { return _style; }
      set { _style = value; }
    }
    public ushort Width
    {
      get { return _width; }
      set { _width = value; }
    }
    public bool DrawValue
    {
      get { return _drawValue; }
      set { _drawValue = value; }
    }
    public Font ValueFont
    {
      get { return _valueFont; }
      set { _valueFont = value; }
    }
    public Rect ValueRect
    {
      get { return _valueRect; }
      set { _valueRect = value; }
    }
    public Step[] Steps 
    {
      get
      { 
        if(_steps == null)
          return null;

        Step[] result = new Step[_steps.Count];

        for (int i = 0; i < _steps.Count; i++)
          result[i] = (Step)_steps[i];

        return result;
      } 
    }

    public void AddStep(Step step)
    {
      if(_steps == null)
        _steps = new ArrayList();

      _steps.Add(step);
    }

    public TickMark[] Ticks 
    {
      get
      {
        if(_ticks == null)
          return null;

        TickMark[] result = new TickMark[_ticks.Count];

        for (int i = 0; i < _ticks.Count; i++)
          result[i] = (TickMark) _ticks[i];

        return result;
      } 
    }

    public void AddTick(TickMark tick)
    {
      if(_ticks == null)
        _ticks = new ArrayList();

      _ticks.Add(tick);
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
      if(offset >= _numValues)
        throw new IndexOutOfRangeException();

      _labels[offset] = value;
    }
  }
}
