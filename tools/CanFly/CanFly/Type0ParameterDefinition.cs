using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;

namespace CanFly
{
  /// <summary>
  /// This class provides a consistent interface to a parameter definition that
  /// is stored in the Kotuku devices
  /// </summary>
  [DefaultProperty("Identifier")]
  public class CanFlyType0ParameterDefinition : IParameterDefinition
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

    private bool isDirty = false;

    private UInt16 identifier;
    private CANASMsg.DataType dataType;
    private UInt16 publishRate;
    private float scale;
    private float offset;

    private mis_message_t definition;

    public string Name 
    {
      get { return definition.name; }
      private set { definition.name = value; }
    }

    [DisplayName("CanFly ID")]
    public UInt16 Identifier 
    {
      get { return identifier; }
      set
      {
        if (value != identifier)
        {
          identifier = value;
          IsDirty = true;
        }
      }
    }

    [Browsable(false)]
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

    public enum Type0DataType
    {
      FLOAT = 2,
      SHORT = 6,
    }

    [DisplayName("Data Type")]
    public Type0DataType PublishType
    {
      get { return (Type0DataType)DataType; }
      set
      {
        DataType = (CANASMsg.DataType)value;
      }
    }

    [DisplayName("Publish Rate (ms)")]
    public UInt16 PublishRate
    {
      get { return publishRate; }
      set
      {
        if (value != publishRate)
        {
          publishRate = value;
          IsDirty = true;
        }
      }
    }

    public float Scale
    {
      get { return scale; }
      set
      {
        if (value != scale)
        {
          scale = value;
          IsDirty = true;
        }
      }
    }

    public float Offset
    {
      get { return offset; }
      set
      {
        if (value != offset)
        {
          offset = value;
          IsDirty = true;
        }
      }
    }

    [Browsable(false)]
    [DisplayName("Last Value")]
    public string LastValue
    {
      get
      {
        return Value.ToString();
      }
    }

    /// <summary>
    /// The last value that was read from the message bus.  Can be null
    /// </summary>
    [Browsable(false)]
    public CANASMsg Value 
    {
      get;
      set;
    }

    public override string ToString()
    {
      return Name;
    }

    [Browsable(false)]
    public UInt16 DownloadBytes { get { return definition.parameter_length; } }

    [Browsable(false)]
    public UInt16 UploadBytes { get { return definition.parameter_length; } }

    public CanFlyType0ParameterDefinition(mis_message_t definition)
    {
      this.definition = definition;
    }

    public UInt32 Memid { get { return definition.memid; } }

    public void LoadDefinition(byte[] downloadData)
    {
      byte[] conversionBuffer = new byte[4];

      // byte 0 is data type
      // byte 1 reserved
      // byte 2,3 is the data length
      // byte 4, 5 is the ID
      // byte 6,7 is the publish rate

      conversionBuffer[0] = downloadData[5];
      conversionBuffer[1] = downloadData[4];
      Identifier = BitConverter.ToUInt16(conversionBuffer, 0);

      DataType = (CANASMsg.DataType)downloadData[0];

      conversionBuffer[0] = downloadData[7];
      conversionBuffer[1] = downloadData[6];
      PublishRate = BitConverter.ToUInt16(conversionBuffer, 0);

      conversionBuffer[0] = downloadData[11];
      conversionBuffer[1] = downloadData[10];
      conversionBuffer[2] = downloadData[9];
      conversionBuffer[3] = downloadData[8];

      Scale = BitConverter.ToSingle(conversionBuffer, 0);

      conversionBuffer[0] = downloadData[15];
      conversionBuffer[1] = downloadData[14];
      conversionBuffer[2] = downloadData[13];
      conversionBuffer[3] = downloadData[12];

      Offset = BitConverter.ToSingle(conversionBuffer, 0);
    }

    public byte[] PrepareForUpload()
    {
      byte[] result = new byte[16];
      byte[] temp;

      result[0] = (byte)DataType;
      result[1] = 0;
      result[2] = 0;
      result[3] = 16;

      temp = BitConverter.GetBytes(Identifier);
      result[4] = temp[1];
      result[5] = temp[0];


      temp = BitConverter.GetBytes(PublishRate);
      result[6] = temp[1];
      result[7] = temp[0];

      temp = BitConverter.GetBytes(Scale);
      result[8] = temp[3];
      result[9] = temp[2];
      result[10] = temp[1];
      result[11] = temp[0];

      temp = BitConverter.GetBytes(Offset);
      result[12] = temp[3];
      result[13] = temp[2];
      result[14] = temp[1];
      result[15] = temp[0];

      IsDirty = false;

      return result;
    }

    [BrowsableAttribute(false), DefaultValueAttribute(false)]
    public bool IsDirty
    {
      get { return isDirty; }
      set
      {
        isDirty = value;
        if (value)
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
