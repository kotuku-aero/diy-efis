#define GLUON_INTERNAL
#include "gluon_private.h"

int isopenlibm(void)
  {
  return 1;
  }

typedef union {
  union IEEEf2bits val;
  float float_val;
} u_float_t;

typedef union {
  union IEEEd2bits val;
  double double_val;
  } u_double_t;

u_double_t __nan = {
  .val.bits.exp = 0x03ff,
  .val.bits.sign = 0,
  .val.bits.manl = 1,
  .val.bits.manh = 0
  };

u_float_t __nanf = {
  .val.bits.exp = 0x0ff,
  .val.bits.sign = 0,
  .val.bits.man = 1
  };

u_double_t __infinity = {
  .val.bits.exp = 0x03ff,
  .val.bits.sign = 0,
  .val.bits.manl = 0,
  .val.bits.manh = 0
  };

u_float_t __infinityf = {
  .val.bits.exp = 0x0ff,
  .val.bits.sign = 0,
  .val.bits.man = 0
  };
