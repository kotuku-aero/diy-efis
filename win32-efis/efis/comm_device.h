#ifndef __comm_device_h__
#define __comm_device_h__

#include "win32_hal.h"
#include "win32_hal_device.h"

namespace kotuku {
class comm_device_t : public hal_device_t {
public:
  comm_device_t(const char *name, const char *com_port);

  void device_close();
  bool device_request(device_t::overlapped_t *);
  result_t device_cancel_io();

  void set_comm_state(int baud_rate);

  // syncronous send of init data
  void send_msg(const unsigned char *, size_t length);
private:
  comm_device_t(const comm_device_t &);
  comm_device_t &operator =(const comm_device_t &);
  // called back from the operating system
  static VOID CALLBACK CommCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

  bool process_ioctl(device_t::overlapped_t *ol);
  bool process_read(device_t::overlapped_t *ol);
  bool process_write(device_t::overlapped_t *ol);

  HANDLE _hndl;

  // event handler for the driver
  event_t *_event_handler;
  };

  }

#endif
