/**************************************************************************
Version identification:
@(#)CGCrtlib.c	1.1 24/10/94

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
			    COPYRIGHTENDKEY

 Programmer:  J.Weiss

 This file implements a runtime library for use with the fixed-point
 calculation macros in the CGC domain.	The code is an overworked
 version of the Fix class implementation.

 See notes in CGCrtlib.h for a description of implementation details.

**********************************************************************/

#include <stdio.h>
#include <math.h>

#include "CGCrtlib.h"

/* private macros */

/* copy the bit field of a fix from source to destination */
#define FIX_Copy(dst,src) \
    memcpy(dst,src, sizeof(fix))

/* compute # of words in internal representation for given length (in bits) */
#define FIX_Words(length) \
    ((length <= FIX_BITS_PER_WORD) ? \
	1 : pMIN(FIX_WORDS_PER_FIX, (length+FIX_BITS_PER_WORD-1)/FIX_BITS_PER_WORD))

/* private versions of the min,max,abs and dim macros */
#define pMIN(x,y) (((x) < (y)) ? (x) : (y))
#define pMAX(x,y) (((x) > (y)) ? (x) : (y))
#define pABS(x)	  (((x) < 0) ? -(x) : (x))
#define pDIM(a)	  (sizeof(a)/sizeof(*a))

#define pFIX_ArithmeticShiftRight(ref, n) \
	((n) ? _pFIX_ArithmeticShiftRight(ref,n) : 0)

/* global flag for overflow detection */
int fix_overflow = 0;

/* shift the fix right by n bits (or left if n < 0) with sign preservation.
   returns 1 if overflow occurs, but does not assign a maximum value then */
int _pFIX_ArithmeticShiftRight(ref, n)
FIX_WORD *ref;
int n;
{
    fix t;
    register FIX_WORD lsbs, msbs;
    register int i, sign = FIX_Sign(ref);

    /* this routine is the central part of all fixed point operations
       so it is attempted to make this routine as fast as possible */


    if (n > 0) {	    /* shift right */

	if (n >= FIX_BITS_PER_WORD) {

	    register int offset = (n + FIX_BITS_PER_WORD-1) / FIX_BITS_PER_WORD - 1;

	    /* set highest bits in order to preserve sign */
	    register FIX_WORD mask = sign ? ~0 : 0;

	    for (i=FIX_WORDS_PER_FIX; i--;)
		t[i] = (i < offset) ? mask : ref[i-offset];

	    n    -= offset * FIX_BITS_PER_WORD;
	    msbs  = sign ? ~((1 << (FIX_BITS_PER_WORD-n))-1) : 0;

	    for (i=offset; i<FIX_WORDS_PER_FIX; i++) {
		lsbs = t[i] >> n;
		ref[i] = lsbs + msbs;
		msbs = t[i] << (FIX_BITS_PER_WORD-n);
	    }

	} else {

	    /* acceleration for 0 offsets */
	    msbs = sign ? ~((1 << (FIX_BITS_PER_WORD-n))-1) : 0;

	    for (i=0; i<FIX_WORDS_PER_FIX; i++) {
		lsbs = msbs + (ref[i] >> n);
		msbs = ref[i] << (FIX_BITS_PER_WORD-n);
		ref[i] = lsbs;
	    }
	}
    }

    else if (n < 0) {	    /* shift left */

	if ((n = -n) >= FIX_BITS_PER_WORD) {

	    register int offset = (n + FIX_BITS_PER_WORD-1) / FIX_BITS_PER_WORD - 1;

	    /* set highest bits in order to preserve sign */
	    register FIX_WORD mask = sign ? ~0 : 0;

	    /* check for overflow */
	    for (i=offset; i--;)
		if (ref[i] != mask)  return 1;

	    for (i=FIX_WORDS_PER_FIX; i--;)
		t[i] = (i < (FIX_WORDS_PER_FIX-offset)) ? ref[i+offset] : 0;

	    n   -= offset * FIX_BITS_PER_WORD;
	    lsbs = 0;

	    for (i=FIX_WORDS_PER_FIX; i--;) {
		msbs = t[i] << n;
		ref[i] = lsbs + msbs;
		lsbs = t[i] >> (FIX_BITS_PER_WORD - n);
	    }

	} else {

	    /* acceleration for 0 offsets */
	    lsbs = 0;

	    for (i=FIX_WORDS_PER_FIX; i--;) {
		msbs = lsbs + (ref[i] << n);
		lsbs = ref[i] >> (FIX_BITS_PER_WORD - n);
		ref[i] = msbs;
	    }
	}

	/* return 1 on overflow;
	   note that no maximum value is assigned to the fix */

	return (lsbs != (sign ? ((1 << n)-1) : 0)) ||
		     (FIX_Sign(ref) != sign);
    }

    /* no overflow */
    return 0;
}

