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
#ifndef __vDevice_h__
#define __vDevice_h__

#include "thread.h"

namespace kotuku {
class device_t
  {
public:
  /////////////////////////////////////////////////////
  // Event mask
  //
  // mask used to control ready events
  typedef uint32_t ioctl_mask;

  static const ioctl_mask input_ready = 0x00000001;
  static const ioctl_mask output_ready = 0x00000002;
  static const ioctl_mask device_error = 0x00000004;
  static const ioctl_mask enable_irq = 0x00000008;
  static const ioctl_mask reset_irq = 0x00000010;
  static const ioctl_mask set_in_queue = 0x00000020;
  static const ioctl_mask set_out_queue = 0x00000040;
  static const ioctl_mask set_irq_len = 0x00000080;

  // remaining 28 bits are user-defined

  /////////////////////////////////////////////////////
  // IOCTL
  //
  // set/get flags

  enum ioctl_type
    {
    set_device_ctl,
    get_device_ctl
      };

  /////////////////////////////////////////////////////
  // General queue control
  //
  static const uint32_t queue_status = 1;

  // this is a read/write

  struct queue_ioctl_t
    {
    size_t version; // == sizeof(queue_ioctl_t)
    ioctl_mask mask;
    size_t in_queue_length;
    size_t out_queue_length;
    size_t irq_threshold;
      };

  enum seek_type
    {
    current,
    begin,
    end
      };

  // overlapped I/O structure
  // note the corresponding methods will fill in
  // the action element of the structure. Any
  // values given will be lost.

  struct overlapped_t
    {

    enum opn_type
      {
      ioctl_opn,
      read_opn,
      write_opn,
      seek_opn
          };

    union action_t
      {

      struct ioctl_opn_t
        {
        void *read_ptr;
        size_t read_size;
        const void *write_ptr;
        size_t write_size;
        ioctl_type ioctl;
        size_t xfer_length;
              } ioctl;

      struct read_opn_t
        {
        void *read_ptr;
        size_t read_size;
        size_t *xfer_length;
              } read;

      struct write_opn_t
        {
        const void *write_ptr;
        size_t write_size;
        size_t *xfer_length;
              } write;

      struct seek_opn_t
        {
        seek_type where;
        size_t posn;
        size_t *new_posn;
              } seek;
          } action;

    opn_type operation;
    // wait on this for notification of the operation
    event_t *completion;
    result_t *result;
    // will be deleted when the structure deleted
    void *os_data;

    overlapped_t();
    ~overlapped_t();
      };

  virtual ~device_t();

  // close the device
  result_t close();

  result_t ioctl(ioctl_type type, const void *in_buffer,
                 size_t in_buffer_size, void *out_buffer, size_t out_buffer_size,
                 size_t *size_returned, event_t *completion = 0, result_t *rslt = 0);

  result_t read(void *, size_t, size_t * = 0, event_t *completion = 0,
                result_t *rslt = 0, overlapped_t **olp = 0);
  result_t write(const void *, size_t, size_t * = 0, event_t *completion = 0,
                 result_t *rslt = 0, overlapped_t **olp = 0);
  result_t seek(seek_type where, size_t position, size_t *new_pos = 0,
                event_t *completion = 0, result_t *rslt = 0, overlapped_t **olp = 0);

  result_t cancel_io();
protected:
  device_t();

