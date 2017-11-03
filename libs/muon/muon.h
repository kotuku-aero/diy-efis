/* muon external interface. This should be the only header you need to use if
 * you're using muon as a library. Internal details are in interpreter.h */
#ifndef __muon_h__
#define __muon_h__

/* muon version number */
#ifdef VER
#define MUON_VERSION "v2.2 beta r" VER         /* VER is the subversion version number, obtained via the Makefile */
#else
#define MUON_VERSION "v2.2"
#endif

#include "../neutron/neutron.h"
#include "cli.h"


/*
 * Typical usage of muon to process a can_message
 * 
 * result_t process_msg(const canmsg_t *msg)
 * {
 * const char *function;
 * 
 * // function is loaded from store:
 * 
 * any_value_t val;
 * val.Pointer = &msg->msg;
 * 
 * muon_t muon;
 * muon_initialise(&muon, 256, 0, 0, 0);
 * muon_include_all_system_headers(&muon);
 * muon_parse(&muon, "script", function, false, false, false, false);
 * muon_call_fn(&muon, "ev_msg", 1, &val);
 * muon_cleanup(&muon);
 * 
 * return s_ok;
 * }
 */

/**
 * Initialize the CLI parser.  Hooks the message queue
 * @return s_ok if hook installed ok
 */
extern result_t muon_initialize_cli(cli_node_t *cli_root);
/**
 * Edit a stream
 * @param stdin   stream to read console from
 * @param stdout  stream to write console to
 * @param title   editor title
 * @param stream  stream to read/write file to
 */
result_t muon_edit(handle_t ci, handle_t co, const char *title, handle_t stream);

#endif /* PICOC_H */
