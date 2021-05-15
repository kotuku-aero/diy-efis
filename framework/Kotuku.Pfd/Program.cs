using CanFly;

namespace Kotuku.Pfd
{
  public class Program
  {
    public static void Main()
    {
     
      // load the main window
      PFDWidget backgroundWidget = new PFDWidget(Screen.Instance, 0, 0);

      // this never returns, unless a reboot is requested.
      Screen.Instance.ProcessMessages();
    }
  }
}
