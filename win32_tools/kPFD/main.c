#include <windows.h>

#include "../../libs/photon/photon_priv.h"
#include "../../libs/neutron/stream.h"
#include "../krypton/slcan.h"
#include "../krypton/getopt.h"
#include "../../libs/proton/proton.h"
#include "../krypton/mfd_cli.h"
//#include "../../libs/proton/layout_window.h"
#include "../../libs/proton/menu_window.h"
#include "../../libs/mfdlib/mfd_layout.h"
#include "../../pic/kPFD/pfd.h"
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../krypton/krypton.h"

#define CAN_TX_BUFFER_LEN 1024
#define CAN_RX_BUFFER_LEN 1024

extern const char* splash_320_240;     // 320x240

static FILE* ci;
static FILE* co;

typedef struct _service_channel_t
  {
  stream_t stream;
  } service_channel_t;

static typeid_t service_channel_type;

// global screen handle
handle_t screen;

#include <Shlwapi.h>

void get_app_dir(char* buffer, size_t len)
  {
  GetModuleFileName(NULL, buffer, len);
  PathRemoveFileSpec(buffer);
  }

static result_t css_stream_eof(handle_t hndl)
  {
  return feof(ci) ? s_ok : s_false;
  }

static HANDLE clipdata = 0;
static int offset = 0;

static result_t css_read_clipboard(handle_t* hndl, void* buffer, uint32_t size, uint32_t* read)
  {
  int16_t bytesRead = 0;
  char* str = (char*)buffer;
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

    if (*str == '\r')
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
    CloseClipboard();
    clipdata = 0;
    }

  if (read != 0)
    *read = (uint16_t)bytesRead;

  if (bytesRead == 0)
    return e_no_more_information;

  return s_ok;
  }

typedef struct _escape_seq_t {
  const char* sequence;
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
  set_can_len(&msg, 3);
  msg.data[0] = CANFLY_INT16;
  msg.data[1] = (uint8_t)(value >> 8);
  msg.data[2] = (uint8_t)value;

  // send to the message queue.  handle 0 means top window
  post_message(0, &msg, 0);
  }

