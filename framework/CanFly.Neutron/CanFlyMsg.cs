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
  ReservedBegin = 32,
  ReservedEnd = 99,
  UserDefinedBegin = 100,
  UserDefinedEnd = 255,
};

public sealed class CanFlyMsg
  {
    // same coding as CanAerospace
    // Bits 15:12 - Length
    // Bit 11 - Reply
    // bits 10-0 ID
    private ushort _flags;
    private byte _b0;
    private byte _b1;
    private byte _b2;
    private byte _b3;
    private byte _b4;
    private byte _b5;
    private byte _b6;
    private byte _b7;

    /// <summary>
    /// Flag for a reply message
    /// </summary>
    public bool Reply
    {
      get { return (_flags & 0x0800) != 0; }
      set { _flags |= 0x0800; }
    }
    /// <summary>
    /// Length of the message
    /// </summary>
    public ushort Length
    {
      get { return (ushort)((_flags & 0xF000) >> 12); }
      private set { _flags = (ushort)((_flags & 0xF000) | ((value & 0x0F) << 12)); }
    }
    /// <summary>
    /// ID of the CAN message
    /// </summary>
    public ushort CanID
    {
      get { return (ushort)(_flags & 0x07FF); }
      set { _flags = (ushort)((_flags & 0xF800) | (value & 0x07FF)); }
    }
    internal ushort Flags {  get { return _flags; } }
    internal byte Data0 { get { return _b0; } }
    internal byte Data1 { get { return _b1; } }
    internal byte Data2 { get { return _b2; } }
    internal byte Data3 { get { return _b3; } }
    internal byte Data4 { get { return _b4; } }
    internal byte Data5 { get { return _b5; } }
    internal byte Data6 { get { return _b6; } }
    internal byte Data7 { get { return _b7; } }
    /// <summary>
    /// Return the message as a boxed value
    /// </summary>
    public object Value
    {
      get
      {
        switch(DataType)
        {
          case CanFlyDataType.Char:
            return GetInt8();
          case CanFlyDataType.Char2:
          case CanFlyDataType.Char3:
          case CanFlyDataType.Char4:
          case CanFlyDataType.Error:
            return "Error";
          case CanFlyDataType.Float:
            return GetFloat();
          case CanFlyDataType.Int32:
            return GetInt32();
          case CanFlyDataType.NoData:
            return null;
          case CanFlyDataType.Short:
            return GetInt16();
          case CanFlyDataType.Short2:
            return GetInt16Array();
          case CanFlyDataType.UChar:
            return GetUInt8();
          case CanFlyDataType.UChar2:
          case CanFlyDataType.UChar3:
          case CanFlyDataType.UChar4:
            return GetUInt8Array();
          case CanFlyDataType.UInt32:
            return GetUInt32();
          case CanFlyDataType.UShort:
            return GetUInt16();
          case CanFlyDataType.UShort2:
            return GetUInt16Array();
          default:
            return DataType.ToString();
        }
      }
    }

    /// <summary>
    /// Create a new message with the requested datatype.  The value is set to default (0 for integers)
    /// </summary>
    /// <param name="id">canfly id to assign</param>
    /// <param name="dataType">implementation datatype</param>
    public CanFlyMsg(ushort id, CanFlyDataType dataType, uint rawData)
    {
      CanID = id;
      Length = 8;
    }

    internal CanFlyMsg(ushort flags, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7)
    {
      _flags = flags;
      _b0 = b0;
      _b1 = b1;
      _b2 = b2;
      _b3 = b3;
      _b4 = b4;
      _b5 = b5;
      _b6 = b6;
      _b7 = b7;
    }
    /// <summary>
    /// CanFly node id
    /// </summary>
    public byte NodeId 
    { 
      get { return _b0; } 
      set { _b0 = value; }
    }
    /// <summary>
    /// The type of data
    /// </summary>
    public CanFlyDataType DataType 
    {
      get { return (CanFlyDataType) _b1; } 
      private set { _b1 = (byte)value; }
    }
    /// <summary>
    /// The service code
    /// </summary>
    internal byte ServiceCode 
    {
      get { return _b2; } 
      set { _b2 = value; }
    }
    /// <summary>
    /// The message code
    /// </summary>
    internal byte MessageCode 
    {
      get { return _b3; } 
      set { _b3 = value; }
    }

    /// <summary>
    /// Create a new datatype with a uint32
    /// </summary>
    /// <param name="id"></param>
    /// <param name="v"></param>
    public CanFlyMsg(ushort id, uint value)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UInt32;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackUInt32(value, ref _b4, ref _b5, ref _b6, ref _b7);
    }

    public CanFlyMsg(ushort id, int value)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Int32;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackInt32(value, ref _b4, ref _b5, ref _b6, ref _b7);
    }


    public CanFlyMsg(ushort id, ushort value)
    {
      Length = 6;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UShort;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackUInt16(value, ref _b4, ref _b5);
      _b6 = 0;
      _b7 = 0;
    }


    public CanFlyMsg(ushort id, short value)
    {
      Length = 6;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Short;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackInt16(value, ref _b4, ref _b5);
      _b6 = 0;
      _b7 = 0;
    }


    public CanFlyMsg(ushort id, ushort v1, ushort v2)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UShort2;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackUInt16(v1, ref _b4, ref _b5);
      Syscall.PackUInt16(v2, ref _b6, ref _b7);
    }


    public CanFlyMsg(ushort id, short v1, short v2)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Short2;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackInt16(v1, ref _b4, ref _b5);
      Syscall.PackInt16(v2, ref _b6, ref _b7);
    }


    public CanFlyMsg(ushort id, float v)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Float;
      ServiceCode = 0;
      MessageCode = 0;
      Syscall.PackFloat(v, ref _b4, ref _b5, ref _b6, ref _b7);
    }


    public CanFlyMsg(ushort id, char c)
    {
      Length = 5;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Char;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = (byte) c;
      _b5 = 0;
      _b6 = 0;
      _b7 = 0;
    }

    public CanFlyMsg(ushort id, byte b0)
    {
      Length = 5;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UChar;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = b0;
      _b5 = 0;
      _b6 = 0;
      _b7 = 0;
    }

    public CanFlyMsg(ushort id, byte b0, byte b1)
    {
      Length = 6;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UChar2;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = b0;
      _b5 = b1;
      _b6 = 0;
      _b7 = 0;
    }


    public CanFlyMsg(ushort id, char c0, char c1)
    {
      Length = 6;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Char2;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = (byte)c0;
      _b5 = (byte)c1;
      _b6 = 0;
      _b7 = 0;
    }


    public CanFlyMsg(ushort id, byte b0, byte b1, byte b2)
    {
      Length = 7;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UChar3;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = b0;
      _b5 = b1;
      _b6 = b2;
      _b7 = 0;
    }


    public CanFlyMsg(ushort id, char c0, char c1, char c2)
    {
      Length = 7;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Char3;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = (byte)c0;
      _b5 = (byte)c1;
      _b6 = (byte)c2;
      _b7 = 0;
    }


    public CanFlyMsg(ushort id, byte b0, byte b1, byte b2, byte b3)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.UChar4;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = b0;
      _b5 = b1;
      _b6 = b2;
      _b7 = b3;
    }


    public CanFlyMsg(ushort id, char c0, char c1, char c2, char c3)
    {
      Length = 8;
      CanID = id;
      NodeId = 0;
      DataType = CanFlyDataType.Char4;
      ServiceCode = 0;
      MessageCode = 0;
      _b4 = (byte)c0;
      _b5 = (byte)c1;
      _b6 = (byte)c2;
      _b7 = (byte)c3;
    }

    public float GetFloat()
    {
      if (DataType != CanFlyDataType.Float)
        throw new InvalidCastException();

      float value;
      ExceptionHelper.ThrowIfFailed(Syscall.GetFloat(_b4, _b5, _b6, _b7, out value));

      return value;
    }
    
    public int GetInt32()
    {
      if (DataType != CanFlyDataType.Int32)
        throw new InvalidCastException();

      int value;
      ExceptionHelper.ThrowIfFailed(Syscall.GetInt32(_b4, _b5, _b6, _b7, out value));

      return value;
    }


    public uint GetUInt32()
    {
      if (DataType != CanFlyDataType.UInt32)
        throw new InvalidCastException();

      uint value;
      ExceptionHelper.ThrowIfFailed(Syscall.GetUInt32(_b4, _b5, _b6, _b7, out value));

      return value;
    }


    public short GetInt16()
    {
      if (DataType != CanFlyDataType.Short)
        throw new InvalidCastException();

      short value;
      ExceptionHelper.ThrowIfFailed(Syscall.GetInt16(_b4, _b5, out value));

      return value;
    }


    public ushort GetUInt16()
    {
      if (DataType != CanFlyDataType.UShort)
        throw new InvalidCastException();

      ushort value;
      ExceptionHelper.ThrowIfFailed(Syscall.GetUInt16(_b4, _b5, out value));

      return value;
    }

    public sbyte GetInt8()
    {
      if(DataType != CanFlyDataType.Char)
        throw new InvalidCastException();

      return (sbyte) _b4;
    }


    public byte GetUInt8()
    {
      if (DataType != CanFlyDataType.Char)
        throw new InvalidCastException();

      return _b4;
    }


    public short[] GetInt16Array()
    {
      if (DataType != CanFlyDataType.Short2)
        throw new InvalidCastException();

      short[] value = new short[2];
      ExceptionHelper.ThrowIfFailed(Syscall.GetInt16(_b4, _b5, out value[0]));

      ExceptionHelper.ThrowIfFailed(Syscall.GetInt16(_b6, _b7, out value[1]));

      return value;
    }


    public ushort[] GetUInt16Array()
    {
      if (DataType != CanFlyDataType.UShort2)
        throw new InvalidCastException();

      ushort[] value = new ushort[2];
      ExceptionHelper.ThrowIfFailed(Syscall.GetUInt16(_b4, _b5, out value[0]));

      ExceptionHelper.ThrowIfFailed(Syscall.GetUInt16(_b6, _b7, out value[1]));

      return value;
    }


    public sbyte[] GetInt8Array()
    {
      switch(DataType)
      {
        case CanFlyDataType.Char:
          return new sbyte[] { (sbyte)_b4 };
        case CanFlyDataType.Char2:
          return new sbyte[] { (sbyte)_b4, (sbyte)_b5 };
        case CanFlyDataType.Char3:
          return new sbyte[] { (sbyte)_b4, (sbyte)_b5, (sbyte)_b6 };
        case CanFlyDataType.Char4:
          return new sbyte[] { (sbyte)_b4, (sbyte)_b5, (sbyte)_b6, (sbyte)_b7 };
      }

      throw new InvalidCastException();
    }


    public byte[] GetUInt8Array()
    {
      switch (DataType)
      {
        case CanFlyDataType.UChar:
          return new byte[] { _b4 };
        case CanFlyDataType.UChar2:
          return new byte[] { _b4, _b5 };
        case CanFlyDataType.UChar3:
          return new byte[] { _b4, _b5, _b6 };
        case CanFlyDataType.UChar4:
          return new byte[] { _b4, _b5, _b6, _b7 };
      }

      throw new InvalidCastException();
    }


    public override string ToString()
    {
      return string.Format("id={0}, dt={1}", CanID, DataType.ToString());
    }
  }
}