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
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#ifndef __can_aerospace_h__
#define __can_aerospace_h__

#include <stdint.h>

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

// Un-used
#define unused_id                     0

// Defined module ID's
#define ahrs_node_id                  128         // this allows for up to 8 AHRS units/bus
#define ahrs_node_id_last             135
#define edu_node_id                   136
#define edu_node_id_last              143
#define fdu_node_id                   144
#define fdu_node_id_last              151
#define aio_start_id                  152
#define aio_end_id                    159
#define efi_node_id                   160
#define efi_node_id_last              164
#define slcan_id                      192         // serial adapter for CanFLY
#define aacu_id                       193

// these are the default node settings

//  Datatype  Units  Notes
// AHRS generated data
#define id_pitch_acceleration         300   // FLOAT      g       forward: + aft: -
#define id_roll_acceleration          301   // FLOAT      g       right: + left: -
#define id_yaw_acceleration           302   // FLOAT      g       up: + down: -
#define id_pitch_rate                 303   // FLOAT      rad/s   nose up: + nose down: -
#define id_roll_rate                  304   // FLOAT      rad/s   roll right: + roll left: -
#define id_yaw_rate                   305   // FLOAT      rad/s   yaw right: + yaw left: -
#define id_roll_angle_magnetic        306   // FLOAT      rad
#define id_pitch_angle_magnetic       307   // FLOAT      rad
#define id_yaw_angle_magnetic         308   // FLOAT      rad
#define id_pitch_angle                311   // FLOAT      rad     nose up: + nose down: -
#define id_roll_angle                 312   // FLOAT      rad     roll right: + roll left: -
#define id_yaw_angle                  313   // FLOAT      rad     yaw right: + yaw left: -
#define id_altitude_rate              314   // FLOAT      m/s
#define id_indicated_airspeed         315   // FLOAT      m/s
#define id_true_airspeed              316   // FLOAT      m/s     
#define id_calibrated_airspeed        317   // FLOAT      m/s
#define id_angle_of_attack            318   // SHORT      deg
#define id_qnh                        319   // SHORT      hPa     published every 60 seconds.  changed with id_qnh_up, id_qnh_dn
#define id_baro_corrected_altitude    320   // FLOAT      m
#define id_heading_angle              321   // SHORT      deg     0-360   True heading
#define id_heading                    322   // SHORT      deg     0-360
#define id_pressure_altitude          323   // FLOAT      m
#define id_outside_air_temperature    324   // SHORT      K
#define id_differential_pressure      325   // FLOAT      hPa
#define id_static_pressure            326   // FLOAT      hPa
#define id_wind_speed                 333   // FLOAT      m/s
#define id_wind_direction             334   // float      rad     0-360
#define id_gps_aircraft_lattitude     1036  // FLOAT      deg
#define id_gps_aircraft_longitude     1037  // FLOAT      deg
#define id_gps_aircraft_height_above_ellipsoid 1038   // FLOAT      m
#define id_gps_groundspeed            1039  // FLOAT      m/s
#define id_true_track                 1040  // SHORT      deg
#define id_magnetic_track             1041  // SHORT      deg
#define id_magnetic_heading           1069  // SHORT      deg
// calculated from the INS
#define id_position_lattitude         1070  // FLOAT      rad
#define id_position_longitude         1071  // FLOAT      rad
#define id_position_altitude          1072  // FLOAT      m       true altitud
#define id_velocity_x                 1073  // FLOAT      m/s
#define id_velocity_y                 1074  // FLOAT      m/s
#define id_velocity_z                 1075  // FLOAT      m/s

