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
#define handle_t win_handle_t
#include <stdio.h>
#include <ctype.h>
#include <Windows.h>
#undef handle_t

#include "slcan.h"


// Filter mask settings
static const char* canusb_flush_cmd = "\r";
static const char* canusb_send_cmd = "t%03.3x%01.1x";

#define RX_QUEUE_LEN  128
#define MAX_MSG_LEN 64
#define NUM_RESPONSE_MSG  16

#if _MSC_VER
#define stricmp _stricmp
#endif

typedef enum _STATE
  {
  FIRST,
  CANMSG,
  RESPONSE,
  EMPTY,
  } STATE;

typedef struct _slcan_t {
  base_t base;
  // these are the can messages decoded
  handle_t rx_can_queue;
  // this is the serial decoder task
  handle_t rx_task;
  // max delay for send or rx
  uint32_t timeout;
  // selected device
  HANDLE comm_device;
  // buffer to prepare messages in
  char send_buffer[MAX_MSG_LEN];
  // current read state
  STATE state;
  // read buffer
  char read_buffer[MAX_MSG_LEN + 1];
  // number of bytes read
  uint16_t num_bytes_read;
  // read from a config
  char progname[REG_NAME_MAX + 1];
  } slcan_t;

static const typeid_t slcan_type = { "sclan_t" };

static slcan_t* driver;

static void slcan_worker(void*);
// send a message and optionally get a reply
static result_t slcan_send_buffer(slcan_t* driver,
  const char* buffer, uint32_t max_wait, uint16_t reply_len, char* reply);


static const char* device_str = "device";