/* assign src to dst; return 1 on overflow, else 0 */
int FIX_Assign(dst_l,dst_i,dst_r, src_l,src_i,src_r)
int dst_l,dst_i, src_l,src_i;
FIX_WORD *dst_r, *src_r;
{
    FIX_Copy(dst_r,src_r);

    if (dst_i != src_i) {

	if (_pFIX_ArithmeticShiftRight(dst_r, dst_i-src_i)) {

	    FIX_AssignMaxValue(dst_l,dst_i,dst_r, FIX_Sign(src_r));
	    return fix_overflow = 1;
	}
	if (dst_l < src_l + (dst_i-src_i))
	    pFIX_ApplyMask(dst_l,dst_i,dst_r);

    } else
	if (dst_l < src_l)
	    pFIX_ApplyMask(dst_l,dst_i,dst_r);

    return 0;
}

/* assign a double value to a fix; return 1 on overflow */
int FIX_DoubleAssign(dst_l,dst_i,dst_r, value)
int dst_l,dst_i;
FIX_WORD *dst_r;
register double value;
{
    register int nwords, overflow;
    register FIX_WORD *tp;

    register double word_scale = (double) (1L << FIX_BITS_PER_WORD);
    int sign = (value < 0);

    /* extract an extra bit for rounding */
    nwords = FIX_Words(dst_l+1);

    if (sign != 0)
	 value = -value;

    /* rescale + add correcting value for rounding */
    value = pFIX_TwoRaisedTo(FIX_BITS_PER_WORD-dst_i) * value +
	    pFIX_TwoRaisedTo(FIX_BITS_PER_WORD-dst_l-1);

    /* check for overflows */
    if ((overflow = (value >= word_scale/2))) {

	/* set dst to maximum value */
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, sign);
	return fix_overflow = 1;

    } else {  /* set dst from value */

	/* mask for rounding */
	FIX_WORD mask = 1 << (FIX_BITS_PER_WORD - (dst_l % FIX_BITS_PER_WORD));

	for (tp=dst_r; nwords--; tp++) {
	    *tp = (FIX_WORD)value;
	    value = (value - *tp) * word_scale;
	}

	tp[-1] &= ~(mask-1);

	if (sign)
	    pFIX_Complement(dst_r,dst_r);

	return 0;
    }
}

/* assign fix maximum or minimum value (if sign == 1) */
void FIX_AssignMaxValue(dst_l,dst_i,dst_r, sign)
int dst_l,dst_i;
FIX_WORD *dst_r;
int sign;
{
    if (sign) {		/* assign (negative) minimum value */
	int i;
	*dst_r++ = (FIX_WORD) (1 << (FIX_BITS_PER_WORD-1));

	for (i = FIX_WORDS_PER_FIX; --i;)
	    *dst_r++ = (FIX_WORD) 0;

    } else {		/* assign (positive) maxmimum value */
	int i,j;
	*dst_r++ = (FIX_WORD)((1 << (FIX_BITS_PER_WORD-1))-1);
	i = FIX_Words(dst_l);
	j = FIX_WORDS_PER_FIX - i;

	while (--i > 0)
	    *dst_r++ = (FIX_WORD) ~0;

	dst_r[-1] &= ~(FIX_WORD)0 << dst_l%FIX_BITS_PER_WORD;

	while (j-- > 0)
	    *dst_r++ = (FIX_WORD) 0;
    }
}

