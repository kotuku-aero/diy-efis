#ifndef __gluon_h__
#define __gluon_h__

#include "fenv.h"


#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#define	MAXFLOAT	((float)3.40282346638528860e+38)

/*
* ANSI/POSIX
*/
extern int	__fpclassifyd(double);
extern int	__fpclassifyf(float);
extern int	__isfinitef(float);
extern int	__isfinite(double);
extern int	__isinff(float);
extern int	__isnanf(float);
extern int	__isnormalf(float);
extern int	__isnormal(double);
extern int	__signbit(double);
extern int	__signbitf(float);

extern double	acos(double);
extern double	asin(double);
extern double	atan(double);
extern double	atan2(double, double);
extern double	cos(double);
extern double	sin(double);
extern double	tan(double);
extern double	cosh(double);
extern double	sinh(double);
extern double	tanh(double);
extern double	exp(double);
extern double	frexp(double, int *);	/* fundamentally !__pure2 */
extern double	ldexp(double, int);
extern double	log(double);
extern double	log10(double);
extern double	modf(double, double *);	/* fundamentally !__pure2 */
extern double	pow(double, double);
extern double	sqrt(double);
extern double	ceil(double);
extern double	fabs(double);
extern double	floor(double);
extern double	fmod(double, double);
extern double	j0(double);
extern double	j1(double);
extern double	jn(int, double);
extern double	y0(double);
extern double	y1(double);
extern double	yn(int, double);
extern double	copysign(double, double);
extern double	fdim(double, double);
extern double	fmax(double, double);
extern double	fmin(double, double);
extern double	nearbyint(double);
extern double	round(double);
extern double	scalbln(double, long);
extern double	scalbn(double, int);
extern double	tgamma(double);
extern double	trunc(double);
extern float	dremf(float, float);
extern float	j0f(float);
extern float	j1f(float);
extern float	jnf(int, float);
extern float	y0f(float);
extern float	y1f(float);
extern float	ynf(int, float);
extern double	acosh(double);
extern double	asinh(double);
extern double	atanh(double);
extern double	cbrt(double);
extern double	erf(double);
extern double	erfc(double);
extern double	exp2(double);
extern double	expm1(double);
extern double	fma(double, double, double);
extern double	hypot(double, double);
extern int	ilogb(double);
extern int	(isinf)(double);
extern int	(isnan)(double);
extern double	lgamma(double);
extern long long llrint(double);
extern long long llround(double);
extern double	log1p(double);
extern double	log2(double);
extern double	logb(double);
extern long	lrint(double);
extern long	lround(double);
extern double	nan(const char *);
extern double	nextafter(double, double);
extern double	remainder(double, double);
extern double	remquo(double, double, int *);
extern double	rint(double);

/*
* Float versions of reentrant version of lgamma; passes signgam back by
* reference as the second argument; user must allocate space for signgam.
*/
extern float	lgammaf_r(float, int *);
/*
* ANSI/POSIX
*/

#ifndef GLUON_INTERNAL
extern double __nan;
extern float __nanf;
extern double __infinity;
extern float __infinityf;

#define	HUGE_VAL	(__infinity)
#define	INFINITY	(__infinity)
#define	NAN		(__nan)
#endif

/* Symbolic constants to classify floating point numbers. */
#define	FP_INFINITE	0x01
#define	FP_NAN		0x02
#define	FP_NORMAL	0x04
#define	FP_SUBNORMAL	0x08
#define	FP_ZERO		0x10
#define	fpclassify(x) ((sizeof (x) == sizeof (float)) ? __fpclassifyf(x) : __fpclassifyd(x))
#define	isfinite(x)((sizeof (x) == sizeof (float)) ? __isfinitef(x)	: __isfinite(x))
#define	isinf(x) ((sizeof (x) == sizeof (float)) ? __isinff(x)	: isinf(x))
#define	isnan(x)((sizeof (x) == sizeof (float)) ? __isnanf(x)	: isnan(x))
#define	isnormal(x) ((sizeof (x) == sizeof (float)) ? __isnormalf(x)	: __isnormal(x))
#define	signbit(x) ((sizeof (x) == sizeof (float)) ? __signbitf(x) : __signbit(x))

#ifdef __MATH_BUILTIN_RELOPS
#define	isgreater(x, y)		__builtin_isgreater((x), (y))
#define	isgreaterequal(x, y)	__builtin_isgreaterequal((x), (y))
#define	isless(x, y)		__builtin_isless((x), (y))
#define	islessequal(x, y)	__builtin_islessequal((x), (y))
#define	islessgreater(x, y)	__builtin_islessgreater((x), (y))
#define	isunordered(x, y)	__builtin_isunordered((x), (y))
#else
#define	isgreater(x, y)		(!isunordered((x), (y)) && (x) > (y))
#define	isgreaterequal(x, y)	(!isunordered((x), (y)) && (x) >= (y))
#define	isless(x, y)		(!isunordered((x), (y)) && (x) < (y))
#define	islessequal(x, y)	(!isunordered((x), (y)) && (x) <= (y))
#define	islessgreater(x, y)	(!isunordered((x), (y)) && \
					((x) > (y) || (y) > (x)))
#define	isunordered(x, y)	(isnan(x) || isnan(y))
#endif /* __MATH_BUILTIN_RELOPS */

#endif
