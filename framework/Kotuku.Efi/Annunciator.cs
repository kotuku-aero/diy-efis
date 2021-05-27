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
using CanFly.Proton;

namespace Kotuku
{
  public class AnnunciatorWidget : Widget
  {
    private short _oat;
    // only used as an alert mechanism
    private short _leftFuel;
    private short _rightFuel;
    private float _fuelFlow;
    private bool _flowValid;
    private uint _flowTicks;
    private uint _lastFlowCalc;
    private float _fuelTotal;
    private ushort _key;
    private Font _nameFont;
    private Color _nameColor;
    private Font _valueFont;
    private Color _valueColor;
    private Color _backgroundColor;

    static readonly string fuelTotalStr = "fuel-total";

    public AnnunciatorWidget(Widget parent, Rect bounds, ushort id, ushort key)
    : base(parent, bounds, id)
    {
      _key = key;

      _oat = 273;
      _leftFuel = 0;
      _rightFuel = 0;
      _fuelTotal = 0;
      _flowValid = false;

      AddCanFlyEvent(CanFlyID.id_outside_air_temperature, OnOAT);
      AddCanFlyEvent(CanFlyID.id_fuel_flow_rate, OnFuelFlow);
      AddCanFlyEvent(CanFlyID.id_left_fuel_quantity, OnLeftFuel);
      AddCanFlyEvent(CanFlyID.id_right_fuel_quantity, OnRightFuel);

      // these are widget events
      AddEventListener(PhotonID.FuelTotal, OnFuelTotal);
      AddEventListener(PhotonID.Timer, OnTimer);

      if(!TryRegGetFloat(key, fuelTotalStr, out _fuelTotal))
        _fuelTotal = 0;

      _flowValid = false;
      _flowTicks = 0;

      // test
      _flowValid = true;
      _fuelTotal = 160;
      _fuelFlow = 28;
    }

    private void OnFuelTotal(CanFlyMsg msg)
    {
      short fuelTotal = msg.GetInt16();

      if(_fuelTotal != fuelTotal)
      {
        _fuelTotal = (float) fuelTotal;
        RegSetFloat(_key, fuelTotalStr, fuelTotal);

        InvalidateRect();
      }
    }

    private void OnTimer(CanFlyMsg msg)
    {
      // the timer is called every 100msec to perform routine changes
      uint now = SystemTicks;
      uint timeSinceLastReading = now - _flowTicks;

      if(timeSinceLastReading < 1000)      // update 1/sec
        return;

      if(timeSinceLastReading > 5000)  // signal invalid if < 5 sec
      {
        _flowValid = false;
        InvalidateRect();
      }
      else
      {
        uint timeSinceLastCalc = now - _lastFlowCalc;
        if (timeSinceLastCalc >= 1000)
        {
          // calculate the flow as lts/msec
          // which is 3600000 msec/hr
          // use 1/360000
          float ltsConsumed = _fuelFlow * (0.00000002777f * timeSinceLastCalc);

          _lastFlowCalc = now;
          _fuelTotal -= ltsConsumed;
          if (ltsConsumed > 0)
          {
            RegSetFloat(_key, fuelTotalStr, _fuelTotal);

            InvalidateRect();
          }
        }
      }
    }

    private void OnOAT(CanFlyMsg msg)
    {
      short value = msg.GetInt16();

      if(_oat != value)
      {
        _oat = value;
        InvalidateRect();
      }
    }

    private void OnLeftFuel(CanFlyMsg msg)
    {
      short value = msg.GetInt16();

      if(_leftFuel != value)
      {
        _leftFuel = value;
        InvalidateRect();
      }
    }

    private void OnRightFuel(CanFlyMsg msg)
    {
      short value = msg.GetInt16();

      if(_rightFuel != value)
      {
        _rightFuel = value;
        InvalidateRect();
      }
    }

    private void OnFuelFlow(CanFlyMsg msg)
    {
      _fuelFlow = ((float)msg.GetInt16()) / 100.0f ;
      _flowTicks = SystemTicks;
    }

    // this widget displays:
    // OAT:   <value>
    // Fuel Flow: <value>
    // Est Rem: <value>
    //
    // only 40 pix so drawing is very controlled

    static readonly string oatStr = "OAT:";
    static readonly string eteStr = "ETE:";
    static readonly string estRemainingStr = "REM:";

    public Font NameFont
    {
      get { return _nameFont; }
      set
      {
        _nameFont = value;
      }
    }

    public Color NameColor
    {
      get { return _nameColor; }
      set { _nameColor = value; }
    }

    public Font ValueFont
    {
      get { return _valueFont; }
      set { _valueFont = value; }
    }

    public Color ValueColor
    {
      get { return _valueColor; }
      set { _valueColor = value; }
    }

    public Color BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
    }


    protected override void OnPaint(CanFlyMsg msg)
    {
      BeginPaint();

      Rectangle(Pens.Hollow, BackgroundColor, WindowRect);

      DrawText(NameFont, NameColor, BackgroundColor, oatStr, Point.Create(2, 1));
      DrawText(NameFont, NameColor, BackgroundColor, estRemainingStr, Point.Create(2, 14));
      DrawText(NameFont, NameColor, BackgroundColor, eteStr, Point.Create(2, 27));

     int degC = _oat - 273;
      DrawText(ValueFont, ValueColor, BackgroundColor, degC.ToString(), Point.Create(41, 1));

      if (_flowValid)
      {
        int rem = (int)_fuelTotal;
        DrawText(ValueFont, ValueColor, BackgroundColor, rem.ToString(), Point.Create(41, 14));

        if (_fuelFlow > 0)
        {
          // ete is fuel total / fuelFlow
          // total is lts, ff is ltr/hr
          float ete = _fuelTotal / _fuelFlow;
          ete *= 60.0f;

          int mins = (int)ete;

          TimeSpan tsEte = new TimeSpan(0, mins, 0);
          string eteStr = String.Format("{0}:{1}", tsEte.Hours, tsEte.Minutes);
          DrawText(ValueFont, ValueColor, BackgroundColor, rem.ToString(), Point.Create(41, 27));
        }
        else
        {
          DrawText(ValueFont, ValueColor, BackgroundColor, "--:--", Point.Create(41, 27));
        }
      }
      else
      {
        // draw X to show data not available
        Line(Pens.RedPen, Point.Create(41, 14), Point.Create(79, 25));
        Line(Pens.RedPen, Point.Create(41, 25), Point.Create(79, 14));

        Line(Pens.RedPen, Point.Create(41, 27), Point.Create(79, 38));
        Line(Pens.RedPen, Point.Create(41, 38), Point.Create(79, 27));
      }

      EndPaint();
    }
  }
}
