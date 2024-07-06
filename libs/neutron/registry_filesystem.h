#ifndef __registry_fs_h__
#define __registry_fs_h__

#include "../../libs/neutron/neutron.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  extern result_t create_registry_fs(memid_t root, filesystem_t **handle);
  
#ifdef __cplusplus
  }
#endif

#endif