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
  public sealed class HSIWidget : Widget
  {
    private Pen track_pen = Pen.Create(Colors.Gray, 1, PenStyle.Dot );

    enum HSIMode {
      ds_course,
      ds_heading,
    };

    private short _direction;
    private short course;
    private short deviation;
    private short track;
    private short distance_to_waypoint;
    private short time_to_waypoint;
    private short wind_direction; // always true
    private short magnetic_variation;
    private short wind_speed;
    private short heading_bug;
    private short heading;
    private Color background_color;
    private bool draw_border;
    private Font font;
    private string waypoint_name;

    internal HSIWidget(Widget parent, Rect bounds, ushort id, ushort key)
    : base(parent, bounds, id)
    {
      if (!LookupColor(key, "background-color", out background_color))
        background_color = Colors.Black;

      TryRegGetBool(key, "draw-border", out draw_border);

      if (!LookupFont(key, "font", out font))
        OpenFont("neo", 9, out font);

      AddEventListener(CanFlyID.id_magnetic_heading, OnMagneticHeading);
      AddEventListener(CanFlyID.id_heading, OnHeading);
      AddEventListener(CanFlyID.id_heading_angle, OnHeadingAngle);
      AddEventListener(CanFlyID.id_deviation, OnDeviation);
      AddEventListener(CanFlyID.id_selected_course, OnSelectedCourse);
      AddEventListener(CanFlyID.id_track, OnTrack);
      AddEventListener(CanFlyID.id_wind_speed, OnWindSpeed);
      AddEventListener(CanFlyID.id_wind_direction, OnWindDirection);
      AddEventListener(CanFlyID.id_distance_to_next, OnDistanceToNext);
      AddEventListener(CanFlyID.id_magnetic_variation, OnMagneticVariation);
      AddEventListener(CanFlyID.id_estimated_time_to_next, OnEstimatedTimeToNext);

      InvalidateRect();
    }

    private void OnEstimatedTimeToNext(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (time_to_waypoint != value)
      {
        time_to_waypoint = value;
        InvalidateRect();
      }
    }

    private void OnMagneticVariation(CanFlyMsg e)
    {
      float v = e.GetFloat();
      short value = (short)RadiansToDegrees(v);
      if (magnetic_variation != value)
      {
        magnetic_variation = value;
        InvalidateRect();
      }
    }

    private void OnDistanceToNext(CanFlyMsg e)
    {
      float v = e.GetFloat();
      
      short value = (short)MetersToNM(v);
      if (distance_to_waypoint != value)
      {
        distance_to_waypoint = value;
        InvalidateRect();
      }
    }

    private void OnWindDirection(CanFlyMsg e)
    {
      float v = e.GetFloat();
      short value = (short)RadiansToDegrees(v);
      if (wind_direction != value)
      {
        wind_direction = value;
        InvalidateRect();
      }
    }

    private void OnWindSpeed(CanFlyMsg e)
    {
      float v = e.GetFloat();
      short value = (short)MetersPerSecondToKnots(v);
      if (wind_speed != value)
      {
        wind_speed = value;
        InvalidateRect();
      }
    }

    private void OnTrack(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (track != value)
      {
        track = value;
        InvalidateRect();
      }
    }

    private void OnSelectedCourse(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (course != value)
      {
        course = value;
        InvalidateRect();
      }
    }

    private void OnDeviation(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (deviation != value)
      {
        deviation = value;
        // the deviation is +/- * 10
        InvalidateRect();
      }
    }

    private void OnHeadingAngle(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (heading != value)
      {
        heading = value;
        InvalidateRect();
      }
    }

    private void OnHeading(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (heading_bug != value)
      {
        heading_bug = value;
        InvalidateRect();
      }
    }

    private void OnMagneticHeading(CanFlyMsg e)
    {
      short direction = e.GetInt16();

      while (direction < 0)
        direction += 360;
      while (direction > 359)
        direction -= 360;

      if (_direction != direction)
      {
        _direction = direction;
        InvalidateRect();
      }
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      Rect wnd_rect = WindowRect;
      Extent ex = wnd_rect.Extent;

      Rectangle(Pens.Hollow, background_color, WindowRect);

      if (draw_border)
        RoundRect(Pens.GrayPen, Colors.Hollow, WindowRect, 12);

      /////////////////////////////////////////////////////////////////////////////
      //
      // Draw the HSI Indicator

      int mark_start = 12;
      int center_x = ex.Dx >> 1;
      int center_y = ex.Dy >> 1;
      int window_x = ex.Dx;
      int window_y = ex.Dy;
      int pixels_per_nm_cdi = 6;
      Point median = Point.Create( center_x, center_y);
      int major_mark = mark_start + 16;
      int minor_mark = mark_start + 8;
      int font_x_y = 19;
      int font_center = (font_x_y >> 1) + 1;
      int font_ordinal = major_mark + font_center;

      // start at 0
      int i = 0;
      int index;
      for (index = -_direction; i < 12; index += 30, i++)
      {
        while (index > 359)
          index -= 360;

        while (index < 0)
          index += 360;

        // draw the marker
        Polyline(Pens.WhitePen, 
          RotatePoint(median, Point.Create(center_x, mark_start), index), 
          RotatePoint(median, Point.Create(center_x, major_mark), index));

        bool do_minor_mark = false;
        short minor_index;
        for (minor_index = 0; minor_index < 30; minor_index += 5)
        {
          Polyline(Pens.WhitePen,
            RotatePoint(median, Point.Create(center_x, mark_start), index + minor_index),
            RotatePoint(median, Point.Create(center_x, do_minor_mark ? minor_mark : major_mark), index + minor_index));

          do_minor_mark = !do_minor_mark;
        }

        // we now draw the text onto the canvas.  The text has a 23x23 pixel
        // block so the center is 12, 12.
        Point fontPt = RotatePoint(median, Point.Create(center_x, font_ordinal), (index < 0) ? index + 360 : index);

        DrawText(font, Colors.White, Colors.Black, i.ToString(),
          Point.Create(fontPt.X - font_center, fontPt.Y - font_center));
      }

      ///////////////////////////////////////////////////////////////////////////
      // Draw the Track

      int rotation = track - _direction;

      // the marker is a dashed line
      Polygon(Pens.GrayPen, Colors.Hollow,
        RotatePoint(median, Point.Create(center_x, center_y - 88), rotation),
        RotatePoint(median, Point.Create(center_x - 7, center_y - 95), rotation),
        RotatePoint(median, Point.Create(center_x + 7, center_y - 95), rotation),
        RotatePoint(median, Point.Create(center_x, center_y - 88), rotation));

      Polyline(track_pen,
        RotatePoint(median, Point.Create(center_x, center_y - 88), rotation),
        RotatePoint(median, Point.Create(median.X, median.Y), rotation));

      ///////////////////////////////////////////////////////////////////////////
      // Draw the CDI

      rotation = course - _direction;

      int dist;
      for (dist = -10; dist < 11; dist += 2)
      {
        if (dist == 0)
          continue;

        Polyline(Pens.GreenPen3,
          RotatePoint(median, Point.Create(center_x + (pixels_per_nm_cdi * dist), center_y - 5), rotation),
          RotatePoint(median, Point.Create(center_x + (pixels_per_nm_cdi * dist), center_y + 5), rotation));
      }

      //RotatePoint(median, Point.Create(), rotation)

      // draw the CDI Marker head next
      Polygon(Pens.Hollow, Colors.Green,
        RotatePoint(median, Point.Create(center_x, center_y - 97), rotation),
        RotatePoint(median, Point.Create(center_x - 6, center_y - 88), rotation),
        RotatePoint(median, Point.Create(center_x + 6, center_y - 88), rotation),
        RotatePoint(median, Point.Create(center_x, center_y - 97), rotation));

      // we now convert the deviation to pixels.
      // 1 degree = 24 pixels
      double cdi_var = pixels_per_nm_cdi * ((double)deviation / 10);

      int cdi = (int)Math.Max(-66, Math.Min(66, Math.Round(cdi_var)));

      Polyline(Pens.GreenPen3,
        RotatePoint(median, Point.Create(center_x, center_y - 98), rotation),
        RotatePoint(median, Point.Create(center_x, center_y - 50), rotation));

      Polyline(Pens.GreenPen3,
        RotatePoint(median, Point.Create(center_x, center_y + 50), rotation),
        RotatePoint(median, Point.Create(center_x, center_y + 98), rotation));

      Polyline(Pens.GreenPen3,
        RotatePoint(median, Point.Create(center_x + cdi, center_y - 48), rotation),
        RotatePoint(median, Point.Create(center_x + cdi, center_y + 48), rotation));

      /////////////////////////////////////////////////////////////////////////////
      //	Draw the heading bug.

      int hdg = heading - _direction;

      Polyline(Pens.MagentaPen,
        RotatePoint(median, Point.Create(center_x - 15, 3), rotation),
        RotatePoint(median, Point.Create(center_x - 5, 3), rotation),
        RotatePoint(median, Point.Create(center_x, 10), rotation),
        RotatePoint(median, Point.Create(center_x + 5, 3), rotation),
        RotatePoint(median, Point.Create(center_x + 15, 3), rotation),
        RotatePoint(median, Point.Create(center_x + 15, 12), rotation),
        RotatePoint(median, Point.Create(center_x - 15, 12), rotation),
        RotatePoint(median, Point.Create(center_x - 15, 3), rotation));

      Polygon(Pens.WhitePen, Colors.White,
        Point.Create(center_x - 5, 0),
        Point.Create(center_x + 5, 0),
        Point.Create(center_x, 10),
        Point.Create(center_x - 5, 0));

      /////////////////////////////////////////////////////////////////////////////
      // Draw the wind direction indicator.
      // it is in the top left of the HSI and has an arrow that is
      // relative to the magnetic heading of the aircraft and the
      // speed/magnetic heading in the form deg/speed so
      // so for a wind of 15 knots at 50 degrees magnetic we would
      // show 050/15.  If the aircraft heading is 240 degrees magnetic we
      // would see a wind vector on the tail of 40 degrees toward the aircraft
      // the wind direction is shown as a yellow triangle around the HSI indicator
      // the text allows for 3 characters with an maximum width of 23 pixels each
      // so the allowance is 69 by 64 pixels

      int relative_wind = _direction + magnetic_variation - _direction;
      while (relative_wind < 0)
        relative_wind += 360;

      // draw the wind first
      Polyline(Pens.YellowPen,
        RotatePoint(median, Point.Create(center_x - 15, 2), relative_wind),
        RotatePoint(median, Point.Create(center_x + 15, 2), relative_wind),
        RotatePoint(median, Point.Create(center_x, 12), relative_wind),
        RotatePoint(median, Point.Create(center_x - 15, 2), relative_wind));

      // now the text in upper left

      string msg = string.Format("{0:3d", _direction + magnetic_variation);

      Extent pixels = TextExtent(font, msg);

      DrawText(font, Colors.Yellow, Colors.Hollow, msg, Point.Create(25 - (pixels.Dx >> 1), 2));

      msg = wind_speed.ToString();
      pixels = TextExtent(font, msg);

      DrawText(font, Colors.Yellow, Colors.Hollow, msg, Point.Create(25 - (pixels.Dx >> 1), 13));

      /////////////////////////////////////////////////////////////////////////////
      // Draw the estimated time to waypoint.
      // drawn in top right as distance/time
      msg = distance_to_waypoint.ToString();
      pixels = TextExtent(font, msg);
      DrawText(font, Colors.Yellow, Colors.Hollow, msg, Point.Create(window_x - 25 - (pixels.Dx >> 1), 2));

      msg = string.Format("{0:2d}:{1:2d}", time_to_waypoint / 60, time_to_waypoint % 60);
      pixels = TextExtent(font, msg);
      DrawText(font, Colors.Yellow, Colors.Hollow, msg, Point.Create(window_x - 25 - (pixels.Dx >> 1), 13));

      if (waypoint_name != null)
      {
        pixels = TextExtent(font, waypoint_name);
        DrawText(font, Colors.Yellow, Colors.Hollow, waypoint_name, Point.Create(window_x - 25 - (pixels.Dx >> 1), 24));
      }
    }
  }
}
