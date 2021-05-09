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
using System.Collections;

namespace CanFly.Proton
{
  /// <summary>
  /// A Menu is a collection of Items. there can be only 1 active item
  /// </summary>
  public class Menu
  {
    // a list of items in the menu.
    private ArrayList _menuItems;

    private string _name; // name of the menu

    // this is the menu item with the current focus.  All ok, cancel is sent
    // here.  This is not the selected active menu (only a popup)
    private ushort _activeItem;

    // bottom menu item displayed.  The menu will expand to fit the
    // vertical height of the screen.
    private ushort _selectedIndex;

    // if there is a popup active, this is keys handler
    private Keys _keys;
    private MenuWidget _widget;

    public Menu(MenuWidget widget)
    {
      _menuItems = new ArrayList();
      _widget = widget;
    }

    public void LoadMenu(ushort key)
    {
      ushort child = 0;
      string itemName;
      while (Widget.TryRegEnumKey(key, ref child, out itemName))
      {
        // the protocol assumes all child keys are menu items
        
        MenuItem item = MenuItem.LoadMenu(_widget, child);

        if (item != null)
          _menuItems.Add(item);
      }

      // we now load the keys
      string keys;
      if (Widget.TryRegGetString(key, "keys", out keys))
        Keys = _widget.LoadKeys(keys);
    }

    public MenuItem this[int index]
    {
      get { return (MenuItem)_menuItems[index]; }
      set { _menuItems[index] = value; }
    }

    public ArrayList MenuItems
    {
      get { return _menuItems; }
    }

    public string Name
    {
      get { return _name; }
      set { _name = value; }
    }

    public ushort ActiveItem
    {
      get { return _activeItem; }
      set { _activeItem = value; }
    }

    public ushort SelectedIndex
    {
      get { return _selectedIndex; }
      set { _selectedIndex = value; }
    }

    public Keys Keys
    {
      get { return _keys; }
      set { _keys = value; }
    }
  };
}