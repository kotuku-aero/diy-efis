#ifndef __deque_h__
#define __deque_h__

// internal file used to expose deque structures for the atom microkernel
#include "../../libs/neutron/neutron.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _deque
  {
  semaphore_p readers;
  semaphore_p writers;
  void *buffer;
  void *back;
  void *front;
  void *end;
  uint16_t length;      // number of elements that can be queued
  uint16_t size;        // size of elements in the queue
  uint16_t count;
  } deque_t;
  
#ifdef __cplusplus
  }
#endif

#endif
