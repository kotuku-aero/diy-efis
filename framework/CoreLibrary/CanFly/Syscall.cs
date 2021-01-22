using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public delegate void CanFlyEventHandler(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
  public static class Syscall
  {
    /*******************************************************/
    // CanFly operating system routines
    //
    // These are simple wrappers around the canfly calls.  the uint returned
    // is a CanFly error code.
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegCreateKey(uint parent, string name, out uint key);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegOpenKey(uint parent, string name, out uint key);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetInt8(uint parent, string name, out sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetInt8(uint parent, string name, sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetUint8(uint parent, string name, out byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetUint8(uint parent, string name, byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetInt16(uint parent, string name, out short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetInt16(uint parent, string name, short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetUint16(uint parent, string name, out ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetUint16(uint parent, string name, ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetInt32(uint parent, string name, out int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetInt32(uint parent, string name, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetUint32(uint parent, string name, out uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetUint32(uint parent, string name, uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetString(uint parent, string name, out string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetString(uint parent, string name, string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetFloat(uint parent, string name, out float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetFloat(uint parent, string name, float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetBool(uint parent, string name, out bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegSetBool(uint parent, string name, bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegEnumKey(uint key, ref uint child, out string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int OpenScreen(ushort orientation, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CreateWindow(uint parent, int left, int top, int right, int bottom, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CreateChildWindow(uint parent, int left, int top, int right, int bottom, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CloseWindow(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetWindowRect(uint window, out int left, out int top, out int right, out int bottom);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetWindowPos(uint window, out int left, out int top, out int right, out int bottom);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SetWindowPos(uint window, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetWindowData(uint window, out object data);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SetWindowData(uint window, object wnd_data, CanFlyEventHandler wndproc);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int AddWidgetEvent(uint window, ushort canId);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetParent(uint window, out uint parent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetWindowById(uint parent, ushort id, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetFirstChild(uint parent, out uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetNextSibling(uint handle, out uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetPreviousSibling(uint handle, out uint child);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int InsertBefore(uint handle, uint sibling);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int InsertAfter(uint handle, uint sibling);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetZOrder(uint handle, out byte z_order);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SetZOrder(uint handle, byte z_order);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CanvasClose(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CreateRectCanvas(int dx, int dy, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CreatePngCanvas(uint stream, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int LoadPng(uint canvas, uint stream, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetCanvasExtents(uint canvas, out ushort bpp, out int dx, out int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetOrientation(uint hwnd, out ushort orientation);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SetOrientation(uint hwnd, ushort orientation);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CreatePen(uint color, ushort width, ushort style, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetPenColor(uint handle, out uint color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetPenWidth(uint handle, out ushort width);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetPenStyle(uint handle, out ushort style);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int DisposePen(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Polyline(uint canvas,
      int clipLeft, int clipTop, int clipRight, int clipBottom,
      uint penHandle, uint pointsHandle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Ellipse(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
      uint pen, uint color, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Polygon(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, uint pen, uint color, uint points);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Rectangle(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, uint pen, uint color,
      int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RoundRect(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, uint pen, uint color,
      int left, int top, int right, int bottom, int radius);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int BitBlt(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
      int dest_left, int dest_top, int dest_right, int dest_bottom,
      uint src_canvas, int src_clip_left, int src_clip_top, int src_clip_right, int src_clip_bottom,
      int src_pt_x, int src_pt_y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetPixel(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, int pt_x, int pt_y, out uint color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SetPixel(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom, int pt_x, int pt_y, uint c, out uint old_pixel);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Arc(uint canvas,
                              int clipLeft, int clipTop, int clipRight, int clipBottom,
                              uint pen,
                              int pt_x, int pt_y,
                              int radius,
                              int start,
                              int end);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Pie(uint canvas,
                              int clipLeft, int clipTop, int clipRight, int clipBottom,
                              uint pen,
                              uint color,
                              int pt_x, int pt_y,
                              int start,
                              int end,
                              int radii,
                              int inner);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int OpenFont(string name, ushort pixels, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int DrawText(uint canvas, int clipLeft, int clipTop, int clipRight, int clipBottom,
                                    uint font,
                                    uint fg,
                                    uint bg,
                                    string str,
                                    int pt_x, int pt_y,
                                    int txt_clip_left, int txt_clip_top, int txt_clip_right, int txt_clip_bottom,
                                    ushort format);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int TextExtent(uint canvas, uint font, string str, out int dx, out int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int InvalidateRect(uint hwnd, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int IsInvalid(uint hwnd, out bool is_invalid);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int BeginPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int EndPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SendMessage(uint hwnd, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PostMessage(uint hwnd, uint max_wait, ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int Send(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SendRaw(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int SendReply(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetFloat(byte b0, byte b1, byte b2, byte b3, out float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetInt32(byte b0, byte b1, byte b2, byte b3, out int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetUInt32(byte b0, byte b1, byte b2, byte b3, out uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetInt16(byte b0, byte b1, out short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetUInt16(byte b0, byte b1, out ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PackFloat(float value, ref byte b0, ref byte b1, ref byte b2, ref byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PackInt32(int value, ref byte b0, ref byte b1, ref byte b2, ref byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PackUInt32(uint value, ref byte b0, ref byte b1, ref byte b2, ref byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PackInt16(short value, ref byte b0, ref byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PackUInt16(ushort value, ref byte b0, ref byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamEof(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamRead(uint stream, ushort size, out byte[] value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamWrite(uint stream, byte[] buffer);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamGetPos(uint stream, out uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamSetPos(uint stream, uint pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamLength(uint stream, out uint lengt);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamTruncate(uint stream, uint length);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamCopy(uint from, uint to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamPath(uint stream, bool full_path, out string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamClose(uint stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int StreamDelete(uint stream);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int CreateDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RemoveDirectory(string path);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetDirectoryEnumerator(string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int FileStreamOpen(string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int FileStreamCreate(string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RegStreamOpen(uint parent, string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RegStreamCreate(uint parent, string path, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetDirectoryEntry(uint hndl, out string path, out string name, out bool isFile);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RewindDirectoryEntry(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int GetFont(string fontName, uint pointSize, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int ReleaseFont(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int LoadFont(uint streamHandle);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayCreate(uint numPoints, out uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayRelease(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArraySetPoint(uint handle, uint index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayClear(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArraySize(uint handle, out uint size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayResize(uint handle, uint size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayAppend(uint handle, int x, int y, out uint new_size);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayInsertAt(uint handle, uint index, int x, int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayGetPoint(uint handle, uint index, out int x, out int y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayIndexOf(uint handle, int x, int y, out uint index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int PointArrayRemoveAt(uint handle, uint index);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int AddEvent(ushort canId, CanFlyEventHandler handler);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int RemoveEvent(ushort canID, CanFlyEventHandler handler);
  }
}
