/*
diy-efis
Copyright (C) 2016-2022 Kotuku Aerospace Limited

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
then the original copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.

If you wish to use any of this code in a commercial application then
you must obtain a licence from the copyright holder.  Contact
support@kotuku.aero for information on the commercial licences.
*/
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#include "../krypton/krypton.h"
#include "../krypton/getopt.h"
#include "../krypton/argv.h"
#include "../krypton/getdelim.h"
#include "../krypton/slcan.h"

#include "../../libs/neutron/neutron-service.pb-c.h"

#ifdef _MSC_VER
#define strdup _strdup

#include <Windows.h>

static FILE *ci;
static FILE *co;

static int get_ch()
  {
  int ch = 0;

  ReadFile(ci, &ch, 1, NULL, NULL);

  return ch;
  }

static void put_ch(char ch)
  {
  WriteFile(co, &ch, 1, NULL, NULL);
  }
#endif

const char *help = \
"msh <options> from to\n" \
"  -r <path>      Set registry path to <path>\n" \
"  -n <size>      Create a new registry with <size> blocks, old path will be deleted\n" \
"  -f <path>      Use <path> as the base of the filesystem\n" \
"  -d device      Device to open for the CanUSB port\n" \
"  -x remote      Remote device to connect to.  Can be set with the con_cmd"
"     kMagL     Left kMag device\n" \
"     kMagR     Right kMag device\n" \
"     kAHRS     Ahrs device\n" \
"     kEDU      Edu device\n" \
"     kHub      Comm and AHRS hub device\n" \
"     kMFD1     First mfd device\n" \
"     kMFD2     Second mfd device\n" \
"     kMFD3     Third mfd device\n" \
"     kMFD4     Fourth mfd device\n" \
"     xxx       Device id 0-14\n" \
"  -i           Upload as flash image\n"
"  -s <name>    Set startup to image name\n"
"  -l           List image names\n"
"  -h           Print this help message\n"
" <from>        Local file to copy\n"
" <to>          Path to copy to (must start with a / character)\n";

static uint8_t remote_id = scan_id;

static bool channel_closed = false;

static const char *base_dir = 0;

//////////////////////////////////////////////////////////////////////
/// 
/// Wrapper functions
/// 
static result_t call_open_key(handle_t client, reply_buffer_t *reply, const char *path, memid_t key)
  {
  return e_not_implemented;
  }

result_t call_stream_create(handle_t client, reply_buffer_t *reply, const char *path, handle_t *stream)
  {
  Canfly__Neutron__StreamCreateResponse *create_response = 0;
  Canfly__Neutron__StreamCreateRequest create_request = CANFLY__NEUTRON__STREAM_CREATE_REQUEST__INIT;

  create_request.path = (char *)path;

  if (failed(canfly__neutron__neutron_service__stream_create(client, &create_request, reply, &create_response)) ||
    !create_response->has_handle ||
    !create_response->has_status ||
    failed(create_response->status) ||
    create_response->handle == 0)
    {
    if (create_response != 0)
      protobuf_c_message_free_unpacked((ProtobufCMessage *)create_response, 0);

    return e_unexpected;
    }

  *stream = (handle_t)create_response->handle;
  protobuf_c_message_free_unpacked((ProtobufCMessage *)create_response, 0);

  return s_ok;
  }

result_t call_stream_open(handle_t client, reply_buffer_t *reply, const char *path, handle_t *stream)
  {
  Canfly__Neutron__StreamOpenResponse *open_response = 0;
  Canfly__Neutron__StreamOpenRequest open_request = CANFLY__NEUTRON__STREAM_OPEN_REQUEST__INIT;

  open_request.path = (char *)path;

  if (failed(canfly__neutron__neutron_service__stream_open(client, &open_request, reply, &open_response)) ||
    !open_response->has_handle ||
    !open_response->has_status ||
    failed(open_response->status) ||
    open_response->handle == 0)
    {
    if (open_response != 0)
      protobuf_c_message_free_unpacked((ProtobufCMessage *)open_response, 0);
    return e_unexpected;
    }

  *stream = (handle_t)open_response->handle;
  protobuf_c_message_free_unpacked((ProtobufCMessage *)open_response, 0);

  return s_ok;
  }

