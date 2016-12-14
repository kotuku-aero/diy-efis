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
#include <string>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <unistd.h>

#include "../gdi-lib/trace.h"

#include "linux_can_driver.h"
#include "../gdi-lib/can_aerospace.h"

kotuku::linux_can_driver_t::linux_can_driver_t() :
    thread_t(4096, this, do_run)
  {
  _provider = 0;
  _can_socket = 0;
  }

kotuku::linux_can_driver_t::~linux_can_driver_t()
  {
  // TODO Auto-generated destructor stub
  }

const int canfd_on = 1;

result_t kotuku::linux_can_driver_t::initialize(const char *driver,
    int receive_timeout, int send_timeout)
  {
  struct sockaddr_can addr;
  struct ifreq ifr;

  trace_info("Create SocketCAN driver on %s\n", driver);

  if((_can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
    trace_error("Error while opening socketcan device %s\n", driver);
    return e_generic_error;
    }

  strcpy(ifr.ifr_name, driver);
  if(ioctl(_can_socket, SIOCGIFINDEX, &ifr) < 0)
    {
    trace_error("Error when attempting to get can socket index for device %s\n",
        driver);
    return e_generic_error;
    }

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
/*
  trace_info("Set socket to raw frames\n");
  setsockopt(_can_socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on,
      sizeof(canfd_on)); */

  timeval timeout;
  int result;

  if(receive_timeout > 0)
    {
    trace_info("Set socket recv timeout to %d\n", receive_timeout);
    timeout.tv_sec = receive_timeout / 1000;
    timeout.tv_usec = (receive_timeout - (timeout.tv_sec * 1000)) * 1000;

    if((result = setsockopt(_can_socket, SOL_SOCKET, SO_RCVTIMEO,
        (char *) &timeout, sizeof(timeout))) < 0)
      trace_error("Setsockopt on can socket failed %d\n", result);
    }

  if(send_timeout > 0)
    {
    trace_info("Set socket xmit timeout to %d\n", send_timeout);
    timeout.tv_sec = send_timeout / 1000;
    timeout.tv_usec = (send_timeout - (timeout.tv_sec * 1000)) * 1000;

    if((result = setsockopt(_can_socket, SOL_SOCKET, SO_SNDTIMEO,
        (char *) &timeout, sizeof(timeout))) < 0)
      trace_error("Setsockopt on can socket failed %d\n", result);
    }

  int sock_buf_size;
  int i = sizeof(sock_buf_size);
  int nbytes = getsockopt(_can_socket, SOL_SOCKET, SO_RCVBUF, &sock_buf_size,
      (socklen_t *) &i);

  trace_debug("Current sock_buf_size %X\n", sock_buf_size);

  sock_buf_size = 0x80000;

  nbytes = setsockopt(_can_socket, SOL_SOCKET, SO_RCVBUF, &sock_buf_size,
      sizeof(sock_buf_size));

  if(bind(_can_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
    trace_error("Error when binding to can socket\n");
    return e_generic_error;
    }

  trace_info("Socket bound OK, start the worker\n");
  resume();     // start the worker...

  return s_ok;
  }

void print_can_frame(const can_frame &frame, const char *dir)
  {
  if(kotuku::the_hal()->trace_level() >= 7)
    {
    char bytes[16];
    char dbgmsg[64];
    sprintf(dbgmsg, "Canbus %s :(%d) [%d] ", dir, frame.can_id, frame.can_dlc);
    for(size_t i = 0; i < frame.can_dlc; i++)
      {
      sprintf(bytes, "%02.2x ", frame.data[i]);
      strcat(dbgmsg, bytes);
      }

    trace_debug("%s\n", dbgmsg);
    }
  }

result_t kotuku::linux_can_driver_t::publish(const can_msg_t &msg)
  {
  if(_can_socket == 0)
    return e_invalid_handle;

  can_frame frame;

  frame.can_id = msg.id;
  frame.can_dlc = msg.length();

  for(size_t i = 0; i < msg.length(); i++)
    frame.data[i] = msg.msg.raw[i];

#ifdef DEBUG_CAN_IO
  print_can_frame(frame, "send");
#endif

  int bytes_to_write = sizeof(can_frame);

  int bytes_written = write(_can_socket, &frame, bytes_to_write);

  if(bytes_to_write != bytes_written)
    trace_error("Error when sending frame, requested %d, sent %d\n",
        bytes_to_write, bytes_written);

  return s_ok;
  }

result_t kotuku::linux_can_driver_t::set_can_provider(
    canaerospace_provider_t *prov)
  {
  _provider = prov;

  return s_ok;
  }

unsigned int kotuku::linux_can_driver_t::do_run(void *pthis)
  {
  return reinterpret_cast<linux_can_driver_t *>(pthis)->run();
  }

unsigned int kotuku::linux_can_driver_t::run()
  {
  while(true)
    {
    // read from the can bus
    can_frame frame;
    if(read(_can_socket, &frame, sizeof(frame)) > 0)
      {
      can_msg_t msg;
      msg.id = frame.can_id;
      msg.length(frame.can_dlc & 0x0f);
      for(size_t i = 0; i < msg.length(); i++)
        msg.msg.raw[i] = frame.data[i];

#ifdef DEBUG_CAN_IO
      print_can_frame(frame, "recv");
#endif

      // process the received message
      if(_provider != 0)
        _provider->receive(msg);
      else
        trace_debug("Message discarded as no provider\n");
      }
    }

  return 0;
  }

