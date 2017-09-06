/*
 * electron.h
 *
 *  Created on: 16/02/2017
 *      Author: petern
 */

#ifndef ELECTRON_H_
#define ELECTRON_H_

#include "../../libs/neutron/bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

extern result_t electron_init(const char *reg_path, bool factory_reset);

extern result_t create_root_screen(gdi_dim_t x, gdi_dim_t y, const char *device, int display_mode);

#ifdef __cplusplus
}
#endif


#endif /* ELECTRON_H_ */