result_t call_image_create(handle_t client, reply_buffer_t *reply, const char *image_name, uint32_t image_size, handle_t *stream)
  {
  Canfly__Neutron__CreateImageResponse *create_response = 0;
  Canfly__Neutron__CreateImageRequest create_request = CANFLY__NEUTRON__CREATE_IMAGE_REQUEST__INIT;

  create_request.image_name = (char *) image_name;
  create_request.image_size = image_size;
  create_request.has_image_size = true;

  if (failed(canfly__neutron__neutron_service__create_image(client, &create_request, reply, &create_response)) ||
    create_response == 0 ||
    !create_response->has_status ||
    !create_response->has_module_handle ||
    failed(create_response->status) ||
    create_response->module_handle == 0)
    {
    if (create_response != 0)
      protobuf_c_message_free_unpacked((ProtobufCMessage *)create_response, 0);
    return e_unexpected;
    }

  *stream = (handle_t)create_response->module_handle;
  protobuf_c_message_free_unpacked((ProtobufCMessage *)create_response, 0);

  return s_ok;
  }


result_t call_stream_rename(handle_t client, reply_buffer_t *reply, handle_t stream, const char *new_filename)
  {
  return e_not_implemented;
  }

result_t call_create_directory(const char *path)
  {
  return e_not_implemented;
  }

result_t call_remove_directory(handle_t client, reply_buffer_t *reply, const char *path)
  {
  return e_not_implemented;
  }

result_t call_open_directory(handle_t client, reply_buffer_t *reply, const char *dirname, handle_t *dirp)
  {
  return e_not_implemented;
  }

result_t call_freespace(handle_t client, reply_buffer_t *reply, const char *path, uint32_t *freespace, uint32_t *totalspace)
  {
  Canfly__Neutron__FreespaceResponse *freespace_response = 0;
  Canfly__Neutron__FreespaceRequest freespace_request = CANFLY__NEUTRON__FREESPACE_REQUEST__INIT;

  freespace_request.path = (char *)path;

  if (failed(canfly__neutron__neutron_service__freespace(client, &freespace_request, reply, &freespace_response)) ||
    !freespace_response->has_freespace ||
    !freespace_response->has_status ||
    freespace_response->status != s_ok)
    {
    if (freespace != 0)
      protobuf_c_message_free_unpacked((ProtobufCMessage *)freespace_response, 0);

    return e_unexpected;
    }

  if (freespace != 0)
    *freespace = freespace_response->freespace;

  if (totalspace != 0)
    *totalspace = freespace_response->totalspace;

  protobuf_c_message_free_unpacked((ProtobufCMessage *)freespace_response, 0);

  return s_ok;
  }

result_t call_close_handle(handle_t client, reply_buffer_t *reply, handle_t handle)
  {
  Canfly__Neutron__CloseHandleResponse *close_response = 0;
  Canfly__Neutron__CloseHandleRequest close_request = CANFLY__NEUTRON__CLOSE_HANDLE_REQUEST__INIT;

  close_request.handle = (uint32_t)handle;
  close_request.has_handle = true;

  if (failed(canfly__neutron__neutron_service__close_handle(client, &close_request, reply, &close_response)) ||
    !close_response->has_status ||
    failed(close_response->status))
    {
    protobuf_c_message_free_unpacked((ProtobufCMessage *)close_response, 0);
    return close_response->has_status ? close_response->status : e_unexpected;
    }

  protobuf_c_message_free_unpacked((ProtobufCMessage *)close_response, 0);
  return s_ok;
  }

result_t call_set_startup_image(handle_t client, reply_buffer_t *reply, const char *image_name)
  {
  Canfly__Neutron__SwitchImageResponse *switch_response = 0;
  Canfly__Neutron__SwitchImageRequest switch_request = CANFLY__NEUTRON__SWITCH_IMAGE_REQUEST__INIT;

  switch_request.image_name = (char *) image_name;

  if (failed(canfly__neutron__neutron_service__switch_image(client, &switch_request, reply, &switch_response)) ||
    !switch_response->has_status ||
    failed(switch_response->status))
    {
    protobuf_c_message_free_unpacked((ProtobufCMessage *)switch_response, 0);
    return switch_response->has_status ? switch_response->status : e_unexpected;
    }

  protobuf_c_message_free_unpacked((ProtobufCMessage *)switch_response, 0);
  return s_ok;
  }

