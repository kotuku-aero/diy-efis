using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  /// <summary>
  /// Class holding information concerning a setting
  /// </summary>
  /// <remarks>
  /// Settings are used to change the internal operation of a device.
  /// The pfd uses this information to allow changes to the layout and
  /// ranges of all devices.
  /// </remarks>
  class SettingDefinition
  {
    public enum SettingType
    {
      String,
      Boolean,
      Integer
    }

    private string stringValue;
    private bool booleanValue;
    private int integerValue;

    public string Name { get; private set; }
    public SettingType Datatype { get; private set; }

    public string StringValue
    {
      get
      {
        if(Datatype != SettingType.String)
          throw new InvalidOperationException();

        return stringValue;
      }
      set
      {
        if(Datatype != SettingType.String)
          throw new InvalidOperationException();

        stringValue = value;
      }
    }

    public bool BooleanValue
    {
      get
      {
        if(Datatype != SettingType.Boolean)
          throw new InvalidOperationException();

        return booleanValue;
      }
      set
      {
        if(Datatype != SettingType.Boolean)
          throw new InvalidOperationException();

        booleanValue = value;
      }
    }

    public int IntegerValue
    {
      get
      {
        if(Datatype != SettingType.Integer)
          throw new InvalidOperationException();

        return integerValue;
      }
      set
      {
        if(Datatype != SettingType.Integer)
          throw new InvalidOperationException();

        integerValue = value;
      }
    }

    public override string ToString()
    {
      switch(Datatype)
      {
        case SettingType.String :
          return stringValue;
        case SettingType.Boolean :
          return booleanValue.ToString();
        case SettingType.Integer :
          return integerValue.ToString();
      }

      return String.Empty;
    }
  }
}
