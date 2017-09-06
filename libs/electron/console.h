#ifndef __console_h__
#define __console_h__

#include <stdint.h>

/**
 * Init the console subsytem
 * @return
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
