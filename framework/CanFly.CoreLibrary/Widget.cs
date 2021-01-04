using System;
using System.Collections;

namespace CanFly
{
  public abstract class Widget : GdiObject
  {
     /// <summary>
    /// 
    /// </summary>
    /// <param name="msg"></param>
    /// <returns>true if message processed and further processing is not required</returns>
    protected virtual bool WndProc(CanFlyMsg msg)
    {
      return false;
    }

    internal Widget(uint hwnd)
      : base(hwnd)
    {
      Photon.SetWindowData(Handle, this);

      ClipRect = WindowRect;
    }

    /// <summary>
    /// Create a child window of a parent
    /// </summary>
    /// <param name="parent">Parent of the window</param>
    /// <param name="bounds">Bounds relative to the parent</param>
    /// <param name="id">Window ID</param>
    protected Widget(Widget parent, Rect bounds, ushort id)
      : base(Photon.CreateChildWindow(parent.Handle, bounds.Left, bounds.Right, bounds.Top, bounds.Bottom, id))
    {
      Photon.SetWindowData(Handle, this);
      // default clipping rectangle
      ClipRect = WindowRect;
    }

    protected abstract void OnPaint();
    
    protected override void OnDispose()
    {
      Photon.CloseWindow(Handle);
    }

    /// <summary>
    /// 
    /// </summary>
    public Rect WindowRect
    {
      get 
      {
        int left;
        int right;
        int top;
        int bottom;

        Photon.GetWindowRect(Handle, out left, out right, out top, out bottom);

        return new Rect(left, top, right, bottom);
      }
    }
    /// <summary>
    /// 
    /// </summary>
    public Rect WindowPos
    {
      get 
      {
        int left;
        int right;
        int top;
        int bottom;

        Photon.GetWindowPos(Handle, out left, out right, out top, out bottom);

        return new Rect(left, top, right, bottom);

      }
      set { Photon.SetWindowPos(Handle, value.Left, value.Right, value.Top, value.Bottom); ; }
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget Parent
    {
      get
      {
        uint parentHndl = Photon.GetParent(Handle);
        if (parentHndl == 0)
          return null;
        
        Widget parent = (Widget) Photon.GetWindowData(parentHndl);
        
        return parent;
      }
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="id"></param>
    /// 
    /// <returns></returns>
    public Widget GetWidgetById(ushort id)
    {
      uint handle = Photon.GetWindowById(Handle, id);
      if (handle != 0)
        return (Widget) Photon.GetWindowData(handle);

      return null;
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget FirstChild
    {
      get
      {
        uint handle = Photon.GetFirstChild(Handle);
        if(handle != 0)
          return (Widget) Photon.GetWindowData(handle);

        return null;
      }
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget NextSibling
    {
      get
      {
        uint handle = Photon.GetNextSibling(Handle);
        if(handle != 0)
          return (Widget) Photon.GetWindowData(handle);

        return null;
      }
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget PreviousSibling
    {
      get
      {
        uint handle = Photon.GetPreviousSibling(Handle);
        if(handle != 0)
          return (Widget) Photon.GetWindowData(handle);

        return null;
      }
    }
    /// <summary>
    /// Insert the window before the sibling in the chain of windows
    /// </summary>
    /// <param name="widget">sibling to insert before</param>
    public void InsertBefore(Widget widget)
    {
      if(widget != null)
        Photon.InsertBefore(Handle, widget.Handle);
    }
    /// <summary>
    /// Insert the window after the sibling in the chain of windows
    /// </summary>
    /// <param name="widget">sibling to insert after</param>
    public void InsertAfter(Widget widget)
    {
      if(widget != null)
        Photon.InsertAfter(Handle, widget.Handle);
    }
    /// <summary>
    /// the z-order assigned to a window, default = 0
    /// </summary>
    public byte ZOrder
    {
      get { return Photon.GetZOrder(Handle); }
      set { Photon.SetZOrder(Handle, value); }
    }
    /// <summary>
    /// Invalidate the area of the canvas.
    /// </summary>
    /// <param name="rect">hint as to rectangle invalidated</param>
    public void InvalidateRect(Rect rect = null)
    {
      if (rect == null)
        rect = WindowRect;

      Photon.InvalidateRect(Handle, rect.Left, rect.Right, rect.Top, rect.Bottom);
    }
    /// <summary>
    /// Return true if the window is invalid
    /// </summary>
    public bool IsValid
    {
      get { return Photon.IsValid(Handle); }
    }
    /// <summary>
    /// Notify the GDI a write operation is beginning
    /// </summary>
    protected void BeginPaint()
    {
      Photon.BeginPaint(Handle);
    }
    /// <summary>
    /// Notify the canvas that the update operation is complete and
    /// clear the invalid flag
    /// </summary>
    protected void EndPaint()
    {
      Photon.EndPaint(Handle);
    }

    public void DisplayRoller(Rect bounds, int value, int digits,
      uint bg_color, uint fg_color, Font  large_font, Font  small_font)
    {
      // we need to work out the size of the roller digits first

      Extent size_medium = TextExtent(small_font, "00");

      Point pt = new Point(
        bounds.Right - (digits == 1 ? size_medium.Dx >>= 1 : size_medium.Dx),
        bounds.Top);

      pt.Y += (bounds.Bottom - bounds.Top) >> 1;
      pt.Y -= size_medium.Dy >> 1;

      if (digits == 1)
        value *= 10;

      // calc the interval / pixel ratio
      pt.Y += (int)((value % 10) * (size_medium.Dy / 10.0));
      int minor = (value / 10) * 10;

      int large_value = minor / 100;
      minor %= 100;

      while (pt.Y > bounds.Top)
      {
        pt.Y -= size_medium.Dy;
        minor += 10;
      }

      string str;

      while (pt.Y <= bounds.Bottom)
      {
        // draw the text + digits first
        minor %= 100;
        if (minor < 0)
          minor += 100;

        if (minor >= 0)
        {
          if (digits == 1)
            str = ((int)(minor / 10)).ToString("d1");
          else
            str = ((int)minor).ToString("d2");

          DrawText(small_font, fg_color, bg_color, str, pt, bounds, TextOutStyle.Clipped);
        }

        minor -= 10;
        pt.Y += size_medium.Dy;
      }

      // now the larger value
      str = large_value.ToString();

      // calc the size
      //cv.font(&arial_15_font);
      Extent large_size = TextExtent(large_font, str);
      
      pt.X -= large_size.Dx;
      pt.Y = bounds.Top;
      pt.Y += (bounds.Bottom - bounds.Top) >> 1;
      pt.Y -= large_size.Dy >> 1;

      DrawText(large_font, fg_color, bg_color, str, pt, bounds, TextOutStyle.Clipped);

    }

    public bool LookupFont(uint key, string name, out Font font)
    {
      font = null;

      return false;
    }

    public bool LookupColor(uint key, string name, out uint color)
    {
      color = Colors.Black;

      return false;
    }

    public bool LookupPen(uint key, string name, out Pen pen)
    {
      pen = null;

      return false;
    }

    public bool OpenFont(string name, ushort pixels, out Font font)
    {
      font = null;

      return false;
    }

    public bool TryRegGetString(uint key, string name, out string value)
    {
      value = null;
      try
      {
        value = Neutron.RegGetString(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryOpenFont(string name, ushort pixels, out Font font)
    {
      font = null;
      try
      {
        Photon.OpenFont(name, pixels);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetFloat(uint key, string name, out float value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetFloat(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetUint8(uint key, string name, out byte value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetUint8(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetUint16(uint key, string name, out ushort value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetUint16(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetUint32(uint key, string name, out uint value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetUint32(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetInt8(uint key, string name, out sbyte value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetInt8(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetInt16(uint key, string name, out short value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetInt16(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetInt32(uint key, string name, out int value)
    {
      value = 0;
      try
      {
        value = Neutron.RegGetInt32(key, name);
      }
      catch
      {
        return false;
      }

      return true;
    }
  }
}