/* convert a fix to a double value */
double FIX_Fix2Double(src_l,src_i,src_r)
int src_l,src_i;
register FIX_WORD *src_r;
{
    if (src_l > 2 * FIX_BITS_PER_WORD) {

	fix t;
	register double d = 0.0;
	register FIX_WORD *tp;
	register int i,sign,nwords;

	register double word_scale = (double) (1L << FIX_BITS_PER_WORD);

	if ((sign = FIX_Sign(src_r)))
	    pFIX_Complement(tp=t,src_r);
       else tp = src_r;

	nwords = FIX_Words(src_l);

	for (i=nwords; i--;)
	    d = d * word_scale + *tp++;

	d *= pFIX_TwoRaisedTo(src_i-nwords*FIX_BITS_PER_WORD);

	return sign ? -d : d;

    } else {	/* optimization for short word lengths */

	if (FIX_Sign(src_r)) 
	    return -pFIX_TwoRaisedTo(src_i-2*FIX_BITS_PER_WORD) *
	    	 (((FIX_DWORD)~src_r[0] << FIX_BITS_PER_WORD) + (FIX_WORD)~src_r[1] +1);
	else
	    return  pFIX_TwoRaisedTo(src_i-2*FIX_BITS_PER_WORD) *
	    	 (((FIX_DWORD) src_r[0] << FIX_BITS_PER_WORD) + (FIX_WORD) src_r[1]);
    }
}

/* convert a fix to an int value; no overflow detection */
int FIX_Fix2Int(src_l,src_i,src_r)
int src_l,src_i;
FIX_WORD *src_r;
{
    register int i = 0;

    /* Sign extend if negative */
    if (FIX_Sign(src_r)) {
	i = ~0;
    }

    while (src_i > FIX_BITS_PER_WORD) {
	i = (i << FIX_BITS_PER_WORD) + *src_r++;
	src_i -= FIX_BITS_PER_WORD;
    }

    return (i << src_i) + (*src_r >> (FIX_BITS_PER_WORD-src_i));
}

/* compute `dst = op1 + op2' */
int FIX_Add(dst_l,dst_i,dst_r, op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)
int dst_l,dst_i, op1_l,op1_i, op2_l,op2_i;
FIX_WORD *dst_r,*op1_r,*op2_r;
{
    register FIX_DWORD sum, carry = 0, mask;
    register int i, nwords;
    fix op1_bits, op2_bits;

    FIX_Copy(op1_bits, op1_r);
    FIX_Copy(op2_bits, op2_r);

    /* align operands */
    if (pFIX_ArithmeticShiftRight(op1_bits, dst_i-op1_i)) {
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, FIX_Sign(op1_r));
	return fix_overflow = 1;
    }
    if (pFIX_ArithmeticShiftRight(op2_bits, dst_i-op2_i)) {
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, FIX_Sign(op2_r));
	return fix_overflow = 1;
    }

#ifdef NO_OPTIMIZATION
    for (i = FIX_WORDS_PER_FIX; i--;) {
	sum   = carry + (FIX_DWORD)op1_bits[i] + (FIX_DWORD)op2_bits[i];
	carry = sum >> FIX_BITS_PER_WORD;
	dst_r[i] = (FIX_WORD)sum;
    }

    pFIX_ApplyMask(dst_l,dst_i,dst_r);
#else
    /* optimized code that tries to add and round in a single step;
       not too well tested, so may be buggy */

    if (dst_l < FIX_MAX_LENGTH) {
	nwords = FIX_Words(dst_l+1);

	for (i = FIX_WORDS_PER_FIX; i-- > nwords;) {
	    sum   = carry + (FIX_DWORD)op1_bits[i] + (FIX_DWORD)op2_bits[i];
	    carry = sum >> FIX_BITS_PER_WORD;
	    dst_r[i] = (FIX_WORD)0;
	}

	/* apply rounding */
	mask = 1 << (FIX_BITS_PER_WORD - (dst_l % FIX_BITS_PER_WORD));

	sum   = carry + (FIX_DWORD)op1_bits[i] + (FIX_DWORD)op2_bits[i] + (mask >> 1);
	carry = sum >> FIX_BITS_PER_WORD;

	/* clear LSBs of last occupied word */
	dst_r[i] = (FIX_WORD)(sum & ~(mask-1));

    } else {
	/* no rounding necessary because the result occupies the whole bit pattern */
	i = FIX_BITS_PER_WORD;
    }

    while (i--) {
	sum   = carry + (FIX_DWORD)op1_bits[i] + (FIX_DWORD)op2_bits[i];
	carry = sum >> FIX_BITS_PER_WORD;
	dst_r[i] = (FIX_WORD)sum;
    }
