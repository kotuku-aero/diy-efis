//
// Copyright (c) .NET Foundation and Contributors
// Original work from Oleg Rakhmatulin.
// See LICENSE file in the project root for full license information.
//

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace CanFly.Tools.MetadataProcessor
{
  /// <summary>
  /// Binary writer for Framework assemblies emitting. Supports different endianness.
  /// </summary>
  public abstract class CLRBinaryWriter
  {
    /// <summary>
    /// Specialized big endian version of <see cref="CLRBinaryWriter"/> class.
    /// </summary>
    private sealed class BigEndianBinaryWriter : CLRBinaryWriter
    {
      /// <summary>
      /// Emulates C++ union using explicit fields layout (with same zero offset)
      /// </summary>
      [StructLayout(LayoutKind.Explicit)]
      private struct BytesMappingHelper
      {
        [FieldOffset(0)]
        public ushort uint16Value;

        [FieldOffset(0)]
        public uint uint32Value;

        [FieldOffset(0)]
        public ulong uint64Value;

        [FieldOffset(0)]
        public float singleValue;

        [FieldOffset(0)]
        public double doubleValue;

        [FieldOffset(0)]
        public unsafe fixed byte bytes[8];
      }

      private BytesMappingHelper _helper;

      /// <summary>
      /// Creates new instance of <see cref="CLRBinaryWriter.BigEndianBinaryWriter"/> object.
      /// </summary>
      /// <param name="baseWriter">Base binary writer for operating on stream.</param>
      public BigEndianBinaryWriter(
          BinaryWriter baseWriter)
          : base(baseWriter)
      {
      }

      /// <inheritdoc/>
      public override unsafe void WriteUInt16(ushort value)
      {
        _helper.uint16Value = value;
        fixed (byte* pBytes = _helper.bytes)
        {
          _baseWriter.Write(pBytes[1]);
          _baseWriter.Write(pBytes[0]);
        }
      }

      /// <inheritdoc/>
      public override unsafe void WriteUInt32(uint value)
      {
        _helper.uint32Value = value;
        fixed (byte* pBytes = _helper.bytes)
        {
          _baseWriter.Write(pBytes[3]);
          _baseWriter.Write(pBytes[2]);
          _baseWriter.Write(pBytes[1]);
          _baseWriter.Write(pBytes[0]);
        }
      }

      /// <inheritdoc/>
      public override unsafe void WriteUInt64(ulong value)
      {
        _helper.uint64Value = value;
        fixed (byte* pBytes = _helper.bytes)
        {
          _baseWriter.Write(pBytes[7]);
          _baseWriter.Write(pBytes[6]);
          _baseWriter.Write(pBytes[5]);
          _baseWriter.Write(pBytes[4]);
          _baseWriter.Write(pBytes[3]);
          _baseWriter.Write(pBytes[2]);
          _baseWriter.Write(pBytes[1]);
          _baseWriter.Write(pBytes[0]);
        }
      }

      /// <inheritdoc/>
      public override unsafe void WriteSingle(float value)
      {
        _helper.singleValue = value;
        fixed (byte* pBytes = _helper.bytes)
        {
          _baseWriter.Write(pBytes[3]);
          _baseWriter.Write(pBytes[2]);
          _baseWriter.Write(pBytes[1]);
          _baseWriter.Write(pBytes[0]);
        }
      }

      /// <inheritdoc/>
      public override unsafe void WriteDouble(double value)
      {
        _helper.doubleValue = value;
        fixed (byte* pBytes = _helper.bytes)
        {
          _baseWriter.Write(pBytes[7]);
          _baseWriter.Write(pBytes[6]);
          _baseWriter.Write(pBytes[5]);
          _baseWriter.Write(pBytes[4]);
          _baseWriter.Write(pBytes[3]);
          _baseWriter.Write(pBytes[2]);
          _baseWriter.Write(pBytes[1]);
          _baseWriter.Write(pBytes[0]);
        }
      }

      /// <inheritdoc/>
      public override CLRBinaryWriter GetMemoryBasedClone(
          MemoryStream stream)
      {
        return new BigEndianBinaryWriter(new BinaryWriter(stream));
      }
    }

    /// <summary>
    /// Specialized little endian version of <see cref="CLRBinaryWriter"/> class.
    /// </summary>
    private sealed class LittleEndianBinaryWriter : CLRBinaryWriter
    {
      /// <summary>
      /// Creates new instance of <see cref="CLRBinaryWriter.LittleEndianBinaryWriter"/> object.
      /// </summary>
      /// <param name="baseWriter">Base binary writer for operating on stream.</param>
      public LittleEndianBinaryWriter(
          BinaryWriter baseWriter)
          : base(baseWriter)
      {
      }

      /// <inheritdoc/>
      public override void WriteUInt16(ushort value)
      {
        _baseWriter.Write(value);
      }

      /// <inheritdoc/>
      public override void WriteUInt32(uint value)
      {
        _baseWriter.Write(value);
      }

      /// <inheritdoc/>
      public override void WriteUInt64(ulong value)
      {
        _baseWriter.Write(value);
      }

      /// <inheritdoc/>
      public override void WriteSingle(float value)
      {
        _baseWriter.Write(value);
      }

      /// <inheritdoc/>
      public override void WriteDouble(double value)
      {
        _baseWriter.Write(value);
      }

      /// <inheritdoc/>
      public override CLRBinaryWriter GetMemoryBasedClone(
          MemoryStream stream)
      {
        return new LittleEndianBinaryWriter(new BinaryWriter(stream));
      }
    }

    /// <summary>
    /// Base binary writer instance for performing basic operation on underlying byte stream.
    /// By design <see cref="BinaryWriter"/> is always little endian regardless of platform.
    /// </summary>
    private readonly BinaryWriter _baseWriter;

    /// <summary>
    /// Creates new instance of <see cref="CLRBinaryWriter"/> object.
    /// </summary>
    /// <param name="baseWriter">Base binary writer for operating on stream.</param>
    protected CLRBinaryWriter(
        BinaryWriter baseWriter)
    {
      _baseWriter = baseWriter;
    }

    /// <summary>
    /// Factory mathod for creating little endian version of <see cref="CLRBinaryWriter"/> class.
    /// </summary>
    /// <param name="baseWriter">Base binary writer for operating on stream.</param>
    /// <returns>
    /// Instance of <see cref="CLRBinaryWriter"/> which writes bytes in little endian.
    /// </returns>
    public static CLRBinaryWriter CreateLittleEndianBinaryWriter(BinaryWriter baseWriter)
    {
      return new LittleEndianBinaryWriter(baseWriter);
    }

    /// <summary>
    /// Factory mathod for creating big endian version of <see cref="CLRBinaryWriter"/> class.
    /// </summary>
    /// <param name="baseWriter">Base binary writer for operating on stream.</param>
    /// <returns>
    /// Instance of <see cref="CLRBinaryWriter"/> which writes bytes in big endian.
    /// </returns>
    public static CLRBinaryWriter CreateBigEndianBinaryWriter(BinaryWriter baseWriter)
    {
      return new BigEndianBinaryWriter(baseWriter);
    }

    /// <summary>
    /// Write single unsigned byte into underlying stream.
    /// </summary>
    /// <param name="value">Unsigned byte value for writing.</param>
    public void WriteByte(byte value)
    {
      _baseWriter.Write(value);
    }

    /// <summary>
    /// Write single signed byte into underlying stream.
    /// </summary>
    /// <param name="value">Signed byte value for writing.</param>
    public void WriteSByte(sbyte value)
    {
      _baseWriter.Write(value);
    }

    /// <summary>
    /// Write version information into underlying stream.
    /// </summary>
    /// <param name="value">Version information value for writing.</param>
    public void WriteVersion(Version value)
    {
      WriteUInt16((ushort)value.Major);
      WriteUInt16((ushort)value.Minor);
      WriteUInt16((ushort)value.Build);
      WriteUInt16((ushort)value.Revision);
    }

    /// <summary>
    /// Write raw string value (in UTF-8 encoding) into underlying stream.
    /// </summary>
    /// <param name="value">String value for writing.</param>
    public void WriteString(string value)
    {
      _baseWriter.Write(Encoding.UTF8.GetBytes(value));
      WriteByte(0);
    }

    /// <summary>
    /// Write raw bytes array into underlying stream.
    /// </summary>
    /// <param name="value">Raw bytes array for writing.</param>
    public void WriteBytes(byte[] value)
    {
      _baseWriter.Write(value);
    }

    /// <summary>
    /// Write single signed word into underlying stream.
    /// </summary>
    /// <param name="value">Signed word value for writing.</param>
    public void WriteInt16(short value)
    {
      WriteUInt16((ushort)value);
    }

    /// <summary>
    /// Write single signed double word into underlying stream.
    /// </summary>
    /// <param name="value">Signed double word value for writing.</param>
    public void WriteInt32(int value)
    {
      WriteUInt32((uint)value);
    }

    /// <summary>
    /// Write single signed quad word into underlying stream.
    /// </summary>
    /// <param name="value">Signed quad word value for writing.</param>
    public void WriteInt64(long value)
    {
      WriteUInt64((ulong)value);
    }

    /// <summary>
    /// Write metadata token in packed format (variable length).
    /// </summary>
    /// <param name="value">Metadata tocken in .NET Mico Framework format.</param>
    public void WriteMetadataToken(uint value)
    {
      if (value <= 0x7F)
      {
        WriteByte((byte)value);
      }
      else if (value <= 0x3FFF)
      {
        WriteByte((byte)(value >> 8 | 0x80));
        WriteByte((byte)value);
      }
      else
      {
        WriteByte((byte)(value >> 24 | 0xC0));
        WriteByte((byte)(value >> 16));
        WriteByte((byte)(value >> 8));
        WriteByte((byte)value);
      }
    }

    /// <summary>
    /// Write single unsigned word into underlying stream.
    /// </summary>
    /// <param name="value">Unsigned word value for writing.</param>
    public abstract void WriteUInt16(ushort value);

    /// <summary>
    /// Write single signed double word into underlying stream.
    /// </summary>
    /// <param name="value">Unsigned double word value for writing.</param>
    public abstract void WriteUInt32(uint value);

    /// <summary>
    /// Write single signed quad word into underlying stream.
    /// </summary>
    /// <param name="value">Unsigned quad word value for writing.</param>
    public abstract void WriteUInt64(ulong value);

    /// <summary>
    /// Write single floating point value (4 bytes) into underlying stream.
    /// </summary>
    /// <param name="value">Floating point value for writing.</param>
    public abstract void WriteSingle(float value);

    /// <summary>
    /// Write single floating point value (8 bytes) into underlying stream.
    /// </summary>
    /// <param name="value">Floating point value for writing.</param>
    public abstract void WriteDouble(double value);

    /// <summary>
    /// Creates new instance of <see cref="CLRBinaryWriter"/> object with same endiannes
    /// as current instance but based on new base stream <paramref name="stream"/>.
    /// </summary>
    /// <param name="stream">Base binary writer for operating on stream for new writer.</param>
    /// <returns>New instance of <see cref="CLRBinaryWriter"/> object with same endiannes.</returns>
    public abstract CLRBinaryWriter GetMemoryBasedClone(MemoryStream stream);

    /// <summary>
    /// Gets base stream for this binary writer object (used for changing stream position).
    /// </summary>
    public Stream BaseStream { get { return _baseWriter.BaseStream; } }

    /// <summary>
    /// Returns <c>true</c> in case of this binary writer is write data in big endian format.
    /// </summary>
    public bool IsBigEndian { get { return (this is BigEndianBinaryWriter); } }
  }
}