  virtual bool device_request(device_t::overlapped_t *) = 0;
  virtual result_t device_cancel_io() = 0;
  virtual result_t device_close() = 0;
  };

#define make_ioctl(cmd, opn) device_t::ioctl_type((int(cmd) << 2) | int(opn))

typedef device_t * device_handle_t;

/////////////////////////////////////////////////////
// Power control device
//
static const uint32_t power_control = 2;

struct power_ioctl_t
  {
  size_t version; // sizeof(power_ioctl_t)
  bool power_on; // set false will power off the unit
  bool on_standby; // true if the master power has gone
  bool low_voltage; // true if the battery voltage is low
  uint8_t screen_brightness; // set to adjust the brightness
  };

///////////////////////////////////////////////////////////////////////////////
//
//  Generic serial port device ioctls.

enum comms_ioctl_type_t
  {
  comms_state_ioctl,
  comms_error_ioctl,
  comms_escape_ioctl,
  comms_break_ioctl,
  comms_event_mask_ioctl,
  comms_modem_status_ioctl,
  comms_timeouts_ioctl,
  };

enum baud_rate_t
  {
  cbr_110 = 110,
  cbr_300 = 300,
  cbr_600 = 600,
  cbr_1200 = 1200,
  cbr_2400 = 2400,
  cbr_4800 = 4800,
  cbr_9600 = 9600,
  cbr_14400 = 14400,
  cbr_19200 = 19200,
  cbr_38400 = 38400,
  cbr_57600 = 57600,
  cbr_115200 = 115200,
  cbr_128000 = 128000,
  cbr_240000 = 240000,
  cbr_256000 = 256000
  };

enum parity_mode_t
  {
  no_parity,
  odd_parity,
  even_parity,
  mark_parity,
  space_parity
  };

enum dtr_mode_t
  {
  dtr_disabled,
  dtr_enabled,
  dtr_flowcontrol
  };

enum rts_control_t
  {
  rts_control_disabled,
  rts_control_enabled,
  rts_control_handshake,
  rts_control_toggle
  };

enum stop_bits_t
  {
  one_stop_bit = 0,
  one_five_stop_bit = 1,
  two_stop_bit = 2
  };

struct comms_ioctl_t
  {
  size_t version;
  comms_ioctl_type_t ioctl_type;
  };

struct comms_state_ioctl_t : public comms_ioctl_t
  {
  baud_rate_t baud_rate;
  bool binary_mode;
  parity_mode_t parity;
  bool cts_out_enabled;
  bool dsr_out_enabled;
  bool dsr_in_enabled;
  dtr_mode_t dtr_mode;
  rts_control_t rts_control;
  bool xmit_continue_on_xoff;
  bool xoff_out_enabled;
  bool xoff_in_enabled;
  bool parity_character_enabled;
  bool ignore_null_characters;
  bool abort_on_error;
  bool parity_checking_enabled;
  short xon_limit;
  short xoff_limit;
  size_t byte_size;
  stop_bits_t stop_bits;
  char xon_char;
  char xoff_char;
  char error_character;
  char parity_character;
  char eof_char;
  char event_char;
  };

struct comms_error_ioctl_t : public comms_ioctl_t
  {
  bool overflow;
  bool overrun;
  bool parity;
  bool framing_error;
  bool break_char;
  };

enum escape_character_type
  {
  escape_set_xoff = 1,
  escape_set_xon = 2,
  escape_set_rts = 3,
  escape_clear_rts = 4,
  escape_set_dtr = 5,
  escape_clear_dtr = 6,
  escape_set_break = 8,
  escape_clear_break = 9
  };

struct comms_escape_ioctl_t : public comms_ioctl_t
  {
  escape_character_type escape_type;
  };

struct comms_break_ioctl_t : public comms_ioctl_t
  {
  bool set_break;
  };

struct comms_event_mask_ioctl_t : public comms_ioctl_t
  {
  // types of events that can be waited on
  typedef uint16_t event_mask_t;

  static const event_mask_t ev_break = 0x0040;
  static const event_mask_t ev_cts = 0x0008;
  static const event_mask_t ev_dsr = 0x0010;
  static const event_mask_t ev_err = 0x0080;
  static const event_mask_t ev_event1 = 0x0800;
  static const event_mask_t ev_event2 = 0x1000;
  static const event_mask_t ev_perr = 0x0200;
  static const event_mask_t ev_ring = 0x0100;
  static const event_mask_t ev_rlsd = 0x0020;
  static const event_mask_t ev_rx80full = 0x0400;
  static const event_mask_t ev_rxchar = 0x0001;
  static const event_mask_t ev_rxflag = 0x0002;
  static const event_mask_t ev_txempty = 0x0004;

