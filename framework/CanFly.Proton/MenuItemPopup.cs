using System;

namespace CanFly.Proton
{
  public sealed class MenuItemPopup : MenuItem
  {
    public MenuItemPopup(LayoutWidget widget, uint key)
          : base(widget)
    {
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      throw new NotImplementedException();
    }
  }
}
