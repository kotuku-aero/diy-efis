using System;

namespace CanFly
{
  public sealed class Screen : Widget
  {
    private static Screen _screen;
    private Screen(uint hwnd) : base(hwnd)
    {
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      // screen does nothing, as is a canvas only.
    }

    private class WidgetLock { };
    private static WidgetLock widgetLock = new WidgetLock();

    /// <summary>
    /// Return the singleton screen widget
    /// </summary>
    public static Screen Instance
    {
      get
      {
        if (_screen == null)
        {
          lock (widgetLock)
            if (_screen == null)
            {
              _screen = new Screen(Syscall.OpenScreen(0, 0));
            }
        }

        return _screen;
      }
    }
  }
}