result_t call_list_images(handle_t client, reply_buffer_t *reply, char *running_image, uint16_t running_image_length,
  char *backup_image, uint16_t backup_image_length)
  {
  Canfly__Neutron__GetImageNamesResponse *get_names_response = 0;
  Canfly__Neutron__GetImageNamesRequest get_names_request = CANFLY__NEUTRON__GET_IMAGE_NAMES_REQUEST__INIT;

  if (failed(canfly__neutron__neutron_service__get_image_names(client, &get_names_request, reply, &get_names_response)) ||
    !get_names_response->has_status ||
    failed(get_names_response->status))
    {
    protobuf_c_message_free_unpacked((ProtobufCMessage *)get_names_response, 0);
    return get_names_response->has_status ? get_names_response->status : e_unexpected;
    }

  if (get_names_response->running_image == 0)
    *running_image = 0;
  else
    strncpy(running_image, get_names_response->running_image, running_image_length);

  if (get_names_response->backup_image == 0)
    *backup_image = 0;
  else
    strncpy(backup_image, get_names_response->backup_image, backup_image_length);

  protobuf_c_message_free_unpacked((ProtobufCMessage *)get_names_response, 0);
  return s_ok;
  }

result_t call_stream_delete(handle_t client, reply_buffer_t *reply, handle_t stream)
  {
  return e_not_implemented;
  }

result_t call_stream_eof(handle_t client, reply_buffer_t *reply, handle_t stream)
  {
  return e_not_implemented;
  }

result_t call_stream_read(handle_t client, reply_buffer_t *reply, handle_t stream, void *buffer, uint16_t size, uint16_t *read)
  {
  Canfly__Neutron__StreamReadResponse *read_response = 0;
  Canfly__Neutron__StreamReadRequest read_request = CANFLY__NEUTRON__STREAM_READ_REQUEST__INIT;

  read_request.handle = (uint32_t)stream;
  read_request.has_handle = true;
  read_request.max_bytes = size;
  read_request.has_max_bytes = true;

  if (failed(canfly__neutron__neutron_service__stream_read(client, &read_request, reply, &read_response)) ||
    !read_response->has_status ||
    failed(read_response->status) ||
    read_response->data.len > size ||
    read_response->data.len > 65535)
    {
    if (read_response != 0)
      protobuf_c_message_free_unpacked((ProtobufCMessage *)read_response, 0);

    return e_unexpected;
    }

  if (read != 0)
    *read = (uint16_t)read_response->data.len;

  memcpy(buffer, read_response->data.data, read_response->data.len);

  protobuf_c_message_free_unpacked((ProtobufCMessage *)read_response, 0);
  return s_ok;
  }

result_t call_stream_write(handle_t client, reply_buffer_t *reply, handle_t stream, const void *buffer, uint16_t size)
  {
  Canfly__Neutron__StreamWriteResponse *write_response = 0;
  Canfly__Neutron__StreamWriteRequest write_request = CANFLY__NEUTRON__STREAM_WRITE_REQUEST__INIT;

  write_request.handle = (uint32_t)stream;
  write_request.has_handle = true;
  write_request.data.data = (uint8_t *)buffer;
  write_request.has_data = true;
  write_request.data.len = size;

  if (failed(canfly__neutron__neutron_service__stream_write(client, &write_request, reply, &write_response)) ||
    !write_response->has_status ||
    failed(write_response->status))
    {
    if (write_response != 0)
      {
      protobuf_c_message_free_unpacked((ProtobufCMessage *)write_response, 0);

      return write_response->has_status ? write_response->status : e_unexpected;
      }
    else
      return e_bad_pointer;
    }

  protobuf_c_message_free_unpacked((ProtobufCMessage *)write_response, 0);
  return s_ok;
  }

result_t call_stream_getpos(handle_t client, reply_buffer_t *reply, handle_t stream, uint32_t *pos)
  {
  return e_not_implemented;
  }

result_t call_stream_setpos(handle_t client, reply_buffer_t *reply, handle_t stream, uint32_t pos)
  {
  return e_not_implemented;
  }

result_t call_stream_length(handle_t client, reply_buffer_t *reply, handle_t stream, uint32_t *length)
  {
  return e_not_implemented;
  }

