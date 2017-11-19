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