#define id_magnetic_variation         1121  // SHORT      deg
#define id_def_utc                    1200  // CHAR4              format: 13h43min22s 13 43 22 00
#define id_def_date                   1201  // CHAR4              format: 12. June 1987 12 06 19 87
#define id_qnh_up                     1202  // SHORT              can increment by # of steps
#define id_qnh_dn                     1203  // SHORT
#define id_set_magnetic_variation     1204
#define id_heading_up                 1206
#define id_heading_dn                 1207
#define id_stall_warning              1211  // SHORT     Stall warning switch activated
#define id_total_time                 1212  // LONG      Total time in service  Hrs * 10
#define id_air_time                   1213  // LONG      Total time where IAS > 10 kts Hrs * 10
#define id_tach_time                  1214  // LONG      Total time where the engine is running Hrs * 10
#define id_pitch                      1217  // SHORT      deg    +/- 180
#define id_ahrs_status                1218  // SHORT     0 = ahrs initializing, 1 = ahrs waiting data, 2 = ahrs operational, -1 = ahrs fail

// Autopilot generated data.  In all cases the service code field contains waypoint # which can be 0..15
#define id_track                      1006  // float      radians   track over the ground (true)
#define id_deviation                  1007  // float      m         deviation from track in meters
#define id_track_angle_error          1008  // SHORT      deg
#define id_estimated_time_to_next     1009  // SHORT      min
#define id_estimated_time_of_arrival  1010  // SHORT      min
#define id_estimated_enroute_time     1011  // SHORT      min
#define id_waypoint_identifier_0_3    1012  // ACHAR4        
#define id_waypoint_identifier_4_7    1013  // ACHAR4        
#define id_waypoint_identifier_8_11   1014  // ACHAR4        
#define id_waypoint_identifier_12_15  1015  // ACHAR4        
#define id_waypoint_type_identifier   1016  // SHORT          
#define id_waypoint_latitude          1017  // FLOAT      deg
#define id_waypoint_longitude         1018  // FLOAT      deg
#define id_waypoint_minimum_altitude  1019  // FLOAT      m
#define id_waypoint_maximum_altitude  1023  // FLOAT      m
#define id_distance_to_next           1024  // float      m
#define id_distance_to_destination    1025  // float      m
#define id_selected_course            1125  // SHORT      deg
#define id_desired_track_angle        1135  // FLOAT      rad       magnetic angle to fly to get to destination
#define id_nav_command                1136  // SHORT      0 = clear route, 1 = activate route, 2 = deactivate_route, 3 = invert route
#define id_nav_valid                  1137  // SHORT      0 = nav data not valid, 1 = enroute data valid
#define id_fix_valid                  1138  // SHORT      0 = nav data not valid, 1 = enroute data valid
#define id_imu_valid                  1139  // SHORT      0 = no data, 1 = valid data
#define id_edu_valid                  1140  // SHORT      0 = error, 1 = valid
#define id_aux_battery                1141  // SHORT      0 = not on battery, 1 = on aux battery
#define id_trim_up                    1142  // SHORT      number of trim steps
#define id_trim_dn                    1143  // SHORT
#define id_trim_left                  1144  // SHORT
#define id_trim_right                 1145  // SHORT
#define id_autopilot_engage           1146  // SHORT      0 = off, 1 = on
#define id_autopilot_set_max_roll     1147  // SHORT      Maximum roll rate allowed
#define id_autopilot_set_vs_rate      1148  // SHORT      Set maximum pitch allowed
#define id_waypoint_turn_heading      1149  // SHORT      deg, heading to turn onto
#define id_roll_servo_set_position    1150  // SHORT      degrees position +/-90
#define id_pitch_servo_set_postion    1151  // SHORT      degrees position +/-90
#define id_autopilot_power            1152  // SHORT      autopilot power is on
#define id_autopilot_alt_mode         1153  // SHORT      0 = off, 1 = on  Altitude hold mode enable
#define id_autopilot_vs_mode          1154  // SHORT      0 = off, 1 = on  Vertical speed hold mode enable
#define id_autopilot_mode             1155  // SHORT      see enumeration below
#define id_autopilot_status           1156  // SHORT      bitmask, published every 5secs or on change

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