result_t call_stream_truncate(handle_t client, reply_buffer_t *reply, handle_t stream, uint32_t length)
  {
  Canfly__Neutron__StreamTruncateResponse *truncate_response = 0;
  Canfly__Neutron__StreamTruncateRequest truncate_request = CANFLY__NEUTRON__STREAM_TRUNCATE_REQUEST__INIT;

  truncate_request.handle = (uint32_t)stream;
  truncate_request.has_handle = true;

  truncate_request.length = length;
  truncate_request.has_length = true;

  result_t result = s_ok;
  if ((failed(result = canfly__neutron__neutron_service__stream_truncate(client, &truncate_request, reply, &truncate_response)) ||
    !truncate_response->has_status ||
    failed(truncate_response->status)) &&
    succeeded(result))
    result = e_unexpected;

  protobuf_c_message_free_unpacked((ProtobufCMessage *)truncate_response, 0);

  return result;
  }

char *base_path = 0;
char *from_path = 0;
char *to_path = 0;
bool is_recursive = false;
const char *remote_dev = 0;
char *cmdline = 0;
bool cmd_opt = false;
bool is_image = false;
bool set_startup = false;
char *startup_image = 0;
bool list_images = false;

int main(int argc, char **argv)
  {
  Sleep(1000);

  krypton_parameters_t init_params;
  memset(&init_params, 0, sizeof(krypton_parameters_t));

  init_params.neutron_params.node_id = (uint8_t)scan_id;
  init_params.neutron_params.tx_length = 64;
  init_params.neutron_params.tx_stack_length = 4096;
  init_params.neutron_params.rx_length = 64;
  init_params.neutron_params.rx_stack_length = 4096;

  int opt;

  while ((opt = getopt(argc, argv, "hils:n:f:r:d:c:x:")) != -1)
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
      case 'f':
        base_path = strdup(optarg);
        break;
      case 'd':
        init_params.init_slcan = true;
        init_params.slcan_device = strdup(optarg);
        break;
      case 'r':
        init_params.config_path = strdup(optarg);
        break;
      case 'c':
        cmd_opt = true;
        cmdline = strdup(optarg);
        break;
      case 'x':
        remote_dev = strdup(optarg);
        break;
      case 's':
        set_startup = true;
        startup_image = strdup(optarg);
        break;
      case 'i':
        is_image = true;
        break;
      case 'l':
        list_images = true;
        break;
      default:
        printf(help);
        exit(-1);
      }
    }

  if (remote_dev == 0)
    {
    printf("A remote device must be provided\n");
    printf(help);
    exit(-1);
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
    remote_id = khub_node_id;
  else if (strncmp(remote_dev, "kmfd", 4) == 0)
    {
    remote_id = mfd_node_id;
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
  else if (strncmp(remote_dev, "kmagl", 5) == 0)
    remote_id = kmag_left_id;
  else if (strncmp(remote_dev, "kmagr", 5) == 0)
    remote_id = kmag_right_id;
  else
    {
    printf("%s is not a valid remote id\n", remote_dev);
    return -1;
    }

  if (failed(krypton_init(argc, argv, &init_params)))
    {
    printf("Unable to initialize the krypton library.");
    return -1;
    }

  result_t result;

  // this is reused
  reply_buffer_t *reply;
  create_reply_buffer(&reply);

  handle_t client;
  if (failed(result = create_protoc_client(remote_id, service_neutron, 5000, reply, &client)))
    {
    printf("Cannot create the neutron client\n");
    return -1;
    }

  // if there is a from/to provided then this is to copy
  // a file to a remote path
  if (optind == argc - 2)
    {
    from_path = strdup(argv[optind]);
    to_path = strdup(argv[optind + 1]);

    if (to_path == 0)
      {
      printf("A destination path must be provided\n");
      return -1;
      }

    if (from_path == 0)
      {
      printf("A source path must be provided\n");
      return -1;
      }

    if (is_recursive)
      {
      printf("Not implemented yet\n");
      return 0;
      }
    else
      {

      char open_path[MAX_PATH];
      if (base_dir != 0)
        {
        strcpy(open_path, base_dir);
        strcat(open_path, "/");
        strcat(open_path, from_path);
        }
      else
        strcpy(open_path, from_path);

      struct stat src_stat;

      if (stat(open_path, &src_stat) != 0)
        {
        printf("Cannot get file information for %s\n", from_path);
        return -1;
        }

      if (src_stat.st_size > 32768)
        {
        printf("The CanFly operating system is intended for small embedded devices\nand the file size limit is 32k, please split the file\n");
        return -1;
        }

      char path[MAX_PATH];
      char filename[MAX_PATH];
      char ext[MAX_PATH];

      _splitpath(to_path, NULL, path, filename, ext);

      if (strlen(path) == 0 ||
        strlen(filename) == 0)
        {
        printf("A destination path and filename is required\n");
        return -1;
        }

      if ((strlen(filename) + strlen(ext)) > REG_NAME_MAX)
        {
        printf("The CanFly operating system is designed for small embedded devices, and the\nfilename limit is %d characters.\nPlease use a smaller filename\n", REG_NAME_MAX);
        return -1;
        }

      uint32_t freespace;

      if (failed(call_freespace(client, reply, to_path, &freespace, 0)) ||
        freespace < (uint32_t)src_stat.st_size)
        {
        printf("There is not enough space on the destination device for the file\n");
        return -1;
        }

      // try to open the file.
      FILE *src = fopen(open_path, "rb");

      if (src == NULL)
        {
        printf("Cannot open %s\n", from_path);
        return -1;
        }

      handle_t handle = 0;
      if (is_image)
        {
        if (failed(call_image_create(client, reply, to_path, (uint32_t)src_stat.st_size, &handle)))
          {
          printf("Cannot create the image %s\n", to_path);
          return -1;
          }
        }
      else
        {
        if (failed(call_stream_open(client, reply, to_path, &handle)) ||
          handle == 0)
          {
          printf("File does not exist, create it\n");

          if (failed(call_stream_create(client, reply, to_path, &handle)) ||
            handle == 0)
            {
            printf("Cannot create the stream\n");
            return -1;
            }
          }
        else
          {
          printf("File exists, truncate it\n");

          if (failed(call_stream_truncate(client, reply, handle, 0)))
            {
            printf("Cannot truncate the file\n");
            return -1;
            }
          }
        }

      int total_read = 0;

      uint8_t buffer[128];        // write 128 bytes at a time;
      do
        {
        int bytes_read = fread(buffer, 1, 128, src);

        total_read += bytes_read;

        if (bytes_read == 0)
          break;

        if ((total_read % 1024) == 0)
          printf("Transferred %d kb of %d\r", total_read / 1024, src_stat.st_size / 1024);

        if (failed(call_stream_write(client, reply, handle, buffer, bytes_read)))
          {
          call_close_handle(client, reply, handle);
          printf("\nCannot write to the stream\n");
          return -1;
          }

        } while (!feof(src));

        call_close_handle(client, reply, handle);

        printf("\nFile transferred ok\n");
        }

    // process the optional commands
    if (list_images)
      {
      char running_image[REG_NAME_MAX];
      char backup_image[REG_NAME_MAX];

      if (failed(call_list_images(client, reply, running_image, REG_NAME_MAX, backup_image, REG_NAME_MAX)))
        {
        printf("Cannot list the images on the device\n");
        return -1;
        }

      printf("Running image : %s\n", running_image);
      printf("Backup image  : %s\n", backup_image);
      }

    if (set_startup)
      {
      if (failed(call_set_startup_image(client, reply, startup_image)))
        {
        printf("Cannot set the startup image\n");
        return -1;
        }
      }

    release_reply_buffer(reply);
    }

  return 0;
  }

// stubs
result_t create_application_module(
  const char *image_name,
  uint32_t length,
  bool is_startup,
  handle_t *fd)
  {
  return e_not_implemented;
  }

result_t remove_application()
  {
  return e_not_implemented;
  }

result_t start_application()
  {
  return e_not_implemented;
  }

result_t stop_application()
  {
  return e_not_implemented;
  }

static handle_t driver;
/*
static const char *bsp_hive = "krypton";

result_t bsp_can_init(handle_t rx_queue, const neutron_parameters_t *params)
  {
  result_t result;
  memid_t key;
  if (failed(result = reg_open_key(0, bsp_hive, &key)))
    return result;

  return slcan_create(key, rx_queue, &driver);
  }

result_t bsp_send_can(const canmsg_t *msg)
  {
  if (msg == 0 || driver == 0)
    return e_bad_parameter;

  return slcan_send(driver, msg);
  }
  */

result_t start_fb(uint16_t x, uint16_t y)
  {
  printf("Cannot run a framebuffer inside the shell\n");
  exit(-1);
  }

result_t cli_init(cli_cfg_t *cfg, cli_t *parser)
  {
  return e_not_implemented;
  }

result_t cli_run(cli_t *parser)
  {
  return e_not_implemented;
  }

result_t muon_initialize_cli(cli_node_t *cli_root, const char *name)
  {
  return e_not_implemented;
  }