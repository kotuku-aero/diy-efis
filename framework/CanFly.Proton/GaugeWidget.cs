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
using System.Collections;

namespace CanFly.Proton
{
  /// <summary>
  /// Base class for Dial and Bar widgers
  /// </summary>
  public abstract class GaugeWidget : Widget
  {
    /// <summary>
    /// Alignment of a value.
    /// </summary>
    public enum ValueOutStyle
    {
      LeftAlign,
      CenterAlign,
      RightAlign
    };

    private string _name;
    private Color _nameColor;
    private Font _nameFont;
    private Point _namePt;
    private bool _drawName;
    private Color _backgroundColor;
    private Pen _borderPen;
    private bool _drawBorder;
    private Font _font;

    private bool _drawValue;         // draw the value
    private bool _valueBoxVisible;
    private ValueOutStyle _valueAlign;    // draw the value centered in the box, otherwise right
    private Font _valueFont;       // what font to draw a value in
    private Rect _valueRect;

    private Canvas _backgroundCanvas;

    private ArrayList _steps;
    private ArrayList _ticks;

    /// <summary>
    /// Construct a gauge widget with no settings
    /// </summary>
    /// <param name="parent">Parent widget</param>
    /// <param name="bounds">Position of the widget relative to the parent</param>
    /// <param name="id">Id of the widget</param>
    public GaugeWidget(Widget parent, Rect bounds, ushort id)
      : base(parent, bounds, id)
    {
      _valueAlign = ValueOutStyle.RightAlign;
      _valueBoxVisible = true;
    }

    /// <summary>
    /// Return the value to display, if the value box is visible
    /// </summary>
    /// <returns></returns>
    public virtual short Value
    {
      get { return 0; }
    }

    public virtual void Initialize()
    {
      _backgroundCanvas = Canvas.Create(WindowRect.Extent);

      Rect wnd_rect = WindowRect;

      // fill without a border
      _backgroundCanvas.Rectangle(Pens.Hollow, BackgroundColor, wnd_rect);

      if (DrawBorder)
        _backgroundCanvas.RoundRect(BorderPen, Colors.Hollow, wnd_rect, 12);

      PaintBackground(_backgroundCanvas);
    }

    /// <summary>
    /// Update the background canvas
    /// </summary>
    /// <param name="backgroundCanvas"></param>
    protected abstract void PaintBackground(Canvas backgroundCanvas);

    protected abstract void PaintWidget();

    protected virtual Color ValueColor(int value)
    {
      return Colors.White;
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      BeginPaint();

      Rect wnd_rect = WindowRect;

      BitBlt(wnd_rect, _backgroundCanvas, Point.Create(0, 0));

      PaintWidget();

      // decorate the canvas
      if (DrawName)
      {
        Extent sz = TextExtent(NameFont, Name);

        DrawText(NameFont, NameColor, BackgroundColor, Name,
          NamePt.Add(-(sz.Dx >> 1), -(sz.Dy >> 1)));
      }

      // draw the value of the gauge as a text string
      if (_drawValue)
      {
        int value = Value;
        string str = value.ToString();
        Extent size = TextExtent(ValueFont, str);

        if(_valueBoxVisible)
          // draw a rectangle around the text
          Rectangle(Pens.GrayPen, Colors.Black, ValueRect);

        Point valuePt;
        switch(_valueAlign)
        {
          case ValueOutStyle.LeftAlign :
            valuePt = Point.Create(ValueRect.Left + 2, ValueRect.Top + 2);
            break;
          case ValueOutStyle.CenterAlign :
            valuePt = Point.Create((ValueRect.Left + (ValueRect.Width >>1)) - (size.Dx >> 1), ValueRect.Top + 2);
            break;
          case ValueOutStyle.RightAlign :
            valuePt = Point.Create(ValueRect.Right - (size.Dx + 2), ValueRect.Top + 2);
            break;
        }

        DrawText(ValueFont, ValueColor(value), Colors.Black, str,
          valuePt, ValueRect, TextOutStyle.Clipped);
      }

      EndPaint();
    }

    protected Step CalculateStep(short value)
    {
      if (_steps == null || _steps.Count == 0)
        return null;

      Step step = (Step)_steps[0];
      // step 0 is only used to set the minimum value for the gauge
      // so it is ignored
      for (int i = 1; i < _steps.Count; i++)
      {
        // get the next one, and see if we are done.
        step = (Step)_steps[i];

        // assume this is our color.
        if (value <= step.Value)
          break;
      }

      return step;
    }

    protected Color CalculateColor(short value)
    {
      Color fill_color = Colors.LightBlue;

      if (_steps == null || _steps.Count == 0)
        return fill_color;

      Step step = (Step)_steps[0];

      for (int i = 0; i < _steps.Count; i++)
      {
        fill_color = step.IndicatorColor;

        step = (Step)_steps[i];

        if (value >= step.Value)
          break;
      }

      return fill_color;
    }

    public string Name
    {
      get { return _name; }
      set { _name = value; }
    }

    public Color NameColor
    {
      get { return _nameColor; }
      set { _nameColor = value; }
    }
    public Font NameFont
    {
      get { return _nameFont; }
      set { _nameFont = value; }
    }
    public Point NamePt
    {
      get { return _namePt; }
      set { _namePt = value; }
    }
    public bool DrawName
    {
      get { return _drawName; }
      set { _drawName = value; }
    }
    public Color BackgroundColor
    {
      get { return _backgroundColor; }
      set { _backgroundColor = value; }
    }

    public Pen BorderPen
    {
      get { return _borderPen; }
      set { _borderPen = value; }
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

    public bool DrawValue
    {
      get { return _drawValue; }
      set { _drawValue = value; }
    }

    public bool ValueBoxVisible
    {
      get { return _valueBoxVisible; }
      set { _valueBoxVisible = value; }
    }

    public Font ValueFont
    {
      get { return _valueFont; }
      set { _valueFont = value; }
    }
    public Rect ValueRect
    {
      get { return _valueRect; }
      set { _valueRect = value; }
    }

    public Step[] Steps
    {
      get
      {
        if (_steps == null)
          return null;

        Step[] result = new Step[_steps.Count];

        for (int i = 0; i < _steps.Count; i++)
          result[i] = (Step)_steps[i];

        return result;
      }
    }

    public int NumSteps
    {
      get { return _steps == null ? 0 : _steps.Count; }
    }

    public Step GetStep(int index)
    {
      if(_steps == null || index >= _steps.Count)
        throw new IndexOutOfRangeException();

      return (Step)_steps[index];
    }

    public void AddStep(Step step)
    {
      if (_steps == null)
        _steps = new ArrayList();

      _steps.Add(step);
    }

    public TickMark[] Ticks
    {
      get
      {
        if (_ticks == null)
          return null;

        TickMark[] result = new TickMark[_ticks.Count];

        for (int i = 0; i < _ticks.Count; i++)
          result[i] = (TickMark)_ticks[i];

        return result;
      }
    }

    public int NumTicks
    {
      get { return _ticks == null ? 0 : _ticks.Count; }
    }

    public TickMark GetTick(int index)
    {
      if(_ticks == null || index >= _ticks.Count)
        throw new IndexOutOfRangeException();

      return (TickMark)_ticks[index];
    }

    public void AddTick(TickMark tick)
    {
      if (_ticks == null)
        _ticks = new ArrayList();

      _ticks.Add(tick);
    }

    public ValueOutStyle ValueAlign
    {
      get { return _valueAlign; }
      set { _valueAlign = value; }
    }
  }
}
