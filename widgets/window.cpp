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
#include "window.h"
#include "hal.h"
#include "application.h"

const kotuku::window_t::enumerate_children_flags kotuku::window_t::all_children      = 0x0000;
const kotuku::window_t::enumerate_children_flags kotuku::window_t::skip_invisible_children    = 0x0001;
const kotuku::window_t::enumerate_children_flags kotuku::window_t::skip_disabled_children      = 0x0002;
const kotuku::window_t::enumerate_children_flags kotuku::window_t::skip_transparent_children  = 0x0004;
const kotuku::window_t::window_scroll_flags kotuku::window_t::scroll_window_t_erase        = 0x0004;
const kotuku::window_t::window_scroll_flags kotuku::window_t::scroll_window_t_invalidate    = 0x0002;

kotuku::window_t::window_t(window_t &parent,
             const rect_t &window_rect,
             uint32_t id)
: _parent(&parent),
  canvas_t(window_rect),
  _id(id),
  _previous(0),
  _next(0),
  _child(0),
  _z_order(0)
  {
  if(_parent->_child != 0)
    {
    _next = _parent->_child;
    _next->_previous = this;
    }

  _parent->_child = this;
  }

inline kotuku::rect_t calculate_screen_rectangle(kotuku::screen_t *screen)
  {
  return kotuku::rect_t(0, 0, screen->screen_metrics()->screen_x, screen->screen_metrics()->screen_y);
  }

kotuku::window_t::window_t(screen_t *screen)
: _parent(0),
  canvas_t(screen, calculate_screen_rectangle(screen)),
  _id(0),
  _previous(0),
  _next(0),
  _child(0),
  _z_order(0)
  {
  }

kotuku::window_t::~window_t()
  {
  if(_previous != 0)
    _previous->_next = _next;
  if(_next != 0)
    _next->_previous = _previous;
  }

bool kotuku::window_t::paint(bool force_redraw)
  {
  bool is_painted = false;

  if(is_invalidated() || force_redraw)
    {
    update_window();
    is_updated();
    is_painted = true;
    }

  for(window_t *child = _child; child != 0; child = child->_next)
    is_painted = is_painted | child->paint(force_redraw);

  return is_painted;
  }


kotuku::window_t *kotuku::window_t::get_window(get_window_type type) const
  {
  window_t *child;
  switch(type)
    {
    case gw_first :
      child = _previous;
      while(child != 0)
        {
        if(child->_previous == 0)
          break;

        child = child->_previous;
        }
      break;
    case gw_last :
      child = _next;
      while(child != 0)
        {
        if(child->_next == 0)
          break;

        child = child->_next;
        }
      break;
    case gw_child :
      child = _child;
      break;
    case gw_parent :
      child = _parent;
      break;
    }

  return child;
  }

int kotuku::window_t::z_order() const
  {
  return _z_order;
  }

void kotuku::window_t::z_order(int order)
  {
  _z_order = order;
  }

void kotuku::window_t::set_window_order(window_t &child,
                                        window_t &insert_before)
  {
  // remove the window first
  window_t *ch = child._parent->_child;
  while(ch != 0)
    {
    if(ch == &child)
      break;

    ch = ch->_next;
    }

  if(ch != 0)
    {
    if(ch->_previous != 0)
      ch->_previous->_next = ch->_next;
    else
      ch->_parent = ch->_next;

    if(ch->_next != 0)
      ch->_next->_previous = ch->_previous;
    }

  // take ownership
  child._parent = this;

  // now find the insertion point otherwise place at start
  ch = _child;
  while(ch != 0 && ch != &insert_before)
    ch = ch->_next;

  if(ch != 0)
    {
    if(ch->_previous == 0)
      _child = &child;

    child._previous = ch->_previous;
    child._next = ch->_next;
    }
  else
    {
    if(_child != 0)
      {
      child._next = _child;
      _child->_previous = &child;
      }

    _child = &child;
    child._previous = 0;
    }
  }

size_t kotuku::window_t::count() const
  {
  size_t n = 0;
  window_t *child = _child;

  while(child != 0)
    {
    n++;
    child = child->_next;
    }

  return n;
  }

kotuku::window_t *kotuku::window_t::at_point(const point_t &pt)
  {
  window_t *child = _child;
  while(child != 0)
    {
    if(child->window_rect() && pt)
      break;

    child = child->_next;
    }

  return child;
  }

kotuku::window_t *kotuku::window_t::find_window(unsigned int id) const
  {
  window_t *child = _child;
  while(child != 0)
    {
    if(child->_id == id)
      break;

    child = child->_next;
    }

  return child;
  }

unsigned int kotuku::window_t::id() const
  {
  return _id;
  }

void kotuku::window_t::id(unsigned int id)
  {
  _id = id;
  }

void kotuku::window_t::scroll(const extent_t &how_much,
                              const rect_t &src_rect,
                              const rect_t &clip_rect,
                              window_scroll_flags flags,
                              rect_t *update_rect)
  {
  // when we scroll a window we perform a bit-blt using the canvas.
  // the sizes all need to be correct
  }

bool kotuku::window_t::visible() const
  {
  return _visible;
  }

void kotuku::window_t::visible(bool is_it)
  {
  _visible = is_it;
  }

kotuku::rect_t kotuku::window_t::get_window_rect(const char *section_name)
  {
  rect_t result;
  int value;
  application_t::instance->hal()->get_config_value(section_name, "origin-x", value);
  result.left = value;
  application_t::instance->hal()->get_config_value(section_name, "origin-y", value);
  result.top = value;
  application_t::instance->hal()->get_config_value(section_name, "extent-x", value);
  result.right = result.left + value;
  application_t::instance->hal()->get_config_value(section_name, "extent-y", value);
  result.bottom = result.top + value;

  return result;
  }

kotuku::window_t *kotuku::window_t::as_window()
  {
  return this;
  }

const kotuku::window_t *kotuku::window_t::as_window() const
  {
  return this;
  }
