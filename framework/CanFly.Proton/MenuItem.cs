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

namespace CanFly.Proton
{
  public enum MenuItemActionResult
  {
    MiaNothing, // no change to menu state
    MiaCancel, // event causes a cancel, will close an editor and not evalute the result
    MiaClose, // event causes a done, all menu close
  };

  public delegate void MenuItemMsgHandler(MenuItem menuItem, CanFlyMsg msg);

  public abstract class MenuItem
  {
    private string _caption;
    private string _enableFormat;
    private ushort _controllingMessageID;
    private CanFlyMsg _controllingVariable;

    // set by parent menu so the item will highlight itself
    private bool _selected;
    private bool _editorOpen; // if true the popup menu editor is active
    //private Regex _patBuff;
    private MenuWidget _widget;

    private MenuItemMsgHandler _msgHandler;

    private System.Text.RegularExpressions.Regex _enableRegex;

    private void DefaultMsgHandler(CanFlyMsg msg)
    {
      SetControllingVariable(msg);
    }

    protected MenuItem(MenuWidget widget)
    {
      _widget = widget;
    }

    public static MenuItem LoadMenu(MenuWidget widget, ushort key)
    {
      string itemType;
      if (!Widget.TryRegGetString(key, "type", out itemType))
        return null;

      MenuItem item = null;
      switch (itemType)
      {
        case "event":
          item = new MenuItemEvent(widget);
          break;
        case "edit":
          item = new MenuItemEdit(widget);
          break;
        case "checklist":
          item = new MenuItemChecklist(widget);
          break;
        case "popup":
          item = new MenuItemPopup(widget);
          break;
      }

      item.Parse(key);

      return item;
    }

    public virtual void Parse(ushort key)
    {
      if (!Widget.TryRegGetString(key, "caption", out _caption))
        _caption = string.Empty;

      string strValue;
      ushort ushortValue;
      Widget.TryRegGetString(key, "caption", out strValue);
      Caption = strValue;

      ushort enable_key;
      if (Widget.TryRegOpenKey(key, "enable", out enable_key))
      {
        if (Widget.TryRegGetUint16(enable_key, "id", out ushortValue))
          ControllingMessageID = ushortValue;

        if (Widget.TryRegGetString(enable_key, "regex", out strValue))
          EnableRegex = new System.Text.RegularExpressions.Regex(strValue);

        if (Widget.TryRegGetString(enable_key, "format", out strValue))
          EnableFormat = strValue;
      }
    }

    public MenuWidget MenuWidget
    {
      get { return _widget; }
    }

    public virtual void Paint(Rect area, bool isHighlighted)
    {
      Point center_pt = Point.Create(
        (area.Width >> 1) + area.Left,
        (area.Height >> 1) + area.Top);

      _widget.Rectangle(_widget.BorderPen,
        Selected ? _widget.SelectedBackgroundColor : _widget.BackgroundColor, area);

      // calculate the text extents
      Extent ex = _widget.TextExtent(_widget.Font, _caption);

      _widget.DrawText(_widget.Font,
        Selected ? _widget.SelectedColor : _widget.TextColor,
        _widget.BackgroundColor,
        _caption, Point.Create(center_pt.X - (ex.Dx >> 1), center_pt.Y - (ex.Dy >> 1)), area, TextOutStyle.Clipped);
    }

    public abstract MenuItemActionResult Evaluate(CanFlyMsg msg);

    public virtual void Edit(CanFlyMsg msg)
    {

    }

    public virtual bool Enabled(CanFlyMsg msg)
    {
      if (ControllingMessageID == 0 ||
          EnableRegex == null)
        return true;

      // build an enabler from the format string

      // we now determine a match against the controlling regular expression
      return EnableRegex.Match(string.Format(EnableFormat, ControllingVariable.ToString())).Success;
    }

