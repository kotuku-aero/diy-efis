using System;

namespace CanFly.Proton
{
  // A checklist is a popup menu with the events being sent to the
  // checklist which then does a message.  This is to allow the current value
  // to be displayed as the popup menu selected item
  public sealed class MenuItemChecklist : MenuItem
  {
    ushort value_id;
    CanFlyMsg value;

    // item that is selected in the list.
    short selected_item;

    // this holds the checklist items
    MenuItemPopup popup;

    public MenuItemChecklist(LayoutWidget widget, uint key)
      : base(widget)
    {
      /*
static MenuItem item_checklist_load(LayoutWindow wnd,
  uint key)
  {
  menu_item_checklist_t *item = (menu_item_checklist_t *)neutron_malloc(sizeof(menu_item_checklist_t));
  memset(item, 0, sizeof(menu_item_checklist_t));

  item.item.item_type = mi_checklist;

  load_item_defaults(wnd, &item.item, key);
  item.item.evaluate = item_checklist_evaluate;
  item.item.event = item_checklist_event;

  reg_get_uint16(key, "value-id", &item.value_id);

  // load the popup menu next.
  item.popup = (menu_item_Menu )item_menu_load(wnd, key, true);

  // save the new item
  vector_push_back(menu_items, &item);
  return &item.item;
  }
      */
    }

    public override MenuItemActionResult Evaluate(CanFlyMsg msg)
    {
      /*
static void item_checklist_event(LayoutWindow wnd, MenuItem item, CanFlyMsg msg)
  {
  // check the enabler
  DefaultEnableHandler(wnd, item, msg);

  menu_item_checklist_t *checklist = (menu_item_checklist_t *)item;
  // see if we update our value
  if (checklist.value_id == get_can_id(msg))
    {
    memcpy(&checklist.value, msg, sizeof(CanFlyMsg));

    ushort value;
    get_param_uint16(msg, 0, &value);

    ushort count;
    vector_count(checklist.popup.menu.menu_items, &count);

    ushort i;
    for (i = 0; i < count; i++)
      {
      MenuItem item;
      vector_at(checklist.popup.menu.menu_items, i, &item);
      if (i == value)
        checklist.popup.menu.SelectedIndex = i;
      }
    }
  }
       */
      return MenuItemActionResult.MiaNothing;
    }
  }
}
