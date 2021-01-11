namespace CanFly.Proton
{
  public enum MenuItemType
  {
    MiMenu, // item is a menu
    MiCancel, // item is a cancel option
    MiEnter, // item is an accept option
    MiEvent, // item is an event generator
    MiEdit, // item is a property editor
    MiChecklist, // item is a selection item
  };

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
    private MenuItemType _itemType;
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

    public virtual void Edit(MenuItem item, CanFlyMsg msg)
    {

    }

    public virtual bool Enabled(CanFlyMsg msg)
    {
      if (ControllingParam == 0 ||
          EnableRegex == null)
        return true;

      // we now determine a match against the controlling regular expression
      return Match(ControllingVariable.ToString(EnableFormat));
    }

    public void EventHandler(MenuItemMsgHandler handler)
    {
      msgHandler = handler;
    }

    public MenuItemType ItemType
    {
      get { return _itemType; }
      set { _itemType = value; }
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