/*
diy-efis
Copyright (C) 2021 Kotuku Aerospace Limited

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice is
subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.

If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
using System;

// ReSharper disable InconsistentNaming

namespace CanFly
{
  // these are the CanFly message types that are known
  public struct CanFlyID
  {
    public static readonly ushort id_unused                   = 0;
    public static readonly ushort id_pitch_acceleration       = 300;  // FLOAT      g       forward: + aft: -
    public static readonly ushort id_roll_acceleration        = 301;   // FLOAT      g       right: + left: -
    public static readonly ushort id_yaw_acceleration         = 302;   // FLOAT      g       up: + down: -
    public static readonly ushort id_pitch_rate               = 303;   // FLOAT      rad/s   nose up: + nose down: -
    public static readonly ushort id_roll_rate                = 304;   // FLOAT      rad/s   roll right: + roll left: -
    public static readonly ushort id_yaw_rate                 = 305;   // FLOAT      rad/s   yaw right: + yaw left: -
    public static readonly ushort id_roll_angle_magnetic      = 306;   // FLOAT      rad
    public static readonly ushort id_pitch_angle_magnetic     = 307;   // FLOAT      rad
    public static readonly ushort id_yaw_angle_magnetic       = 308;   // FLOAT      rad
    public static readonly ushort id_pitch_angle              = 311;   // FLOAT      rad     nose up: + nose down: -
    public static readonly ushort id_roll_angle               = 312;   // FLOAT      rad     roll right: + roll left: -
    public static readonly ushort id_yaw_angle                = 313;   // FLOAT      rad     yaw right: + yaw left: -
    public static readonly ushort id_altitude_rate            = 314;   // FLOAT      m/s
    public static readonly ushort id_indicated_airspeed       = 315;   // FLOAT      m/s
    public static readonly ushort id_true_airspeed            = 316;   // FLOAT      m/s
    public static readonly ushort id_calibrated_airspeed      = 317;   // FLOAT      m/s
    public static readonly ushort id_angle_of_attack          = 318;   // SHORT      deg
    public static readonly ushort id_qnh                      = 319;   // SHORT      hPa     published every 60 seconds.  changed with public static readonly ushort id_qnh_up; public static readonly ushort id_qnh_dn
    public static readonly ushort id_baro_corrected_altitude  = 320;   // FLOAT      m
    public static readonly ushort id_heading_angle            = 321;   // SHORT      deg     0-360   True heading
    public static readonly ushort id_heading                  = 322;   // SHORT      deg     0-360
    public static readonly ushort id_pressure_altitude        = 323;   // FLOAT      m
    public static readonly ushort id_outside_air_temperature  = 324;   // SHORT      K
    public static readonly ushort id_differential_pressure    = 325;   // FLOAT      hPa
    public static readonly ushort id_static_pressure          = 326;   // FLOAT      hPa
    public static readonly ushort id_wind_speed               = 333;   // FLOAT      m/s
    public static readonly ushort id_wind_direction           = 334;   // float      rad     0-360
    public static readonly ushort id_gps_latitude             = 1036;  // FLOAT      deg
    public static readonly ushort id_gps_longitude            = 1037;  // FLOAT      deg
    public static readonly ushort id_gps_height               = 1038;   // FLOAT      m
    public static readonly ushort id_gps_groundspeed          = 1039;  // FLOAT      m/s
    public static readonly ushort id_true_track               = 1040;  // SHORT      deg
    public static readonly ushort id_magnetic_track           = 1041;  // SHORT      deg
    public static readonly ushort id_magnetic_heading         = 1069;  // SHORT      deg
    public static readonly ushort id_position_latitude        = 1070;  // FLOAT      rad
    public static readonly ushort id_position_longitude       = 1071;  // FLOAT      rad
    public static readonly ushort id_position_altitude        = 1072;  // FLOAT      m       true altitud
    public static readonly ushort id_velocity_x               = 1073;  // FLOAT      m/s
    public static readonly ushort id_velocity_y               = 1074;  // FLOAT      m/s
    public static readonly ushort id_velocity_z               = 1075;  // FLOAT      m/s
    public static readonly ushort id_imu_mag_x                = 1076;  // FLOAT      gauss +/-
    public static readonly ushort id_imu_mag_y                = 1077;  // FLOAT      gauss +/-
    public static readonly ushort id_imu_mag_z                = 1078;  // FLOAT      gauss +/-
    public static readonly ushort id_imu_accel_x              = 1079;  // FLOAT      m/s
    public static readonly ushort id_imu_accel_y              = 1080;  // FLOAT      m/s
    public static readonly ushort id_imu_accel_z              = 1081;  // FLOAT      m/s
    public static readonly ushort id_imu_gyro_x               = 1082;  // FLOAT      rad/s
    public static readonly ushort id_imu_gyro_y               = 1083;  // FLOAT      rad/s
    public static readonly ushort id_imu_gyro_z               = 1084;  // FLOAT      rad/s
    public static readonly ushort id_magnetic_variation       = 1121;  // SHORT      deg
    public static readonly ushort id_def_utc                  = 1200;  // CHAR4              format: 13h43min22s 13 43 22 00
    public static readonly ushort id_def_date                 = 1201;  // CHAR4              format: 12. June 1987 12 06 19 87
    public static readonly ushort id_qnh_up                   = 1202;  // SHORT              can increment by # of steps
    public static readonly ushort id_qnh_dn                   = 1203;  // SHORT
    public static readonly ushort id_set_magnetic_variation   = 1204;
    public static readonly ushort id_heading_up               = 1206;
    public static readonly ushort id_heading_dn               = 1207;
    public static readonly ushort id_stall_warning            = 1211;  // SHORT     Stall warning switch activated
    public static readonly ushort id_total_time               = 1212;  // LONG      Total time in service  Hrs * 10
    public static readonly ushort id_air_time                 = 1213;  // LONG      Total time where IAS > 10 kts Hrs * 10
    public static readonly ushort id_tach_time                = 1214;  // LONG      Total time where the engine is running Hrs * 10
    public static readonly ushort id_pitch                    = 1217;  // SHORT      deg    +/- 180
    public static readonly ushort id_track                    = 1006;  // float      radians   track over the ground (true)
    public static readonly ushort id_deviation                = 1007;  // float      m         deviation from track in meters
    public static readonly ushort id_track_angle_error        = 1008;  // SHORT      deg
    public static readonly ushort id_estimated_time_to_next   = 1009;  // SHORT      min
    public static readonly ushort id_estimated_time_of_arrival = 1010;  // SHORT      min
    public static readonly ushort id_estimated_enroute_time   = 1011;  // SHORT      min
    public static readonly ushort id_waypoint_identifier_0_3  = 1012;  // ACHAR4
    public static readonly ushort id_waypoint_identifier_4_7  = 1013;  // ACHAR4
    public static readonly ushort id_waypoint_identifier_8_11 = 1014;  // ACHAR4
    public static readonly ushort id_waypoint_identifier_12_15 = 1015;  // ACHAR4
    public static readonly ushort id_waypoint_type_identifier = 1016;  // SHORT
    public static readonly ushort id_waypoint_latitude        = 1017;  // FLOAT      deg
    public static readonly ushort id_waypoint_longitude       = 1018;  // FLOAT      deg
    public static readonly ushort id_waypoint_minimum_altitude = 1019;  // FLOAT      m
    public static readonly ushort id_waypoint_maximum_altitude = 1023;  // FLOAT      m
    public static readonly ushort id_distance_to_next         = 1024;  // float      m
    public static readonly ushort id_distance_to_destination  = 1025;  // float      m
    public static readonly ushort id_selected_course          = 1125;  // SHORT      deg
    public static readonly ushort id_desired_track_angle      = 1135;  // FLOAT      rad       magnetic angle to fly to get to destination
    public static readonly ushort id_nav_command              = 1136;  // SHORT      0 = clear route; 1 = activate route; 2 = deactivate_route; 3 = invert route
    public static readonly ushort id_nav_valid                = 1137;  // SHORT      0 = nav data not valid; 1 = enroute data valid
    public static readonly ushort id_gps_valid                = 1138;  // SHORT      0 = nav data not valid; 1 = enroute data valid
    public static readonly ushort id_imu_valid                = 1139;  // SHORT      0 = no data; 1 = valid data
    public static readonly ushort id_edu_valid                = 1140;  // SHORT      0 = error; 1 = valid
    public static readonly ushort id_aux_battery              = 1141;  // SHORT      0 = not on battery; 1 = on aux battery
    public static readonly ushort id_trim_up                  = 1142;  // SHORT      number of trim steps
    public static readonly ushort id_trim_dn                  = 1143;  // SHORT
    public static readonly ushort id_trim_left                = 1144;  // SHORT
    public static readonly ushort id_trim_right               = 1145;  // SHORT
    public static readonly ushort id_autopilot_engage         = 1146;  // SHORT      0 = off; 1 = on
    public static readonly ushort id_autopilot_set_max_roll   = 1147;  // SHORT      Maximum roll rate allowed
    public static readonly ushort id_autopilot_set_vs_rate    = 1148;  // SHORT      Set maximum pitch allowed
    public static readonly ushort id_waypoint_turn_heading    = 1149;  // SHORT      deg; heading to turn onto
    public static readonly ushort id_roll_servo_set_position  = 1150;  // SHORT      degrees position +/-90
    public static readonly ushort id_pitch_servo_set_postion  = 1151;  // SHORT      degrees position +/-90
    public static readonly ushort id_autopilot_power          = 1152;  // SHORT      autopilot power is on
    public static readonly ushort id_autopilot_alt_mode       = 1153;  // SHORT      0 = off; 1 = on  Altitude hold mode enable
    public static readonly ushort id_autopilot_vs_mode        = 1154;  // SHORT      0 = off; 1 = on  Vertical speed hold mode enable
    public static readonly ushort id_autopilot_mode           = 1155;  // SHORT      see enumeration below
    public static readonly ushort id_autopilot_status         = 1156;  // SHORT      bitmask; published every 5secs or on change
    public static readonly ushort id_roll_servo_status        = 1157;
    public static readonly ushort id_pitch_servo_status       = 1158;
    public static readonly ushort id_autopilot_altitude       = 1157;  // SHORT      m assigned altitude
    public static readonly ushort id_autopilot_vertical_speed = 1158;  // SHORT      m/s vertical rate of climb/descent
    public static readonly ushort id_engine_rpm               = 500;   // SHORT      r/min
    public static readonly ushort id_engine_rpm_a             = 501;   // SHORT      r/min
    public static readonly ushort id_exhaust_gas_temperature1 = 520;   // SHORT      K
    public static readonly ushort id_exhaust_gas_temperature2 = 521;   // SHORT      K
    public static readonly ushort id_exhaust_gas_temperature3 = 522;   // SHORT      K
    public static readonly ushort id_exhaust_gas_temperature4 = 523;   // SHORT      K
    public static readonly ushort id_manifold_pressure        = 528;   // FLOAT      hPa
    public static readonly ushort id_oil_pressure             = 532;   // FLOAT      hPa
    public static readonly ushort id_oil_temperature          = 536;   // SHORT      K
    public static readonly ushort id_cylinder_head_temperature1 = 548;   // SHORT      K
    public static readonly ushort id_cylinder_head_temperature2 = 549;   // SHORT      K
    public static readonly ushort id_cylinder_head_temperature3 = 550;   // SHORT      K
    public static readonly ushort id_cylinder_head_temperature4 = 551;   // SHORT      K
    public static readonly ushort id_engine_rpm_b             = 564;   // SHORT      r/min
    public static readonly ushort id_manifold_pressure_b      = 592;   // FLOAT      hPa
    public static readonly ushort id_fuel_pressure            = 684;   // FLOAT      hPa
    public static readonly ushort id_dc_voltage               = 920;   // SHORT      V
    public static readonly ushort id_dc_current               = 930;   // SHORT      A
    public static readonly ushort id_fuel_flow_rate           = 590;   // SHORT      l/hr
    public static readonly ushort id_fuel_consumed            = 591;   // SHORT      l     Fuel consumed since power on
    public static readonly ushort id_edu_left_fuel_quantity   = 666;   // SHORT      l
    public static readonly ushort id_edu_right_fuel_quantity  = 667;   // SHORT      l
    public static readonly ushort id_timing_divergence        = 2200;  // SHORT      msec difference between left & right tach
    public static readonly ushort id_left_mag_rpm             = 2201;
    public static readonly ushort id_right_mag_rpm            = 2202;
    public static readonly ushort id_left_mag_adv             = 2203;
    public static readonly ushort id_right_mag_adv            = 2204;
    public static readonly ushort id_left_mag_map             = 2205;
    public static readonly ushort id_right_mag_map            = 2206;
    public static readonly ushort id_left_mag_volt            = 2207;
    public static readonly ushort id_right_mag_volt           = 2208;
    public static readonly ushort id_left_mag_temp            = 2209;
    public static readonly ushort id_right_mag_temp           = 2210;
    public static readonly ushort id_left_mag_coil1           = 2211;
    public static readonly ushort id_right_mag_coil1          = 2212;
    public static readonly ushort id_left_mag_coil2           = 2213;
    public static readonly ushort id_right_mag_coil2          = 2214;
    public static readonly ushort id_fuel_flow_rate_left      = 524;   // SHORT      l/hr
    public static readonly ushort id_fuel_flow_rate_right     = 588;   // SHORT      l/hr
    public static readonly ushort id_active_fuel_tank         = 591;   // SHORT      1=left; 2=right; 3=both
    public static readonly ushort id_left_fuel_quantity       = 668;   // SHORT      l
    public static readonly ushort id_right_fuel_quantity      = 669;   // SHORT      l
    public static readonly ushort id_pitot_temperature        = 670;   // SHORT      degrees
    public static readonly ushort id_pitot_heat_status        = 671;   // SHORT      1 = heater on; 0 = off
    public static readonly ushort id_pitot_power_status       = 672;   // SHORT      1 = power on; 0 = off
    public static readonly ushort id_fdu_board_temperature    = 673;   // SHORT      degrees
    public static readonly ushort id_set_left_fuel_qty        = 674;   // SHORT      l
    public static readonly ushort id_set_right_fuel_qty       = 675;   // SHORT      l
    public static readonly ushort id_mixture_lever           = 700;    // SHORT    0-100%
    public static readonly ushort id_injector_dwell          = 701;    // SHORT    0-1024
    public static readonly ushort id_propeller_speed_lever   =  800;   // SHORT      rpm
    public static readonly ushort id_propeller_motor_current =  801;   // SHORT      A
    public static readonly ushort id_propeller_low_speed     =  802;   // SHORT      rpm
    public static readonly ushort id_propeller_high_speed    =  803;   // SHORT      rpm
    public static readonly ushort id_set_route                =  1140;  // SHORT              service code 0=current plan; 1..n specific plan
    public static readonly ushort id_set_route_segments       =  1141;  // SHORT              service code 0=current plan; 1..n specific plan; number of route segments (max 16 for route 0)
    public static readonly ushort id_set_waypoint_identifier_0_3   = 1142;  // ACHAR4
    public static readonly ushort id_set_waypoint_identifier_4_7   = 1143;  // ACHAR4
    public static readonly ushort id_set_waypoint_identifier_8_11  = 1144;  // ACHAR4
    public static readonly ushort id_set_waypoint_identifier_12_15 = 1145;  // ACHAR4
    public static readonly ushort id_set_waypoint_type_identifier  = 1146;  // SHORT
    public static readonly ushort id_set_waypoint_latitude         = 1146;  // FLOAT      deg
    public static readonly ushort id_set_waypoint_longitude        = 1148;  // FLOAT      deg
    public static readonly ushort id_set_waypoint_minimum_altitude = 1149;  // FLOAT      m
    public static readonly ushort id_set_waypoint_maximum_altitude = 1150;  // FLOAT      m
    public static readonly ushort id_navigation_mode               = 1250;  // SHORT    0 = GPS 1 Navigation; 1 = GPS 2 Navligation; 2 = NAV 1;  3 = NAV 2
    public static readonly ushort id_alternator_status             = 1251;  // SHORT    0 = alternator failed; 1 = alternator ok
    public static readonly ushort id_voltage_warning               = 1252;  // SHORT    0 = voltage ok; 1 = low voltage
    public static readonly ushort id_panel_brightness              = 1253;  // SHORT    0-100 %
    public static readonly ushort id_pitot_temperature_alarm       = 1254;  // SHORT    0 = pitot temperature ok; 1 = temperature low; 2 = temperature high
    public static readonly ushort id_nav_msg                       = 1256;  // SHORT    0 = no message; 1 = msg displayed
    public static readonly ushort id_nav_appr                      = 1257;  // SHORT    0 = GPS not in approach mode; 1 = GPS in approach mode
    public static readonly ushort id_nav_ils                       = 1258;  // SHORT    0 = NAV receiver does not have ILD; 1 = NAV receiver has ILS detected
    public static readonly ushort id_voltage_sense_1               = 1259;  // FLOAT    0-16v
    public static readonly ushort id_voltage_sense_2               = 1260;  // FLOAT    0-16v
    public static readonly ushort id_user_defined_start            = 1800;
    public static readonly ushort id_user_defined_end              = 1899;
    public static readonly ushort id_node_register                 = 2032;
  };

/*
  // Defined hardware_types
#define unit_ahrs                     1
#define unit_edu                      2
#define unit_pi                       3
#define unit_mfd                      4
#define unit_usb                      5
#define unit_scan                     6

#define ahrs_node_id                  128         // this allows for up to 8 AHRS units/bus
#define ahrs_node_id_last             135
#define edu_node_id                   136
#define edu_node_id_last              143
#define mfd_node_id                   144
#define mfd_node_id_last              159
#define efi_node_id                   160
#define efi_node_id_last              164
#define scan_id                       192         // serial adapter for CanFLY

// the can driver is used to queue messages for the can bus.  Since
// the worker processes tend to run at fixed intervals, we need a seperate
// thread to process sending data to the can bus.

*/

