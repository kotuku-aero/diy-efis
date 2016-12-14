using System;
using System.Collections.Generic;
using System.Text;

namespace CanFly
{
  public interface ICanFlyBrowser
  {
    bool Connect();
    void Refresh();
    void EnableSave();
    void Save();
  }
}
