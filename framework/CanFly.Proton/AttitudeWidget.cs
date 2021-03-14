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
  public class AttitudeWidget : Widget
  {
    static readonly int pixels_per_degree = 49;

    private int _pitch;
    private int _roll;
    private int _yaw;
    private ushort _AOADegrees;
    private float _AOAPixelsPerDegree;
    private float _AOADegreesPerMark;
    private short _glideslope;
    private short _localizer;
    private Point _widgetMedian;
    private bool _glideslopeAquired;
    private ushort _criticalAOA;
    private ushort _approachAOA;
    private ushort _climbAOA;
    private ushort _cruiseAOA;
    private ushort _yawMax;
    private bool _showAOA;
    private bool _showGlideslope;
    private bool _localizerAquired;
    private Font _font;

    public AttitudeWidget(Widget parent, Rect bounds, ushort id, ushort key)
  : base(parent, bounds, id)
    {
      TryRegGetUint16(key, "critical-aoa", out _criticalAOA);
      TryRegGetUint16(key, "approach-aoa",  out _approachAOA);
      TryRegGetUint16(key, "climb-aoa", out _climbAOA);
      TryRegGetUint16(key, "cruise-aoa", out _cruiseAOA);

      if (!TryRegGetUint16(key, "yaw-max", out _yawMax))
        _yawMax = 45;

      TryRegGetBool(key, "show-aoa", out _showAOA);
      TryRegGetBool(key, "show-gs", out _showGlideslope);

      _AOADegrees = _cruiseAOA;

      // aoa is 40 pixels
      _AOAPixelsPerDegree = (float) 40.0 / (_criticalAOA - _cruiseAOA);
      _AOADegreesPerMark = (float)((_criticalAOA - _cruiseAOA) / 8.0);

      Rect wnd_rect = WindowRect;

      _widgetMedian = new Point();

      short value;
      if (TryRegGetInt16(key, "center-x", out value))
        _widgetMedian.X = value;
      else
        _widgetMedian.X = wnd_rect.Width >> 1;

      if (TryRegGetInt16(key, "center-y", out value))
        _widgetMedian.Y = value;
      else
        _widgetMedian.Y = wnd_rect.Height >> 1;

      if (!LookupFont(key, "font", out _font))
      {
        // we always have the neo font.
        OpenFont("neo", 9, out _font);
      }

      AddEventListener(CanFlyID.id_yaw_angle, OnYawAngle);
      AddEventListener(CanFlyID.id_roll_angle, OnRollAngle);
      AddEventListener(CanFlyID.id_pitch_angle, OnPitchAngle);
    }

    private void OnPitchAngle(CanFlyMsg e)
    {
      float v = e.GetFloat();

      int angle = (int)RadiansToDegrees(v);

      while (angle < -180)
        angle += 180;

      while (angle > 180)
        angle -= 180;

      // angle is now within range
      if (angle > 90)
        angle = 180 - angle;

      else if (angle < -90)
        angle = -180 - angle;

      if(_pitch != angle)
      {
        _pitch = angle;
        InvalidateRect();
      }  
    }

    private void OnRollAngle(CanFlyMsg e)
    {
      float v = e.GetFloat();

      int value = (int)RadiansToDegrees(v);
      while (value > 179)
        value -= 360;

      while (value < -180)
        value += 360;

      if (value > 90 || value < -90)
        value -= 180;

      while (value < -180)
        value += 360;

      value = Math.Min(90, Math.Max(-90, value));

      if (_roll != value)
      {
        _roll = value;
        InvalidateRect();
      }
    }

    private void OnYawAngle(CanFlyMsg e)
    {
      float v = e.GetFloat();

      short value = (short)RadiansToDegrees(v);
      while (value > 179)
        value -= 360;

      while (value < -180)
        value += 360;

      if(_yaw != value)
      {
        _yaw = value;
        InvalidateRect();
      }
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wnd_rect = WindowRect;
      // first step is to draw the background.  Then we can rotate it correctly
      // the background is 240x240 pixels with 20 pixels being the median line

      // the display is +/- 21.2132 degrees so we round to +/- 20 degrees
      // or 400 pixels
      int pitch = _pitch;

      // pitch is now +/- 90
      // limit to 25 degrees as we don't need any more than that
      pitch = Math.Min(25, Math.Max(-25, pitch));

      // make this == pixels that is the azimuth line
      pitch *= 10;

      // draw the upper area
      Polygon(null, Colors.LightBlue,
        RotatePoint(_widgetMedian, new Point(-500, _widgetMedian.Y - 500 + pitch), _roll),
        RotatePoint(_widgetMedian, new Point(500, _widgetMedian.Y - 500 + pitch), _roll),
        RotatePoint(_widgetMedian, new Point(500, _widgetMedian.Y + pitch), _roll),
        RotatePoint(_widgetMedian, new Point(-500, _widgetMedian.Y + pitch), _roll),
        RotatePoint(_widgetMedian, new Point(-500, _widgetMedian.Y - 500 + pitch), _roll));

      // draw the lower area
      Polygon(null, Colors.Brown,
        RotatePoint(_widgetMedian, new Point(-500, _widgetMedian.Y + pitch), _roll),
        RotatePoint(_widgetMedian, new Point(500, _widgetMedian.Y + pitch), _roll),
        RotatePoint(_widgetMedian, new Point(500, _widgetMedian.Y + pitch + 500), _roll),
        RotatePoint(_widgetMedian, new Point(-500, _widgetMedian.Y + pitch + 500), _roll),
        RotatePoint(_widgetMedian, new Point(-500, _widgetMedian.Y + pitch), _roll));

      /////////////////////////////////////////////////////////////////////////////
      //	Draw the pitch indicator

      //background_color(Colors.Black);
      //pen(Pens.WhitePen);

      /////////////////////////////////////////////////////////////////////////////
      // we now draw the image of the bank angle marks
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 104, 60), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 94, 66), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 60, 17), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 54, 27), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 40, 11), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 37, 19), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 20, 6), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 18, 14), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 20, 6), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 18, 14), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 39, 11), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 36, 19), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 59, 17), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 53, 27), _roll));
      Polyline(Pens.GreenPen, RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 103, 60), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 93, 66), _roll));
      Polyline(Pens.GreenPen3, RotatePoint(_widgetMedian, new Point(_widgetMedian.X, 0), _roll), RotatePoint(_widgetMedian, new Point(_widgetMedian.X, 12), _roll));

      /////////////////////////////////////////////////////////////////////////////
      //	Draw the rotated roll/pitch indicator

      // The window height is 240 pixels, and each 25 deg of pitch
      // is 20 pixels.  So the display is +/- 150 degrees (120/20)*25
      //
      int pitch_range = (_widgetMedian.Y / 20) * 25;
      int pitch_angle = ((int)(pitch * 10)) + pitch_range;
      int line = 0;

      // now we draw the pitch line(s)
      if ((pitch_angle % 25) == 0)
      {
        int new_pitch = ((pitch_angle / 25) + 1) * 25;
        line = pitch_angle - new_pitch;
        pitch_angle = new_pitch;
      }

      while (line < wnd_rect.Bottom)
      {
        if (pitch_angle == 3600 || pitch_angle == 0)
        {
          pitch_angle -= 25;
          line += 20;
        }
        else if ((pitch_angle % 100) == 0)
        {
          Polyline(Pens.WhitePen,
            RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 40, line), _roll),
          RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 40, line), _roll));

          // we have a bitmap which is the text to draw.  We then select the bitmap
          // from the text angle and the rotation angle.
          // the text is 19x19 pixels

          // calc the text angle as 10, 20..90
          int text_angle = Math.Abs(pitch_angle / 100);

          // only draw the angle if the text is != 0
          if (text_angle > 0)
          {
            // calc the left/right center point
            Point pt_left = RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 48, line), _roll);
            Point pt_right = RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 47, line), _roll);

            // we now calc the left and right points to write the text to
            pt_left.X -= 9; pt_left.Y -= 9;
            pt_right.X -= 9; pt_right.Y -= 9;

            string txt = text_angle.ToString();
            DrawText(_font, Colors.White, Colors.Hollow, txt, pt_left, wnd_rect, TextOutStyle.Clipped);
            DrawText(_font, Colors.White, Colors.Hollow, txt, pt_right, wnd_rect, TextOutStyle.Clipped);
          }
          pitch_angle -= 25;
          line += 20;
        }
        else if ((pitch_angle % 50) == 0)
        {
          Polyline(Pens.WhitePen,
            RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 26, line), _roll),
            RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 25, line), _roll));

          pitch_angle -= 25;
          line += 20;
        }
        else
        {
          Polyline(Pens.WhitePen,
            RotatePoint(_widgetMedian, new Point(_widgetMedian.X - 12, line), _roll),
            RotatePoint(_widgetMedian, new Point(_widgetMedian.X + 12, line), _roll));

          pitch_angle -= 25;
          line += 20;
        }
      }

      int offset;
      int pixels;

      /////////////////////////////////////////////////////////////////////////////
      // Draw the angle-of-attack indicator
      //
      // this is 40 pixels up/down

      if (_showAOA)
      {
        // calc the effective AOA
        int aoa = Math.Min(_criticalAOA, Math.Max(_cruiseAOA, _AOADegrees));
        aoa -= _cruiseAOA;
        pixels = (int)(aoa * _AOAPixelsPerDegree);

        float aoa_marker = _criticalAOA;
        for (offset = 60; offset > 0; offset -= 6)
        {
          if (aoa_marker > _approachAOA)
          {
            // draw red chevron.
            Polyline(Pens.RedPen3,
              new Point(_widgetMedian.X - 15, pixels + _widgetMedian.Y - offset),
              new Point(_widgetMedian.X, pixels + _widgetMedian.Y - offset + 4),
              new Point(_widgetMedian.X + 15, pixels + _widgetMedian.Y - offset));
          }
          else if (aoa_marker > _climbAOA)
          {
            Polyline(Pens.YellowPen3,
              new Point(_widgetMedian.X - 15, pixels + _widgetMedian.Y - offset),
              new Point(_widgetMedian.X + 15, pixels + _widgetMedian.Y - offset));
          }
          else
          {
            Polyline(Pens.GreenPen3,
              new Point(_widgetMedian.X - 15, pixels + _widgetMedian.Y - offset),
              new Point(_widgetMedian.X + 15, pixels + _widgetMedian.Y - offset));

            aoa_marker -= _AOADegreesPerMark;
          }
        }

        /////////////////////////////////////////////////////////////////////////////
        // draw the aircraft image
        Polyline(Pens.WhitePen,
          new Point(_widgetMedian.X - 7, _widgetMedian.Y),
          new Point(_widgetMedian.X - 22, _widgetMedian.Y));

        Polyline(Pens.WhitePen,
          new Point(_widgetMedian.X + 7, _widgetMedian.Y),
          new Point(_widgetMedian.X + 22, _widgetMedian.Y));

        Polyline(Pens.WhitePen,
          new Point(_widgetMedian.X, _widgetMedian.Y - 7),
          new Point(_widgetMedian.X, _widgetMedian.Y - 15));

        Ellipse(Pens.WhitePen, Colors.Hollow,
          new Rect(_widgetMedian.X - 7, _widgetMedian.Y - 7, _widgetMedian.X + 7, _widgetMedian.Y + 7));

        /////////////////////////////////////////////////////////////////////////////
        // draw the glide slope and localizer indicators
        if (_glideslopeAquired && _showGlideslope)
        {
          // draw the marker, 0.7 degrees = 59 pixels
          double deviation = Math.Max(-1.2, Math.Min(1.2, _glideslope / 100.0));

          pixels = (int)(deviation / (1.0 / pixels_per_degree));

          pixels += 120;

          Polyline(Pens.WhitePen,
            new Point(wnd_rect.Right - 12, _widgetMedian.Y),
            new Point(wnd_rect.Right, _widgetMedian.Y));


          // rest are hollow
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(230, 57, 238, 65));
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(230, 86, 238, 94));
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(230, 146, 238, 154));
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(230, 175, 238, 183));

          // black filled ellipse
          Ellipse(Pens.WhitePen, Colors.Black,
            new Rect(230, pixels - 4, 238, pixels + 4));

        }

        if (_localizerAquired && _showGlideslope)
        {
          // draw the marker, 1.0 degrees = 74 pixels
          double deviation = Math.Max(-1.2, Math.Min(1.2, _localizer / 100.0));

          pixels = (int)(deviation / (1.0 / pixels_per_degree));

          pixels += _widgetMedian.X;

          Polyline(Pens.WhitePen3,
            new Point(_widgetMedian.X, _widgetMedian.Y - 15),
            new Point(_widgetMedian.X, _widgetMedian.Y));

          // rest are hollow
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(57, 230, 65, 238));
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(86, 230, 94, 238));
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(146, 230, 154, 238));
          Ellipse(Pens.WhitePen, Colors.Hollow, new Rect(175, 230, 183, 238));

          // black filled ellipse
          Ellipse(Pens.WhitePen, Colors.Black,
            new Rect(pixels - 4, 230, pixels + 4, 238));

        }

        /////////////////////////////////////////////////////////////////////////////
        // Draw the roll indicator
        // Draw the aircraft pointer at the top of the window.

        // the roll indicator is shifted left/right by the yaw angle,  1degree = 2pix
        offset = Math.Min(_yawMax, Math.Max(-_yawMax, _yaw << 1));

        Polygon(Pens.WhitePen, Colors.Hollow,
          new Point(_widgetMedian.X + offset, 12),
          new Point(_widgetMedian.X + offset - 6, 23),
          new Point(_widgetMedian.X + offset + 6, 23),
          new Point(_widgetMedian.X + offset, 12));

        Polygon(Pens.WhitePen, Colors.Hollow,
          new Point(_widgetMedian.X - 6, 23),
          new Point(_widgetMedian.X + 6, 23),
          new Point(_widgetMedian.X + 9, 27),
          new Point(_widgetMedian.X - 9, 27),
          new Point(_widgetMedian.X - 6, 23));
      }

      EndPaint();
    }
  }
}
