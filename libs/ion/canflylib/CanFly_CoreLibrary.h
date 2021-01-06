//-----------------------------------------------------------------------------
//
//    ** DO NOT EDIT THIS FILE! **
//    This file was generated by a tool
//    re-running the tool will overwrite this file.
//
//-----------------------------------------------------------------------------

#ifndef _CANFLY_CORELIBRARY_H_
#define _CANFLY_CORELIBRARY_H_

#include "../../nano/CLR/Include/nanoCLR_Interop.h"
#include "../../nano/CLR/Include/nanoCLR_Runtime.h"
#include "../../nano/CLR/Include/nanoPackStruct.h"

typedef enum __nfpack DataType
{
    DataType_NoData = 0,
    DataType_Error = 1,
    DataType_Float = 2,
    DataType_Int32 = 3,
    DataType_UInt32 = 4,
    DataType_Short = 6,
    DataType_UShort = 7,
    DataType_Char = 9,
    DataType_UChar = 10,
    DataType_Short2 = 12,
    DataType_UShort2 = 13,
    DataType_Char4 = 15,
    DataType_UChar4 = 16,
    DataType_Char2 = 18,
    DataType_UChar2 = 19,
    DataType_Char3 = 26,
    DataType_UChar3 = 27,
    DataType_ReservedBegin = 32,
    DataType_ReservedEnd = 99,
    DataType_UserDefinedBegin = 100,
    DataType_UserDefinedEnd = 255,
} DataType;

typedef enum __nfpack Flags
{
    Flags_Reply = 1,
    Flags_Loopback = 2,
    Flags_Broadcast = 4,
} Flags;

typedef enum __nfpack PenStyle
{
    PenStyle_Solid = 0,
    PenStyle_Dash = 1,
    PenStyle_Dot = 2,
    PenStyle_DashDot = 3,
    PenStyle_DashDotDot = 4,
    PenStyle_Null = 5,
} PenStyle;

typedef enum __nfpack TextOutStyle
{
    TextOutStyle_Clipped = 2,
    TextOutStyle_Opaque = 4,
} TextOutStyle;

struct Library_CanFly_CoreLibrary_CanFly_CanFlyEventDispatcher
{
    static const int FIELD__threadSpawn = 1;
    static const int FIELD__callbacks = 2;
    static const int FIELD__disposed = 3;
    static const int FIELD___CanFlyEventDispatcher = 4;

