using System;

namespace CanFly.Proton
{
  internal sealed class MenuItemMenu : MenuItem
  {
    // if this is a checklist popup then this holds the ushort value
    // that is matched to create the checklist option.
    private ushort value;

    // if a checklist item then this has no value
    private string caption;
    private Menu menu;

    internal MenuItemMenu(LayoutWidget widget, uint key, bool isChecklist)
      : base(widget)
    {
      /*
   menu_item_Menu item = (menu_item_Menu )neutron_malloc(sizeof(menu_item_Menu));
  memset(item, 0, sizeof(menu_item_Menu));

  load_item_defaults(wnd, &item.item, key);

  item.item.evaluate = item_menu_evaluate;
  item.item.item_type = mi_menu;

  load_item_defaults(wnd, &item.item, key);

  reg_get_string(key, "caption", item.caption, 0);
  char popup_name;
  reg_get_string(key, "menu", popup_name, 0);
  // load the popup menu
  FindMenu(wnd, popup_name, &item.menu);

  if (is_checklist)
    reg_get_uint16(key, "value", &item.value);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
      */
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      return MenuItemActionResult.MiaCancel;
    }

    public Menu Menu
    {
      get { return menu; }
    }
  }
}