#endif

    /* return 1 if overflow occurred
       check: signs of args same and sign of result different */

    if (FIX_Sign(op1_r) == FIX_Sign(op2_r) &&
	FIX_Sign(dst_r) != FIX_Sign(op1_r)) {

	FIX_AssignMaxValue(dst_l,dst_i,dst_r,FIX_Sign(op1_r));
	return fix_overflow = 1;
    }

    return 0;
}

/* compute `dst = op1 - op2' */
int FIX_Sub(dst_l,dst_i,dst_r, op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)
int dst_l,dst_i, op1_l,op1_i, op2_l,op2_i;
FIX_WORD *dst_r,*op1_r,*op2_r;
{
    register FIX_DWORD sum, carry = 0, mask;
    register int i, nwords;
    fix op1_bits, op2_bits;

    FIX_Copy(op1_bits, op1_r);
    FIX_Copy(op2_bits, op2_r);

    /* align operands */
    if (pFIX_ArithmeticShiftRight(op1_bits, dst_i-op1_i)) {
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, FIX_Sign(op1_r));
	return fix_overflow = 1;
    }
    if (pFIX_ArithmeticShiftRight(op2_bits, dst_i-op2_i)) {
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, FIX_Sign(op2_r));
	return fix_overflow = 1;
    }

#ifdef NO_OPTIMIZATION
    for (i = FIX_WORDS_PER_FIX; i--;) {
	sum   = carry + (FIX_DWORD)op1_bits[i] - (FIX_DWORD)op2_bits[i];
	carry = sum >> FIX_BITS_PER_WORD;
	dst_r[i] = (FIX_WORD)sum;
    }

    pFIX_ApplyMask(dst_l,dst_i,dst_r);
#else
    /* optimized code that tries to substract and round in a single step;
       not too well tested, so may be buggy */

    if (dst_l < FIX_MAX_LENGTH) {
	nwords = FIX_Words(dst_l+1);

	for (i = FIX_WORDS_PER_FIX; i-- > nwords;) {
	    sum   = carry + (FIX_DWORD)op1_bits[i] - (FIX_DWORD)op2_bits[i];
	    carry = sum >> FIX_BITS_PER_WORD;
	    dst_r[i] = (FIX_WORD)0;
	}

	/* apply rounding */
	mask = 1 << (FIX_BITS_PER_WORD - (dst_l % FIX_BITS_PER_WORD));

	sum   = carry + (FIX_DWORD)op1_bits[i] - (FIX_DWORD)op2_bits[i] + (mask >> 1);
	carry = sum >> FIX_BITS_PER_WORD;

	/* clear LSBs of last occupied word */
	dst_r[i] = (FIX_WORD)(sum & ~(mask-1));

    } else {
	/* no rounding necessary because the result occupies the whole bit pattern */
	i = FIX_BITS_PER_WORD;
    }

    while (i--) {
	sum   = carry + (FIX_DWORD)op1_bits[i] - (FIX_DWORD)op2_bits[i];
	carry = sum >> FIX_BITS_PER_WORD;
	dst_r[i] = (FIX_WORD)sum;
    }
#endif

    /* return 1 if overflow occurred
       check: signs of args different and sign of result different from op1 */

    if (FIX_Sign(op1_r) != FIX_Sign(op2_r) &&
	FIX_Sign(dst_r) != FIX_Sign(op1_r)) {

	FIX_AssignMaxValue(dst_l,dst_i,dst_r,FIX_Sign(op1_r));
	return fix_overflow = 1;
    }

    return 0;
}

