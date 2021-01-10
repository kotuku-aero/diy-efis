using System;

namespace CanFly.Proton
{
  public sealed class MenuItemOK : MenuItem
  {
    private ushort id;
    private short value;     // cancel value

    public MenuItemOK(LayoutWidget widget, uint key)
      : base(widget)
    {
      /*
static MenuItem item_enter_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_enter_t *item = (menu_item_enter_t *)neutron_malloc(sizeof(menu_item_enter_t));
  memset(item, 0, sizeof(menu_item_enter_t));

  item.item.item_type = mi_enter;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_enter_evaluate;

  if (failed(reg_get_uint16(key, "value", &item.value)))
    item.value = id_menu_ok;

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }
       */
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      /*
static menu_item_action_result item_enter_evaluate(LayoutWindow wnd,
  MenuItem item,
  CanFlyMsg msg)
  {
  // get the current menu and select the item
  MenuItem mi;
  if (succeeded(vector_at(_currentMenu.menu_items, _currentMenu.SelectedIndex, &mi)) &&
    (*mi.is_enabled)(wnd, mi, msg))
    (*mi.evaluate)(wnd, mi, msg);

  return mia_enter;
  }
       */
      return MenuItemActionResult.MiaEnter;
    }
  }
}
