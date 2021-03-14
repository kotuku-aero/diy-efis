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

namespace CanFly
{
  public abstract class GdiObject : IDisposable
  {
    private uint _hndl;
    private Rect _clip_rect;
    
    public void Dispose()
    {
      OnDispose();
      _hndl = 0;
    }

    protected abstract void OnDispose();

    protected GdiObject(uint handle)
    {
      _hndl = handle;
      _clip_rect = new Rect(new Point(0, 0), Extents);
    }

    protected uint Handle
    {
      get { return _hndl; }
    }

    internal uint InternalHandle
    {
      get { return _hndl; }
    }
    /// <summary>
    /// Extents of the drawing surface
    /// </summary>
    public Extent Extents
    {
      get
      {
        int dx;
        int dy;
        ushort bpp;
        Syscall.GetCanvasExtents(Handle, out bpp, out dx, out dy);

        return new Extent(dx, dy);
      }
    }
    /// <summary>
    /// Rectangle to clip drawing to
    /// </summary>
    public Rect ClipRect
    {
      get { return _clip_rect; }
      set { _clip_rect = value; }
    }
    /// <summary>
    /// Draw a polyline
    /// </summary>
    /// <param name="pen">pen to draw with</param>
    /// <param name="points">points to draw</param>
    public void Polyline(Pen pen, params Point[] points)
    {
      using (PointArray pts = new PointArray(points))
        Syscall.Polyline(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
          pen.Handle, pts.Handle);
    }
    /// <summary>
    /// Draw a line
    /// </summary>
    /// <param name="pen">pen to draw with</param>
    /// <param name="from">Point to draw from</param>
    /// <param name="to">Point to draw to</param>
    public void Line(Pen pen, Point from, Point to)
    {
      Syscall.Line(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        pen.Handle, from.X, from.Y, to.X, to.Y);
    }
    /// <summary>
    /// Draw a polyline
    /// </summary>
    /// <param name="clipRect">Rectangle to clip to</param>
    /// <param name="pen">pen to draw with</param>
    /// <param name="points">points to draw</param>
    public void Polyline(Rect clipRect, Pen pen, params Point[] points)
    {
      using (PointArray pts = new PointArray(points))
        Syscall.Polyline(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
          pen.Handle, pts.Handle);
    }
    /// <summary>
    /// Draw a polyline
    /// </summary>
    /// <param name="pen">Pen to draw with</param>
    /// <param name="pts">Array of points to use</param>
    public void Polyline(Pen pen, PointArray pts)
    {
      Syscall.Polyline(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
          pen.Handle, pts.Handle);
    }
    /// <summary>
    /// Draw an ellipse
    /// </summary>
    /// <param name="pen">pen to draw ellipse with</param>
    /// <param name="color">color to fill ellipse with</param>
    /// <param name="area">area of the ellipse</param>
    public void Ellipse(Pen pen, uint color, Rect area)
    {
      Syscall.Ellipse(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        Pen.SafeHandle(pen), color, area.Left, area.Top, area.Right, area.Bottom);
    }
    /// <summary>
    /// Draw a polygon
    /// </summary>
    /// <param name="pen">Outline color</param>
    /// <param name="color">Fill color</param>
    /// <param name="points">Points to draw</param>
    public void Polygon(Pen pen, uint color, params Point[] points)
    {
      using(PointArray pts = new PointArray(points))
        Syscall.Polygon(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, Pen.SafeHandle(pen), color, pts.Handle);
    }
    /// <summary>
    /// Draw a polygon
    /// </summary>
    /// <param name="pen">Outline color</param>
    /// <param name="color">Fill color</param>
    /// <param name="pts">Points to draw</param>
    public void Polygon(Pen pen, uint color, PointArray pts)
    {
      Syscall.Polygon(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, Pen.SafeHandle(pen), color, pts.Handle);
    }
    /// <summary>
    /// Draw a rectangle
    /// </summary>
    /// <param name="pen">Pen to draw border with</param>
    /// <param name="color">Fill color</param>
    /// <param name="area">Area to draw</param>
    public void Rectangle(Pen pen, uint color, Rect area)
    {

      Syscall.Rectangle(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, Pen.SafeHandle(pen), color, 
        area.Left, area.Top, area.Right, area.Bottom);
    }
    /// <summary>
    /// Draw a rectangle with rounded corners
    /// </summary>
    /// <param name="pen">Pen to draw border with</param>
    /// <param name="color">Fill color</param>
    /// <param name="area">Area to draw</param>
    /// <param name="radius">Radius of corners</param>
    public void RoundRect(Pen pen, uint color, Rect area, int radius)
    {
      Syscall.RoundRect(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, Pen.SafeHandle(pen), color, 
        area.Left, area.Top, area.Right, area.Bottom, radius);
    }
    /// <summary>
    /// Bit transfer
    /// </summary>
    /// <param name="dest_rect">Rectangle to transfer to</param>
    /// <param name="src_canvas">Canvas to copy from</param>
    /// <param name="src_pt">Point on source to copy from</param>
    public void BitBlt(Rect dest_rect, GdiObject src_canvas, Point src_pt)
    {
      Syscall.BitBlt(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        dest_rect.Left, dest_rect.Top, dest_rect.Right, dest_rect.Bottom,
        src_canvas.Handle,
        src_canvas.ClipRect.Left, src_canvas.ClipRect.Top, src_canvas.ClipRect.Right, src_canvas.ClipRect.Bottom,
        src_pt.X, src_pt.Y);
    }
    /// <summary>
    /// Return a pixel from the screen
    /// </summary>
    /// <param name="pt">Point to get</param>
    /// <returns>Color value</returns>
    public uint GetPixel(Point pt)
    {
      uint color;
      Syscall.GetPixel(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pt.X, pt.Y, out color);
      return color;
    }