#define id_autopilot_altitude         1157  // SHORT      m assigned altitude
#define id_autopilot_vertical_speed   1158  // SHORT      m/s vertical rate of climb/descent

// EDU generated data
#define id_engine_rpm                 500   // SHORT      r/min
#define id_engine_rpm_a               501   // SHORT      r/min
#define id_exhaust_gas_temperature1   520   // SHORT      K
#define id_exhaust_gas_temperature2   521   // SHORT      K
#define id_exhaust_gas_temperature3   522   // SHORT      K
#define id_exhaust_gas_temperature4   523   // SHORT      K
#define id_manifold_pressure          528   // FLOAT      hPa
#define id_oil_pressure               532   // FLOAT      hPa
#define id_oil_temperature            536   // SHORT      K
#define id_cylinder_head_temperature1 548   // SHORT      K
#define id_cylinder_head_temperature2 549   // SHORT      K
#define id_cylinder_head_temperature3 550   // SHORT      K
#define id_cylinder_head_temperature4 551   // SHORT      K
#define id_engine_rpm_b               564   // SHORT      r/min
#define id_manifold_pressure_b        592   // FLOAT      hPa
#define id_fuel_pressure              684   // FLOAT      hPa
#define id_dc_voltage                 920   // SHORT      V
#define id_dc_current                 930   // SHORT      A
#define id_fuel_flow_rate             590   // SHORT      l/hr
#define id_fuel_consumed              591   // SHORT      l     Fuel consumed since power on
#define id_edu_left_fuel_quantity     666   // SHORT      l
#define id_edu_right_fuel_quantity    667   // SHORT      l

#define id_timing_divergence          2200  // SHORT      msec difference between left & right tach
#define id_left_mag_rpm               2201
#define id_right_mag_rpm              2202
#define id_left_mag_adv               2203
#define id_right_mag_adv              2204
#define id_left_mag_map               2205
#define id_right_mag_map              2206
#define id_left_mag_volt              2207
#define id_right_mag_volt             2208
#define id_left_mag_temp              2209
#define id_right_mag_temp             2210
#define id_left_mag_coil1             2211
#define id_right_mag_coil1            2212
#define id_left_mag_coil2             2213
#define id_right_mag_coil2            2214

// FGDU generated data
#define id_fuel_flow_rate_left        524   // SHORT      l/hr
#define id_fuel_flow_rate_right       588   // SHORT      l/hr
#define id_active_fuel_tank           591   // SHORT      1=left, 2=right, 3=both
#define id_left_fuel_quantity         668   // SHORT      l
#define id_right_fuel_quantity        669   // SHORT      l
#define id_pitot_temperature          670   // SHORT      degrees
#define id_pitot_heat_status          671   // SHORT      1 = heater on, 0 = off
#define id_pitot_power_status         672   // SHORT      1 = power on, 0 = off
#define id_fdu_board_temperature      673   // SHORT      degrees
#define id_set_left_fuel_qty          674   // SHORT      l
#define id_set_right_fuel_qty         675   // SHORT      l

// TODO: check these
// EFI definitions
#define id_mixture_lever             700    // SHORT    0-100%
#define id_injector_dwell            701    // SHORT    0-1024

// Propellor speed definitions
#define id_propeller_speed_lever      800   // SHORT      rpm
#define id_propeller_motor_current    801   // SHORT      A
#define id_propeller_low_speed        802   // SHORT      rpm
#define id_propeller_high_speed       803   // SHORT      rpm

// Definitions for a Rotax ECU, sends data on 2 channels at 125kHz

