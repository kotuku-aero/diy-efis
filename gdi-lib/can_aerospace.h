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
#ifndef __canfly_h__
#define __canfly_h__

#include "stddef.h"
#include "thread.h"
#include "can_driver.h"
#include "errors.h"
#include "time.h"
#include "../can-aerospace/can_aerospace.h"

#include <vector>
#include <map>
#include <deque>

namespace kotuku {

class msg_t : public can_msg_t
  {
public:
  // These are the standard data types
  static const uint8_t datatype_nodata = 0;
  static const uint8_t datatype_error = 1;
  static const uint8_t datatype_float = 2;
  static const uint8_t datatype_long = 3;
  static const uint8_t datatype_ulong = 4;
  static const uint8_t datatype_blong = 5;
  static const uint8_t datatype_short = 6;
  static const uint8_t datatype_ushort = 7;
  static const uint8_t datatype_bshort = 8;
  static const uint8_t datatype_char = 9;
  static const uint8_t datatype_uchar = 10;
  static const uint8_t datatype_bchar = 11;
  static const uint8_t datatype_short2 = 12;
  static const uint8_t datatype_ushort2 = 13;
  static const uint8_t datatype_bshort2 = 14;
  static const uint8_t datatype_char4 = 15;
  static const uint8_t datatype_uchar4 = 16;
  static const uint8_t datatype_bchar4 = 17;
  static const uint8_t datatype_char2 = 18;
  static const uint8_t datatype_uchar2 = 19;
  static const uint8_t datatype_bchar2 = 20;
  static const uint8_t datatype_memid = 21;
  static const uint8_t datatype_chksum = 22;
  static const uint8_t datatype_achar = 23;
  static const uint8_t datatype_achar2 = 24;
  static const uint8_t datatype_achar4 = 25;
  static const uint8_t datatype_char3 = 26;
  static const uint8_t datatype_uchar3 = 27;
  static const uint8_t datatype_bchar3 = 28;
  static const uint8_t datatype_achar3 = 29;
  static const uint8_t datatype_doubleh = 30;
  static const uint8_t datatype_doublel = 31;
  static const uint8_t datatype_udef_begin = 100;
  static const uint8_t datatype_udef_end = 255;
  static const uint8_t datatype_all_end = 255;

  // Node Service codes
  static const uint8_t service_ids = 0; // Requests a �sign-of-life� response together
  // with configuration information from the addressed node.
  static const uint8_t service_nss = 1; // Node synchronisation service, used to trigger a specific node or
  // to perform a network wide time synchronisation.
  static const uint8_t service_dss = 2; // Data download service. Sends a block of data to another node.
  static const uint8_t service_dus = 3; // Data upload service. Receives a block of data from another node.
  static const uint8_t service_scs = 4; // Simulation Control Service. Allows to change the behaviour of
  // the addressed node by controlling internal simulation software
  // functions.
  static const uint8_t service_tis = 5; // Transmission Interval Service.  Sets the transmission rate of a
  // specific CAN message transmitted by the addressed node.
  static const uint8_t service_fps = 6; // FLASH Programming Service.  Triggers a node-internal routine
  // to store configuration data permantently into non-volatile memory.
  static const uint8_t service_sts = 7; // State Transmission Service.  Causes the addressed node to
  // transmit all its CAN messages once.
  static const uint8_t service_fss = 8; // Filter Setting Service. Used to modify the limit frequencies of
  // node-internal highpass, lowpass or bandpass filters.
  static const uint8_t service_tcs = 9; // Test Control Service. Triggers internal test functions of the
  // addressed node.
  static const uint8_t service_bss = 10; // CAN Baudrate Setting Service.  Sets the CAN baudrate ot the
  // addressed node.
  static const uint8_t service_nis = 11; // Node-ID Setting Service. Sets the Node-ID of the addressed node.
  static const uint8_t service_mis = 12; // Module Information Service. Obtains information about installed
  // modules within the addressed

  enum message_type
    {
    canas_ecc = 0,
    canas_noc = 1,
    canas_nsc = 4,
    canas_udc = 5,
    canas_tmc = 6,
    canas_fmc = 7,
      };

  msg_t(uint16_t label);
  msg_t(uint16_t label, uint8_t, uint8_t service_code = 0);
  msg_t(uint16_t label, uint16_t, uint8_t service_code = 0);
  msg_t(uint16_t label, uint32_t, uint8_t service_code = 0);
  msg_t(uint16_t label, int8_t, uint8_t service_code = 0);
  msg_t(uint16_t label, int16_t, uint8_t service_code = 0);
  msg_t(uint16_t label, int32_t, uint8_t service_code = 0);
  msg_t(uint16_t label, float, uint8_t service_code = 0);
  msg_t(uint16_t label, const uint8_t data[4], size_t length, uint8_t service_code = 0);
  msg_t(const msg_t &);
  msg_t(const can_msg_t &);

  msg_t()
    {
    flags = 0;
    id = 0;
    std::fill(msg.raw, msg.raw + 8, 0);
    }

  const msg_t &operator=(const msg_t &);
  uint8_t operator[](int index) const;

  uint8_t msg_data0() const
    {
    return (*this)[0];
    }

  uint8_t msg_data1() const
    {
    return (*this)[1];
    }

  uint8_t msg_data2() const
    {
    return (*this)[2];
    }

  uint8_t msg_data3() const
    {
    return (*this)[3];
    }

  uint8_t msg_data_u8(int index = 0) const;
  uint16_t msg_data_u16(int index = 0) const;
  uint32_t msg_data_u32() const;
  int8_t msg_data_8(int index = 0) const;
  int16_t msg_data_16(int index = 0) const;
  int32_t msg_data_32() const;
  float msg_data_float() const;

  // return a value coerced to a scalar value

