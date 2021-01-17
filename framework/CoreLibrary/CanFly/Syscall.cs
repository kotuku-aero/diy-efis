using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  /// <summary>
  /// Class holding all system calls.  The order of this class never changes.
  /// Methods can be added to the end of it, but never removed.
  /// </summary>
  internal static class Syscall
  {
    /*******************************************************/
    // CanFly operating system routines
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint RegCreateKey(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint RegOpenKey(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern sbyte RegGetInt8(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetInt8(uint parent, string name, sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte RegGetUint8(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetUint8(uint parent, string name, byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short RegGetInt16(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetInt16(uint parent, string name, short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort RegGetUint16(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetUint16(uint parent, string name, ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RegGetInt32(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetInt32(uint parent, string name, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint RegGetUint32(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetUint32(uint parent, string name, uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string RegGetString(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetString(uint parent, string name, string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float RegGetFloat(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetFloat(uint parent, string name, float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool RegGetBool(uint parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RegSetBool(uint parent, string name, bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string RegEnumKey(uint key, ref uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint OpenScreen(ushort orientation, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateWindow(uint parent, int left, int top, int right, int bottom, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateChildWindow(uint parent, int left, int top, int right, int bottom, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CloseWindow(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SendMessage(uint hwnd, ushort id, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PostMessage(uint hend, ushort id, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowRect(uint window, out int left, out int top, out int right, out int bottom);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowPos(uint window, out int left, out int top, out int right, out int bottom);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowPos(uint window, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern object GetWindowData(uint window);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowData(uint window, object wnd_data);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetParent(uint window);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetWindowById(uint parent, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetFirstChild(uint parent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetNextSibling(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetPreviousSibling(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InsertBefore(uint handle, uint sibling);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InsertAfter(uint handle, uint sibling);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetZOrder(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetZOrder(uint handle, byte z_order);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CanvasClose(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateRectCanvas(int dx, int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreatePngCanvas(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void LoadPng(uint canvas, uint stream, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetCanvasExtents(uint canvas, out ushort bpp, out int dx, out int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetOrientation(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetOrientation(uint hwnd, ushort orientation);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreatePen(uint color, ushort width, ushort style);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetPenColor(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetPenWidth(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetPenStyle(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DisposePen(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Polyline(uint canvas,
      int clipLeft, int clipTop, int clipRight, int clipBottom,
      uint penHandle, uint pointsHandle);
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
    internal static extern uint GetPixel(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, int pt_x, int pt_y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint SetPixel(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, int pt_x, int pt_y, uint c);
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
    internal static extern uint OpenFont(string name, ushort pixels);
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
    internal static extern bool IsValid(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void BeginPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void EndPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool GetMessage(uint hwnd, out uint msg_for, ref ushort flags, ref byte b0, ref byte b1, ref byte b2, ref byte b3, ref byte b4, ref byte b5, ref byte b6, ref byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DispatchMessage(uint hwnd, ushort routing_flags, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PostMessage(uint hwnd, uint max_wait, ushort routing_flags, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Send(ushort routing_flags, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SendRaw(ushort routing_flags, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SendReply(ushort routing_flags, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetFloat(byte b0, byte b1, byte b2, byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetInt32(byte b0, byte b1, byte b2, byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetUInt32(byte b0, byte b1, byte b2, byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetInt16(byte b0, byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetUInt16(byte b0, byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PackFloat(float value, ref byte b0, ref byte b1, ref byte b2, ref byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PackInt32(int value, ref byte b0, ref byte b1, ref byte b2, ref byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PackUInt32(uint value, ref byte b0, ref byte b1, ref byte b2, ref byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PackInt16(short value, ref byte b0, ref byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PackUInt16(ushort value, ref byte b0, ref byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool StreamEof(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte[] StreamRead(uint stream, ushort size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint StreamGetPos(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint StreamLength(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string StreamPath(uint stream, bool full_path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void StreamDelete(uint stream);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CreateDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RemoveDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetDirectoryEnumerator(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint FileStreamOpen(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint FileStreamCreate(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint RegStreamOpen(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint RegStreamCreate(uint parent, string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool GetDirectoryEntry(uint hndl, ref string path, ref string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RewindDirectoryEntry(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint GetFont(string fontName, uint pointSize);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ReleaseFont(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void LoadFont(uint streamHandle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint PointArrayCreate(uint numPoints);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayRelease(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArraySetPoint(uint handle, uint index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayClear(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint PointArraySize(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayResize(uint handle, uint size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint PointArrayAppend(uint handle, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayInsertAt(uint handle, uint index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayGetPoint(uint handle, uint index, out int x, out int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayIndexOf(uint handle, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PointArrayRemoveAt(uint handle, uint index);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void IncarnatePhoton();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void AddEvent(uint handle, ushort id);
  }
}
