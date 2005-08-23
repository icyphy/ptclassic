/*******************************************************************
Version identification:
@(#)ptdspExtendedGCD.c	1.7 8/6/96

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

 Programmer: Brian L. Evans

      Greatest common divisor and least common multiple routines

********************************************************************/

#include "ptdspExtendedGCD.h"

#define intabs(m)               ( ( (m) > 0 ) ? (m) : (-(m)) )

/* Greatest common divisor function
   If the second arg is negative, result is negative.
   Magnitude of result equals gcd(abs(a),abs(b)).
*/
int
Ptdsp_GCD(int a, int b) {
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
  if (a < b) { int t; t = a; a = b; b = t;}
  if (b == 0) return a;
  while (b > 1) {
    int rem = a%b;
    if (rem == 0) break;
    a = b;
    b = rem;
  }
  return b*sign;
}

/* Least common multiple function.
   Order of the multiplication and division is chosen to avoid
   overflow in cases where a*b > MAXINT but lcm(a,b) < MAXINT.  The
   division always goes evenly.
   This routines returns 0 for the lcm of 0 and 1.
*/
int 
Ptdsp_LCM(int a, int b) { 
  return a == b ? a : a * (b / Ptdsp_GCD(a,b));
}

/* Extended greatest common divisor function.
   Solves the Bezout identity
       alpha a + beta b == 1
  over the integers given the values for a and b.

  The values at addresses alpha and beta are set to store the return
  values.
*/ 
int
Ptdsp_ExtendedGCD(int a, int b, int* alphap, int* betap) {
  int mu, lambda;
  int lambdavalue = 0, muvalue = 0;
  int gcdvalue = Ptdsp_GCD(a, b);
  int anorm = a / gcdvalue;
  int bnorm = b / gcdvalue;

  if ( intabs(a) <= intabs(b) ) {
    int mumax = intabs(anorm);
    for ( mu = 0; mu < mumax; mu++ ) {
      int lambdap = ( 1 - mu * bnorm );
      lambda = lambdap / anorm;
      if ( lambdap == (lambda * anorm) ) {
	lambdavalue = lambda;
	muvalue = mu;
	break;
      }
    }
  }
  else {
    int lambdamax = intabs(bnorm);
    for ( lambda = 0; lambda < lambdamax; lambda++ ) {
      int mup = ( 1 - lambda * anorm );
      mu = mup / bnorm;
      if ( mup == ( mu * bnorm ) ) {
	muvalue = mu;
	lambdavalue = lambda;
	break;
      }
    }
  }

  *alphap = lambdavalue;
  *betap = muvalue;
  
  return(gcdvalue);
}
