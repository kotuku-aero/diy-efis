using System;

namespace CanFly.Proton
{
  public sealed class MenuItemEvent : MenuItem
  {
    private CanFlyMsg msg;
    public MenuItemEvent(LayoutWidget widget, uint key)
      : base(widget)
    {
      /*
static MenuItem item_event_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_event_t *item = (menu_item_event_t *)neutron_malloc(sizeof(menu_item_event_t));
  memset(item, 0, sizeof(menu_item_event_t));

  item.item.item_type = mi_event;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_event_evaluate;

  // read the event message from the registry
  LoadCanMessage(key, &item.msg);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }
       */
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      /*
static menu_item_action_result item_event_evaluate(LayoutWindow wnd, MenuItem item, CanFlyMsg msg)
  {
  menu_item_event_t *mi = (menu_item_event_t *)item;

  // see if the event is an internal menu one
  if (get_can_id(msg) >= id_first_internal_msg && get_can_id(msg) <= id_last_internal_msg)
    send_message(window, &mi.msg);
  else
    can_send(&mi.msg);

  return mia_nothing;
  }
       */

      return MenuItemActionResult.MiaNothing;
    }
  }
}