    public uint SetPixel(Point pt, uint color)
    {
      uint value;
      Syscall.SetPixel(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, 
        pt.X, pt.Y, color, out value);
      return value;
    }
    /// <summary>
    /// Draw an Arc
    /// </summary>
    /// <param name="pen">Pen to draw with</param>
    /// <param name="pt">Center point</param>
    /// <param name="radius">Arc radius</param>
    /// <param name="start">Start angle</param>
    /// <param name="end">End angle</param>
    public void Arc(Pen pen, Point pt, int radius, int start, int end)
    {
      Syscall.Arc(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, 
        Pen.SafeHandle(pen), pt.X, pt.Y,
        radius, start, end);
    }
    /// <summary>
    /// Draw a filled arc
    /// </summary>
    /// <param name="pen">Pen to draw with</param>
    /// <param name="pt">Center point</param>
    /// <param name="start">Start angle</param>
    /// <param name="end">End angle</param>
    /// <param name="radii">Outer arc radii</param>
    /// <param name="inner">Inner arc radii</param>
    public void Pie(Pen pen, uint color, Point pt, int start, int end, int radii, int inner)
    {
      Syscall.Pie(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        Pen.SafeHandle(pen), color, pt.X, pt.Y, start, end, radii, inner);
    }
    /// <summary>
    /// Draw a string
    /// </summary>
    /// <param name="font">Font to use</param>
    /// <param name="fg">Foreground color</param>
    /// <param name="bg">Background color</param>
    /// <param name="str">Text to render</param>
    /// <param name="src_pt">Orgin on canvas</param>
    /// <param name="txt_clip_rect">Rectangle to clip to</param>
    /// <param name="format">Format flags</param>
    public void DrawText(Font font, uint fg, uint bg, string str, Point src_pt, Rect txt_clip_rect = null, TextOutStyle format = TextOutStyle.Clipped)
    {
      if (txt_clip_rect == null)
        txt_clip_rect = ClipRect;

      Syscall.DrawText(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        font.Handle, fg, bg, str, src_pt.X, src_pt.Y,
        txt_clip_rect.Left, txt_clip_rect.Top, txt_clip_rect.Right, txt_clip_rect.Bottom, (ushort) format);
    }
    /// <summary>
    /// Draw a string
    /// </summary>
    /// <param name="clipRect">Rectangle to clip to</param>
    /// <param name="font">Font to use</param>
    /// <param name="fg">Foreground color</param>
    /// <param name="bg">Background color</param>
    /// <param name="str">Text to render</param>
    /// <param name="src_pt">Orgin on canvas</param>
    /// <param name="txt_clip_rect">Rectangle to clip to</param>
    /// <param name="format">Format flags</param>
    public void DrawText(Rect clipRect, Font font, uint fg, uint bg, string str, Point src_pt, Rect txt_clip_rect, TextOutStyle format)
    {
      Syscall.DrawText(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        font.Handle, fg, bg, str, src_pt.X, src_pt.Y,
        txt_clip_rect.Left, txt_clip_rect.Right, txt_clip_rect.Top, txt_clip_rect.Bottom, (ushort) format);
    }
    /// <summary>
    /// Get the rendering extent of text
    /// </summary>
    /// <param name="font">Font to use</param>
    /// <param name="str">Text to measure</param>
    /// <returns>Rendered text extents</returns>
    public Extent TextExtent(Font font, string str)
    {
      int dx;
      int dy;
      Syscall.TextExtent(Handle, font.Handle, str, out dx, out dy);

      return new Extent(dx, dy);
    }
  }
}