    public string Caption
    {
      get { return _caption; }
      set { _caption = value; }
    }
    /// <summary>
    /// Regular expression to match for the item to be enabled
    /// </summary>
    /// <value></value>
    public System.Text.RegularExpressions.Regex EnableRegex
    {
      get { return _enableRegex; }
      set { _enableRegex = value; }
    }
    /// <summary>
    /// Format string to be applied to the controlling message
    /// </summary>
    /// <value></value>
    public string EnableFormat
    {
      get { return _enableFormat; }
      set { _enableFormat = value; }
    }
    /// <summary>
    /// ID to listen for
    /// </summary>
    /// <value></value>
    public ushort ControllingMessageID
    {
      get { return _controllingMessageID; }
      set
      {
        if (_controllingMessageID != 0)
          Widget.RemoveEvent(_controllingMessageID, DefaultMsgHandler);

        _controllingMessageID = value;

        if (value != 0)
          Widget.AddEvent(value, DefaultMsgHandler);
      }
    }
    /// <summary>
    /// This is the message that the menu item listens for. This holds
    /// the last received message
    /// </summary>
    /// <value></value>
    public CanFlyMsg ControllingVariable
    {
      get { return _controllingVariable; }
    }

    private void SetControllingVariable(CanFlyMsg value)
    {
      _controllingVariable = value;
    }

    public bool Selected
    {
      get { return _selected; }
      set { _selected = value; }
    }

    public bool EditorOpen
    {
      get { return _editorOpen; }
      set { _editorOpen = value; }
    }
    
    protected CanFlyMsg LoadCanMessage(ushort key)
    {
      CanFlyMsg result = null;
      ushort id;
      if (Widget.TryRegGetUint16(key, "can-id", out id))
      {
        string type;
        if (Widget.TryRegGetString(key, "can-type", out type))
        {
          string value;
          string[] values;
          // decode the message
          if (type == "NODATA" ||
              !Widget.TryRegGetString(key, "can-value", out value))
          {
            result = CanFlyMsg.Create(id);
          }
          else
          {
            switch (type)
            {
              case "ERROR":
                result = CanFlyMsg.CreateErrorMessage(id, Convert.ToUInt32(value));
                break;
              case "FLOAT":
                result = CanFlyMsg.Create(id, (float)Convert.ToDouble(value));
                break;
              case "LONG":
                result = CanFlyMsg.Create(id, Convert.ToInt32(value));
                break;
              case "ULONG":
                result = CanFlyMsg.Create(id, Convert.ToUInt32(value));
                break;
              case "SHORT":
                result = CanFlyMsg.Create(id, Convert.ToInt16(value));
                break;
              case "USHORT":
                result = CanFlyMsg.Create(id, Convert.ToUInt16(value));
                break;
              case "CHAR":
                result = CanFlyMsg.Create(id, value[0]);
                break;
              case "SBYTE" :
                result = CanFlyMsg.Create(id, (sbyte)byte.Parse(value));
                break;
              case "BYTE":
                result = CanFlyMsg.Create(id, (byte)byte.Parse(value));
                break;
              case "SBYTE2":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (sbyte)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (sbyte)(values.Length > 1 ? byte.Parse(values[1]) : 0));
                break;
              case "BYTE2":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (byte)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (byte)(values.Length > 1 ? byte.Parse(values[1]) : 0));
                break;
              case "SBYTE3":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (sbyte)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (sbyte)(values.Length > 1 ? byte.Parse(values[1]) : 0),
                  (sbyte)(values.Length > 2 ? byte.Parse(values[2]) : 0));
                break;
              case "BYTE3":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (byte)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (byte)(values.Length > 1 ? byte.Parse(values[1]) : 0),
                  (byte)(values.Length > 2 ? byte.Parse(values[2]) : 0));
                break;
              case "SBYTE4":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (sbyte)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (sbyte)(values.Length > 1 ? byte.Parse(values[1]) : 0),
                  (sbyte)(values.Length > 2 ? byte.Parse(values[2]) : 0),
                  (sbyte)(values.Length > 3 ? byte.Parse(values[3]) : 0));
                break;
              case "BYTE4":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (byte)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (byte)(values.Length > 1 ? byte.Parse(values[1]) : 0),
                  (byte)(values.Length > 2 ? byte.Parse(values[2]) : 0),
                  (byte)(values.Length > 3 ? byte.Parse(values[3]) : 0));
                break;
              case "SHORT2":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (short)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (short)(values.Length > 1 ? byte.Parse(values[1]) : 0));
                break;
              case "USHORT2":
                values = value.Split(',');
                result = CanFlyMsg.Create(id,
                  (ushort)(values.Length > 0 ? byte.Parse(values[0]) : 0),
                  (ushort)(values.Length > 1 ? byte.Parse(values[1]) : 0));
                break;
              default:
                result = CanFlyMsg.Create(id);
                break;
            }
          }
        }
      }

      return result;
    }

  }
}