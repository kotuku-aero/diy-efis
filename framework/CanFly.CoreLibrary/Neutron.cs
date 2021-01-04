using System.Runtime.CompilerServices;

// ReSharper disable InconsistentNaming

namespace CanFly
{
  public sealed class Neutron
  {
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegCreateKey(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegOpenKey(uint parent, string name);
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
  }
}