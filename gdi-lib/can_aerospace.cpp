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
#include "can_aerospace.h"
#include "trace.h"
#include "hal.h"

static const int CANAS_DEFAULT_REPEAT_TIMEOUT_MSEC = 30 * 1000;

#ifdef __GNUC__
// range_inclusive() may produce a lot of these warnings
#  pragma GCC diagnostic ignored "-Wtype-limits"
#endif

enum message_group
  {
  msggroup_wtf,
  msggroup_parameter,
  msggroup_service
  };

static const uint16_t msgtype_emergency_event_min = 0;
static const uint16_t msgtype_emergency_event_max = 127;
static const uint16_t msgtype_node_service_high_min = 128;
static const uint16_t msgtype_node_service_high_max = 199;
static const uint16_t msgtype_user_defined_high_min = 200;
static const uint16_t msgtype_user_defined_high_max = 299;
static const uint16_t msgtype_normal_operation_min = 300;
static const uint16_t msgtype_normal_operation_max = 1799;
static const uint16_t msgtype_user_defined_low_min = 1800;
static const uint16_t msgtype_user_defined_low_max = 1899;
static const uint16_t msgtype_debug_service_min = 1900;
static const uint16_t msgtype_debug_service_max = 1999;
static const uint16_t msgtype_node_service_low_min = 2000;
static const uint16_t msgtype_node_service_low_max = 2031;

inline bool range_inclusive(uint16_t id, uint16_t min_value, uint16_t max_value)
  {
  return id >= min_value && id <= max_value;
  }

static message_group detect_message_group(uint16_t id)
  {
  if(range_inclusive(id, msgtype_emergency_event_min,
    msgtype_emergency_event_max))
    return msggroup_parameter;

  if(range_inclusive(id, msgtype_node_service_high_min,
    msgtype_node_service_high_max))
    return msggroup_service;

  if(range_inclusive(id, msgtype_user_defined_high_min,
    msgtype_user_defined_high_max))
    return msggroup_parameter;

  if(range_inclusive(id, msgtype_normal_operation_min,
    msgtype_normal_operation_max))
    return msggroup_parameter;

  if(range_inclusive(id, msgtype_user_defined_low_min,
    msgtype_user_defined_low_max))
    return msggroup_parameter;

  if(range_inclusive(id, msgtype_debug_service_min, msgtype_debug_service_max))
    return msggroup_parameter;

  if(range_inclusive(id, msgtype_node_service_low_min,
    msgtype_node_service_low_max))
    return msggroup_service;

  trace_warning("msggroup: failed to detect, msgid=%03x\n", (unsigned int )id);
  return msggroup_wtf;
  }

kotuku::canaerospace_provider_t::canaerospace_provider_t()
  : _publishing_enabled(false),
    thread_t(4096, this, do_run)
  {
  the_hal()->set_can_provider(this);
  }

bool kotuku::canaerospace_provider_t::publishing_enabled(bool is_it)
  {
  bool was_it = _publishing_enabled;
  _publishing_enabled = is_it;
  return was_it;
  }

result_t kotuku::canaerospace_provider_t::receive(const can_msg_t &can_msg)
  {
  if(!_publishing_enabled)
    return s_ok;

  msg_t msg(can_msg);

  // todo: handle extended frames
  switch(detect_message_group((uint16_t) can_msg.id))
    {
    case msggroup_parameter:
      {
      subscriptions_t::iterator it = _subscriptions.find((uint16_t) can_msg.id);
      if(it != _subscriptions.end())
        {
        for(subscribers_t::iterator sub_it = it->second.begin();
          sub_it != it->second.end(); sub_it++)
          {
          // receive the parameter to the subscriber
          (*sub_it)->receive_parameter(this, msg);
          }
        }
      }
      break;
    case msggroup_service:
      {
      services_t::iterator it = _services.find(msg.service_code());
      if(it != _services.end())
        it->second->service(this, it->first, msg);
      }
      break;
    default:
      return e_generic_error;
    }

  return s_ok;
  }

result_t kotuku::canaerospace_provider_t::create_service(uint8_t service_id, can_service_t *service)
  {
  if(_services.find(service_id) != _services.end())
    return e_generic_error;

  _services.insert(services_t::value_type(service_id, service));
  return s_ok;
  }

result_t kotuku::canaerospace_provider_t::subscribe(uint16_t id, can_parameter_handler_t *handler)
  {
  subscriptions_t::iterator it = _subscriptions.find(id);

  if(it == _subscriptions.end())
    it =
    _subscriptions.insert(subscriptions_t::value_type(id, subscribers_t())).first;

  for(subscribers_t::iterator sub_it = it->second.begin();
    sub_it != it->second.end(); sub_it++)
    {
    // if the handler already registered then we won't let it be re-registered.
    if((*sub_it)->is_equal(*handler))
      return e_bad_parameter;
    }

  it->second.push_back(handler);

  return s_ok;
  }

