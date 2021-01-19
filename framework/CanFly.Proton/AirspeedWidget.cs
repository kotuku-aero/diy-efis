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
using CanFly;

namespace CanFly.Proton
{
  public sealed class AirspeedWidget : Widget
  {
    private ushort _vs0;
    private ushort _vs1;
    private ushort _vfe;
    private ushort _vno;
    private ushort _vne;
    private ushort _va;
    private ushort _vx;
    private ushort _vy;

    private ushort _pixelsPerUnit;
  
    private float _scale;
    private float _offset;
	
    // this is updated on each window message
    private int _airspeed;
  
    private uint _backgroundColor;
    private uint _textColor;
    private Pen _pen;
    Font  _font;
    bool _drawBorder;
    Font _largeRoller;
    Font _smallRoller;
    
    /// <summary>
    /// Construct an airspeed window
    /// </summary>
    /// <param name="parent">Parent window</param>
    /// <param name="bounds">Area to draw relative to parent</param>
    /// <param name="id">id of this window</param>
    /// <param name="key">registry key with settings in</param>
    public AirspeedWidget(Widget parent, Rect bounds, ushort id, uint key) 
      : base(parent, bounds, id)
    {
      if (!TryRegGetUint16(key, "pix-per-unit", out _pixelsPerUnit))
        _pixelsPerUnit = 10;

      TryRegGetUint16(key, "vs0", out _vs0);
      _vs0 *= _pixelsPerUnit;
      TryRegGetUint16(key, "vs1", out _vs1);
      _vs1 *= _pixelsPerUnit;
      TryRegGetUint16(key, "vfe", out _vfe);
      _vfe *= _pixelsPerUnit;
      TryRegGetUint16(key, "vno", out _vno);
      _vno *= _pixelsPerUnit;
      TryRegGetUint16(key, "vne", out _vne);
      _vne *= _pixelsPerUnit;
      TryRegGetUint16(key, "va", out _va);
      _va *= _pixelsPerUnit;
      TryRegGetUint16(key, "vx", out _vx);
      _vx *= _pixelsPerUnit;
      TryRegGetUint16(key, "vy", out _vy);
      _vy *= _pixelsPerUnit;
  
      // this conversion factor is for knots
      if (TryRegGetFloat(key, "scale", out _scale))
        _scale = 0.5144444445610519f;

      _scale = 1 / _scale;

      if (!TryRegGetFloat(key, "offset", out _offset))
        _offset = 0;

      if (!LookupFont(key, "font", out _font))
      {
        // we always have the neo font.
        OpenFont("neo", 9, out _font);
      }

      if (!LookupFont(key, "large-font", out _largeRoller))
      {
        // we always have the neo font.
        OpenFont("neo", 12, out _largeRoller);
      }

      if (!LookupFont(key, "small-font", out _smallRoller))
      {
        // we always have the neo font.
        OpenFont("neo", 9, out _smallRoller);
      }
      
      if (!LookupColor(key, "back-color", out _backgroundColor))
        _backgroundColor = Colors.Black;

      if (!LookupColor(key, "text-color", out _textColor))
        _textColor = Colors.White;

      if (!LookupPen(key, "pen", out _pen))
        _pen = new Pen(Colors.White, 1, PenStyle.Solid);

      // hook the canbus messages
      AddEventListener(CanFlyID.id_indicated_airspeed, OnIndicatedAirspeed);
      
      InvalidateRect();
    }

