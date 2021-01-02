using System;
using CanFly;
using System.Runtime.CompilerServices;

// ReSharper disable InconsistentNaming

namespace CanFly
{
  /// <summary>
  /// Class representing a point and elevation above a WGS-84 ellipsoid.
  /// </summary>
  public enum FieldType {
    field_none,                     // no definition
    field_key,											// a key is a parent of other fields
    field_bool,                     // a boolean field (see notes)
    field_int16,
    field_uint16,
    field_int32,
    field_uint32,
    field_float,
    field_stream,                   // a stream field.  needs stream functions
  };

  public delegate void MsgHookFn(CanFlyMsg msg, object arg);

  public sealed class Neutron
  {
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegCreateKey(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegOpenKey(uint parent, string name);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegEnumKey(uint key, ref FieldType type, 
      out ushort length, out byte[] data, 
      out string  name, out uint child);

    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegDeleteKey(uint key);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegDeleteValue(uint key, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void ReqQueryKey(uint key, out FieldType type, out string name, out ushort length, out uint parent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegQueryChild(uint parent, string name, out uint memid, out FieldType type, out ushort length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegRenameValue(uint parent, string name, string newName);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern sbyte RegGetInt8(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt8(uint parent, string name, sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern byte RegGetUint8(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint8(uint parent, string name, byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern short RegGetInt16(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt16(uint parent, string name, short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ushort RegGetUint16(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint16(uint parent, string name, ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetInt32(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt32(uint parent, string name, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegGetUint32(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint32(uint parent, string name, uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string RegGetString(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetString(uint parent, string name, string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern float RegGetFloat(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetFloat(uint parent, string name, float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool RegGetBool(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetBool(uint parent, string name, bool value);
    // Registry stream functions
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamOpen(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamCreate(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamClose(uint  stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamDelete(uint  stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool RegStreamEof(uint  stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern byte[] RegStreamRead(uint  stream, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamWrite(uint  stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamGetPos(uint  stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamSetPos(uint  stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegStreamLength(uint  stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamTruncate(uint  stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegStreamCopy(uint  from, uint  to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string RegStreamPath(uint  stream, bool full_path);
  }
}