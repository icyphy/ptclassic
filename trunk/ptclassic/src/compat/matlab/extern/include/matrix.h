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

#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)      args
#else
#define ARGS(args)      ()
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef char Matrix;
typedef double Real;

/* mat.type types for text and matrix */
#define TEXT    1
#define MATRIX  0

/* defines for type of matrix */
#define REAL    0
#define COMPLEX 1
 
extern Matrix*	mxCreateFull ARGS((int numrows, int numcols, int complexFlag));
extern void	mxFreeMatrix ARGS((Matrix* matrixPtr));
extern void*	mxCalloc ARGS((unsigned int numelements, unsigned int size));
extern void	mxFree ARGS((void* memoryPtr));
extern const char*	mxGetName ARGS((const Matrix* matrixPtr));
extern void	mxSetName ARGS((Matrix* matrixPtr, const char* name));
extern int	mxGetM ARGS((const Matrix* matrixPtr));
extern void	mxSetM ARGS((Matrix* matrixPtr, int numrows));
extern int	mxGetN ARGS((const Matrix* matrixPtr));
extern void	mxSetN ARGS((Matrix* matrixPtr, int numcols));
extern Real*	mxGetPr ARGS((const Matrix* matrixPtr));
extern void	mxSetPr ARGS((Matrix* matrixPtr, Real* realMatrixPtr));
extern Real*	mxGetPi ARGS((const Matrix* matrixPtr));
extern void	mxSetPi ARGS((Matrix* matrixPtr, Real* imagMatrixPtr));
extern int	mxGetNzmax ARGS((const Matrix* matrixPtr));;
extern void	mxSetNzmax ARGS((Matrix* matrixPtr, int numNonZeroElements));
extern int*	mxGetIr ARGS((const Matrix* matrixPtr));
extern void	mxSetIr ARGS((Matrix* matrixPtr, int* sparseIndexArray));
extern int*	mxGetJc ARGS((const Matrix* matrixPtr));
extern void	mxSetJc ARGS((Matrix* matrixPtr, int* sparseIndexArray));
extern int	mxGetString ARGS((const Matrix* matrixPtr, char* buffer,
				  int bufferLength));
extern Matrix*	mxCreateString ARGS((const char* string));
extern double	mxGetScalar ARGS((const Matrix* matrixPtr));
extern int	mxIsFull ARGS((const Matrix* matrixPtr));
extern int	mxIsSparse ARGS((const Matrix* matrixPtr));
extern int	mxIsDouble ARGS((const Matrix* matrixPtr));
extern void	mxSetString ARGS((Matrix* matrixPtr));
extern int	mxIsString ARGS((const Matrix* matrixPtr));
extern int	mxIsNumeric ARGS((const Matrix* matrixPtr));
extern int	mxIsComplex ARGS((const Matrix* matrixPtr));

extern Matrix*	mxCreateSparse ARGS((int numrows, int numcols,
				     int numNonZeroElements, int complexFlag));

#ifdef __cplusplus
}
#endif

#endif /* matrix_h */