#define id_rotax_rpm_a                500
#define id_rotax_fuel_flow_a          524   // liter / hour
#define id_rotax_manifold_pressure_a  532   // hpa
#define id_rotax_oil_pressure_a       532   // hpa
#define id_rotax_oil_temperature_a    536   // kelvin
#define id_rotax_coolant_temp_a       548
#define id_rotax_egt1_a               628
#define id_rotax_egt2_a               630
#define id_rotax_egt3_a               632
#define id_rotax_egt4_a               634
#define id_rotax_intake_temperature_a 640
#define id_rotax_outside_air_temp_a   642
#define id_rotax_throttle_position_a  692
#define id_rotax_ambient_pressure_a   694
#define id_rotax_voltage_a            950
#define id_rotax_engine_status_a      556
#define id_rotax_engine_hours_a       1208
#define id_rotax_ecu_hours_a          1216
#define id_rotax_sensor_status_1_a    1500
#define id_rotax_sensor_status_2_a    1504
#define id_rotax_device_status_1_a    1508
#define id_rotax_device_status_2_a    1512

#define id_rotax_rpm_b                564
#define id_rotax_fuel_flow_b          588
#define id_rotax_manifold_pressure_b  592
#define id_rotax_oil_pressure_b       596
#define id_rotax_oil_temperature_b    600
#define id_rotax_coolant_temp_b       612
#define id_rotax_egt1_b               644
#define id_rotax_egt2_b               646
#define id_rotax_egt3_b               648
#define id_rotax_egt4_b               650
#define id_rotax_intake_temperature_b 656
#define id_rotax_outside_air_temp_b   658
#define id_rotax_throttle_position_b  696
#define id_rotax_ambient_pressure_b   698
#define id_rotax_voltage_b            954
#define id_rotax_engine_status_b      620
#define id_rotax_engine_hours_b       1212
#define id_rotax_ecu_hours_b          1220
#define id_rotax_sensor_status_1_b    1516
#define id_rotax_sensor_status_2_b    1520
#define id_rotax_device_status_1_b    1524
#define id_rotax_device_status_2_b    1528

// this is the Flight plan service.  It supports up to 255 routes.
// Route 0 is the current route
// Generated by the Flight Director/PC when a flight plan is loaded
//
// id_set_route commands
//
// 0 = clear route
// 1 = activate route         copies route to route 0, then must send activate route 0
// 2 = invert route
#define id_set_route                  1140  // SHORT              service code 0=current plan, 1..n specific plan 
#define id_set_route_segments         1141  // SHORT              service code 0=current plan, 1..n specific plan, number of route segments (max 16 for route 0)
#define id_set_waypoint_identifier_0_3    1142  // ACHAR4        
#define id_set_waypoint_identifier_4_7    1143  // ACHAR4        
#define id_set_waypoint_identifier_8_11   1144  // ACHAR4        
#define id_set_waypoint_identifier_12_15  1145  // ACHAR4        
#define id_set_waypoint_type_identifier   1146  // SHORT          
#define id_set_waypoint_latitude          1146  // FLOAT      deg
#define id_set_waypoint_longitude         1148  // FLOAT      deg
#define id_set_waypoint_minimum_altitude  1149  // FLOAT      m
#define id_set_waypoint_maximum_altitude  1150  // FLOAT      m
  
#define id_navigation_mode                1250  // SHORT    0 = GPS 1 Navigation, 1 = GPS 2 Navligation, 2 = NAV 1,  3 = NAV 2
#define id_alternator_status              1251  // SHORT    0 = alternator failed, 1 = alternator ok
#define id_voltage_warning                1252  // SHORT    0 = voltage ok, 1 = low voltage
#define id_panel_brightness               1253  // SHORT    0-100 %
#define id_pitot_temperature_alarm        1254  // SHORT    0 = pitot temperature ok, 1 = temperature low, 2 = temperature high
#define id_nav_msg                        1256  // SHORT    0 = no message, 1 = msg displayed
#define id_nav_appr                       1257  // SHORT    0 = GPS not in approach mode, 1 = GPS in approach mode
#define id_nav_ils                        1258  // SHORT    0 = NAV receiver does not have ILD, 1 = NAV receiver has ILS detected
#define id_voltage_sense_1                1259  // FLOAT    0-16v
#define id_voltage_sense_2                1260  // FLOAT    0-16v
  

