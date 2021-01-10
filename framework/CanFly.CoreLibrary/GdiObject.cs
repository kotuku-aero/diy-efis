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
        Photon.GetCanvasExtents(Handle, out bpp, out dx, out dy);

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
        Photon.Polyline(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
          pen.Handle, pts.Handle);
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
        Photon.Polyline(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
          pen.Handle, pts.Handle);
    }
    /// <summary>
    /// Draw a polyline
    /// </summary>
    /// <param name="pen">Pen to draw with</param>
    /// <param name="pts">Array of points to use</param>
    public void Polyline(Pen pen, PointArray pts)
    {
      Photon.Polyline(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
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
      Photon.Ellipse(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        pen.Handle, color, area.Left, area.Top, area.Right, area.Bottom);
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
        Photon.Polygon(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pen.Handle, color, pts.Handle);
    }
    /// <summary>
    /// Draw a polygon
    /// </summary>
    /// <param name="pen">Outline color</param>
    /// <param name="color">Fill color</param>
    /// <param name="pts">Points to draw</param>
    public void Polygon(Pen pen, uint color, PointArray pts)
    {
      Photon.Polygon(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pen.Handle, color, pts.Handle);
    }
    /// <summary>
    /// Draw a rectangle
    /// </summary>
    /// <param name="pen">Pen to draw border with</param>
    /// <param name="color">Fill color</param>
    /// <param name="area">Area to draw</param>
    public void Rectangle(Pen pen, uint color, Rect area)
    {
      Photon.Rectangle(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pen.Handle, color, 
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
      Photon.RoundRect(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pen.Handle, color, 
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
      Photon.BitBlt(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        dest_rect.Left, dest_rect.Top, dest_rect.Right, dest_rect.Bottom,
        src_canvas.Handle, src_canvas.ClipRect.Left, src_canvas.ClipRect.Top, src_canvas.ClipRect.Right, src_canvas.ClipRect.Bottom,
        src_pt.X, src_pt.Y);
    }
    /// <summary>
    /// Return a pixel from the screen
    /// </summary>
    /// <param name="pt">Point to get</param>
    /// <returns>Color value</returns>
    public uint GetPixel(Point pt)
    {
      return Photon.GetPixel(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pt.X, pt.Y);
    }

    public uint SetPixel(Point pt, uint color)
    {
      return Photon.SetPixel(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pt.X, pt.Y, color);
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
      Photon.Arc(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pen.Handle, pt.X, pt.Y, radius, start, end);
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
      Photon.Pie(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom, pen.Handle, color, pt.X, pt.Y, start, end, radii, inner);
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

      Photon.DrawText(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
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
      Photon.DrawText(Handle, ClipRect.Left, ClipRect.Top, ClipRect.Right, ClipRect.Bottom,
        font.Handle, fg, bg, str, src_pt.X, src_pt.Y, 
        txt_clip_rect.Left, txt_clip_rect.Right, txt_clip_rect.Top, txt_clip_rect.Bottom, (ushort) format);
    }
    /// <summary>
    /// Get the rendering extent of text
    /// </summary>
    /// <param name="font">Fornt to use</param>
    /// <param name="str">Text to measure</param>
    /// <returns>Rendered text extents</returns>
    public Extent TextExtent(Font font, string str)
    {
      int dx;
      int dy;
      Photon.TextExtent(Handle, font.Handle, str, out dx, out dy);

      return new Extent(dx, dy);
    }
  }
}