/* compute `dst = op1 * op2';  */
int FIX_Mul(dst_l,dst_i,dst_r, op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)
int dst_l,dst_i, op1_l,op1_i, op2_l,op2_i;
FIX_WORD *dst_r,*op1_r,*op2_r;
{
    fix op1_bits, op2_bits;
    register int op1_words,op2_words;
    register FIX_DWORD mask;
    register int i,j,k, post_scale;
    int sign1,sign2;

    if (pFIX_IsZero(op1_r) || pFIX_IsZero(op2_r))
	return FIX_SetToZero(dst_l,dst_i,dst_r), 0;

    if ((sign1 = FIX_Sign(op1_r)))
	pFIX_Complement(op1_bits,op1_r);
   else FIX_Copy(op1_bits, op1_r);

    if ((sign2 = FIX_Sign(op2_r)))
	pFIX_Complement(op2_bits,op2_r);
   else FIX_Copy(op2_bits, op2_r);

    /* compute difference between actual and required intbits */
    post_scale = dst_i - (op1_i+op2_i-1);

    /* we do the following optional scale down to guarantee that the
       multiplication cannot overflow.	If it is needed, the scale_up
       at the end may cause an overflow though. */

    if (post_scale < 0) {
	/* here we effectively scale down X and Y.
	   we will scale up later. */
	if (op1_i > -post_scale) 
	    op1_i += post_scale;
	else
	    op1_i = 1,
	    op2_i = dst_i;
    }

    op1_words = FIX_Words(op1_l);
    op2_words = FIX_Words(op2_l);

    /* set result to 0.0 */
    FIX_SetToZero(dst_l,dst_i,dst_r);

    mask = ((FIX_DWORD)1 << FIX_BITS_PER_WORD)-1;

    for (i=op1_words; i--;) {

	FIX_DWORD carry = 0;

	for (j=op2_words; j--;) {
	    k = i + j + 1;

	    if (k < FIX_WORDS_PER_FIX+1) {
		FIX_DWORD a = (FIX_DWORD)op1_bits[i] * (FIX_DWORD)op2_bits[j];
		FIX_DWORD b = ((a << 1) & mask) + carry;

		if (k < FIX_WORDS_PER_FIX)
		    dst_r[k] = (FIX_WORD)(b += dst_r[k]);
		carry = (a >> (FIX_BITS_PER_WORD-1)) + (b >> FIX_BITS_PER_WORD);
	    }
	}

	dst_r[i] = (FIX_WORD)carry;
    }

    /* if post_scale is set, we must now shift up the product by that
       many bits.  it may not fit, so the shift routine may return
       an overflow error. */

    if (post_scale != 0) {

	if (_pFIX_ArithmeticShiftRight(dst_r, post_scale)) {
	    FIX_AssignMaxValue(dst_l,dst_i,dst_r, sign1 ^ sign2);
	    return fix_overflow = 1;
	}
    }

    if (sign1 != sign2) 
	pFIX_Complement(dst_r,dst_r);

    pFIX_ApplyMask(dst_l,dst_i,dst_r);
    return 0;
}

/* compute `dst = op1 / op2'; 
   this operation cheats and does a floating point division */
int FIX_Div(dst_l,dst_i,dst_r, op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)
int dst_l,dst_i, op1_l,op1_i, op2_l,op2_i;
FIX_WORD *dst_r,*op1_r,*op2_r;
{
    double op1,op2;
    op2 = FIX_Fix2Double(op2_l,op2_i,op2_r);

    if (op2 == 0) {
	/* division by zero; assign maximum value and return 1 */
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, FIX_Sign(op1_r));
	return fix_overflow = 1;
    }

    op1 = FIX_Fix2Double(op1_l,op1_i,op1_r);
    return FIX_DoubleAssign(dst_l,dst_i,dst_r, op1/op2);
}

/* multiply and accumulate; compute `dst += op1 * op2'
   uses a temporary variable to hold the product.   */
int FIX_MulAdd(dst_l,dst_i,dst_r, op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)
int dst_l,dst_i, op1_l,op1_i, op2_l,op2_i;
FIX_WORD *dst_r,*op1_r,*op2_r;
{
    fix product;

    int prod_l = op1_l+op2_l-1;
    int prod_i = op1_i+op2_i-1;

    /* note that it is not necessary to trim the precision of the
       product since it already is in the mul/add routines */

    if (FIX_Mul(prod_l,prod_i, product,
		op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)) {

	/* overflow: copy maximum value from product into destination */
	return FIX_Copy(dst_r,op2_r), fix_overflow = 1;
    }

    return FIX_Add(dst_l,dst_i,dst_r,
		   dst_l,dst_i,dst_r, prod_l,prod_i,product);
}