#define id_user_defined_start             1800
#define is_user_defined_end               1899
#endif

#include <stdint.h>

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

// Un-used
#define unused_id                     0

// Defined module ID's
#define ahrs_node_id                  128         // this allows for up to 8 AHRS units/bus
#define ahrs_node_id_last             135
#define edu_node_id                   136
#define edu_node_id_last              143
#define fdu_node_id                   144
#define fdu_node_id_last              151
#define aio_start_id                  152
#define aio_end_id                    159
#define efi_node_id                   160
#define efi_node_id_last              164
#define slcan_id                      192         // serial adapter for CanFLY
#define aacu_id                       193

// these are the default node settings

//  Datatype  Units  Notes
// AHRS generated data
#define id_pitch_acceleration         300   // FLOAT      g       forward: + aft: -
#define id_roll_acceleration          301   // FLOAT      g       right: + left: -
#define id_yaw_acceleration           302   // FLOAT      g       up: + down: -
#define id_pitch_rate                 303   // FLOAT      rad/s   nose up: + nose down: -
#define id_roll_rate                  304   // FLOAT      rad/s   roll right: + roll left: -
#define id_yaw_rate                   305   // FLOAT      rad/s   yaw right: + yaw left: -
#define id_roll_angle_magnetic        306   // FLOAT      rad
#define id_pitch_angle_magnetic       307   // FLOAT      rad
#define id_yaw_angle_magnetic         308   // FLOAT      rad
#define id_pitch_angle                311   // FLOAT      rad     nose up: + nose down: -
#define id_roll_angle                 312   // FLOAT      rad     roll right: + roll left: -
#define id_yaw_angle                  313   // FLOAT      rad     yaw right: + yaw left: -
#define id_altitude_rate              314   // FLOAT      m/s
#define id_indicated_airspeed         315   // FLOAT      m/s
#define id_true_airspeed              316   // FLOAT      m/s     
#define id_calibrated_airspeed        317   // FLOAT      m/s
#define id_angle_of_attack            318   // SHORT      deg
#define id_qnh                        319   // SHORT      hPa     published every 60 seconds.  changed with id_qnh_up, id_qnh_dn
#define id_baro_corrected_altitude    320   // FLOAT      m
#define id_heading_angle              321   // SHORT      deg     0-360   True heading
#define id_heading                    322   // SHORT      deg     0-360
#define id_pressure_altitude          323   // FLOAT      m
#define id_outside_air_temperature    324   // SHORT      K
#define id_differential_pressure      325   // FLOAT      hPa
#define id_static_pressure            326   // FLOAT      hPa
#define id_wind_speed                 333   // FLOAT      m/s
#define id_wind_direction             334   // float      rad     0-360
#define id_gps_aircraft_lattitude     1036  // FLOAT      deg
#define id_gps_aircraft_longitude     1037  // FLOAT      deg
#define id_gps_aircraft_height_above_ellipsoid 1038   // FLOAT      m
#define id_gps_groundspeed            1039  // FLOAT      m/s
#define id_true_track                 1040  // SHORT      deg
#define id_magnetic_track             1041  // SHORT      deg
#define id_magnetic_heading           1069  // SHORT      deg
// calculated from the INS
#define id_position_lattitude         1070  // FLOAT      rad
#define id_position_longitude         1071  // FLOAT      rad
#define id_position_altitude          1072  // FLOAT      m       true altitud
#define id_velocity_x                 1073  // FLOAT      m/s
#define id_velocity_y                 1074  // FLOAT      m/s
#define id_velocity_z                 1075  // FLOAT      m/s

