using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  /// <summary>
  /// Base class for a service channel
  /// </summary>
  public abstract class ServiceChannel
  {
    private CANaerospace.ProcessCanMsg receiveHandler = null;
    public UInt16 ChannelNumber { get; private set; }
    public CANaerospace Connection { get; private set; }

    public ServiceChannel(CANaerospace connection, UInt16 channelNumber)
    {
      Connection = connection;
      ChannelNumber = channelNumber;
      receiveHandler = new CANaerospace.ProcessCanMsg(Process);
      Connection.CanMsgReceived += receiveHandler;
    }

    public void ReleaseService()
    {
      Connection.CanMsgReceived -= receiveHandler;
      receiveHandler = null;
    }

    private void Process(CANASMsg msg)
    {
      // determine if it is for our node service channel
      if (msg.ID == ChannelNumber + 1)
        ProcessCANMSG(msg);
    }

    public abstract void ProcessCANMSG(CANASMsg msg);
  }
}
