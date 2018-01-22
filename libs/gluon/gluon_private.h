/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * from: @(#)fdlibm.h 5.1 93/09/24
 * $FreeBSD: src/lib/msun/src/math_private.h,v 1.34 2011/10/21 06:27:56 das Exp $
 */

#ifndef _MATH_PRIVATE_H_
#define	_MATH_PRIVATE_H_

#include <float.h>
#include <stdint.h>
#include "gluon.h"

#define	FP_ILOGB0	(-INT_MAX)
#define	FP_ILOGBNAN	INT_MAX

 /* Definitions provided directly by GCC and Clang. */
#if !(defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))

#if defined(_WIN32)

#define __ORDER_LITTLE_ENDIAN__  1234
#define __ORDER_BIG_ENDIAN__     4321
#define __BYTE_ORDER__           __ORDER_LITTLE_ENDIAN__

#endif

#endif /* __BYTE_ORDER__, __ORDER_LITTLE_ENDIAN__ and __ORDER_BIG_ENDIAN__ */

#ifndef __FLOAT_WORD_ORDER__
#define __FLOAT_WORD_ORDER__     __BYTE_ORDER__
#endif

 /*
 * A union which permits us to convert between a double and two 32 bit
 * ints.
 */

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__

typedef union
  {
  double value;
  struct
    {
    uint32_t msw;
    uint32_t lsw;
    } parts;
  struct
    {
    uint64_t w;
    } xparts;
  } ieee_double_shape_type;

#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__

typedef union
  {
  double value;
  struct
    {
    uint32_t lsw;
    uint32_t msw;
    } parts;
  struct
    {
    uint64_t w;
    } xparts;
  } ieee_double_shape_type;

#endif

 // 32 bit float
union IEEEf2bits {
  float	f;
  struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int	man : 23;
    unsigned int	exp : 8;
    unsigned int	sign : 1;
#else /* _BIG_ENDIAN */
    unsigned int	sign : 1;
    unsigned int	exp : 8;
    unsigned int	man : 23;
#endif
    } bits;
  };

#define	DBL_MANH_SIZE	20
#define	DBL_MANL_SIZE	32

// 64 bit float
union IEEEd2bits {
  double	d;
  struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned int	manl : 32;
#endif
    unsigned int	manh : 20;
    unsigned int	exp : 11;
    unsigned int	sign : 1;
#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
    unsigned int	manl : 32;
#endif
#else /* _BIG_ENDIAN */
    unsigned int	sign : 1;
    unsigned int	exp : 11;
    unsigned int	manh : 20;
    unsigned int	manl : 32;
#endif
    } bits;
  };

/* Get two 32 bit ints from a double.  */

#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

 /* Get a 64-bit int from a double. */
#define EXTRACT_WORD64(ix,d)					\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix) = ew_u.xparts.w;						\
} while (0)

 /* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0)

 /* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)					\
do {								\
  ieee_double_shape_type gl_u;					\
  gl_u.value = (d);						\
  (i) = gl_u.parts.lsw;						\
} while (0)

 /* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

 /* Set a double from a 64-bit int. */
#define INSERT_WORD64(d,ix)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.xparts.w = (ix);						\
  (d) = iw_u.value;						\
} while (0)

 /* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

 /* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)					\
do {								\
  ieee_double_shape_type sl_u;					\
  sl_u.value = (d);						\
  sl_u.parts.lsw = (v);						\
  (d) = sl_u.value;						\
} while (0)

 /*
 * A union which permits us to convert between a float and a 32 bit
 * int.
 */

typedef union
  {
  float value;
  /* FIXME: Assumes 32 bit int.  */
  unsigned int word;
  } ieee_float_shape_type;

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0)


#define	STRICT_ASSIGN(type, lval, rval)	((lval) = (rval))

/*
 * TRUNC() is a macro that sets the trailing 27 bits in the mantissa of an
 * IEEE double variable to zero.  It must be expression-like for syntactic
 * reasons, and we implement this expression using an inline function
 * instead of a pure macro to avoid depending on the gcc feature of
 * statement-expressions.
 */
#define	TRUNC(d)	(_b_trunc(&(d)))

static __inline void _b_trunc(volatile double *_dp)
  {
  //VBS
  //uint32_t _lw;
  uint32_t _lw;

  GET_LOW_WORD(_lw, *_dp);
  SET_LOW_WORD(*_dp, _lw & 0xf8000000);
  }

struct Double {
  double	a;
  double	b;
  };


/*
* Functions internal to the math package, yet not static.
*/
double	__exp__D(double, double);
struct Double __log__D(double);

/*
 * The original fdlibm code used statements like:
 *	n0 = ((*(int*)&one)>>29)^1;		* index of high word *
 *	ix0 = *(n0+(int*)&x);			* high word of x *
 *	ix1 = *((1-n0)+(int*)&x);		* low word of x *
 * to dig two 32 bit words out of the 64 bit IEEE floating point
 * value.  That is non-ANSI, and, moreover, the gcc instruction
 * scheduler gets it wrong.  We instead use the following macros.
 * Unlike the original code, we determine the endianness at compile
 * time, not at run time; I don't see much benefit to selecting
 * endianness at run time.
 */


/*
 * Common routine to process the arguments to nan(), nanf(), and nanl().
 */
extern void __scan_nan(uint32_t *__words, int __num_words, const char *__s);


/* fdlibm kernel function */
extern int	__kernel_rem_pio2(double*,double*,int,int,int);

/* double precision kernel functions */
extern int	rem_pio2(double,double*);
extern double	__kernel_sin(double,double,int);
extern double	__kernel_cos(double,double);
extern double	__kernel_tan(double,double,int);
extern double	__ldexp_exp(double,int);

/* float precision kernel functions */
extern int	rem_pio2f(float,double*);
extern float	__kernel_sindf(double);
extern float	__kernel_cosdf(double);
extern float	__kernel_tandf(double,int);
extern float	__ldexp_expf(float,int);

#endif /* !_MATH_PRIVATE_H_ */
