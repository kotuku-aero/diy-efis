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

namespace CanFly.Proton
{
  public sealed class AnnunciatorWidget : Widget
  {
    private bool draw_border;
    private uint hours; // hobbs hours, stored in AHRS as hours * 100
    private ushort qnh; // qnh, stored in AHRS
    private ushort clock;
    private short oat;
    private ushort cas;

    private Font small_font;
    private Font large_font;

    private Point clock_pt = new Point(1, 10 );
    private Point hrs_pt = new Point(1, 55 );
    private Point qnh_pt = new Point(1, 100 );
    private Point oat_pt = new Point(1, 145 );
    private Point cas_pt = new Point(1, 190 );

    private readonly string hours_value = "TTS";
    private readonly string qnh_value = "QNH";

    public AnnunciatorWidget(Widget parent, Rect bounds, ushort id, uint key)
  : base(parent, bounds, id)
    {
      if (!TryRegGetBool(key, "draw-border", out draw_border))
        draw_border = true;

      if (!LookupFont(key, "small-font", out small_font))
      {
        // we always have the neo font.
        OpenFont("neo", 9, out small_font);
      }

      if (!LookupFont(key, "large-font", out large_font))
      {
        // we always have the neo font.
        OpenFont("neo", 15, out large_font);
      }

      AddEventListener(CanFlyID.id_def_utc, OnDefUtc);
      AddEventListener(CanFlyID.id_qnh, OnQnh);
      AddEventListener(CanFlyID.id_true_airspeed, OnTrueAirspeed);
      AddEventListener(CanFlyID.id_outside_air_temperature, OnOutsideAirTemperature);
      AddEventListener(CanFlyID.id_air_time, OnAirTime);
    }

    private void OnAirTime(CanFlyMsg e)
    {
      uint value = e.GetUInt32();

      if(hours != value)
      {
        hours = value;
        InvalidateRect();
      }
    }

    private void OnOutsideAirTemperature(CanFlyMsg e)
    {
      short value = e.GetInt16();

      if(oat != value)
      {
        oat = value;
        InvalidateRect();
      }
    }

    private void OnTrueAirspeed(CanFlyMsg e)
    {
      ushort value = e.GetUInt16();
      if(cas != value)
      {
        cas = value;
        InvalidateRect();
      }
    }

    private void OnQnh(CanFlyMsg e)
    {
      ushort value = e.GetUInt16();

      if(qnh != value)
      {
        qnh = value;
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

        if(clock != minutes)
        {
          clock = minutes;
          InvalidateRect();
        }
      }
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wnd_rect = WindowRect;

      if (draw_border)
        RoundRect(Pens.WhitePen, Colors.Hollow, wnd_rect, 12);

      DrawAnnunciator(wnd_rect, clock_pt, "UTC", string.Format("{0:d2}:{1:d2}", clock / 60, clock % 60));
      DrawAnnunciator(wnd_rect, hrs_pt, "Hrs", string.Format("{0:f6.1", ((double)hours) / 100));
      DrawAnnunciator(wnd_rect, qnh_pt, "QNH", string.Format("{0:d}", qnh));
      DrawAnnunciator(wnd_rect, oat_pt, "OAT", string.Format("{0:d}", oat));
      DrawAnnunciator(wnd_rect, cas_pt, "CAS", string.Format("{0:d}", cas));

      EndPaint();
    }

    private void DrawAnnunciator(Rect wnd_rect, Point pt, string label, string value)
    {
      // calculate the size of the label
      Rect clip_rect = new Rect(pt.X, pt.Y, pt.X + 75, pt.Y + 50 );

      Extent label_size = TextExtent(small_font, label);

      int width = wnd_rect.Width;
      int text_start = pt.X + width - (label_size.Dx + 3);
      int right = pt.X + width;

      Rectangle(null, Colors.Gray, new Rect(pt.X, pt.Y + 1, right, pt.Y + 4));
      Rectangle(null, Colors.Gray, new Rect(pt.X, pt.Y + 4, pt.X + 3, pt.Y + 28));
      Rectangle(null, Colors.Gray, new Rect(right - 3, pt.Y + 4, right, pt.Y + 45));
      Rectangle(null, Colors.Gray, new Rect(pt.X, pt.Y + 28, right, pt.Y + 31));
      Rectangle(null, Colors.Gray, new Rect(text_start - 6, pt.Y + 31, right, pt.Y + 45));

      Rectangle(null, Colors.Black, new Rect(pt.X + 3, pt.Y + 4, right - 3, pt.Y + 28));

      Point label_origin = new Point(text_start, pt.Y + 30 );

      Rect txtRect = new Rect(text_start, pt.Y + 30, text_start + label_size.Dx, pt.Y + 30 + label_size.Dy);

      DrawText(small_font, Colors.White, Colors.Black, label, label_origin, txtRect, TextOutStyle.Clipped);

      Extent text_size = TextExtent(large_font, value);

      // center of the text is 37, 16
      Point origin = new Point(pt.X + 37 - (text_size.Dx >> 1), pt.Y + 16 - (text_size.Dy >> 1));

      txtRect = new Rect(origin, text_size);

      DrawText(large_font, Colors.White, Colors.Hollow, value, origin, txtRect, TextOutStyle.Clipped);
    }
  }
}