result_t slcan_create(memid_t key, handle_t msg_rx_queue, handle_t* hndl)
  {
  result_t result;

  driver = (slcan_t*)malloc(sizeof(slcan_t));
  memset(driver, 0, sizeof(slcan_t));

  driver->base.type = &slcan_type;

  *hndl = (handle_t)driver;

  char device_name[REG_NAME_MAX + 1];
  uint16_t len = REG_NAME_MAX;
  if (failed(result = reg_get_string(key, device_str, device_name, &len)))
    return result;

  len = REG_NAME_MAX;
  if (failed(reg_get_string(key, "progname", driver->progname, &len)))
    strcpy("unknown", driver->progname);

  driver->comm_device = CreateFile(device_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

  if (driver->comm_device == INVALID_HANDLE_VALUE)
    {
    printf("Cannot open the CanUSB device %s will ignore can messages\n", device_name);
    }
  else
    {
    if (!SetupComm(driver->comm_device, 2048, 2048))
      {
      printf("Cannot set up the comm port\n");
      return e_unexpected;
      }

    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    dcb.BaudRate = 9600;
    dcb.ByteSize = 7;
    dcb.EofChar = 0;
    dcb.ErrorChar = 0;
    dcb.EvtChar = '\r';
    dcb.fAbortOnError = 0;
    dcb.fBinary = 0;
    dcb.fDsrSensitivity = 0;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fErrorChar = 0;
    dcb.fInX = 0;
    dcb.fNull = 1;
    dcb.fOutX = 0;
    dcb.fOutxCtsFlow = 1;
    dcb.fOutxDsrFlow = 0;
    dcb.fParity = 0;
    dcb.Parity = NOPARITY;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fTXContinueOnXoff = 0;
    dcb.StopBits = 2;
    dcb.XoffChar = 0;
    dcb.XoffLim = 0;
    dcb.XonChar = 0;
    dcb.XonLim = 0;

    if (!SetCommState(driver->comm_device, &dcb))
      {
      printf("Cannot set up the comm device\n");
      return e_unexpected;
      }

    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(driver->comm_device, &timeouts);

    // store where the can messages are sent to
    driver->rx_can_queue = msg_rx_queue;

    // create the worker
    if (failed(result = task_create("SLCAN", 0, slcan_worker, driver, NORMAL_PRIORITY, &driver->rx_task)) ||
      failed(result = task_resume(driver->rx_task)))
      return result;

    slcan_send_buffer(driver, canusb_flush_cmd, driver->timeout, 0, 0);
    slcan_send_buffer(driver, canusb_flush_cmd, driver->timeout, 0, 0);
    slcan_send_buffer(driver, canusb_flush_cmd, driver->timeout, 0, 0);
    }

  return s_ok;
  }

result_t slcan_close(handle_t hndl)
  {
  result_t result;
  slcan_t* driver = (slcan_t*)hndl;
  if (failed(result = is_typeof(hndl, &slcan_type, (void**)&driver)))
    return result;

  CloseHandle(driver->comm_device);

  free(driver);
  return s_ok;
  }

result_t slcan_send(handle_t hndl, const canmsg_t* msg)
  {
  result_t result;
  slcan_t* driver = (slcan_t*)hndl;
  if (failed(result = is_typeof(hndl, &slcan_type, (void**)&driver)))
    return result;

  if (driver->comm_device == INVALID_HANDLE_VALUE)
    return s_ok;

  uint16_t len = get_can_len(msg);

  sprintf(driver->send_buffer, canusb_send_cmd, get_can_id(msg), len);
  char byte_str[16];

  uint16_t n;
  for (n = 0; n < len; n++)
    {
    sprintf(byte_str, "%2.2x", msg->data[n]);
    strcat(driver->send_buffer, byte_str);
    }

  strcat(driver->send_buffer, "\r");

  // trace_debug("%s send %s\n", driver->progname,  driver->send_buffer);

  if (failed(result = slcan_send_buffer(driver, driver->send_buffer, 0, 0, 0)))
    return result;

  return s_ok;

  }

inline bool ishex(char ch)
  {
  return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f');
  }

inline uint8_t tohex(char ch)
  {
  return (uint8_t)((ch >= '0' && ch <= '9') ? ch - '0' : (ch - 'a') + 10);
  }

static result_t decode_message(const char* rx_msg)
  {
  if (tolower(*rx_msg) == 't')
    {
    canmsg_t msg;

    // trace_debug("%s receive %s\n", driver->progname, rx_msg);
    rx_msg++;
    uint16_t can_id;

    char ch = tolower(*rx_msg++);
    if (!ishex(ch))
      return e_bad_type;       // ignore

    can_id = tohex(ch);

    ch = tolower(*rx_msg++);
    if (!ishex(ch))
      return;

    can_id <<= 4;
    can_id |= tohex(ch);

    ch = tolower(*rx_msg++);
    if (!ishex(ch))
      return e_bad_type;

    can_id <<= 4;
    can_id |= tohex(ch);

    set_can_id(&msg, can_id);
    uint16_t len;
    ch = *rx_msg++;
    if (!isdigit(ch))   // only support 0-8
      return e_bad_type;

    len = tohex(ch);

    set_can_len(&msg, len);
    bool valid_payload = true;
    for (uint16_t i = 0; i < len; i++)
      {
      uint8_t nibble = 0;
      // must be 2 bytes per nibble
      ch = tolower(*rx_msg++);
      if (!ishex(ch))
        {
        valid_payload = false;
        break;
        }

      nibble = tohex(ch) << 4;

      ch = tolower(*rx_msg++);
      if (!ishex(ch))
        {
        valid_payload = false;
        break;
        }

      nibble |= tohex(ch);
      msg.data[i] = nibble;
      }

    if (valid_payload)
      return push_back(driver->rx_can_queue, &msg, INDEFINITE_WAIT);

    return e_bad_type;
    }
  }

/**
 * Worker process
 * @param parg handle to the can message queue
 */
static void slcan_worker(void* parg)
  {
  // reset the buffer
  driver->num_bytes_read = 0;



  while (true)
    {
    DWORD chars_actual = 0;
    // fill buffer if any room
    if ((MAX_MSG_LEN - driver->num_bytes_read) > 0)
      {
      DWORD errors;
      ClearCommError(driver->comm_device, &errors, NULL);

      if (!ReadFile(driver->comm_device, driver->read_buffer + driver->num_bytes_read, MAX_MSG_LEN - driver->num_bytes_read, &chars_actual, NULL))
        printf("Error reading from comm port\n");
      }

    if (chars_actual == 0)
      continue;

    driver->num_bytes_read += (uint16_t)chars_actual;

    uint16_t msg_start = 0;
    // scan for the starting 't' message
    while (msg_start < driver->num_bytes_read && driver->read_buffer[msg_start] != 't')
      msg_start++;

    if (msg_start >= driver->num_bytes_read)
      {
      // reset read ptr
      driver->num_bytes_read = 0;
      continue;
      }

    // remove all chars not a message
    if (msg_start > 0)
      {
      for (uint16_t i = 0; i < msg_start && (msg_start + i) < driver->num_bytes_read; i++)
        driver->read_buffer[i] = driver->read_buffer[i + msg_start];

      driver->num_bytes_read -= msg_start;
      msg_start = 0;
      }

    uint16_t msg_end = msg_start + 1;
    while (msg_end < driver->num_bytes_read)
      {
      if (driver->read_buffer[msg_end] == '\r')
        break;
      msg_end++;
      }

    if (msg_end < driver->num_bytes_read)
      {
      driver->read_buffer[msg_end] = 0;

      if (failed(decode_message(driver->read_buffer)))
        trace_error("badly formed can message", driver->read_buffer);

      msg_end++;

      for (uint16_t i = 0; (msg_end + i) < driver->num_bytes_read; i++)
        driver->read_buffer[i] = driver->read_buffer[i + msg_end];

      // remove the message
      driver->num_bytes_read -= msg_end;
      }
    }
  }

result_t slcan_send_buffer(slcan_t* driver,
  const char* buffer,
  uint32_t max_wait,
  uint16_t reply_len,
  char* reply)
  {
  DWORD written;
  DWORD result = WriteFile(driver->comm_device, buffer, strlen(buffer), &written, NULL);

  if (result != 0)
    return e_unexpected;

  return s_ok;
  }
