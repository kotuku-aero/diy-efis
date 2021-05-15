using CanFly;
using CanFly.Proton;

namespace Kotuku.Pfd
{
  class PFDWidget : Widget
  {
    private MenuWidget _menu;
    private AttitudeWidget _attitudeWidget;
    private AirspeedWidget _airspeedWidget;
    private AltitudeWidget _altitudeWidget;

    public PFDWidget(Widget parent, ushort orientation, ushort id)
      : base(parent, parent.WindowRect, id)
    {
      _menu = new MenuWidget(parent, orientation, (ushort)(id + 1));
      _menu.ZOrder = 0x80;    // make sure on top layer

      Font fontNeo9;
      OpenFont("neo", 9, out fontNeo9);
      Font fontNeo12;
      OpenFont("neo", 12, out fontNeo12);

      Rect wndRect = WindowRect;

      _attitudeWidget = new AttitudeWidget(this, wndRect, 100);

      _airspeedWidget = new AirspeedWidget(this, Rect.Create(0, 0, 50, 240), 101);
      _airspeedWidget.ZOrder = 10;
      _airspeedWidget.Va = 110;
      _airspeedWidget.Vfe = 90;
      _airspeedWidget.Vne = 200;
      _airspeedWidget.Vno = 160;
      _airspeedWidget.Vs0 = 45;
      _airspeedWidget.Vs1 = 65;
      _airspeedWidget.Vx = 75;
      _airspeedWidget.Vy = 120;
      _airspeedWidget.BackgroundColor = Color.Create(0x80, 0x80, 0x80, 0x80);
      _airspeedWidget.Scale = 0.514444f;
      _airspeedWidget.SmallRollerFont = fontNeo9;
      _airspeedWidget.Pen = Pens.WhitePen;
      _airspeedWidget.Font = fontNeo9;
      _airspeedWidget.TextColor = Colors.White;
      _airspeedWidget.LargeRollerFont = fontNeo12;

      _altitudeWidget = new AltitudeWidget(this, Rect.Create(240, 0, 320, 240), 102);
      _altitudeWidget.ZOrder = 10;
      _altitudeWidget.BackgroundColor = Color.Create(0x80, 0x80, 0x80, 0x80);
      _altitudeWidget.Scale = 3.38084f;
      _altitudeWidget.TextColor = Colors.White;
      _altitudeWidget.SmallRollerFont = fontNeo9;
      _altitudeWidget.Font = fontNeo9;
      _altitudeWidget.LargeRollerFont = fontNeo12;
    }

  
    protected override void OnPaint(CanFlyMsg msg)
    {
    }
  }
}