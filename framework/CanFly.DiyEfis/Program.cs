namespace CanFly.DiyEfis
{
  public class Program
  {
    public static void Main()
    {
      uint protonKey = 0;
      uint layoutKey = 0;
      ushort orientation;
      string layoutName;
      // all except orientation are required
      Syscall.RegOpenKey(0, "proton", out protonKey);
      Syscall.RegGetString(protonKey, "layout", out layoutName);
      Syscall.RegOpenKey(protonKey, layoutName, out layoutKey);
      
      try
      {
        Syscall.RegGetUint16(protonKey, "orientation", out orientation);
      }
      catch
      {
        orientation = 0;
      }

      // load the main window
      Proton.LayoutWidget layoutWidget = new Proton.LayoutWidget(Screen.Instance, orientation, 0, layoutKey);

      // this never returns, unless a reboot is requested.
      layoutWidget.Run();
    }
  }
}
