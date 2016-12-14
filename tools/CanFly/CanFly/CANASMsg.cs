using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  public class CANASMsg
  {
    private static byte nextMessageCode = 0;

    // the format of the message is
    //
    // 0 .. nodeID
    // 1 .. dataType
    // 2 .. serviceCode
    // 3 .. Message Code
    // 4..7 Message data
    private byte[] data = new byte[8];
    private byte length;
    private bool extendedAddress;
    private bool reply;
    private UInt32 address;

    public UInt32 ID { get { return address; } }
    // Our node id
    public byte NodeID 
    {
      get { return data[0]; }
      private set { data[0] = value; }
    }

    public enum DataType
    {
      NODATA,
      ERROR,
      FLOAT,
      LONG,
      ULONG,
      BLONG,
      SHORT,
      USHORT,
      BSHORT,
      CHAR,
      UCHAR,
      BCHAR,
      SHORT2,
      USHORT2,
      BSHORT2,
      CHAR4,
      UCHAR4,
      BCHAR4,
      CHAR2,
      UCHAR2,
      BCHAR2,
      MEMID,
      CHKSUM,
      ACHAR,
      ACHAR2,
      ACHAR4,
      CHAR3,
      UCHAR3,
      BCHAR3,
      ACHAR3,
      DOUBLEH,
      DOUBLEL,
    };

    public DataType MessageDataType 
    {
      get { return (DataType)data[1]; }
      private set { data[1] = (byte)value; }
    }

    public byte ServiceCode 
    {
      get { return data[2]; }
      private set { data[2] = value; }
    }

    public byte MessageCode 
    {
      get { return data[3]; }
      private set { data[3] = value; }
    }

    
    private UInt32 ToHex(char ch)
    {
      return (UInt32)(ch >= '0' && ch <= '9' ? ch - '0' :( Char.ToLower(ch) - 'a')+10);
    }

    public CANASMsg(string msg)
    {
      length = 0;         // assume 0
      switch(msg[0])
      {
        case 'T' :
        extendedAddress = true;
        reply = false;
        break;
        case 't' :
        extendedAddress = false;
        reply = false;
        break;
        case 'R' :
        extendedAddress = true;
        reply = true;
        break;
        case 'r' :
        extendedAddress = false;
        reply = true;
        break;
        default :
        extendedAddress = false;
        reply = false;
        return;
      }

      address = 0;
      int num_digits = extendedAddress ? 8 : 3;
      int index = 1;
      char val;
      UInt32 num;

      while(num_digits-- > 0)
      {
        if (index >= msg.Length)
          return;

        // todo: range checks?
        val = msg[index++];
        num = ToHex(val);
        address = (address  << 4) |(num & 0x0f);
      }

      //trace("Can ID is %d\n", id);
      if (index >= msg.Length)
        return;

      length = (byte) ToHex(msg[index++]);

      //trace("Can length is %d\n", len);
      if(reply)
        return;

      for(int i = 0; i < length && i < 8; i++)
      {
        if(index >= msg.Length-1)
          break;
        data[i] = (byte)((((byte)ToHex(msg[index])) << 4) | ((byte)ToHex(msg[index + 1])));
        index += 2;
      }
    }

    public CANASMsg()
    {
      ChangeDataType(DataType.NODATA);
      ServiceCode = 0;
    }

    public CANASMsg(float value)
      : this(value, 0)
    {
    }

    public CANASMsg(float value, byte serviceCode)
    {
      byte[] valueBytes = BitConverter.GetBytes(value);
      data[4] = valueBytes[0];
      data[5] = valueBytes[1];
      data[6] = valueBytes[2];
      data[7] = valueBytes[3];

      ChangeDataType(DataType.FLOAT);
      ServiceCode = serviceCode;
    }

    public CANASMsg(Int32 value)
      : this(value, 0)
    {
    }

    public CANASMsg(Int32 value, byte serviceCode)
    {
      byte[] valueBytes = BitConverter.GetBytes(value);
      data[4] = valueBytes[0];
      data[5] = valueBytes[1];
      data[6] = valueBytes[2];
      data[7] = valueBytes[3];

      ChangeDataType(DataType.LONG);
      ServiceCode = serviceCode;
    }

    public CANASMsg(UInt32 value)
      : this(value, 0)
    {
    }

    public CANASMsg(UInt32 value, byte serviceCode)
    {
      data[4] = (byte)(value >> 24);
      data[5] = (byte)(value >> 16);
      data[6] = (byte)(value >> 8);
      data[7] = (byte)value;
      ChangeDataType(DataType.ULONG);

      ServiceCode = serviceCode;
    }

    public CANASMsg(Int16 value)
      : this(value, 0)
    {
    }

    public CANASMsg(Int16 value, byte serviceCode)
    {
      data[4] = (byte)(value >> 8);
      data[5] = (byte)value;

      ChangeDataType(DataType.SHORT);
      ServiceCode = serviceCode;
    }

    public CANASMsg(UInt16 value)
      : this(value, 0)
    {
    }

    public CANASMsg(UInt16 value, byte serviceCode)
    {
      data[4] = (byte)(value >> 8);
      data[5] = (byte)value;

      ChangeDataType(DataType.ULONG);
      ServiceCode = serviceCode;
    }

    public CANASMsg(char value)
      : this(value, 0)
    {
    }

    public CANASMsg(char value, byte serviceCode)
    {
      data[4] = (byte)value;

      ChangeDataType(DataType.CHAR);
      ServiceCode = serviceCode;
    }

    public CANASMsg(byte value)
      : this(value, 0)
    {
    }

    public CANASMsg(byte value, byte serviceCode)
    {
      data[4] = value;

      ChangeDataType(DataType.UCHAR);
      ServiceCode = serviceCode;
    }

    public CANASMsg(byte value1, byte value2, byte serviceCode)
    {
      data[4] = value1;
      data[5] = value2;

      ChangeDataType(DataType.UCHAR2);
      ServiceCode = serviceCode;
    }

    public CANASMsg(byte value1, byte value2, byte value3, byte serviceCode)
    {
      data[4] = value1;
      data[5] = value2;
      data[6] = value3;

      ChangeDataType(DataType.UCHAR3);
      ServiceCode = serviceCode;
    }

    public CANASMsg(byte value1, byte value2, byte value3, byte value4, byte serviceCode)
    {
      data[4] = value1;
      data[5] = value2;
      data[6] = value3;
      data[7] = value4;

      ChangeDataType(DataType.UCHAR4);
      ServiceCode = serviceCode;
    }

    public CANASMsg(Int16 value1, Int16 value2)
      : this(value1, value2, 0)
    {
    }

    public CANASMsg(Int16 value1, Int16 value2, byte serviceCode)
    {
      data[4] = (byte)(value1 >> 8);
      data[5] = (byte)value1;
      data[4] = (byte)(value2 >> 8);
      data[5] = (byte)value2;


      ChangeDataType(DataType.SHORT2);

      ServiceCode = serviceCode;
    }

    public CANASMsg(UInt16 value1, UInt16 value2)
      : this(value1, value2, 0)
    {
    }

    public CANASMsg(UInt16 value1, UInt16 value2, byte serviceCode)
    {
      data[4] = (byte)(value1 >> 8);
      data[5] = (byte)value1;
      data[4] = (byte)(value2 >> 8);
      data[5] = (byte)value2;

      ChangeDataType(DataType.USHORT2);

      ServiceCode = serviceCode;
    }

    public void ChangeDataType(DataType newType)
    {
      switch(newType)
      {
        case DataType.NODATA:
          length = 4;
          break;
        case DataType.ERROR:
          length = 4;
          break;
        case DataType.FLOAT:
          length = 8;
          break;
        case DataType.LONG:
          length = 8;
          break;
        case DataType.ULONG:
          length = 8;
          break;
        case DataType.BLONG:
          length = 8;
          break;
        case DataType.SHORT:
          length = 6;
          break;
        case DataType.USHORT:
          length = 6;
          break;
        case DataType.BSHORT:
          length = 6;
          break;
        case DataType.CHAR:
          length = 5;
          break;
        case DataType.UCHAR:
          length = 5;
          break;
        case DataType.BCHAR:
          length = 5;
          break;
        case DataType.SHORT2:
          length = 8;
          break;
        case DataType.USHORT2:
          length = 8;
          break;
        case DataType.BSHORT2:
          length = 8;
          break;
        case DataType.CHAR4:
          length = 8;
          break;
        case DataType.UCHAR4:
          length = 8;
          break;
        case DataType.BCHAR4:
          length = 8;
          break;
        case DataType.CHAR2:
          length = 6;
          break;
        case DataType.UCHAR2:
          length = 6;
          break;
        case DataType.BCHAR2:
          length = 6;
          break;
        case DataType.MEMID:
          length = 8;
          break;
        case DataType.CHKSUM:
          length = 8;
          break;
        case DataType.ACHAR:
          length = 5;
          break;
        case DataType.ACHAR2:
          length = 6;
          break;
        case DataType.ACHAR4:
          length = 8;
          break;
        case DataType.CHAR3:
          length = 7;
          break;
        case DataType.UCHAR3:
          length = 7;
          break;
        case DataType.BCHAR3:
          length = 7;
          break;
        case DataType.ACHAR3:
          length = 7;
          break;
        case DataType.DOUBLEH:
          length = 8;
          break;
        case DataType.DOUBLEL:
          length = 8;
          break;
      }

      MessageDataType = newType;
    }

    private UInt16 ToUShort(byte hi, byte lo) { return (UInt16)((((UInt32)hi) * ((UInt32)256)) + ((UInt16)lo)); }
    private Int16 ToShort(byte hi, byte lo) { return (Int16)((((Int32)hi) * ((Int32)256)) + ((UInt16)lo)); ; }

    public float FLOAT
    {
      get
      {
        byte[] bigEndian = new byte[4];

        bigEndian[0] = data[7];
        bigEndian[1] = data[6];
        bigEndian[2] = data[5];
        bigEndian[3] = data[4];

        return BitConverter.ToSingle(bigEndian, 0);
      }
    }
    public Int32 LONG { get { return BitConverter.ToInt32(data, 4); } }
    public UInt32 ULONG { get { return BitConverter.ToUInt32(data, 4); } }
    public UInt32 BLONG { get { return BitConverter.ToUInt32(data, 4); } }

    public char CHAR { get { return BitConverter.ToChar(data, 4); } }
    public char[] CHAR2 { get { return new char[2] { BitConverter.ToChar(data, 4), BitConverter.ToChar(data, 5) }; } }
    public char[] CHAR3 { get { return new char[3] { BitConverter.ToChar(data, 4), BitConverter.ToChar(data, 5), BitConverter.ToChar(data, 6) }; } }
    public char[] CHAR4 { get { return new char[4] { BitConverter.ToChar(data, 4), BitConverter.ToChar(data, 5), BitConverter.ToChar(data, 6), BitConverter.ToChar(data, 7) }; } }

    public byte UCHAR { get { return data[4]; } }
    public byte[] UCHAR2 { get { return new byte[2] { data[4], data[5] }; } }
    public byte[] UCHAR3 { get { return new byte[3] { data[4], data[5], data[6] }; } }
    public byte[] UCHAR4 { get { return new byte[4] { data[4], data[5], data[6], data[7] }; } }

    public Int16 SHORT 
    { 
      get
      {
        byte[] bigEndian = new byte[2];
        bigEndian[0] = data[5];
        bigEndian[1] = data[4];

        return BitConverter.ToInt16(bigEndian, 0);
      }
    }
    public Int16[] SHORT2 { get { return new Int16[2] { ToShort(data[4], data[5]), ToShort(data[6], data[7]) }; } }

    public UInt16 USHORT { get { return BitConverter.ToUInt16(data, 4); } }
    public UInt16[] USHORT2 { get { return new UInt16[2] { ToUShort(data[4], data[5]), ToUShort(data[6], data[7]) }; } }

    public UInt32 MEMID { get { return BitConverter.ToUInt32(data, 4); } }
    public UInt32 CHKSUM { get { return BitConverter.ToUInt32(data, 4); } }

    public UInt32 DOUBLEH { get { return BitConverter.ToUInt32(data, 4); } }
    public UInt32 DOUBLEL { get { return BitConverter.ToUInt32(data, 4); } }

    public void Send(CANaerospace connection, UInt32 msgID, byte messageCode)
    {
      MessageCode = messageCode;
      NodeID = connection.NodeID;
      Reply(connection, msgID);
    }

    public void Send(CANaerospace connection, UInt32 msgID, byte nodeID, byte messageCode)
    {
      MessageCode = messageCode;
      NodeID = nodeID;
      Reply(connection, msgID);
    }

    public void Send(CANaerospace connection, UInt32 msgID)
    {
      MessageCode = ++nextMessageCode;
      NodeID = connection.NodeID;
      Reply(connection, msgID);
    }

    public void Reply(CANaerospace connection, UInt32 msgID)
    {
      address = msgID;
      connection.SendMessage(this);
    }

    private static readonly string sendCommand = "t{0:X3}{1:X1}";
    private static readonly string sendCommandEx = "T{0:X8}{1:X1}";

    public override string ToString()
    {
      StringBuilder sb = new StringBuilder();
      sb.Append(String.Format(extendedAddress ? sendCommandEx : sendCommand, address, length));

      for(int i = 0; i < length; i++)
        sb.Append(String.Format("{0:X2}", data[i]));

      return sb.ToString();
    }

    public string DisplayString()
    {
      return DisplayString(0);
    }

    public string DisplayString(float offset)
    {
      string result;
      switch (MessageDataType)
      {
        case DataType.ULONG :
          result = String.Format("{0}", ULONG + ((UInt32)offset));
          break;
        case DataType.SHORT :
          result = String.Format("{0}", SHORT + ((short) offset));
          break;
        case DataType.FLOAT :
          result = String.Format("{0}", FLOAT + offset);
          break;
        default :
          result = "----";
          break;
      }

      return result;
    }
  };
}