    private void OnIndicatedAirspeed(CanFlyMsg msg)
    {
      
      // airspeed is in m/s convert to display value (knots, km, m/h etc.)
      float v = msg.GetFloat();
      
      float airspeed = v * _scale;
      airspeed += _offset;
      
      int value = (int)(Math.Round(airspeed));

      if (airspeed != value)
      {
        _airspeed = value;
        InvalidateRect();
      }
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wndRect = WindowRect;
      Extent ex = Extents;

      Rectangle(null, _backgroundColor, new Rect(8, 8, ex.Dx - 9, ex.Dy - 8));

      int median = ex.Dy >> 1;

      // the vertical tape displays 28 knots around the current position
      // as there are 240 pixels.  We calc the upper one first
      int topAsi = ((int) _airspeed * _pixelsPerUnit) + median - _pixelsPerUnit;

      // assign the first line airspeed
      int asiLine = (topAsi / 25) * 25;
      int markerLine;
      int width = wndRect.Width;
      // work out how many lines to the next lowest marker
      for (markerLine = (topAsi - asiLine) + _pixelsPerUnit; markerLine < ex.Dy; markerLine += 25)
      {
        // draw a line from 10 pixels to 30 pixels then the text.
        // lines at 25 are shorter
        Point[] pts =
        {
          new Point(asiLine == ((asiLine / 50) * 50) ? width - 20 : width - 18, markerLine),
          new Point(width - 13, markerLine)
        };

        Polyline(_pen, pts);

        if (asiLine == ((asiLine / 100) * 100))
        {
          string str = ((int) asiLine / _pixelsPerUnit).ToString();
          Extent size = TextExtent(_font, str);

          DrawText(_font, _textColor, _backgroundColor,
            str, new Point(width - 20 - size.Dx, markerLine - (size.Dy >> 1)),
            null, TextOutStyle.Clipped);
        }

        asiLine -= 25;

        if (asiLine < 0)
          break;
      }

      // finally draw the markers that indicate the v-speeds
      // each knot is 10 pixels in the y direction
      // the scale is 240 pixels high. 

      int vnePixels = (topAsi - _vne + _pixelsPerUnit);
      int vnoPixels = (topAsi - _vno + _pixelsPerUnit);
      int vaPixels = (topAsi - _va + _pixelsPerUnit);
      int vfePixels = (topAsi - _vfe + _pixelsPerUnit);
      int vs0Pixels = (topAsi - _vs0 + _pixelsPerUnit); // stall flaps extended
      int vs1Pixels = (topAsi - _vs1 + _pixelsPerUnit); // stall flaps up
      int vxPixels = (topAsi - _vx + _pixelsPerUnit); // best angle of climb
      int vyPixels = (topAsi - _vy + _pixelsPerUnit); // best rate of climb

      int bar0 = width - 12;
      int bar1 = width - 8;
      int bar2 = width - 4;

      // draw vne exceeded
      if (vnePixels >= 8)
        Rectangle(null, Colors.Red, new Rect(bar2, 8, bar2 + 4, Math.Min((int) ex.Dy - 8, vnePixels)));

      // draw vne->vno
      if (vnoPixels >= (int) 8 && vnePixels < (int) ex.Dy - 8)
        Rectangle(null, Colors.Yellow,
          new Rect(bar2, Math.Max((int) 8, vnePixels), bar2 + 4,
            Math.Min((int) ex.Dy - 8, vnoPixels)));

      // draw vno->vs1
      if (vs1Pixels >= 8 && vnoPixels < 232)
        Rectangle(null, Colors.Green,
          new Rect(bar2, Math.Max((int) 8, vnoPixels),
            bar2 + 4, Math.Min((int) ex.Dy - 8, vs1Pixels)));

      // draw vfe->vs0
      if (vs0Pixels >= 8 && vfePixels < 232)
        Rectangle(null, Colors.White,
          new Rect(bar1, Math.Max((int) 8, vfePixels),
            bar1 + 4, Math.Min((int) ex.Dy - 8, vs0Pixels)));

      // draw vy -> vx
      if (vxPixels >= 8 && vyPixels < 232)
        Rectangle(null, Colors.Blue,
          new Rect(bar0, Math.Max((int) 8, vyPixels), bar0 + 4,
            Math.Min((int) ex.Dy - 8, vxPixels)));


      Point[] roller =
      {
        new Point(width - 13, median),
        new Point(width - 20, median + 7),
        new Point(width - 20, median + 20),
        new Point(0, median + 20),
        new Point(0, median - 20),
        new Point(width - 20, median - 20),
        new Point(width - 20, median - 7),
        new Point(width - 13, median)
      };

      Polygon(Pens.WhitePen, Colors.Black, roller);

      // now we draw the roller
      DisplayRoller(new Rect(1, median - 19, width - 20, median + 19),
        _airspeed, 1, Colors.Black, Colors.White,
        _largeRoller, _smallRoller);

      EndPaint();
    }

    public ushort Vs0
    {
      get { return _vs0; }
      set { _vs0 = value; }
    }

    public ushort Vs1
    {
      get { return _vs1; }
      set { _vs1 = value; }
    }

    public ushort Vfe
    {
      get { return _vfe; }
      set { _vfe = value; }
    }

    public ushort Vno
    {
      get { return _vno; }
      set { _vno = value; }
    }

    public ushort Vne
    {
      get { return _vne; }
      set { _vne = value; }
    }

    public ushort Va
    {
      get { return _va; }
      set { _va = value; }
    }

    public ushort Vx
    {
      get { return _vx; }
      set { _vx = value; }
    }

    public ushort Vy
    {
      get { return _vy; }
      set { _vy = value; }
    }
  }
}
