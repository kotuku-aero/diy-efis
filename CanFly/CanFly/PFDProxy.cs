using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  public class PFDProxy : CanFlyProxy
  {
    private List<SettingCollectionProxy> moduleInformation;

    public PFDProxy(CANaerospace channel, int channelNumber, CANASMsg idsMessage)
      : base("PFD", channel, channelNumber, idsMessage)
    {
      // a pfd has a range of settings, as opposed to parameters as it does not publish anything
    }

    protected override void ProxyInitialize(int channelNumber)
    {
      // once the base has completed loading the module information we request
      // what modules are installed.  We do this with a moduleinformation request with
      // a memid of 0.  What is returned is the number of modules.
      // From this we then ask for memid's of 1..number of modules which
      // returns a message that is the memid of each module.
      // The proxy will then create the same number of SettingCollectionProxy objects
      // that will then download using each memid.  However they are created 1 at a time
      // so that we don't run out of channels.

    }

    public override bool Save()
    {
      return true;
    }
  }
}