result_t kotuku::canaerospace_provider_t::unsubscribe(uint16_t id, can_parameter_handler_t *handler)
  {
  subscriptions_t::iterator it = _subscriptions.find(id);

  if(it == _subscriptions.end())
    return e_bad_parameter;

  for(subscribers_t::iterator sub_it = it->second.begin();
    sub_it != it->second.end(); sub_it++)
    {
    // if the handler already registered then we won't let it be re-registered.
    can_parameter_handler_t *other = *sub_it;
    if(other != 0 && other->is_equal(*handler))
      {
      it->second.erase(sub_it);
      return s_ok;
      }
    }

  return e_bad_parameter;
  }

uint32_t kotuku::canaerospace_provider_t::do_run(void *parg)
  {
  return reinterpret_cast<canaerospace_provider_t *>(parg)->run();
  }

uint32_t kotuku::canaerospace_provider_t::run()
  {
  while(!should_terminate())
    {
    event_t::lock_t butler(_doorbell);

    bool do_publish = true;
    can_msg_t msg;
    while(do_publish)
      {
        {
        critical_section_t::lock_t lock(_cs);
        do_publish = _publishing.size() > 0;
        if(do_publish)
          {
          msg = _publishing.front();
          _publishing.pop_front();
          }
        }

      if(do_publish)
        the_hal()->publish(msg);
      }
    }

  return 0;
  }

result_t kotuku::canaerospace_provider_t::publish(const msg_t &msg)
  {
  if((msg.flags & LOOPBACK_MESSAGE) != 0)
    return receive(msg);

  critical_section_t::lock_t lock(_cs);
  _publishing.push_back(msg);
  _doorbell.set();

  return s_ok;
  }

kotuku::canaerospace_provider_t::can_service_t::can_service_t(uint8_t service_code)
  {
  _service_code = service_code;
  }

uint32_t kotuku::canaerospace_provider_t::can_service_t::request_id(bool is_reply, bool extended)
  {
  // TODO: implement this
  return 0;
  }

static kotuku::canaerospace_provider_t *provider = 0;

static uint8_t node_id()
  {
  if(provider == 0)
    kotuku::the_hal()->get_can_provider(&provider);

  if(provider == 0)
    return 0;

  return provider->node_id();
  }

uint8_t kotuku::msg_t::message_code() const
  {
  return msg.raw[3];
  }

uint16_t kotuku::msg_t::message_id() const
  {
  return (uint16_t) (id);
  }

uint8_t kotuku::msg_t::service_code() const
  {
  return msg.raw[2];
  }

uint8_t kotuku::msg_t::msg_data_u8(int index) const
  {
  return msg.raw[index + 4];
  }

uint16_t kotuku::msg_t::msg_data_u16(int index) const
  {
  return to_local_endian(*((uint16_t *) (&msg.raw[index == 0 ? 4 : 6])));;
  }

uint32_t kotuku::msg_t::msg_data_u32() const
  {
  return to_local_endian(*((uint32_t *) (&msg.raw[4])));
  }

int8_t kotuku::msg_t::msg_data_8(int index) const
  {
  return (char) msg.raw[index + 4];
  }

int16_t kotuku::msg_t::msg_data_16(int index) const
  {
  return (short) to_local_endian(*((uint16_t *) (&msg.raw[index == 0 ? 4 : 6])));
  }

int32_t kotuku::msg_t::msg_data_32() const
  {
  return (int32_t) to_local_endian(*((uint32_t *) (&msg.raw[4])));
  }

float kotuku::msg_t::msg_data_float() const
  {
  uint32_t value = to_local_endian(*((uint32_t *) (&msg.raw[4])));

  return *((float *) &value);
  }

uint8_t kotuku::msg_t::data_type() const
  {
  return msg.raw[1];
  }

uint8_t kotuku::msg_t::operator[](int index) const
  {
  return msg.raw[index + 4];
  }

kotuku::msg_t::msg_t(const msg_t &msg)
: _timestamp(msg._timestamp)
    {
    can_msg_t::operator =(msg);
    }

kotuku::msg_t::msg_t(const can_msg_t &msg)
  {
  can_msg_t::operator =(msg);
  }

uint8_t kotuku::msg_t::node_id() const
  {
  return msg.raw[0];
  }

const kotuku::msg_t &kotuku::msg_t::operator =(const msg_t &msg)
  {
  _timestamp = msg._timestamp;
  can_msg_t::operator =(msg);

  return *this;
  }

static long msg_code = 0;
static uint8_t message_code()
  {
  long value = interlocked_increment(msg_code);

  return (uint8_t) value;
  }

kotuku::msg_t::msg_t(uint16_t label)
  {
  id = label;
  length(4);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_nodata;
  msg.raw[2] = 0;
  msg.raw[3] = message_code();
  }

kotuku::msg_t::msg_t(uint16_t label, uint8_t value, uint8_t service_code)
  {
  id = label;
  length(5);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_uchar;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  msg.raw[4] = value;
  }

