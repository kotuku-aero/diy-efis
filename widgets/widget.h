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
#ifndef __widget_h__
#define __widget_h__

#include "window.h"
#include "thread.h"
#include "can_aerospace.h"
#include <vector>

// return true if the value has changed
namespace kotuku {
template<typename _Ty>
bool assign_msg(_Ty new_value, _Ty &old_value)
  {
  if (old_value != new_value)
    {
    old_value = new_value;
    return true;
    }

  return false;
  }

class widget_t : public window_t, public canaerospace_provider_t::can_parameter_handler_t
  {
public:
  widget_t(widget_t &, const char *section);
  widget_t(canaerospace_provider_t *can_provider, screen_t *screen);
  widget_t(const char *name, widget_t &, const rect_t &);
  ~widget_t();

  // subscribe to a parameter
  result_t subscribe(uint16_t msg_id);
  result_t unsubscribe(uint16_t msg_id);
  result_t subscribe(uint16_t msg_id, canaerospace_provider_t::can_parameter_handler_t *);
  result_t unsubscribe(uint16_t msg_id, canaerospace_provider_t::can_parameter_handler_t *);

  canaerospace_provider_t *can_provider()
    {
    return _can_provider;
    }

  virtual const std::string &name() const
    {
    return _name;
    }

  bool draw_border() const
    {
    return _draw_border;
    }

  const std::string &section_name() const
    {
    return _section;
    }
protected:
  virtual bool ev_msg(const msg_t &) = 0;

  void set_can_provider(canaerospace_provider_t *can_provider);

  // look up a settings and return the enumeration value
  // returns -1 if setting not found otherwise integer index of lookup
  int lookup_enum_setting(const char *section, const char *name,
                          const char **values, int max_values);
  const font_t *lookup_font(const char *section, const char *name);
  color_t lookup_color(const char *section, const char *name);
  color_t lookup_color(const char *value);
  int lookup_enum(const char *value, const char **values, int max_values);
  pen_style lookup_pen_style(const char *value);
private:
  std::string _name;

  virtual void receive_parameter(canaerospace_provider_t *, const msg_t &);
  virtual bool is_equal(const can_parameter_handler_t &) const;

  canaerospace_provider_t *_can_provider;
  bool _draw_border;
  std::string _section;
  };
  };
#endif