/* dito., but weight the product before accumulating.
   the weight parameter is a positive or negative integer specifying a
   basis-2 potence for the weight, i.e. the number of bits to shift left */
int FIX_MulAddWeighted(dst_l,dst_i,dst_r, op1_l,op1_i,op1_r, op2_l,op2_i,op2_r, weight)
int dst_l,dst_i, op1_l,op1_i, op2_l,op2_i;
FIX_WORD *dst_r,*op1_r,*op2_r;
int weight;
{
    fix product;
    register int prod_l, prod_i;
    register int t, wabs = pABS(weight);

    if (weight < 0) {

	/* shift right the operands before multiplying in order to
	   avoid overflow errors */

	if (t=wabs, (wabs-=op1_i) <= 0)
	    op1_i -= t;
   else if (t=wabs, (wabs-=op2_i) <= 0)
	    op2_i -= t, op1_i = 0;
	else {
	    op1_i = op2_i = 0;
	    pFIX_ArithmeticShiftRight(op1_r, wabs/2);
	    pFIX_ArithmeticShiftRight(op2_r, wabs-wabs/2);
	}
    }

    prod_l = op1_l+op2_l-1;
    prod_i = op1_i+op2_i-1;

    if (FIX_Mul(prod_l,prod_i, product,
		op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)) {
	/* overflow: copy maximum value from product into destination */
	return FIX_Copy(dst_r,product), fix_overflow = 1;
    }

    if (weight > 0) {

	/* shift left the product before accumulating */
	
	if (t=wabs, (wabs-=prod_l-prod_i) <= 0)
	    prod_i += t;
	else {
	    int sign = FIX_Sign(product);
	    prod_i = prod_l;

	    if (pFIX_ArithmeticShiftRight(product, -wabs)) {
		/* overflow: set destination to minimum or maximum value */
		FIX_AssignMaxValue(dst_l,dst_i,dst_r, sign);
		return fix_overflow = 1;
	    }
	}
    }

    return FIX_Add(dst_l,dst_i,dst_r,
		   dst_l,dst_i,dst_r, prod_l,prod_i,product);
}

/* compute `dst = -src' */
int FIX_Complement(dst_l,dst_i,dst_r, src_l,src_i,src_r)
int dst_l,dst_i, src_l,src_i;
FIX_WORD *dst_r,*src_r;
{
    pFIX_Complement(dst_r,src_r);
    if (src_i != dst_i || src_l != dst_l)
	return FIX_Assign(dst_l,dst_i,dst_r, src_l,src_i,dst_r);
    return 0;
}

/* return a value less than, equal to, or greater than 0, based upon whether
   the value of op1 is less than, equal to or greater than that of op2 */
int FIX_Compare(op1_l,op1_i,op1_r, op2_l,op2_i,op2_r)
int op1_l,op1_i, op2_l,op2_i;
FIX_WORD *op1_r,*op2_r;
{
    fix t_bits;
    register FIX_WORD *tp1,*tp2;
    register int i, op_len;

    int sign_diff = FIX_Sign(op2_r) - FIX_Sign(op1_r);
    if (sign_diff != 0)
	return sign_diff;

    if (op1_l == op2_l && op1_i == op2_i)
	tp1 = op1_r,  tp2 = op2_r,
	op_len = op1_l;

    else if (op1_i > op2_i)
	FIX_Assign(op_len=op1_l,op1_i,t_bits, op2_l,op2_i,op2_r),
	tp1 = op1_r,  tp2 = t_bits;
    else
	FIX_Assign(op_len=op2_l,op2_i,t_bits, op1_l,op1_i,op1_r),
	tp1 = t_bits, tp2 = op2_r;
	
    i = FIX_Words(op_len);
    while (i--) {
	int diff = *tp1++ - *tp2++;
	if (diff)
	    return diff;
    }
	
    return 0;
}

/* shift the fix right by n bits (or left if n < 0) with sign preservation.
   returns 1 if overflow occurs */