public enum CanFlyDataType 
{
  NoData = 0,
  Error = 1,
  Float = 2,
  Int32 = 3,
  UInt32 = 4,
  Short = 6,
  UShort = 7,
  Char = 9,
  UChar = 10,
  Short2 = 12,
  UShort2 = 13,
  Char4 = 15,
  UChar4 = 16,
  Char2 = 18,
  UChar2 = 19,
  Char3 = 26,
  UChar3 = 27,
};

public sealed class CanFlyMsg
  {
    private static byte nodeId = 0;
    /// <summary>
    /// The global node id for the canfly application
    /// </summary>
    /// <value>Node ID</value>
    public static byte NodeID
    {
      get { return nodeId; }
      set { nodeId = value; }
    }
    /// <summary>
    /// Flag for a reply message
    /// </summary>
    public bool Reply
    {
      get { return Syscall.GetReply(this); }
      set { Syscall.SetReply(this, value); }
    }
    /// <summary>
    /// ID of the CAN message
    /// </summary>
    public ushort CanID
    {
      get { return Syscall.GetCanID(this); }
      set { Syscall.SetCanID(this, value); }
    }
    /// <summary>
    /// CanFly node id
    /// </summary>
    public byte NodeId 
    { 
      get { return Syscall.GetNodeID(this); } 
      set { Syscall.SetNodeID(this, value); }
    }
    /// <summary>
    /// The type of data
    /// </summary>
    public CanFlyDataType DataType 
    {
      get { return Syscall.GetDataType(this); }
     }
    /// <summary>
    /// The service code
    /// </summary>
    internal byte ServiceCode 
    {
      get { return Syscall.GetServiceCode(this); } 
      set { Syscall.SetServiceCode(this, value); }
    }
    /// <summary>
    /// The message code
    /// </summary>
    internal byte MessageCode 
    {
      get { return Syscall.GetMessageCode(this); } 
      set { Syscall.SetServiceCode(this, value); }
    }
    /// <summary>
    /// Create a message with the NoData type
    /// </summary>
    /// <param name="id">Message ID</param>
    public CanFlyMsg(ushort id)
    {
      Syscall.CreateMessage(this, nodeId, id);
    }
    private CanFlyMsg(ushort id, bool isError, uint error)
    {
      Syscall.CreateErrorMessage(this, nodeId, id, error);
    }
    /// <summary>
    /// Create an error message
    /// </summary>
    /// <param name="id">Can ID of the message</param>
    /// <param name="value">Value to report as an error</param>
    /// <returns>Constructed message</returns>
    public static CanFlyMsg CreateErrorMessage(ushort id, uint error)
    {
      return new CanFlyMsg(id, true, error);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public CanFlyMsg(ushort id, uint value)
    {
      Syscall.CreateMessage(this, nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public CanFlyMsg(ushort id, int value)
    {
      Syscall.CreateMessage(this, nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public CanFlyMsg(ushort id, ushort value)
    {
      Syscall.CreateMessage(this, nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public CanFlyMsg(ushort id, short value)
    {
      Syscall.CreateMessage(this, nodeId, id, value);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v1">value1 to send</param>
    /// <param name="v2">value2 to send</param>
    public CanFlyMsg(ushort id, ushort v1, ushort v2)
    {
      Syscall.CreateMessage(this, nodeId, id, v1, v2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v1">value1 to send</param>
    /// <param name="v2">value2 to send</param>
    public CanFlyMsg(ushort id, short v1, short v2)
    {
       Syscall.CreateMessage(this, nodeId, id, v1, v2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="v">value to assign</param>
    public CanFlyMsg(ushort id, float v)
    {
      Syscall.CreateMessage(this, nodeId, id, v);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    public CanFlyMsg(ushort id, sbyte b0)
    {
      Syscall.CreateMessage(this, nodeId, id, b0);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    public CanFlyMsg(ushort id, byte b0)
    {
      Syscall.CreateMessage(this, nodeId, id, b0);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    public CanFlyMsg(ushort id, byte b0, byte b1)
    {
      Syscall.CreateMessage(this, nodeId, id, b0, b1);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    public CanFlyMsg(ushort id, sbyte b0, sbyte b1)
    {
      Syscall.CreateMessage(this, nodeId, id, b0, b1);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    public CanFlyMsg(ushort id, byte b0, byte b1, byte b2)
    {
      Syscall.CreateMessage(this, nodeId, id, b0, b1, b2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    public CanFlyMsg(ushort id, sbyte b0, sbyte b1, sbyte b2)
    {
      Syscall.CreateMessage(this, nodeId, id, b0, b1, b2);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    /// <param name="b3">value to assign</param>
    public CanFlyMsg(ushort id, byte b0, byte b1, byte b2, byte b3)
    {
      Syscall.CreateMessage(this, nodeId, id, b0, b1, b2, b3);
    }
    /// <summary>
    /// Create a new message
    /// </summary>
    /// <param name="id">Can ID</param>
    /// <param name="b0">value to assign</param>
    /// <param name="b1">value to assign</param>
    /// <param name="b2">value to assign</param>
    /// <param name="b3">value to assign</param>
    public CanFlyMsg(ushort id, sbyte b0, sbyte b1, sbyte b2, sbyte b3)
    {
      Syscall.CreateMessage(this, nodeId, id, b0, b1, b2, b3);
    }
    /// <summary>
    /// Get the float value encapsulated
    /// </summary>
    /// <returns>Value as a float</returns>
    public float GetFloat()
    {
      return Syscall.GetFloat(this);
    }
    /// <summary>
    /// Get the value as an int32
    /// </summary>
    /// <returns>Value as an Int32</returns>
    public int GetInt32()
    {
      return Syscall.GetInt32(this);
    }
    /// <summary>
    /// Get the value as a uint
    /// </summary>
    /// <returns>Value as an UInt32</returns>
    public uint GetUInt32()
    {
      return Syscall.GetUInt32(this);
    }
    /// <summary>
    /// Gets the value as a short
    /// </summary>
    /// <returns>Value as a Int16</returns>
    public short GetInt16()
    {
      return Syscall.GetInt16(this);
    }
    /// <summary>
    /// Gets the value as a ushort
    /// </summary>
    /// <returns>Value as a UInt16</returns>
    public ushort GetUInt16()
    {
      return Syscall.GetUInt16(this);
    }
    /// <summary>
    /// Return the value as a signed byte
    /// </summary>
    /// <returns>Value as a Int8</returns>
    public sbyte[] GetInt8Array()
    {
      return Syscall.GetInt8Array(this);
    }
    /// <summary>
    /// Return the value as a byte
    /// </summary>
    /// <returns>Value as a UInt7</returns>
    public byte[] GetUInt8Array()
    {
      return Syscall.GetUInt8Array(this);
    }
    /// <summary>
    /// Return the value as an array of Int16
    /// </summary>
    /// <returns>Values as an array of Int16</returns>
    public short[] GetInt16Array()
    {
      return Syscall.GetInt16Array(this);
    }
    /// <summary>
    /// Return the value as an array of ushorts
    /// </summary>
    /// <returns>Values as an array of UInt16</returns>
    public ushort[] GetUInt16Array()
    {
      return Syscall.GetUInt16Array(this);
    }

    public override string ToString()
    {
      return Syscall.MessageToString(this);
    }
  }
}