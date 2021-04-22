/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;

// ReSharper disable InconsistentNaming

namespace CanFly
{

public enum CanFlyDataType 
{
  NoData = 0,
  Error = 1,
  Float = 2,
  Int32 = 3,
  UInt32 = 4,
  Short = 6,
  UShort = 7,
  Char = 9,
  UChar = 10,
  Short2 = 12,
  UShort2 = 13,
  Char4 = 15,
  UChar4 = 16,
  Char2 = 18,
  UChar2 = 19,
  Char3 = 26,
  UChar3 = 27,
};

public sealed class CanFlyMsg
  {
    private static byte nodeId = 0;
    /// <summary>
    /// The global node id for the canfly application
    /// </summary>
    /// <value>Node ID</value>
    public static byte NodeID
    {
      get { return nodeId; }
    }
    /// <summary>
    /// Flag for a reply message
    /// </summary>
    public bool Reply
    {
      get { return Syscall.GetReply(this); }
    }
    /// <summary>
    /// ID of the CAN message
    /// </summary>
    public ushort CanID
    {
      get { return Syscall.GetCanID(this); }
    }
    /// <summary>
    /// CanFly node id
    /// </summary>
    public byte NodeId 
    { 
      get { return Syscall.GetNodeID(this); } 
    }
    /// <summary>
    /// The type of data
    /// </summary>
    public CanFlyDataType DataType 
    {
      get { return Syscall.GetDataType(this); }
     }
    /// <summary>
    /// The service code
    /// </summary>
    internal byte ServiceCode 
    {
      get { return Syscall.GetServiceCode(this); } 
    }
    /// <summary>
    /// The message code
    /// </summary>
    internal byte MessageCode 
    {
      get { return Syscall.GetMessageCode(this); } 
    }
    /// <summary>
    /// Create a message with the NoData type
    /// </summary>
    /// <param name="id">Message ID</param>
    public static CanFlyMsg Create(ushort id)
    {
      return Syscall.CreateMessage(nodeId, id);
    }
    /// <summary>
    /// Create an error message
    /// </summary>
    /// <param name="id">Can ID of the message</param>
    /// <param name="value">Value to report as an error</param>
    /// <returns>Constructed message</returns>
    public static CanFlyMsg CreateErrorMessage(ushort id, uint error)
    {
      return Syscall.CreateErrorMessage(nodeId, id, error);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public static CanFlyMsg Create(ushort id, uint value)
    {
      return Syscall.CreateMessage(nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public static CanFlyMsg Create(ushort id, int value)
    {
      return Syscall.CreateMessage(nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public static CanFlyMsg Create(ushort id, ushort value)
    {
      return Syscall.CreateMessage(nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public static CanFlyMsg Create(ushort id, short value)
    {
      return Syscall.CreateMessage(nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v1">value1 to send</param>
    /// <param name="v2">value2 to send</param>
    public static CanFlyMsg Create(ushort id, ushort v1, ushort v2)
    {
      return Syscall.CreateMessage(nodeId, id, v1, v2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v1">value1 to send</param>
    /// <param name="v2">value2 to send</param>
    public static CanFlyMsg Create(ushort id, short v1, short v2)
    {
       return Syscall.CreateMessage(nodeId, id, v1, v2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public static CanFlyMsg Create(ushort id, float v)
    {
      return Syscall.CreateMessage(nodeId, id, v);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    public static CanFlyMsg Create(ushort id, sbyte b0)
    {
      return Syscall.CreateMessage(nodeId, id, b0);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    public static CanFlyMsg Create(ushort id, byte b0)
    {
      return Syscall.CreateMessage(nodeId, id, b0);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    public static CanFlyMsg Create(ushort id, byte b0, byte b1)
    {
      return Syscall.CreateMessage(nodeId, id, b0, b1);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    public static CanFlyMsg Create(ushort id, sbyte b0, sbyte b1)
    {
      return Syscall.CreateMessage(nodeId, id, b0, b1);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    public static CanFlyMsg Create(ushort id, byte b0, byte b1, byte b2)
    {
      return Syscall.CreateMessage(nodeId, id, b0, b1, b2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    public static CanFlyMsg Create(ushort id, sbyte b0, sbyte b1, sbyte b2)
    {
      return Syscall.CreateMessage(nodeId, id, b0, b1, b2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    /// <param name="b3">value to assign</param>
    public static CanFlyMsg Create(ushort id, byte b0, byte b1, byte b2, byte b3)
    {
      return Syscall.CreateMessage(nodeId, id, b0, b1, b2, b3);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    /// <param name="b3">value to assign</param>
    public static CanFlyMsg Create(ushort id, sbyte b0, sbyte b1, sbyte b2, sbyte b3)
    {
      return Syscall.CreateMessage(nodeId, id, b0, b1, b2, b3);
    }
    /// <summary>
    /// Get the float value encapsulated
    /// </summary>
    /// <returns>Value as a float</returns>
    public float GetFloat()
    {
      return Syscall.GetFloat(this);
    }
    /// <summary>
    /// Get the value as an int32
    /// </summary>
    /// <returns>Value as an Int32</returns>
    public int GetInt32()
    {
      return Syscall.GetInt32(this);
    }
    /// <summary>
    /// Get the value as a uint
    /// </summary>
    /// <returns>Value as an UInt32</returns>
    public uint GetUInt32()
    {
      return Syscall.GetUInt32(this);
    }
    /// <summary>
    /// Gets the value as a short
    /// </summary>
    /// <returns>Value as a Int16</returns>
    public short GetInt16()
    {
      return Syscall.GetInt16(this);
    }
    /// <summary>
    /// Gets the value as a ushort
    /// </summary>
    /// <returns>Value as a UInt16</returns>
    public ushort GetUInt16()
    {
      return Syscall.GetUInt16(this);
    }
    /// <summary>
    /// Return the value as a signed byte
    /// </summary>
    /// <returns>Value as a Int8</returns>
    public sbyte[] GetInt8Array()
    {
      return Syscall.GetInt8Array(this);
    }
    /// <summary>
    /// Return the value as a byte
    /// </summary>
    /// <returns>Value as a UInt7</returns>
    public byte[] GetUInt8Array()
    {
      return Syscall.GetUInt8Array(this);
    }
    /// <summary>
    /// Return the value as an array of Int16
    /// </summary>
    /// <returns>Values as an array of Int16</returns>
    public short[] GetInt16Array()
    {
      return Syscall.GetInt16Array(this);
    }
    /// <summary>
    /// Return the value as an array of ushorts
    /// </summary>
    /// <returns>Values as an array of UInt16</returns>
    public ushort[] GetUInt16Array()
    {
      return Syscall.GetUInt16Array(this);
    }

    public override string ToString()
    {
      return Syscall.MessageToString(this);
    }
  }
}