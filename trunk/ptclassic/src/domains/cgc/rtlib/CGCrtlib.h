/**************************************************************************
Version identification:
@(#)CGCrtlib.h	1.1 24/10/94

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

 This file defines the interface to the fix runtime library for the
 CGC domain.  See the implementation notes below for details.
*/

#ifndef _CGCrtlib_h
#define _CGCrtlib_h

/* 
 Definition of single (FIX_WORD) and double (FIX_DWORD) unsigned 
 word types.
  -----------------
   IMPORTANT NOTE:
  -----------------
 If the FIX_WORD and FIX_DWORD types are redefined in the CGC gen-
 erated code on compilation, make sure that the redefinition also
 applies while compiling the runtime library.
 This implementation relies on consistent definitions for both the
 generated code and the library routines.

 Further note that a variable of type FIX_DWORD must have at least
 twice the word size of a FIX_WORD veriable.
*/

#ifndef FIX_WORD
#define FIX_WORD  unsigned short
#endif
#ifndef FIX_DWORD
#define FIX_DWORD unsigned long
#endif

#define FIX_MAX_LENGTH 64 
#define FIX_BITS_PER_WORD  (8*sizeof(FIX_WORD))
#define FIX_WORDS_PER_FIX  ((FIX_MAX_LENGTH+FIX_BITS_PER_WORD-1) / FIX_BITS_PER_WORD)

typedef FIX_WORD fix[FIX_WORDS_PER_FIX];
typedef struct { int len,intb; } fix_prec;

#ifdef __cplusplus
#define EXIT_CGC(a) {if (a) Error::abortRun(*this,a); else SimControl::requestHalt(); return;}
#else
#define EXIT_CGC(a) {if (a) fprintf(stderr,"%s\n",a); exit(1);}
#endif

/* Implementation notes for the Fix Library routines:

   + Most library functions expect one or more fixed-point variables as
     parameters, passed as the triplet "LENGTH,INTBITS,REFERENCE", where
     LENGTH specifies the total word length of the fix, INTBITS is the
     number of bits reserved for the integer part and so determines the
     position of the binary point and REFERENCE is a pointer to a var-
     iable of type "fix" containing the bit pattern.
     The CGC macro expansion mechanism for $ref macros is already pre-
     pared to follow this format.

   + Source operands are saved before setting the result. By concequence.
     source and destination parameters may coincide  so that missing
     C operators like += may be easily implemented as:

	   FIX_Add($ref(result),$ref(result),$ref(term));

   + Fix triplets may be weighted by a power of two before passing them
     to library functions, e.g:

	   FIX_Add(FIX_Weight($ref(result),2),
			FIX_Weight($ref(op1),-1), $ref(op2)) 

     Here the first operand is weighted by 2^-1 = 0.5 and added to the
     second operand.  Then the sum is multiplied by 2^-2 - note the
     minus sign - and then assigned to the result.
     The FIX_Weight macro simply changes the precision passed to the
     library arguments

   + Most library functions with integer return value return 1 if the
     operations caused an overflow and 0 if not.
     This return value is internally or'ed together with the contents
     of the global variable `fix_overflow' so that the following scheme
     can be used to check sequences of contiguous fix operations against
     overflow errors:

	   (* don't forget to reset the overflow flag *)
	   fix_overflow = 0;

	   (* do the fix operations, e.g. *)
	   FIX_Add(...);
	   FIX_Mul(...);

	   (* now check whether overflow occured *)
	   if (fix_overflow)
		... do whatever you want, e.g. print error message ...

     CGC stars should be derived from the CGCFix star that provides
     basic code generation methods for overflow checking.
*/

extern int fix_overflow;

/* prototypes are for commentation purposes only */
#define FIX_PROTO(list)  ()

/* ASSIGNMENT */
extern int FIX_Assign FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int src_l, int src_i, const FIX_WORD* src_r));
extern int FIX_DoubleAssign FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r, double value));

