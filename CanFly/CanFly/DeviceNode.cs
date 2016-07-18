using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  class DeviceNode : TreeNode, IPropertyBag
  {
    private CanFlyProxy proxy;

    public DeviceNode(CanFlyProxy proxy)
      : base(proxy.Name)
    {
      this.proxy = proxy;
    }

    public object PropertyBag
    {
      get { return proxy; }
    }
  }
}
