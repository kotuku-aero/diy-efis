using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  public struct TabIdentifier
  {
    public int DeviceType;
    public int NodeID;
    public string DeviceName;
  };

  /// <summary>
  /// All tab pages that can monitor the bus and display data derive from this interface
  /// </summary>
  public interface ICanFlyMonitor
  {
    TabIdentifier Device { get; }
    void Disconnect();
  }
}
