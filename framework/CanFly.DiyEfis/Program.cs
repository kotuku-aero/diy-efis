namespace CanFly.DiyEfis
{
  public class Program
  {
    public static void Main()
    {
      uint protonKey = 0;
      ushort orientation = 0;
      uint layoutKey = 0;
      string layoutName;
      // all except orientation are required
      if (Syscall.RegOpenKey(0, "proton", out protonKey) != 0 ||
         Syscall.RegGetString(protonKey, "layout", out layoutName) != 0 ||
         Syscall.RegOpenKey(protonKey, layoutName, out layoutKey) != 0)
        return;

      if (Syscall.RegGetUint16(protonKey, "orientation", out orientation) != 0)
        orientation = 0;

      // load the main window
      Proton.LayoutWidget layoutWidget = new Proton.LayoutWidget(Screen.Instance, orientation, 0, layoutKey);

      // this never returns, unless a reboot is requested.
      layoutWidget.Run();
    }
  }
}
