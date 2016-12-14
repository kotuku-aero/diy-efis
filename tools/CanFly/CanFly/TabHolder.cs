using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  class TabHolder : TabPage, ICanFlyMonitor
  {
    private ICanFlyMonitor monitor;

    public TabHolder(string name, Control page)
      : base(name)
    {
      page.Parent = this;
      page.Dock = DockStyle.Fill;

      monitor = page as ICanFlyMonitor;
    }

    public TabIdentifier Device
    {
      get { return monitor.Device; }
    }

    public void Disconnect()
    {
      monitor.Disconnect();
    }
  }
}
