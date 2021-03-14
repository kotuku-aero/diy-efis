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
using System;

namespace CanFly.Proton
{
  // A checklist is a popup menu with the events being sent to the
  // checklist which then does a message.  This is to allow the current value
  // to be displayed as the popup menu selected item
  public sealed class MenuItemChecklist : MenuItem
  {
    CanFlyMsg value;

    // item that is selected in the list.
    short selected_item;

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
