using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  public partial class FDUMonitor : UserControl, ICanFlyMonitor
  {
    private TabIdentifier id;
    private CANaerospace.ProcessCanMsg monitor;
    private ParameterDefinitionProxy proxy;

    public const UInt16 id_fuel_flow_rate_left = 524;   // SHORT      l/hr
    public const UInt16 id_fuel_flow_rate_right = 588;   // SHORT      l/hr
    public const UInt16 id_active_fuel_tank = 591;   // SHORT      1=left, 2=right, 3=both
    public const UInt16 id_left_fuel_quantity = 668;   // SHORT      l
    public const UInt16 id_right_fuel_quantity = 669;   // SHORT      l
    public const UInt16 id_pitot_temperature = 670;   // SHORT      degrees
    public const UInt16 id_pitot_heat_status = 671;   // SHORT      1 = heater on, 0 = off
    public const UInt16 id_pitot_power_status = 672;   // SHORT      1 = power on, 0 = off
    public const UInt16 id_fdu_board_temperature = 673;   // SHORT      degrees

    public FDUMonitor()
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
        case id_fuel_flow_rate_left:
          AssignMsg(id524, msg);
          break;
        case id_fuel_flow_rate_right:
          AssignMsg(id588, msg);
          break;
        case id_active_fuel_tank:
          {
            if (id591_1 == null || !id591_1.Created ||
              id591_2 == null || !id591_2.Created)
              break;
            short value = msg.SHORT;

            id591_1.Invoke((MethodInvoker)delegate
            {
              id591_1.Checked = (value & 0x01) != 0;
              id591_2.Checked = (value & 0x01) != 0;
            });
          }
          break;
        case id_left_fuel_quantity:
          AssignMsg(id668, msg);
          break;
        case id_right_fuel_quantity:
          AssignMsg(id669, msg);
          break;
        case id_pitot_temperature:
          AssignMsg(id670, msg);
          break;
        case id_pitot_heat_status:
          if (id672 == null || !id672.Created)
            break;

          id672.Invoke((MethodInvoker)delegate { id672.Checked = msg.SHORT == 1; });
          break;
        case id_pitot_power_status:
          if (id671 == null || !id671.Created)
            break;

          id671.Invoke((MethodInvoker)delegate { id671.Checked = msg.SHORT == 1; });
          break;
        case id_fdu_board_temperature:
          AssignMsg(id673, msg);
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
