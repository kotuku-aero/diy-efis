using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  public delegate void ThreadStart();
  [Flags]
  public enum TextOutStyle
  {
    Clipped = 0x02,
    Opaque = 0x04
  }

  public delegate void CanFlyEventHandler(CanFlyMsg msg);

  public class Syscall
  {
    /*******************************************************/
    // CanFly operating system routines
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SystemInformation(out ulong machine_id, out uint version);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint Ticks();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Yield();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateThread(byte priority, string name, ThreadStart start, object arg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern object ThreadGetArg(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint ThreadGetCurrentId();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadSuspend(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadResume(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetThreadPriority(uint id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetThreadPriority(uint id, byte priority);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ThreadSleep(uint id, uint millisecondsTimeout);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint SemaphoreCreate();
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreClose(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreSignal(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SemaphoreWait(uint handle, uint milliseconds);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int InterlockedIncrement(ref int location);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern int InterlockedDecrement(ref int location);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Enter(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Exit(Object obj);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ushort RegCreateKey(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ushort RegOpenKey(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern sbyte RegGetInt8(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt8(ushort parent, string name, sbyte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern byte RegGetUint8(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint8(ushort parent, string name, byte value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern short RegGetInt16(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt16(ushort parent, string name, short value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern ushort RegGetUint16(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint16(ushort parent, string name, ushort value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern int RegGetInt32(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetInt32(ushort parent, string name, int value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint RegGetUint32(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetUint32(ushort parent, string name, uint value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern string RegGetString(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetString(ushort parent, string name, string value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern float RegGetFloat(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetFloat(ushort parent, string name, float value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern bool RegGetBool(ushort parent, string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegSetBool(ushort parent, string name, bool value);
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void RegEnumKey(ushort key, ref ushort child, out string name);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint OpenScreen(ushort orientation, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateWindow(uint parent, Rect rect, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateChildWindow(uint parent, Rect rect, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CloseWindow(uint hwnd);
    /// <summary>
    /// Process the message queue
    /// </summary>
    /// <param name="screen">Screen to receive windows messages from</param>
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void ProcessMessages(uint screen);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Rect GetWindowRect(uint window);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Rect GetWindowPos(uint window);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowPos(uint window, Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyEventHandler GetWindowData(uint window);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowData(uint window, CanFlyEventHandler wndproc);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void AddWidgetEvent(uint window, ushort canId);
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
    internal static extern void CanvasRender(uint handle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateRectCanvas(Extent extent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreatePngCanvas(Stream stream);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateGeospatialCanvas(Stream stream, uint key);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void LoadPng(uint canvas, Stream stream, Point pt);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Extent GetCanvasExtents(uint canvas);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetOrientation(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetOrientation(uint hwnd, ushort orientation);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Color CreateColor(byte red, byte green, byte blue, byte alpha);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Color CreateEncodedColor(uint encoded);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetRed(Color color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetGreen(Color color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetBlue(Color color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetAlpha(Color color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Pen CreatePen(Color color, ushort width, PenStyle style);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Color GetPenColor(Pen pen);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetPenWidth(Pen pen);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern PenStyle GetPenStyle(Pen pen);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetPointX(Point point);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetPointY(Point point);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Point CreatePoint(short x, short y);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetExtentDX(Extent extent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetExtentDY(Extent extent);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Extent CreateExtent(short dx, short dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Rect CreateRect(short left, short top, short right, short bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetRectLeft(Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetRectTop(Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetRectRight(Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern short GetRectBottom(Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Euler CreateEuler(float x, float y, float z);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetEulerX(Euler e);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetEulerY(Euler e);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetEulerZ(Euler e);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Euler AddEuler(Euler e1, Euler e2);
     [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Euler SubtractEuler(Euler e1, Euler e2);
   [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Euler MultiplyEuler(Euler e1, Euler e2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Euler DivideEuler(Euler e1, Euler e2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Euler TransformEuler(Euler e1, Euler[] matrix);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Position CreatePosition(float lat, float lng, float elev);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetLat(Position pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetLng(Position pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float GetElev(Position pos);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float Distance(Position p1, Position p2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float Bearing(Position p1, Position p2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern float MagneticVariation(Position p1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Point RotatePoint(Point center, Point pt, short degrees);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Polyline(uint canvas, Rect clipRect, Pen pen, Point[] points);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Line(uint canvas, Rect clipRect, Pen pen, Point from, Point to);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Ellipse(uint canvas, Rect clipRect, Pen pen, Color fillColor, Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Polygon(uint canvas, Rect clipRect, Pen pen, Color fillColor, Point[] points);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Rectangle(uint canvas, Rect clipRect, Pen pen, Color fillColor, Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RoundRect(uint canvas, Rect clipRect, Pen pen, Color fillColor, Rect rect, uint radius);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void BitBlt(uint canvas, Rect clipRect, Rect destRect, uint src_canvas, Rect srcClipRect, Point srcPoint);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Color GetPixel(uint canvas, Rect clipRect, Point point);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Color SetPixel(uint canvas, Rect clipRect, Point point, Color color);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Arc(uint canvas, Rect clipRect, Pen pen, Point center, ushort radius, ushort start, ushort end);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Pie(uint canvas, Rect clipRect, Pen pen, Color color, Point center, ushort start, ushort end, ushort radii, ushort inner);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Font OpenFont(string name, ushort pixels);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern ushort GetFontSize(Font font);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern string GetFontName(Font font);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void DrawText(uint canvas, Rect clipRect, Font font, Color fg, Color bg, string str, Point point, Rect textClipRect, TextOutStyle format);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern Extent TextExtent(uint canvas, Font font, string str);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InvalidateRect(uint hwnd, Rect rect);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsInvalid(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void BeginPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void EndPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateErrorMessage(byte nodeId, ushort id, uint error);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, byte b0);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, byte b0, byte b1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, byte b0, byte b1, byte b2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, byte b0, byte b1, byte b2, byte b3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, sbyte c1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, sbyte c1, sbyte c2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, sbyte c1, sbyte c2, sbyte c3);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, sbyte c1, sbyte c2, sbyte c3, sbyte c4);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, ushort v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, ushort v1, ushort v2);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, short v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, short v1, short v2);
     [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, uint v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, int v1);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyMsg CreateMessage(byte nodeId, ushort id, float v1);
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
    internal static extern ushort GetCanID(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetNodeID(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetServiceCode(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern byte GetMessageCode(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern CanFlyDataType GetDataType(CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SendMessage(uint hndl, CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PostMessage(uint hndl, uint maxWait, CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void Send(CanFlyMsg msg);
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
    internal static extern uint ManifestStreamOpen(string assemblyName, string manifestPath);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetDirectoryEntry(uint hndl, out string path, out string name, out bool isFile);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RewindDirectoryEntry(uint hndl);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void LoadFont(uint streamHandle);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void AddEvent(ushort canId, CanFlyEventHandler handler);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void RemoveEvent(ushort canID, CanFlyEventHandler handler);
  }
}
