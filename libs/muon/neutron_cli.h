#ifndef __neutron_cli_h__
#define __neutron_cli_h__
/* This is an auto-generated file by cli */
#include "muon.h"

#ifdef __cplusplus
extern "C" {
#endif

extern cli_node_t neutron_cli_root;

extern result_t uint16_name_value_action(cli_t *context, const char * uint16_name_, uint16_t uint16_name_value_);

extern result_t int16_name_value_action(cli_t *context, const char * int16_name_, int16_t int16_name_value_);

extern result_t uint32_name_value_action(cli_t *context, const char * uint32_name_, uint32_t uint32_name_value_);

extern result_t int32_name_value_action(cli_t *context, const char * int32_name_, int32_t int32_name_value_);

extern result_t xyz_name_value_action(cli_t *context, const char * xyz_name_, xyz_t *xyz_name_value_);

extern result_t matrix_name_value_action(cli_t *context, const char * matrix_name_, matrix_t *matrix_name_value_);

extern result_t string_name_value_action(cli_t *context, const char * string_name_, const char * string_name_value_);

extern result_t stream_name_value_action(cli_t *context, const char * stream_name_, const char * stream_name_value_);

extern result_t bool_name_value_action(cli_t *context, const char * bool_name_, uint16_t bool_name_value_);

extern const enum_t booleans[];

extern result_t float_name_value_action(cli_t *context, const char * float_name_, float float_name_value_);

extern result_t edit_name_action(cli_t *context, const char * edit_name_);

extern result_t cat_name_action(cli_t *context, const char * cat_name_);

extern result_t rm_name_action(cli_t *context, const char * rm_name_);

extern result_t mkdir_path_action(cli_t *context, const char * mkdir_path_);

extern result_t rmdir_path_action(cli_t *context, const char * rmdir_path_);

extern result_t cd_path_action(cli_t *context, const char * cd_path_);

extern result_t ls_path_recursive_action(cli_t *context, const char * ls_path_);

extern result_t neutron_ls_id_action(cli_t *context, uint16_t *neutron_ls_id_);

extern const enum_t can_ids[];

extern result_t neutron_rm_id_action(cli_t *context, uint16_t neutron_rm_id_);

extern result_t neutron_publish_id_rate_rate_action(cli_t *context, uint16_t neutron_publish_id_rate_rate_);

extern result_t neutron_publish_id_type_type_action(cli_t *context, uint16_t neutron_publish_id_type_type_);

extern const enum_t publish_types[];

extern result_t neutron_publish_id_sample_sample_action(cli_t *context, uint16_t neutron_publish_id_sample_sample_);

extern const enum_t sample_types[];

extern result_t neutron_publish_id_publish_is_published_action(cli_t *context, bool neutron_publish_id_publish_is_published_);

extern result_t neutron_publish_id_loopback_loopback_action(cli_t *context, bool neutron_publish_id_loopback_loopback_);

extern result_t neutron_publish_id_filter_filter_type_length_value_action(cli_t *context, uint16_t neutron_publish_id_filter_filter_type_length_value_);

extern result_t neutron_publish_id_filter_filter_type_coeff_index_value_action(cli_t *context, uint16_t neutron_publish_id_filter_filter_type_coeff_index_, float neutron_publish_id_filter_filter_type_coeff_index_value_);

extern result_t neutron_publish_id_filter_filter_type_gain_value_action(cli_t *context, float neutron_publish_id_filter_filter_type_gain_value_);

extern result_t neutron_publish_id_filter_filter_type_exit_action(cli_t *context);

extern result_t neutron_publish_id_filter_filter_type_action(cli_t *context, uint16_t neutron_publish_id_filter_filter_type_);

extern const enum_t filter_types[];

extern result_t neutron_publish_id_alarm_alarm_id_type_alarm_type_action(cli_t *context, uint16_t neutron_publish_id_alarm_alarm_id_type_alarm_type_);

extern const enum_t alarm_types[];

extern result_t neutron_publish_id_alarm_alarm_id_period_length_action(cli_t *context, uint16_t neutron_publish_id_alarm_alarm_id_period_length_);

extern result_t neutron_publish_id_alarm_alarm_id_max_max_value_action(cli_t *context, float neutron_publish_id_alarm_alarm_id_max_max_value_);

extern result_t neutron_publish_id_alarm_alarm_id_min_min_value_action(cli_t *context, float neutron_publish_id_alarm_alarm_id_min_min_value_);

extern result_t neutron_publish_id_alarm_alarm_id_reset_reset_id_action(cli_t *context, uint16_t neutron_publish_id_alarm_alarm_id_reset_reset_id_);

extern result_t neutron_publish_id_alarm_alarm_id_exit_action(cli_t *context);

extern result_t neutron_publish_id_alarm_alarm_id_action(cli_t *context, uint16_t neutron_publish_id_alarm_alarm_id_);

extern result_t neutron_publish_id_exit_action(cli_t *context);

extern result_t neutron_publish_id_action(cli_t *context, uint16_t neutron_publish_id_);

extern result_t neutron_exit_action(cli_t *context);

extern result_t neutron_action(cli_t *context);

extern result_t send_can_id_type_session_val1_val2_val3_val4_action(cli_t *context, uint16_t send_can_id_, uint16_t send_can_id_type_, uint16_t send_can_id_type_session_, const char * send_can_id_type_session_val1_, const char * send_can_id_type_session_val1_val2_, const char * send_can_id_type_session_val1_val2_val3_, const char * send_can_id_type_session_val1_val2_val3_val4_);

extern const enum_t can_types[];

#ifdef __cplusplus
}
#endif
#endif
