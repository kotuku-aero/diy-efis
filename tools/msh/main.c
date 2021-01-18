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
#include "../../libs/muon/muon.h"
#include "../../libs/neutron/stream.h"
#include "../../libs/neutron/slcan.h"
#include "../../libs/ion/ion.h"

#include "../../libs/ion/nano/HAL/Include/nanoHAL_v2.h"

#include "msh_cli.h"

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef __linux__
#include "../../libs/electron/electron.h"
#include <termios.h>
#include <sys/stat.h>
#else
extern result_t krypton_init(int argc, const char**argv);
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

#ifndef __linux__
static HANDLE clipdata = 0;
static int offset = 0;

static result_t css_read_clipboard(stream_handle_t *hndl, void *buffer, uint16_t size, uint16_t *read)
  {
  int16_t bytesRead = 0;
  char *str = (char *)buffer;
  LPSTR hglb = GlobalLock(clipdata);

  if (hglb == 0)
    return e_not_found;

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
#endif

typedef struct _escape_seq_t {
  const char *sequence;
  int numChar;
  uint16_t can_id;
  int16_t value;
  } escape_sequence_t;

static const escape_sequence_t sequences[] = {
    { "OP", 3, id_key0, 1 }, // F1
    { "OQ", 3, id_key1, 1 }, // F2
    { "OR", 3, id_key2, 1 }, // F3
    { "OS", 3, id_key3, 1 }, // F4
    { "[15~", 5, id_key4, 1 }, // F5
    { "[17~", 5, id_decka, 1 }, // F6
    { "[18~", 5,  id_decka, -1 }, // F7
    { "[19~", 5, id_decka, 1 }, // F8
    { "[20~", 5,  id_decka, -1 }, // F9
    { 0, 0, 0, 0 }
  };

static uint16_t numSeq = 0;
static char escapeSeq[16] = { 0 };

static void send_escape(uint16_t can_id, uint16_t value)
  {
  canmsg_t msg;
  memset(&msg, 0, sizeof(canmsg_t));
  set_can_id(&msg, can_id);
  msg.canas.data_type = CANAS_DATATYPE_SHORT;
  set_can_len(&msg, 6);
  msg.canas.service_code = 1;
  msg.canas.data[0] = value >> 8;
  msg.canas.data[1] = value;

  // send to the message queue.  handle 0 means top window
  post_message(0, &msg, 0);
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

    bool inEscape = false;
    

    do
      {
      if (size > 0)
        {
        // if we have an escape sequence in the buffer then we process it.
        char *bp = (char *)buffer;
        while (!inEscape && escapeSeq[0] != 0 && size > 0)
          {
          *bp++ = escapeSeq[0];

          uint16_t i;
          for (i = 1; i < 16 && escapeSeq[i-1] != 0; i++)
            escapeSeq[i - 1] = escapeSeq[i];
          size--;
          }

        buffer = bp;

        if (size > 0)
          ReadFile(ci, buffer, size, &bytesRead, NULL);
        else
          bytesRead = 0;
        }

      if (inEscape)
        {
        char ch = *str;
        escapeSeq[numSeq++] = ch;
        escapeSeq[numSeq] = 0;
        // get the next and match
        // must be numerically ascending...
        const escape_sequence_t *sequence = sequences;
        bool partialMatch = false;

        while (inEscape && sequence->sequence != 0)
          {
          uint16_t cp;
          for (cp = 0; cp < sequence->numChar; cp++)
            {
            if (escapeSeq[cp] == 0 && sequence->sequence[cp] == 0)
              {
              inEscape = false;
              send_escape(sequence->can_id, sequence->value);
              numSeq = 0;
              escapeSeq[0] = 0;
              break;
              }

            if (sequence->sequence[cp] == 0 ||
              escapeSeq[cp] != sequence->sequence[cp] ||
              escapeSeq[cp] == 0)
              {
              // no match
              sequence++;
              break;
              }
            else if (escapeSeq[cp+1] == 0)
              partialMatch = true;
            }
          }

        if (!partialMatch)
          {
          inEscape = false;     // just stop processing

          // push escape into the buffer
          uint16_t chnum = numSeq;
          escapeSeq[chnum + 1] = 0;
          while (chnum > 0)
            {
            escapeSeq[chnum] = escapeSeq[chnum - 1];
            chnum--;
            }

          escapeSeq[0] = 0x1b;
          numSeq = 0;
          }
        }
      else
        {
        if (read != 0)
          *read = (uint16_t)bytesRead;

        while (bytesRead--)
          {
          size--;
          switch (*str)
            {
            case 0x1b:
              inEscape = true;
              size = 1;
              break;
            case '\r':
              *str = '\n';
              break;
            case  0x16:  // windows CTRL-V
              if (clipdata == NULL)
                {
                if (!OpenClipboard(GetShellWindow()))
                  continue;
                clipdata = GetClipboardData(CF_TEXT);
                offset = 0;
                size++;
                return css_read_clipboard(hndl, str, size, read);
                }

              break;
            }
          }
        }
      } while (inEscape || size > 0);
       
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

// TODO:init the services
HAL_SYSTEM_CONFIG HalSystemConfig;

int main(int argc, char **argv)
  {
#ifdef __linux__
    if(failed(electron_init(argc, argv)))
    {
        printf("Unable to initialize the krypton library.");
        return -1;
    }

#else
  if(failed(krypton_init(argc, argv)))
    {
    printf("Unable to initialize the krypton library.");
    return -1;
    }
#endif

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

  stream_p splash_screen;
  manifest_create(splash_base64, &splash_screen);

  // this creates the display and loads the screen

  handle_t main_window;
  result_t result;
  uint16_t orientation = 0;

  ion_context_t *context = (ion_context_t *)neutron_malloc(sizeof(ion_context_t));

  if (failed(reg_open_key(0, "ion", &context->home)))
    {
    if (failed(reg_create_key(0, "ion", &context->home)))
      {
      trace_error("Cannot create the ion root key");
      return -1;
      }
    }

  bool start_ion = true;
  char startup_assembly[REG_STRING_MAX];
  if (failed(result = reg_get_string(context->home, "start", startup_assembly, 0)))
    {
    trace_warning("No startup assembly defined, ion will not start");
    start_ion = false;
    }

  // try to start the screen.  This is hardware dependent and will be
  // implemented in the hardware abstraction
  if (succeeded(result = open_screen(orientation, defwndproc, 0, &main_window)))
    {
    // see if a splash screen is loaded
    if (splash_screen != 0)
      {
      // determine if the destination window is suitable for
      // expanding the PNG.  We must have a color that
      // is 32 bits.  If not we create a canvas that is
      // guaranteed to be 32 bits
      extent_t ex;
      uint16_t bpp;
      if (succeeded(get_canvas_extents(main_window, &ex, &bpp)) &&
        bpp < 32)
        {
        handle_t canvas = 0;
        if (succeeded(create_rect_canvas(&ex, &canvas)) &&
          succeeded(load_png(canvas, splash_screen, 0)))
          {
          rect_t wnd_rect = { 0, 0, ex.dx, ex.dy };
          point_t pt = { 0, 0 };

          // copy the rendered bitmap over
          bit_blt(main_window, &wnd_rect, &wnd_rect, canvas, &wnd_rect, &pt);
          }
        canvas_close(canvas);
        }
      else
        load_png(main_window, splash_screen, 0);

      stream_close(splash_screen);
      }
    }

  if (start_ion)
    {
    // start ion
    context->console_in = &channel.stream;
    context->console_out = &channel.stream;
    context->console_err = &channel.stream;

    if (failed(ion_create(context)))
      {
      trace_error("Unable to start ion");
      return -1;
      }

    task_create("PROTON", DEFAULT_STACK_SIZE * 4, ion_run, context, NORMAL_PRIORITY, &context->worker);
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

result_t bsp_can_init(deque_p rx_queue, uint16_t bit_rate)
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

void Watchdog_Reset()
  {

  }

/*
    static const CLR_UINT32 c_CapabilityFlags_FloatingPoint = 0x00000001;
    static const CLR_UINT32 c_CapabilityFlags_SourceLevelDebugging = 0x00000002;
    static const CLR_UINT32 c_CapabilityFlags_AppDomains = 0x00000004;
    static const CLR_UINT32 c_CapabilityFlags_ExceptionFilters = 0x00000008;
    static const CLR_UINT32 c_CapabilityFlags_IncrementalDeployment = 0x00000010;
    static const CLR_UINT32 c_CapabilityFlags_SoftReboot = 0x00000020;
    static const CLR_UINT32 c_CapabilityFlags_Profiling = 0x00000040;
    static const CLR_UINT32 c_CapabilityFlags_Profiling_Allocations = 0x00000080;
    static const CLR_UINT32 c_CapabilityFlags_Profiling_Calls = 0x00000100;
    static const CLR_UINT32 c_CapabilityFlags_ThreadCreateEx = 0x00000400;
    static const CLR_UINT32 c_CapabilityFlags_ConfigBlockRequiresErase = 0x00000800;
*/

uint32_t GetPlatformCapabilities()
  {
  return 0x00000023L;
    ;
  }


void CPU_Sleep()
  {
  yield();
  }

void CPU_Reset()
  {
  DebugBreak();
  }

bool CPU_IsSoftRebootSupported()
  {
  return true;
  }

void *platform_malloc(size_t size)
  {
  return malloc(size);
  }

void platform_free(void *arg)
  {
  free(arg);
  }


int hal_snprintf(char *buffer, size_t len, const char *format, ...)
  {
  va_list arg_ptr;
  int chars;

  va_start(arg_ptr, format);

  chars = hal_vsnprintf(buffer, len, format, arg_ptr);

  va_end(arg_ptr);

  return chars;
  }

int hal_vsnprintf(char *buffer, size_t len, const char *format, va_list arg)
  {
  return _vsnprintf_s(buffer, len, len - 1 /* force space for trailing zero*/, format, arg);
  }
