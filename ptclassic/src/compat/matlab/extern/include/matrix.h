/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
*/
/*
 * $Id$
 *
 * Provide a dummy header that mimics "matrix.h" that comes with Matlab.
 *
 */

#ifndef matrix_h
#define matrix_h

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
#define	mxSetPi(pm,p)
#define	mxGetNzmax(pm)		0
#define	mxSetNzmax(pm,n)
#define mxGetIr(pm)		0
#define mxSetIr(pm,ir)
#define	mxGetJc(pm)		0
#define	mxSetJc(pm,jc)
#define	mxGetString(pm,sp,sl)	1
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
