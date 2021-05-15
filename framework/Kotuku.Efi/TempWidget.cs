using CanFly;
using CanFly.Proton;

namespace Kotuku
{
  /// <summary>
  /// Widget to display a series of CHT/EGT values
  /// </summary>
  public class TempWidget : Widget
  {
    public delegate void OnValueChanged(ushort canID, ushort value);
    public abstract class Bar
    {
      private ushort _canID;
      private Color _defaultColor;
      private ushort _alarmID;
      private Color _alarmColor;
      private ushort _resetID;
      private string _barName;
      private float _offset;
      private float _scale;
      private bool _showMax;
      private ushort _value;
      private ushort _maxValue;
      private ushort _sensorID;
      private bool _sensorFail;
      private OnValueChanged _valueChanged;

      // calculated at run-time
      public Bar(ushort canID, Color defaultColor, ushort alarmID, Color alarmColor, ushort sensorID, ushort resetID, string barName)
      {
        _canID = canID;
        _defaultColor = defaultColor;
        _alarmID = alarmID;
        _alarmColor = alarmColor;
        _resetID = resetID;
        _barName = barName;
        _sensorID = sensorID;
        _sensorFail = true;
        _offset = 0.0f;
        _scale = 0.0f;
        _showMax = false;
        _value = 0;
        _maxValue = 0;

        Widget.AddCanFlyEvent(canID, OnTempValue);
        Widget.AddCanFlyEvent(alarmID, OnAlarm);

        if(sensorID != 0)
          Widget.AddCanFlyEvent(sensorID, OnSensorStatus);

        if(resetID != 0)
          Widget.AddCanFlyEvent(resetID, OnResetMarker);
      }

      public abstract bool IsEGT { get; }

      public ushort CanID { get { return _canID; } }
      public Color DefaultColor { get { return _defaultColor; }  }
      public ushort AlarmID { get { return _alarmID; }  }
      public Color AlarmColor { get { return _alarmColor; }  }
      public ushort ResetID { get { return _resetID; }  }
      public string BarName { get { return _barName; } }
      /// <summary>
      /// Offset to apply to the can value that is read
      /// </summary>
      /// <value></value>
      public float Offset
      {
        get { return _offset; }
        set { _offset = value; }
      }
      /// <summary>
      /// Value that is multiplied by
      /// </summary>
      /// <value></value>
      public float Scale
      {
        get { return _scale; }
        set { _scale = value; }
      }
      /// <summary>
      /// Flag to say if the max marker is visible.
      /// </summary>
      /// <value></value>
      public bool ShowMax
      {
        get { return _showMax; }
        set { _showMax = value; }
      }
      public ushort Value
      {
        get { return _value; }
        set { _value = value; }
      }
      public ushort MaxValue
      {
        get { return _maxValue; }
        set { _maxValue = value; }
      }

      public ushort SensorID { get { return _sensorID; } }
      public bool SensorFail { get { return _sensorFail; } }

      public event OnValueChanged ValueChanged 
      {
        add { _valueChanged += value; } 
        remove { _valueChanged -= value; }
      }

      private void NotifyChanged(ushort canID, ushort value)
      {
        if(_valueChanged != null)
          _valueChanged(canID, value);
      }

      private void OnTempValue(CanFlyMsg msg)
      {
        ushort value = (ushort) msg.GetInt16();

        if(_value != value)
        {
          _value = value;
          NotifyChanged(msg.CanID, value);
        }
      }

      private void OnAlarm(CanFlyMsg msg)
      {

      }

      private void OnSensorStatus(CanFlyMsg msg)
      {

      }

      private void OnResetMarker(CanFlyMsg mgs)
      {

      }

      public void OnTimer()
      {

      }
    }

    public sealed class CHTBar : Bar
    {
      public CHTBar(ushort canID, Color defaultColor, ushort alarmID, Color alarmColor, ushort sensorID, ushort resetID, string barName)
      : base(canID, defaultColor, alarmID, alarmColor, sensorID, resetID, barName)
      {
      }

