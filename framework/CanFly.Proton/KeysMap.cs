using System;
using System.Collections;

namespace CanFly.Proton
{
  public sealed class KeysMap : Hashtable
  {
    public KeysMap()
    {

    }

    public bool Contains(string value)
    {
      return base.Contains(value);
    }

    public Keys this[string index]
    {
      get { return base[index] as Keys; }
      set { base[index] = value; }
    }
  }
}
