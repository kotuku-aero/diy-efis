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
#ifndef __can_driver_h__
#define __can_driver_h__

#include "stddef.h"
#include "errors.h"

namespace kotuku {

class can_msg_t : public ::can_msg_t
  {
public:
  can_msg_t();
  can_msg_t(const can_msg_t &);
  const can_msg_t &operator=(const can_msg_t &);

  bool is_extended() const
    {
    return (flags & EXTENDED_ADDRESS) != 0;
    }

  bool is_reply() const
    {
    return (flags & REPLY_MSG) != 0;
    }

  size_t length() const
    {
    return flags & DLC_MASK;
    }

  void length(size_t len)
    {
    flags = (flags & ~DLC_MASK) | (len & DLC_MASK);
    }

  void is_reply(bool value)
    {
    flags = (flags & ~REPLY_MSG) | (value ? REPLY_MSG : 0);
    }

  void is_extended(bool value)
    {
    flags = (flags & ~EXTENDED_ADDRESS) | (value ? EXTENDED_ADDRESS : 0);
    }
  };
  };

#endif
