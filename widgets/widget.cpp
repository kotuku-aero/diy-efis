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
#include "widget.h"
#include "application.h"

#include "fonts.h"
#include "colors.h"

kotuku::widget_t::widget_t(widget_t &parent, const char *section)
:  window_t(parent, window_t::get_window_rect(section)),
   _can_provider(parent.can_provider()),
   _section(section)
  {
  application_t::instance->hal()->get_config_value(section, "name", _name);

  if(failed(application_t::instance->hal()->get_config_value(section, "draw-border", _draw_border)))
    _draw_border = false;

  int value;
  if(succeeded(application_t::instance->hal()->get_config_value(section, "z-order", value)))
    z_order(value);
  }

inline kotuku::rect_t calculate_screen_rectangle(kotuku::screen_t *screen)
  {
  return kotuku::rect_t(0, 0, screen->screen_metrics()->screen_x, screen->screen_metrics()->screen_y);
  }

kotuku::widget_t::widget_t(canaerospace_provider_t *can_provider, screen_t *screen)
: window_t(screen)
  {
  set_can_provider(can_provider);
  }

kotuku::widget_t::widget_t(const char *name, widget_t &parent, const rect_t &r)
: window_t(parent, r), _name(name), _can_provider(parent.can_provider())
  {
  }

kotuku::widget_t::~widget_t()
  {
  }

void kotuku::widget_t::set_can_provider(canaerospace_provider_t *can_provider)
  {
  _can_provider = can_provider;
  }

result_t kotuku::widget_t::subscribe(uint16_t msg_id)
  {
  return _can_provider->subscribe(msg_id, this);
  }

result_t kotuku::widget_t::unsubscribe(uint16_t msg_id)
  {
  return _can_provider->unsubscribe(msg_id, this);
  }

result_t kotuku::widget_t::subscribe(uint16_t msg_id, canaerospace_provider_t::can_parameter_handler_t *hndlr)
  {
  return _can_provider->subscribe(msg_id, hndlr);
  }

result_t kotuku::widget_t::unsubscribe(uint16_t msg_id, canaerospace_provider_t::can_parameter_handler_t *hndlr)
  {
  return _can_provider->unsubscribe(msg_id, hndlr);
  }

void kotuku::widget_t::receive_parameter(canaerospace_provider_t *prov,
    const msg_t &msg)
  {
  ev_msg(msg);
  }

bool kotuku::widget_t::is_equal(const can_parameter_handler_t &other) const
  {
  return false;     // TODO:
  }

int kotuku::widget_t::lookup_enum_setting(const char *section, const char *name,
    const char **values, int max_values)
  {
  std::string value;
  if(failed(application_t::instance->hal()->get_config_value(section, name, value)))
    return -1;

  for(int i = 0; i < max_values; i++)
    if(value == values[i])
      return i;

  return -1;
  }

static const char *font_names[] =
{
    "arial_9_font",
    "arial_12_font",
    "arial_15_font",
    "arial_20_font",
    "arial_25_font"
};

const font_t *kotuku::widget_t::lookup_font(const char *section,
  const char *name)
{
switch (lookup_enum_setting(section, name, font_names, 5))
  {
case 0:
  return &arial_9_font;
case 1:
  return &arial_12_font;
case 2:
  return &arial_15_font;
case 3:
  return &arial_20_font;
case 4:
  return &arial_25_font;
  }

return 0;
}

struct color_lookup_t {
  const char *name;
  color_t color;
};

static  color_lookup_t color_lookups[] = {
    { "color_white", RGB(255, 255, 255) },
    { "color_black", RGB(0, 0, 0) },
    { "color_gray", RGB(128, 128, 128) },
    { "color_light_gray", RGB(192, 192, 192) },
    { "color_dark_gray", RGB(64, 64, 64) },
    { "color_red", RGB(255, 0, 0) },
    { "color_pink", RGB(255, 128, 128) },
    { "color_blue", RGB(0, 0, 255) },
    { "color_green", RGB(0, 255, 0) },
    { "color_lightgreen", RGB(0, 192, 0) },
    { "color_yellow", RGB(255, 255, 64) },
    { "color_magenta", RGB(255, 0, 255) },
    { "color_cyan", RGB(0, 255, 255) },
    { "color_pale_yellow", RGB(255, 255, 208) },
    { "color_light_yellow", RGB(255, 255, 128) },
    { "color_lime_green", RGB(192, 220, 192) },
    { "color_teal", RGB(64, 128, 128) },
    { "color_dark_green", RGB(0, 128, 0) },
    { "color_maroon", RGB(128, 0, 0) },
    { "color_purple", RGB(128, 0, 128) },
    { "color_orange", RGB(255, 192, 64) },
    { "color_khaki", RGB(167, 151, 107) },
    { "color_olive", RGB(128, 128, 0) },
    { "color_brown", RGB(192, 128, 32) },
    { "color_navy", RGB(0, 64, 128) },
    { "color_light_blue", RGB(128, 128, 255) },
    { "color_faded_blue", RGB(192, 192, 255) },
    { "color_hollow", 0xffffffff },
    { "color_lightgrey", RGB(192, 192, 192) },
    { "color_darkgrey", RGB(64, 64, 64) },
    { "color_paleyellow", RGB(255, 255, 208) },
    { "color_lightyellow", RGB(255, 255, 128) },
    { "color_limegreen", RGB(192, 220, 192) },
    { "color_darkgreen", RGB(0, 128, 0) },
    { "color_lightblue", RGB(128, 128, 255) },
    { "color_fadedblue", RGB(192, 192, 255) },
    { 0, 0 }
};

color_t kotuku::widget_t::lookup_color(const char *section,
  const char *name)
  {
  std::string value;
  if(failed(application_t::instance->hal()->get_config_value(section, name, value)))
    return color_black;

  return lookup_color(value.c_str());
  }

color_t kotuku::widget_t::lookup_color(const char *value)
  {
  color_lookup_t *color = color_lookups;

  while(color->name != 0)
    {
    if(strcmp(value, color->name) == 0)
      return color->color;

    color++;
    }

  return color_black;
  }

int kotuku::widget_t::lookup_enum(const char *value, const char **values,
  int max_values)
  {
  for(int i = 0; i < max_values; i++)
    if(strcmp(values[i], value)== 0)
      return i;

  return -1;
  }

struct pen_value_lookup_t {
  const char *name;
  pen_style style;
};

static const pen_value_lookup_t pen_values[] = {
    { "ps_solid", ps_solid },
    { "ps_dash", ps_dash },
    { "ps_dot", ps_dot },
    { "ps_dash_dot", ps_dash_dot },
    { "ps_dash_dot_dot", ps_dash_dot_dot },
    { "ps_null", ps_null },
    { 0, ps_solid }
};

pen_style kotuku::widget_t::lookup_pen_style(const char *value)
  {
  const pen_value_lookup_t *lookup = pen_values;

  while(lookup->name != 0)
    {
    if(strcmp(value, lookup->name)== 0)
      return lookup->style;

    lookup++;
    }

  return ps_solid;
  }
