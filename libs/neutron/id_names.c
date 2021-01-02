#include "neutron.h"

static const char* id_names_300_326[] =
  {
  "id_pitch_acceleration",
  "id_roll_acceleration",
  "id_yaw_acceleration",
  "id_pitch_rate",
  "id_roll_rate",
  "id_yaw_rate",
  "id_roll_angle_magnetic",
  "id_pitch_angle_magnetic",
  "id_yaw_angle_magnetic",
  0,
  0,
  "id_pitch_angle",
  "id_roll_angle",
  "id_yaw_angle",
  "id_altitude_rate",
  "id_indicated_airspeed",
  "id_true_airspeed",
  "id_calibrated_airspeed",
  "id_angle_of_attack",
  "id_qnh",
  "id_baro_corrected_altitude",
  "id_heading_angle",
  "id_heading",
  "id_pressure_altitude",
  "id_outside_air_temperature",
  "id_differential_pressure",
  "id_static_pressure",
  0,
  0,
  0,
  0,
  0,
  0,
  "id_wind_speed",
  "id_wind_direction",
  };

static const char* id_names_500_711[] =
  {
  "id_engine_rpm",
  "id_engine_rpm_a",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_exhaust_gas_temperature1",
  "id_exhaust_gas_temperature2",
  "id_exhaust_gas_temperature3",
  "id_exhaust_gas_temperature4",
  "id_fuel_flow_rate_left",
  0,
  0,
  0,
  "id_manifold_pressure",
  0,
  "id_manifold_pressure_a",
  0,
  "id_oil_pressure",
  0,
  0,
  0,
  "id_oil_temperature",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_cylinder_head_temperature1",
  "id_cylinder_head_temperature2",
  "id_cylinder_head_temperature3",
  "id_cylinder_head_temperature4",
  0,
  0,
  0,
  0,
  "id_engine_status_a",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_engine_rpm_b",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_fuel_flow_rate_right",
  0,
  "id_fuel_flow_rate",
  "id_fuel_consumed",
  "id_manifold_pressure_b",
  0,
  0,
  0,
  "id_oil_pressure_b",
  0,
  0,
  0,
  "id_oil_temperature_b",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_coolant_temp_b",
  "id_cylinder_head_temperature5",
  "id_cylinder_head_temperature6",
  "id_cylinder_head_temperature7",
  "id_cylinder_head_temperature8",
  0,
  0,
  0,
  0,
  "id_engine_status_b",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_egt1_a",
  0,
  "id_egt2_a",
  0,
  "id_egt3_a",
  0,
  "id_egt4_a",
  0,
  0,
  0,
  0,
  0,
  "id_intake_temperature_a",
  0,
  "id_outside_air_temp_a",
  0,
  "id_egt1_b",
  0,
  "id_egt2_b",
  0,
  "id_egt3_b",
  0,
  "id_egt4_b",
  0,
  0,
  0,
  0,
  0,
  "id_intake_temperature_b",
  0,
  "id_outside_air_temp_b",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_edu_left_fuel_quantity",
  "id_edu_right_fuel_quantity",
  "id_left_fuel_quantity",
  "id_right_fuel_quantity",
  "id_pitot_temperature",
  "id_pitot_heat_status",
  "id_pitot_power_status",
  "id_fdu_board_temperature",
  "id_set_left_fuel_qty",
  "id_set_right_fuel_qty",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_fuel_pressure",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_throttle_position_a",
  0,
  "id_ambient_pressure_a",
  0,
  "id_throttle_position_b",
  0,
  "id_ambient_pressure_b",
  0,
  "id_mixture_lever",
  "id_mixture_lever_a",
  "id_mixture_lever_b",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_aux1_fuel_quantity",
  "id_aux2_fuel_quantity",
  };

static const char* id_names_800_803[] =
  {
  "id_propeller_speed_lever ",
  "id_propeller_motor_current ",
  "id_propeller_low_speed ",
  "id_propeller_high_speed ",
  };

static const char* id_names_920_930[] =
  {
  "id_dc_voltage ",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_dc_current ",
  };

static const char* id_names_950_954[] =
  {
  "id_voltage_a ",
  0,
  0,
  0,
  "id_voltage_b ",
  };