#define id_magnetic_variation         1121  // SHORT      deg
#define id_def_utc                    1200  // CHAR4              format: 13h43min22s 13 43 22 00
#define id_def_date                   1201  // CHAR4              format: 12. June 1987 12 06 19 87
#define id_qnh_up                     1202  // SHORT              can increment by # of steps
#define id_qnh_dn                     1203  // SHORT
#define id_set_magnetic_variation     1204
#define id_heading_up                 1206
#define id_heading_dn                 1207
#define id_stall_warning              1211  // SHORT     Stall warning switch activated
#define id_total_time                 1212  // LONG      Total time in service  Hrs * 10
#define id_air_time                   1213  // LONG      Total time where IAS > 10 kts Hrs * 10
#define id_tach_time                  1214  // LONG      Total time where the engine is running Hrs * 10
#define id_pitch                      1217  // SHORT      deg    +/- 180
#define id_ahrs_status                1218  // SHORT     0 = ahrs initializing, 1 = ahrs waiting data, 2 = ahrs operational, -1 = ahrs fail

// Autopilot generated data.  In all cases the service code field contains waypoint # which can be 0..15
#define id_track                      1006  // float      radians   track over the ground (true)
#define id_deviation                  1007  // float      m         deviation from track in meters
#define id_track_angle_error          1008  // SHORT      deg
#define id_estimated_time_to_next     1009  // SHORT      min
#define id_estimated_time_of_arrival  1010  // SHORT      min
#define id_estimated_enroute_time     1011  // SHORT      min
#define id_waypoint_identifier_0_3    1012  // ACHAR4        
#define id_waypoint_identifier_4_7    1013  // ACHAR4        
#define id_waypoint_identifier_8_11   1014  // ACHAR4        
#define id_waypoint_identifier_12_15  1015  // ACHAR4        
#define id_waypoint_type_identifier   1016  // SHORT          
#define id_waypoint_latitude          1017  // FLOAT      deg
#define id_waypoint_longitude         1018  // FLOAT      deg
#define id_waypoint_minimum_altitude  1019  // FLOAT      m
#define id_waypoint_maximum_altitude  1023  // FLOAT      m
#define id_distance_to_next           1024  // float      m
#define id_distance_to_destination    1025  // float      m
#define id_selected_course            1125  // SHORT      deg
#define id_desired_track_angle        1135  // FLOAT      rad       magnetic angle to fly to get to destination
#define id_nav_command                1136  // SHORT      0 = clear route, 1 = activate route, 2 = deactivate_route, 3 = invert route
#define id_nav_valid                  1137  // SHORT      0 = nav data not valid, 1 = enroute data valid
#define id_fix_valid                  1138  // SHORT      0 = nav data not valid, 1 = enroute data valid
#define id_imu_valid                  1139  // SHORT      0 = no data, 1 = valid data
#define id_edu_valid                  1140  // SHORT      0 = error, 1 = valid
#define id_aux_battery                1141  // SHORT      0 = not on battery, 1 = on aux battery
#define id_trim_up                    1142  // SHORT      number of trim steps
#define id_trim_dn                    1143  // SHORT
#define id_trim_left                  1144  // SHORT
#define id_trim_right                 1145  // SHORT
#define id_autopilot_engage           1146  // SHORT      0 = off, 1 = on
#define id_autopilot_set_max_roll     1147  // SHORT      Maximum roll rate allowed
#define id_autopilot_set_vs_rate      1148  // SHORT      Set maximum pitch allowed
#define id_waypoint_turn_heading      1149  // SHORT      deg, heading to turn onto
#define id_roll_servo_set_position    1150  // SHORT      degrees position +/-90
#define id_pitch_servo_set_postion    1151  // SHORT      degrees position +/-90
#define id_autopilot_power            1152  // SHORT      autopilot power is on
#define id_autopilot_alt_mode         1153  // SHORT      0 = off, 1 = on  Altitude hold mode enable
#define id_autopilot_vs_mode          1154  // SHORT      0 = off, 1 = on  Vertical speed hold mode enable
#define id_autopilot_mode             1155  // SHORT      see enumeration below
#define id_autopilot_status           1156  // SHORT      bitmask, published every 5secs or on change

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

#define id_autopilot_altitude         1157  // SHORT      m assigned altitude
#define id_autopilot_vertical_speed   1158  // SHORT      m/s vertical rate of climb/descent