/* CONVERSION */
extern double FIX_Fix2Double FIX_PROTO((
    int src_l, int src_i, const FIX_WORD* src_r));
extern int FIX_Fix2Int FIX_PROTO((
    int src_l, int src_i, const FIX_WORD* src_r));

/* ARITHMETIC OPERATIONS */

/* - compute `dst = op1 + op2' */
extern int FIX_Add FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r));
/* - compute `dst = op1 - op2' */
extern int FIX_Sub FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r));
/* - compute `dst = op1 * op2' */
extern int FIX_Mul FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r));
/* - compute `dst = op1 / op2' */
extern int FIX_Div FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r));

/* - compute `dst += op1 * op2' */
extern int FIX_MulAdd FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r));
/* - dito, but weight the product by 2^weight before accumulating */
extern int FIX_MulAddWeighted FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r,
    int weight));

/* - compute `dst = -src' */
int FIX_Complement FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int src_l, int src_i, const FIX_WORD* src_r));

/* - shift right by n bits (or left if n < 0) */
int FIX_ArithmeticShiftRight FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int src_l, int src_i, const FIX_WORD* src_r,
    int n));


/* COMPARISON */

/* - return a value less than, equal to, or greater than 0, based upon whether
     the value of op1 is less than, equal to or greater than that of op2 */
int FIX_Compare FIX_PROTO((
    int op1_l, int op1_i, const FIX_WORD* op1_r,
    int op2_l, int op2_i, const FIX_WORD* op2_r));


/* MISCELLANEOUS */

/* - macro to weight a fix triplet by 2^weight for function arguments
     and 2^-weight for function results */
#define FIX_Weight(var_l,var_i,var_r, weight) \
    var_l,(var_i)+(weight),var_r

/* - return the terms of the fix triplet;  note that the precision
     is trimmed according to the implementation limits */
#define FIX_GetLength(src_l,src_i,src_r) \
    MIN(FIX_MAX_LENGTH, src_l)
#define FIX_GetIntBits(src_l,src_i,src_r) \
    MIN(MIN(FIX_MAX_LENGTH,src_l), src_i)
#define FIX_GetReference(src_l,src_i,src_r) \
    src_r

/* - set the entries of a fix_prec structure from a double value;
     the intb field will be just large enough to hold the integer
     part of the double while the len field will equal the maximum
     word length;
     this macro returns 1 if the integer part would be larger than
     the total word length */
#define FIX_SetPrecisionFromDouble(precision, double) \
    pFIX_SetPrecisionFromDouble(&precision.len,&precision.intb, double)

/* - assign maximum value that is representable in given format
     [or minimum value if sign != 0] */
extern void FIX_AssignMaxValue FIX_PROTO((
    int dst_l, int dst_i, FIX_WORD* dst_r,
    int sign));

/* - return TRUE if src represents the value 0.0 */
#define FIX_IsZero(src_l,src_i,src_r) \
    pFIX_IsZero(src_r)
/* - set the fix to 0.0 */
#define FIX_SetToZero(dst_l,dst_i,dst) \
    memset(dst,0, sizeof(fix))

/* - return the sign bit of a fix */
#define FIX_Sign(ref) \
    ((*(ref) & (1l << (FIX_BITS_PER_WORD-1))) != 0)

/* private functions */
extern int  pFIX_IsZero FIX_PROTO((FIX_WORD* src_r));
extern void pFIX_ApplyMask FIX_PROTO((int dst_l,int dst_i,FIX_WORD* dst_r));
extern int  pFIX_SetPrecisionFromDouble FIX_PROTO((int* len_field,int* intb_field, double d));
extern double pFIX_TwoRaisedTo();
extern void pFIX_Complement();
extern int  pFIX_ArithmeticShiftRight();

/* MIN/MAX macro used for combining precisions in additions;
  (this is needed for the code generated by the + operator of class Precision) */
#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#endif /* _CGCrtlib_h */

