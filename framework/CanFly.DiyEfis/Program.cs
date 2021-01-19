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
      try
      {
        // all except orientation are required
        protonKey = Syscall.RegOpenKey(0, "proton");
        layoutName = Syscall.RegGetString(protonKey, "layout");
        layoutKey = Syscall.RegOpenKey(protonKey, layoutName);
        orientation = Syscall.RegGetUint16(protonKey, "orientation");
      }
      catch
      {
        if (layoutKey == 0)
          return;
      }

      // load the main window
      Proton.LayoutWidget layoutWidget = new Proton.LayoutWidget(Screen.Instance, orientation, 0, layoutKey);

      // this never returns, unless a reboot is requested.
      layoutWidget.Run();
    }
  }
}
