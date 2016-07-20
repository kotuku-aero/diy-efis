#ifndef __ftdi_can_device_h__
#define __ftdi_can_device_h__

#include "win32_hal_device.h"
#include "../../gdi-lib/can_driver.h"

#include <deque>
#include <vector>
#include <string>

class can_worker_t;
namespace kotuku {

  class serial_can_device_t : public can_device_t {
  public:
    serial_can_device_t(const std::string &name, int baud_rate);

    virtual result_t send(const can_msg_t &);
    virtual result_t set_can_provider(canaerospace_provider_t *);

    result_t reset();
  private:
    friend class can_worker_t;
    can_worker_t *_worker;
    canaerospace_provider_t *_provider;

    result_t process_msg(const can_msg_t &);

    typedef std::deque<can_msg_t> can_msg_queue_t;
    can_msg_queue_t _rcv_queue;

    unsigned long _read_timeout;
    event_t *_doorbell;
    critical_section_t _cs;
  };
}

#endif