kotuku::msg_t::msg_t(uint16_t label, uint16_t value, uint8_t service_code)
  {
  id = label;
  length(6);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_ushort;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  msg.raw[4] = (uint8_t) (value >> 8);
  msg.raw[5] = (uint8_t) (value);
  }

kotuku::msg_t::msg_t(uint16_t label, uint32_t value, uint8_t service_code)
  {
  id = label;
  length(8);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_ulong;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  msg.raw[4] = (uint8_t) (value >> 24);
  msg.raw[5] = (uint8_t) (value >> 16);
  msg.raw[6] = (uint8_t) (value >> 8);
  msg.raw[7] = (uint8_t) (value);
  }

kotuku::msg_t::msg_t(uint16_t label, int8_t value, uint8_t service_code)
  {
  id = label;
  length(5);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_char;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  msg.raw[4] = value;
  }

kotuku::msg_t::msg_t(uint16_t label, int16_t value, uint8_t service_code)
  {
  id = label;
  length(6);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_short;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  msg.raw[4] = (uint8_t) (value >> 8);
  msg.raw[5] = (uint8_t) (value);
  }

kotuku::msg_t::msg_t(uint16_t label, int32_t value, uint8_t service_code)
  {
  id = label;
  length(8);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_long;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  msg.raw[4] = (uint8_t) (value >> 24);
  msg.raw[5] = (uint8_t) (value >> 16);
  msg.raw[6] = (uint8_t) (value >> 8);
  msg.raw[7] = (uint8_t) (value);
  }

kotuku::msg_t::msg_t(uint16_t label, float fvalue, uint8_t service_code)
  {
  id = label;
  length(8);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_float;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();
  uint32_t value = *((uint32_t *) &fvalue);
  msg.raw[4] = (uint8_t) (value >> 24);
  msg.raw[5] = (uint8_t) (value >> 16);
  msg.raw[6] = (uint8_t) (value >> 8);
  msg.raw[7] = (uint8_t) (value);
  }

kotuku::msg_t::msg_t(uint16_t label, const uint8_t data[4], size_t _length, uint8_t service_code)
  {
  _length = _length > 4 ? 4 : _length;
  id = label;
  length(_length + 4);
  msg.raw[0] = ::node_id();
  msg.raw[1] = datatype_uchar4;
  msg.raw[2] = service_code;
  msg.raw[3] = message_code();

  for(size_t i = 4; i < _length; i++)
    msg.raw[i] = data[i];
  }


std::string kotuku::msg_t::to_string(const char *format) const
  {
  char buffer[64] = { 0 };
  switch(data_type())
    {
    case datatype_error :
      strcpy(buffer, "error");
      break;;
    case datatype_float :
      snprintf(buffer, 64, format, value<double>());
      break;
    case datatype_long :
      snprintf(buffer, 64, format, value<int32_t>());
      break;
    case datatype_memid :
    case datatype_chksum :
    case datatype_ulong :
      snprintf(buffer, 64, format, value<uint32_t>());
      break;
    case datatype_short :
      snprintf(buffer, 64, format, value<int16_t>());
      break;
    case datatype_ushort :
      snprintf(buffer, 64, format, value<uint16_t>());
      break;
    case datatype_char :
      snprintf(buffer, 64, format, value<int8_t>());
      break;
    case datatype_uchar :
      snprintf(buffer, 64, format, value<uint8_t>());
      break;
    case datatype_short2 :
      snprintf(buffer, 64, format, value<int16_t>(0), value<int16_t>(1));
      break;
    case datatype_ushort2 :
      snprintf(buffer, 64, format, value<uint16_t>(0), value<uint16_t>(1));
      break;
    case datatype_bshort2 :
      break;
    case datatype_achar4 :
    case datatype_char4 :
      snprintf(buffer, 64, format, value<int8_t>(0), value<int8_t>(1), value<int8_t>(2), value<int8_t>(3));
      break;
    case datatype_uchar4 :
      snprintf(buffer, 64, format, value<uint8_t>(0), value<uint8_t>(1), value<uint8_t>(2), value<uint8_t>(3));
      break;
    case datatype_bchar4 :
      break;
    case datatype_achar2 :
    case datatype_char2 :
      snprintf(buffer, 64, format, value<int8_t>(0), value<int8_t>(1));
      break;
    case datatype_uchar2 :
      snprintf(buffer, 64, format, value<uint8_t>(0), value<uint8_t>(1));
      break;
    case datatype_bchar2 :
      break;
    case datatype_achar :
      snprintf(buffer, 64, format, value<int8_t>(0));
      break;
    case datatype_achar3 :
    case datatype_char3 :
      snprintf(buffer, 64, format, value<int8_t>(0), value<int8_t>(1), value<int8_t>(2));
      break;
    case datatype_uchar3 :
      snprintf(buffer, 64, format, value<uint8_t>(0), value<uint8_t>(1), value<uint8_t>(2));
      break;
    }

  return buffer;
  }
