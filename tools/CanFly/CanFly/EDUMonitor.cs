using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CanFly
{
  public partial class EDUMonitor : UserControl, ICanFlyMonitor
  {
    private TabIdentifier id;
    private CANaerospace.ProcessCanMsg monitor;
    private ParameterDefinitionProxy proxy;

    // monitored values
    public const UInt16 id_engine_rpm = 500;   // SHORT      r/min
    public const UInt16 id_engine_rpm_a = 501;   // SHORT      r/min
    public const UInt16 id_exhaust_gas_temperature1 = 520;   // SHORT      K
    public const UInt16 id_exhaust_gas_temperature2 = 521;   // SHORT      K
    public const UInt16 id_exhaust_gas_temperature3 = 522;   // SHORT      K
    public const UInt16 id_exhaust_gas_temperature4 = 523;   // SHORT      K
    public const UInt16 id_manifold_pressure = 528;   // FLOAT      hPa
    public const UInt16 id_oil_pressure = 532;   // FLOAT      hPa
    public const UInt16 id_oil_temperature = 536;   // SHORT      K
    public const UInt16 id_cylinder_head_temperature1 = 548;   // SHORT      K
    public const UInt16 id_cylinder_head_temperature2 = 549;   // SHORT      K
    public const UInt16 id_cylinder_head_temperature3 = 550;   // SHORT      K
    public const UInt16 id_cylinder_head_temperature4 = 551;   // SHORT      K
    public const UInt16 id_engine_rpm_b = 564;   // SHORT      r/min
    public const UInt16 id_manifold_pressure_b = 592;   // FLOAT      hPa
    public const UInt16 id_fuel_pressure = 684;   // FLOAT      hPa
    public const UInt16 id_dc_voltage = 920;   // SHORT      V
    public const UInt16 id_dc_current = 930;   // SHORT      A
    public const UInt16 id_fuel_flow_rate = 590;
    public const UInt16 id_timing_divergence = 2200;  // SHORT      msec difference between left & right tach
    public const UInt16 id_left_mag_rpm = 2201;
    public const UInt16 id_right_mag_rpm = 2202;
    public const UInt16 id_left_mag_adv = 2203;
    public const UInt16 id_right_mag_adv = 2204;
    public const UInt16 id_left_mag_map = 2205;
    public const UInt16 id_right_mag_map = 2206;
    public const UInt16 id_left_mag_volt = 2207;
    public const UInt16 id_right_mag_volt = 2208;
    public const UInt16 id_left_mag_temp = 2209;
    public const UInt16 id_right_mag_temp = 2210;
    public const UInt16 id_left_mag_coil1 = 2211;
    public const UInt16 id_right_mag_coil1 = 2212;
    public const UInt16 id_left_mag_coil2 = 2213;
    public const UInt16 id_right_mag_coil2 = 2214;

    public EDUMonitor()
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
      if (control == null)
        return;

      if (control.IsDisposed)
        return;

      if (control.InvokeRequired)
      {
          control.Invoke((MethodInvoker)delegate
          {
            if (!control.IsDisposed)
              control.Text = msg.DisplayString(offset);
          });
      }
      else
        control.Text = msg.DisplayString(offset);
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
        case id_exhaust_gas_temperature1:
          AssignMsg(id520, msg, -273.15f);
          break;
        case id_exhaust_gas_temperature2:
          AssignMsg(id521, msg, -273.15f);
          break;
        case id_exhaust_gas_temperature3:
          AssignMsg(id522, msg, -273.15f);
          break;
        case id_exhaust_gas_temperature4:
          AssignMsg(id523, msg, -273.15f);
          break;
        case id_manifold_pressure:
          AssignMsg(id528, msg);
          break;
        case id_oil_pressure: 
          AssignMsg(id532, msg);
          break;
        case id_oil_temperature: 
          AssignMsg(id536, msg);
          break;
        case id_cylinder_head_temperature1:
          AssignMsg(id548, msg, -273.15f);
          break;
        case id_cylinder_head_temperature2:
          AssignMsg(id549, msg, -273.15f);
          break;
        case id_cylinder_head_temperature3:
          AssignMsg(id550, msg, -273.15f);
          break;
        case id_cylinder_head_temperature4:
          AssignMsg(id551, msg, -273.15f);
          break;
        case id_engine_rpm_b: 
          AssignMsg(id564, msg);
          break;
        case id_fuel_pressure: 
          AssignMsg(id684, msg);
          break;
        case id_dc_voltage: 
          AssignMsg(id920, msg);
          break;
        case id_dc_current: 
          AssignMsg(id930, msg);
          break;
        case id_fuel_flow_rate :
          AssignMsg(id590, msg);
          break;
        case id_timing_divergence: 
          AssignMsg(id2200, msg);
          break;
        case id_left_mag_rpm: 
          AssignMsg(id2201, msg);
          break;
        case id_right_mag_rpm: 
          AssignMsg(id2202, msg);
          break;
        case id_left_mag_adv: 
          AssignMsg(id2203, msg);
          break;
        case id_right_mag_adv: 
          AssignMsg(id2204, msg);
          break;
        case id_left_mag_map: 
          AssignMsg(id2205, msg);
          break;
        case id_right_mag_map: 
          AssignMsg(id2206, msg);
          break;
        case id_left_mag_volt: 
          AssignMsg(id2207, msg);
          break;
        case id_right_mag_volt: 
          AssignMsg(id2208, msg);
          break;
        case id_left_mag_temp: 
          AssignMsg(id2209, msg);
          break;
        case id_right_mag_temp: 
          AssignMsg(id2210, msg);
          break;
        case id_left_mag_coil1: 
          AssignMsg(id2211, msg);
          break;
        case id_right_mag_coil1: 
          AssignMsg(id2212, msg);
          break;
        case id_left_mag_coil2: 
          AssignMsg(id2213, msg);
          break;
        case id_right_mag_coil2: 
          AssignMsg(id2214, msg);
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
