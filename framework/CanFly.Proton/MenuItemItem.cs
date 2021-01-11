using System;

namespace CanFly.Proton
{
  public sealed class MenuItemItem : MenuItem
  {
    public MenuItemItem(LayoutWidget widget, uint key)
          : base(widget)
    {
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      throw new NotImplementedException();
    }
  }
}
