
/* @(#)e_lgamma.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* lgamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call lgamma_r
 */



#include "gluon_private.h"

 double lgamma(double x)
{
	int signgam;

	return lgamma_r(x, &signgam);
}
