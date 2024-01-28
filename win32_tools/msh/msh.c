#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#define strdup _strdup
#define handle_t win_handle_t
#include <Windows.h>
#undef handle_t

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#include "../krypton/krypton.h"
#include "../krypton/getopt.h"
#include "../krypton/argv.h"
#include "../krypton/getdelim.h"
#include "../krypton/slcan.h"
#include "../../libs/atom/board_id.h"
#include "../../libs/neutron/neutron-service.pb-c.h"

static FILE *ci;
static FILE *co;

static int ch = 0;

static int get_ch()
  {
  if (ch == '\r')
    Sleep(100);

  ReadFile(ci, &ch, 1, NULL, NULL);

  // trace_debug("Console: 0x%02.2x\n", ch);

  return ch;
  }

static void put_ch(char ch)
  {
  WriteFile(co, &ch, 1, NULL, NULL);
  }
#endif

const char *help = \
"msh <options> <-c [command]>\n" \
"  -r <path>      Set registry path to <path>\n" \
"  -n <size>      Create a new registry with <size> blocks, old path will be deleted\n" \
"  -d device      Device to open for the CanUSB port\n" \
"  -x remote      Remote device to connect to."
"     ecu-l     left ECU device\n" \
"     ecu-r     right ECU device\n" \
"     kahrs     ahrs device\n" \
"     kedu      edu device\n" \
"     khub      comm and ahrs hub device\n" \
"     kmfd1     first mfd device\n" \
"     kmfd2     second mfd device\n" \
"     kmfd3     third mfd device\n" \
"     kmfd4     fourth mfd device\n" \
"     xxx       device id 0-14\n" \
"  -h             Print this help message\n" \
" [command] is optional and if provided is a shell command to run\n" \
"           to get a list of commands run interactive and press ?\n";

const char *connect_help = \
"     ecu-l     left kmag device\n" \
"     ecu-r     right kmag device\n" \
"     kahrs     ahrs device\n" \
"     kedu      edu device\n" \
"     khub      comm and ahrs hub device\n" \
"     kmfd1     first mfd device\n" \
"     kmfd2     second mfd device\n" \
"     kmfd3     third mfd device\n" \
"     kmfd4     fourth mfd device\n" \
"     xxx       device id 0-14\n";


static uint8_t remote_id = scan_id;

static bool channel_closed = false;

static const char *base_dir = 0;

static result_t con_accept_msg(handle_t hndl, void **parg)
  {
  // for now just return the console
  *parg = 0;

  return s_ok;
  }

static result_t con_bearer_msg(handle_t channel, void *parg, uint16_t len, const uint8_t *msg)
  {
  // write the bytes received for up to 8 char
  uint16_t sent = 0;
  while (sent < len)
    {
    put_ch(*msg++);
    sent++;
    }

  return s_ok;
  }

static result_t con_close_msg(handle_t channel, void *parg)
  {
  channel_closed = true;

  printf("\n\nThe remote server has terminated the connection\n");
  
#ifdef _WIN32
  // set up the console
  HANDLE ci = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE co = GetStdHandle(STD_OUTPUT_HANDLE);

  DWORD dwMode;
  GetConsoleMode(ci, &dwMode);

  dwMode |= ENABLE_LINE_INPUT;
  dwMode |= ENABLE_PROCESSED_INPUT;
  dwMode &= ~ENABLE_VIRTUAL_TERMINAL_INPUT;
  SetConsoleMode(ci, dwMode);

  GetConsoleMode(co, &dwMode);

  dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(co, dwMode);
#endif
  exit(0);
  return s_ok;
  }

