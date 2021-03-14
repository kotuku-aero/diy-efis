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
  internal sealed class MenuItemMenu : MenuItem
  {
    // if this is a checklist popup then this holds the ushort value
    // that is matched to create the checklist option.

    // if a checklist item then this has no value
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
