using System;

namespace CanFly.Proton
{
  public sealed class MenuItemPopup : MenuItem
  {
    private Menu popupMenu;
    public MenuItemPopup(LayoutWidget widget, uint key)
          : base(widget)
    {
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      return MenuItemActionResult.MiaNothing;
    }

    public Menu PopupMenu
    {
      get { return popupMenu; }
      set { popupMenu = value; }
    }
  }
}
