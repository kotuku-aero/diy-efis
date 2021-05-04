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
namespace CanFly.Proton
{
  public class Keys
  {
    private MenuItem _key0;
    private MenuItem _key1;
    private MenuItem _key2;
    private MenuItem _key3;
    private MenuItem _key4;
    private MenuItem _key5;
    private MenuItem _key6;
    private MenuItem _key7;
    private MenuItem _deckaUp;
    private MenuItem _deckaDn;
    private MenuItem _deckbUp;
    private MenuItem _deckbDn;

    public Keys()
    {

    }

    /// <summary>
    /// Load a key set from a registry key
    /// </summary>
    /// <param name="key">Registry key to load</param>
    public static Keys Parse(LayoutWidget layoutWidget, ushort key)
    {
      // must be done before we load any items so if they recurse we don't get
      // created more than once!
      Keys theKeys = null;

      ushort child;
      if (Widget.TryRegOpenKey(key, "key0", out child))
        theKeys.Key0 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key1", out child))
        theKeys.Key1 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key2", out child))
        theKeys.Key2 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key3", out child))
        theKeys.Key3 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key4", out child))
        theKeys.Key4 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key5", out child))
        theKeys.Key5 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key6", out child))
        theKeys.Key6 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "key7", out child))
        theKeys.Key7 = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "decka-up", out child))
        theKeys.DeckaUp = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "decka-dn", out child))
        theKeys.DeckaDn = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "deckb-up", out child))
        theKeys.DeckbUp = MenuItem.Parse(layoutWidget, child);

      if (Widget.TryRegOpenKey(key, "deckb-dn", out child))
        theKeys.DeckbDn = MenuItem.Parse(layoutWidget, child);

      return theKeys;
    }

    public MenuItem Key0
    {
      get { return _key0; }
      set { _key0 = value; }
    }
    public MenuItem Key1
    {
      get { return _key1; }
      set
      {
        _key1 = value;
      }
    }
    public MenuItem Key2
    {
      get { return _key2; }
      set
      {
        _key2 = value;
      }
    }
    public MenuItem Key3
    {
      get { return _key3; }
      set
      {
        _key3 = value;
      }
    }
    public MenuItem Key4
    {
      get { return _key4; }
      set
      {
        _key4 = value;
      }
    }

    public MenuItem Key5
    {
      get { return _key5; }
      set
      {
        _key5 = value;
      }
    }

    public MenuItem Key6
    {
      get { return _key6; }
      set
      {
        _key6 = value;
      }
    }
    public MenuItem Key7
    {
      get { return _key7; }
      set
      {
        _key7 = value;
      }
    }

    public MenuItem DeckaUp
    {
      get { return _deckaUp; }
      set
      {
        _deckaUp = value;
      }
    }
    public MenuItem DeckaDn
    {
      get { return _deckaDn; }
      set
      {
        _deckaDn = value;
      }
    }
    public MenuItem DeckbUp
    {
      get { return _deckbUp; }
      set
      {
        _deckbUp = value;
      }
    }
    public MenuItem DeckbDn
    {
      get { return _deckbDn; }
      set
      {
        _deckbDn = value;
      }
    }
  };
}