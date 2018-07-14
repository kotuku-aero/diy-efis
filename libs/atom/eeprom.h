#ifndef __eeprom_h__
#define __eeprom_h__

#include "../../diy-efis/libs/neutron/bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

result_t eeprom_init(bool init_mode, int i2c_channel, uint32_t eeprom_size);
  
#ifdef __cplusplus
}
#endif


#endif