  template<typename T> T value() const
    {
    return value<T>(0);
    }
  template<typename T> T value(int index) const;

  uint16_t message_id() const;
  uint8_t node_id() const;
  uint8_t data_type() const;
  uint8_t message_code() const;
  uint8_t service_code() const;
  const date_time_t &timestamp() const;

  /**
   * Format the value of the msg
   * @param format  optional string to format the value. see remarks
   * @return  formatted string
   * @remarks the format string must be sympathetic to the value.
   * characters, shorts, and ints are all presented as integers
   * floats are converted to doubles
   * multi-values are passed in as positional variables.
   * If no format provided a default format is used.
   */
  std::string to_string(const char *format) const;
private:

  date_time_t _timestamp;
  };

template<typename t_result>
inline t_result msg_t::value(int index) const
  {
  switch (data_type())
    {
    case msg_t::datatype_nodata:
      return t_result();
    case msg_t::datatype_long:
      return t_result(msg_data_32());
    case msg_t::datatype_ulong:
    case msg_t::datatype_blong:
    case msg_t::datatype_error:
    case msg_t::datatype_memid:
    case msg_t::datatype_chksum:
      return t_result(msg_data_u32());
    case msg_t::datatype_float:
      return t_result(msg_data_float());
      break;
    case msg_t::datatype_bshort:
    case msg_t::datatype_bshort2:
    case msg_t::datatype_short:
      return t_result(msg_data_16(index));
    case msg_t::datatype_ushort:
    case msg_t::datatype_short2:
    case msg_t::datatype_ushort2:
      return t_result(msg_data_u16(index));
    case msg_t::datatype_uchar:
    case msg_t::datatype_bchar:
    case msg_t::datatype_char:
    case msg_t::datatype_char4:
    case msg_t::datatype_uchar4:
    case msg_t::datatype_bchar4:
    case msg_t::datatype_char2:
    case msg_t::datatype_uchar2:
    case msg_t::datatype_bchar2:
    case msg_t::datatype_achar:
    case msg_t::datatype_achar2:
    case msg_t::datatype_achar4:
    case msg_t::datatype_char3:
    case msg_t::datatype_uchar3:
    case msg_t::datatype_bchar3:
    case msg_t::datatype_achar3:
      return t_result(msg_data_8(index));
    }
  return t_result();
  }

class canaerospace_provider_t : public thread_t
  {
public:
  // Broadcast Node ID can be used to perform network-wide service requests
  static const int broadcast_node_id = 0;
  // Maximum number of nodes per one network
  static const int maximum_nodes = 255;
  // This library can work with the number of redundant interfaces not higher than this.
  static const int maximum_interfaces = 8;

  class can_service_t
    {
  public:
    can_service_t(uint8_t service_code);

    // called when a remote service responds to a service request.
    virtual result_t service(canaerospace_provider_t *,
                             uint8_t service_channel, const msg_t &) = 0;
    // called when a remote service requests the service to provide data
    virtual result_t service_response(canaerospace_provider_t *,
                                      uint8_t service_channel, const msg_t &) = 0;

    // generate the service request_id
    uint32_t request_id(bool is_reply, bool extended = false);
  private:
    uint8_t _service_code;
      };

  // create a service
  result_t create_service(uint8_t service_id, can_service_t *service);

  class can_parameter_handler_t
    {
  public:
    virtual void receive_parameter(canaerospace_provider_t *, const msg_t &) = 0;
    virtual bool is_equal(const can_parameter_handler_t &) const = 0;
      };

  // register a parameter handler
  result_t subscribe(uint16_t, can_parameter_handler_t *handler);
  result_t unsubscribe(uint16_t, can_parameter_handler_t *handler);

  // advertise a parameter.  Is placed in the dictionary
  //result_t advertise(uint16_t id);
  //result_t unadvertise(uint16_t id);
  result_t publish(const msg_t &);

  virtual uint8_t node_id() const = 0;
  // used to receive messages.
  virtual result_t receive(const can_msg_t &);

  // used to enable publishing received events.
  // the subscriptions is not re-entrant so to enable windows
  // to be created, the application needs to turn off publishing while
  // windows subscribe/unsubscribe
  // returns the previous state on the enable (default = no)
  bool publishing_enabled(bool is_it);
protected:
  canaerospace_provider_t();
private:
  bool _publishing_enabled;
  typedef std::vector<can_parameter_handler_t *> subscribers_t;
  typedef std::map<uint16_t, subscribers_t> subscriptions_t;
  subscriptions_t _subscriptions;

  typedef std::map<uint8_t, can_service_t *> services_t;
  services_t _services;

  typedef std::deque<msg_t> published_values_t;
  published_values_t _publishing;

  event_t _doorbell;
  critical_section_t _cs;
  static uint32_t do_run(void *);
  virtual uint32_t run();
  };

// canaerospace error codes
static const result_t canas_err_argument = 0x80040001;
static const result_t canas_err_not_enough_memory = 0x80040002;
static const result_t canas_err_driver = 0x80040003;
static const result_t canas_err_no_such_entry = 0x80040004;
static const result_t canas_err_entry_exists = 0x80040005;
static const result_t canas_err_bad_data_type = 0x80040006;
static const result_t canas_err_bad_message_id = 0x80040007;
static const result_t canas_err_bad_node_id = 0x80040008;
static const result_t canas_err_bad_redund_chan = 0x80040009;
static const result_t canas_err_bad_service_chan = 0x8004000A;
static const result_t canas_err_bad_can_frame = 0x8004000B;
static const result_t canas_err_quota_exceeded = 0x8004000C;
static const result_t canas_err_logic = 0x8004000D;

inline const date_time_t &msg_t::timestamp() const
  {
  return _timestamp;
  }
  };

#endif
