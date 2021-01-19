using System;
using System.Collections;

namespace CanFly.Proton
{
  public class Menu
  {
    // a list of items in the menu.  is a vector.
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

    public Menu()
    {
      _menuItems = new ArrayList();
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