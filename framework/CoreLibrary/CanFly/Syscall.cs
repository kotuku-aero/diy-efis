using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public delegate void ThreadStart();

  public delegate void CanFlyEventHandler(CanFlyMsg msg);
  public class Syscall
  {
    /*******************************************************/
    // CanFly operating system routines
    //
    // These are simple wrappers around the canfly calls.  the uint returned
    // is a CanFly error code.
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SystemInformation(out ulong machine_id, out uint version);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Ticks(out uint ticks);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Yield();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateThread(byte priority, string name, ThreadStart start, object arg, out uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadGetArg(uint id, out object arg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadGetCurrentId(out uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadSuspend(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadResume(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetThreadPriority(uint id, out byte priority);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetThreadPriority(uint id, byte priority);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadSleep(uint id, uint millisecondsTimeout);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreCreate(out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreClose(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreSignal(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreWait(uint handle, uint milliseconds);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InterlockedIncrement(ref int location, out int old_value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InterlockedDecrement(ref int location, out int old_value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegCreateKey(ushort parent, string name, out ushort key);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegOpenKey(ushort parent, string name, out ushort key);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetInt8(ushort parent, string name, out sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt8(ushort parent, string name, sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetUint8(ushort parent, string name, out byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint8(ushort parent, string name, byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetInt16(ushort parent, string name, out short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt16(ushort parent, string name, short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetUint16(ushort parent, string name, out ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint16(ushort parent, string name, ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetInt32(ushort parent, string name, out int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt32(ushort parent, string name, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetUint32(ushort parent, string name, out uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint32(ushort parent, string name, uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetString(ushort parent, string name, out string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetString(ushort parent, string name, string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetFloat(ushort parent, string name, out float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetFloat(ushort parent, string name, float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegGetBool(ushort parent, string name, out bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetBool(ushort parent, string name, bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegEnumKey(ushort key, ref ushort child, out string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void OpenScreen(ushort orientation, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateWindow(uint parent, int left, int top, int right, int bottom, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateChildWindow(uint parent, int left, int top, int right, int bottom, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CloseWindow(uint hwnd);
    /// <summary>
    /// Process the message queue
    /// </summary>
    /// <param name="screen">Screen to receive windows messages from</param>
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ProcessMessages(uint screen);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowRect(uint window, out int left, out int top, out int right, out int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowPos(uint window, out int left, out int top, out int right, out int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowPos(uint window, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowData(uint window, out object widget);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowData(uint window, CanFlyEventHandler wndproc);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void AddWidgetEvent(uint window, ushort canId);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetParent(uint window, out uint parent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowById(uint parent, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetFirstChild(uint parent, out uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetNextSibling(uint handle, out uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetPreviousSibling(uint handle, out uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InsertBefore(uint handle, uint sibling);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InsertAfter(uint handle, uint sibling);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetZOrder(uint handle, out byte z_order);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetZOrder(uint handle, byte z_order);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CanvasClose(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CanvasRender(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateRectCanvas(int dx, int dy, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreatePngCanvas(uint stream, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateGeospatialCanvas(uint stream, uint key, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void LoadPng(uint canvas, uint stream, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetCanvasExtents(uint canvas, out ushort bpp, out int dx, out int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetOrientation(uint hwnd, out ushort orientation);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetOrientation(uint hwnd, ushort orientation);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreatePen(uint color, ushort width, ushort style, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetPenColor(uint handle, out uint color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetPenWidth(uint handle, out ushort width);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetPenStyle(uint handle, out ushort style);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DisposePen(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Polyline(uint canvas,
      int clipLeft, int clipTop, int clipRight, int clipBottom,
      uint penHandle, uint pointsHandle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Line(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
      uint penHandle, int from_x, int from_y, int to_x, int to_y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Ellipse(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
      uint pen, uint color, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Polygon(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, uint pen, uint color, uint points);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Rectangle(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, uint pen, uint color,
      int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RoundRect(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, uint pen, uint color,
      int left, int top, int right, int bottom, int radius);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void BitBlt(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
      int dest_left, int dest_top, int dest_right, int dest_bottom,
      uint src_canvas, int src_clip_left, int src_clip_top, int src_clip_right, int src_clip_bottom,
      int src_pt_x, int src_pt_y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetPixel(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, int pt_x, int pt_y, out uint color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetPixel(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, int pt_x, int pt_y, uint c, out uint old_pixel);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Arc(uint canvas,
                              int clipLeft, int clipTop, int clipRight, int clipBottom,
                              uint pen,
                              int pt_x, int pt_y,
                              int radius,
                              int start,
                              int end);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Pie(uint canvas,
                              int clipLeft, int clipTop, int clipRight, int clipBottom,
                              uint pen,
                              uint color,
                              int pt_x, int pt_y,
                              int start,
                              int end,
                              int radii,
                              int inner);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void OpenFont(string name, ushort pixels, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DrawText(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
                                    uint font,
                                    uint fg,
                                    uint bg,
                                    string str,
                                    int pt_x, int pt_y,
                                    int txt_clip_left, int txt_clip_top, int txt_clip_right, int txt_clip_bottom,
                                    ushort format);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void TextExtent(uint canvas, uint font, string str, out int dx, out int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InvalidateRect(uint hwnd, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void IsInvalid(uint hwnd, out bool is_invalid);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void BeginPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void EndPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateErrorMessage(CanFlyMsg msg, byte nodeId, ushort id, uint error);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, byte b0);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, byte b0, byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, byte b0, byte b1, byte b2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, byte b0, byte b1, byte b2, byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, sbyte c1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, sbyte c1, sbyte c2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, sbyte c1, sbyte c2, sbyte c3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, sbyte c1, sbyte c2, sbyte c3, sbyte c4);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, ushort v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, ushort v1, ushort v2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, short v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, short v1, short v2);
     [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, uint v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, uint v1, uint v2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, int v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, int v1, int v2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateMessage(CanFlyMsg msg, byte nodeId, ushort id, float v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string MessageToString(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetFloat(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetInt32(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetUInt32(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetInt16(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetUInt16(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern sbyte[] GetInt8Array(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] GetUInt8Array(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short[] GetInt16Array(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort[] GetUInt16Array(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool GetReply(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetReply(CanFlyMsg msg, bool isReply);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetCanID(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetCanID(CanFlyMsg msg, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetNodeID(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetNodeID(CanFlyMsg msg, byte id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetServiceCode(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetServiceCode(CanFlyMsg msg, byte id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetMessageCode(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetMessageCode(CanFlyMsg msg, byte id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyDataType GetDataType(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SendMessage(uint hndl, CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PostMessage(uint hndl, uint maxWait, CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Send(CanFlyMsg msg);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamEof(uint stream, out bool is_eof);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamRead(uint stream, ushort size, out byte[] value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamGetPos(uint stream, out uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamLength(uint stream, out uint lengt);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamPath(uint stream, bool full_path, out string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamDelete(uint stream);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RemoveDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetDirectoryEnumerator(string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void FileStreamOpen(string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void FileStreamCreate(string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegStreamOpen(uint parent, string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegStreamCreate(uint parent, string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetDirectoryEntry(uint hndl, out string path, out string name, out bool isFile);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RewindDirectoryEntry(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetFont(string fontName, ushort pointSize, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ReleaseFont(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void LoadFont(uint streamHandle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayCreate(ushort numPoints, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayRelease(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArraySetPoint(uint handle, ushort index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayClear(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArraySize(uint handle, out ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayResize(uint handle, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayAppend(uint handle, int x, int y, out ushort new_size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayInsertAt(uint handle, ushort index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayGetPoint(uint handle, ushort index, out int x, out int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayIndexOf(uint handle, int x, int y, out int index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayRemoveAt(uint handle, ushort index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void AddEvent(ushort canId, CanFlyEventHandler handler);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RemoveEvent(ushort canID, CanFlyEventHandler handler);
  }
}
