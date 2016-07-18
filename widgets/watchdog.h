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
#ifndef __watchdog_h__
#define __watchdog_h__

#include "../gdi-lib/thread.h"
#include "../gdi-lib/can_aerospace.h"

namespace kotuku {

class watchdog_settings_t;
class layout_window_t;
/**
 * Callback function to perform any application specific failover tests
 * @return  false if the application has failed
 */
typedef bool (*callback_fn)(void *parg);

class watchdog_t : public canaerospace_provider_t::can_service_t
  {
public:
  watchdog_t(layout_window_t *root_window, canaerospace_provider_t *app);
  virtual ~watchdog_t();

  virtual result_t service(canaerospace_provider_t *,
                           uint8_t service_channel, const msg_t &);
  virtual result_t service_response(canaerospace_provider_t *,
                                    uint8_t service_channel, const msg_t &);


  /**
   * Must be called on the main thread of the application or a
   * worker thread
   * @param test              function to call each time a check is made
   * @param monitor_interval  how many milliseconds to wait for a test
   */
  void run(callback_fn test, void *parg, uint16_t monitor_interval);
private:
  watchdog_settings_t *_settings;
  layout_window_t *_root_window;
  };
  };

#endif /* WATCHDOG_H_ */
