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
namespace CanFly.Proton
{
  public enum MenuItemActionResult
  {
    MiaNothing, // no change to menu state
    MiaCancel, // event causes a cancel
    MiaEnter, // event causes a done, all menu close
    MiaCloseItem, // either a popup or edit is closed
  };

  public delegate void MenuItemMsgHandler(MenuItem menuItem, CanFlyMsg msg);

  public abstract class MenuItem
  {
    private string _caption;

    private string _enableRegex;
    private string _enableFormat;

    private ushort _controllingParam;
    private CanFlyMsg _controllingVariable;

    // set by parent menu so the item will highlight itself
    private bool _selected;
    private bool _editorOpen; // if true the popup menu editor is active

    //private Regex _patBuff;
    private LayoutWidget _widget;

    private MenuItemMsgHandler msgHandler;


    protected MenuItem(LayoutWidget widget)
    {
      _widget = widget;
    }

    public LayoutWidget LayoutWidget
    {
      get { return _widget; }
    }

    public virtual void Paint(Rect area, bool isHighlighted)
    {
      Point center_pt = new Point(
        (area.Width >> 1) + area.Left,
        (area.Height >> 1) + area.Top);

      _widget.Rectangle(_widget.BorderPen,
        Selected ? _widget.SelectedBackgroundColor : _widget.BackgroundColor, area);

      // calculate the text extents
      Extent ex = _widget.TextExtent(_widget.Font, _caption);

      center_pt.X -= ex.Dx >> 1;
      center_pt.Y -= ex.Dy >> 1;

      _widget.DrawText(_widget.Font,
        Selected ? _widget.SelectedColor : _widget.TextColor,
        _widget.BackgroundColor,
        _caption, center_pt, area, TextOutStyle.Clipped);

    }


    public abstract MenuItemActionResult Evaluate(CanFlyMsg msg);

    public virtual void Edit(CanFlyMsg msg)
    {

    }

    public virtual bool Enabled(CanFlyMsg msg)
    {
      if (ControllingParam == 0 ||
          EnableRegex == null)
        return true;

      // build an enabler from the format string

      // we now determine a match against the controlling regular expression
      return Match(string.Format(EnableFormat, ControllingVariable.ToString()));
    }

    public void EventHandler(MenuItemMsgHandler handler)
    {
      msgHandler = handler;
    }

    public string Caption
    {
      get { return _caption; }
      set { _caption = value; }
    }

    public string EnableRegex
    {
      get { return _enableRegex; }
      set { _enableRegex = value; }
    }

    public string EnableFormat
    {
      get { return _enableFormat; }
      set { _enableFormat = value; }
    }

    public ushort ControllingParam
    {
      get { return _controllingParam; }
      set { _controllingParam = value; }
    }

    public CanFlyMsg ControllingVariable
    {
      get { return _controllingVariable; }
      set { _controllingVariable = value; }
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

    public bool Match(string value)
    {
      return false;
      // return _patBuff.Match(value).Success;
    }
  }
}