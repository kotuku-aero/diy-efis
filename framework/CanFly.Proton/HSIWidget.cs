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
    private short _course;
    private short _deviation;
    private short _track;
    private short _distanceToWaypoint;
    private short _timeToWaypoint;
    private short _windDirection; // always true
    private short _magneticVariation;
    private short _windSpeed;
    private short _headingBug;
    private short _heading;
    private Color _backgroundColor;
    private bool _drawBorder;
    private Font _font;
    private string _waypointName;

    public short Direction { get { return _direction; } }
    public short Course { get { return _course; } }
    public short Deviation { get { return _deviation; } }
    public short Track { get { return _track; } }
    public short DistanceToWaypoint { get { return _distanceToWaypoint; } }
    public short MagneticVariation { get { return _magneticVariation; } }
    public short WindSpeed { get { return _windSpeed; } }
    public short HeadingBug { get { return _headingBug; } }
    public short Heading { get { return _heading; } }
    public Color BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
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
    public string WaypointName
    {
      get { return _waypointName; }
      set { _waypointName = value; }
    }

    public short WindDirection { get { return _windDirection; } }

    internal HSIWidget(Widget parent, Rect bounds, ushort id)
    : base(parent, bounds, id)
    {
      BackgroundColor = Colors.Black;
      DrawBorder = true;
      OpenFont("neo", 9, out _font);

      AddCanFlyEvent(CanFlyID.id_magnetic_heading, OnMagneticHeading);
      AddCanFlyEvent(CanFlyID.id_heading, OnHeading);
      AddCanFlyEvent(CanFlyID.id_heading_angle, OnHeadingAngle);
      AddCanFlyEvent(CanFlyID.id_deviation, OnDeviation);
      AddCanFlyEvent(CanFlyID.id_selected_course, OnSelectedCourse);
      AddCanFlyEvent(CanFlyID.id_track, OnTrack);
      AddCanFlyEvent(CanFlyID.id_wind_speed, OnWindSpeed);
      AddCanFlyEvent(CanFlyID.id_wind_direction, OnWindDirection);
      AddCanFlyEvent(CanFlyID.id_distance_to_next, OnDistanceToNext);
      AddCanFlyEvent(CanFlyID.id_magnetic_variation, OnMagneticVariation);
      AddCanFlyEvent(CanFlyID.id_estimated_time_to_next, OnEstimatedTimeToNext);

      InvalidateRect();
    }

    private void OnEstimatedTimeToNext(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (_timeToWaypoint != value)
      {
        _timeToWaypoint = value;
        InvalidateRect();
      }
    }

    private void OnMagneticVariation(CanFlyMsg e)
    {
      float v = e.GetFloat();
      short value = (short)RadiansToDegrees(v);
      if (_magneticVariation != value)
      {
        _magneticVariation = value;
        InvalidateRect();
      }
    }

    private void OnDistanceToNext(CanFlyMsg e)
    {
      float v = e.GetFloat();
      
      short value = (short)MetersToNM(v);
      if (_distanceToWaypoint != value)
      {
        _distanceToWaypoint = value;
        InvalidateRect();
      }
    }

    private void OnWindDirection(CanFlyMsg e)
    {
      float v = e.GetFloat();
      short value = (short)RadiansToDegrees(v);
      if (_windDirection != value)
      {
        _windDirection = value;
        InvalidateRect();
      }
    }

    private void OnWindSpeed(CanFlyMsg e)
    {
      float v = e.GetFloat();
      short value = (short)MetersPerSecondToKnots(v);
      if (_windSpeed != value)
      {
        _windSpeed = value;
        InvalidateRect();
      }
    }

    private void OnTrack(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (_track != value)
      {
        _track = value;
        InvalidateRect();
      }
    }

    private void OnSelectedCourse(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (_course != value)
      {
        _course = value;
        InvalidateRect();
      }
    }

    private void OnDeviation(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (_deviation != value)
      {
        _deviation = value;
        // the deviation is +/- * 10
        InvalidateRect();
      }
    }

    private void OnHeadingAngle(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (_heading != value)
      {
        _heading = value;
        InvalidateRect();
      }
    }

    private void OnHeading(CanFlyMsg e)
    {
      short value = e.GetInt16();
      if (_headingBug != value)
      {
        _headingBug = value;
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

      Rectangle(Pens.Hollow, BackgroundColor, WindowRect);

      if (DrawBorder)
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
      for (index = -Direction; i < 12; index += 30, i++)
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

        DrawText(Font, Colors.White, Colors.Black, i.ToString(),
          Point.Create(fontPt.X - font_center, fontPt.Y - font_center));
      }

      ///////////////////////////////////////////////////////////////////////////
      // Draw the Track

      int rotation = Track - Direction;

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

      rotation = Course - Direction;

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
      double cdi_var = pixels_per_nm_cdi * ((double)Deviation / 10);

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

      int hdg = Heading - Direction;

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

      int relative_wind = Direction + MagneticVariation - Direction;
      while (relative_wind < 0)
        relative_wind += 360;

      // draw the wind first
      Polyline(Pens.YellowPen,
        RotatePoint(median, Point.Create(center_x - 15, 2), relative_wind),
        RotatePoint(median, Point.Create(center_x + 15, 2), relative_wind),
        RotatePoint(median, Point.Create(center_x, 12), relative_wind),
        RotatePoint(median, Point.Create(center_x - 15, 2), relative_wind));

      // now the text in upper left

      string msg = string.Format("{0:3d", Direction + MagneticVariation);

      Extent pixels = TextExtent(Font, msg);

      DrawText(Font, Colors.Yellow, Colors.Hollow, msg, Point.Create(25 - (pixels.Dx >> 1), 2));

      msg = WindSpeed.ToString();
      pixels = TextExtent(Font, msg);

      DrawText(Font, Colors.Yellow, Colors.Hollow, msg, Point.Create(25 - (pixels.Dx >> 1), 13));

      /////////////////////////////////////////////////////////////////////////////
      // Draw the estimated time to waypoint.
      // drawn in top right as distance/time
      msg = DistanceToWaypoint.ToString();
      pixels = TextExtent(Font, msg);
      DrawText(Font, Colors.Yellow, Colors.Hollow, msg, Point.Create(window_x - 25 - (pixels.Dx >> 1), 2));

      msg = string.Format("{0:2d}:{1:2d}", _timeToWaypoint / 60, _timeToWaypoint % 60);
      pixels = TextExtent(Font, msg);
      DrawText(Font, Colors.Yellow, Colors.Hollow, msg, Point.Create(window_x - 25 - (pixels.Dx >> 1), 13));

      if (WaypointName != null)
      {
        pixels = TextExtent(Font, WaypointName);
        DrawText(Font, Colors.Yellow, Colors.Hollow, WaypointName, Point.Create(window_x - 25 - (pixels.Dx >> 1), 24));
      }
    }
  }
}
