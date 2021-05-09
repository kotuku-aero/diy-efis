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
  
    private Color _backgroundColor;
    private Color _textColor;
    private Pen _pen;
    Font  _font;
    bool _drawBorder;
    Font _largeRollerFont;
    Font _smallRollerFont;

    /// <summary>
    /// Constructor that does not set any defaults.
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="bounds"></param>
    /// <param name="id"></param>
    public AirspeedWidget(Widget parent, Rect bounds, ushort id) 
      : base(parent, bounds, id)
      {
      _pixelsPerUnit = 10;

      Scale = 0.5144444445610519f;

      // hook the canbus messages
      AddCanFlyEvent(CanFlyID.id_indicated_airspeed, OnIndicatedAirspeed);
      
      InvalidateRect();
      }


    private void OnIndicatedAirspeed(CanFlyMsg msg)
    {
      
      // airspeed is in m/s convert to display value (knots, km, m/h etc.)
      float v = msg.GetFloat();
      
      float airspeed = v / Scale;
      airspeed += Offset;
      
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

      Rectangle(Pens.Hollow, _backgroundColor, Rect.Create(8, 8, ex.Dx - 9, ex.Dy - 8));

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
          Point.Create(asiLine == ((asiLine / 50) * 50) ? width - 20 : width - 18, markerLine),
          Point.Create(width - 13, markerLine)
        };

        Polyline(_pen, pts);

        if (asiLine == ((asiLine / 100) * 100))
        {
          string str = ((int) asiLine / _pixelsPerUnit).ToString();
          Extent size = TextExtent(_font, str);

          DrawText(_font, _textColor, _backgroundColor,
            str, Point.Create(width - 20 - size.Dx, markerLine - (size.Dy >> 1)));
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
        Rectangle(Pens.Hollow, Colors.Red, Rect.Create(bar2, 8, bar2 + 4, Math.Min((int) ex.Dy - 8, vnePixels)));

      // draw vne->vno
      if (vnoPixels >= (int) 8 && vnePixels < (int) ex.Dy - 8)
        Rectangle(Pens.Hollow, Colors.Yellow,
          Rect.Create(bar2, Math.Max((int) 8, vnePixels), bar2 + 4, Math.Min((int) ex.Dy - 8, vnoPixels)));

      // draw vno->vs1
      if (vs1Pixels >= 8 && vnoPixels < 232)
        Rectangle(Pens.Hollow, Colors.Green,
          Rect.Create(bar2, Math.Max((int) 8, vnoPixels), bar2 + 4, Math.Min((int) ex.Dy - 8, vs1Pixels)));

      // draw vfe->vs0
      if (vs0Pixels >= 8 && vfePixels < 232)
        Rectangle(Pens.Hollow, Colors.White,
          Rect.Create(bar1, Math.Max((int) 8, vfePixels),
            bar1 + 4, Math.Min((int) ex.Dy - 8, vs0Pixels)));

      // draw vy -> vx
      if (vxPixels >= 8 && vyPixels < 232)
        Rectangle(Pens.Hollow, Colors.Blue,
          Rect.Create(bar0, Math.Max((int) 8, vyPixels), bar0 + 4,
            Math.Min((int) ex.Dy - 8, vxPixels)));


      Point[] roller =
      {
        Point.Create(width - 13, median),
        Point.Create(width - 20, median + 7),
        Point.Create(width - 20, median + 20),
        Point.Create(0, median + 20),
        Point.Create(0, median - 20),
        Point.Create(width - 20, median - 20),
        Point.Create(width - 20, median - 7),
        Point.Create(width - 13, median)
      };

      Polygon(Pens.WhitePen, Colors.Black, roller);

      // now we draw the roller
      DisplayRoller(Rect.Create(1, median - 19, width - 20, median + 19),
        _airspeed, 1, Colors.Black, Colors.White,
        _largeRollerFont, _smallRollerFont);

      EndPaint();
    }
    /// <summary>
    /// Speed in knots to display as vs0
    /// </summary>
    /// <value></value>
    public ushort Vs0
    {
      get { return _vs0; }
      set { _vs0 = value; }
    }
    /// <summary>
    /// Speed in knots to display as vs1
    /// </summary>
    /// <value></value>
    public ushort Vs1
    {
      get { return _vs1; }
      set { _vs1 = value; }
    }
    /// <summary>
    /// Speed to display in knots as Vfe
    /// </summary>
    /// <value></value>
    public ushort Vfe
    {
      get { return _vfe; }
      set { _vfe = value; }
    }
    /// <summary>
    /// Speed to display in knots as Vno
    /// </summary>
    /// <value></value>
    public ushort Vno
    {
      get { return _vno; }
      set { _vno = value; }
    }
    /// <summary>
    /// Speed to display in knots as Vne
    /// </summary>
    /// <value></value>
    public ushort Vne
    {
      get { return _vne; }
      set { _vne = value; }
    }
    /// <summary>
    /// Speed to display in knots as Va
    /// </summary>
    /// <value></value>
    public ushort Va
    {
      get { return _va; }
      set { _va = value; }
    }
    /// <summary>
    /// Speed to display in knots as Vx
    /// </summary>
    /// <value></value>
    public ushort Vx
    {
      get { return _vx; }
      set { _vx = value; }
    }
    /// <summary>
    /// Speed to display in knots as Vy
    /// </summary>
    /// <value></value>
    public ushort Vy
    {
      get { return _vy; }
      set { _vy = value; }
    }
    /// <summary>
    /// Scale to adjust the airspeed by.
    /// </summary>
    /// <value></value>
    public float Scale
    {
      get { return _scale; }
      set { _scale = value; }
    }
    /// <summary>
    /// Scale to offset the airspeed by
    /// </summary>
    /// <value></value>
    public float Offset
    {
      get { return _offset; }
      set { _offset = value; }
    }

    public Color BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
    }

    public Color TextColor
    {
      get { return _textColor; }
      set { _textColor = value; }
    }

    public Pen Pen
    {
      get { return _pen; }
      set { _pen = value; }
    }

    public Font Font
    {
      get { return _font; }
      set { _font = value; }
    }

    public bool DrawBorder
    {
      get { return _drawBorder; }
      set { _drawBorder = value; }
    }

    public Font LargeRollerFont
    {
      get { return _largeRollerFont; }
      set { _largeRollerFont = value; }
    }

    public Font SmallRollerFont
    {
      get { return _smallRollerFont; }
      set { _smallRollerFont = value; }
    }
  }
}
