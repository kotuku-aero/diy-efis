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
using System.Collections;

namespace CanFly
{
  /// <summary>
  /// A Widget is a class that can handle messages from the internal
  /// can message bus, as well as having child windows.
  /// </summary>
  public abstract class Widget : GdiObject
  {
    // list of events this widget is registered to receive
    private static ArrayList eventInfoTable = null;
    private class EventInfo
    {
      // the canfly msg ID of this event
      private ushort eventID;
      private CanFlyMsgHandler handler;
      
      public EventInfo(Widget widget, ushort eventId)
      {
        this.eventID = eventId;
        Syscall.AddWidgetEvent(widget.InternalHandle, eventID);
      }

      public ushort EventID {  get { return eventID; } }
      public void OnMessage(CanFlyMsg msg)
      {
        if(handler != null)
          handler.Invoke(msg);
      }

      public event CanFlyMsgHandler Handler
      {
        add { handler += value; }
        remove { handler -= value; }

      }
    }

    public event CanFlyMsgHandler BeforePaint = null;
    public event CanFlyMsgHandler AfterPaint = null;

    static Widget()
    {
      eventInfoTable = new ArrayList();
    }

    /// <summary>
    /// Private constructor for the screen
    /// </summary>
    /// <param name="hwnd"></param>
    internal Widget(uint hwnd) : base(hwnd)
    {
      Syscall.SetWindowData(Handle, OnMessage);

      ClipRect = WindowRect;
    }

    private static uint CreateChildWindow(uint parent, int left, int top, int right, int bottom, ushort id)
    {
      uint handle;
      Syscall.CreateChildWindow(parent, left, top, right, bottom, id, out handle);

      return handle;
    }

    /// <summary>
    /// Create a child window of a parent
    /// </summary>
    /// <param name="parent">Parent of the window</param>
    /// <param name="bounds">Bounds relative to the parent</param>
    /// <param name="id">Window ID</param>
    protected Widget(Widget parent, Rect bounds, ushort id)
      : base(CreateChildWindow(parent.Handle, bounds.Left, bounds.Top, bounds.Right, bounds.Bottom, id))
    {
      Syscall.SetWindowData(Handle, OnMessage);
      // default clipping rectangle
      ClipRect = WindowRect;

      // hook the paint message
      AddEventListener(PhotonID.id_paint, OnPaintMsg);
    }

    private void OnPaintMsg(CanFlyMsg e)
    {
      try
      {
        if (BeforePaint != null)
          BeforePaint(e);

        OnPaint(e);

        if (AfterPaint != null)
          AfterPaint(e);
      }
      catch
      {
      }
    }

    protected abstract void OnPaint(CanFlyMsg e);

    protected override void OnDispose()
    {
      Syscall.CloseWindow(Handle);
    }

    /// <summary>
    /// Add an event handler to the current widget message queue
    /// </summary>
    /// <param name="message_id">Id of message to handle</param>
    /// <param name="eventListener">Callback</param>
    public void AddEventListener(ushort message_id, CanFlyMsgHandler eventListener)
    {
      EventInfo eventInfo = FindEvent(message_id);
      if (eventInfo == null)
      {
        eventInfo = new EventInfo(this, message_id);
        eventInfoTable.Add(eventInfo);
      }

      eventInfo.Handler += eventListener;
    }
    /// <summary>
    /// Remove an event handler
    /// </summary>
    /// <param name="message_id"></param>
    /// <param name="eventListener"></param>
    public void RemoveEventListener(ushort message_id, CanFlyMsgHandler eventListener)
    {
      EventInfo eventInfo = FindEvent(message_id);
      if (eventInfo != null)
        eventInfo.Handler -= eventListener;
    }
 
    private EventInfo FindEvent(ushort eventID)
    {
      foreach (EventInfo theHandler in eventInfoTable)
      {
        if (eventID == theHandler.EventID)
          return theHandler;
      }

      return null;
    }

    private void OnMessage(CanFlyMsg msg)
    {
      // find the handler for the message (if any)
      EventInfo handler = FindEvent(msg.CanID);
      if (handler != null)
      {
        try
        {
          handler.OnMessage(msg);
        }
        catch
        {
        }
      }
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

        Syscall.GetWindowRect(Handle, out left, out top, out right, out bottom);

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

        Syscall.GetWindowPos(Handle, out left, out top, out right, out bottom);

        return new Rect(left, top, right, bottom);

      }
      set { Syscall.SetWindowPos(Handle, (short) value.Left, (short)value.Right, (short)value.Top, (short) value.Bottom); ; }
    }

    public void SendMessage(CanFlyMsg msg)
    {
      Syscall.SendMessage(InternalHandle, msg);
    }

    public void PostMessage(CanFlyMsg msg, uint maxWait)
    {
      Syscall.PostMessage(InternalHandle, maxWait, msg);
    }

    public void PostMessage(CanFlyMsg msg)
    {
      Syscall.PostMessage(InternalHandle, 0, msg);
    }

    private Widget GetWidget(uint handle)
    {
      if (handle == 0)
        return null;

      object wndData;
      Syscall.GetWindowData(handle, out wndData);

      return (Widget)wndData;
    }

