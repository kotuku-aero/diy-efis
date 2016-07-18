using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  public partial class EFIMonitor : UserControl, ICanFlyMonitor
  {
    private TabIdentifier id;
    private CANaerospace.ProcessCanMsg monitor;
    private ParameterDefinitionProxy proxy;

    // monitored values
    public const UInt16 id_engine_rpm = 500;   // SHORT      r/min
    public const UInt16 id_engine_rpm_a = 501;   // SHORT      r/min
    public const UInt16 id_manifold_pressure = 528;   // FLOAT      hPa
    public const UInt16 id_engine_rpm_b = 564;   // SHORT      r/min
    public const UInt16 id_fuel_pressure = 684;   // FLOAT      hPa
    public const UInt16 id_fuel_flow_rate = 590;
    public const UInt16 id_mixture_lever = 700;
    public const UInt16 id_injector_dwell = 701;

    public EFIMonitor()
    {
      InitializeComponent();
    }

    public void AssignProxy(TabIdentifier id, ParameterDefinitionProxy proxy)
    {
      this.id = id;

      this.proxy = proxy;
      // connect the listener
      monitor = new CANaerospace.ProcessCanMsg(Channel_CanMsgReceived);
      proxy.Channel.CanMsgReceived += monitor;
    }

    private void AssignMsg(Control control, CANASMsg msg)
    {
      AssignMsg(control, msg, 0);
    }

    private void AssignMsg(Control control, CANASMsg msg, float offset)
    {
      if (control == null || !control.Created)
        return;

      control.Invoke((MethodInvoker)delegate
      {
        control.Text = msg.DisplayString(offset);
      });
    }

    private void Channel_CanMsgReceived(CANASMsg msg)
    {
      switch (msg.ID)
      {
        case id_engine_rpm:
          AssignMsg(id500, msg);
          break;
        case id_engine_rpm_a:
          AssignMsg(id501, msg);
          break;
        case id_manifold_pressure:
          AssignMsg(id528, msg);
          break;
        case id_engine_rpm_b:
          AssignMsg(id564, msg);
          break;
        case id_fuel_pressure:
          AssignMsg(id684, msg);
          break;
        case id_fuel_flow_rate:
          AssignMsg(id590, msg);
          break;
        case id_mixture_lever:
          AssignMsg(id700, msg);
          break;
        case id_injector_dwell:
          AssignMsg(id701, msg);
          break;
      }
    }

    public TabIdentifier Device
    {
      get { return id; }
    }

    public void Disconnect()
    {
      proxy.Channel.CanMsgReceived -= monitor;
    }
  }
}
