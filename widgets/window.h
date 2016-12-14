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
#ifndef __window_h__
#define __window_h__

#include "canfly.h"
#include "rect.h"
#include "canvas.h"

namespace kotuku {
class window_t;

class window_t : public canvas_t
  {
public:
  // this is used to set window_t rectangles to a default (-1 as all members)
  static rect_t default_rect;
  static rect_t empty_rect;

  static rect_t get_window_rect(const char *section_name);

  typedef uint32_t enumerate_children_flags;
  static const enumerate_children_flags all_children;
  static const enumerate_children_flags skip_invisible_children;
  static const enumerate_children_flags skip_disabled_children;
  static const enumerate_children_flags skip_transparent_children;

  typedef uint32_t window_scroll_flags;
  static const window_scroll_flags scroll_window_t_erase;
  static const window_scroll_flags scroll_window_t_invalidate;

  enum mouse_key
    {
    mk_left,
    mk_middle,
    mk_right
      };

  // these are the defined window_ts.
  static const rect_t rect_default;

  // this is the total screen area window_t. Use it to create
  // child windows. The window_t cannot receive the focus, nor
  // does it paint the background, or respond to paint events
  // The first window_t created should be:
  //
  //  class my_window_t : public window_t {
  //    ....
  //  };
  //
  //  my_window_t main_window_t(window_t::screen,
  //                        window_t::screen.window_t_rect());
  //
  //  From there any number of window_ts can be created.
  //
  //  Alternately you can create tiled windows using the screen as the parent.

  window_t(window_t &parent, const rect_t &window_t_rect,
           uint32_t id = 0);
  virtual ~window_t();

  enum get_window_type
    {
    gw_first,
    gw_last,
    gw_child,
    gw_parent
      };

  virtual window_t *get_window(get_window_type) const;

  virtual void set_window_order(window_t &child, window_t &insert_before);
  virtual size_t count() const;

  // return the window at a point
  virtual window_t *at_point(const point_t &);

  // get the window_t given a window_t ID
  virtual window_t *find_window(unsigned int) const;

  // returns the _id of the window_t.
  virtual unsigned int id() const;
  virtual void id(unsigned int);

  // scroll a window client area
  virtual void scroll(const extent_t &how_much, const rect_t &src_rect,
                      const rect_t &clip_rect, window_scroll_flags, rect_t *update_rect = 0);

  virtual bool visible() const;
  virtual void visible(bool);

  window_t *parent() const
    {
    return _parent;
    }

  virtual int z_order() const;
  virtual void z_order(int);

  /**
   * Paint the screen.  Will call children that are visible then call
   * update_window.
   * @param force_redraw  Always draw
   * @return  true if the windows performed the operation.
   */
  virtual bool paint(bool force_redraw);

  virtual window_t *as_window();
  virtual const window_t *as_window() const;
protected:
  // used to create the root window
  window_t(screen_t *);

  /**
   * Must be implemented by the window to draw the scene onto the background
   * canvas
   */
  virtual void update_window() = 0;
private:
  window_t &operator=(const window_t &);
  window_t(const window_t &);

  // we always have a parent window
  window_t *_parent;

  window_t *_previous;
  window_t *_next;
  window_t *_child;

  // this is our position relative to our parent
  bool _visible;
  unsigned int _id;
  int _z_order;
  };
  };
  
#endif