    /// <summary>
    /// 
    /// </summary>
    public Widget Parent
    {
      get
      {
        uint parentHndl;
        Syscall.GetParent(Handle, out parentHndl);

        return GetWidget(parentHndl);
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
      uint handle;
      Syscall.GetWindowById(Handle, id, out handle);

      return GetWidget(handle);
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget FirstChild
    {
      get
      {
        uint handle;
        Syscall.GetFirstChild(Handle, out handle);

        return GetWidget(handle);
      }
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget NextSibling
    {
      get
      {
        uint handle;
        Syscall.GetNextSibling(Handle, out handle);

        return GetWidget(handle);
      }
    }
    /// <summary>
    /// 
    /// </summary>
    public Widget PreviousSibling
    {
      get
      {
        uint handle;
        Syscall.GetPreviousSibling(Handle, out handle);

        return GetWidget(handle);
      }
    }
    /// <summary>
    /// Insert the window before the sibling in the chain of windows
    /// </summary>
    /// <param name="widget">sibling to insert before</param>
    public void InsertBefore(Widget widget)
    {
      if(widget != null)
        Syscall.InsertBefore(Handle, widget.Handle);
    }
    /// <summary>
    /// Insert the window after the sibling in the chain of windows
    /// </summary>
    /// <param name="widget">sibling to insert after</param>
    public void InsertAfter(Widget widget)
    {
      if(widget != null)
        Syscall.InsertAfter(Handle, widget.Handle);
    }
    /// <summary>
    /// the z-order assigned to a window, default = 0
    /// </summary>
    public byte ZOrder
    {
      get 
      {
        byte value;
        Syscall.GetZOrder(Handle, out value);

        return value;
      }
      set { Syscall.SetZOrder(Handle, value); }
    }

    /// <summary>
    /// Invalidate the area of the canvas.
    /// </summary>
    /// <param name="rect">hint as to rectangle invalidated</param>
    public void InvalidateRect(Rect rect)
    {
      if (rect == null)
        rect = WindowRect;

      Syscall.InvalidateRect(Handle, (short)rect.Left, (short)rect.Right, (short)rect.Top, (short)rect.Bottom);
    }
  
    /// <summary>
    /// Invalidate the area of the canvas.
    /// </summary>
    public void InvalidateRect()
    {
      Rect rect = WindowRect;

      Syscall.InvalidateRect(Handle, (short)rect.Left, (short)rect.Right, (short)rect.Top, (short)rect.Bottom);
    }
    /// <summary>
    /// Return true if the window is invalid
    /// </summary>
    public bool IsInvalid
    {
      get
      {
        bool value;
        Syscall.IsInvalid(Handle, out value);

        return value;
      }
    }
    /// <summary>
    /// Notify the GDI a write operation is beginning
    /// </summary>
    protected void BeginPaint()
    {
      Syscall.BeginPaint(Handle);
    }
    /// <summary>
    /// Notify the canvas that the update operation is complete and
    /// clear the invalid flag
    /// </summary>
    protected void EndPaint()
    {
      Syscall.EndPaint(Handle);
    }

    public void DisplayRoller(Rect bounds, int value, int digits,
      uint bg_color, uint fg_color, Font  large_font, Font  small_font)
    {
      // we need to work out the size of the roller digits first

      Extent size_medium = TextExtent(small_font, "00");

      Point pt = new Point((short)(
        bounds.Right - (digits == 1 ? size_medium.Dx >>= 1 : size_medium.Dx)),
        bounds.Top);

      pt.Y += (short)((bounds.Bottom - bounds.Top) >> 1);
      pt.Y -= (short)(size_medium.Dy >> 1);

      if (digits == 1)
        value *= 10;

      // calc the interval / pixel ratio
      pt.Y += (short)((value % 10) * (size_medium.Dy / 10.0));
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
      pt.Y += (short)((bounds.Bottom - bounds.Top) >> 1);
      pt.Y -= (short)(large_size.Dy >> 1);

      DrawText(large_font, fg_color, bg_color, str, pt, bounds, TextOutStyle.Clipped);

    }
    /// <summary>
    /// Lookup a font definition based on settings in a config hive
    /// </summary>
    /// <param name="key">Configuration hive to look for font settings</param>
    /// <param name="name">Name of the Font definition registry key</param>
    /// <param name="font">Handle to the font</param>
    /// <returns>true if the font was defined</returns>
    public bool LookupFont(ushort key, string name, out Font font)
    {
      font = null;
      try
      {
        ushort fontSize;
        string fontName;

        ushort fontKey;
        CanFly.Syscall.RegOpenKey(key, name, out fontKey);

        CanFly.Syscall.RegGetString(fontKey, "name", out fontName);
        CanFly.Syscall.RegGetUint16(fontKey, "size", out fontSize);

        return OpenFont(fontName, fontSize, out font);
      }
      catch
      {
        return false;
      }
    }

    public bool LookupColor(ushort key, string name, out uint color)
    {
      color = Colors.Black;
      try
      {
        string color_str;
        CanFly.Syscall.RegGetString(key, name, out color_str);

        if(color_str[0] == '0')
        {
          // parse number
          if (color_str[1] == 'x')
            color = Convert.ToUInt32(color_str.Substring(2), 16);
          else
            color = Convert.ToUInt32(color_str);
        }
        else
        {
          switch(color_str.ToLower())
          {
            case "white":
              color = Colors.White;
              break;
            case "black":
              color = Colors.Black;
              break;
            case "gray":
              color = Colors.Gray;
              break;
            case "light-gray":
              color = Colors.LightGray;
              break;
            case "dark-gray":
              color = Colors.DarkGray;
              break;
            case "red":
              color = Colors.Red;
              break;
            case "pink":
              color = Colors.Pink;
              break;
            case "blue":
              color = Colors.Blue;
              break;
            case "green":
              color = Colors.Green;
              break;
            case "light-green":
              color = Colors.LightGreen;
              break;
            case "yellow":
              color = Colors.Yellow;
              break;
            case "magenta":
              color = Colors.Magenta;
              break;
            case "cyan":
              color = Colors.Cyan;
              break;
            case "pale-yellow":
              color = Colors.PaleYellow;
              break;
            case "light-yellow":
              color = Colors.LightYellow;
              break;
            case "lime-green":
              color = Colors.White;
              break;
            case "teal":
              color = Colors.Teal;
              break;
            case "dark-green":
              color = Colors.DarkGreen;
              break;
            case "maroon":
              color = Colors.Maroon;
              break;
            case "purple":
              color = Colors.Purple;
              break;
            case "orange":
              color = Colors.Orange;
              break;
            case "khaki":
              color = Colors.Khaki;
              break;
            case "olive":
              color = Colors.Olive;
              break;
            case "brown":
              color = Colors.Brown;
              break;
            case "navy":
              color = Colors.Navy;
              break;
            case "light-blue":
              color = Colors.LightBlue;
              break;
            case "faded-blue":
              color = Colors.FadedBlue;
              break;
            case "light-grey":
              color = Colors.LightGrey;
              break;
            case "dark-grey":
              color = Colors.DarkGray;
              break;
            case "hollow":
              color = Colors.Hollow;
              break;

          }
}
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool LookupPen(ushort key, string name, out Pen pen)
    {
      pen = null;

      try
      {
        ushort width;
        string style;
        uint color;

        ushort penKey;
        CanFly.Syscall.RegOpenKey(key, name, out penKey);

        CanFly.Syscall.RegGetUint16(penKey, "width", out width);
        CanFly.Syscall.RegGetString(penKey, "style", out style);
        CanFly.Syscall.RegGetUint32(penKey, "color", out color);

        PenStyle theStyle = PenStyle.Solid;

        switch (style)
        {
          case "solid":
            theStyle = PenStyle.Solid;
            break;
          case "dash":
            theStyle = PenStyle.Dash;
            break;
          case "dot":
            theStyle = PenStyle.Dot;
            break;
          case "dash_dot":
            theStyle = PenStyle.DashDot;
            break;
          case "dash_dot_dot":
            theStyle = PenStyle.DashDotDot;
            break;
          case "null":
            theStyle = PenStyle.Null;
            break;
        }


        pen = new Pen(color, width, theStyle);
      }
      catch
      {
        return false;
      }

      return false;
    }

    public bool OpenFont(string name, ushort size, out Font font)
    {
      font = null;

      try
      {
        font = new Font(name, size);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetString(ushort key, string name, out string value)
    {
      value = null;
      try
      {
        Syscall.RegGetString(key, name, out value);
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
      uint handle;
      try
      {
        Syscall.OpenFont(name, pixels, out handle);

        font = new Font(handle);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetBool(ushort key, string name, out bool value)
    {
      value = false;
      try
      {
        Syscall.RegGetBool(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetFloat(ushort key, string name, out float value)
    {
      value = 0;
      try
      {
        Syscall.RegGetFloat(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetUint8(ushort key, string name, out byte value)
    {
      value = 0;
      try
      {
        Syscall.RegGetUint8(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetUint16(ushort key, string name, out ushort value)
    {
      value = 0;
      try
      {
        Syscall.RegGetUint16(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetUint32(ushort key, string name, out uint value)
    {
      value = 0;
      try
      {
        Syscall.RegGetUint32(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetInt8(ushort key, string name, out sbyte value)
    {
      value = 0;
      try
      {
        Syscall.RegGetInt8(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetInt16(ushort key, string name, out short value)
    {
      value = 0;
      try
      {
        Syscall.RegGetInt16(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }
   
    public bool TryRegGetUInt16(ushort key, string name, out ushort value)
    {
      value = 0;
      try
      {
        Syscall.RegGetUint16(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }
    
    public bool TryRegGetInt32(ushort key, string name, out int value)
    {
      value = 0;
      try
      {
        Syscall.RegGetInt32(key, name, out value);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public bool TryRegGetRect(ushort key, out Rect rect)
    {
      rect = new Rect();

      int value;
      if (!TryRegGetInt32(key, "x", out value))
        value = 0;

      rect.Left = value;

      if (!TryRegGetInt32(key, "y", out value))
        value = 0;

      rect.Top = value;

      if (!TryRegGetInt32(key, "width", out value))
        value = 0;

      rect.Right = value + rect.Left;

      if (!TryRegGetInt32(key, "height", out value))
        value = 0;

      rect.Bottom = value + rect.Top;

      return true;
    }

    public bool TryRegOpenKey(ushort key, string name, out ushort child)
    {
      child = 0;
      try
      {
        Syscall.RegOpenKey(key, name, out child);
      }
      catch
      {
        return false;
      }

      return true;
    }

    /// <summary>
    /// Fast integer rotate of a point
    /// </summary>
    /// <param name="center">Point around which to rotate</param>
    /// <param name="pt">The point to rotate</param>
    /// <param name="degrees">Degrees to rotate</param>
    /// <returns>the rotated point</returns>
    public Point RotatePoint(Point center, Point pt, int degrees)
    {

      return pt;
    }

    public double RadiansToDegrees(double radians)
    {
      return radians * 0.3183098; // n * (1/ Math.PI);
    }

    public double DegressToRadians(double degrees)
    {
      return degrees * 0.0174533; //(Math.PI / 180);
    }

    public double MetersToNM(double meters)
    {
      return meters * 0.000539957; // meters / 1852
    }

    public double MetersPerSecondToKnots(double value)
    {
      return value * 1.94384;
    }
  }
}

/*
//
// This array is designed for mapping upper and lower case letter
// together for a case independent comparison.  The mappings are
// based upon ascii character sequences.
//
static const unsigned char charmap[] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
  };

int strcasecmp(const char* s1, const char* s2)
{
  register const unsigned char* cm = charmap,
    *us1 = (const unsigned char*)s1,
    *us2 = (const unsigned char  *)s2;

while (cm[*us1] == cm[*us2++])
  if (*us1++ == '\0')
    return (0);

return (cm[*us1] - cm[*--us2]);
  }

static result_t get_x(handle_t hwnd, variant_t* value)
{
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect.left;

  return s_ok;
}

static result_t set_x(handle_t hwnd, const variant_t* value)
{
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect.left != value->v_int16;
  if (changed)
  {
    rect.left = value->v_int16;
    set_window_pos(hwnd, &rect);
  }

  return s_ok;
}

static result_t get_y(handle_t hwnd, variant_t* value)
{
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect.top;

  return s_ok;
}

static result_t set_y(handle_t hwnd, const variant_t* value)
{
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect.top != value->v_int16;
  if (changed)
  {
    rect.top = value->v_int16;
    return set_window_pos(hwnd, &rect);
  }

  return s_ok;
}

static result_t get_width(handle_t hwnd, variant_t* value)
{
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect_width(&rect);

  return s_ok;
}

static result_t set_width(handle_t hwnd, const variant_t* value)
{
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect_width(&rect) != value->v_int16;
  if (changed)
  {
    rect.right = rect.left + value->v_int16;
    return set_window_pos(hwnd, &rect);
  }

  return s_ok;
}

static result_t get_height(handle_t hwnd, variant_t* value)
{
  value->dt = field_int16;
  rect_t rect;
  get_window_pos(hwnd, &rect);

  value->v_int16 = rect_height(&rect);

  return s_ok;
}

static result_t set_height(handle_t hwnd, const variant_t* value)
{
  if (value->dt != field_int16)
    return e_bad_type;

  rect_t rect;
  get_window_pos(hwnd, &rect);

  bool changed = rect_height(&rect) != value->v_int16;
  if (changed)
  {
    rect.bottom = rect.top + value->v_int16;
    return set_window_pos(hwnd, &rect);
  }

  return s_ok;
}

static result_t get_zorder(handle_t hwnd, variant_t* value)
{
  value->dt = field_uint16;
  uint8_t order;
  get_z_order(hwnd, &order);

  value->v_uint16 = order;

  return s_ok;
}

static result_t set_zorder(handle_t hwnd, const variant_t* value)
{
  if (value->dt != field_uint16)
    return e_bad_type;

  if (value->v_uint16 > 255)
    return e_bad_parameter;

  uint8_t order;
  get_z_order(hwnd, &order);


  bool changed = order != value->v_uint16;
  if (changed)
    return set_z_order(hwnd, (uint8_t)value->v_uint16);

  return s_ok;
}

static canmsg_t create_msg = {
  .flags = id_create
  };

typedef struct _event_name_t
{
  const char* name;
  uint16_t id;
}
event_name_t;

// all names are prefixed with on_ so not needed here
static const event_name_t events[] = {
    { "paint", id_paint },
{ "create", id_create },
{ "close", id_close },
{ "timer", id_timer },
{ "key0", id_key0 },
{ "key1", id_key1 },
{ "key2", id_key2 },
{ "key3", id_key3 },
{ "key4", id_key4 },
{ "decka", id_decka },
{ "deckb", id_deckb },
{ "menu_up", id_menu_up },
{ "menu_dn", id_menu_dn },
{ "menu_left", id_menu_left },
{ "menu_right", id_menu_right },
{ "menu_ok", id_menu_ok },
{ "menu_cancel", id_menu_cancel },
{ "menu_select", id_menu_select },
{ "buttonpress", id_buttonpress },
{ "pitch_accel", id_pitch_acceleration },
  { "roll_accel", id_roll_acceleration },
  { "yaw_accel", id_yaw_acceleration },
  { "pich_rate", id_pitch_rate },
  { "roll_rate", id_roll_rate },
  { "yaw_rate", id_yaw_rate },
  { "roll_anglem", id_roll_angle_magnetic },
  { "pitch_anglem", id_pitch_angle_magnetic },
  { "yaw_anglem", id_yaw_angle_magnetic },
  { "pitch_angle", id_pitch_angle },
  { "roll_angle", id_roll_angle },
  { "yaw_angle", id_yaw_angle },
  { "alt_rate", id_altitude_rate },
  { "ias", id_indicated_airspeed },
  { "tas", id_true_airspeed },
  { "cas", id_calibrated_airspeed },
  { "aoa", id_angle_of_attack },
  { "qnh", id_qnh },
  { "alt", id_baro_corrected_altitude },
  { "hdg", id_heading_angle },
  { "set_hdr", id_heading },
  { "pa", id_pressure_altitude },
  { "oat", id_outside_air_temperature },
  { "dynp", id_differential_pressure },
  { "statp", id_static_pressure },
  { "wind_spd", id_wind_speed },
  { "wind_dir", id_wind_direction },
  { "gps_lat", id_gps_latitude },
  { "gps_lng", id_gps_longitude },
  { "gps_alt", id_gps_height },
  { "gps_gs", id_gps_groundspeed },
  { "true_track", id_true_track },
  { "mag_track", id_magnetic_track },
  { "mag_heading", id_magnetic_heading },
  { "lat", id_position_latitude },
  { "lng", id_position_longitude },
  { "pres_alt", id_position_altitude },
  { "velocity_x", id_velocity_x },
  { "velocity_y", id_velocity_y },
  { "velocity_z", id_velocity_z },
  { "imu_mag_x", id_imu_mag_x },
  { "imu_mag_y", id_imu_mag_y },
  { "imu_mag_z", id_imu_mag_z },
  { "imu_accel_x", id_imu_accel_x },
  { "imu_accel_y", id_imu_accel_y },
  { "imu_accel_z", id_imu_accel_z },
  { "imu_gyro_x", id_imu_gyro_x },
  { "imu_gyro_y", id_imu_gyro_y },
  { "imu_gyro_z", id_imu_gyro_z },
  { "onmag_var", id_magnetic_variation },
  { "def_utc", id_def_utc },
  { "def_date", id_def_date },
  { "qnh_up", id_qnh_up },
  { "qnh_dn", id_qnh_dn },
  { "set_mag_var", id_set_magnetic_variation },
  { "heading_up", id_heading_up },
  { "heading_dn", id_heading_dn },
  { "stall_wrn", id_stall_warning },
  { "total_time", id_total_time },
  { "air_time", id_air_time },
  { "tach_time", id_tach_time },
  { "pitch", id_pitch },
  { "track", id_track },
  { "deviation", id_deviation },
  { "track_error", id_track_angle_error },
  { "est_to_next", id_estimated_time_to_next },
  { "est_arrival", id_estimated_time_of_arrival },
  { "est_time", id_estimated_enroute_time },
  { "wp_0_3", id_waypoint_identifier_0_3 },
  { "wp_4_7", id_waypoint_identifier_4_7 },
  { "wp_8_11", id_waypoint_identifier_8_11 },
  { "wp_12_15", id_waypoint_identifier_12_15 },
  { "wp_type", id_waypoint_type_identifier },
  { "wp_lat", id_waypoint_latitude },
  { "wp_lng", id_waypoint_longitude },
  { "wp_min_alt", id_waypoint_minimum_altitude },
  { "wp_max_alt", id_waypoint_maximum_altitude },
  { "dist_to_next", id_distance_to_next },
  { "dist_to_go", id_distance_to_destination },
  { "sel_course", id_selected_course },
  { "req_trk_angl", id_desired_track_angle },
  { "nav_command", id_nav_command },
  { "nav_valid", id_nav_valid },
  { "gps_valid", id_gps_valid },
  { "imu_valid", id_imu_valid },
  { "edu_valid", id_edu_valid },
  { "aux_battery", id_aux_battery },
  { "trim_up", id_trim_up },
  { "trim_dn", id_trim_dn },
  { "trim_left", id_trim_left },
  { "trim_right", id_trim_right },
  { "ap_engage", id_autopilot_engage },
  { "ap_mroll", id_autopilot_set_max_roll },
  { "ap_vs_rate", id_autopilot_set_vs_rate },
  { "wp_turn_hdg", id_waypoint_turn_heading },
  { "roll_setp", id_roll_servo_set_position },
  { "pitch_setp", id_pitch_servo_set_postion },
  { "ap_power", id_autopilot_power },
  { "ap_alt_mode", id_autopilot_alt_mode },
  { "ap_vs_mode", id_autopilot_vs_mode },
  { "ap_mode", id_autopilot_mode },
  { "ap_status", id_autopilot_status },
  { "roll_srvs", id_roll_servo_status },
  { "pitch_srvs", id_pitch_servo_status },
  { "ap_altitude", id_autopilot_altitude },
  { "ap_vs", id_autopilot_vertical_speed },
  { "rpm", id_engine_rpm },
  { "rpm_a", id_engine_rpm_a },
  { "egt1", id_exhaust_gas_temperature1 },
  { "egt2", id_exhaust_gas_temperature2 },
  { "egt3", id_exhaust_gas_temperature3 },
  { "egt4", id_exhaust_gas_temperature4 },
  { "map", id_manifold_pressure },
  { "oilp", id_oil_pressure },
  { "oilt", id_oil_temperature },
  { "cht1", id_cylinder_head_temperature1 },
  { "cht2", id_cylinder_head_temperature2 },
  { "cht3", id_cylinder_head_temperature3 },
  { "cht4", id_cylinder_head_temperature4 },
  { "rpm_b", id_engine_rpm_b },
  { "map_b", id_manifold_pressure_b },
  { "fuel1", id_fuel_pressure },
  { "dcvolt", id_dc_voltage },
  { "dcamp", id_dc_current },
  { "fuelf", id_fuel_flow_rate },
  { "fuelc", id_fuel_consumed },
  { "leftfuel", id_edu_left_fuel_quantity },
  { "rfuel", id_edu_right_fuel_quantity },
  { "tim_err", id_timing_divergence },
  { "lmag_rpm", id_left_mag_rpm },
  { "rmag_rpm", id_right_mag_rpm },
  { "lmag_adv", id_left_mag_adv },
  { "rmag_adv", id_right_mag_adv },
  { "lmag_map", id_left_mag_map },
  { "rmag_map", id_right_mag_map },
  { "lmag_volt", id_left_mag_volt },
  { "rmag_volt", id_right_mag_volt },
  { "lmag_temp", id_left_mag_temp },
  { "rmag_temp", id_right_mag_temp },
  { "lmag_coil1", id_left_mag_coil1 },
  { "rmag_coil2", id_right_mag_coil1 },
  { "lmag_coil1", id_left_mag_coil2 },
  { "rmag_coil2", id_right_mag_coil2 },
  { "lff", id_fuel_flow_rate_left },
  { "rff", id_fuel_flow_rate_right },
  { "active_tank", id_active_fuel_tank },
  { "lfuel_q", id_left_fuel_quantity },
  { "r_fuel_q", id_right_fuel_quantity },
  { "pitot_temp", id_pitot_temperature },
  { "piotoht", id_pitot_heat_status },
  { "pitotpwr", id_pitot_power_status },
  { "fdu_temp", id_fdu_board_temperature },
  { "set_lfuel_q", id_set_left_fuel_qty },
  { "set_rfuel_q", id_set_right_fuel_qty },
  { "mix_set", id_mixture_lever },
  { "inj_dwell", id_injector_dwell },
  { "prop_set", id_propeller_speed_lever },
  { "prop_amps", id_propeller_motor_current },
  { "prop_low", id_propeller_low_speed },
  { "prop_high", id_propeller_high_speed },
  { "rtx_rpm_a", id_rpm_a },
  { "rtx_ff_a", id_fuel_flow_a },
  { "rtx_map_a", id_manifold_pressure_a },
  { "rtx_oilp_a", id_oil_pressure_a },
  { "rtx_oilt_a", id_oil_temperature_a },
  { "rtx_wtrt_a", id_coolant_temp_a },
  { "rtx_egt1_a", id_egt1_a },
  { "rtx_egt2_a", id_egt2_a },
  { "rtx_egt3_a", id_egt3_a },
  { "rtx_egt4_a", id_egt4_a },
  { "rtx_tmp_a", id_intake_temperature_a },
  { "rtx_oat_a", id_outside_air_temp_a },
  { "rtx_tps_a", id_throttle_position_a },
  { "rtx_temp_a", id_ambient_pressure_a },
  { "rtx_vlt_a", id_voltage_a },
  { "rtx_es_a", id_engine_status_a },
  { "rtx_hours_a", id_engine_hours_a },
  { "rtx_ehrs_a", id_ecu_hours_a },
  { "rtx_ss_1_a", id_sensor_status_1_a },
  { "rtx_ss_2_a", id_sensor_status_2_a },
  { "rtx_ds_1_a", id_device_status_1_a },
  { "rtx_ds_2_a", id_device_status_2_a },
  { "rtx_rpm_b", id_rpm_b },
  { "rtx_ff_b", id_fuel_flow_b },
  { "rtx_map_b", id_manifold_pressure_b },
  { "rtx_oilp_b", id_oil_pressure_b },
  { "rtx_oilt_b", id_oil_temperature_b },
  { "rtx_wtrt_b", id_coolant_temp_b },
  { "rtx_egt1_b", id_egt1_b },
  { "rtx_egt2_b", id_egt2_b },
  { "rtx_egt3_b", id_egt3_b },
  { "rtx_egt4_b", id_egt4_b },
  { "rtx_tmp_b", id_intake_temperature_b },
  { "rtx_oat_b", id_outside_air_temp_b },
  { "rtx_tps_b", id_throttle_position_b },
  { "rtx_temp_b", id_ambient_pressure_b },
  { "rtx_vlt_b", id_voltage_b },
  { "rtx_es_b", id_engine_status_b },
  { "rtx_hours_b", id_engine_hours_b },
  { "rtx_ehrs_b", id_ecu_hours_b },
  { "rtx_ss_1_b", id_sensor_status_1_b },
  { "rtx_ss_2_b", id_sensor_status_2_b },
  { "rtx_ds_1_b", id_device_status_1_b },
  { "rtx_ds_2_b", id_device_status_2_b },
  0,0
  };

result_t get_event_id(const char* name, uint16_t* id)
{
  if (strncmp(name, "on_", 3) != 0)
    return e_unexpected;

  name += 3;

  if (isdigit(*name))
  {
    *id = (uint16_t)atoi(name);
    return s_ok;
  }

  const event_name_t* evp = events;
  while (evp->name != 0)
  {
    if (strcmp(name, evp->name) == 0)
    {
      *id = evp->id;
      return s_ok;
    }

    evp++;
  }

  return e_not_found;
}

result_t create_child_widget(handle_t parent, memid_t key, wndproc cb, handle_t* hwnd)
{
  result_t result;
  rect_t rect;
  uint16_t id = 0;

  int16_t value;
  if (failed(result = reg_get_int16(key, "x", &value)))
    return result;
  rect.left = (gdi_dim_t)value;

  if (failed(result = reg_get_int16(key, "y", &value)))
    return result;
  rect.top = (gdi_dim_t)value;

  if (failed(result = reg_get_int16(key, "width", &value)))
    return result;
  rect.right = rect.left + (gdi_dim_t)value;

  if (failed(result = reg_get_int16(key, "height", &value)))
    return result;
  rect.bottom = rect.top + (gdi_dim_t)value;

  reg_get_uint16(key, "id", &id);

  char type[REG_NAME_MAX];
  const char* prototype = 0;
  if (succeeded(reg_get_string(key, "type", type, 0)))
  {
    // testing hack.  Until we remove all of the native code
    //prototype = type;
  }

  if (failed(result = create_child_window(parent, &rect, cb, id, key, prototype, hwnd)))
    return result;

  // set the z-order
  if (succeeded(result = reg_get_uint16(key, "zorder", &id)))
  {
    if (id < 256)
      set_z_order(*hwnd, (uint8_t)id);
  }

  add_property(*hwnd, "x", get_x, set_x, field_int16);
  add_property(*hwnd, "y", get_y, set_y, field_int16);
  add_property(*hwnd, "width", get_width, set_width, field_int16);
  add_property(*hwnd, "height", get_height, set_height, field_int16);
  add_property(*hwnd, "zorder", get_zorder, set_zorder, field_uint16);

  // The widget properties are loaded ok, now we can register them

  // all of the scripts should be attached to the window.  Now we attach the events
  memid_t events;
  if (succeeded(reg_open_key(key, "events", &events)))
  {
    // enumerate the keys
    field_datatype dt = field_stream;
    char name[REG_NAME_MAX];

    memid_t child = 0;
    while (succeeded(reg_enum_key(events, &dt, 0, 0, REG_NAME_MAX, name, &child)))
    {
      dt = field_stream;
      // open the stream
      stream_p stream;
      if (succeeded(reg_stream_open(events, name, &stream)))
      {
        if (failed(result = compile_function(*hwnd, name, stream)))
        {
          trace_debug("Cannot compile scriptlet %s when loading window\r\n", name);
        }
      }
      else
        continue;   // not a valid stream

      uint16_t can_id = 0;

      if (failed(get_event_id(name, &can_id)))
        continue;       // ignore it...

      // the event handler is an anonymous function that is
      // added to the widget using the event name as the name
      if (can_id > 0)
        add_handler(*hwnd, can_id, name);

    }
  }


  send_message(*hwnd, &create_msg);

  return s_ok;
}

result_t lookup_enum(memid_t key,
  const char* name,
  const char** values,
  int max_values,
  int* value)
{
  result_t result;
  char str[REG_STRING_MAX + 1];

  if (name == 0 ||
    values == 0 ||
    value == 0)
    return e_bad_parameter;

  if (failed(result = reg_get_string(key, name, str, 0)))
    return result;

  int i;
  for (i = 0; i < max_values; i++)
    if (strcasecmp(str, values[i]) == 0)
    {
      *value = i;
      return s_ok;
    }

  return e_not_found;
}

result_t lookup_font(memid_t key, const char* name, handle_t* font)
{
  result_t result;
  if (name == 0 ||
    font == 0)
    return e_bad_parameter;

  *font = 0;

  memid_t font_key;
  if (failed(result = reg_open_key(key, name, &font_key)))
    return e_not_found;

  char font_name[REG_STRING_MAX];

  if (failed(result = reg_get_string(font_key, "name", font_name, 0)))
    return result;

  uint16_t pts;
  if (failed(result = reg_get_uint16(font_key, "size", &pts)))
    return result;

  // pts is the vertical size.
  return open_font(font_name, pts, font);
}

typedef struct _color_lookup_t
{
  const char* name;
  color_t color;
}
color_lookup_t;

static color_lookup_t color_lookups[] = {
  { "white", color_white},
  { "black", color_black},
  { "gray", color_gray},
  { "light_gray", color_light_gray},
  { "dark_gray", color_dark_gray},
  { "red", color_red},
  { "pink", color_pink},
  { "blue", color_blue},
  { "green", color_green},
  { "lightgreen", color_lightgreen},
  { "yellow", color_yellow},
  { "magenta", color_magenta},
  { "cyan", color_cyan},
  { "pale_yellow", color_pale_yellow},
  { "light_yellow", color_light_yellow},
  { "lime_green", color_lime_green},
  { "teal", color_teal},
  { "dark_green", color_dark_green},
  { "maroon", color_maroon},
  { "purple", color_purple},
  { "orange", color_orange},
  { "khaki", color_khaki},
  { "olive", color_olive},
  { "brown", color_brown},
  { "navy", color_navy},
  { "light_blue", color_light_blue},
  { "faded_blue", color_faded_blue},
  { "hollow", color_hollow},
  { "lightgrey", color_lightgrey},
  { "darkgrey", color_darkgrey},
  { "paleyellow", color_paleyellow},
  { "lightyellow", color_lightyellow},
  { "limegreen", color_limegreen},
  { "darkgreen", color_darkgreen},
  { "lightblue", color_lightblue},
  { "fadedblue", color_fadedblue},
  { 0, 0}
  };

result_t lookup_color(memid_t key, const char* name, color_t* color)
{
  result_t result;

  if (name == 0 ||
    color == 0)
    return e_bad_parameter;

  // determine the type of the registry key
  field_datatype dt = 0;
  memid_t memid;
  if (failed(result = reg_query_child(key, name, &memid, &dt, 0)))
    return result;

  if (dt == field_string)
  {
    // handle a string type
    char str[REG_STRING_MAX];

    *color = color_hollow;

    if (failed(result = reg_get_string(key, name, str, 0)))
      return result;

    if (str[0] == '0' || str[0] == '-')
    {
      *color = (color_t)strtoul(str, 0, 0);
      return s_ok;
    }

    color_lookup_t* colors = color_lookups;

    while (colors->name != 0)
    {
      if (strcasecmp(str, colors->name) == 0)
      {
        *color = colors->color;
        return s_ok;
      }

      colors++;
    }
    return e_not_found;
  }
  else if (dt = field_uint32)
  {
    // numeric field
    return reg_get_uint32(key, name, color);
  }

  return e_bad_parameter;
}

typedef struct _pen_value_lookup_t
{
  const char* name;
  pen_style style;
}
pen_value_lookup_t;

static const pen_value_lookup_t pen_values[] = {
  { "solid", ps_solid},
  { "dash", ps_dash},
  { "dot", ps_dot},
  { "dash_dot", ps_dash_dot},
  { "dash_dot_dot", ps_dash_dot_dot},
  { "null", ps_null},
  { 0, ps_solid}
  };

result_t lookup_pen_style(memid_t key, const char* name, pen_style* value)
{
  result_t result;
  if (name == 0 ||
    value == 0)
    return e_bad_parameter;

  const pen_value_lookup_t* lookup = pen_values;

  char str[REG_STRING_MAX];

  *value = ps_solid;

  if (failed(result = reg_get_string(key, name, str, 0)))
    return result;

  while (lookup->name != 0)
  {
    if (strcmp(str, lookup->name) == 0)
    {
      *value = lookup->style;
      return s_ok;
    }

    lookup++;
  }

  return e_not_found;
}

result_t lookup_pen(memid_t key, pen_t* pen)
{
  result_t result;

  if (failed(result = lookup_color(key, "color", &pen->color)) ||
    failed(result = lookup_pen_style(key, "style", &pen->style)) ||
    failed(result = reg_get_uint16(key, "width", &pen->width)))
    return result;

  return s_ok;
}

result_t display_roller(handle_t hwnd,
  const rect_t* bounds,
  uint32_t value,
  int digits,
  color_t bg_color,
  color_t fg_color,
  handle_t large_font,
  handle_t small_font)
{
  // we need to work out the size of the roller digits first
  //  const handle_t  *old_font = cv.font(&arial_12_font);

  extent_t size_medium;
  text_extent(hwnd, small_font, "00", 2, &size_medium);

  point_t pt = {
    bounds->right - (digits == 1 ? size_medium.dx >>= 1 : size_medium.dx),
    bounds->top
    };

  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= size_medium.dy >> 1;

  if (digits == 1)
    value *= 10;

  // calc the interval / pixel ratio
  pt.y += (value % 10) * (size_medium.dy / 10.0);
  gdi_dim_t minor = (value / 10) * 10;

  gdi_dim_t large_value = minor / 100;
  minor %= 100;

  while (pt.y > bounds->top)
  {
    pt.y -= size_medium.dy;
    minor += 10;
  }

  char str[64];

  while (pt.y <= bounds->bottom)
  {
    // draw the text + digits first
    minor %= 100;
    if (minor < 0)
      minor += 100;

    if (minor >= 0)
    {
      if (digits == 1)
        sprintf(str, "%d", (int)minor / 10);
      else
        sprintf(str, "%02.2d", (int)minor);

      draw_text(hwnd, bounds, small_font, fg_color, bg_color,
        str, 0, &pt, bounds, eto_clipped, 0);
    }

    minor -= 10;
    pt.y += size_medium.dy;
  }

  // now the larger value

  sprintf(str, "%d", (int)large_value);
  size_t len = strlen(str);

  // calc the size
  //cv.font(&arial_15_font);
  extent_t large_size;
  text_extent(hwnd, large_font, str, len, &large_size);

  pt.x -= large_size.dx;
  pt.y = bounds->top;
  pt.y += (bounds->bottom - bounds->top) >> 1;
  pt.y -= large_size.dy >> 1;

  draw_text(hwnd, bounds, large_font, fg_color, bg_color,
    str, len, &pt, bounds, eto_clipped, 0);

  return s_ok;
}

// create a generic widget
result_t create_widget(handle_t parent, memid_t key, handle_t* hwnd)
{
  result_t result;

  // create our window.  This does not 
  if (failed(result = create_child_widget(parent, key, defwndproc, hwnd)))
    return result;

  rect_t rect;
  get_window_rect(*hwnd, &rect);
  invalidate_rect(*hwnd, &rect);

  return s_ok;
}
*/