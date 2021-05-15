using System;
using CanFly;
using CanFly.Proton;

namespace Kotuku
{
  public class EFIDialGauge : DialWidget
  {
    public enum ValueStatus
    {
      NoStatus,
      Failed,
      Divergence,
      OK
    };
    private short _leftValue;
    private short _rightValue;
    private ValueStatus _leftStatus;
    private ValueStatus _rightStatus;
    private bool _showStatus;
    private short _maxDivergence;
    private int _leftTicks;
    private int _rightTicks;

    public EFIDialGauge(Widget parent, Rect bounds, ushort id, ushort leftValue, ushort rightValue, bool showStatus)
    : base(parent, bounds, id, 0, 0)
    {
      _leftStatus = ValueStatus.NoStatus;
      _rightStatus = ValueStatus.NoStatus;

      _leftTicks = 0;
      _rightTicks = 0;

      // allow for any value
      _maxDivergence = Int16.MaxValue;

      // hook each value
      if (leftValue != 0)
      {
        AddCanFlyEvent(leftValue, OnLeftValue);
        AddCanFlyEvent(CanFlyID.id_left_engine_status, OnLeftStatus);
      }

      if (rightValue != 0)
      {
        AddCanFlyEvent(rightValue, OnRightValue);
        AddCanFlyEvent(CanFlyID.id_right_engine_status, OnRightStatus);
      }

      _showStatus = showStatus;
      if (showStatus)
      {
        // hook the widget timer message
        AddEventListener(PhotonID.Timer, OnTimer);
      }
    }

    private void CheckValues(short leftValue, short rightValue)
    {
      // only check divergence if the status of both = 1
      bool leftRunning = ((int)_leftStatus) >= 2;
      bool rightRunning = ((int)_rightStatus) >= 2;

      if (leftRunning && !rightRunning)
      {
        SetValue(leftValue);
        return;
      }

      if (rightRunning && !leftRunning)
      {
        SetValue(rightValue);
        return;
      }

      if (!leftRunning && !rightRunning)
      {
        SetValue(0);
        return;
      }

      // set to the average of left+right
      SetValue((short)((leftValue + rightValue) >> 1));

      // check for a divergence error
      if (_showStatus)
      {
        int divergence = leftValue - rightValue;
        if (divergence < 0)
          divergence = 0 - divergence;

        if (divergence > (int)_maxDivergence)
        {
          if (_leftStatus == ValueStatus.OK)
            _leftStatus = ValueStatus.Divergence;

          if (_rightStatus == ValueStatus.OK)
            _rightStatus = ValueStatus.Divergence;
        }
        else
        {
          if (_leftStatus == ValueStatus.Divergence)
            _leftStatus = ValueStatus.OK;

          if (_rightStatus == ValueStatus.Divergence)
            _rightStatus = ValueStatus.OK;
        }
      }
    }

    private void OnLeftValue(CanFlyMsg msg)
    {
      short value = msg.GetInt16();

      if (_leftValue != value)
      {
        _leftValue = value;
        CheckValues(_leftValue, _rightValue);
        InvalidateRect();
      }
    }

    private void OnRightValue(CanFlyMsg msg)
    {
      short value = msg.GetInt16();

      if (_rightValue != value)
      {
        _rightValue = value;
        CheckValues(_leftValue, _rightValue);
        InvalidateRect();
      }
    }

    protected override void PaintWidget()
    {
      // draw the base widget
      base.PaintWidget();

      // draw 2 dots
      if (_showStatus)
      {
        // we show
        // Green = controller OK
        // Red = controller fail
        // Orange = controller divergence
        Color statusColor = Colors.Hollow;

        switch (_leftStatus)
        {
          case ValueStatus.NoStatus:
            // draw no status received, and not timed out = nothing
            statusColor = BackgroundColor;
            break;
          case ValueStatus.Failed:
            statusColor = Colors.Red;
            break;
          case ValueStatus.Divergence:
            statusColor = Colors.Orange;
            break;
          case ValueStatus.OK:
            statusColor = Colors.Green;
            break;
        }

        Rect rectStatus = Rect.Create(8, 8, 18, 18);
        Ellipse(Pens.WhitePen, statusColor, rectStatus);

        // draw the right status
        switch (_rightStatus)
        {
          case ValueStatus.NoStatus:
            // draw no status received, and not timed out = nothing
            statusColor = BackgroundColor;
            break;
          case ValueStatus.Failed:
            statusColor = Colors.Red;
            break;
          case ValueStatus.Divergence:
            statusColor = Colors.Orange;
            break;
          case ValueStatus.OK:
            statusColor = Colors.Green;
            break;
        }

        Rect wndRect = WindowRect;

        rectStatus = Rect.Create(wndRect.Right - 18, wndRect.Top + 8, wndRect.Right - 8, wndRect.Top + 18);
        Ellipse(Pens.WhitePen, statusColor, rectStatus);
      }
    }

    public short LeftValue
    {
      get { return _leftValue; }
    }

    public short RightValue
    {
      get { return _rightValue; }
    }

    public short MaxDivergence
    {
      get { return _maxDivergence; }
      set { _maxDivergence = value; }
    }

    public bool ShowStatus
    {
      get { return _showStatus; }
      set { _showStatus = value; }
    }

    private void OnLeftStatus(CanFlyMsg msg)
    {
      _leftTicks = 0;

      short statusValue = msg.GetInt16();

      if (statusValue > 0)
      {
        switch (_leftStatus)
        {
          case ValueStatus.NoStatus:
          case ValueStatus.Failed:
            _leftStatus = ValueStatus.OK;
            InvalidateRect();
            break;
        }
      }
      else if (_leftStatus != ValueStatus.Failed)
      {
        _leftStatus = ValueStatus.Failed;
        InvalidateRect();
      }
    }

    private void OnRightStatus(CanFlyMsg msg)
    {
      _rightTicks = 0;

      short statusValue = msg.GetInt16();

      if (statusValue > 0)
      {
        switch (_rightStatus)
        {
          case ValueStatus.NoStatus:
          case ValueStatus.Failed:
            _rightStatus = ValueStatus.OK;
            InvalidateRect();
            break;
        }
      }
      else if (_rightStatus != ValueStatus.Failed)
      {
        _rightStatus = ValueStatus.Failed;
        InvalidateRect();
      }
    }

    private void OnTimer(CanFlyMsg msg)
    {

    }
  }
}