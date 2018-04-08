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
#ifndef __console_h__
#define __console_h__

#include <stdint.h>

/**
 * Init the console subsytem
 * @return >= 0 if ok
 */
extern int init_console();
/**
 * Create a channel
 * @param channel_id  Channel number
 * @return 0 if created ok, <0 if error
 */
extern int create_console(uint16_t channel_id);
/**
 * Close a channel and release all resources
 * @param channel_id  Channel to close
 * @return
 */
extern int close_console(uint16_t channel_id);
/**
 * Send a character to a channel
 * @param channel_id
 * @param ch
 * @return
 */
extern int send_console(uint16_t channel_id, char ch);
/**
 * Receive a character from a console
 * @param channel_id    Channel to read from
 * @param ch            character from channel
 * @return 0 = ok, <0 no data available
 */
extern int receive_console(uint16_t channel_id, char *ch, uint16_t len, uint16_t *chars_read);

#endif
