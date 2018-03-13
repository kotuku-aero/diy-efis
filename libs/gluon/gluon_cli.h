#ifndef __gluon_cli_h__
#define __gluon_cli_h__
/* This is an auto-generated file by cli */
#include "../muon/muon.h"

#ifdef __cplusplus
extern "C" {
#endif

extern cli_node_t gluon_cli_root;

extern result_t gluon_ls_name_action(cli_t *context, const char * gluon_ls_name_);

extern result_t gluon_rm_name_action(cli_t *context, const char * gluon_rm_name_);

extern result_t gluon_capture_name_rate_rate_action(cli_t *context, uint16_t gluon_capture_name_rate_rate_);

extern result_t gluon_capture_name_length_length_action(cli_t *context, uint32_t gluon_capture_name_length_length_);

extern result_t gluon_capture_name_trigger_eq_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_eq_id_, int16_t gluon_capture_name_trigger_eq_id_value_);

extern const enum_t can_ids[];

extern result_t gluon_capture_name_trigger_lt_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_lt_id_, int16_t gluon_capture_name_trigger_lt_id_value_);

extern result_t gluon_capture_name_trigger_gt_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_gt_id_, int16_t gluon_capture_name_trigger_gt_id_value_);

extern result_t gluon_capture_name_trigger_eqf_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_eqf_id_, float gluon_capture_name_trigger_eqf_id_value_);

extern result_t gluon_capture_name_trigger_ltf_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_ltf_id_, float gluon_capture_name_trigger_ltf_id_value_);

extern result_t gluon_capture_name_trigger_gtf_id_value_action(cli_t *context, uint16_t gluon_capture_name_trigger_gtf_id_, float gluon_capture_name_trigger_gtf_id_value_);

extern result_t gluon_capture_name_trigger_rm_eq_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_eq_id_);

extern result_t gluon_capture_name_trigger_rm_lt_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_lt_id_);

extern result_t gluon_capture_name_trigger_rm_gt_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_gt_id_);

extern result_t gluon_capture_name_trigger_rm_eqf_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_eqf_id_);

extern result_t gluon_capture_name_trigger_rm_ltf_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_ltf_id_);

extern result_t gluon_capture_name_trigger_rm_gtf_id_action(cli_t *context, uint16_t gluon_capture_name_trigger_rm_gtf_id_);

extern result_t gluon_capture_name_trigger_exit_action(cli_t *context);

extern result_t gluon_capture_name_trigger_action(cli_t *context);

extern result_t gluon_capture_name_ls_action(cli_t *context);

extern result_t gluon_capture_name_id_id_name_action(cli_t *context, uint16_t gluon_capture_name_id_id_, const char * gluon_capture_name_id_id_name_);

extern result_t gluon_capture_name_rm_id_action(cli_t *context, uint16_t gluon_capture_name_rm_id_);

extern result_t gluon_capture_name_exit_action(cli_t *context);

extern result_t gluon_capture_name_action(cli_t *context, const char * gluon_capture_name_);

extern result_t gluon_exit_action(cli_t *context);

extern result_t gluon_action(cli_t *context);

#ifdef __cplusplus
}
#endif
#endif