    NANOCLR_NATIVE_DECLARE(Dispose___VOID__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_CanFlyID
{
    static const int FIELD_STATIC__id_unused = 0;
    static const int FIELD_STATIC__id_pitch_acceleration = 1;
    static const int FIELD_STATIC__id_roll_acceleration = 2;
    static const int FIELD_STATIC__id_yaw_acceleration = 3;
    static const int FIELD_STATIC__id_pitch_rate = 4;
    static const int FIELD_STATIC__id_roll_rate = 5;
    static const int FIELD_STATIC__id_yaw_rate = 6;
    static const int FIELD_STATIC__id_roll_angle_magnetic = 7;
    static const int FIELD_STATIC__id_pitch_angle_magnetic = 8;
    static const int FIELD_STATIC__id_yaw_angle_magnetic = 9;
    static const int FIELD_STATIC__id_pitch_angle = 10;
    static const int FIELD_STATIC__id_roll_angle = 11;
    static const int FIELD_STATIC__id_yaw_angle = 12;
    static const int FIELD_STATIC__id_altitude_rate = 13;
    static const int FIELD_STATIC__id_indicated_airspeed = 14;
    static const int FIELD_STATIC__id_true_airspeed = 15;
    static const int FIELD_STATIC__id_calibrated_airspeed = 16;
    static const int FIELD_STATIC__id_angle_of_attack = 17;
    static const int FIELD_STATIC__id_qnh = 18;
    static const int FIELD_STATIC__id_baro_corrected_altitude = 19;
    static const int FIELD_STATIC__id_heading_angle = 20;
    static const int FIELD_STATIC__id_heading = 21;
    static const int FIELD_STATIC__id_pressure_altitude = 22;
    static const int FIELD_STATIC__id_outside_air_temperature = 23;
    static const int FIELD_STATIC__id_differential_pressure = 24;
    static const int FIELD_STATIC__id_static_pressure = 25;
    static const int FIELD_STATIC__id_wind_speed = 26;
    static const int FIELD_STATIC__id_wind_direction = 27;
    static const int FIELD_STATIC__id_gps_latitude = 28;
    static const int FIELD_STATIC__id_gps_longitude = 29;
    static const int FIELD_STATIC__id_gps_height = 30;
    static const int FIELD_STATIC__id_gps_groundspeed = 31;
    static const int FIELD_STATIC__id_true_track = 32;
    static const int FIELD_STATIC__id_magnetic_track = 33;
    static const int FIELD_STATIC__id_magnetic_heading = 34;
    static const int FIELD_STATIC__id_position_latitude = 35;
    static const int FIELD_STATIC__id_position_longitude = 36;
    static const int FIELD_STATIC__id_position_altitude = 37;
    static const int FIELD_STATIC__id_velocity_x = 38;
    static const int FIELD_STATIC__id_velocity_y = 39;
    static const int FIELD_STATIC__id_velocity_z = 40;
    static const int FIELD_STATIC__id_imu_mag_x = 41;
    static const int FIELD_STATIC__id_imu_mag_y = 42;
    static const int FIELD_STATIC__id_imu_mag_z = 43;
    static const int FIELD_STATIC__id_imu_accel_x = 44;
    static const int FIELD_STATIC__id_imu_accel_y = 45;
    static const int FIELD_STATIC__id_imu_accel_z = 46;
    static const int FIELD_STATIC__id_imu_gyro_x = 47;
    static const int FIELD_STATIC__id_imu_gyro_y = 48;
    static const int FIELD_STATIC__id_imu_gyro_z = 49;
    static const int FIELD_STATIC__id_magnetic_variation = 50;
    static const int FIELD_STATIC__id_def_utc = 51;
    static const int FIELD_STATIC__id_def_date = 52;
    static const int FIELD_STATIC__id_qnh_up = 53;
    static const int FIELD_STATIC__id_qnh_dn = 54;
    static const int FIELD_STATIC__id_set_magnetic_variation = 55;
    static const int FIELD_STATIC__id_heading_up = 56;
    static const int FIELD_STATIC__id_heading_dn = 57;
    static const int FIELD_STATIC__id_stall_warning = 58;
    static const int FIELD_STATIC__id_total_time = 59;
    static const int FIELD_STATIC__id_air_time = 60;
    static const int FIELD_STATIC__id_tach_time = 61;
    static const int FIELD_STATIC__id_pitch = 62;
    static const int FIELD_STATIC__id_track = 63;
    static const int FIELD_STATIC__id_deviation = 64;
    static const int FIELD_STATIC__id_track_angle_error = 65;
    static const int FIELD_STATIC__id_estimated_time_to_next = 66;
    static const int FIELD_STATIC__id_estimated_time_of_arrival = 67;
    static const int FIELD_STATIC__id_estimated_enroute_time = 68;
    static const int FIELD_STATIC__id_waypoint_identifier_0_3 = 69;
    static const int FIELD_STATIC__id_waypoint_identifier_4_7 = 70;
    static const int FIELD_STATIC__id_waypoint_identifier_8_11 = 71;
    static const int FIELD_STATIC__id_waypoint_identifier_12_15 = 72;
    static const int FIELD_STATIC__id_waypoint_type_identifier = 73;
    static const int FIELD_STATIC__id_waypoint_latitude = 74;
    static const int FIELD_STATIC__id_waypoint_longitude = 75;
    static const int FIELD_STATIC__id_waypoint_minimum_altitude = 76;
    static const int FIELD_STATIC__id_waypoint_maximum_altitude = 77;
    static const int FIELD_STATIC__id_distance_to_next = 78;
    static const int FIELD_STATIC__id_distance_to_destination = 79;
    static const int FIELD_STATIC__id_selected_course = 80;
    static const int FIELD_STATIC__id_desired_track_angle = 81;
    static const int FIELD_STATIC__id_nav_command = 82;
    static const int FIELD_STATIC__id_nav_valid = 83;
    static const int FIELD_STATIC__id_gps_valid = 84;
    static const int FIELD_STATIC__id_imu_valid = 85;
    static const int FIELD_STATIC__id_edu_valid = 86;
    static const int FIELD_STATIC__id_aux_battery = 87;
    static const int FIELD_STATIC__id_trim_up = 88;
    static const int FIELD_STATIC__id_trim_dn = 89;
    static const int FIELD_STATIC__id_trim_left = 90;
    static const int FIELD_STATIC__id_trim_right = 91;
    static const int FIELD_STATIC__id_autopilot_engage = 92;
    static const int FIELD_STATIC__id_autopilot_set_max_roll = 93;
    static const int FIELD_STATIC__id_autopilot_set_vs_rate = 94;
    static const int FIELD_STATIC__id_waypoint_turn_heading = 95;
    static const int FIELD_STATIC__id_roll_servo_set_position = 96;
    static const int FIELD_STATIC__id_pitch_servo_set_postion = 97;
    static const int FIELD_STATIC__id_autopilot_power = 98;
    static const int FIELD_STATIC__id_autopilot_alt_mode = 99;
    static const int FIELD_STATIC__id_autopilot_vs_mode = 100;
    static const int FIELD_STATIC__id_autopilot_mode = 101;
    static const int FIELD_STATIC__id_autopilot_status = 102;
    static const int FIELD_STATIC__id_roll_servo_status = 103;
    static const int FIELD_STATIC__id_pitch_servo_status = 104;
    static const int FIELD_STATIC__id_autopilot_altitude = 105;
    static const int FIELD_STATIC__id_autopilot_vertical_speed = 106;
    static const int FIELD_STATIC__id_engine_rpm = 107;
    static const int FIELD_STATIC__id_engine_rpm_a = 108;
    static const int FIELD_STATIC__id_exhaust_gas_temperature1 = 109;
    static const int FIELD_STATIC__id_exhaust_gas_temperature2 = 110;
    static const int FIELD_STATIC__id_exhaust_gas_temperature3 = 111;
    static const int FIELD_STATIC__id_exhaust_gas_temperature4 = 112;
    static const int FIELD_STATIC__id_manifold_pressure = 113;
    static const int FIELD_STATIC__id_oil_pressure = 114;
    static const int FIELD_STATIC__id_oil_temperature = 115;
    static const int FIELD_STATIC__id_cylinder_head_temperature1 = 116;
    static const int FIELD_STATIC__id_cylinder_head_temperature2 = 117;
    static const int FIELD_STATIC__id_cylinder_head_temperature3 = 118;
    static const int FIELD_STATIC__id_cylinder_head_temperature4 = 119;
    static const int FIELD_STATIC__id_engine_rpm_b = 120;
    static const int FIELD_STATIC__id_manifold_pressure_b = 121;
    static const int FIELD_STATIC__id_fuel_pressure = 122;
    static const int FIELD_STATIC__id_dc_voltage = 123;
    static const int FIELD_STATIC__id_dc_current = 124;
    static const int FIELD_STATIC__id_fuel_flow_rate = 125;
    static const int FIELD_STATIC__id_fuel_consumed = 126;
    static const int FIELD_STATIC__id_edu_left_fuel_quantity = 127;
    static const int FIELD_STATIC__id_edu_right_fuel_quantity = 128;
    static const int FIELD_STATIC__id_timing_divergence = 129;
    static const int FIELD_STATIC__id_left_mag_rpm = 130;
    static const int FIELD_STATIC__id_right_mag_rpm = 131;
    static const int FIELD_STATIC__id_left_mag_adv = 132;
    static const int FIELD_STATIC__id_right_mag_adv = 133;
    static const int FIELD_STATIC__id_left_mag_map = 134;
    static const int FIELD_STATIC__id_right_mag_map = 135;
    static const int FIELD_STATIC__id_left_mag_volt = 136;
    static const int FIELD_STATIC__id_right_mag_volt = 137;
    static const int FIELD_STATIC__id_left_mag_temp = 138;
    static const int FIELD_STATIC__id_right_mag_temp = 139;
    static const int FIELD_STATIC__id_left_mag_coil1 = 140;
    static const int FIELD_STATIC__id_right_mag_coil1 = 141;
    static const int FIELD_STATIC__id_left_mag_coil2 = 142;
    static const int FIELD_STATIC__id_right_mag_coil2 = 143;
    static const int FIELD_STATIC__id_fuel_flow_rate_left = 144;
    static const int FIELD_STATIC__id_fuel_flow_rate_right = 145;
    static const int FIELD_STATIC__id_active_fuel_tank = 146;
    static const int FIELD_STATIC__id_left_fuel_quantity = 147;
    static const int FIELD_STATIC__id_right_fuel_quantity = 148;
    static const int FIELD_STATIC__id_pitot_temperature = 149;
    static const int FIELD_STATIC__id_pitot_heat_status = 150;
    static const int FIELD_STATIC__id_pitot_power_status = 151;
    static const int FIELD_STATIC__id_fdu_board_temperature = 152;
    static const int FIELD_STATIC__id_set_left_fuel_qty = 153;
    static const int FIELD_STATIC__id_set_right_fuel_qty = 154;
    static const int FIELD_STATIC__id_mixture_lever = 155;
    static const int FIELD_STATIC__id_injector_dwell = 156;
    static const int FIELD_STATIC__id_propeller_speed_lever = 157;
    static const int FIELD_STATIC__id_propeller_motor_current = 158;
    static const int FIELD_STATIC__id_propeller_low_speed = 159;
    static const int FIELD_STATIC__id_propeller_high_speed = 160;
    static const int FIELD_STATIC__id_set_route = 161;
    static const int FIELD_STATIC__id_set_route_segments = 162;
    static const int FIELD_STATIC__id_set_waypoint_identifier_0_3 = 163;
    static const int FIELD_STATIC__id_set_waypoint_identifier_4_7 = 164;
    static const int FIELD_STATIC__id_set_waypoint_identifier_8_11 = 165;
    static const int FIELD_STATIC__id_set_waypoint_identifier_12_15 = 166;
    static const int FIELD_STATIC__id_set_waypoint_type_identifier = 167;
    static const int FIELD_STATIC__id_set_waypoint_latitude = 168;
    static const int FIELD_STATIC__id_set_waypoint_longitude = 169;
    static const int FIELD_STATIC__id_set_waypoint_minimum_altitude = 170;
    static const int FIELD_STATIC__id_set_waypoint_maximum_altitude = 171;
    static const int FIELD_STATIC__id_navigation_mode = 172;
    static const int FIELD_STATIC__id_alternator_status = 173;
    static const int FIELD_STATIC__id_voltage_warning = 174;
    static const int FIELD_STATIC__id_panel_brightness = 175;
    static const int FIELD_STATIC__id_pitot_temperature_alarm = 176;
    static const int FIELD_STATIC__id_nav_msg = 177;
    static const int FIELD_STATIC__id_nav_appr = 178;
    static const int FIELD_STATIC__id_nav_ils = 179;
    static const int FIELD_STATIC__id_voltage_sense_1 = 180;
    static const int FIELD_STATIC__id_voltage_sense_2 = 181;
    static const int FIELD_STATIC__id_user_defined_start = 182;
    static const int FIELD_STATIC__id_user_defined_end = 183;
    static const int FIELD_STATIC__id_node_register = 184;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_CanFlyMsg
{
    static const int FIELD___flags = 1;
    static const int FIELD___data0 = 2;
    static const int FIELD___data1 = 3;
    static const int FIELD___data2 = 4;
    static const int FIELD___data3 = 5;
    static const int FIELD___data4 = 6;
    static const int FIELD___data5 = 7;
    static const int FIELD___data6 = 8;
    static const int FIELD___data7 = 9;

    NANOCLR_NATIVE_DECLARE(ToString___STRING);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U2__U4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__I4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U2);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__I2);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U2__U2);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__I2__I2);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__R4);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U1__U1);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__CHAR__CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__CHAR__CHAR__CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U1__U1__U1__U1);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__CHAR__CHAR__CHAR__CHAR);
    NANOCLR_NATIVE_DECLARE(_ctor___VOID__U2__U1);
    NANOCLR_NATIVE_DECLARE(get_Length___U1);
    NANOCLR_NATIVE_DECLARE(SetFlags___VOID__U2);
    NANOCLR_NATIVE_DECLARE(GetFlags___U2);
    NANOCLR_NATIVE_DECLARE(get_Id___U2);
    NANOCLR_NATIVE_DECLARE(get_NodeId___U1);
    NANOCLR_NATIVE_DECLARE(set_NodeId___VOID__U1);
    NANOCLR_NATIVE_DECLARE(GetDataType___U2);
    NANOCLR_NATIVE_DECLARE(get_ServiceCode___U1);
    NANOCLR_NATIVE_DECLARE(set_ServiceCode___VOID__U1);
    NANOCLR_NATIVE_DECLARE(get_MessageCode___U1);
    NANOCLR_NATIVE_DECLARE(set_MessageCode___VOID__U1);
    NANOCLR_NATIVE_DECLARE(GetFloat___R4);
    NANOCLR_NATIVE_DECLARE(GetInt32___I4);
    NANOCLR_NATIVE_DECLARE(GetUInt32___U4);
    NANOCLR_NATIVE_DECLARE(GetInt16___I2);
    NANOCLR_NATIVE_DECLARE(GetUInt16___U2);
    NANOCLR_NATIVE_DECLARE(GetInt8___I1);
    NANOCLR_NATIVE_DECLARE(GetUInt8___U1);
    NANOCLR_NATIVE_DECLARE(GetInt16Array___SZARRAY_I2);
    NANOCLR_NATIVE_DECLARE(GetUInt16Array___SZARRAY_U2);
    NANOCLR_NATIVE_DECLARE(GetInt8Array___SZARRAY_I1);
    NANOCLR_NATIVE_DECLARE(GetUInt8Array___SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(ToString___STRING__STRING);
    NANOCLR_NATIVE_DECLARE(SendMessage___VOID__U4);
    NANOCLR_NATIVE_DECLARE(Send___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(SendRaw___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(SendReply___STATIC__VOID);
    NANOCLR_NATIVE_DECLARE(GetMessage___STATIC__BOOLEAN__U4__BYREF_U4);
    NANOCLR_NATIVE_DECLARE(DispatchMessage___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(PostMessage___STATIC__VOID__U4__U4);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_CanFlyMsgSink__EventInfo
{
    static const int FIELD__EventListener = 1;
    static const int FIELD__EventID = 2;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_CanFlyMsgSink
{
    static const int FIELD_STATIC__s_eventSink = 185;
    static const int FIELD_STATIC__s_eventInfoTable = 186;

    NANOCLR_NATIVE_DECLARE(EventConfig___VOID);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Colors
{
    static const int FIELD_STATIC__White = 187;
    static const int FIELD_STATIC__Black = 188;
    static const int FIELD_STATIC__Gray = 189;
    static const int FIELD_STATIC__LightGray = 190;
    static const int FIELD_STATIC__DarkGray = 191;
    static const int FIELD_STATIC__Red = 192;
    static const int FIELD_STATIC__Pink = 193;
    static const int FIELD_STATIC__Blue = 194;
    static const int FIELD_STATIC__Green = 195;
    static const int FIELD_STATIC__LightGreen = 196;
    static const int FIELD_STATIC__Yellow = 197;
    static const int FIELD_STATIC__Magenta = 198;
    static const int FIELD_STATIC__Cyan = 199;
    static const int FIELD_STATIC__PaleYellow = 200;
    static const int FIELD_STATIC__LightYellow = 201;
    static const int FIELD_STATIC__LimeGreen = 202;
    static const int FIELD_STATIC__Teal = 203;
    static const int FIELD_STATIC__DarkGreen = 204;
    static const int FIELD_STATIC__Maroon = 205;
    static const int FIELD_STATIC__Purple = 206;
    static const int FIELD_STATIC__Orange = 207;
    static const int FIELD_STATIC__Khaki = 208;
    static const int FIELD_STATIC__Olive = 209;
    static const int FIELD_STATIC__Brown = 210;
    static const int FIELD_STATIC__Navy = 211;
    static const int FIELD_STATIC__LightBlue = 212;
    static const int FIELD_STATIC__FadedBlue = 213;
    static const int FIELD_STATIC__LightGrey = 214;
    static const int FIELD_STATIC__DarkGrey = 215;
    static const int FIELD_STATIC__Hollow = 216;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_FileStream
{
    NANOCLR_NATIVE_DECLARE(CreateDirectory___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(RemoveDirectory___STATIC__VOID__STRING);
    NANOCLR_NATIVE_DECLARE(GetDirectoryEnumertor___STATIC__U4__STRING);
    NANOCLR_NATIVE_DECLARE(FileStreamOpen___STATIC__U4__STRING);
    NANOCLR_NATIVE_DECLARE(FileStreamCreate___STATIC__U4__STRING);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_DirectoryEntry
{
    static const int FIELD___path = 1;
    static const int FIELD___name = 2;
    static const int FIELD___isFile = 3;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_DirectoryEnumerator
{
    static const int FIELD___handle = 1;
    static const int FIELD___pos = 2;
    static const int FIELD___enumPos = 3;

    NANOCLR_NATIVE_DECLARE(GetDirectoryEntry___STATIC__BOOLEAN__U4__BYREF_STRING__BYREF_STRING);
    NANOCLR_NATIVE_DECLARE(RewindDirectoryEntry___STATIC__VOID__U4);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Extent
{
    static const int FIELD___dx = 1;
    static const int FIELD___dy = 2;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Font
{
    static const int FIELD___hndl = 1;

    NANOCLR_NATIVE_DECLARE(GetFont___STATIC__U4__STRING__U4);
    NANOCLR_NATIVE_DECLARE(ReleaseFont___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(LoadFont___STATIC__VOID__U4);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Point
{
    static const int FIELD___x = 1;
    static const int FIELD___y = 2;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Rect
{
    static const int FIELD___left = 1;
    static const int FIELD___top = 2;
    static const int FIELD___right = 3;
    static const int FIELD___bottom = 4;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_GdiObject
{
    static const int FIELD___hndl = 1;
    static const int FIELD___clip_rect = 2;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Neutron
{
    NANOCLR_NATIVE_DECLARE(RegCreateKey___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegOpenKey___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegGetInt8___STATIC__I1__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetInt8___STATIC__VOID__U4__STRING__I1);
    NANOCLR_NATIVE_DECLARE(RegGetUint8___STATIC__U1__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetUint8___STATIC__VOID__U4__STRING__U1);
    NANOCLR_NATIVE_DECLARE(RegGetInt16___STATIC__I2__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetInt16___STATIC__VOID__U4__STRING__I2);
    NANOCLR_NATIVE_DECLARE(RegGetUint16___STATIC__U2__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetUint16___STATIC__VOID__U4__STRING__U2);
    NANOCLR_NATIVE_DECLARE(RegGetInt32___STATIC__I4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetInt32___STATIC__VOID__U4__STRING__I4);
    NANOCLR_NATIVE_DECLARE(RegGetUint32___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetUint32___STATIC__VOID__U4__STRING__U4);
    NANOCLR_NATIVE_DECLARE(RegGetString___STATIC__STRING__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetString___STATIC__VOID__U4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(RegGetFloat___STATIC__R4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetFloat___STATIC__VOID__U4__STRING__R4);
    NANOCLR_NATIVE_DECLARE(RegGetBool___STATIC__BOOLEAN__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegSetBool___STATIC__VOID__U4__STRING__BOOLEAN);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Pen
{
    static const int FIELD___handle = 1;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Photon
{
    NANOCLR_NATIVE_DECLARE(OpenScreen___STATIC__U4__U2__U2);
    NANOCLR_NATIVE_DECLARE(CreateWindow___STATIC__U4__U4__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(CreateChildWindow___STATIC__U4__U4__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(CloseWindow___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(GetWindowRect___STATIC__VOID__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(GetWindowPos___STATIC__VOID__U4__BYREF_I4__BYREF_I4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(SetWindowPos___STATIC__VOID__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetWindowData___STATIC__OBJECT__U4);
    NANOCLR_NATIVE_DECLARE(SetWindowData___STATIC__VOID__U4__OBJECT);
    NANOCLR_NATIVE_DECLARE(GetParent___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetWindowById___STATIC__U4__U4__U2);
    NANOCLR_NATIVE_DECLARE(GetFirstChild___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetNextSibling___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetPreviousSibling___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(InsertBefore___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(InsertAfter___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(GetZOrder___STATIC__U1__U4);
    NANOCLR_NATIVE_DECLARE(SetZOrder___STATIC__VOID__U4__U1);
    NANOCLR_NATIVE_DECLARE(CanvasClose___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(CreateRectCanvas___STATIC__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(CreatePngCanvas___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(LoadPng___STATIC__VOID__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetCanvasExtents___STATIC__VOID__U4__BYREF_U2__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(GetOrientation___STATIC__U2__U4);
    NANOCLR_NATIVE_DECLARE(SetOrientation___STATIC__VOID__U4__U2);
    NANOCLR_NATIVE_DECLARE(CreatePen___STATIC__U4__U4__U2__U2);
    NANOCLR_NATIVE_DECLARE(DisposePen___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(Polyline___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4);
    NANOCLR_NATIVE_DECLARE(Ellipse___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Polygon___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__U4);
    NANOCLR_NATIVE_DECLARE(Rectangle___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(RoundRect___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(BitBlt___STATIC__VOID__U4__I4__I4__I4__I4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(GetPixel___STATIC__U4__U4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(SetPixel___STATIC__U4__U4__I4__I4__I4__I4__I4__I4__U4);
    NANOCLR_NATIVE_DECLARE(Arc___STATIC__VOID__U4__I4__I4__I4__I4__U4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(Pie___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__I4__I4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(OpenFont___STATIC__U4__STRING__U2);
    NANOCLR_NATIVE_DECLARE(DrawText___STATIC__VOID__U4__I4__I4__I4__I4__U4__U4__U4__STRING__I4__I4__I4__I4__I4__I4__U2);
    NANOCLR_NATIVE_DECLARE(TextExtent___STATIC__VOID__U4__U4__STRING__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(InvalidateRect___STATIC__VOID__U4__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(IsValid___STATIC__BOOLEAN__U4);
    NANOCLR_NATIVE_DECLARE(BeginPaint___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(EndPaint___STATIC__VOID__U4);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_PhotonID
{
    static const int FIELD_STATIC__id_photon = 217;
    static const int FIELD_STATIC__id_paint = 218;
    static const int FIELD_STATIC__id_create = 219;
    static const int FIELD_STATIC__id_close = 220;
    static const int FIELD_STATIC__id_timer = 221;
    static const int FIELD_STATIC__id_key0 = 222;
    static const int FIELD_STATIC__id_key1 = 223;
    static const int FIELD_STATIC__id_key2 = 224;
    static const int FIELD_STATIC__id_key3 = 225;
    static const int FIELD_STATIC__id_key4 = 226;
    static const int FIELD_STATIC__id_decka = 227;
    static const int FIELD_STATIC__id_deckb = 228;
    static const int FIELD_STATIC__id_menu_up = 229;
    static const int FIELD_STATIC__id_menu_dn = 230;
    static const int FIELD_STATIC__id_menu_left = 231;
    static const int FIELD_STATIC__id_menu_right = 232;
    static const int FIELD_STATIC__id_menu_ok = 233;
    static const int FIELD_STATIC__id_menu_cancel = 234;
    static const int FIELD_STATIC__id_menu_select = 235;
    static const int FIELD_STATIC__id_buttonpress = 236;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_PointArray
{
    static const int FIELD___handle = 1;

    NANOCLR_NATIVE_DECLARE(PointArrayCreate___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayRelease___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(PointArraySetPoint___STATIC__VOID__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayClear___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(PointArraySize___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayResize___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(PointArrayAppend___STATIC__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayInsertAt___STATIC__VOID__U4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayGetPoint___STATIC__VOID__U4__U4__BYREF_I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(PointArrayIndexOf___STATIC__I4__U4__I4__I4);
    NANOCLR_NATIVE_DECLARE(PointArrayRemoveAt___STATIC__VOID__U4__U4);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_PointArray__PointEnumerator
{
    static const int FIELD___outer = 1;
    static const int FIELD___index = 2;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_RegistryStream
{
    NANOCLR_NATIVE_DECLARE(RegStreamOpen___STATIC__U4__U4__STRING);
    NANOCLR_NATIVE_DECLARE(RegStreamCreate___STATIC__U4__U4__STRING);

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Screen
{
    static const int FIELD_STATIC___screen = 237;
    static const int FIELD_STATIC__widgetLock = 238;

    //--//

};

struct Library_CanFly_CoreLibrary_CanFly_Stream
{
    static const int FIELD___handle = 1;

    NANOCLR_NATIVE_DECLARE(StreamEof___STATIC__BOOLEAN__U4);
    NANOCLR_NATIVE_DECLARE(StreamRead___STATIC__SZARRAY_U1__U4__U2);
    NANOCLR_NATIVE_DECLARE(StreamWrite___STATIC__VOID__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(StreamGetPos___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamSetPos___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamLength___STATIC__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamTruncate___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamCopy___STATIC__VOID__U4__U4);
    NANOCLR_NATIVE_DECLARE(StreamPath___STATIC__STRING__U4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(StreamClose___STATIC__VOID__U4);
    NANOCLR_NATIVE_DECLARE(StreamDelete___STATIC__VOID__U4);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_canflylib;

#endif  //_CANFLY_CORELIBRARY_H_
