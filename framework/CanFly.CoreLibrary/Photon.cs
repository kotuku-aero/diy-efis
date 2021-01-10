using System;
using System.Runtime.CompilerServices;

namespace CanFly
{
  [Flags]
  public enum TextOutStyle
  {
    Clipped = 0x02,
    Opaque = 0x04
  }

  public struct PhotonID
  {
    public static readonly ushort id_photon = 1750;
    public static readonly ushort id_paint = 1751;
    public static readonly ushort id_timer = 1754;
    public static readonly ushort id_key0 = 1755;
    public static readonly ushort id_key1 = 1756;
    public static readonly ushort id_key2 = 1757;
    public static readonly ushort id_key3 = 1758;
    public static readonly ushort id_key4 = 1759;
    public static readonly ushort id_key5 = 1760;
    public static readonly ushort id_key6 = 1761;
    public static readonly ushort id_key7 = 1762;
    public static readonly ushort id_decka = 1763;
    public static readonly ushort id_deckb = 1764;
    public static readonly ushort id_menu_up = 1765;
    public static readonly ushort id_menu_dn = 1766;
    public static readonly ushort id_menu_left = 1767;
    public static readonly ushort id_menu_right = 1768;
    public static readonly ushort id_menu_ok = 1769;
    public static readonly ushort id_menu_cancel = 1770;
    public static readonly ushort id_menu_select = 1771;
    public static readonly ushort id_buttonpress = 1772;
    public static readonly ushort id_cancel = 1773;
  }

  public sealed class Photon
  {
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint OpenScreen(ushort orientation, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateWindow(uint parent, int left, int top, int right, int bottom, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern uint CreateChildWindow(uint parent, int left, int top, int right, int bottom, ushort id);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void CloseWindow(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SendMessage(uint hwnd, CanFlyMsg msg);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void PostMessage(uint hend, CanFlyMsg msg);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowRect(uint window, out int left, out int top, out int right, out int bottom);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void GetWindowPos(uint window, out int left, out int top, out int right, out int bottom);

    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void SetWindowPos(uint window, int left, int top, int right, int bottom);

    /// <summary>
    /// Return the window data assigned to the window.
    /// </summary>
    /// <param name="window"></param>
    /// <returns></returns>
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
                                    uint  font,
                                    uint fg,
                                    uint bg,
                                    string str,
                                    int pt_x, int pt_y,
                                    int txt_clip_left, int txt_clip_top, int txt_clip_right, int txt_clip_bottom,
                                    ushort format);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void TextExtent(uint canvas, uint  font, string str, out int dx, out int dy);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void InvalidateRect(uint hwnd, int left, int top, int right, int bottom);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern bool IsValid(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void BeginPaint(uint hwnd);
    [MethodImpl(MethodImplOptions.InternalCall)]
    internal static extern void EndPaint(uint hwnd);
  }
}
/*
diy-efis
Copyright (C) 2020 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