int FIX_ArithmeticShiftRight(dst_l,dst_i,dst_r, src_l,src_i,src_r, n)
int dst_l,dst_i, src_l,src_i,n;
FIX_WORD *dst_r, *src_r;
{
    int sign = FIX_Sign(src_r);

    FIX_Copy(dst_r,src_r);

    if (pFIX_ArithmeticShiftRight(dst_r, n)) {
	FIX_AssignMaxValue(dst_l,dst_i,dst_r, sign);
	return fix_overflow = 1;
    }
    pFIX_ApplyMask(dst_l,dst_i,dst_r);
    return 0;
}

/* store the complement of the bit pattern referenced through src_r
   in dst_r;  len is the total word length of the pattern */
void pFIX_Complement(dst_r,src_r)
register FIX_WORD *dst_r,*src_r;
{
    register int i;
    register FIX_DWORD carry = 1;

    src_r += FIX_WORDS_PER_FIX;
    dst_r += FIX_WORDS_PER_FIX;

    for (i=FIX_WORDS_PER_FIX; i--; ) {
	/* cast to prevent sign extension */
	FIX_DWORD t = (FIX_WORD)(~*--src_r) + carry;
	*--dst_r = (FIX_WORD)t;
	carry  = t >> FIX_BITS_PER_WORD;
    }
}

/* restrict result to the correct number of bits applying rounding */
void pFIX_ApplyMask(var_l,var_i,var_r)
int var_l,var_i;
FIX_WORD *var_r;
{
    /* nwords must include an extra bit for rounding */
    register int m, nwords = FIX_Words(var_l+1);

    /* return immediately if there's nothing to adjust */ 
    if (var_l > FIX_MAX_LENGTH-1)
	    return;

    /* clear unoccupied words */
    memset((char*)&var_r[nwords], 0, 
	   (FIX_WORDS_PER_FIX-nwords) * sizeof(*var_r));

    m = var_l % FIX_BITS_PER_WORD;
    var_r += nwords-1;

    /* apply rounding */
    {
	register FIX_DWORD mask = 1 << (FIX_BITS_PER_WORD-m);
	register FIX_DWORD sum  = *var_r + (mask >> 1);
	register FIX_WORD  carry;

	/* clear LSBs of last occupied word */
	*var_r = (FIX_WORD)(sum & ~(mask-1));

	while (--nwords) {
	    if ((carry = (FIX_WORD)(sum >> FIX_BITS_PER_WORD))) {
		sum = carry + (FIX_DWORD)*--var_r;
		*var_r = (FIX_WORD)sum;
	    } else
		break;
	}
    }
}

/* return 1 if a fix variable represents value 0.0 */
int pFIX_IsZero(var_r)
register FIX_WORD *var_r;
{
    register int i = FIX_WORDS_PER_FIX;
    while (i--)
	if (*var_r++)  return 0;
    return 1;
}

/* return 2.0 ^ n using a lookup table for -FIX_MAXLENGTH <= n <= FIX_MAX_LENGTH */
double pFIX_TwoRaisedTo(n)
register int n;
{
    static double twoRaisedTo[FIX_MAX_LENGTH+1];
    static initialized = 0;

    if (!initialized) {
	double d = 1.0;	 int i;

	for (i=0; i<pDIM(twoRaisedTo); i++)
	    twoRaisedTo[i] = d, d *= 2.0;
	initialized = 1;
    }

    if (n >= 0)
	return (n >= pDIM(twoRaisedTo)) ? pow(2.0,(double)n) : twoRaisedTo[n];
   else return (n <= pDIM(twoRaisedTo)) ? pow(2.0,(double)n) : 1.0/twoRaisedTo[-n];
}

int pFIX_SetPrecisionFromDouble(len_field,intb_field, d)
register int *len_field, *intb_field;
register double d;
{
    register int i = (int) d;

    /* set total word length to maximum
       NOTE: this differs from the FIX class implementation where
       	     the length is set to a default value, but I think this
       	     is better in order to avoid losing accuracy. */
    *len_field  = FIX_MAX_LENGTH;

    /* reserve one bit for the sign */
    *intb_field = 1;

    if (i != 0) {
	do
	    (*intb_field)++;
	while ((i = (int)(i/2)) != 0);
    }

    if (*intb_field > *len_field)
	return fix_overflow = 1;
    return 0;
}
