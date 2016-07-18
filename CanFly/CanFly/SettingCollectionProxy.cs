using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  public class SettingCollectionProxy
  {
    private string moduleName;
    private SettingCollection settings;
    private MIService miService;
    private uint memid;
    private byte numSettings;

    public SettingCollectionProxy(CANaerospace channel, uint memid)
    {
      moduleName = "Loading...";
      settings = new SettingCollection();

    }

    public string ModuleName { get { return moduleName; } }

    public override string ToString()
    {
      return moduleName;
    }
  }
}
