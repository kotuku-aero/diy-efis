using CanFly;

namespace Kotuku.Efi
{
  public class Program
  {
    public static void Main()
    {
     
      // load the main window
      EFIWidget backgroundWidget = new EFIWidget(Screen.Instance, 0, 0);

      // this never returns, unless a reboot is requested.
      Screen.Instance.ProcessMessages();
    }
  }
}