// EDU generated data
#define id_engine_rpm                 500   // SHORT      r/min
#define id_engine_rpm_a               501   // SHORT      r/min
#define id_exhaust_gas_temperature1   520   // SHORT      K
#define id_exhaust_gas_temperature2   521   // SHORT      K
#define id_exhaust_gas_temperature3   522   // SHORT      K
#define id_exhaust_gas_temperature4   523   // SHORT      K
#define id_manifold_pressure          528   // FLOAT      hPa
#define id_oil_pressure               532   // FLOAT      hPa
#define id_oil_temperature            536   // SHORT      K
#define id_cylinder_head_temperature1 548   // SHORT      K
#define id_cylinder_head_temperature2 549   // SHORT      K
#define id_cylinder_head_temperature3 550   // SHORT      K
#define id_cylinder_head_temperature4 551   // SHORT      K
#define id_engine_rpm_b               564   // SHORT      r/min
#define id_manifold_pressure_b        592   // FLOAT      hPa
#define id_fuel_pressure              684   // FLOAT      hPa
#define id_dc_voltage                 920   // SHORT      V
#define id_dc_current                 930   // SHORT      A
#define id_fuel_flow_rate             590   // SHORT      l/hr
#define id_fuel_consumed              591   // SHORT      l     Fuel consumed since power on
#define id_edu_left_fuel_quantity     666   // SHORT      l
#define id_edu_right_fuel_quantity    667   // SHORT      l

#define id_timing_divergence          2200  // SHORT      msec difference between left & right tach
#define id_left_mag_rpm               2201
#define id_right_mag_rpm              2202
#define id_left_mag_adv               2203
#define id_right_mag_adv              2204
#define id_left_mag_map               2205
#define id_right_mag_map              2206
#define id_left_mag_volt              2207
#define id_right_mag_volt             2208
#define id_left_mag_temp              2209
#define id_right_mag_temp             2210
#define id_left_mag_coil1             2211
#define id_right_mag_coil1            2212
#define id_left_mag_coil2             2213
#define id_right_mag_coil2            2214

// FGDU generated data
#define id_fuel_flow_rate_left        524   // SHORT      l/hr
#define id_fuel_flow_rate_right       588   // SHORT      l/hr
#define id_active_fuel_tank           591   // SHORT      1=left, 2=right, 3=both
#define id_left_fuel_quantity         668   // SHORT      l
#define id_right_fuel_quantity        669   // SHORT      l
#define id_pitot_temperature          670   // SHORT      degrees
#define id_pitot_heat_status          671   // SHORT      1 = heater on, 0 = off
#define id_pitot_power_status         672   // SHORT      1 = power on, 0 = off
#define id_fdu_board_temperature      673   // SHORT      degrees
#define id_set_left_fuel_qty          674   // SHORT      l
#define id_set_right_fuel_qty         675   // SHORT      l

// TODO: check these
// EFI definitions
#define id_mixture_lever             700    // SHORT    0-100%
#define id_injector_dwell            701    // SHORT    0-1024

// Propellor speed definitions
#define id_propeller_speed_lever      800   // SHORT      rpm
#define id_propeller_motor_current    801   // SHORT      A
#define id_propeller_low_speed        802   // SHORT      rpm
#define id_propeller_high_speed       803   // SHORT      rpm

// Definitions for a Rotax ECU, sends data on 2 channels at 125kHz

