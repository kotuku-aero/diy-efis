using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace CanFly
{
  /// <summary>
  /// This class forms the base for all canfly specific modules
  /// </summary>
  public abstract class CanFlyProxy
  {
    private CANaerospace communicationsChannel;
    private byte nodeID;              // this is the node ID of the remote service channel
    private byte hardwareRevision;
    private byte softwareRevision;
    private TreeNode node;
    private int channelNumber;

    private string nodeName;

    public CanFlyProxy(string name, CANaerospace channel, int channelNumber, CANASMsg idsMessage)
    {
      this.channelNumber = channelNumber;
      communicationsChannel = channel;
      hardwareRevision = idsMessage.UCHAR4[0];
      softwareRevision = idsMessage.UCHAR4[1];
      nodeID = idsMessage.UCHAR4[3];

      // assign the node name as <name> (nodeID)
      nodeName = String.Format("{0} - ({1})", name, nodeID);
    }

    [Browsable(false)]
    public int ChannelNumber
    {
      get { return channelNumber; }
    }

    /// <summary>
    /// Tree node associated with this proxy
    /// </summary>
    [Browsable(false)]
    public TreeNode Node
    {
      get { return node; }
      set { node = value; }
    }
    /// <summary>
    /// Return the id of the remote node
    /// </summary>
    [ReadOnly(true)]
    [Category("Device")]
    public byte NodeID { get { return nodeID; } }

    /// <summary>
    /// Return the hardware revision of the remote node
    /// </summary>
    [ReadOnly(true)]
    [Category("Device")]
    public byte HardwareRevision { get { return hardwareRevision; } }

    /// <summary>
    /// Return the software revision of the remote node
    /// </summary>
    [ReadOnly(true)]
    [Category("Device")]
    public byte SoftwareRevision { get { return softwareRevision; } }

    /// <summary>
    /// The name of the node.
    /// </summary>
    [ReadOnly(true)]
    [Category("Device")]
    public string Name { get { return nodeName; } }

    [Browsable(false)]
    public CANaerospace Channel { get { return communicationsChannel; } }

    public void Initialize()
    {
      ProxyInitialize(channelNumber);
    }

    /// <summary>
    /// Called when the derived proxy should initialize itself
    /// </summary>
    protected abstract void ProxyInitialize(int channelNumber);

    /// <summary>
    /// Save any changed parameters to the device.
    /// </summary>
    /// <returns></returns>
    public abstract bool Save();
  }
}
