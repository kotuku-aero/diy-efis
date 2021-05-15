using System;
using CanFly;
using CanFly.Proton;

namespace Kotuku
{
  public class AnnunciatorWidget : GaugeWidget
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

    static readonly string fuelTotalStr = "fuel-total";

    public AnnunciatorWidget(Widget parent, Rect bounds, ushort id, ushort key)
    : base(parent, bounds, id)
    {
      _key = key;

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

    }

    private void OnLeftFuel(CanFlyMsg msg)
    {

    }

    private void OnRightFuel(CanFlyMsg msg)
    {

    }

    private void OnFuelFlow(CanFlyMsg msg)
    {
      _fuelFlow = msg.GetFloat();
      _flowTicks = SystemTicks;
    }

    protected override void PaintBackground(Canvas backgroundCanvas)
    {
    }

    protected override void PaintWidget()
    {
    }
  }
}
