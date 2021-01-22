/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

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
  public sealed class GaugeWidget : Widget
  {
    public enum gauge_style
    {
      gs_pointer,         // a simple line
      gs_sweep,           // a pie sweep
      gs_bar,             // line sweep
      gs_point,           // simple-pointer
      gs_pointer_minmax,  // a pointer with min-max markers
      gs_point_minmax,    // simple-pointer with min-max markers
      gs_hbar,            // small horizonatal bar
                          // bar graph styles
      bgs_point, // simple-pointer
      bgs_pointer_minmax, // a pointer with min-max markers
      bgs_pointer_max, // simple-pointer with max markers
      bgs_pointer_min,
      bgs_small,            // vertical bars with tick marks
    };

    // needs to be same order as above
    internal string[] gauge_style_values =
    {
      "gs_pointer",
      "gs_sweep",
      "gs_bar",
      "gs_point",
      "gs_pointer_minmax",
      "gs_point_minmax",
      "gs_hbar",
      "bgs_point",
      "bgs_pointer_minmax",
      "bgs_pointer_max",
      "bgs_pointer_min",
      "bgs_small"
    };

    internal class step_t
    {
      public short value;
      public Pen pen;
      public uint gauge_color;
    };

    internal class tick_mark_t
    {
      public short value;
      public string text;
    };

    private string name;
    private uint name_color;
    private Font name_font;
    private Point name_pt;
    private bool draw_name;

    private uint background_color;
    private Pen border_pen;
    private bool draw_border;
    private Font font;

    private ushort num_values;

    // a gauge supports up to 4 values
    // this is updated on each window message
    private float[] values = new float[4];
    private float[] min_values = new float[4];
    private float[] max_values = new float[4];
    private ushort[] labels = new ushort[4];

    private ushort reset_label;
    private ushort label;

    private Point center;
    private int gauge_radii;

    private ushort arc_begin;
    private ushort arc_range;
    private float reset_value;

    private gauge_style style;
    private ushort width;          // pointer or sweep width

    private bool draw_value;         // draw the value
    private Font value_font;       // what font to draw a value in
    private Rect value_rect;

    private ArrayList steps;
    private ArrayList ticks;

    private float scale;
    private float offset;

    public GaugeWidget(Widget parent, Rect bounds, ushort id, uint key)
  : base(parent, bounds, id)
    {
      Rect rect_wnd = WindowRect;

      if (!TryRegGetFloat(key, "scale", out scale))
        scale = 1.0f;

      if (!TryRegGetFloat(key, "offset", out offset))
        offset = 0;

      int enumValue;
      if (!LookupEnum(key, "style", gauge_style_values, out enumValue))
        style = gauge_style.gs_pointer;
      else
        style = (gauge_style)enumValue;

      // get the ID's of the reset can msg
      TryRegGetUint16(key, "reset-id", out reset_label);
      TryRegGetFloat(key, "reset-value", out reset_value);

      if (!LookupFont(key, "font", out font))
        // we always have the neo font.
        OpenFont("neo", 9, out font);

      short int_value;
      if (!TryRegGetInt16(key, "center-x", out int_value))
        center.X = rect_wnd.Width >> 1;
      else
        center.X = int_value;

      if (!TryRegGetInt16(key, "center-y", out int_value))
        center.Y = rect_wnd.Height >> 1;
      else
        center.Y = int_value;

      // get the details for the name
      if (!TryRegGetBool(key, "draw-name", out draw_name))
        draw_name = true;

      if (draw_name && TryRegGetString(key, "name", out name))
      {

        if (!LookupColor(key, "name-color", out name_color))
          name_color = Colors.White;

        if (!LookupFont(key, "name-font", out name_font))
          // we always have the neo font.
          OpenFont("neo", 9, out name_font);

        if (!TryRegGetInt16(key, "name-x", out int_value))
          name_pt.X = center.X;
        else
          name_pt.X = int_value;

        if (!TryRegGetInt16(key, "name-y", out int_value))
          name_pt.Y = center.Y;
        else
          name_pt.Y = int_value;
      }

      if (!TryRegGetUint16(key, "arc-begin", out arc_begin))
        arc_begin = 120;

      if (!TryRegGetUint16(key, "arc-range", out arc_range))
        arc_range = 270;

      if (!TryRegGetUint16(key, "width", out width))
        width = 7;

      if (!TryRegGetBool(key, "draw-value", out draw_value))
        draw_value = true;

      ushort uint_value;

      if (draw_value)
      {
        if (!LookupFont(key, "value-font", out value_font))
          OpenFont("neo", 9, out value_font);

        // default values
        value_rect.Bottom = rect_wnd.Bottom;
        value_rect.Left = center.X + 2;
        value_rect.Top = value_rect.Bottom - 25;
        value_rect.Right = rect_wnd.Right;

        if (TryRegGetInt16(key, "value-x", out int_value))
          value_rect.Left = (int)int_value;

        if (TryRegGetUint16(key, "value-y", out uint_value))
          value_rect.Top = (int)uint_value;

        if (TryRegGetUint16(key, "value-w", out uint_value))
          value_rect.Right = (int)uint_value + value_rect.Left;

        if (TryRegGetUint16(key, "value-h", out uint_value))
          value_rect.Bottom = (int)uint_value + value_rect.Top;
      }

      if (!TryRegGetUint16(key, "can-id", out uint_value))
      {
        // could be can value 0..3
        for (num_values = 0; num_values < 4; num_values++)
        {
          string temp_name = string.Format("can-id-{0}", num_values);

          if (!TryRegGetUint16(key, temp_name, out uint_value))
            break;

          labels[num_values] = uint_value;
          values[num_values] = reset_value;
          min_values[num_values] = reset_value;
          max_values[num_values] = reset_value;
        }
      }
      else
      {
        labels[0] = uint_value;
        values[0] = reset_value;
        min_values[0] = reset_value;
        max_values[0] = reset_value;
        num_values = 1;
      }

      if (!TryRegGetUint16(key, "radii", out uint_value))
        uint_value = (ushort)((rect_wnd.Width >> 1) - 5);

      gauge_radii = uint_value;

      // open a step key
      uint step_key;
      if (Syscall.RegOpenKey(key, "step", out step_key) == 0)
      {
        steps = new ArrayList();

        int i;
        // only support 99 ticks/steps
        for (i = 0; i < 99; i++)
        {
          // this stops as soon as the first key is not found
          uint child_key;

          while (Syscall.RegOpenKey(step_key, i.ToString(), out child_key) == 0)
          {
            step_t new_step = new step_t();

            TryRegGetInt16(child_key, "value", out new_step.value);
            LookupColor(child_key, "color", out new_step.gauge_color);
            if (!LookupPen(child_key, "pen", out new_step.pen))
              new_step.pen = Pens.LightbluePen;

            steps.Add(new_step);
          }
        }
      }

      uint ticks_key;
      if (Syscall.RegOpenKey(key, "tick", out ticks_key) == 0)
      {
        ticks = new ArrayList();

        for (int i = 0; i < 99; i++)
        {
          uint child_key;
          while (Syscall.RegOpenKey(ticks_key, i.ToString(), out child_key) == 0)
          {
            // tick-0=650, 650
            // param1 . tick point
            // param2 . tick label
            tick_mark_t new_tick = new tick_mark_t();

            if (!TryRegGetInt16(child_key, "value", out new_tick.value))
              new_tick.value = 0;

            TryRegGetString(child_key, "text", out new_tick.text);

            ticks.Add(new_tick);
          }
        }
      }

      if (!LookupColor(key, "background-color", out background_color))
        background_color = Colors.Black;

      TryRegGetBool(key, "draw-border", out draw_border);

      if (!LookupPen(key, "border-pen", out border_pen))
        border_pen = Pens.GrayPen;


      if (reset_label != 0)
        AddEventListener(reset_label, OnResetLabel);

      for (int i = 0; i < num_values; i++)
        AddEventListener(labels[i], OnValueLabel);

      InvalidateRect();
    }

    private bool LookupEnum(uint key, string name, string[] values, out int index)
    {
      for (index = 0; index < values.Length; index++)
        if (values[index] == name)
          return true;

      return false;
    }

    private void OnValueLabel(CanFlyMsg msg)
    {
      try
      {
        float float_value = msg.GetFloat();

        for (int i = 0; i < num_values; i++)
        {
          if (msg.CanID == labels[i])
          {

            float_value *= scale;
            float_value += offset;

            if (values[i] != float_value)
            {
              values[i] = float_value;

              min_values[i] = Math.Min(min_values[i], values[i]);
              max_values[i] = Math.Max(max_values[i], values[i]);

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
      for (int i = 0; i < num_values; i++)
      {
        min_values[i] = reset_value;
        max_values[i] = reset_value;
      }

      InvalidateRect();
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wnd_rect = WindowRect;

      // fill without a border
      Rectangle(null, background_color, wnd_rect);

      if (draw_border)
        RoundRect(border_pen, Colors.Hollow, wnd_rect, 12);

      if (IsBarStyle)
        UpdateBarGauge();
      else
        UpdateDialGauge();

      EndPaint();
    }

    private void UpdateBarGauge()
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
      if (steps == null || steps.Count < 2)
        return;

      Rect wnd_rect = WindowRect;
      Rect graph = new Rect(wnd_rect.Left, wnd_rect.Top, wnd_rect.Left + 14, wnd_rect.Height);

      for (uint i = 0; i < num_values; i++)
      {
        step_t first_step = (step_t)steps[0];
        step_t last_step = (step_t)steps[steps.Count - 1];

        long range = last_step.value - first_step.value;

        float pixels_per_unit = ((float)graph.Height - 12) / ((float)range);

        // so we now have the increment, just start at the bottom
        Rect drawing_rect = new Rect(
          graph.Left + 8,
          graph.Top + 3,
          graph.Right - 2,
          graph.Bottom - 7);

        for (int step = 1; step < steps.Count; step++)
        {
          last_step = (step_t)steps[step];

          float relative_value = last_step.value - first_step.value;
          first_step = last_step;

          float pixels = relative_value * pixels_per_unit;
          drawing_rect.Top = drawing_rect.Bottom - (int)(pixels);

          Rectangle(null, last_step.pen.Color, drawing_rect);
          drawing_rect.Bottom = drawing_rect.Top;
        }

        graph.Left += 14;
        graph.Right += 14;
      }

      if (draw_name)
      {
        Extent sz = TextExtent(font, name);
        Point pt = wnd_rect.BottomRight;

        pt.X -= 4;
        pt.X -= sz.Dx;
        pt.Y -= sz.Dy;

        DrawText(font, name_color, background_color, name, pt);
      }

      int height = graph.Height - 12;

      if (ticks != null)
      {
        int num_ticks = ticks.Count;


        tick_mark_t first_tick = (tick_mark_t)ticks[0];
        tick_mark_t last_tick = (tick_mark_t)ticks[num_ticks - 1];

        int range = last_tick.value - first_tick.value;
        float pixels_per_unit = ((float)height) / ((float)range);

        for (int i = 0; i < num_ticks; i++)
        {
          last_tick = (tick_mark_t)ticks[i];

          if (last_tick.text == null)
            continue;

          short value = last_tick.value;

          Extent sz = TextExtent(font, last_tick.text);

          float relative_value = value - first_tick.value;
          float pixels = relative_value * pixels_per_unit;
          Point pt = new Point(graph.Left + 2, (graph.Bottom - (int)pixels) - 7 - (sz.Dy >> 1));

          DrawText(font, name_color, background_color, last_tick.text, pt);
        }
      }

      int offset = 1;

      for (int i = 0; i < num_values; i++)
      {
        step_t first_step = (step_t)steps[0];
        step_t last_step = (step_t)steps[steps.Count - 1];

        int drawingHeight = wnd_rect.Height - 12;
        int range = last_step.value - first_step.value;
        float pixels_per_unit = ((float)drawingHeight) / ((float)range);

        float min_range = first_step.value;

        float value = values[i];
        value = Math.Max(value, min_range);
        value = Math.Min(value, min_range + range);

        float relative_value = value - min_range;
        int position = (int)(-(relative_value * pixels_per_unit) - 8);

        position = Math.Max(position, 5);

        Polygon(Pens.LightbluePen, Colors.LightBlue,
          new Point(offset, position + 5),
          new Point(offset + 5, position),
          new Point(offset, position - 5),
          new Point(offset, position + 5));

        if (style == gauge_style.bgs_pointer_minmax || style == gauge_style.bgs_pointer_max)
        {
          relative_value = max_values[i] - min_range;
          if (relative_value >= 0.0)
          {
            position = (int)(drawingHeight - (relative_value * pixels_per_unit) - 8);
            position = Math.Max(position, 5);

            Polygon(Pens.LightbluePen, Colors.LightBlue,
              new Point(offset, position + 5),
              new Point(offset + 5, position),
              new Point(offset, position - 5),
              new Point(offset, position + 5));

          }
        }

        if (style == gauge_style.bgs_pointer_minmax || style == gauge_style.bgs_pointer_min)
        {
          relative_value = min_values[i] - min_range;
          if (relative_value >= 0.0)
          {
            position = (int)(drawingHeight - (relative_value * pixels_per_unit) - 8);
            position = Math.Max(position, 5);

            Polygon(Pens.LightbluePen, Colors.Hollow,
              new Point(offset, position + 5),
              new Point(offset + 5, position),
              new Point(offset, position - 5),
              new Point(offset, position + 5));
          }
        }
        offset += 14;
      }
    }

    private void UpdateDialGauge()
    {
      Rect wnd_rect = WindowRect;
      if (draw_name)
      {
        Extent sz = TextExtent(name_font, name);

        name_pt.X -= sz.Dx >> 1;
        name_pt.Y -= sz.Dy >> 1;

        DrawText(name_font, name_color, background_color, name, name_pt);
      }

      short min_range = 0;
      short max_range = 0;
      float degrees_per_unit = 1.0f;

      // we now draw an arc.  The range is the last value in the arc
      // minus the first value.  Note that the values can be -
      // the range of the gauge is gauge_y - 360 degrees
      if (steps != null)
      {
        int num_steps = steps.Count;

        step_t first_step = (step_t)steps[0];
        step_t last_step = (step_t)steps[num_steps - 1];

        min_range = first_step.value;
        max_range = last_step.value;

        int steps_range = max_range - min_range;

        degrees_per_unit = ((float)arc_range) / steps_range;

        float arc_start = arc_begin;
        ushort i;

        //    font(_font);
        //    background_color(Colors.Black);
        //    text_color(Colors.White);
        //
        for (i = 1; i < num_steps; i++)
        {
          last_step = (step_t)steps[i];

          // the arc angle is the step end
          float arc_end = (last_step.value - min_range) * degrees_per_unit;
          arc_end += arc_begin;

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

          Arc(last_step.pen, center, gauge_radii, arc_angles[0], arc_angles[1]);

          if (arc_angles[2] >= 0)
            Arc(last_step.pen, center, gauge_radii, 0, arc_angles[2]);

          arc_start = arc_end;
        }
      }

      if (ticks != null)
      {
        int num_ticks = ticks.Count;

        // we now draw the tick marks.  They are a line from the point 5 pixels long.

        // we create a white pen for this
        //pen(&white_pen_2);

        int line_start = gauge_radii;
        line_start += width >> 1;              // add the line half width

        int line_end = line_start - width - 5;
        float arc_start;

        for (int i = 0; i < num_ticks; i++)
        {
          tick_mark_t tick = (tick_mark_t)ticks[i];

          arc_start = (tick.value - min_range) * degrees_per_unit;
          arc_start += arc_begin;
          // make it point to correct spot....
          arc_start += 180;

          Polyline(Pens.WhitePen,
            RotatePoint(center, new Point(center.X, center.Y - line_start), (int)arc_start),
          RotatePoint(center, new Point(center.X, center.Y - line_end), (int)arc_start));

          if (tick.text != null && font != null)
          {
            // write the text at the point
            Extent size = TextExtent(font, tick.text);

            // the text is below the tick marks
            Point top_left = RotatePoint(center, new Point(center.X, center.Y - line_end + (size.Dy >> 1)), (int)arc_start);

            top_left.X -= (size.Dx >> 1);
            top_left.Y -= (size.Dy >> 1);

            DrawText(font, Colors.White, background_color, tick.text, top_left);

          }
        }
      }

      // we now calculate the range of the segments.  There are 8 segments
      // around a 240 degree arc.  We draw the nearest based on the range
      // of the gauge.
      int rotation;

      switch (style)
      {
        case gauge_style.gs_pointer_minmax:
          DrawPoint(wnd_rect, CalculatePen(width, min_values[0]), CalculateRotation(min_values[0]));
          DrawPoint(wnd_rect, CalculatePen(1, max_values[0]), CalculateRotation(max_values[0]));
          break;
        case gauge_style.gs_pointer:
          rotation = CalculateRotation(values[0]);
          Polyline(CalculatePen(width, values[0]),
            RotatePoint(center, new Point(center.X, center.Y - 5), rotation),
            RotatePoint(center, new Point(center.X, center.Y - gauge_radii + 5), rotation));
          break;
        case gauge_style.gs_sweep:
          Pie(CalculatePen(width, values[0]),
            CalculateColor(values[0]),
            center, arc_begin + 90,
            CalculateRotation(values[0]) + 90,
            center.Y - gauge_radii + 5, 5);
          break;
        case gauge_style.gs_bar:
          Pen outlinePen = CalculatePen(width, values[0]);
          rotation = CalculateRotation(values[0]);

          int[] arc_angles = new int[3] { arc_begin + 90, rotation + 90, -1 };

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

          int radii = gauge_radii - (width >> 1);
          radii -= outlinePen.Width >> 1;

          Arc(outlinePen, center, radii, arc_angles[0], arc_angles[1]);

          if (arc_angles[2] > 0)
            Arc(outlinePen, center, radii, 0, arc_angles[2]);
          break;
        case gauge_style.gs_point_minmax:
          DrawPoint(wnd_rect, CalculatePen(1, min_values[0]), CalculateRotation(min_values[0]));
          DrawPoint(wnd_rect, CalculatePen(1, max_values[0]), CalculateRotation(max_values[0]));
          DrawPoint(wnd_rect, CalculatePen(1, values[0]), CalculateRotation(values[0]));
          break;
        case gauge_style.gs_point:
          DrawPoint(wnd_rect, CalculatePen(1, values[0]), CalculateRotation(values[0]));
          break;
      }

      // draw the value of the gauge as a text string
      if (draw_value && value_font != null)
      {
        string str = ((int)values[0]).ToString();
        Extent size = TextExtent(value_font, str);

        // draw a rectangle around the text
        Rectangle(Pens.GrayPen, Colors.Black, value_rect);

        DrawText(value_font, Colors.LightBlue, Colors.Black, str,
          new Point(value_rect.Right - (size.Dx + 2), value_rect.Top + 2), value_rect, TextOutStyle.Clipped);
      }
    }

    private bool IsBarStyle
    {
      get { return (int)style >= (int)gauge_style.bgs_point; }
    }


    private void DrawPoint(Rect rect, Pen outLine, int rotation)
    {
      int rotn = (int)DegressToRadians(rotation + 90);
      Polygon(outLine, outLine.Color,
        RotatePoint(center, new Point(center.X, center.Y - gauge_radii + 5), rotn),
        RotatePoint(center, new Point(center.X - 6, center.Y - gauge_radii + 11), rotn),
        RotatePoint(center, new Point(center.X + 6, center.Y - gauge_radii + 11), rotn),
        RotatePoint(center, new Point(center.X, center.Y - gauge_radii + 5), rotn));
    }

    private int CalculateRotation(double value)
    {

      if (steps == null)
        return 0;

      int count = steps.Count;

      step_t first_step = (step_t)steps[0];
      step_t last_step = (step_t)steps[count - 1];

      double min_range = first_step.value;
      double max_range = last_step.value;

      // get the percent that the gauge is displaying
      double percent = (Math.Max(Math.Min(value, max_range), min_range) - min_range) / (max_range - min_range);

      return (int)(arc_begin + (arc_range * percent));
    }

    private Pen CalculatePen(ushort width, float value)
    {
      if (steps == null || steps.Count == 0)
        return new Pen(Colors.Hollow, width, PenStyle.Solid);

      uint color = Colors.Hollow;
      // step 0 is only used to set the minimum value for the gauge
      // so it is ignored
      for (int i = 1; i < steps.Count; i++)
      {
        // get the next one, and see if we are done.
        step_t step = (step_t)steps[i];

        // assume this is our color.
        color = step.gauge_color;
        if (value <= step.value)
          break;
      }

      return new Pen(color, width, PenStyle.Solid);
    }

    private uint CalculateColor(float value)
    {
      uint fill_color = Colors.LightBlue;

      if (steps == null || steps.Count == 0)
        return fill_color;

      step_t step = (step_t)steps[0];

      for (int i = 0; i < steps.Count; i++)
      {
        fill_color = step.gauge_color;

        step = (step_t)steps[i];

        if (value >= step.value)
          break;
      }

      return fill_color;
    }
  }
}
