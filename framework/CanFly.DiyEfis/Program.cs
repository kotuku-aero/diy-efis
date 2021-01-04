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
        protonKey = Neutron.RegOpenKey(0, "proton");
        layoutName = Neutron.RegGetString(protonKey, "layout");
        layoutKey = Neutron.RegOpenKey(protonKey, layoutName);
        orientation = Neutron.RegGetUint16(protonKey, "orientation");
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
