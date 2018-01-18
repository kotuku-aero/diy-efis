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
// Basic can aerospace framework support files
#include "../../libs/electron/electron.h"
#include "../../libs/proton/proton.h"
#include "../../libs/muon/muon.h"
#include "../../libs/neutron/stream.h"
#include "../../libs/neutron/slcan.h"

#include "../../libs/ion/interpreter.h"
#include "msh_cli.h"

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef __linux__
#include <termios.h>
#include <sys/stat.h>
#else
#include <Windows.h>
#endif

#ifndef __linux__

#pragma comment(lib, "muon")
#pragma comment(lib, "neutron")
#pragma comment(lib, "krypton")

#endif

extern cli_node_t cli_root;

#define CAN_TX_BUFFER_LEN 1024
#define CAN_RX_BUFFER_LEN 1024

const char *node_name = "muon";
extern const char *splash_base64;     // 320x240

static FILE *ci;
static FILE *co;

typedef struct _service_channel_t
  {
  stream_handle_t stream;
  // this MUST be first so we can cast a parser to a channel !!!
  // parser handling the commands
  cli_t parser;
  } service_channel_t;


  static result_t css_stream_eof(stream_handle_t *hndl)
    {
    return feof(ci) ? s_ok : s_false;
    }

static HANDLE clipdata = 0;
static int offset = 0;

