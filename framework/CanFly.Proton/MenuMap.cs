using System;
using System.Collections;

namespace CanFly.Proton
{
  public sealed class MenuMap : Hashtable
  {
    public MenuMap()
    {

    }

    public bool Contains(string value)
    {
      return base.Contains(value);
    }

    public Menu this[string index]
    {
      get { return base[index] as Menu; }
      set { base[index] = value; }
    }
  }
}
