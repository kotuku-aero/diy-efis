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
#include "std_nodesync.h"

kotuku::std_nodesync_t::std_nodesync_t()
  : can_service_t(msg_t::service_nss)
  {
  }

result_t kotuku::std_nodesync_t::service(canaerospace_provider_t *provider, uint8_t service_channel, const msg_t &msg)
  {
  // the message datatype ULONG is a time_t that is used to set the HAL time
  // TODO: implement this
  return s_ok;
  }

result_t kotuku::std_nodesync_t::service_response(canaerospace_provider_t *, uint8_t service_channel, const msg_t &)
  {
  return s_ok;
  }