static const char* id_names_1006_1025[] =
  {
  "id_voltage_b ",
  "id_track ",
  "id_deviation ",
  "id_track_angle_error ",
  "id_estimated_time_to_next ",
  "id_estimated_time_of_arrival ",
  "id_estimated_enroute_time ",
  "id_waypoint_identifier_0_3 ",
  "id_waypoint_identifier_4_7 ",
  "id_waypoint_identifier_8_11 ",
  "id_waypoint_identifier_12_15 ",
  "id_waypoint_type_identifier ",
  "id_waypoint_latitude ",
  "id_waypoint_longitude ",
  "id_waypoint_minimum_altitude ",
  0,
  0,
  0,
  "id_waypoint_maximum_altitude ",
  "id_distance_to_next ",
  "id_distance_to_destination ",
  };

static const char* id_names_1036_1041[] =
  {
  "id_gps_latitude ",
  "id_gps_longitude ",
  "id_gps_height ",
  "id_gps_groundspeed ",
  "id_true_track ",
  "id_magnetic_track ",
  };

static const char* id_names_1069_1084[] =
  {
  "id_magnetic_heading ",
  "id_position_latitude ",
  "id_position_longitude ",
  "id_position_altitude ",
  "id_velocity_x ",
  "id_velocity_y ",
  "id_velocity_z ",
  "id_imu_mag_x ",
  "id_imu_mag_y ",
  "id_imu_mag_z ",
  "id_imu_accel_x ",
  "id_imu_accel_y ",
  "id_imu_accel_z ",
  "id_imu_gyro_x ",
  "id_imu_gyro_y ",
  "id_imu_gyro_z ",
  };

static const char* id_names_1120_1220[] =
  {
  "id_imu_gyro_z ",
  0,
  "id_magnetic_variation ",
  0,
  0,
  0,
  "id_selected_course ",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_desired_track_angle ",
  "id_nav_command ",
  "id_nav_valid ",
  "id_gps_valid ",
  0,
  0,
  "id_set_route_segments ",
  "id_set_waypoint_identifier_0_3 ",
  "id_set_waypoint_identifier_4_7 ",
  "id_set_waypoint_identifier_8_11 ",
  "id_set_waypoint_identifier_12_15 ",
  "id_set_waypoint_type_identifier ",
  "id_set_waypoint_latitude ",
  "id_set_waypoint_longitude ",
  "id_waypoint_turn_heading ",
  "id_set_waypoint_minimum_altitude ",
  "id_set_waypoint_maximum_altitude ",
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  "id_trim_up ",
  "id_trim_dn ",
  "id_trim_left ",
  "id_trim_right ",
  0,
  0,
  0,
  0,
  0,
  0,
  "id_autopilot_engage ",
  "id_autopilot_set_max_roll ",
  "id_autopilot_set_vs_rate ",
  "id_autopilot_power ",
  "id_autopilot_alt_mode ",
  "id_autopilot_vs_mode ",
  "id_autopilot_mode ",
  "id_autopilot_status ",
  "id_autopilot_altitude ",
  "id_autopilot_vertical_speed ",
  0,
  "id_roll_servo_status ",
  "id_pitch_servo_status ",
  "id_roll_servo_set_position ",
  "id_pitch_servo_set_postion ",
  "id_imu_valid ",
  "id_edu_valid ",
  "id_set_route ",
  "id_aux_battery ",
  0,
  0,
  0,
  0,
  0,
  0,
  "id_def_utc ",
  "id_def_date ",
  "id_qnh_up ",
  "id_qnh_dn ",
  "id_set_magnetic_variation ",
  "id_heading_up ",
  "id_heading_dn ",
  "id_engine_hours_a ",
  0,
  0,
  "id_stall_warning ",
  "id_total_time ",
  "id_engine_hours_b ",
  "id_air_time ",
  "id_tach_time ",
  "id_ecu_hours_a ",
  "id_pitch ",
  0,
  0,
  "id_ecu_hours_b ",
  };

static const char* id_names_1250_1260[] =
  {
  "id_ecu_hours_b ",
  "id_navigation_mode ",
  "id_alternator_status ",
  "id_voltage_warning ",
  "id_panel_brightness ",
  "id_pitot_temperature_alarm ",
  "id_nav_msg ",
  "id_nav_appr ",
  "id_nav_ils ",
  "id_voltage_sense_1 ",
  "id_voltage_sense_2 ",
  };

static const char* id_names_1500_1528[] =
  {
  "id_sensor_status_1_a ",
  0,
  0,
  0,
  "id_sensor_status_2_a ",
  0,
  0,
  0,
  "id_device_status_1_a ",
  0,
  0,
  0,
  "id_device_status_2_a ",
  0,
  0,
  0,
  "id_sensor_status_1_b ",
  0,
  0,
  0,
  "id_sensor_status_2_b ",
  0,
  0,
  0,
  "id_device_status_1_b ",
  0,
  0,
  0,
  "id_device_status_2_b ",
  };

