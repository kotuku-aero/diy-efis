using System;

namespace CanFly.Proton
{
  internal sealed class MenuItemCancel : MenuItem
  {
    private ushort id;
    private short value;     // cancel value

    public MenuItemCancel(LayoutWidget widget, uint key)
      : base(widget)
    {
      /*
static MenuItem item_cancel_load(LayoutWindow wnd, uint key)
  {
  menu_item_cancel_t *item = (menu_item_cancel_t *)neutron_malloc(sizeof(menu_item_cancel_t));
  memset(item, 0, sizeof(menu_item_cancel_t));
  item.item.item_type = mi_cancel;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_cancel_evaluate;

  if (failed(reg_get_uint16(key, "value", &item.value)))
    item.value = id_menu_cancel;

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }
       */
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      if(id != 0)
        LayoutWidget.SendMessage(new CanFlyMsg(id, value));

      return MenuItemActionResult.MiaCancel;
    }
  }
}