static result_t css_stream_read(handle_t hndl, void* buffer, uint32_t size, uint32_t* read)
  {
  if (hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;

  DWORD bytesRead = 0;
  char* str = (char*)buffer;

  if (clipdata != 0 && succeeded(css_read_clipboard(hndl, buffer, size, read)))
    return s_ok;

  bool inEscape = false;


  do
    {
    if (size > 0)
      {
      // if we have an escape sequence in the buffer then we process it.
      char* bp = (char*)buffer;
      while (!inEscape && escapeSeq[0] != 0 && size > 0)
        {
        *bp++ = escapeSeq[0];

        uint16_t i;
        for (i = 1; i < 16 && escapeSeq[i - 1] != 0; i++)
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
      const escape_sequence_t* sequence = sequences;
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
          else if (escapeSeq[cp + 1] == 0)
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

    return s_ok;
  }

static result_t css_stream_write(handle_t hndl, const void* buffer, uint32_t size)
  {
  if (hndl == 0 || buffer == 0 || size == 0)
    return e_bad_parameter;
  WriteFile(co, buffer, size, NULL, NULL);

  return s_ok;
  }

static result_t css_stream_close(handle_t hndl)
  {
  return s_ok;
  }

// these are internal routines
extern result_t create_can_msg(canmsg_t* msg, uint16_t can_id, uint16_t type, const char* val1,
  const char* val2, const char* val3, const char* val4);

extern result_t open_key(memid_t current, const char* path, bool create, memid_t* memid);

static const char* help =
"CanFly <Options> registry_path\n"
"  -n             Create a new config database\n"
"  -c <size>      Open config database with <size> blocks\n"
"  -x <x-pixels>  Set the screen width to x-pixels\n"
"  -y <y-pixels>  Set the screen height to y-pixels\n"
"  -d device      Device to open for the serial com port\n"
"                 needs to be slcan:COMx: the COM port can be determined by the dos mode command\n"
"  -h             Print this help message\n"
" Values for canfly on Windows:\n"
" mfd10 -c 4096 -x 1024 -y 600 -d COM3 canfly.cdb\n";

// Debug  -x 320 -y 240 -f ../diy-efis/framework/emulator/fs  ../diy-efis/framework/emulator/diy-efis.reg

static result_t print_error(const char* msg, result_t result)
  {
  fprintf(stderr, "%s %d\n", msg, result);
  return result;
  }

static const char* neutron_str = "neutron";
static const char* flash_str = "flash";

int main(int argc, char** argv)
  {
  krypton_parameters_t init_params;
  memset(&init_params, 0, sizeof(init_params));
  init_params.neutron_params.node_id = 14;
  init_params.neutron_params.tx_length = CAN_TX_BUFFER_LEN;
  init_params.neutron_params.tx_stack_length = 4096;
  init_params.neutron_params.rx_length = CAN_RX_BUFFER_LEN;
  init_params.neutron_params.rx_stack_length = 4096;
  init_params.config_blocks = (uint16_t)(4096 * 32);
  init_params.init_gdi = true;
  init_params.neutron_params.status_interval = 1000;
  
  init_params.factory_reset = false;

  init_params.width = 320;
  init_params.height = 240;

  const char* app_path = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hnc:x:y:d:")) != -1)
    {
    switch (opt)
      {
      case 'h':
        puts(help);
        return s_false;
      case 'n':
        init_params.factory_reset = true;
        break;
      case 'c':
        init_params.config_blocks = atoi(optarg);
        break;
      case 'x':
        init_params.width = atoi(optarg);
        break;
      case 'y':
        init_params.height = atoi(optarg);
        break;
      case 'd':
        init_params.init_slcan = true;
        init_params.slcan_device = _strdup(optarg);
        break;
      case 't':
        init_params.touch_tools = true;
        break;
      default:
        print_error(help, s_false);
        exit(-1);
      }
    }

  if (optind >= argc)
    {
    printf(help);
    return -1;
    }

  init_params.config_path = argv[optind];

  if (failed(krypton_init(argc, argv, &init_params)))
    {
    printf("Unable to initialize the krypton library.");
    return -1;
    }

  photon_parameters_t photon_params;
  memset(&photon_params, 0, sizeof(photon_params));

  photon_params.gdi_extents.dx = init_params.width;
  photon_params.gdi_extents.dy = init_params.height;

  // start photon
  if (failed(photon_init(&photon_params, init_params.factory_reset)))
    {
    MessageBox(NULL, "Unable to start the GDI", "Canfly Startup Error", MB_OK);
    return -1;
    }

  ci = GetStdHandle(STD_INPUT_HANDLE);
  co = GetStdHandle(STD_OUTPUT_HANDLE);
  // set up the console
  DWORD dwMode;
  bool is_console = GetConsoleMode(ci, &dwMode);

  dwMode &= ~ENABLE_LINE_INPUT;
  dwMode &= ~ENABLE_PROCESSED_INPUT;
  dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
  SetConsoleMode(ci, dwMode);

  GetConsoleMode(co, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(co, dwMode);

  stream_t channel;
  memset(&channel, 0, sizeof(service_channel_t));

  service_channel_type.base = &stream_type;
  service_channel_type.name = "service_channel_t";

  channel.base.type = &service_channel_type;
  channel.eof = css_stream_eof;
  channel.read = css_stream_read;
  channel.write = css_stream_write;

  init_params.config_path = argv[optind];


  // open the registry at /neutron/fs
  memid_t root;
  if (failed(reg_open_key(0, neutron_str, &root)) &&
    failed(reg_create_key(0, neutron_str, &root)))
    {
    trace_error("Cannot open the root registry");
    return;
    }

  // open the windows filesystem so files can be copied.
  filesystem_t *win_fs;
  if (failed(krypton_fs_init("C:", &win_fs)))
    {
    trace_error("Cannot create the windows filesystem mount");
    return;
    }

  if (failed(mount('C', win_fs)))
    {
    trace_error("Cannot mount the windows filesystem onto the vfs");
    return;
    }

  memid_t fs;
  if (succeeded(reg_open_key(root, flash_str, &fs)) ||
    succeeded(reg_create_key(root, flash_str, &fs)))
    {
    flash_disk_t* boot_disk;
    flash_params_t params;
    bool init_mode;
    if (failed(krypton_create_flash_disk(fs, &params, &init_mode, &boot_disk)))
      {
      trace_error("Cannot create the flash disk system");
      return;
      }

    filesystem_t *flash;
    if (init_mode)
      {
      if (failed(create_flash_filesystem(boot_disk, &params, false, &flash)))
        {
        trace_error("Cannot initialize a new file system");
        return;
        }
      }
    else
      {
      if (failed(open_flash_filesystem(boot_disk, &params, &flash)))
        {
        trace_error("Cannot open existing file system");
        return;
        }
      }


    if (failed(mount('F', flash)))
      {
      trace_error("Cannot mount the flash file system");
      return;
      }
    }

    static const char *root_fs_name = "fs";

  // and mount the registry as the root filesystem
  filesystem_t *root_fs;
  memid_t fs_memid;
  if (failed(reg_open_key(0, root_fs_name, &fs_memid)) &&
    failed(reg_create_key(0, root_fs_name, &fs_memid)))
    {
    trace_error("Cannot create the root filesystem");
    return;
    }

  if (failed(create_registry_fs(fs_memid, &root_fs)) ||
    failed(mount('R', root_fs)))
    {
    trace_error("Cannot mount the root file system");
    return;
    }

  proton_params_t args;

  // load the layout from the registry
  args.create_widgets = create_pfd;
  args.create_layout = create_layout;
  args.photon_params.orientation = 0;

  if (failed(task_create("PROTON", DEFAULT_STACK_SIZE, (task_callback)run_proton, &args, NORMAL_PRIORITY, 0)))
    trace_error("Cannot start proton");

  run_krypton(&channel, "canfly", &mfd_cli_root);

  return 0;
  }


  // in the emulator this does nothing (yet...)
  static uint16_t brightness = 100;

  result_t get_brightness(menu_item_spin_edit_t* edit, int32_t* value)
    {
    *value = brightness;
    return s_ok;
    }

  result_t set_brightness(menu_item_spin_edit_t* edit, int32_t value)
    {
    brightness = (uint16_t)value;
    return s_ok;
    }