static const char* id_names_2200_2214[] =
  {
  "id_timing_divergence ",
  "id_left_mag_rpm ",
  "id_right_mag_rpm ",
  "id_left_mag_adv ",
  "id_right_mag_adv ",
  "id_left_mag_map ",
  "id_right_mag_map ",
  "id_left_mag_volt ",
  "id_right_mag_volt ",
  "id_left_mag_temp ",
  "id_right_mag_temp ",
  "id_left_mag_coil1 ",
  "id_right_mag_coil1 ",
  "id_left_mag_coil2 ",
  "id_right_mag_coil2 ",
  };

typedef struct _name_ranges_t {
  uint16_t from;
  uint16_t to;
  uint16_t count;
  const char** names;
  } name_ranges_t;

static const name_ranges_t name_ranges[] =
  {
    { 300, 326, numelements(id_names_300_326), id_names_300_326 },
    { 500, 711, numelements(id_names_500_711), id_names_500_711 },
    { 800, 803, numelements(id_names_800_803), id_names_800_803 },
    { 920, 930, numelements(id_names_920_930), id_names_920_930 },
    { 950, 954, numelements(id_names_950_954), id_names_950_954 },
    { 1006, 1025, numelements(id_names_1006_1025), id_names_1006_1025 },
    { 1036, 1042, numelements(id_names_1036_1041), id_names_1036_1041 },
    { 1069, 1084, numelements(id_names_1069_1084), id_names_1069_1084 },
    { 1120, 1220, numelements(id_names_1120_1220), id_names_1120_1220 },
    { 1250, 1260, numelements(id_names_1250_1260), id_names_1250_1260 },
    { 1500, 1528, numelements(id_names_1500_1528), id_names_1500_1528 },
    { 2200, 2214, numelements(id_names_2200_2214), id_names_2200_2214 },
    { 0, 0, 0}
  };

const char* get_name_from_id(uint16_t id)
  {
  const name_ranges_t* range = name_ranges;
  while (range->names != 0)
    if (id >= range->from && id <= range->to)
      {
      uint16_t index = id - range->from;
      if (index > range->count)
        return 0;             // this is a data entry problem...
      return range->names[id - range->from];
      }

  return 0;
  }

uint16_t get_id_from_name(const char* name)
  {
  if (name == 0 || strlen(name) == 0)
    return 0;

  const name_ranges_t* range = name_ranges;
  while (range->names != 0)
    {
    for (uint16_t i = 0; i < range->count; i++)
      {
      if (range->names[i] == 0)
        continue;

      if (strcmp(range->names[i], name) == 0)
        return range->from + i;
      }
    }

  return 0;
  }

const char* get_datatype_name(uint8_t data_type)
  {
  if (data_type == CANAS_DATATYPE_NODATA)
    return "NODATA";
  if (data_type == CANAS_DATATYPE_ERROR)
    return "ERROR";
  if (data_type == CANAS_DATATYPE_FLOAT)
    return "float";
  if (data_type == CANAS_DATATYPE_INT32)
    return "Int32";
  if (data_type == CANAS_DATATYPE_UINT32)
    return "UInt32";
  if (data_type == CANAS_DATATYPE_SHORT)
    return "Int16";
  if (data_type == CANAS_DATATYPE_USHORT)
    return "UInt16";
  if (data_type == CANAS_DATATYPE_CHAR)
    return "Int8";
  if (data_type == CANAS_DATATYPE_UCHAR)
    return "UInt8";
  if (data_type == CANAS_DATATYPE_SHORT2)
    return "Int16[2]";
  if (data_type == CANAS_DATATYPE_USHORT2)
    return "UInt16[2]";
  if (data_type == CANAS_DATATYPE_CHAR4)
    return "Int8[4]";
  if (data_type == CANAS_DATATYPE_UCHAR4)
    return "UInt8[4]";
  if (data_type == CANAS_DATATYPE_CHAR2)
    return "Int8[2]";
  if (data_type == CANAS_DATATYPE_UCHAR2)
    return "UInt8[2]";
  if (data_type == CANAS_DATATYPE_CHAR3)
    return "Int8[3]";
  if (data_type == CANAS_DATATYPE_UCHAR3)
    return "UInt8[3]";

  if (data_type >= CANAS_DATATYPE_RESVD_BEGIN &&
    data_type <= CANAS_DATATYPE_RESVD_END)
    return "RESERVED";

  if (data_type >= CANAS_DATATYPE_UDEF_BEGIN &&
    data_type <= CANAS_DATATYPE_UDEF_END)
    return "USER_DEFINED";

  return "UNKNOWN";
  }
