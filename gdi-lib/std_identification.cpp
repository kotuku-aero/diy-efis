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
#include "std_identification.h"

kotuku::std_identification_t::std_identification_t(uint8_t hardware_revision,  uint8_t software_revision, uint8_t id_distribution, uint8_t header_type)
  : _hardware_revision(hardware_revision),
    _software_revision(software_revision),
    _id_distribution(id_distribution),
    _header_type(header_type),
    can_service_t(msg_t::service_ids)
  {
  }

result_t kotuku::std_identification_t::service(canaerospace_provider_t *, uint8_t service_channel, const msg_t &)
  {
  // this service does not request anything from a remote channel so we ignore it.
  // derived class can handle this as required
  return canas_err_logic;
  }

result_t kotuku::std_identification_t::service_response(canaerospace_provider_t *provider, uint8_t service_channel, const msg_t &request_msg)
  {
  msg_t reply(request_id(true));

  reply.msg.raw[0] = provider->node_id();        // this is who we are
  reply.msg.raw[1] = msg_t::datatype_achar4;
  reply.msg.raw[2] = 0;                          // service code
  reply.msg.raw[3] = request_msg.message_code(); // echo the message code to the requestor
  reply.msg.raw[4] = _hardware_revision;
  reply.msg.raw[5] = _software_revision;
  reply.msg.raw[6] = _id_distribution;
  reply.msg.raw[7] = _header_type;

  provider->publish(reply);

  return s_ok;
  }
