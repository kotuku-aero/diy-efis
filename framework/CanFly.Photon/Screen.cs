using System;

namespace CanFly
{
  public sealed class Screen : Widget
  {
    private static Screen _screen;
    private Screen(uint hwnd) : base(hwnd)
    {
    }

    public void ProcessMessages()
    {
      CanFly.Syscall.ProcessMessages(Handle);
    }

    protected override void OnPaint(CanFlyMsg e)
    {
      // screen does nothing, as is a canvas only.
    }

    /// <summary>
    /// Return the singleton screen widget
    /// </summary>
    public static Screen Instance
    {
      get
      {
        if (_screen == null)
        {
          uint handle;
          Syscall.OpenScreen(0, 0, out handle);

          _screen = new Screen(handle);
        }

        return _screen;
      }
    }
  }
}