  event_t *notification_event;
  event_mask_t mask;
  };

struct comms_modem_status_ioctl_t : public comms_ioctl_t
  {
  bool cts_on;
  bool dsr_on;
  bool ring_on;
  bool rlsd_on;
  };

struct comms_timeouts_ioctl_t : public comms_ioctl_t
  {
  /*
   The maximum time allowed to elapse between the arrival of two bytes on the communications line,
   in milliseconds. During a ReadFile operation, the time period begins when the first uint8_t is received.
   If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is
   completed and any buffered data is returned. A value of zero indicates that interval time-outs are not used.

   A value of limit<unsigned int>::max(), combined with zero values for both the ReadTotalTimeoutConstant and
   ReadTotalTimeoutMultiplier members, specifies that the read operation is to return immediately with the bytes
   that have already been received, even if no bytes have been received.
   */
  unsigned int read_interval_timeout;
  /*
   The multiplier used to calculate the total time-out period for read operations, in milliseconds.
   For each read operation, this value is multiplied by the requested number of bytes to be read.
   */
  unsigned int read_total_timeout_multiplier;
  /*
   A constant used to calculate the total time-out period for read operations, in milliseconds.
   For each read operation, this value is added to the product of the ReadTotalTimeoutMultiplier member
   and the requested number of bytes.

   A value of zero for both the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant members indicates
   that total time-outs are not used for read operations.
   */
  unsigned int read_total_timeout_constant;
  /*
   The multiplier used to calculate the total time-out period for write operations, in milliseconds.
   For each write operation, this value is multiplied by the number of bytes to be written.
   */
  unsigned int write_total_timeout_multiplier;
  /*
   A constant used to calculate the total time-out period for write operations, in milliseconds.
   For each write operation, this value is added to the product of the WriteTotalTimeoutMultiplier
   member and the number of bytes to be written.

   A value of zero for both the WriteTotalTimeoutMultiplier and WriteTotalTimeoutConstant members
   indicates that total time-outs are not used for write operations.
   */
  unsigned int write_total_timeout_constant;
  };

/////////////////////////////////////////////////////
// socket device ioctls
//

enum socket_ioctl_type
  {
  socket_bind, // initialize the stack and bind to a tcp address
  socket_resolv, // resolve a dns name
  socket_listen, // listen on a port/socket
  socket_connect, // connect to a remote tcp port
  socket_purge, // purge all data that is queued
  };

enum socket_connection_type
  {
  socket_stream, // reliable stream socket (ip)
  socket_datagram, // datagram stream (udp)
  socket_seqpacket, // reliable sequenced packet service
  socket_raw, // raw packets
  socket_can, // canbus protocol
  socket_can_ex, // canbus extended protocol
  };

// IP v4 address

struct socket_address_t
  {
  uint8_t u8[4];

  socket_address_t(uint8_t a1, uint8_t a2, uint8_t a3,
                   uint8_t a4)
    {
    u8[0] = a1;
    u8[1] = a2;
    u8[2] = a3;
    u8[3] = a4;
    }

  socket_address_t(int addr)
    {
    *this = addr;
    }

  socket_address_t()
    {
    u8[0] = 0;
    u8[1] = 0;
    u8[2] = 0;
    u8[3] = 0;
    }

  const socket_address_t &operator=(const socket_address_t &other)
    {
    u8[0] = other.u8[0];
    u8[1] = other.u8[1];
    u8[2] = other.u8[2];
    u8[3] = other.u8[3];

    return *this;
    }

  const socket_address_t &operator=(int addr)
    {
    u8[0] = (uint8_t) (addr >> 24);
    u8[1] = (uint8_t) (addr >> 18);
    u8[2] = (uint8_t) (addr >> 8);
    u8[3] = (uint8_t) addr;

    return *this;
    }

  socket_address_t(const socket_address_t &other)
    {
    *this = other;
    }

  bool operator==(int addr) const
    {
    return ((int) ((uint32_t) * this)) == addr;
    }

  bool operator==(const socket_address_t &other) const
    {
    return ((uint32_t) * this) == ((uint32_t) other);
    }

  bool operator<(const socket_address_t &other) const
    {
    return ((uint32_t) * this) < ((uint32_t) other);
    }

  operator uint32_t() const
    {
    return ((uint32_t) u8[0]) << 24 | ((uint32_t) u8[1]) << 16
      | ((uint32_t) u8[2]) << 8 | u8[3];
    }
  };

struct socket_ioctl_t
  {
  size_t version;
  socket_ioctl_type type;
  };

struct socket_ioctl_bind_t : socket_ioctl_t
  {
  socket_connection_type connection_type;
  socket_address_t addr;
  uint32_t read_timeout;

  socket_ioctl_bind_t(uint32_t to = event_t::no_limit) :
  read_timeout(to)
    {
    }
  };

// return false to cancel callback.
struct socket_ioctl_resolv_t;
typedef bool (*socket_ioctl_resolv_async)(socket_ioctl_resolv_t *request,
                                          socket_address_t *address);

struct socket_ioctl_resolv_t : socket_ioctl_t
  {
  std::string in_address;
  socket_address_t *address;
  size_t buffer_length; // number of addresses to return.  (0 if call-back)
  socket_ioctl_resolv_async callback; // callback to handle resolved addresses, 0 if buffer is used.
  };

typedef void (*socket_ioctl_accept)(device_t *server, device_t *client,
                                    socket_ioctl_resolv_t *request);

struct socket_ioctl_accept_t : socket_ioctl_t
  {
  size_t max_connections; // maximum number of tcp connections allowed
  socket_ioctl_accept callback; // where the device is to call when a new connection is made.
  };

struct socket_ioctl_connect_t : socket_ioctl_t
  {
  socket_address_t address; // address to connect to.
  };

struct socket_ioctl_purge_t : socket_ioctl_t
  {
  };
  };
  
#endif
