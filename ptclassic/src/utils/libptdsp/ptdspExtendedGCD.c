/**CFile***********************************************************************

  FileName    [ ptdspExtendedGCD.c ]

  PackageName [ ptdsp ]

  Synopsis    [ Function definition for Ptdsp_ExtendedGCD ]

  Author      [ Brian Evans ]

  Copyright   [ 

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

					PT_COPYRIGHT_VERSION_2
					COPYRIGHTENDKEY
]

   Version [ $Id$ ]

******************************************************************************/

#include "ptdspExtendedGCD.h"

#define intabs(m)               ( ( (m) > 0 ) ? (m) : (-(m)) )

// Greatest common divisor function.  If 2nd arg is negative, result is
// negative.  Magnitude of result equals gcd(abs(a),abs(b)).
// these are so simplify is easy to write.
int gcd(int a, int b) {
  
  int rem, t;
  int sign = 1;
  /* record signs */
  if (a < 0) {
    a = -a;
  }
  if (b < 0) {
    sign = -1;
    b = -b;
  }
  /* swap to make a > b if needed */
  if (a < b) { t = a; a = b; b = t;}
  if (b == 0) return a;
  while (1) {
    if (b <= 1) return b*sign;
    rem = a%b;
    if (rem == 0) return b*sign;
    a = b;
    b = rem;
  }
}

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function*******************************************************************
  Synopsis    [ ]
  SideEffects []
******************************************************************************/

int Ptdsp_ExtendedGCD( int a, int b, int *alpha, int *beta) {

  int mumax, mu, lambdap, lambda, lambdamax, mup;

  int lambdavalue = 0, muvalue = 0;
  int gcdvalue = gcd(a, b);
  int anorm = a / gcdvalue;
  int bnorm = b / gcdvalue;

  if ( intabs(a) <= intabs(b) ) {
    mumax = intabs(anorm);
    for ( mu = 0; mu < mumax; mu++ ) {
      lambdap = ( 1 - mu * bnorm );
      lambda = lambdap / anorm;
      if ( lambdap == (lambda * anorm) ) {
	lambdavalue = lambda;
	muvalue = mu;
	break;
      }
    }
  }
  else {
    lambdamax = intabs(bnorm);
    for ( lambda = 0; lambda < lambdamax; lambda++ ) {
      mup = ( 1 - lambda * anorm );
      mu = mup / bnorm;
      if ( mup == ( mu * bnorm ) ) {
	muvalue = mu;
	lambdavalue = lambda;
	break;
      }
    }
  }
  
  *alpha = lambdavalue;
  *beta = muvalue;
  
  return(gcdvalue);
}
