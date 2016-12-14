using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  public interface IParameterDefinition
  {
    string Name { get; }
    UInt32 Memid { get; }
    CANASMsg.DataType DataType { get; set; }
    UInt16 DownloadBytes { get; }
    void LoadDefinition(byte[] downloadData);
    UInt16 UploadBytes { get; }
    byte[] PrepareForUpload();
    bool IsDirty { get; set; }
  }
}