      public override bool IsEGT { get { return false; } }
    }

    public sealed class EGTBar : Bar
    {
      public EGTBar(ushort canID, Color defaultColor, ushort alarmID, Color alarmColor, ushort sensorID, ushort resetID, string barName)
      : base(canID, defaultColor, alarmID, alarmColor, sensorID, resetID, barName)
      {
      }

      public override bool IsEGT { get { return true; } }
    }

    private Font _font;

    private Bar[] _graph;
    private ushort _chtRedline;
    private ushort _egtRedline;
    private ushort _chtMin;
    private ushort _egtMin;
    private int _leftGutter;
    private int _rightGutter;
    private int _cylinderDrawWidth;
    private int _egtLine;
    private int _chtLine;
    private int _chtRedlinePos;
    private int _barTop;
    private int _barBottom;
    private int _statusTop;
    private bool _drawBorder;

    public TempWidget(Widget parent, Rect bounds, ushort id, params Bar[] graph)
    : base(parent, bounds, id)
    {
      _graph = graph;

      foreach(Bar bar in _graph)
        bar.ValueChanged += ValueChanged;

      _leftGutter = 24;
      _rightGutter = bounds.Width - 24;

      _cylinderDrawWidth = bounds.Width - 48;
      _cylinderDrawWidth /= (_graph.Length / 2);

      _egtLine = 5;
      _chtLine = 17;
      _barTop = 31;

      _barBottom = bounds.Height - 16;
      _statusTop = _barBottom + 2;

      _chtRedlinePos = ((_barBottom - _barTop) >> 1) + _barTop;

      _font = Font.Open("neo", 9);

      //_drawBorder = true;

      InvalidateRect();
    }

    private void ValueChanged(ushort canID, ushort value)
    {
      InvalidateRect();
    }

