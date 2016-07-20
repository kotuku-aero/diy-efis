#ifndef __win32_hal_device_h__
#define __win32_hal_device_h__

#include "../../gdi-lib/errors.h"
#include "../../gdi-lib/device.h"
#include "../../gdi-lib/hal.h"
#include <string>

namespace kotuku {

class can_msg_t;
// this is a class used to implement a windows 32 emulation device
class hal_device_t {
public:
  virtual void device_close() = 0;
  virtual bool device_request(device_t::overlapped_t *) = 0;
  virtual result_t device_cancel_io() = 0;

  const std::string name() const { return _name; }
protected:
  hal_device_t(const char *name)
    : _name(name)
    {
    }
private:
  std::string _name;
  };

class can_device_t {
  public:
    virtual result_t send(const can_msg_t &)= 0;
    virtual result_t set_can_provider(canaerospace_provider_t *) = 0;
  }; 
}

#endif
