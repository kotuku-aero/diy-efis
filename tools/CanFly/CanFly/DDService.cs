using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  /// <summary>
  /// Class implementing a data download service
  /// </summary>
  public class DDService : ServiceChannel
  {
    private CANaerospace connection;
    private byte[] data;
    private byte nextBlockNumber;
    private byte nodeId;
    private int dataOffset = 0;
    private StatusHandler statusHandler;

    public enum DDSResult
    {
      InProgress,
      Error,
      BadChecksum,
      Complete
    };

    public delegate void StatusHandler(DDSResult result);

    public DDService(CANaerospace connection, ushort channelNumber)
      : base(connection, channelNumber)
    {
    }

    public override void ProcessCANMSG(CANASMsg msg)
    {
      // the message can be a checksum or -1 on error.  If error then we are done.
      switch (msg.MessageDataType)
      {
        case CANASMsg.DataType.CHKSUM :
          // TODO: check checksum
          statusHandler(DDSResult.InProgress);
          break;
        case CANASMsg.DataType.LONG :
          if (msg.LONG == -1)
          {
            statusHandler(DDSResult.Error);
            return;
          }
          break;
        default :
          break;
      }

      // send the next block of data
      if (dataOffset >= data.Length)
      {
        statusHandler(DDSResult.Complete);
        return;
      }

      CANASMsg uploadMsg;
      switch (data.Length - dataOffset)
      {
        case 1 :
          uploadMsg = new CANASMsg(data[dataOffset++], 2);
          break;
        case 2 :
          uploadMsg = new CANASMsg(data[dataOffset], data[dataOffset + 1], 2);
          dataOffset += 2;
          break;
        case 3 :
          uploadMsg = new CANASMsg(data[dataOffset], data[dataOffset + 1], data[dataOffset + 2], 2);
          dataOffset += 3;
          break;
        default :
          uploadMsg = new CANASMsg(data[dataOffset], data[dataOffset + 1], data[dataOffset + 2], data[dataOffset + 3], 2);
          dataOffset += 4;
          break;
      }
      uploadMsg.Send(Connection, ChannelNumber, nodeId, 0);
    }

    public void Send(byte nodeId, UInt32 memid, byte[] data, StatusHandler statusHandler)
    {
      this.nodeId = nodeId;
      this.data = data;
      this.statusHandler = statusHandler;
      dataOffset = 0;

      // send the memid to start the process off

      CANASMsg uploadMsg = new CANASMsg(memid, 2);
      uploadMsg.ChangeDataType(CANASMsg.DataType.MEMID);

      byte blocksToSend = (byte)((((data.Length -1)|3)+1)/4);
      uploadMsg.Send(Connection, ChannelNumber, nodeId, blocksToSend);
    }
  }
}