    private ushort ToCelcius(ushort kelvin)
    {
      if(kelvin == 0)
        return 0;

      return (ushort)(kelvin - 273);
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      //  Draw each bar graph tuple as CGT then EGT.
      BeginPaint();

      Rect wndRect = WindowRect;

      Rectangle(Pens.Hollow, Colors.Black, wndRect);

      if (DrawBorder)
        RoundRect(Pens.LightGrayPen, Colors.Hollow, wndRect, 12);

      string val;
      Extent dims;
      Point pt;
      int barCenter = _cylinderDrawWidth >> 1;
      int pos;


      // TODO: this should be background
      // draw the CHT redline colors
      DrawText(_font, Colors.LightGreen, Colors.Black, "CHT", Point.Create(2, _chtLine));
      DrawText(_font, Colors.LightBlue, Colors.Black, "EGT", Point.Create(_rightGutter, _egtLine));

      val = ToCelcius(_chtRedline).ToString();
      dims = TextExtent(_font, val);
      pt = Point.Create(_leftGutter - dims.Dx, _chtRedlinePos -(dims.Dy >> 1));

      DrawText(_font, Colors.Red, Colors.Black, val, pt);

      // draw the EGT redline colors
      val = ToCelcius(_egtRedline).ToString();
      dims = TextExtent(_font, val);
      pt = Point.Create(_rightGutter, _barTop - (dims.Dy >> 1));

      DrawText(_font, Colors.Red, Colors.Black, val, pt);

      // draw the CHT min
      val = ToCelcius(_chtMin).ToString();
      dims = TextExtent(_font, val);
      pt = Point.Create(_leftGutter - dims.Dx, _barBottom - (dims.Dy >> 1));

      DrawText(_font, Colors.White, Colors.Black, val, pt);

      // draw the EGT min
      val = ToCelcius(_egtMin).ToString();
      dims = TextExtent(_font, val);
      pt = Point.Create(_rightGutter, _barBottom - (dims.Dy >> 1));

      DrawText(_font, Colors.White, Colors.Black, val, pt);
      // draw the vercal seps
      pos = _leftGutter + barCenter;
      for (int i = 0; i < _graph.Length; i += 2)
      {
        Line(Pens.WhitePen, Point.Create(pos, _barTop), Point.Create(pos, _barBottom+1));
        Line(Pens.RedPen, Point.Create(pos - barCenter +1, _chtRedlinePos), Point.Create(pos -1, _chtRedlinePos));

        pos += _cylinderDrawWidth;
      }

      // Draw the thermocouple values

      // EGT values
      pos = _leftGutter + barCenter;
      int graphWidth = barCenter - 2;
      for (int i = 1; i < _graph.Length; i += 2)
      {
        ushort value = _graph[i].Value;
        ushort egt = ToCelcius(value);      // kelvin to C
        val = egt.ToString();

        dims = TextExtent(_font, val);
        pt = Point.Create(pos - (dims.Dx >> 1), _egtLine);
        DrawText(_font, _graph[i].DefaultColor, Colors.Black, val, pt);

        int topPx = CalculateHeight(value, _barTop, _barTop, _barBottom, _egtRedline, _egtMin);

        Rect graphRect = Rect.Create(pos +3, topPx, pos + graphWidth, _barBottom);
        if(value >= _egtRedline)
          DrawBar(Pens.Hollow, Colors.Red, graphRect);
        else
          DrawBar(Pens.Hollow, _graph[i].DefaultColor, graphRect);

        pos += _cylinderDrawWidth;
      }

      pos = _leftGutter + barCenter;
      for (int i = 0; i < _graph.Length; i += 2)
      {
        ushort value = _graph[i].Value;
        int cht = ToCelcius(value);      // kelvin to C
        val = cht.ToString();

        dims = TextExtent(_font, val);
        pt = Point.Create(pos - (dims.Dx >> 1), _chtLine);
        DrawText(_font, _graph[i].DefaultColor, Colors.Black, val, pt);

        int topPx = CalculateHeight(value, _chtRedlinePos, _barTop, _barBottom, _chtRedline, _chtMin);

        Rect graphRect = Rect.Create(pos - graphWidth, topPx, pos - 2, _barBottom);
        if(value >= _chtRedline)
          DrawBar(Pens.Hollow, Colors.Red, graphRect);
        else
          DrawBar(Pens.Hollow, _graph[i].DefaultColor, graphRect);

        pos += _cylinderDrawWidth;
      }

      EndPaint();
    }

    private void DrawBar(Pen outline, Color color, Rect rect)
    {
      Pen barPen = Pen.Create(color, 1, PenStyle.Solid);

      for (int row = rect.Bottom; row >= rect.Top; row -= 2)
      {
        Line(barPen, Point.Create(rect.Left, row), Point.Create(rect.Right, row));
      }
    }

    private int CalculateHeight(ushort value, int calcTop, int barTop, int barBottom, ushort maxValue, ushort minValue)
    {
      if(value <= minValue)
        return barBottom - 1;

      // calculate the distance
      int dist = value - minValue;
      int barHeight = barBottom - calcTop;
      float pct = ((float)dist) / ((float)(maxValue - minValue));
      pct *= barHeight;

      dist = (int)pct;
      dist = barBottom - dist;

      if(dist < barTop)
        dist = barTop;

      return dist;
    }

    public ushort ChtRedline
    {
      get { return _chtRedline; }
      set { _chtRedline = value; }
    }

    public ushort EgtRedline
    {
      get { return _egtRedline; }
      set { _egtRedline = value; }
    }

    public ushort ChtMin
    {
      get { return _chtMin; }
      set { _chtMin = value; }
    }

    public ushort EgtMin
    {
      get { return _egtMin; }
      set { _egtMin = value; }
    }

    public bool DrawBorder
    {
      get { return _drawBorder; }
      set { _drawBorder = value; }
    }
  }
}