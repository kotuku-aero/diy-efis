using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  /// <summary>
  /// A named collection of settings
  /// </summary>
  class SettingCollection : IEnumerable<SettingDefinition>, IList<SettingDefinition>
  {
    private Dictionary<string, SettingDefinition> settings;
    private string name;

    public SettingCollection()
    {
      settings = new Dictionary<string, SettingDefinition>();
    }

    public string Name { get { return name; } }

    public override string ToString() { return name; }

    public IEnumerator<SettingDefinition> GetEnumerator()
    {
      throw new NotImplementedException();
    }

    System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
    {
      throw new NotImplementedException();
    }

    public int IndexOf(SettingDefinition item)
    {
      throw new NotImplementedException();
    }

    public void Insert(int index, SettingDefinition item)
    {
      throw new NotImplementedException();
    }

    public void RemoveAt(int index)
    {
      throw new NotImplementedException();
    }

    public SettingDefinition this[int index]
    {
      get
      {
        throw new NotImplementedException();
      }
      set
      {
        throw new InvalidOperationException("Cannot add a new setting, device must enumerate them");
      }
    }

    public SettingDefinition this[string name]
    {
      get { return settings[name]; }
    }

    public void Add(SettingDefinition item)
    {
      throw new NotImplementedException();
    }

    public void Clear()
    {
      throw new NotImplementedException();
    }

    public bool Contains(SettingDefinition item)
    {
      throw new NotImplementedException();
    }

    public void CopyTo(SettingDefinition[] array, int arrayIndex)
    {
      throw new NotImplementedException();
    }

    public int Count
    {
      get { throw new NotImplementedException(); }
    }

    public bool IsReadOnly
    {
      get { throw new NotImplementedException(); }
    }

    public bool Remove(SettingDefinition item)
    {
      throw new NotImplementedException();
    }
  }
}