static result_t css_read_clipboard(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  int16_t bytesRead = 0;
  char *str = (char *)buffer;
  LPSTR hglb = GlobalLock(clipdata);

  int len = strlen(hglb);

  while (size > 0)
    {
    size--;
    if (offset >= len ||
        hglb[offset] == 0)
      {
      offset = -1;
      break;
      }

    *str = hglb[offset++];

    if(*str == '\r')
      {
      size++;
      continue;
      }

    str++;
    bytesRead++;
    }

  GlobalUnlock(clipdata);

  if (offset == -1)
    {
    CloseClipboard(GetShellWindow());
    clipdata = 0;
    }

  if (read != 0)
    *read = (uint16_t)bytesRead;

  if(bytesRead == 0)
    return e_no_more_information;

  return s_ok;
  }

  static result_t css_stream_read(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
    {
    if(hndl == 0 || buffer == 0 || size == 0)
      return e_bad_parameter;

#ifdef __linux__
    char *buf = (char *)buffer;
    int ch;
    fflush(ci);
    fflush(co);

    while(size-- > 0)
      {
      ch = getc(ci);
      if(ch == EOF)
        break;

      if(ch == '\r')
        ch= '\n';

      *buf++ = ch;
      }

    if(read != 0)
      *read = 1;
#else
    DWORD bytesRead = 0;
    char *str = (char *)buffer;

    if(clipdata != 0 && succeeded(css_read_clipboard(hndl, buffer, size, read)))
      return s_ok;

    if(size > 0 )
      ReadFile(ci, buffer, size, &bytesRead, NULL);

    if (read != 0)
      *read = (uint16_t)bytesRead;

    while (bytesRead--)
      {
      size--;
      switch(*str)
        {
        case '\r' :
          *str = '\n';
          break;
        case  0x16 :  // windows CTRL-V
          if (clipdata == NULL)
            {
            if(!OpenClipboard(GetShellWindow()))
              continue;
            clipdata = GetClipboardData(CF_TEXT);
            offset = 0;
            size++;
            return css_read_clipboard(hndl, str, size, read);
            }

          break;
        }
      }
      //if (*str == '\r')
      //  *str = '\n';
#endif

    return s_ok;
    }

  static result_t css_stream_write(stream_handle_t *hndl, const void *buffer, uint16_t size)
    {
    if(hndl == 0 || buffer == 0 || size == 0)
      return e_bad_parameter;
#ifdef __linux__

    const char *buf = buffer;
    while(size-- > 0)
      putc(*buf++, co);

    fflush(co);
#else
    WriteFile(co, buffer, size, NULL, NULL);
#endif

    return s_ok;
    }

static result_t css_stream_close(stream_handle_t *hndl)
  {
  return s_ok;
  }

static void fb_sync(void *parg)
  {
  semaphore_p sync;
  semaphore_create(&sync);

  while(true)
    {
    // this simulates the hardware.  We only sync every 500msec
    // as this is a test harness
    semaphore_wait(sync, 500);
    bsp_sync();
    }
  }


int main(int argc, char **argv)
  {
	// The command line can pass in the name of the registry used to set us up.  In any
  // case we need to implement some code
  const char *ini_path;
  if(argc > 1)
    ini_path = argv[1];
  else
    ini_path = "diy-efis.reg";


  // TODO: handle this better
  bool factory_reset = false;

  if(failed(electron_init(ini_path, factory_reset)))
    {
    printf("Unable to initialize the krypton library.");
    return -1;
    }

  uint16_t node_id;
  if(failed(reg_get_uint16(0, "node-id", &node_id)))
    node_id = mfd_node_id;

  neutron_parameters_t init_params;
  init_params.node_id = (uint8_t) node_id;
  init_params.node_type = unit_pi;
  init_params.hardware_revision = 0x11;
  init_params.software_revision = 0x10;
  init_params.tx_length = CAN_TX_BUFFER_LEN;
  init_params.tx_stack_length = 4096;
  init_params.rx_length = CAN_RX_BUFFER_LEN;
  init_params.rx_stack_length = 4096;
  init_params.publisher_stack_length = 4096;

#ifdef __linux__
    struct termios tio;
    tcgetattr(0, &tio);

    //
    // Input flags - Turn off input processing
    // convert break to null byte, no CR to NL translation,
    // no NL to CR translation, don't mark parity errors or breaks
    // no input parity check, don't strip high bit off,
    // no XON/XOFF software flow control
    //
    tio.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    //
    // Output flags - Turn off output processing
    // no CR to NL translation, no NL to CR-NL translation,
    // no NL to CR translation, no column 0 CR suppression,
    // no Ctrl-D suppression, no fill characters, no case mapping,
    // no local output processing
    //
    tio.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);
    //
    // No line processing:
    // echo off, echo newline off, canonical mode off,
    // extended input processing off, signal chars off
    //
    tio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    //
    // Turn off character processing
    // clear current char size mask, no parity checking,
    // no output processing, force 8 bit input
    //
    tio.c_cflag &= ~(CSIZE | PARENB);
    tio.c_cflag |= CS8;
    //
    // One input byte is enough to return from read()
    // Inter-character timer off
    //
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
    //tio.c_cc[VEOL] = '\r';

    tio.c_cflag |= CREAD;
    speed_t speed  = B115200;

    cfsetospeed(&tio, speed);
    cfsetispeed(&tio, speed);

    tcflush(0, TCIFLUSH);

    tcsetattr(0, TCSANOW, &tio);

    ci = stdin;
    co = stdout;
 #else
  ci = GetStdHandle(STD_INPUT_HANDLE);
  DWORD dwMode;
  GetConsoleMode(ci, &dwMode);
  dwMode &= ~ENABLE_LINE_INPUT;
  dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
  SetConsoleMode(ci, dwMode);

  co = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleMode(co, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(co, dwMode);
#endif

  service_channel_t channel;
  memset(&channel, 0, sizeof(service_channel_t));

  channel.stream.version = sizeof(service_channel_t);
  channel.stream.stream_eof = css_stream_eof;
  channel.stream.stream_read = css_stream_read;
  channel.stream.stream_write = css_stream_write;
  channel.stream.stream_close = css_stream_close;

  channel.parser.cfg.root = &msh_cli_root;
  channel.parser.cfg.ch_complete = '\t';
  channel.parser.cfg.ch_erase = '\b';
  channel.parser.cfg.ch_del = 127;
  channel.parser.cfg.ch_help = '?';
  channel.parser.cfg.flags = 0;
  channel.parser.cfg.prompt = node_name;

  channel.parser.cfg.console_in = &channel.stream;
  channel.parser.cfg.console_out = &channel.stream;
  channel.parser.cfg.console_err = &channel.stream;

  proton_args_t args;

  ion_init();

  // start proton if the key exists
  memid_t proton_key;
  if(succeeded(reg_open_key(0, "proton", &proton_key)))
    {
    manifest_create(splash_base64, &args.stream);

    args.ci = &channel.stream;
    args.co = &channel.stream;
    args.cerr = &channel.stream;

    task_create("PROTON", DEFAULT_STACK_SIZE * 4, run_proton, &args, NORMAL_PRIORITY, 0);
    }

  if (failed(cli_init(&channel.parser.cfg, &channel.parser)))
    {
    stream_printf(&channel.stream, "Unable to start muon\r\n");
    return -1;
    }

  task_create("FBSYNC", DEFAULT_STACK_SIZE, fb_sync, 0, BELOW_NORMAL, 0);

  return cli_run(&channel.parser);
  }

static comm_device_p driver;

#ifdef __linux__
static const char *bsp_hive = "electron";
#else
static const char *bsp_hive = "krypton";
#endif

result_t bsp_can_init(handle_t rx_queue)
  {
  result_t result;
  memid_t key;
  if(failed(result = reg_open_key(0, bsp_hive, &key)))
    return result;

  return slcan_create(key, rx_queue, &driver);
  }

result_t bsp_send_can(const canmsg_t *msg)
  {
  if(msg == 0 || driver == 0)
    return e_bad_parameter;

  return slcan_send(driver, msg);
  }

// these are internal routines
extern result_t create_can_msg(canmsg_t *msg, uint16_t can_id, uint16_t type, uint16_t session, const char * val1, const char * val2, const char * val3, const char * val4);
extern void publish_local(const canmsg_t *msg);

// special command to simulate a message from the can bus.
result_t recv_can_id_type_session_val1_val2_val3_val4_action(cli_t *context,
  uint16_t can_id, uint16_t type, uint16_t session, const char * val1, const char * val2, const char * val3, const char * val4)
  {
  canmsg_t msg;
  result_t result;

  if (failed(result = create_can_msg(&msg, can_id, type, session, val1, val2, val3, val4)))
    return result;

  // enqueue the message onto the can bus.
  publish_local(&msg);
  return s_ok;
  }