#define id_rotax_rpm_a                500
#define id_rotax_fuel_flow_a          524   // liter / hour
#define id_rotax_manifold_pressure_a  532   // hpa
#define id_rotax_oil_pressure_a       532   // hpa
#define id_rotax_oil_temperature_a    536   // kelvin
#define id_rotax_coolant_temp_a       548
#define id_rotax_egt1_a               628
#define id_rotax_egt2_a               630
#define id_rotax_egt3_a               632
#define id_rotax_egt4_a               634
#define id_rotax_intake_temperature_a 640
#define id_rotax_outside_air_temp_a   642
#define id_rotax_throttle_position_a  692
#define id_rotax_ambient_pressure_a   694
#define id_rotax_voltage_a            950
#define id_rotax_engine_status_a      556
#define id_rotax_engine_hours_a       1208
#define id_rotax_ecu_hours_a          1216
#define id_rotax_sensor_status_1_a    1500
#define id_rotax_sensor_status_2_a    1504
#define id_rotax_device_status_1_a    1508
#define id_rotax_device_status_2_a    1512

#define id_rotax_rpm_b                564
#define id_rotax_fuel_flow_b          588
#define id_rotax_manifold_pressure_b  592
#define id_rotax_oil_pressure_b       596
#define id_rotax_oil_temperature_b    600
#define id_rotax_coolant_temp_b       612
#define id_rotax_egt1_b               644
#define id_rotax_egt2_b               646
#define id_rotax_egt3_b               648
#define id_rotax_egt4_b               650
#define id_rotax_intake_temperature_b 656
#define id_rotax_outside_air_temp_b   658
#define id_rotax_throttle_position_b  696
#define id_rotax_ambient_pressure_b   698
#define id_rotax_voltage_b            954
#define id_rotax_engine_status_b      620
#define id_rotax_engine_hours_b       1212
#define id_rotax_ecu_hours_b          1220
#define id_rotax_sensor_status_1_b    1516
#define id_rotax_sensor_status_2_b    1520
#define id_rotax_device_status_1_b    1524
#define id_rotax_device_status_2_b    1528

// this is the Flight plan service.  It supports up to 255 routes.
// Route 0 is the current route
// Generated by the Flight Director/PC when a flight plan is loaded
//
// id_set_route commands
//
// 0 = clear route
// 1 = activate route         copies route to route 0, then must send activate route 0
// 2 = invert route
#define id_set_route                  1140  // SHORT              service code 0=current plan, 1..n specific plan 
#define id_set_route_segments         1141  // SHORT              service code 0=current plan, 1..n specific plan, number of route segments (max 16 for route 0)
#define id_set_waypoint_identifier_0_3    1142  // ACHAR4        
#define id_set_waypoint_identifier_4_7    1143  // ACHAR4        
#define id_set_waypoint_identifier_8_11   1144  // ACHAR4        
#define id_set_waypoint_identifier_12_15  1145  // ACHAR4        
#define id_set_waypoint_type_identifier   1146  // SHORT          
#define id_set_waypoint_latitude          1146  // FLOAT      deg
#define id_set_waypoint_longitude         1148  // FLOAT      deg
#define id_set_waypoint_minimum_altitude  1149  // FLOAT      m
#define id_set_waypoint_maximum_altitude  1150  // FLOAT      m
  
#define id_navigation_mode                1250  // SHORT    0 = GPS 1 Navigation, 1 = GPS 2 Navligation, 2 = NAV 1,  3 = NAV 2
#define id_alternator_status              1251  // SHORT    0 = alternator failed, 1 = alternator ok
#define id_voltage_warning                1252  // SHORT    0 = voltage ok, 1 = low voltage
#define id_panel_brightness               1253  // SHORT    0-100 %
#define id_pitot_temperature_alarm        1254  // SHORT    0 = pitot temperature ok, 1 = temperature low, 2 = temperature high
#define id_nav_msg                        1256  // SHORT    0 = no message, 1 = msg displayed
#define id_nav_appr                       1257  // SHORT    0 = GPS not in approach mode, 1 = GPS in approach mode
#define id_nav_ils                        1258  // SHORT    0 = NAV receiver does not have ILD, 1 = NAV receiver has ILS detected
#define id_voltage_sense_1                1259  // FLOAT    0-16v
#define id_voltage_sense_2                1260  // FLOAT    0-16v
  

#define id_user_defined_start             1800
#define is_user_defined_end               1899
#endif
