#include "../../libs/neutron/neutron.h"
#include "../../libs/electron/console.h"
#include "diyefis_cli.h"

/*  Neutron shell service for diy-efis
 *
 * creates a linux shell when a id_shl_service message arrives
 */

typedef struct _channel_t {
  cli_t *context;
  uint16_t channel_id;
  } channel_t;

#define CONSOLE_BUFFER_LEN 128

static void receive_worker(void *parg)
  {
  channel_t *channel = (channel_t *)parg;

  char buffer[CONSOLE_BUFFER_LEN];
  uint16_t chars_read = 0;
  while (receive_console(channel->channel_id, buffer, CONSOLE_BUFFER_LEN, &chars_read) >= 0)
    {
    if (failed(stream_write(channel->context->cfg.console_out, buffer, chars_read)))
      break;
    }
  // otherwise we are done.
  }

result_t shell_action(cli_t *context)
  {
  // get the stream
  result_t result;

  channel_t channel;
  channel.context = context;

  if (create_console(0) < 0)
    return e_unexpected;

  channel.channel_id = 0;

  if (failed(result = task_create("CONSOLE", DEFAULT_STACK_SIZE, receive_worker, &channel, NORMAL_PRIORITY, 0)))
    return result;

  char buffer[CONSOLE_BUFFER_LEN];

  do
    {
    if (failed(stream_gets(context->cfg.console_in, buffer, CONSOLE_BUFFER_LEN)))
      break;

    char *str = buffer;
    while (str != 0 && *str)
      {
      if (send_console(0, *str) < 0)
        {
        str = 0;
        break;
        }

      str++;
      }

    if (str == 0)
      break;
    } while (true);

  return s_ok;
  }