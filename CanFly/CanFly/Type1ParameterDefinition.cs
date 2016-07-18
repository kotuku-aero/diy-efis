using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Forms;

namespace CanFly
{
  /// <summary>
  /// This class provides a consistent interface to a parameter definition that
  /// is stored in the Kotuku devices
  /// </summary>
  [DefaultProperty("Identifier")]
  public class CanFlyType1ParameterDefinition : IParameterDefinition
  {
    /*
   typedef struct _can_parameter_definition_t {
      uint8_t data_type;              // type of data stored or published
      uint16_t length;                // sizeof(parameter_type_x)
      } can_parameter_definition_t;

      Parameter Type 0
   * 
   * Used by analog channels in most cases
  * The DUS/DDS service allows for defining can channels.
  * a MEMID of 0..number of configurable channels
  *
  * So a request would be:
  * DDS -> nodeid, MEMID, service_code = 2, message_code = 8, source id = 0
  *
  * This will read the first identifier (8 records or 32 bytes)
  *
  * Depending on the module, there are a variable number of parameter definitions
  typedef struct _can_parameter_type_0_t {
    can_parameter_definition_t hdr;
    uint16_t can_id;                  // id used to identify this parameter
    uint16_t publish_rate;            // publish rate.  Assuming a 4khz sample time, is count of 250 microseconds
    float scale;                      // trim value to scale result by
    float offset;                     // offset value for the parameter
    } can_parameter_type_0_t;
     */

    private bool settingsChanged = false;

    private CANASMsg.DataType dataType;
    private short value;

    private mis_message_t definition;

    public string Name 
    {
      get { return definition.name; }
      private set { definition.name = value; }
    }

    [DisplayName("Data Type")]
    public CANASMsg.DataType DataType
    {
      get { return dataType; }
      set
      {
        if (value != dataType)
        {
          dataType = value;
          IsDirty = true;
        }
      }
    }

    private Int32 setting;

    [DisplayName("Setting")]
    public Int32 Value
    {
      get { return setting; }
      set
      {
        // limit to correct range
        if (DataType == CANASMsg.DataType.SHORT)
          value = Math.Min(Int16.MaxValue, Math.Max(Int16.MinValue, value));

        setting = value;
        IsDirty = true;
      }
    }

    public override string ToString()
    {
      return Name;
    }

    [Browsable(false)]
    public UInt16 DownloadBytes { get { return definition.parameter_length; } }

    [Browsable(false)]
    public UInt16 UploadBytes { get { return definition.parameter_length; } }

    public CanFlyType1ParameterDefinition(mis_message_t definition)
    {
      this.definition = definition;
    }

    public UInt32 Memid { get { return definition.memid; } }

    public void LoadDefinition(byte[] downloadData)
    {
      byte[] conversionBuffer = new byte[4];

      CANASMsg.DataType dt = (CANASMsg.DataType)downloadData[0];
      DataType = dt;
      // byte 0 is data type
      // byte 1 reserved
      // byte 2,3 is the length

      StringBuilder sb = new StringBuilder();
      sb.Append("Type1 Setting: ");
      for(int i = 0; i < downloadData.Length; i++)
        sb.Append(String.Format("{0:X} ", downloadData[i]));

      Trace.TraceInformation(sb.ToString());

      switch (dt)
      {
        case CANASMsg.DataType.SHORT:
          {
          conversionBuffer[0] = downloadData[5];
          conversionBuffer[1] = downloadData[4];
          Int16 value = BitConverter.ToInt16(conversionBuffer, 0);
          Value = value;
          }
          break;
        case CANASMsg.DataType.LONG:
          {
          conversionBuffer[0] = downloadData[7];
          conversionBuffer[1] = downloadData[6];
          conversionBuffer[2] = downloadData[5];
          conversionBuffer[3] = downloadData[4];
          Int32 value = BitConverter.ToInt32(conversionBuffer, 0);
          Value = value;
          }
          break;
        default:
          Value = 0;
          break;
      }

      IsDirty = false;
    }

    public byte[] PrepareForUpload()
    {
      byte[] result = null;
      byte[] temp;

      if (DataType == CANASMsg.DataType.SHORT)
      {
        result = new byte[6];
        result[0] = (byte)DataType;
        result[1] = 0;
        result[2] = 0;
        result[3] = 6;

        Int16 value = (Int16)Value;
        temp = BitConverter.GetBytes(value);
        result[4] = temp[1];
        result[5] = temp[0];
      }
      else if (DataType == CANASMsg.DataType.LONG)
      {
        result = new byte[8];
        result[0] = (byte)DataType;
        result[1] = 0;
        result[2] = 0;
        result[3] = 8;

        Int32 value = Value;
        temp = BitConverter.GetBytes(value);
        result[4] = temp[3];
        result[5] = temp[2];
        result[6] = temp[1];
        result[7] = temp[0];
      }
      IsDirty = false;

      return result;
    }

    [BrowsableAttribute(false), DefaultValueAttribute(false)]
    public bool IsDirty
    {
      get { return settingsChanged; }
      set 
      {
        settingsChanged = value;
        if(value)
          foreach (Form form in Application.OpenForms)
          {
            if (form is ICanFlyBrowser)
            {
              ICanFlyBrowser browser = form as ICanFlyBrowser;
              browser.EnableSave();
              return;
            }
          }
      }
    }
  }
}
