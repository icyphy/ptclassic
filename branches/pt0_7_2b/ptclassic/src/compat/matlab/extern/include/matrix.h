/*
 * $Id$
 *
 * Provide a dummy header that mimics "matrix.h" that comes with Matlab.
 *
 */

#ifndef matrix_h
#define matrix_h

typedef double Real;
typedef char Matrix;

#define TEXT    1                       /* mat.type indicating text */
#define MATRIX  0                       /* mat.type indicating matrix */
#define REAL    0
#define COMPLEX 1

#define mxCreateFull(m,n,f)	0
#define mxFreeMatrix(p)
#define mxCalloc(n,size)
#define mxFree(p)
#define mxGetName(p)		0
#define mxSetName(p,s)
#define	mxGetM(pm)		0
#define	mxSetM(pm,m)
#define	mxGetN(pm)		0
#define	mxSetN(pm,n)
#define	mxGetPr(pm)		0
#define	mxSetPr(pm,p)
#define mxGetPi(pm)		0
#define	mxGetPr(pm,p)
#define	mxGetNzmax(pm)		0
#define	mxSetNzmax(pm,n)
#define mxGetIr(pm)		0
#define mxSetIr(pm,ir)
#define	mxGetJc(pm)		0
#define	mxSetJc(pm,jc)
#define	mxGetString(pm,sp,sl)	0
#define	mxCreateString(s)	0
#define	mxGetScalar(pm)		0.0
#define	mxIsFull(pm)		0
#define	mxIsSparse(pm)		0
#define	mxIsDouble(pm)		0
#define	mxIsString(pm)		0
#define	mxIsNumeric(pm)		0
#define	mxIsComplex(pm)		0
#define	mxCreateSparse(m,n,z,c)	0

#endif /* matrix_h */
