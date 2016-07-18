using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace CanFly
{
  /// <summary>
  /// Class that implements a data upload service.
  /// </summary>
  public class DUService : ServiceChannel
  {
    private UInt16 bytesToReceive;
    private ushort nodeId;
    private byte nextBlockNumber;
    private List<byte> receivedBytes = new List<byte>();
    private ReceiveHandler receiveHandler;

    public delegate void ReceiveHandler(byte[] buffer);

    public DUService(CANaerospace connection, UInt16 channelNumber)
      : base(connection, channelNumber)
    {
    }

    public override void ProcessCANMSG(CANASMsg msg)
    {
      UInt32 chksum = 0;
      // send an acknowldge message
      CANASMsg reply = new CANASMsg(chksum, 3);
      reply.ChangeDataType(CANASMsg.DataType.CHKSUM);

      msg.Send(Connection, msg.ID + 1, msg.MessageCode);

      // copy the bytes
      receivedBytes.AddRange(msg.UCHAR4);

      if (receivedBytes.Count >= bytesToReceive)
      {
        Trace.TraceInformation("DuService All bytes received");
        receiveHandler(receivedBytes.ToArray());
      }
    }

    /// <summary>
    /// Send a receive request to the addressed node.
    /// </summary>
    /// <param name="nodeID">Node to receive data from</param>
    /// <param name="memid">Memory ID of the data to request</param>
    /// <param name="bytesToReceive">Number of bytes wanted see remarks</param>
    /// <param name="handler">Callback to process the received data</param>
    /// <remarks>
    /// The bytesToRecieve parameter must be from 1..1020 bytes
    /// </remarks>
    public void Receive(byte nodeID, UInt32 memid, UInt16 bytesToReceive, ReceiveHandler handler)
    {
      receiveHandler = handler;
      this.bytesToReceive = bytesToReceive;
      this.nodeId = nodeID;

      CANASMsg uploadMsg = new CANASMsg(memid, 3);
      uploadMsg.ChangeDataType(CANASMsg.DataType.MEMID);

      receivedBytes.Clear();

      UInt16 blocksToReceive = (UInt16)((((bytesToReceive-1) | 3) +1) >> 2);

      uploadMsg.Send(Connection, ChannelNumber, nodeID, (byte)blocksToReceive);
    }
  }
}
