/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __neutron_h__
#define __neutron_h__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/**
 * @file neutron.h
 * Neutron micro kernel definitions
 */

#ifdef __cplusplus
extern "C" {
#endif

  typedef int result_t;
  typedef uint8_t byte_t;

  typedef void *handle_t;

  extern void enter_critical();
  extern void exit_critical();

  enum {
    s_ok = 0,
    s_false = -1,
    e_unexpected = -2,
    e_invalid_handle = -3,
    e_not_implemented = -4,
    e_bad_pointer = -5,
    e_bad_parameter = -6,
    e_more_data = -7,
    e_no_more_information = -8,
    e_path_not_found = -9,
    e_operation_pending = -10,
    e_operation_cancelled = -11,
    e_invalid_operation = -12,
    e_buffer_too_small = -13,
    e_generic_error = -14,
    e_timeout_error = -15,
    e_no_space = -16,
    e_not_enough_memory = -17,
    e_not_found = -18,
    e_bad_handle = -19,
    e_bad_ioctl = -20,
    e_ioctl_buffer_too_small = -21,
    e_not_initialized = -22,
    e_exists = -23,
    e_wrong_type = -24,
    e_parse_error = -25,
    e_incomplete_command = -26,
    e_bad_type = -27,
    };

  static inline bool failed(result_t r)
    {
    return r < 0;
    }

  static inline bool succeeded(result_t r)
    {
    return r >= 0;
    }

  // High Priority Node service channels
#define node_service_channel_0        128
#define node_service_channel_1        130
#define node_service_channel_2        132
#define node_service_channel_3        134
#define node_service_channel_4        136
#define node_service_channel_5        138
#define node_service_channel_6        140
#define node_service_channel_7        142
#define node_service_channel_8        144
#define node_service_channel_9        146
#define node_service_channel_10       148
#define node_service_channel_11       150
#define node_service_channel_12       152
#define node_service_channel_13       154
#define node_service_channel_14       156
#define node_service_channel_15       158
#define node_service_channel_16       160
#define node_service_channel_17       162
#define node_service_channel_18       164
#define node_service_channel_19       166
#define node_service_channel_20       168
#define node_service_channel_21       170
#define node_service_channel_22       172
#define node_service_channel_23       174
#define node_service_channel_24       176
#define node_service_channel_25       178
#define node_service_channel_26       180
#define node_service_channel_27       182
#define node_service_channel_28       184
#define node_service_channel_29       186
#define node_service_channel_30       188
#define node_service_channel_31       190
#define node_service_channel_32       192
#define node_service_channel_33       194
#define node_service_channel_34       196
#define node_service_channel_35       198

  // LOW Priority node service channels
#define node_service_channel_100      2000
#define node_service_channel_101      2002
#define node_service_channel_102      2004
#define node_service_channel_103      2006
#define node_service_channel_104      2008
#define node_service_channel_105      2010
#define node_service_channel_106      2012
#define node_service_channel_107      2014
#define node_service_channel_108      2016
#define node_service_channel_109      2018
#define node_service_channel_110      2020
#define node_service_channel_111      2022
#define node_service_channel_112      2024
#define node_service_channel_113      2026
#define node_service_channel_114      2028
#define node_service_channel_115      2030


// service ID's
#define id_ids_service                0
#define id_nss_service                1
#define id_dds_service                2
#define id_dus_service                3
#define id_scs_service                4
#define id_tis_service                5
#define id_fps_service                6
#define id_sts_service                7
#define id_fss_service                8
#define id_tcs_service                9
#define id_bss_service                10
#define id_nis_service                11
#define id_mis_service                12
#define id_mcs_service                13
#define id_css_service                14
#define id_dss_service                15
#define id_ccs_service                16    // command line config service
#define id_shl_service                17    // shell service
#define num_services                  18

// Un-used
#define unused_id                     0

// these are the default node settings

//  Datatype  Units  Notes
// AHRS generated data
#define id_pitch_acceleration  300 //  FLOAT   g forward: + aft: -
#define id_roll_acceleration  301 //  FLOAT   g right: + left: -
#define id_yaw_acceleration  302 //  FLOAT   g up: + down: -
#define id_pitch_rate  303 //  FLOAT   rad/s nose up: + nose down: -
#define id_roll_rate  304 //  FLOAT   rad/s roll right: + roll left: -
#define id_yaw_rate  305 //  FLOAT   rad/s yaw right: + yaw left: -
#define id_roll_angle_magnetic  306 //  FLOAT   rad
#define id_pitch_angle_magnetic  307 //  FLOAT   rad
#define id_yaw_angle_magnetic  308 //  FLOAT   rad
#define id_pitch_angle  311 //  FLOAT   rad nose up: + nose down: -
#define id_roll_angle  312 //  FLOAT   rad roll right: + roll left: -
#define id_yaw_angle  313 //  FLOAT   rad yaw right: + yaw left: -
#define id_altitude_rate  314 //  FLOAT   m/s
#define id_indicated_airspeed  315 //  FLOAT   m/s
#define id_true_airspeed  316 //  FLOAT   m/s
#define id_calibrated_airspeed  317 //  FLOAT   m/s
#define id_angle_of_attack  318 //  SHORT   deg
#define id_qnh  319 //  SHORT   hPa published every 60 seconds. changed with id_qnh_up, id_qnh_dn
#define id_baro_corrected_altitude  320 //  FLOAT   m
#define id_heading_angle  321 //  SHORT   deg 0-360 True heading
#define id_heading  322 //  SHORT   deg 0-360
#define id_pressure_altitude  323 //  FLOAT   m
#define id_outside_air_temperature  324 //  SHORT   K
#define id_differential_pressure  325 //  FLOAT   hPa
#define id_static_pressure  326 //  FLOAT   hPa
#define id_wind_speed  333 //  FLOAT   m/s
#define id_wind_direction  334 //  FLOAT   rad 0-360
#define id_engine_rpm  500 //  SHORT   r/min
#define id_engine_rpm_a  501 //  SHORT   r/min
#define id_exhaust_gas_temperature1  520 //  SHORT   K
#define id_exhaust_gas_temperature2  521 //  SHORT   K
#define id_exhaust_gas_temperature3  522 //  SHORT   K
#define id_exhaust_gas_temperature4  523 //  SHORT   K
#define id_fuel_flow_rate_left  524 //  SHORT   l/hr
#define id_manifold_pressure  528 //  FLOAT   hPa
#define id_manifold_pressure_a  530 //  FLOAT   hpa
#define id_oil_pressure  532 //  FLOAT   hPa
#define id_oil_temperature  536 //  SHORT   K
#define id_cylinder_head_temperature1  548 //  SHORT   K
#define id_cylinder_head_temperature2  549 //  SHORT   K
#define id_cylinder_head_temperature3  550 //  SHORT   K
#define id_cylinder_head_temperature4  551 //  SHORT   K
#define id_engine_status_a  556 //  SHORT 
#define id_engine_rpm_b  564 //  SHORT   r/min
#define id_fuel_flow_rate_a  588 //  SHORT   l/hr
#define id_fuel_flow_rate_b  589 //  SHORT   l/hr
#define id_fuel_flow_rate  590 //  SHORT   l/hr
#define id_fuel_consumed  591 //  SHORT   l Fuel consumed since power on
#define id_manifold_pressure_b  592 //  FLOAT   hPa
#define id_oil_pressure_b  596 //  FLOAT   hPa
#define id_oil_temperature_b  600 //  SHORT   K
#define id_coolant_temp_b  612 //  SHORT 
#define id_cylinder_head_temperature5  612 //  SHORT   K
#define id_cylinder_head_temperature6  613 //  SHORT   K
#define id_cylinder_head_temperature7 614 //  SHORT   K
#define id_cylinder_head_temperature8 615 //  SHORT   K
#define id_engine_status_b  620 //  SHORT 
#define id_egt1_a  628 //  SHORT  K
#define id_egt2_a  630 //  SHORT  K
#define id_egt3_a  632 //  SHORT  K
#define id_egt4_a  634 //  SHORT  K
#define id_intake_temperature_a  640 //  SHORT  K
#define id_outside_air_temp_a  642 //  SHORT  K
#define id_egt1_b  644 //  SHORT   K
#define id_egt2_b  646 //  SHORT  K
#define id_egt3_b  648 //  SHORT  K
#define id_egt4_b  650 //  SHORT   K
#define id_intake_temperature_b  656 //  SHORT  K
#define id_outside_air_temp_b  658 //  SHORT   K
#define id_edu_left_fuel_quantity  666 //  SHORT   l
#define id_edu_right_fuel_quantity  667 //  SHORT   l
#define id_left_fuel_quantity  668 //  SHORT   l
#define id_right_fuel_quantity  669 //  SHORT   l
#define id_pitot_temperature  670 //  SHORT   degrees
#define id_pitot_heat_status  671 //  SHORT   1 = heater on, 0 = off
#define id_pitot_power_status  672 //  SHORT   1 = power on, 0 = off
#define id_fdu_board_temperature  673 //  SHORT   degrees
#define id_set_left_fuel_qty  674 //  SHORT   l
#define id_set_right_fuel_qty  675 //  SHORT   l
#define id_fuel_pressure  684 //  FLOAT   hPa
#define id_fuel_pressure_a  685 //  FLOAT   hPa
#define id_fuel_pressure_b  686 //  FLOAT   hPa
#define id_throttle_position_a  692 //  SHORT 
#define id_ambient_pressure_a  694 //  SHORT 
#define id_throttle_position_b  696 //  SHORT 
#define id_ambient_pressure_b  698 //  SHORT 
#define id_mixture_lever  700 //  SHORT   0-100%
#define id_mixture_lever_a 701  // SHORT  0-100%
#define id_mixture_lever_b 702  // SHORT  0-100%
#define id_aux1_fuel_quantity 710   // SHORT l
#define id_aux2_fuel_quantity 711   // SHORT l
#define id_propeller_speed_lever  800 //  SHORT   rpm
#define id_propeller_motor_current  801 //  SHORT   A
#define id_propeller_low_speed  802 //  SHORT   rpm
#define id_propeller_high_speed  803 //  SHORT   rpm
#define id_dc_voltage  920 //  SHORT   V
#define id_dc_current  930 //  FLOAT   A
#define id_dc_current_a 931 // FLOAT A
#define id_dc_current_b 932 // FLOAT A
#define id_voltage_a  950 //  SHORT 
#define id_voltage_b  954 //  SHORT 
#define id_track  1006 //  FLOAT   radians track over the ground (true)
#define id_deviation  1007 //  FLOAT   m deviation from track in meters
#define id_track_angle_error  1008 //  SHORT   deg
#define id_estimated_time_to_next  1009 //  SHORT   min
#define id_estimated_time_of_arrival  1010 //  SHORT   min
#define id_estimated_enroute_time  1011 //  SHORT   min
#define id_waypoint_identifier_0_3  1012 //  ACHAR4  
#define id_waypoint_identifier_4_7  1013 //  ACHAR4  
#define id_waypoint_identifier_8_11  1014 //  ACHAR4  
#define id_waypoint_identifier_12_15  1015 //  ACHAR4  
#define id_waypoint_type_identifier  1016 //  SHORT  
#define id_waypoint_latitude  1017 //  FLOAT   deg
#define id_waypoint_longitude  1018 //  FLOAT   deg
#define id_waypoint_minimum_altitude  1019 //  FLOAT   m
#define id_waypoint_maximum_altitude  1023 //  FLOAT   m
#define id_distance_to_next  1024 //  FLOAT   m
#define id_distance_to_destination  1025 //  FLOAT   m
#define id_gps_latitude  1036 //  FLOAT   deg
#define id_gps_longitude  1037 //  FLOAT   deg
#define id_gps_height  1038 //  FLOAT  m
#define id_gps_groundspeed  1039 //  FLOAT   m/s
#define id_true_track  1040 //  SHORT   deg
#define id_magnetic_track  1041 //  SHORT   deg
#define id_magnetic_heading  1069 //  SHORT   deg
#define id_position_latitude  1070 //  FLOAT   rad
#define id_position_longitude  1071 //  FLOAT   rad
#define id_position_altitude  1072 //  FLOAT   m true altitud
#define id_velocity_x  1073 //  FLOAT   m/s
#define id_velocity_y  1074 //  FLOAT   m/s
#define id_velocity_z  1075 //  FLOAT   m/s
#define id_imu_mag_x  1076 //  FLOAT   gauss +/-
#define id_imu_mag_y  1077 //  FLOAT   gauss +/-
#define id_imu_mag_z  1078 //  FLOAT   gauss +/-
#define id_imu_accel_x  1079 //  FLOAT   m/s
#define id_imu_accel_y  1080 //  FLOAT   m/s
#define id_imu_accel_z  1081 //  FLOAT   m/s
#define id_imu_gyro_x  1082 //  FLOAT   rad/s
#define id_imu_gyro_y  1083 //  FLOAT   rad/s
#define id_imu_gyro_z  1084 //  FLOAT   rad/s
#define id_magnetic_variation  1121 //  SHORT   deg
#define id_selected_course  1125 //  SHORT   deg
#define id_desired_track_angle  1135 //  FLOAT   rad magnetic angle to fly to get to destination
#define id_nav_command  1136 //  SHORT   0 = clear route, 1 = activate route, 2 = deactivate_route, 3 = invert route
#define id_nav_valid  1137 //  SHORT   0 = nav data not valid, 1 = enroute data valid
#define id_gps_valid  1138 //  SHORT   0 = nav data not valid, 1 = enroute data valid
#define id_set_route_segments  1141 //  SHORT  service code 0=current plan, 1..n specific plan, number of route segments (max 16 for route 0)
#define id_set_waypoint_identifier_0_3  1142 //  ACHAR4 
#define id_set_waypoint_identifier_4_7  1143 //  ACHAR4 
#define id_set_waypoint_identifier_8_11  1144 //  ACHAR4 
#define id_set_waypoint_identifier_12_15  1145 //  ACHAR4 
#define id_set_waypoint_type_identifier  1146 //  SHORT 
#define id_set_waypoint_latitude  1146 //  FLOAT   deg
#define id_set_waypoint_longitude  1148 //  FLOAT   deg
#define id_waypoint_turn_heading  1149 //  SHORT   deg, heading to turn onto
#define id_set_waypoint_minimum_altitude  1149 //  FLOAT   m
#define id_set_waypoint_maximum_altitude  1150 //  FLOAT   m
#define id_trim_up  1160 //  SHORT   number of trim steps
#define id_trim_dn  1161 //  SHORT  
#define id_trim_left  1162 //  SHORT  
#define id_trim_right  1163 //  SHORT  
#define id_autopilot_engage  1170 //  SHORT   0 = off, 1 = on
#define id_autopilot_set_max_roll  1171 //  SHORT   Maximum roll rate allowed
#define id_autopilot_set_vs_rate  1172 //  SHORT   Set maximum pitch allowed
#define id_autopilot_power  1172 //  SHORT   autopilot power is on
#define id_autopilot_alt_mode  1173 //  SHORT   0 = off, 1 = on Altitude hold mode enable
#define id_autopilot_vs_mode  1174 //  SHORT   0 = off, 1 = on Vertical speed hold mode enable
#define id_autopilot_mode  1175 //  SHORT   see enumeration below
#define id_autopilot_status  1176 //  SHORT   bitmask, published every 5secs or on change
#define id_autopilot_altitude  1177 //  SHORT   m assigned altitude
#define id_autopilot_vertical_speed  1178 //  SHORT   m/s vertical rate of climb/descent
#define id_roll_servo_status  1180 // SHORT  
#define id_pitch_servo_status  1181 // SHORT  
#define id_roll_servo_set_position  1182 //  SHORT   degrees position +/-90
#define id_pitch_servo_set_postion  1183 //  SHORT   degrees position +/-90
#define id_imu_valid  1190 //  SHORT   0 = no data, 1 = valid data
#define id_edu_valid  1191 //  SHORT   0 = error, 1 = valid
#define id_set_route  1192 //  SHORT   service code 0=current plan, 1..n specific plan
#define id_aux_battery  1193 //  SHORT   0 = not on battery, 1 = on aux battery
#define id_def_utc  1200 //  CHAR4   format: 13h43min22s 13 43 22 00
#define id_def_date  1201 //  CHAR4   format: 12. June 1987 12 06 19 87
#define id_qnh_up  1202 //  SHORT   can increment by # of steps
#define id_qnh_dn  1203 //  SHORT  
#define id_set_magnetic_variation  1204 //  SHORT   
#define id_heading_up  1206 //  SHORT  
#define id_heading_dn  1207 //  SHORT  
#define id_engine_hours_a  1208 //  SHORT 
#define id_stall_warning  1211 //  SHORT   Stall warning switch activated
#define id_total_time  1212 //  LONG   Total time in service Hrs * 10
#define id_engine_hours_b  1212 //  SHORT 
#define id_air_time  1213 //  LONG   Total time where IAS > 10 kts Hrs * 10
#define id_tach_time  1214 //  LONG   Total time where the engine is running Hrs * 10
#define id_ecu_hours_a  1216 //  SHORT 
#define id_pitch  1217 //  SHORT   deg +/- 180
#define id_ecu_hours_b  1220 //  SHORT 
#define id_navigation_mode  1250 //  SHORT   0 = GPS 1 Navigation, 1 = GPS 2 Navligation, 2 = NAV 1, 3 = NAV 2
#define id_alternator_status  1251 //  SHORT   0 = alternator failed, 1 = alternator ok
#define id_voltage_warning  1252 //  SHORT   0 = voltage ok, 1 = low voltage
#define id_panel_brightness  1253 //  SHORT   0-100 %
#define id_pitot_temperature_alarm  1254 //  SHORT   0 = pitot temperature ok, 1 = temperature low, 2 = temperature high
#define id_nav_msg  1256 //  SHORT   0 = no message, 1 = msg displayed
#define id_nav_appr  1257 //  SHORT   0 = GPS not in approach mode, 1 = GPS in approach mode
#define id_nav_ils  1258 //  SHORT   0 = NAV receiver does not have ILD, 1 = NAV receiver has ILS detected
#define id_voltage_sense_1  1259 //  FLOAT   0-16v
#define id_voltage_sense_2  1260 //  FLOAT   0-16v
#define id_sensor_status_1_a  1500 //  SHORT 
#define id_sensor_status_2_a  1504 //  SHORT 
#define id_device_status_1_a  1508 //  SHORT 
#define id_device_status_2_a  1512 //  SHORT 
#define id_sensor_status_1_b  1516 //  SHORT 
#define id_sensor_status_2_b  1520 //  SHORT 
#define id_device_status_1_b  1524 //  SHORT 
#define id_device_status_2_b  1528 //  SHORT 
#define id_timing_divergence  2200 //  SHORT   msec difference between left & right tach
#define id_left_mag_rpm  2201 // SHORT  
#define id_right_mag_rpm  2202 // SHORT  
#define id_left_mag_adv  2203 // SHORT  
#define id_right_mag_adv  2204 // SHORT  
#define id_left_mag_map  2205 // SHORT  
#define id_right_mag_map  2206 // SHORT  
#define id_left_mag_volt  2207 // SHORT  
#define id_right_mag_volt  2208 // SHORT  
#define id_left_mag_temp  2209 // SHORT  
#define id_right_mag_temp  2210 // SHORT  
#define id_left_mag_coil1  2211 // SHORT  
#define id_right_mag_coil1  2212 // SHORT  
#define id_left_mag_coil2  2213 // SHORT  
#define id_right_mag_coil2  2214 // SHORT  

  extern const char *get_name_from_id(uint16_t id);
  extern uint16_t get_id_from_name(const char *name);
  extern const char *get_datatype_name(uint8_t data_type);

  // status bits for autopilot
#define APSTATUSMODEMASK    0x0007          // mask bits
#define APMODENONE          0x0000          // no mode engaged, ap standby
#define APMODEHOLD          0x0001          // hold last heading, wings level
#define APMODEGPS           0x0002          // navigation by GPS
#define APMODENAV           0x0003          // navigation mode by NAV Rcvr
#define APMODEHDGHOLD       0x0004          // naviagtion by heading set.
#define APSTATUSENGAGED     0x0008          // autopilot engaged
#define APSTATUSALTMODE     0x0010          // autopilot altitude hold engaged
#define APSTATUSVSMODE      0x0020          // autopilot vertical mode enaged
#define APSTATUSAPRMODE     0x0040          // approach mode
#define APSTATUSGPSARM      0x0080          // GPS is active and available
#define APSTATISNAVARM      0x0100          // NAV receiver is active


// this is the Flight plan service.  It supports up to 255 routes.
// Route 0 is the current route
// Generated by the Flight Director/PC when a flight plan is loaded
//
// id_set_route commands
//
// 0 = clear route
// 1 = activate route         copies route to route 0, then must send activate route 0
// 2 = invert route


#define id_user_defined_start             1800
#define id_user_defined_end               1899


  // message sent by a node with a uchar4 format
  //
  // 0 .. node_id
  // 1 .. unit type
  // 2 .. hardware_revision     high/low nibble <major>.<minor>
  // 3 .. software_revision     high/low nibble <major>.<minor>
#define id_node_register                  2032

// Defined hardware_types
#define unit_ahrs                     1
#define unit_edu                      2
#define unit_pi                       3
#define unit_mfd                      4
#define unit_usb                      5
#define unit_scan                     6
#define unit_efi                      7

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

#define CANAS_DATATYPE_NODATA 0
#define CANAS_DATATYPE_ERROR 1
#define CANAS_DATATYPE_FLOAT 2
#define CANAS_DATATYPE_INT32 3
#define CANAS_DATATYPE_UINT32 4
//#define CANAS_DATATYPE_BLONG 5
#define CANAS_DATATYPE_SHORT 6
#define CANAS_DATATYPE_USHORT 7
//#define CANAS_DATATYPE_BSHORT 8
#define CANAS_DATATYPE_CHAR 9
#define CANAS_DATATYPE_UCHAR 10
//#define CANAS_DATATYPE_BCHAR 11
#define CANAS_DATATYPE_SHORT2 12
#define CANAS_DATATYPE_USHORT2 13
//#define CANAS_DATATYPE_BSHORT2 14
#define CANAS_DATATYPE_CHAR4 15
#define CANAS_DATATYPE_UCHAR4 16
//#define CANAS_DATATYPE_BCHAR4 17
#define CANAS_DATATYPE_CHAR2 18
#define CANAS_DATATYPE_UCHAR2 19
//#define CANAS_DATATYPE_BCHAR2 20
//#define CANAS_DATATYPE_MEMID 21
//#define CANAS_DATATYPE_CHKSUM 22
//#define CANAS_DATATYPE_ACHAR 23
//#define CANAS_DATATYPE_ACHAR2 24
//#define CANAS_DATATYPE_ACHAR4 25
#define CANAS_DATATYPE_CHAR3 26
#define CANAS_DATATYPE_UCHAR3 27
//#define CANAS_DATATYPE_BCHAR3 28
//#define CANAS_DATATYPE_ACHAR3 29
//#define CANAS_DATATYPE_DOUBLEH 30
//#define CANAS_DATATYPE_DOUBLEL 31
#define CANAS_DATATYPE_RESVD_BEGIN 32
#define CANAS_DATATYPE_RESVD_END 99
#define CANAS_DATATYPE_UDEF_BEGIN 100
#define CANAS_DATATYPE_UDEF_END 255

/**
 * @struct canas_msg_t
 * CanFly low level message
 * @param node_id Node if of sender 1..255
 * @param data_type Can Aerospace data type
 * @param service_code Message dependent service code
 * @param message_code Message dependent message code
 * @param data message data
 */
  typedef struct _canas_msg_t
    {
    uint8_t node_id;
    uint8_t data_type;              // is canaerospace_data_type or custom
    uint8_t service_code;
    uint8_t message_code;           // incremented as each message is published
    uint8_t data[4];
    } canas_msg_t;

  /**
   * @struct canmsg_t
   * This is the message that is passed around the CanFly infrastructure
   * defined by Neutron
   * @param flags	Length, reply, ID
   * @param canas Encoded canas_msg_t
   */
  typedef struct
    {
    // Bits 15:12 - Length
    // Bit 11 - Reply
    // bits 10-0 ID
    uint16_t flags;

    // following is 8 bytes
    union {
      canas_msg_t canas;
      uint8_t raw[8];
      };
    } canmsg_t;

#define LENGTH_MASK 0xF000
#define RTR_MASK 0x0800
#define ID_MASK 0x07FF

  static inline void set_can_len(canmsg_t *msg, uint16_t len)
    {
    msg->flags &= ~LENGTH_MASK;
    msg->flags |= (len << 12) & LENGTH_MASK;
    }

  static inline void set_can_reply(canmsg_t *msg, bool is_reply)
    {
    msg->flags &= ~RTR_MASK;
    msg->flags |= is_reply ? RTR_MASK : 0;
    }

  static inline void set_can_id(canmsg_t *msg, uint16_t id)
    {
    msg->flags &= ~ID_MASK;
    msg->flags |= (id & ID_MASK);
    }

  static inline uint16_t get_can_len(const canmsg_t *msg)
    {
    return (msg->flags & LENGTH_MASK) >> 12;
    }

  static inline bool get_can_reply(const canmsg_t *msg)
    {
    return (msg->flags & RTR_MASK) != 0;
    }

  static inline uint16_t get_can_id(const canmsg_t *msg)
    {
    return msg->flags & ID_MASK;
    }

  /**
   * @struct lla_t
   * Lat/Lng altitude parameter type
   * @param lat Lattitude
   * @param lng Longitude
   * @param alt Altitude
   */
  typedef struct _lla_t {
    float lat;
    float lng;
    float alt;
    } lla_t;

  /**
   * @struct xyz_t
   * Generic Euclidean point
   * @param x X
   * @param y Y
   * @param z Z
   */
  typedef struct _xyz_t {
    float x;
    float y;
    float z;
    } xyz_t;

  /** @struct qtn_t
   * Quarternion type
   * @param q0 Q0
   * @param q1 Q1
   * @param q2 Q2
   */
  typedef struct _qtn_t {
    float q0;
    float q1;
    float q2;
    float q3;
    } qtn_t;

  /** @struct matrix_t
   * Generic matrix parameter
   * @param v Values
   */
  typedef struct _matrix_t {
    float v[3][3];
    } matrix_t;

  extern const matrix_t identity_matrix;

  extern const xyz_t *matrix_dot(const matrix_t *m, const xyz_t *v, xyz_t *r);
  extern const matrix_t *matrix_add(const matrix_t *m, const matrix_t *v, matrix_t *r);
  extern const matrix_t *matrix_subtract(const matrix_t *m, const matrix_t *v, matrix_t *r);
  extern const matrix_t *matrix_multiply(const matrix_t *m, const matrix_t *v, matrix_t *r);
  extern const matrix_t *matrix_divide(const matrix_t *m, const matrix_t *v, matrix_t *r);
  extern const matrix_t *matrix_transpose(const matrix_t *m, matrix_t *r);
  extern const matrix_t *matrix_exponetiation(const matrix_t *m, matrix_t *r);
  extern const matrix_t *matrix_copy(const matrix_t *m, matrix_t *r);

  /**
   * @function create_can_msg_int16(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int16_t data)
   * Create a can message sending a 16bit integer
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int16(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int16_t data);
  /**
   * @function create_can_msg_int16_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int16_t data1, int16_t data2)
   * Create a can message sending two 16bit integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int16_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int16_t data1, int16_t data2);
  /**
   * @function create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint16_t data)
   * Create a can message sending a 16bit unsigned integer
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint16(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint16_t data);
  /**
   * @function create_can_msg_uint16_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint16_t data1, uint16_t data2)
   * Create a can message sending two unsigned 16bit integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint16_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint16_t data1, uint16_t data2);
  /**
   * @function create_can_msg_int32(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int32_t data)
   * Create a can message sending a 32bit integer
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int32(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int32_t data);
  /**
   * @function create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint32_t data)
   * Create a can message sending a 32bit unsigned integer
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint32(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint32_t data);
  /**
   * @function create_can_msg_float(canmsg_t *msg, uint16_t message_id, uint8_t service_code, float data)
   * Create a can message sending a floating point value
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_float(canmsg_t *msg, uint16_t message_id, uint8_t service_code, float data);
  /**
   * @function create_can_msg_int8(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data)
   * Create a can message sending an 8bit integer
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int8(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data);
  /**
   * @function create_can_msg_int8_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data1, int8_t data2)
   * Create a can message sending two 8bit integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int8_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data1, int8_t data2);
  /**
   * @function create_can_msg_int8_3(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data1, int8_t data2, int8_t data3)
   * Create a can message sending three 8bit integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @param data3          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int8_3(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data1, int8_t data2, int8_t data3);
  /**
   * @function create_can_msg_int8_4(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data1, int8_t data2, int8_t data3, int8_t data4)
   * Create a can message sending four 8bit integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @param data3          Data to send
   * @param data4          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_int8_4(canmsg_t *msg, uint16_t message_id, uint8_t service_code, int8_t data1, int8_t data2, int8_t data3, int8_t data4);
  /**
   * @function create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data)
   * Create a can message sending an 8bit unsigned integer
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint8(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data);
  /**
   * @function create_can_msg_uint8_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data1, uint8_t data2)
   * Create a can message sending two 8bit unsigned integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint8_2(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data1, uint8_t data2);
  /**
   * @function create_can_msg_uint8_3(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data1, uint8_t data2, uint8_t data3)
   *  Create a can message sending three 8bit unsigned integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @param data3          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint8_3(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data1, uint8_t data2, uint8_t data3);
  /**
   * @function create_can_msg_uint8_4(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
   * Create a can message sending four 8bit unsigned integers
   * @param msg           Message to construct
   * @param message_id    11 bit CANAerspace ID
   * @param service_code  Message service code
   * @param data1          Data to send
   * @param data2          Data to send
   * @param data3          Data to send
   * @param data4          Data to send
   * @return pointer to message that is constructed.
   */
  extern canmsg_t *create_can_msg_uint8_4(canmsg_t *msg, uint16_t message_id, uint8_t service_code, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);

  /**
   * @function get_param_float(const canmsg_t *msg, float *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to a float, and the data is returned
   */
  extern result_t get_param_float(const canmsg_t *msg, float *value);
  /**
   * @function get_param_int8(const canmsg_t *msg, int8_t *value)
   * @param msg         Message to extract parameter from
   * @param index       Index of the value (0..3)
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int8, and the data is returned
   */
  extern result_t get_param_int8(const canmsg_t *msg, uint16_t index, int8_t *value);
  /**
   * @function get_param_uint8(const canmsg_t *msg, uint8_t *value)
   * @param msg         Message to extract parameter from
   * @param index       Index of the value (0..3)
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an uint8, and the data is returned
   */
  extern result_t get_param_uint8(const canmsg_t *msg, uint16_t index, uint8_t *value);
  /**
   * @function get_param_int16(const canmsg_t *msg, int16_t *value)
   * @param msg         Message to extract parameter from
   * @param index       Index of the value (0..1)
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int16, and the data is returned
   */
  extern result_t get_param_int16(const canmsg_t *msg, uint16_t index, int16_t *value);
  /**
   * @function get_param_uint16(const canmsg_t *msg, uint16_t *value)
   * @param msg         Message to extract parameter from
   * @param index       Index of the value (0..1)
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int, and the data is returned
   */
  extern result_t get_param_uint16(const canmsg_t *msg, uint16_t index, uint16_t *value);
  /**
   * @function get_param_int32(const canmsg_t *msg, int32_t *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an int32, and the data is returned
   */
  extern result_t get_param_int32(const canmsg_t *msg, int32_t *value);
  /**
   * @function get_param_uint32(const canmsg_t *msg, uint32_t *value)
   * @param msg         Message to extract parameter from
   * @param value       extracted value
   * @return s_ok if the message can be coerced to an uint32, and the data is returned
   */
  extern result_t get_param_uint32(const canmsg_t *msg, uint32_t *value);

#ifdef min
#undef min
#endif
#define min(a, b) ((a) < (b) ? (a) : b)


#ifdef max
#undef max
#endif
#define max(a, b) ((a) > (b) ? (a) : b)


  // this is the node ID that is normally defined in the registry
  extern uint8_t node_id;
  /**
   * @function can_send(canmsg_t *)
   * queue a can message to be sent.  Will update node_id and message_code to correct values
   * @param message to send
   * @return result_t s_ok if sent OK.
   */
  extern result_t can_send(canmsg_t *);
  /**
   * @function can_send(canmsg_t *)
   * queue a can message to be sent.  Does not set node_id, message_code or service_code
   * @param message to send
   * @return result_t s_ok if sent OK.
   */
  extern result_t can_send_raw(canmsg_t *);
  /**
   * @function can_send(canmsg_t *)
   * queue a can message to be sent. Will only set the node_id
   * @param message to send
   * @return result_t s_ok if sent OK.
   */
  extern result_t can_send_reply(canmsg_t *);

  /**
   * @function bool (*msg_hook_fn)(const canmsg_t *msg, void *parg)
   * @param msg   Can message to process
   * @param parg  Argument used when hook function registered
   * @return true if the message was handled.  Only used when a service callback
   * is used.
   */

  typedef bool (*msg_hook_fn)(const canmsg_t *, void *parg);

  /**
   * @struct msg_hook_t
   * @param next    next hook handler, or 0
   * @param prev    previous hook handler, or 0
   * @param callback  Function to be called when the service is called
   */
  typedef struct _msg_hook_t
    {
    struct _msg_hook_t *next;
    struct _msg_hook_t *prev;
    msg_hook_fn callback;
    void *parg;
    } msg_hook_t;

  /**
   * @function register_service(uint8_t service, msg_hook_t *handler, void *parg)
   * Register a service handler
   * @param service   service ID, cannot be 0
   * @param handler   handler
   * @return s_ok if the handler installed ok
   */
  extern result_t register_service(uint8_t service, msg_hook_t *handler);
  /**
   * @function subscribe(msg_hook_t *handler)
   * register a call-back function to handle messages.
   * @param handler Handler to add to the hook chain
   * @return s_ok if the handler can be added
   */
  extern result_t subscribe(msg_hook_t *handler);
  /**
   * @function unsubscribe(msg_hook_t *handler)
   * Remove the msg hook function
   * @param handler Handler to remove from the hook chain
   * @return s_ok if the handler can be removed
   */
  extern result_t unsubscribe(msg_hook_t *handler);
  /**
   * @function publish_float(uint16_t id, float value)
   * Update the datapoint with a float value
   * @param id        Can ID to publish
   * @param value     value to publish
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_float(uint16_t id, float value);
  /**
   * @function publish_int8(uint16_t id, const int8_t *value, uint16_t len)
   * Update the datapoint with a series of 8 bit integers
   * @param id        Can ID to publish
   * @param value     values to publish
   * @param len       number to publish (1..4)
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_int8(uint16_t id, const int8_t *value, uint16_t len);
  /**
   * @function publish_uint8(uint16_t id, const uint8_t *value, uint16_t len)
   * Update the datapoint with a series of 8 bit unsigned integers
   * @param id        Can ID to publish
   * @param value     values to publish
   * @param len       number to publish (1..4)
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_uint8(uint16_t id, const uint8_t *value, uint16_t len);
  /**
   * @function publish_int16(uint16_t id, const int16_t *value, uint16_t len)
   * Update the datapoint with a series of 16 bit integers
   * @param id        Can ID to publish
   * @param value     values to publish
   * @param len       number to publish (1..2)
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_int16(uint16_t id, const int16_t *value, uint16_t len);
  /**
   * @function publish_uint16(uint16_t id, const uint16_t *value, uint16_t len)
   * Update the datapoint with a series of 16 bit unsigned integers
   * @param id        Can ID to publish
   * @param value     values to publish
   * @param len       number to publish (1..2)
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_uint16(uint16_t id, const uint16_t *value, uint16_t len);
  /**
   * @function publish_int32(uint16_t id, int32_t value)
   * Update the datapoint with a 32bit value
   * @param id        Can ID to publish
   * @param value     values to publish
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_int32(uint16_t id, int32_t value);
  /**
   * @function publish_uint32(uint16_t id, uint32_t value)
   * Update the datapoint with a 32bit value
   * @param id        Can ID to publish
   * @param value     values to publish
   * @return  s_ok if the datapoint is registered with the publisher
   */
  extern result_t publish_uint32(uint16_t id, uint32_t value);
  /**
   * @function get_datapoint_int8(uint16_t id, int8_t *value, uint16_t *len)
   * Return the value of a datapoint as a int8_t
   * @param id      The id to publish
   * @param value   The value to get
   * @param len     Size of value array when called, returned with actual number read
   * @return s_ok if the datapoint is found, e_not_found if not.
   */
  extern result_t lookup_int8(uint16_t id, int8_t *value, uint16_t *len);
  /**
   * @function get_datapoint_uint8(uint16_t id, uint8_t *value, uint16_t *len)
  * Return the value of a datapoint as a uint8_t
  * @param id      The id to publish
  * @param value   The value to get
  * @param len     Size of value array when called, returned with actual number read
  * @return s_ok if the datapoint is found, e_not_found if not.
  */
  extern result_t lookup_uint8(uint16_t id, uint8_t *value, uint16_t *len);
  /**
   * @function get_datapoint_int16(uint16_t id, int16_t *value, uint16_t *len)
   * Return the value of a datapoint as a int16_t
   * @param id      The id to publish
   * @param value   The value to get
   * @param len     Size of value array when called, returned with actual number read
   * @return s_ok if the datapoint is found, e_not_found if not.
   */
  extern result_t lookup_int16(uint16_t id, int16_t *value, uint16_t *len);
  /**
   * @function get_datapoint_uint16(uint16_t id, uint16_t *value, uint16_t *len)
  * Return the value of a datapoint as a uint16_t
  * @param id      The id to publish
  * @param value   The value to get
  * @param len     Size of value array when called, returned with actual number read
  * @return s_ok if the datapoint is found, e_not_found if not.
  */
  extern result_t lookup_uint16(uint16_t id, uint16_t *value, uint16_t *len);
  /**
   * @function get_datapoint_int32(uint16_t id, int32_t *value)
   * Return the value of a datapoint as a int32_t
   * @param id      The id to publish
   * @param value   The value to get
   * @return s_ok if the datapoint is found, e_not_found if not.
   */
  extern result_t lookup_int32(uint16_t id, int32_t *value);
  /**
   * @function get_datapoint_uint32(uint16_t id, uint32_t *value)
   * Return the value of a datapoint as a uint32_t
   * @param id      The id to publish
   * @param value   The value to get
   * @return s_ok if the datapoint is found, e_not_found if not.
   */
  extern result_t lookup_uint32(uint16_t id, uint32_t *value);
  /**
   * @function get_datapoint_float(uint16_t id, float *value)
   * Return the value of a datapoint as a float
   * @param id      The id to publish
   * @param value   The value to get
   * @return s_ok if the datapoint is found, e_not_found if not.
   */
  extern result_t lookup_float(uint16_t id, float *value);

  typedef enum _filter_type_e
    {
    ft_none,
    ft_boxcar,
    ft_iir,
    ft_fir
    } filter_type_e;

  /**
   * @function define_datapoint(uint16_t can_id, uint16_t rate, uint16_t type, uint16_t boxcar_length, bool loopback, bool publish)
   * Utility function to create a published datapoint.  Primarily used by embedded devices to quickly set
   * up the registry to publish simple data.
   * @param can_id      Id to publish.
   * @param rate        Milli-seconds between published data
   * @param filter_type type of filter
   * @param filter_length Optional (if > 0) simple boxcar filter length.  Allows for simple filtering.
   * @param loopback    True if the datapoint is also published internally
   * @param publish     True if the datapoint is published to the can-bus
   * @return s_ok if the datapoint is published.
   * @remark If the datapoint can be created, the publisher is notified immediately of the new datapoint
   */
  extern result_t publish_datapoint(uint16_t can_id, uint16_t rate,
    filter_type_e type, uint16_t boxcar_length, bool loopback, bool publish);

  typedef struct _publish_setup_t {
    uint16_t can_id;
    uint16_t rate;
    filter_type_e filter_type;
    uint16_t filter_length;
    bool loopback;
    bool publish;
    } publish_setup_t;

  /**
   * Setup a set of published datapoints
   * @param values list of publishing defaults, can_id = 0 ends the list
   * @return s_ok if setup ok.
   */
  extern result_t setup_publisher(const publish_setup_t *values);

  /**
   * @function Monitor the CAN bus for a published datapoint
   * @param can_id  Id to monitor
   * @return s_ok if enough resources to monitor the datapoint
   */
  extern result_t monitor_datapoint(uint16_t can_id);

#define numelements(a) (sizeof(a) / sizeof(a[0]))

#define NO_WAIT 0
#define INDEFINITE_WAIT 0xFFFFFFFF


  ////////////////////////////////////////////////////////////////////////////////
  //
  // Memory allocation routines.  Do not use malloc/free which are not thread safe
  //
  /**
   * @function neutron_malloc(size_t size)
   * Allocate a block of memory, thread-safe
   * @param size  size to allocate
   * @return pointer to memory, 0 if failed
   */
  extern void *neutron_malloc(size_t size);
  /**
   * @function void *neutron_calloc(size_t count, size_t size)
   * Allocate a block of memory and initialize to 0's, thread-safe
   * @param count count of blocks to allocate
   * @param size  size of block to allocate
   * @return pointer to memory, 0 if failed
   */
  extern void *neutron_calloc(size_t count, size_t size);
  /**
   * @function  neutron_free(void *mem)
   * Free a kernel allocated block of memory, thread-safe
   * @param mem memory to free
   */
  extern void neutron_free(void *mem);
  /**
   * @function neutron_realloc(void *mem, size_t new_size)
   * Resize the buffer to be new_size, thread-safe
   * @param mem       pointer to the existing buffer
   * @param new_size  new size of the buffer
   * @return pointer to the new buffer
   */
  extern void *neutron_realloc(void *mem, size_t new_size);
  /**
   * @function neutron_strdup(const char *str)
   * Thread safe string duplicate
   * @param str string to copy
   * @return the duplicated string, can be freed with neutron_free
   */
  extern char *neutron_strdup(const char *str);

  struct _semaphore_t;
  typedef struct _semaphore_t *semaphore_p;
  typedef uint16_t memid_t;

  typedef void (*task_callback)(void *parg);
  /**
   * @function emaphore_create(semaphore_p *semp)
   * Create a new semaphore
   * @param semp newly constructed semaphore
   * @return s_ok if resources available to create the semaphore
   */
  extern result_t semaphore_create(semaphore_p *semp);
  /**
   * @function semaphore_close(semaphore_p hndl)
   * Close a semaphore and release all resources
   * @param hndl semaphore to close
   * @return s_ok if all closed
   */
  extern result_t semaphore_close(semaphore_p hndl);
  /**
   * @function semaphore_signal(semaphore_p semaphore)
   * signal that an event is set.  Should be called from an interrupt service
   * routine.
   * @param event_mask
   * @return s_ok if signaled ok
   */
  extern result_t semaphore_signal(semaphore_p semaphore);
  /**
   * @function semaphore_wait(semaphore_p semaphore, uint32_t ticks)
   * Suspend the calling task, waiting for an event
   * @param event_mask  event to wait on.
   * @returns s_ok if the event was signaled, s_false if timeout
   */
  extern result_t semaphore_wait(semaphore_p semaphore, uint32_t ticks);

  ////////////////////////////////////////////////////////////////////////////////
  //
  //  Task functions

#define IDLE_PRIORITY 0
#define BELOW_NORMAL 4
#define NORMAL_PRIORITY 8
#define HIGH_PRIORITY 12

#define IDLE_STACK_SIZE 512
#define DEFAULT_STACK_SIZE 2048

  struct _task_t;
  typedef struct _task_t *task_p;

  /**
   * @function task_create(const char *name, uint16_t stack_size, task_callback callback, void *parg, uint8_t priority, task_p *task)
   * Create a new scheduled task
   * @param name          name of the task
   * @param stack_size    size of stack to allocate (words)
   * @param callback      task main routine
   * @param parg          argument to pass to task
   * @param priority      initial priority
   * @param task          created task handle
   * @return s_ok if task created ok
   */
  extern result_t task_create(const char *name,
    uint16_t stack_size,
    task_callback callback,
    void *parg,
    uint8_t priority,
    task_p *task);

  /**
   * @function get_current_task()
   * return the handle to the currently executing task
   * @return task handle
   */
  extern task_p get_current_task();
  /**
   * @function set_task_priority(task_p task, uint8_t priority)
   * Change the priority of a scheduled task
   * @param task        task to change
   * @param priority    priority to set
   * @return s_ok if priority set
   */
  extern result_t set_task_priority(task_p task, uint8_t priority);
  /**
   * @function get_task_priority(task_p task, uint8_t *priority)
   * Return the priority for a task
   * @param task        task to query
   * @param priority    assigned priority
   * @return s_ok if task is valid
   */
  extern result_t get_task_priority(task_p task, uint8_t *priority);
  /**
   * @function task_suspend(task_p task)
   * Suspend a task
   * @param task
   * @return s_ok if suspended
   */
  extern result_t task_suspend(task_p task);
  /**
   * @function task_resume(task_p task)
   * Resume a suspended task
   * @param task  task to resume
   * @return s_ok if task resumed ok
   */
  extern result_t task_resume(task_p task);
  /**
   * @function task_sleep(uint32_t n)
   * Suspend the current task
   * @param n milliseconds to suspend
   * @return s_ok after delay
   */
  extern result_t task_sleep(uint32_t n);
  /**
   * @function close_task(task_p h)
   * Close a task
   * @param h   Handle to the task
   * @return s_ok if task is closed
   */
  extern result_t close_task(task_p h);
  /**
   * @function ticks()
   * return the 1khz tick counter
   * @return
   */
  extern uint32_t ticks();
  /**
   * @function yield()
   * This is used to force a context switch.
  */
  extern void yield();

  typedef struct _tm_t {
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
    uint16_t milliseconds;
    } tm_t;

  /**
  * @function now(tm_t *tm)
  * This function returns the current time based on a gps time fix.
  * @param tm   Where to receive the time.
  * @return s_ok if the time is known.
  * @remark This function will return e_unexpected if the CanFly messages with the
  * utc date and utc time have not been received.
  */
  extern result_t now(tm_t *tm);
  /**
  * @function gmtime(const tm_t *tm, uint32_t *time)
  * convert a tm to a 32bit time format
  * @param tm the time to covert, if 0 then the system time is used
  * @param time resulting time
  * @return s_ok if the tm is valid (times greater than 1970-01-01)
  * @remark this differs from the unix time which is signed
  */
  extern result_t gmtime(const tm_t *tm, uint32_t *time);
  /**
  * @function gmtime(const tm_t *tm, uint64_t *time)
  * convert a tm to a 64 bit time format where is 10^-6 seconds
  * @param tm the time to covert, if 0 then the system time is used
  * @param time resulting time
  * @return s_ok if the tm is valid (times greater than 1970-01-01)
  */
  extern result_t gmtime_ns(const tm_t *tm, uint64_t *time);
  /**
  * @function settime(const tm_t *tm)
  * set the time of the clock.  If a canfly time or date message is received it will update this
  * @param tm values to assign
  * @return s_ok if a valid time
  */
  extern result_t settime(const tm_t *tm);
  /**
  * @function settime_ns(uint64_t time)
  * set the time of the clock based on the ns since 1970-01-01
  * @param time  time in ns
  * @return s_ok if set ok
  */
  extern result_t settime_ns(uint64_t time);
  /**
  * @function rtc_hook()
  * Update the real-time clock
  * @remark if more frequent time updates are required, other than the gps position
  * fix time update the framework should call this every second, or as often as the
  * current_time should be updated.  The exact tick time will be used to increment the real-time clock
  */
  extern void rtc_hook();

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Vector functions

  struct _vector_t;
  typedef struct _vector_t *vector_p;

  /**
   * @function vector_create(uint16_t element_size, vector_p *hndl)
   * Create a dynamic vector
   * @param element_size  size of the elements
   * @param length        number of elements
   * @param vector        resulting vector
   * @return s_ok if vector created ok
   */
  extern result_t vector_create(uint16_t element_size, vector_p *hndl);
  /**
   * @function vector_copy(uint16_t element_size, uint16_t length, const void *elements, vector_p *hndl)
   * Create a new vector and copy the elements into it
   * @param element_size  size of the element
   * @param length        number of elements
   * @param elements      elements to create vector with
   * @param hndl          resulting vector
   * @return s_ok if created ok
   */
  extern result_t vector_copy(uint16_t element_size, uint16_t length, const void *elements, vector_p *hndl);
  /**
   * @function vector_close(vector_p hndl)
   * Release a vector
   * @param hndl vector to release
   * @return s_ok if released ok
   */
  extern result_t vector_close(vector_p hndl);
  typedef void (*incarnate_element_fn)(void *element, void *parg);
  /**
   * @function vector_expand(vector_p vector, uint16_t size, incarnate_element_fn callback, void *parg)
   * Expand the vector to a minimum size and call-back to construct each element
   * @param vector      vector to expand
   * @param size        number of
   * @param callback    constructor callback, if 0 then element filled with 0
   * @param parg        optional argument
   * @return s_ok if expanded ok
   */
  extern result_t vector_expand(vector_p vector, uint16_t size, incarnate_element_fn callback, void *parg);
  //
  typedef int(*compare_element_fn)(const void *left, const void *right);
  typedef void(*swap_fn)(void *left, void *right);
  /**
   * @function vector_compare(vector_p vector, compare_element_fn comp, swap_fn swap)
   * Sort the vector using the passed in compare function
   * @param vector        Handle to the vector
   * @param comp          Comparison function
   * @param swap          Swap elements function
   * @return s_ok if a valid vector
   */
  extern result_t vector_sort(vector_p vector, compare_element_fn comp, swap_fn swap);
  /**
   * @function vector_push_back(vector_p hndl, const void *element)
   * Push an element to the back of the vector
   * @param hndl    Vector to append to
   * @param element element to append
   * @return s_ok if appended ok
   */
  extern result_t vector_push_back(vector_p hndl, const void *element);
  /**
   * @function vector_pop_back(vector_p hndl, void *element)
   * Return the last element from the vector and remove it
   * @param hndl      Vector to change
   * @param element   Element removed
   * @return s_ok if there is an element available
   */
  extern result_t vector_pop_back(vector_p hndl, void *element);
  /**
   * @function vector_count(vector_p hndl, uint16_t *value)
   * Return the number of elements in the vector
   * @param hndl    Vector to query
   * @param value   Number of elements
   * @return s_ok if a valid vector
   */
  extern result_t vector_count(vector_p hndl, uint16_t *value);
  /**
   * @function vector_at(vector_p hndl, uint16_t pos, void *element)
   * Retrieve an element at a position
   * @param hndl      Vector to query
   * @param pos       Position to query
   * @param element   Copy of element at a position
   * @return s_ok if a valid position and the element was copied
   */
  extern result_t vector_at(vector_p hndl, uint16_t pos, void *element);
  /**
   * @function vector_set(vector_p hndl, uint16_t pos, const void *element)
   * Set an element at a position
   * @param hndl      Vector to query
   * @param pos       Position to query
   * @param element   Element at a position
   * @return s_ok if a valid position and the element was copied
   */
  extern result_t vector_set(vector_p hndl, uint16_t pos, const void *element);
  /**
   * @function vector_begin(vector_p hndl, void **it)
   * Return a pointer to the start of the vector
   * @param hndl      Vector to query
   * @param it        Pointer to the start of the vector storage
   * @return s_ok if the vaector is valid
   */
  extern result_t vector_begin(vector_p hndl, void **it);
  /**
   * @function vector_end(vector_p hndl, void **it)
   * Return a pointer to the end of the vector +1 i.e. vector_begin[vector_count]
   * @param hndl      Vector to query
   * @param it        Pointer to the start of the vector storage
   * @return s_ok if the vaector is valid
   */
  extern result_t vector_end(vector_p hndl, void **it);
  /**
   * @function vector_empty(vector_p hndl)
   * Return s_ok if the vector is empty (0 elements)
   * @param hndl  Handle to the vector
   * @return s_ok if empty, s_false if not
   */
  extern result_t vector_empty(vector_p hndl);
  /**
   * @function vector_clear(vector_p hndl)
   * Remove all elements from a vector
   * @param hndl  Vector to clear
   * @return s_ok if a valid vector
   */
  extern result_t vector_clear(vector_p hndl);
  /**
   * @function vector_insert(vector_p hndl, uint16_t at, const void *element)
   * Insert an element into a vector
   * @param hndl      Vector to change
   * @param at        position in the vector, must be less than vector_count
   * @param element   element to copy into the vector
   * @return s_ok if copied ok
   */
  extern result_t vector_insert(vector_p hndl, uint16_t at, const void *element);
  /**
   * @function vector_assign(vector_p hndl, uint16_t len, const void *elements)
   * Assign content as the vector
   * @param hndl      Handle to a created contained
   * @param len       Number of elements to assign
   * @param elements  Elements
   * @return s_ok if assigned ok
   */
  extern result_t vector_assign(vector_p hndl, uint16_t len, const void *elements);
  /**
   * @function vector_erase(vector_p hndl, uint16_t at)
   * Remove an element from a vector
   * @param hndl  Vector to change
   * @param at    Element to remove
   * @return s_ok if removed ok
   */
  extern result_t vector_erase(vector_p hndl, uint16_t at);
  /**
   * @function vector_append(vector_p hndl, uint16_t length, const void *elements)
   * Append the elements to a vector
   * @param hndl      Vector to change
   * @param length    Number of elements to append
   * @param elements  Elements to add to the vector
   * @return s_ok if elements appended ok
   */
  extern result_t vector_append(vector_p hndl, uint16_t length, const void *elements);
  /**
  * @function vector_truncate(vector_p hndl, uint16_t length)
  * set the length of the vector to the requested length
  * @param hndl      Vector to change
  * @param length    Number of elements to append
  * @return s_ok if vector truncated to size, if the size of the vector is less
  * than the length then e_invalid_argument is returned
  */
  extern result_t vector_truncate(vector_p hndl, uint16_t length);

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Map functions name-value pairs
  //
  typedef void (*dup_fn)(const void *src, void **dst);
  typedef int (*compare_key_fn)(const void *left, const void *right);
  typedef void (*destroy_key_fn)(void *key);
  typedef void (*destroy_value_fn)(void *);

  struct _map_t;
  typedef struct _map_t *map_p;

  /**
   * @function map_create(dup_fn copy_key, dup_fn copy_value, compare_key_fn comp_fn, destroy_key_fn destroy_key, destroy_value_fn destroy_value, map_p *handle)
   * Create a map of key:value pairs
   * @param comp_fn       Compare two keys
   * @param destroy_key   Destroy a key
   * @param destroy_value Destroy a value
   * @param handle        created map
   * @return s_ok if created ok
   */
  extern result_t map_create(dup_fn copy_key,
    dup_fn copy_value,
    compare_key_fn comp_fn,
    destroy_key_fn destroy_key,
    destroy_value_fn destroy_value,
    map_p *handle);
  /**
   * @function map_create_nv(dup_fn copy_value, destroy_value_fn destroy_value, map_p *handle)
   * Create a string:value map to store named values
   * @param value_size      Size of value stored
   * @param destroy_value   Function to destroy a value, can be 0
   * @param handle          Created map
   * @return s_ok if map created ok
   */
  extern result_t map_create_nv(dup_fn copy_value,
    destroy_value_fn destroy_value,
    map_p *handle);
  /**
   * @function map_add(map_p map, const void *key, const void *value)
   * Add a value to a map
   * @param key     key value
   * @param value   data to store
   * @return s_ok if added
   */
  extern result_t map_add(map_p map, const void *key, const void *value);
  /**
   * @function map_remove(map_p map, const void *key)
   * Remove and destroy an item from the map
   * @param map     Map to remove from
   * @param key     Key to use
   * @return s_ok if removed ok
   */
  extern result_t map_remove(map_p map, const void *key);
  /**
   * @function map_close(map_p map)
   * Close and release all resources
   * @param map     Map to release
   * @return        s_ok if released ok
   */
  extern result_t map_close(map_p map);
  /**
   * @function map_find(map_p map, const void *key, void **value)
   * Find an item in the map
   * @param map     Map to lookup
   * @param key     Key to match
   * @param value   Value if found
   * @return s_ok if value found
   */
  extern result_t map_find(map_p map, const void *key, void **value);

  //////////////////////////////////////////////////////////////////////////////
  //
  //  Queue Functions
  struct _deque_t;
  typedef struct _deque_t *deque_p;
  /**
   * @function deque_create(uint16_t element_size, uint16_t length, deque_p *deque)
   * Create a queue and initialize it
   * @param element_size  Size of elements in the deque
   * @param length        Number of elements in the deque
   * @param deque         handle to the deque
   * @return s_ok if created ok
   */
  extern result_t deque_create(uint16_t element_size, uint16_t length, deque_p *deque);
  /**
   * @function deque_close(deque_p deque)
   * Release a deque and return all resources
   * @param deque Deque to release
   * @return s_ok if released ok
   */
  extern result_t deque_close(deque_p deque);
  /**
   * @function can_pop(deque_p deque)
   * return true if there is an item on the queue
   * @param   queue to pop
   * @return  s_true if an item available, s_false if not
   */
  extern result_t can_pop(deque_p deque);
  /**
   * @function count(deque_p deque, uint16_t *value)
   * Number of items in the deque
   * @param deque  deque to query
   * @param value  number of items in the deque
   * @result s_ok if a valid queue
   */
  extern result_t count(deque_p deque, uint16_t *value);
  /**
   * @function push_back(deque_p deque, const void *item, uint32_t max_wait)
   * Push an item onto the deque.  will block if no space
   * @param deque   queue to push onto
   * @param item    item to copy onto queue
   */
  extern result_t push_back(deque_p deque, const void *item, uint32_t max_wait);
  /**
   * @function pop_front(deque_p deque, void *item, uint32_t max_wait)
   * Remove an item from a queue
   * @param deque deque to pop from
   * @param item  if non 0 copy of item
   */
  extern result_t pop_front(deque_p deque, void *item, uint32_t max_wait);
  /**
   * @function capacity(deque_p deque, uint16_t *value)
   * Return the number of items that can be in a deque
   * @param deque   Queue to query
   * @param value   number of items that can be added to the queue
   * @return s_ok if a valid deque
   */
  extern result_t capacity(deque_p deque, uint16_t *value);

  ///////////////////////////////////////////////////////////////////////////////
  //
  //    Registry functions
  //
#define REG_NAME_MAX (16)

/**
 * @function reg_create_key(memid_t parent, const char *name, memid_t *key)
 * Create a registry key
 * @param parent    Parent key to create within, 0 for root
 * @param name      Path to the key
 * @param key       Key created
 * @return s_ok if created ok
 * @remarks If the key exists, then is equivalent to an open call
 */
  extern result_t reg_create_key(memid_t parent, const char *name, memid_t *key);
  /**
   * @function reg_open_key(memid_t parent, const char *name, memid_t *key)
   * Open a registry key
   * @param parent    parent to open relative to
   * @param name      path to a key
   * @param key       resulting key
   * @return
   */
  extern result_t reg_open_key(memid_t parent, const char *name, memid_t *key);

  // these are the hdr.data_type values for a field
  typedef enum {
    field_none,                     // no definition
    field_key,											// a key is a parent of other fields
    field_bool,                     // a boolean field (see notes)
    field_int8,
    field_uint8,
    field_int16,
    field_uint16,
    field_int32,
    field_uint32,
    field_float,
    field_xyz,
    field_matrix,
    field_string,
    field_qtn,
    field_lla,
    field_stream,                   // a stream field.  needs stream functions
    } field_datatype;
  /**
   * @function reg_enum_key(memid_t key, field_datatype *type, uint16_t *length, void *data, uint16_t len, char *name, memid_t *child)
   * Enumerate the children of a key
   * @param key     Key to enumerate
   * @param type    Type of child key, if not field_none then will restrict result
   * @param length  Length of enumerated key, when called sets the size of the data area if non-0
   * @param data    Data buffer for result if child is not field_key
   * @param len     length of name buffer
   * @param name    buffer for name
   * @param child   Resulting child key, when called if non-0 then the next key is used
   * @return s_ok if key found ok
   *
   * @example
   * // example to enumerate keys:
   * char name[17];
   * field_datatype type = 0;
   * // must be 0 on first call
   * memid_t child = 0;
   * result_t result;
   *
   * while(succeeded(result = reg_enum_key(0, &type, 0, 0, 17, name, &child)))
   *    {
   *    // field_datatype has the field type, name is the child name
   *    field_datatype = 0;
   *    }
   *
   */
  extern result_t reg_enum_key(memid_t key, field_datatype *type, uint16_t *length, void *data, uint16_t len, char *name, memid_t *child);
  /**
   * @function reg_delete_key(memid_t key)
   * Remove a key, all sub-keys and values
   * @param key Key to remove
   * @return s_ok if completed ok
   */
  extern result_t reg_delete_key(memid_t key);
  /**
   * @function reg_delete_value(memid_t parent, const char *name)
   * Remove a value
   * @param parent  Parent key to remove from
   * @param name    Name of the value to remove
   * @return
   */
  extern result_t reg_delete_value(memid_t parent, const char *name);
  /**
   * @function reg_query_memid(memid_t entry, field_datatype *type, char *name, uint16_t *length, memid_t *parent)
   * Return the information about a memid
   * @param entry   memid to query
   * @param type    Returned type of the entry
   * @param name    Name of the entry must be REG_NAME_MAX length
   * @param length  Length of the entry
   * @param parent  Memid of th parent of the key
   * @return s_ok if a valid memid.
   */
  extern result_t reg_query_memid(memid_t entry, field_datatype *type, char *name, uint16_t *length, memid_t *parent);
  /**
   * @function reg_query_child(memid_t key, const char *entry, memid_t *memid, field_datatype *type, uint16_t *length)
  * Return the information about a child
  * @param key     parent key
  * @param entry   name of entry to query
  * @param type    Returned type of the entry
  * @param name    Name of the entry
  * @param length  Length of the entry
  * @param memid  Memid of the child
  * @return s_ok if a valid memid.
  */
  extern result_t reg_query_child(memid_t key, const char *entry, memid_t *memid, field_datatype *type, uint16_t *length);
  /**
   * @function reg_rename_value(memid_t parent, const char *name, const char *new_name)
   * Rename a value
   * @param parent      Parent key
   * @param name        Value name
   * @param new_name    New Name
   * @return s_ok if the value was renamed
   */
  extern result_t reg_rename_value(memid_t parent, const char *name, const char *new_name);
  /**
   * @function reg_get_int8(memid_t parent, const char *name, int8_t *result)
   * Read a int8_t setting from the registry
   * @param parent        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_int8(memid_t parent, const char *name, int8_t *result);

  /**
   * @function reg_set_int8(memid_t parent, const char *name, int8_t value)
   * Write a int8_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_int8(memid_t parent, const char *name, int8_t value);

  /**
   * @function reg_get_uint8(memid_t parent, const char *name, uint8_t *result)
   * Read a uint8_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_uint8(memid_t parent, const char *name, uint8_t *result);

  /**
   * @function reg_set_uint8(memid_t parent, const char *name, uint8_t value)
   * Write a uint8_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_uint8(memid_t parent, const char *name, uint8_t value);
  /**
   * @function  reg_get_int16(memid_t parent, const char *name, int16_t *result)
   * Read a int16_t setting from the registry
   * @param parent        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_int16(memid_t parent, const char *name, int16_t *result);

  /**
   * @function reg_set_int16(memid_t parent, const char *name, int16_t value)
   * Write a int16_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_int16(memid_t parent, const char *name, int16_t value);

  /**
   * @function reg_get_uint16(memid_t parent, const char *name, uint16_t *result)
   * Read a uint16_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_uint16(memid_t parent, const char *name, uint16_t *result);

  /**
   * @function reg_set_uint16(memid_t parent, const char *name, uint16_t value)
   * Write a uint16_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_uint16(memid_t parent, const char *name, uint16_t value);
  /**
   * @function reg_get_int32(memid_t parent, const char *name, int32_t *result)
   * Read a int32_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_int32(memid_t parent, const char *name, int32_t *result);
  /**
   * @function reg_set_int32(memid_t parent, const char *name, int32_t value)
   * Write a int32_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_int32(memid_t parent, const char *name, int32_t value);
  /**
   * @function reg_get_uint32(memid_t parent, const char *name, uint32_t *result)
   * Read a uint32_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_uint32(memid_t parent, const char *name, uint32_t *result);
  /**
   * @function reg_set_uint32(memid_t parent, const char *name, uint32_t value)
   * Write a uint32_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_uint32(memid_t parent, const char *name, uint32_t value);

  /**
   * @function reg_get_lla(memid_t parent, const char *name, lla_t *result)
   * Read a lla_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_lla(memid_t parent, const char *name, lla_t *result);

  /**
   * @function reg_set_lla(memid_t parent, const char *name, const lla_t *value)
   * Write a lla_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_lla(memid_t parent, const char *name, const lla_t *value);

  /**
   * @function reg_get_xyz(memid_t parent, const char *name, xyz_t *result)
   * Read a xyz_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_xyz(memid_t parent, const char *name, xyz_t *result);

  /**
   * @function reg_set_xyz(memid_t parent, const char *name, const xyz_t *value)
   * Write a xyz_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_xyz(memid_t parent, const char *name, const xyz_t *value);

  /**
   * @function reg_get_matrix(memid_t parent, const char *name, matrix_t *result)
   * Read a matrix_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_matrix(memid_t parent, const char *name, matrix_t *result);

  /**
   * @function reg_set_matrix(memid_t parent, const char *name, matrix_t *value)
   * Write a matrix_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_matrix(memid_t parent, const char *name, matrix_t *value);

  /**
   * @function reg_get_qtn(memid_t parent, const char *name, qtn_t *result)
   * Read a qtn_t setting from the registry
   * @param memid        ID of the setting
   * @param result        Where to put the word read
   * @return true if the setting can be read
   */
  extern result_t reg_get_qtn(memid_t parent, const char *name, qtn_t *result);

  /**
   * @function reg_set_qtn(memid_t parent, const char *name, const qtn_t *value)
   * Write a qtn_t setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_qtn(memid_t parent, const char *name, const qtn_t *value);

  // all registry strings are quite small, but fast
#define REG_STRING_MAX 36
/**
 * @function reg_get_string(memid_t parent, const char *name, char *value, uint16_t *length)
 * Read a string from the registry
 * @param parent          Parent key
 * @param name            name of the setting
 * @param value           buffer with the string, if 0 then actual length returned in length
 * @param length          length of buffer if defined, otherwise where length returned
 * @return s_ok if string read
 */
  extern result_t reg_get_string(memid_t parent, const char *name, char *value, uint16_t *length);

  /**
   * @function reg_set_string(memid_t parent, const char *name, const char *value)
   * Write a string setting
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_string(memid_t parent, const char *name, const char *value);

  /**
   * @function reg_get_float(memid_t parent, const char *name, float *result)
   * Read a float from the registry
   * @param memid          ID of the setting
   * @param result          value to read
   * @return true if the setting can be read
   */
  extern result_t reg_get_float(memid_t parent, const char *name, float *result);

  /**
   * @function reg_set_float(memid_t parent, const char *name, float value)
   * Write a float to the registry
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_float(memid_t parent, const char *name, float value);

  /**
   * @function reg_get_bool(memid_t parent, const char *name, bool *result)
   * Read a bool from the registry
   * @param memid          ID of the setting
   * @param result          value to read
   * @return true if the setting can be read
   */
  extern result_t reg_get_bool(memid_t parent, const char *name, bool *result);

  /**
   * @function reg_set_bool(memid_t parent, const char *name, bool value)
   * Write a bool to the registry
   * @param memid          ID of the setting
   * @param value           value to write
   * @return true if the setting can be written
   */
  extern result_t reg_set_bool(memid_t parent, const char *name, bool value);

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Serial port communications functions
  //
  struct _comm_device_t;
  typedef struct _comm_device_t *comm_device_p;
  /**
   * @function comm_create_device(memid_t key, comm_device_p *device)
   * Open a new comms device
   * @param key       Registry key that holds the setup information
   * @param worker    Semaphore used to block calling thread on
   * @param device    resulting comms device
   * @return s_ok if device is opened ok
   */
  extern result_t comm_create_device(memid_t key, comm_device_p *device);
  /**
   * @function comm_close_device(comm_device_p device)
   * Close a communications device
   * @param device  Device to close
   * @return s_ok if closed ok
   */
  extern result_t comm_close_device(comm_device_p device);
  /**
   * @function comm_write(comm_device_p device, const byte_t *data, uint16_t len, uint32_t timeout)
   * Write a string to the comms device
   * @param device  Device to write to
   * @param data    Character string to write
   * @param len     length of the buffer to write
   * @param timeout max milliseconds to wait to send each byte
   * @return s_ok if written ok
   * @remark this is a blocking call and will only return when the timeout is reached or
   * all characters are sent
   */
  extern result_t comm_write(comm_device_p device, const byte_t *data, uint16_t len, uint32_t timeout);
  /**
   * @function comm_read(comm_device_p device, byte_t *data, uint16_t len, uint16_t *bytes_read, uint32_t timeout)
   * Read bytes from the comms device
   * @param device      Device to read from
   * @param data        Data to read
   * @param len         Length of read buffer
   * @param bytes_read  How many bytes read
   * @param timeout     Max milliseconds to wait for each character
   * @return s_ok if bytes were read
   */
  extern result_t comm_read(comm_device_p device, byte_t *data, uint16_t len, uint16_t *bytes_read, uint32_t timeout);

  typedef enum _ioctl_type
    {
    set_device_ctl,
    get_device_ctl
    } ioctl_type;
  /**
   * @function comm_ioctl(comm_device_p device, ioctl_type type, const void *in_buffer,
      uint16_t in_buffer_size, void *out_buffer, uint16_t out_buffer_size,
                 uint16_t *size_returned)
   * Perform an IOCTL on the device
   * @param device            Device to control
   * @param type              Type of ioctl
   * @param in_buffer         Data to be assigned
   * @param in_buffer_size    Size of data
   * @param out_buffer        Data to be received
   * @param out_buffer_size   Size of out buffer
   * @param size_returned     Actual result size
   * @return s_ok if IOCTL performed.
   */
  extern result_t comm_ioctl(comm_device_p device, ioctl_type type, const void *in_buffer,
    uint16_t in_buffer_size, void *out_buffer, uint16_t out_buffer_size,
    uint16_t *size_returned);

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Generic serial port device ioctls.

  typedef enum
    {
    comms_state_ioctl,
    comms_error_ioctl,
    comms_escape_ioctl,
    comms_break_ioctl,
    comms_event_mask_ioctl,
    comms_modem_status_ioctl,
    comms_timeouts_ioctl,
    } comms_ioctl_type_t;

  typedef enum
    {
    cbr_110 = 110,
    cbr_300 = 300,
    cbr_600 = 600,
    cbr_1200 = 1200,
    cbr_2400 = 2400,
    cbr_4800 = 4800,
    cbr_9600 = 9600,
    cbr_14400 = 14400,
    cbr_19200 = 19200,
    cbr_38400 = 38400,
    cbr_57600 = 57600,
    cbr_115200 = 115200,
    cbr_128000 = 128000,
    cbr_240000 = 240000,
    cbr_256000 = 256000
    } baud_rate_t;

  typedef enum
    {
    no_parity,
    odd_parity,
    even_parity,
    mark_parity,
    space_parity
    } parity_mode_t;

  typedef enum
    {
    dtr_disabled,
    dtr_enabled,
    dtr_flowcontrol
    } dtr_mode_t;

  typedef enum
    {
    rts_control_disabled,
    rts_control_enabled,
    rts_control_handshake,
    rts_control_toggle
    } rts_control_t;

  typedef enum
    {
    one_stop_bit = 0,
    one_five_stop_bit = 1,
    two_stop_bit = 2
    } stop_bits_t;

  typedef struct _comms_ioctl_t
    {
    uint16_t version;
    comms_ioctl_type_t ioctl_type;
    } comms_ioctl_t;

  typedef struct _comms_state_ioctl_t
    {
    comms_ioctl_t ioctl;
    baud_rate_t baud_rate;
    bool binary_mode;
    parity_mode_t parity;
    bool cts_out_enabled;
    bool dsr_out_enabled;
    bool dsr_in_enabled;
    dtr_mode_t dtr_mode;
    rts_control_t rts_control;
    bool xmit_continue_on_xoff;
    bool xoff_out_enabled;
    bool xoff_in_enabled;
    bool parity_character_enabled;
    bool ignore_null_characters;
    bool abort_on_error;
    bool parity_checking_enabled;
    int16_t xon_limit;
    int16_t xoff_limit;
    uint16_t byte_size;
    stop_bits_t stop_bits;
    char xon_char;
    char xoff_char;
    char error_character;
    char parity_character;
    char eof_char;
    char event_char;
    } comms_state_ioctl_t;

  typedef struct _comms_error_ioctl_t
    {
    bool overflow;
    bool overrun;
    bool parity;
    bool framing_error;
    bool break_char;
    } comms_error_ioctl_t;

  typedef enum
    {
    escape_set_xoff = 1,
    escape_set_xon = 2,
    escape_set_rts = 3,
    escape_clear_rts = 4,
    escape_set_dtr = 5,
    escape_clear_dtr = 6,
    escape_set_break = 8,
    escape_clear_break = 9
    } escape_character_type;

  typedef struct _comms_escape_ioctl_t
    {
    comms_ioctl_t ioctl;
    escape_character_type escape_type;
    } comms_escape_ioctl_t;

  typedef struct _comms_break_ioctl_t
    {
    comms_ioctl_t ioctl;
    bool set_break;
    } comms_break_ioctl_t;

  typedef uint16_t event_mask_t;
  // types of events that can be waited on

#define ev_break 0x0040
#define ev_cts 0x0008
#define ev_dsr 0x0010
#define ev_err 0x0080
#define ev_event1 0x0800
#define ev_event2 0x1000
#define ev_perr 0x0200
#define ev_ring 0x0100
#define ev_rlsd 0x0020
#define ev_rxfull 0x0400
#define ev_rxchar 0x0001
#define ev_rxflag 0x0002
#define ev_txempty 0x0004

  typedef struct _comms_event_mask_ioctl_t
    {
    comms_ioctl_t ioctl;

    semaphore_p notification_event;
    event_mask_t mask;
    } comms_event_mask_ioctl_t;

  typedef struct _comms_modem_status_ioctl_t
    {
    comms_ioctl_t ioctl;
    bool cts_on;
    bool dsr_on;
    bool ring_on;
    bool rlsd_on;
    } comms_modem_status_ioctl_t;

  typedef struct _comms_timeouts_ioctl_t
    {
    comms_ioctl_t ioctl;
    /*
     The maximum time allowed to elapse between the arrival of two bytes on the communications line,
     in milliseconds. During a ReadFile operation, the time period begins when the first uint8_t is received.
     If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is
     completed and any buffered data is returned. A value of zero indicates that interval time-outs are not used.

     A value of limit<unsigned int>::max(), combined with zero values for both the ReadTotalTimeoutConstant and
     ReadTotalTimeoutMultiplier members, specifies that the read operation is to return immediately with the bytes
     that have already been received, even if no bytes have been received.
     */
    unsigned int read_interval_timeout;
    /*
     The multiplier used to calculate the total time-out period for read operations, in milliseconds.
     For each read operation, this value is multiplied by the requested number of bytes to be read.
     */
    unsigned int read_total_timeout_multiplier;
    /*
     A constant used to calculate the total time-out period for read operations, in milliseconds.
     For each read operation, this value is added to the product of the ReadTotalTimeoutMultiplier member
     and the requested number of bytes.

     A value of zero for both the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant members indicates
     that total time-outs are not used for read operations.
     */
    unsigned int read_total_timeout_constant;
    /*
     The multiplier used to calculate the total time-out period for write operations, in milliseconds.
     For each write operation, this value is multiplied by the number of bytes to be written.
     */
    unsigned int write_total_timeout_multiplier;
    /*
     A constant used to calculate the total time-out period for write operations, in milliseconds.
     For each write operation, this value is added to the product of the WriteTotalTimeoutMultiplier
     member and the number of bytes to be written.

     A value of zero for both the WriteTotalTimeoutMultiplier and WriteTotalTimeoutConstant members
     indicates that total time-outs are not used for write operations.
     */
    unsigned int write_total_timeout_constant;
    } comms_timeouts_ioctl_t;

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Stream functions.
  //
  // The registry supports a stream object which can be treated as a bufferred
  // stream.  The size of a stream is limited to
  // 32 kBytes only
  struct _stream_t;
  typedef struct _stream_t *stream_p;
  /**
   * @function reg_stream_open(memid_t parent, const char *path, stream_p *stream)
   * Open a stream given a path.
   * @param parent    registry key the path is relative to, 0 for root
   * @param path      path to the stream
   * @param stream    opened stream
   * @return s_ok if the stream exists.
   */
  extern result_t reg_stream_open(memid_t parent, const char *path, stream_p *stream);
  /**
   * @function reg_stream_create(memid_t parent, const char *path, stream_p *stream)
   * Create a new stream.
   * @param parent    registry key the path is relative to.  The key path must exist
   * @param path      path to the stream.
   * @param stream    newly created stream
   * @return s_ok if the stream created ok.
   */
  extern result_t reg_stream_create(memid_t parent, const char *path, stream_p *stream);
  /**
   * @function manifest_open(const char *path, stream_p *stream)
   * Open a manifest stream.
   * @param key       Key to open resource from
   * @param path      Path to the stream. see remarks
   * @param stream    opened stream
   * @return s_ok if the stream was opened ok
   * @remark A manifest stream is a file that is stored as a base64
   * encoded file.  This allows for streams to be created using
   * the CLI to store resources (images and fonts) in the registry
   * or in code.  The resource is read-only
   */
  extern result_t manifest_open(memid_t key, const char *path, stream_p *stream);
  /**
   * @function manifest_create(const char *path, stream_p *stream)
   * Open a manifest stream.
   * @param literal   Base64 encoded binary image
   * @param stream    opened stream
   * @return s_ok if the stream was opened ok
   * @remark A manifest stream is a file that is stored as a base64
   * encoded file.  This allows for streams to be created using
   * the CLI to store resources (images and fonts) in the registry
   * or in code.  The resource is read only
   */
  extern result_t manifest_create(const char *literal, stream_p *stream);
#ifndef NAME_MAX
#define NAME_MAX	256
#endif
  /**
  * @function stream_create(const char *path, stream_p *stream)
  * Create a stream in the file system
  * @param path path to the new stream
  * @param stream stream that is created
  * @return s_ok if the stream created
  */
  result_t stream_create(const char *path, stream_p *stream);
  /**
  * @function stream_open(const char *path, stream_p *stream)
  * Open a stream in the file system
  * @param path path to the stream.  Is unix style "/dir/<file>
  * @param stream resulting stream
  * @return s_ok if file is found
  */
  result_t stream_open(const char *path, stream_p *stream);
  /**
  * @function stream_rename(stream_p stream, const char *new_filename)
  * Rename a stream
  * @param stream already opened stream
  * @param new_filename the name of the new file
  * @return s_ok if renamed ok
  */
  result_t stream_rename(stream_p stream, const char *new_filename);
  /**
  * @function create_directory(const char *path)
  * Create a directory
  * @param path path to the new directory
  * @return s_ok if path created
  */
  result_t create_directory(const char *path);
  /**
  * @function remove_directory(const char *path)
  * Remoce an empty directory
  * @param path path to the empty directory
  * @return s_ok if the path can be removed
  */
  result_t remove_directory(const char *path);
  /**
  * @function open_directory(const char *dirname, handle_t *dirp)
  * Open a directory to enumerate the contents
  * @param dirname path to the directory
  * @param dirp handle to a directory enumeration
  * @return s_ok if opened
  */
  result_t open_directory(const char *dirname, handle_t *dirp);

  typedef enum _dir_entry_type {
    et_file,
    et_directory
    } dir_entry_type;
  /**
  * @function read_directory(handle_t dirp, dir_entry_type *et, char *buffer, size_t len)
  * Read a directory entry and advance the directory pointer
  * @param dirp opened directory pointer
  * @param et type of entry
  * @param buffer buffer to receove the name
  * @param len length of the buffer passed in
  * @return s_ok if opened ok
  */
  result_t read_directory(handle_t dirp, dir_entry_type *et, char *buffer, size_t len);
  /**
  * @function rewind_directory(handle_t dirp)
  * Skip to the previous directory entry
  * @param dirp directory entry pointer
  * @return s_ok if rewound
  */
  result_t rewind_directory(handle_t dirp);
  /**
  * @function close_directory(handle_t dirp)
  * Close a directory handle
  * @param dirp handle to the directory
  * @return s_ok if closed ok
  */
  result_t close_directory(handle_t dirp);
  /**
  * @function stream_sync(stream_p stream)
  * Force a flush of all data for the stream
  * @param stream  stream to ensure all data is sync
  * @return s_ok if sync happened
  */
  result_t stream_sync(stream_p stream);
  /**
  * @function freespace(const char *path, off_t *space)
  * return how much space is available on the device
  * @param path path to a device
  * @param space resulting free space
  * @return s_ok if the path is valid
  */
  result_t freespace(const char *path, uint32_t *space);
  /**
  * @function totalspace(const char *path, off_t *space)
  * return the total space available on a device
  * @param path path to the mounted file system
  * @param space resulting space available
  * @return s_ok if a valid path
  */
  result_t totalspace(const char *path, uint32_t *space);
  /**
   * @function stream_close(stream_p stream)
   * Close the stream handle and release all resources
   * @param stream    stream
   * @return s_ok if closed.  The handle is no longer valid
   */
  extern result_t stream_close(stream_p stream);
  /**
   * @function stream_delete(stream_p stream)
   * Close the stream, and delete the entire persistent state
   * @param stream    stream to close
   * @return s_ok if closed ok.
   */
  extern result_t stream_delete(stream_p stream);
  /**
   * @function stream_eof(stream_p stream)
   * Detect end of read pointer on stream
   * @param stream    stream to check
   * @return s_ok if at eof, s_false if not.
   */
  extern result_t stream_eof(stream_p stream);
  /**
   * @function stream_read(stream_p stream, void *buffer, uint16_t size, uint16_t *read)
   * Read from a stream
   * @param stream    Stream to read from
   * @param buffer    Buffer to read bytes into
   * @param size      Size of read buffer
   * @param read      Number of bytes read into the buffer
   * @return s_ok if read ok.
   */
  extern result_t stream_read(stream_p stream, void *buffer, uint16_t size, uint16_t *read);
  /**
   * @function stream_write(stream_p stream, const void *buffer, uint16_t size)
   * Write bytes to a stream
   * @param stream    Stream to write to
   * @param buffer    Buffer of bytes to write
   * @param size      Number of bytes to write
   * @return s_ok if the buffer was written to the stream
   */
  extern result_t stream_write(stream_p stream, const void *buffer, uint16_t size);
  /**
   * @function stream_getpos(stream_p stream, uint16_t *pos)
   * Get the position of the stream for the next operation
   * @param stream    Stream to query
   * @param pos       Position of the stream for the next read or write
   * @return s_ok if a valid stream
   */
  extern result_t stream_getpos(stream_p stream, uint32_t *pos);
  /**
   * @function stream_setpos(stream_p stream, uint16_t pos)
   * Move the stream offset
   * @param stream    Stream to query
   * @param pos       Position to set
   * @return s_ok if the position is valid.  This can at the end of the stream
   * or within the stream.  setting to UINT16_MAX will move the pointer to the
   * end of the stream.
   */
  extern result_t stream_setpos(stream_p stream, uint32_t pos);
  /**
   * @function stream_length(stream_p stream, uint16_t *length)
   * Return the number of bytes in a stream
   * @param stream    Stream to query
   * @param length    Length of the stream, max is 32768
   * @return s+ok if the handle is a valid stream
   */
  extern result_t stream_length(stream_p stream, uint32_t *length);
  /**
   * @function stream_truncate(stream_p stream, uint16_t length)
   * Set the stream length
   * @param stream    Stream to set
   * @param length    Length to truncate to.  This must be at or less than the
   * stream length
   * @return s_ok if truncated ok
   */
  extern result_t stream_truncate(stream_p stream, uint32_t length);
  /**
   * @function stream_copy(stream_p from, stream_p to)
   * Copy betrween two streams
   * @param from      Stream to copy from
   * @param to        Stream to copy to
   * @return s_ok if truncated ok
   */
  extern result_t stream_copy(stream_p from, stream_p to);
  /**
   * @function stream_path(stream_p stream, bool full_path, uint16_t path_length, char *path)
  * Return the name of a stream
  * @param stream    Stream to get name of
  * @param full_path True if the full path of the stream is needed
  * @param path      Resulting path
  * @return s_ok if truncated ok
  */
  extern result_t stream_path(stream_p stream, bool full_path, uint16_t path_length, char *path);
  /**
   * Current ASCII input stream.  May be a persistent stream or a
   * console.
   */
  extern stream_p console_in;
  /**
   * Current ASCII output stream.  May be a persistent stream or console
   */
  extern stream_p console_out;
  /**
   * Current ASCII error stream.  Generally always a console
   */
  extern stream_p console_err;
  /**
   * @function strstream_create(const char *lit, stream_p *stream)
   * Create an in-memory stream.  Should be deleted when done.
   * @param lit     Optional literal to copy into the stream
   * @param len     Length of the literal
   * @param read_only If set then the stream will be created read-only and the
   * buffer will not be copied into a dynamic memory
   * @param stream  Resulting stream
   * @return s_ok if created ok
   */
  extern result_t strstream_create(const uint8_t *buffer, uint32_t len, bool read_only, stream_p *stream);
  /**
   * @function strstream_get(stream_p stream, const char **lit)
   * Return the underlying character buffer of the stream
   * @param stream  Stream to query
   * @param lit     Resulting buffer
   * @return s_ok if stream is a string stream.
   */
  extern result_t strstream_get(stream_p stream, const void **lit);
  /**
   * @function stream_printf(stream_p stream, const char *format, ...)
   * Print a formatted string.
   * @param stream    Stream to print to
   * @param format    Format to print as
   * @param cb        Callback to get an argument
   * @param argv      Pointer to arguments
   * @return s_ok if printed ok
   */
  extern result_t stream_printf(stream_p stream, const char *format, ...); // __attribute__((format(printf, 2, 3)));
  /**
   * @function stream_vprintf(stream_p stream, const char *fmt, va_list ap)
   * print a formatted stream
   * @param stream
   * @param fmt
   * @param ap
   * @return
   */
  extern result_t stream_vprintf(stream_p stream, const char *fmt, va_list ap);

  typedef enum _scan_type
    {
    s_int8,       // char
    s_uint8,      // unsigned char
    s_int16,      // int
    s_uint16,     // unsigned int
    s_int32,      // long
    s_uint32,     // unsigned long
    s_str,        // char *
    s_float,      // float, double
    } scan_type;
  /**
   * @function get_arg_fn(uint16_t arg, void *argv, scan_type dt, void *value)
   * Callback function for scanf/printf
   * @param arg     Argument number 0..n
   * @param argv    Callback parameter
   * @param dt      Datatype scanned or requested
   * @param value   Pointer to pointer to the value passed-on or returned
   * @return s_ok if a valid param
   */
  typedef result_t(*get_arg_fn)(uint16_t arg, void *argv, scan_type dt, void *value);
  /**
   * @function stream_printf_cb(stream_p stream, const char *format, get_arg_fn cb, void *argv)
   * Print a formatted string.
   * @param stream    Stream to print to
   * @param format    Format to print as
   * @param cb        Callback to get an argument
   * @param argv      Pointer to arguments
   * @return s_ok if printed ok
   */
  extern result_t stream_printf_cb(stream_p stream, const char *format, get_arg_fn cb, void *argv);
  /**
   * @function stream_scanf_cb(stream_p stream, const char *format, get_arg_fn cb, void *argv)
   * Scan arguments from a string.
   * @param stream    Stream to scan from
   * @param format    Format of the arguments
   * @param cb        Callback to get an argument
   * @param argv      Pointer to arguments
   * @return s_ok if scanned ok
   */
  extern result_t stream_scanf_cb(stream_p stream, const char *format, get_arg_fn cb, void *argv);
  /**
   * @function stream_scanf(stream_p stream, const char *format, ...)
   * Scan arguments from a stream.
   * @param stream    Stream to scan from
   * @param format    Format of the arguments
   * @param cb        Callback to get an argument
   * @param argv      Pointer to arguments
   * @return s_ok if scanned ok
   */
  extern result_t stream_scanf(stream_p stream, const char *format, ...); //  __attribute__((format(scanf, 2, 3)));
  /**
   * @function stream_vscanf(stream_p stream, const char *fmt, va_list ap)
   * Scan arguments from a stream
   * @param stream    Stream to scan from
   * @param fmt       Format of the arguments
   * @param ap        Varargs list
   * @return s_ok if scanned ok
   */
  extern result_t stream_vscanf(stream_p stream, const char *fmt, va_list ap);
  /**
   * @function stream_getc(stream_p stream, char *ch)
   * Read one character from the stream
   * @param stream    Stream to read from
   * @param ch        character read
   * @return s_ok if character available, or e_end_of_file if no characters
   */
  extern result_t stream_getc(stream_p stream, char *ch);
  /**
   * @function stream_putc(stream_p stream, char ch)
   * Send one character to the stream
   * @param stream    Stream to write to
   * @param ch        character to write
   * @return s_ok if written ok
   */
  extern result_t stream_putc(stream_p stream, char ch);
  /**
   * @function stream_ungetc(stream_p stream, char ch
   * Push the character ch onto the end of the stream.
   * @param stream    Stream to write to
   * @param ch        character to append
   * @return s_ok if the stream position was rewound
   */
  extern result_t stream_ungetc(stream_p stream, char ch);
  /**
   * @function stream_gets(stream_p stream, char *buffer, uint16_t len)
   * Read an ascii line of characters from the stream.
   * @param stream    Stream to read from
   * @param buffer    Buffer to read into
   * @param len       Length of read buffer
   * @return s_ok if read ok
   * This routine will wait for an EOL character or a buffer full, or EOF before
   * returning.  The end of line character is a \n character.  Depending on the
   * stream implementation this may be translated to a \r character
   * the bytes read will always be len-1 so the trailing \0 can be appended
   */
  extern result_t stream_gets(stream_p stream, char *buffer, uint16_t len);
  /**
   * @function stream_puts(stream_p stream, const char *str)
   * Write a string to a stream
   * @param stream    Stream to write to
   * @param str       string to send
   * @return s_ok if written ok.
   */
  extern result_t stream_puts(stream_p stream, const char *str);

  ///////////////////////////////////////////////////////////////////////////////
  //
  // Decompression of a stream.
  //
  // Functions to take a stream of bytes that is compressed using the DEFLATE
  // algorithm.
  //
  /**
   * @function result_t (*get_byte_fn)(handle_t parg, uint32_t offset, uint8_t *data)
   * Get a byte from the decompressed buffer
   * @param parg    Opaque argument for the accessor
   * @param offset  Offset into the buffer
   * @param data    Data returned
   * @return s_ok if the offset is within bounds of the buffer and the data is available
   */
  typedef result_t(*get_byte_fn)(handle_t parg, uint32_t offset, uint8_t *data);
  /**
  * @function result_t (*get_byte_fn)(handle_t parg, uint32_t offset, uint8_t *data)
  * Get a byte from the decompressed buffer
  * @param parg    Opaque argument for the accessor
  * @param offset  Offset into the buffer
  * @param data    Data to de assigned
  * @return s_ok if the offset is within bounds of the buffer and the data was written
  */
  typedef result_t(*set_byte_fn)(handle_t parg, uint32_t offset, uint8_t data);
  /**
   * @function result_t decompress(stream_p stream, handle_t parg, get_byte_fn getter, set_byte_fn setter, uint32_t *length)
   * Decompress a stream into a user defined buffer
   * @param stream    Source stream to read from.  Must be a valid DEFLATE format stream
   * @param parg      User defined callback argument
   * @param getter    Function to read a byte from the user buffer
   * @param setter    Function to set a byte into the user buffer
   * @param length    Optional value that counts the number of bytes decompressed
   * @return s_ok if the stream was decoded ok.
   */
  extern result_t decompress(stream_p stream, handle_t parg, get_byte_fn getter, set_byte_fn setter, uint32_t *length);

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Kernel start routines
  /**
   * @function neutron_run(const char *name, uint16_t stack_size, task_callback callback, void *parg, uint8_t priority, task_p *task)
   * Initialize memory, create a root task and dispatch it.
   * Never returns
   * @param heap          optional memory heap if a built-in scheduler
   * @param length        length of the heap
   * @param name          name of the task
   * @param stack_size    size of stack to allocate (words)
   * @param callback      task main routine
   * @param parg          argument to pass to task
   * @param priority      initial priority
   * @param task          created task handle
   */
  extern void neutron_run(void *heap,
    size_t length,
    const char *name,
    uint16_t stack_size,
    task_callback callback,
    void *parg,
    uint8_t priority,
    task_p *task);


  ////////////////////////////////////////////////////////////////////////////////
  //
  // Functions that allow hooking the publisher to filter or process
  // can datapoints
  //


  typedef struct _filter_params_t {
    float coefficient;
    float value;
    } filter_params_t;

  typedef float (*mac_fn)(filter_type_e filter_type,
    float val,
    uint16_t length,
    filter_params_t *filter);
  /**
   * Get a function that implements a mac.  If none provided a 'c' function is used
   * @param memid The key for the filter that is to be filtered
   * @return a function, if 0 then a default function is used.
   */
  extern mac_fn bsp_get_mac(memid_t memid);

  //////////////////////////////////////////////////////////////////////
  //
  //    Diagnostics and tracing
  extern void __assert(const char *file, int line, bool check);

#ifdef _DEBUG
#define assert(chk) __assert(__FILE__, __LINE__, chk)
#else
#define assert(chk)
#endif

#ifdef trace
#undef trace
#endif

  // trace writes messages to a stream in the registry that is limited to 4kbytes.
  // used for startup logging
  //
  // can be accessed using shell
  //    cat dmesg
  // similar to the linux varsion
  extern void trace(uint16_t level, const char *msg, ...);

#define trace_emergency(...)  trace(0, __VA_ARGS__)
#define trace_alert(...)      trace(1, __VA_ARGS__)
#define trace_critical(...)   trace(2, __VA_ARGS__)
#define trace_error(...)      trace(3, __VA_ARGS__)
#define trace_warning(...)    trace(4, __VA_ARGS__)
#define trace_notice(...)     trace(5, __VA_ARGS__)
#define trace_info(...)       trace(6, __VA_ARGS__)
#define trace_debug(...)      trace(7, __VA_ARGS__)

  // platform trace
#if defined(_DEBUG) & !defined(_NO_TRACE)
  extern void platform_trace(uint16_t level, const char *msg, va_list va);
#else
#define platform_trace(lvl, msg, va)
#endif


#ifdef __cplusplus
  }
#endif

#endif
