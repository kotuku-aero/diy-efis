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
  public sealed class AnnunciatorWidget : Widget
  {
    private bool _drawBorder;
    private uint _hours; // hobbs hours, stored in AHRS as hours * 100
    private ushort _qnh; // qnh, stored in AHRS
    private ushort _clock;
    private short _oat;
    private ushort _cas;

    private Font _smallFont;
    private Font _largeFont;

    private Point _clockPt = Point.Create(1, 10 );
    private Point _hrsPt = Point.Create(1, 55 );
    private Point _qnhPt = Point.Create(1, 100 );
    private Point _oatPt = Point.Create(1, 145 );
    private Point _casPt = Point.Create(1, 190 );

    private readonly string hours_value = "TTS";
    private readonly string qnh_value = "QNH";

    public bool DrawBorder
    {
      get { return _drawBorder; }
      set { _drawBorder = value; }
    }
    public uint Hours
    {
      get { return _hours; }
    }
    public ushort Qnh
    {
      get { return _qnh; }
    }
    public ushort Clock
    {
      get { return _clock; }
    }
    public short Oat { get { return _oat; } }
    public ushort Cas { get { return _cas; } }
    public Font SmallFont
    {
      get { return _smallFont; }
      set { _smallFont = value; }
    }
    public Font LargeFont
    {
      get { return _largeFont; }
      set { _largeFont = value; }
    }
    public Point ClockPt
    {
      get { return _clockPt; }
      set { _clockPt = value; }
    }
    public Point HrsPt
    {
      get { return _hrsPt; }
      set { _hrsPt = value; }
    }
    public Point QnhPt
    {
      get { return _qnhPt; }
      set { _qnhPt = value; }
    }
    public Point OatPt
    {
      get { return _oatPt; }
      set { _oatPt = value; }
    }
    public Point CasPt
    {
      get { return _casPt; }
      set { _casPt = value; }
    }

    public AnnunciatorWidget(Widget parent, Rect bounds, ushort id)
  : base(parent, bounds, id)
    {
      AddCanFlyEvent(CanFlyID.id_def_utc, OnDefUtc);
      AddCanFlyEvent(CanFlyID.id_qnh, OnQnh);
      AddCanFlyEvent(CanFlyID.id_true_airspeed, OnTrueAirspeed);
      AddCanFlyEvent(CanFlyID.id_outside_air_temperature, OnOutsideAirTemperature);
      AddCanFlyEvent(CanFlyID.id_air_time, OnAirTime);
    }

    private void OnAirTime(CanFlyMsg e)
    {
      uint value = e.GetUInt32();

      if(_hours != value)
      {
        _hours = value;
        InvalidateRect();
      }
    }

    private void OnOutsideAirTemperature(CanFlyMsg e)
    {
      short value = e.GetInt16();

      if(_oat != value)
      {
        _oat = value;
        InvalidateRect();
      }
    }

    private void OnTrueAirspeed(CanFlyMsg e)
    {
      ushort value = e.GetUInt16();
      if(_cas != value)
      {
        _cas = value;
        InvalidateRect();
      }
    }

    private void OnQnh(CanFlyMsg e)
    {
      ushort value = e.GetUInt16();

      if(_qnh != value)
      {
        _qnh = value;
        InvalidateRect();
      }
    }

    private void OnDefUtc(CanFlyMsg e)
    {
      byte[] values = e.GetUInt8Array();
      if(values.Length == 3)
      {
        ushort minutes = 0;
        minutes += (ushort)(values[0] * 60);
        minutes += (ushort)(values[1]);

        if(_clock != minutes)
        {
          _clock = minutes;
          InvalidateRect();
        }
      }
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wnd_rect = WindowRect;

      if (DrawBorder)
        RoundRect(Pens.WhitePen, Colors.Hollow, wnd_rect, 12);

      DrawAnnunciator(wnd_rect, ClockPt, "UTC", string.Format("{0:d2}:{1:d2}", Clock / 60, Clock % 60));
      DrawAnnunciator(wnd_rect, HrsPt, "Hrs", string.Format("{0:f6.1", ((double)Hours) / 100));
      DrawAnnunciator(wnd_rect, QnhPt, "QNH", string.Format("{0:d}", Qnh));
      DrawAnnunciator(wnd_rect, OatPt, "OAT", string.Format("{0:d}", Oat));
      DrawAnnunciator(wnd_rect, CasPt, "CAS", string.Format("{0:d}", Cas));

      EndPaint();
    }

    private void DrawAnnunciator(Rect wnd_rect, Point pt, string label, string value)
    {
      // calculate the size of the label
      Rect clip_rect = Rect.Create(pt.X, pt.Y, pt.X + 75, pt.Y + 50 );

      Extent label_size = TextExtent(SmallFont, label);

      int width = wnd_rect.Width;
      int text_start = pt.X + width - (label_size.Dx + 3);
      int right = pt.X + width;

      Rectangle(Pens.Hollow, Colors.Gray, Rect.Create(pt.X, pt.Y + 1, right, pt.Y + 4));
      Rectangle(Pens.Hollow, Colors.Gray, Rect.Create(pt.X, pt.Y + 4, pt.X + 3, pt.Y + 28));
      Rectangle(Pens.Hollow, Colors.Gray, Rect.Create(right - 3, pt.Y + 4, right, pt.Y + 45));
      Rectangle(Pens.Hollow, Colors.Gray, Rect.Create(pt.X, pt.Y + 28, right, pt.Y + 31));
      Rectangle(Pens.Hollow, Colors.Gray, Rect.Create(text_start - 6, pt.Y + 31, right, pt.Y + 45));

      Rectangle(Pens.Hollow, Colors.Black, Rect.Create(pt.X + 3, pt.Y + 4, right - 3, pt.Y + 28));

      Point label_origin = Point.Create(text_start, pt.Y + 30 );

      Rect txtRect = Rect.Create(text_start, pt.Y + 30, text_start + label_size.Dx, pt.Y + 30 + label_size.Dy);

      DrawText(SmallFont, Colors.White, Colors.Black, label, label_origin, txtRect, TextOutStyle.Clipped);

      Extent text_size = TextExtent(LargeFont, value);

      // center of the text is 37, 16
      Point origin = Point.Create(pt.X + 37 - (text_size.Dx >> 1), pt.Y + 16 - (text_size.Dy >> 1));

      txtRect = Rect.Create(origin, text_size);

      DrawText(LargeFont, Colors.White, Colors.Hollow, value, origin, txtRect, TextOutStyle.Clipped);
    }
  }
}