int main(int argc, char **argv)
  {
  krypton_parameters_t init_params;
  memset(&init_params, 0, sizeof(krypton_parameters_t));

  ci = GetStdHandle(STD_INPUT_HANDLE);
  co = GetStdHandle(STD_OUTPUT_HANDLE);

  init_params.neutron_params.node_id = (uint8_t)scan_id;
  init_params.neutron_params.tx_length = 64;
  init_params.neutron_params.tx_stack_length = 4096;
  init_params.neutron_params.rx_length = 64;
  init_params.neutron_params.rx_stack_length = 4096;

  // init_params.no_raw_console = true;

  const char *remote_dev = 0;
  char *cmdline = 0;
  int opt;
  while ((opt = getopt(argc, argv, "hn:r:d:x:")) != -1)
    {
    switch (opt)
      {
      case 'h':
        puts(help);
        return s_false;
      case 'n':
        init_params.factory_reset = true;
        init_params.config_blocks = atoi(optarg);
        break;
      case 'd':
        init_params.init_slcan = true;
        init_params.slcan_device = strdup(optarg);
        break;
      case 'r' :
        init_params.config_path = strdup(optarg);
        break;
      case 'x':
        remote_dev = strdup(optarg);
        break;
      default:
        printf(help);
        exit(-1);
      }
    }

  if (failed(krypton_init(argc, argv, &init_params)))
    {
    printf("Unable to initialize the krypton library.");
    return -1;
    }

  if (remote_dev == 0)
    {
    char buffer[1000];
    printf("Please select the remote device (press ? for a list)\n");
    while (remote_dev == 0)
      {
      scanf("%s", buffer);
      if (strlen(buffer) == 0 || strcmp(buffer, "?") == 0)
        {
        printf(connect_help);
        continue;
        }

      remote_dev = strdup(buffer);
      break;
      }
    }

  // connect to the device specified.
  if (isdigit(remote_dev[0]))
    {
    int id = atoi(remote_dev);
    if (id < 0 || id >= scan_id)
      {
      printf("%s is not a valid node id to connect to.  Must be 0..14\n", remote_dev);
      return -1;
      }

    remote_id = (uint8_t)id;
    }
  else if (strncmp(remote_dev, "kahrs", 5) == 0)
    remote_id = ahrs_node_id;
  else if (strncmp(remote_dev, "kedu", 4) == 0)
    remote_id = edu_node_id;
  else if (strncmp(remote_dev, "khub", 4) == 0)
    remote_id = hub_node_id;
  else if (strncmp(remote_dev, "kpfd", 4) == 0)
    {
    remote_id = edm_node_id;
    if (remote_dev[4] != 0)
      {
      int id = atoi(remote_dev + 4);
      if (id < 1 || id > 8)
        {
        printf("Only a mfd with a prefix of 1..8 can be used\n");
        return -1;
        }
      remote_id += id - 1;
      }
    }
  else if (strncmp(remote_dev, "ecu-l", 5) == 0)
    remote_id = mag_left_id;
  else if (strncmp(remote_dev, "ecu-r", 5) == 0)
    remote_id = mag_right_id;
  else
    {
    printf("%s is not a valid remote id\n", remote_dev);
    return -1;
    }

  char *line = *argv;

  const char *cmd = 0;
  printf("CanFly console 1.0\n");


#ifdef _WIN32
  // set up the console
  DWORD dwMode;
  bool is_console = GetConsoleMode(ci, &dwMode);

  dwMode &= ~ENABLE_LINE_INPUT;
  dwMode &= ~ENABLE_PROCESSED_INPUT;
  dwMode &= ~ENABLE_MOUSE_INPUT;
  dwMode &= ~ENABLE_WINDOW_INPUT;
  dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
  SetConsoleMode(ci, dwMode);

  GetConsoleMode(co, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(co, dwMode);
#endif

  handle_t channel;
  channel_closed = false;
  if (succeeded(create_service_client(remote_id, service_console, con_accept_msg, con_bearer_msg, con_close_msg, 0, &channel)))
    {
    send_bearer_message(channel, 1, "\n");

    while (!channel_closed)
      {
      // todo: should be on a thread that can be cancelled.
      char ch = (int)get_ch(co);

      send_bearer_message(channel, 1, &ch);
      }
    }
  else
    printf("Cannot connect to the remote device\n");

  return 0;
  }

result_t start_fb(uint16_t x, uint16_t y)
  {
  printf("Cannot run a framebuffer inside the shell\n");
  exit(-1);
  }
