/*
Copyright (c) 1990-1997 The Regents of the University of California.
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
 * @(#)matrix.h	1.8	07/08/97
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

typedef char mxArray;
typedef double Real;

#if !defined(__cplusplus) || defined(NO_BUILT_IN_SUPPORT_FOR_BOOL)
#ifdef _MSC_VER		  /* defined by Microsoft's C/C++ compiler */
/* suppress warnings from VC++ about bool being a reserved keyword */
#pragma warning(disable : 4237)
#endif /* _MSC_VER */
typedef int bool;
#if !defined(false)
#define false (0)
#endif
#if !defined(true)
#define true (1)
#endif
#endif /* !defined(__cplusplus) || defined(NO_BUILT_IN_SUPPORT_FOR_BOOL) */


#ifdef MATLAB_MEX_FILE
#  ifndef NDEBUG
     extern void mexPrintAssertion(
                                   const char *test, 
                                   const char *fname, 
                                   int linenum, 
                                   const char *message);
#    define mxAssert(test, message) ( (test) ? (void) 0 : mexPrintAssertion(": " #test ",", __FILE__, __LINE__, message))
#    define mxAssertS(test, message) ( (test) ? (void) 0 : mexPrintAssertion("", __FILE__, __LINE__, message))
#  else
#    define mxAssert(test, message) ((void) 0)
#    define mxAssertS(test, message) ((void) 0)
#  endif
#else
#  include <assert.h>
#  define mxAssert(test, message) assert(test)
#  define mxAssertS(test, message) assert(test)
#endif

/* maximum name length */
#define mxMAXNAM  32

typedef short mxChar;

typedef enum {
	mxCELL_CLASS = 1,
	mxSTRUCT_CLASS,
	mxOBJECT_CLASS,
	mxCHAR_CLASS,
	mxSPARSE_CLASS,
	mxDOUBLE_CLASS,
	mxSINGLE_CLASS,
	mxINT8_CLASS,
	mxUINT8_CLASS,
	mxINT16_CLASS,
	mxUINT16_CLASS,
	mxINT32_CLASS,
	mxUINT32_CLASS,
	mxINT64_CLASS,		/* place holder - future enhancements */
	mxUINT64_CLASS,		/* place holder - future enhancements */
	mxUNKNOWN_CLASS = -1
} mxClassID;

typedef enum {
    mxREAL,
    mxCOMPLEX
} mxComplexity;

extern mxClassID mxGetClassID ARGS((const mxArray* pa));
extern const char *mxGetName ARGS((const mxArray* pa));
extern void mxSetName ARGS((mxArray* pa, const char *s));
extern void *mxGetData ARGS((const mxArray* pa));
extern void mxSetData ARGS((mxArray *pa, void *pd));
extern double *mxGetPr ARGS((const mxArray *pa));
extern void mxSetPr ARGS((mxArray *pa, double* pr));
extern void *mxGetImagData ARGS((const mxArray* pa));
extern void mxSetImagData ARGS((mxArray* pa, void* pi));
extern double *mxGetPi ARGS((const mxArray* pa));
extern void mxSetPi ARGS((mxArray* pa, double* pi));
extern bool mxIsNumeric ARGS((const mxArray *pa));
extern bool mxIsCell ARGS((const mxArray *pa));
extern bool mxIsChar ARGS((const mxArray *pa));
extern bool mxIsSparse ARGS((const mxArray *pa));
extern bool mxIsStruct ARGS((const mxArray *pa));
extern bool mxIsComplex ARGS((const mxArray *pa));
extern bool mxIsDouble ARGS((const mxArray *pa));
extern bool mxIsSingle ARGS((const mxArray *pa));
extern bool mxIsLogical ARGS((const mxArray *pa));
extern bool mxIsInt8 ARGS((const mxArray *pa));
extern bool mxIsUint8 ARGS((const mxArray *pa));
extern bool mxIsInt16 ARGS((const mxArray *pa));
extern bool mxIsUint16 ARGS((const mxArray *pa));
extern bool mxIsInt32 ARGS((const mxArray *pa));
extern bool mxIsUint32 ARGS((const mxArray *pa));


#ifdef __WATCOMC__
#ifndef __cplusplus
#pragma aux mxGetScalar value [8087];
#endif
#endif


extern double mxGetScalar ARGS((const mxArray *pa));
extern void mxSetLogical ARGS((mxArray *pa));
extern void mxClearLogical ARGS((mxArray *pa));
extern bool mxIsFromGlobalWS ARGS((const mxArray *pa));
extern void mxSetFromGlobalWS ARGS((mxArray *pa, bool global));
extern int mxGetNumberOfDimensions ARGS((const mxArray *pa));
extern int mxGetM ARGS((const mxArray *pa));
extern void mxSetM ARGS((mxArray *pa, int m));
extern int mxGetN ARGS((const mxArray* pa));
extern const int *mxGetDimensions ARGS((const mxArray *pa));
extern bool mxIsEmpty ARGS((const mxArray *pa));
extern int *mxGetIr ARGS((const mxArray *pa));
extern void mxSetIr ARGS((mxArray *pa, int *ir));
extern int *mxGetJc ARGS((const mxArray *pa));
extern void mxSetJc ARGS((mxArray *pa, int *jc));
extern int mxGetNzmax ARGS((const mxArray *pa));
extern void mxSetNzmax ARGS((mxArray *pa, int nzmax));
extern int mxGetNumberOfElements ARGS((const mxArray *pa));
extern int mxGetElementSize ARGS((const mxArray *pa));
extern int mxCalcSingleSubscript ARGS((const mxArray *pa,
					int nsubs, const int *subs));
extern mxArray *mxGetCell ARGS((const mxArray *pa, int i));
extern void mxSetCell ARGS((mxArray *pa, int i, mxArray *value));
extern int mxGetNumberOfFields ARGS((const mxArray *pa));
extern mxArray *mxGetField ARGS((const mxArray *pa, int i,
				 const char *fieldname));
extern void mxSetField ARGS((mxArray *pa, int i,
				const char *fieldname, mxArray *value));
extern int mxGetFieldNumber ARGS((const mxArray *pa, const char *name));
extern mxArray *mxGetFieldByNumber ARGS((const mxArray *pa, int i,
					int fieldnum));
extern void mxSetFieldByNumber ARGS((mxArray *pa, int i, int fieldnum,
					mxArray *value));
extern const char *mxGetFieldNameByNumbe ARGS((const mxArray *pa, int n));
extern const char *mxGetClassName ARGS((const mxArray *pa));
extern bool mxIsClass ARGS((const mxArray *pa, const char *name));
extern int mxGetString ARGS((const mxArray *pa, char *buf, int buflen));



#if defined(ARRAY_ACCESS_INLINING)
#define mxGetName(pa)           ((const char *)((pa)->name))
#define mxGetPr(pa)             ((double *)((pa)->data.number_array.pdata))
#define mxSetPr(pa,pv)          ((pa)->data.number_array.pdata = (pv))
#define mxGetPi(pa)             ((double *)((pa)->data.number_array.pimag_data))
#define mxSetPi(pa,pv)          ((pa)->data.number_array.pimag_data = (pv))
#define mxIsCell(pa)            ((pa)->type == mxCELL_ARRAY)
#define mxIsChar(pa)            ((pa)->type == mxCHARACTER_ARRAY)
#define mxIsSparse(pa)          ((pa)->type == mxSPARSE_ARRAY)
#define mxIsStruct(pa)          ((pa)->type == mxSTRUCTURE_ARRAY || (pa)->type == mxOBJECT_ARRAY)
#define mxIsComplex(pa)         (!mxIsCell(pa) && !mxIsStruct(pa) && mxGetPi(pa) != NULL)
#define mxIsDouble(pa)          ((pa)->type == mxDOUBLE_ARRAY)
#define mxIsSingle(pa)          ((pa)->type == mxFLOAT_ARRAY)
#define mxIsInt8(pa)            ((pa)->type == mxINT8_ARRAY)
#define mxIsUint8(pa)           ((pa)->type == mxUINT8_ARRAY) 
#define mxIsInt16(pa)           ((pa)->type == mxINT16_ARRAY)  
#define mxIsInt32(pa)           ((pa)->type == mxINT32_ARRAY)
#define mxIsUint32(pa)          ((pa)->type == mxUINT32_ARRAY)
#define mxSetLogical(pa)        ((pa)->flags.logical_flag = true)
#define mxClearLogical(pa)      ((pa)->flags.logical_flag = false)
#define mxGetNumberOfDimensions(pa)          ((pa)->number_of_dims)
#endif  /* defined(ARRAY_ACCESS_INLINING) */


#include <stdlib.h>

typedef void * (*calloc_proc)(size_t nmemb, size_t size);
typedef void (*free_proc)(void *ptr);
typedef void * (*malloc_proc)(size_t size);
typedef void * (*realloc_proc)(void *ptr, size_t size);

#if !defined(MATLAB_MEX_FILE)

extern void mxSetAllocFcns(
	calloc_proc		callocfcn,
	free_proc		freefcn,
	realloc_proc	reallocfcn,
	malloc_proc		mallocfcn
	);


#endif /* !defined(MATLAB_MEX_FILE) */


extern void *mxCalloc ARGS((size_t n, size_t size));
extern void mxFree ARGS((void *ptr));
extern void *mxRealloc ARGS((void *ptr, size_t size));
extern void mxSetN ARGS((mxArray *pa, int n));
extern int mxSetDimensions ARGS((mxArray *pa, const int *size, int ndims));
extern void mxDestroyArray ARGS((mxArray *pa));
extern mxArray *mxCreateNumericArray ARGS((int ndim, const int *dims,
			mxClassID classid, mxComplexity flag));
extern mxArray *mxCreateDoubleMatrix
			ARGS((int m, int n, mxComplexity flag));
extern mxArray *mxCreateSparse
			ARGS((int m, int n, int nzmax, mxComplexity flag));
extern mxArray *mxCreateString
			ARGS((const char *str));
extern mxArray *mxCreateCharArray
			ARGS((int ndim, const int *dims));
extern mxArray *mxCreateCharMatrixFromStrings ARGS((int m, const char **str));
extern mxArray *mxCreateCellMatrix ARGS((int m, int n));
extern mxArray *mxCreateCellArray ARGS((int ndim, const int *dims));
extern mxArray *mxCreateStructMatrix
		ARGS((int m, int n, int nfields, const char **fieldnames));
extern mxArray *mxCreateStructArray
		ARGS((int ndim, const int *dims, int nfields,
			const char **fieldnames));
extern mxArray *mxDuplicateArray ARGS((const mxArray *in));
extern int mxSetClassName ARGS((mxArray *pa, const char *classname));


#ifdef __WATCOMC__
#ifndef __cplusplus
#pragma aux mxGetEps value [8087];
#pragma aux mxGetInf value [8087];
#pragma aux mxGetNaN value [8087];
#endif
#endif

extern double mxGetEps ARGS((void));
extern double mxGetInf ARGS((void));
extern double mxGetNaN ARGS((void));
extern bool mxIsFinite ARGS((double x));
extern bool mxIsInf ARGS((double x));
extern bool mxIsNaN ARGS((double x));


#if defined(V4_COMPAT)
#define Matrix  mxArray
#define COMPLEX mxCOMPLEX
#define REAL    mxREAL
#endif /* V4_COMPAT */


#if defined(V4_COMPAT)

extern int mxIsFull ARGS((const Matrix *pm));

#define mxCreateFull mxCreateDoubleMatrix
#define mxIsString   mxIsChar
#define mxFreeMatrix mxDestroyArray
#endif /* V4_COMPAT */


/* $Revision$ */
#ifdef ARGCHECK

#include "mwdebug.h" /* Prototype _d versions of API functions */

#define mxCalcSingleSubscript(pa, nsubs, subs) mxCalcSingleSubscript_d(pa, nsubs, subs, __FILE__, __LINE__) 
#define mxCalloc(nelems, size) mxCalloc_d(nelems, size, __FILE__, __LINE__) 
#define mxClearLogical(pa)				mxClearLogical_d(pa, __FILE__, __LINE__)
#define mxCreateCellArray(ndim, dims)	mxCreateCellArray_d(ndim, dims, __FILE__, __LINE__)
#define mxCreateCellMatrix(m, n)		mxCreateCellMatrix_d(m, n, __FILE__, __LINE__)
#define mxCreateCharArray(ndim, dims) mxCreateCharArray_d(ndim, dims, __FILE__, __LINE__)
#define mxCreateCharMatrixFromStrings(m, strings) mxCreateCharMatrixFromStrings_d(m, strings, __FILE__, __LINE__)
#define mxCreateDoubleMatrix(m, n, cplxflag)				mxCreateDoubleMatrix_d(m, n, cplxflag, __FILE__, __LINE__)
#define mxCreateNumericArray(ndim, dims, classname, cplxflag) mxCreateNumericArray_d(ndim, dims, classname, cplxflag, __FILE__, __LINE__) 
#define mxCreateSparse(m, n, nzmax, cplxflag) mxCreateSparse_d(m, n, nzmax, cplxflag, __FILE__, __LINE__) 
#define mxCreateString(string) mxCreateString_d(string, __FILE__, __LINE__)
#define mxCreateStructArray(ndim, dims, nfields, fieldnames) mxCreateStructArray_d(ndim, dims, nfields, fieldnames, __FILE__, __LINE__)
#define mxCreateStructMatrix(m, n, nfields, fieldnames) mxCreateStructMatrix_d(m, n, nfields, fieldnames, __FILE__, __LINE__)
#define mxDestroyArray(pa)                    mxDestroyArray_d(pa, __FILE__, __LINE__)
#define mxDuplicateArray(pa)                    mxDuplicateArray_d(pa, __FILE__, __LINE__)
#define mxFree(pm)			mxFree_d(pm, __FILE__, __LINE__)
#define mxGetCell(pa, index)			mxGetCell_d(pa, index, __FILE__, __LINE__)
#define mxGetClassID(pa) 			mxGetClassID_d(pa, __FILE__, __LINE__)
#define mxGetClassName(pa) 			mxGetClassName_d(pa, __FILE__, __LINE__)
#define mxGetData(pa) mxGetData_d(pa, __FILE__, __LINE__)
#define mxGetDimensions(pa)  				mxGetDimensions_d(pa, __FILE__, __LINE__)
#define mxGetElementSize(pa)			mxGetElementSize_d(pa, __FILE__, __LINE__)
#define mxGetField(pa, index, fieldname) mxGetField_d(pa, index, fieldname, __FILE__, __LINE__)
#define mxGetFieldByNumber(pa, index, fieldnum) mxGetFieldByNumber_d(pa, index, fieldnum, __FILE__, __LINE__)
#define mxGetFieldNameByNumber(pa, fieldnum) mxGetFieldNameByNumber_d(pa, fieldnum, __FILE__, __LINE__)
#define mxGetFieldNumber(pa, fieldname) mxGetFieldNumber_d(pa, fieldname, __FILE__, __LINE__)
#define mxGetImagData(pa) mxGetImagData_d(pa, __FILE__, __LINE__)
#define mxGetIr(pa) mxGetIr_d(pa, __FILE__, __LINE__)
#define mxGetJc(pa) mxGetJc_d(pa, __FILE__, __LINE__)
#define mxGetName(pa)  				mxGetName_d(pa, __FILE__, __LINE__)
#define mxGetNumberOfDimensions(pa)	mxGetNumberOfDimensions_d(pa, __FILE__, __LINE__)
#define mxGetNumberOfElements(pa)	mxGetNumberOfElements_d(pa, __FILE__, __LINE__)
#define mxGetNumberOfFields(pa)			mxGetNumberOfFields_d(pa, __FILE__, __LINE__)
#define mxGetNzmax(pa)					mxGetNzmax_d(pa, __FILE__, __LINE__)
#define mxGetM(pa)					mxGetM_d(pa, __FILE__, __LINE__)
#define mxGetN(pa)					mxGetN_d(pa, __FILE__, __LINE__)
#define mxGetPi(pa) mxGetPi_d(pa, __FILE__, __LINE__)
#define mxGetPr(pa) mxGetPr_d(pa, __FILE__, __LINE__)
#define mxGetScalar(pa)					mxGetScalar_d(pa, __FILE__, __LINE__)
#define mxGetString(pa, buffer, buflen) mxGetString_d(pa, buffer, buflen, __FILE__, __LINE__)
#define mxIsCell(pa)					mxIsCell_d(pa, __FILE__, __LINE__)
#define mxIsChar(pa)					mxIsChar_d(pa, __FILE__, __LINE__)
#define mxIsClass(pa, classname) mxIsClass_d(pa, classname, __FILE__, __LINE__)
#define mxIsComplex(pa)					mxIsComplex_d(pa, __FILE__, __LINE__)
#define mxIsDouble(pa)					mxIsDouble_d(pa, __FILE__, __LINE__)
#define mxIsEmpty(pa)					mxIsEmpty_d(pa, __FILE__, __LINE__)
#define mxIsFromGlobalWS(pa)					mxIsFromGlobalWS_d(pa, __FILE__, __LINE__)
#define mxIsInt8(pa)					mxIsInt8_d(pa, __FILE__, __LINE__)
#define mxIsInt16(pa)					mxIsInt16_d(pa, __FILE__, __LINE__)
#define mxIsInt32(pa)					mxIsInt32_d(pa, __FILE__, __LINE__)
#define mxIsLogical(pa)					mxIsLogical_d(pa, __FILE__, __LINE__)
#define mxIsNumeric(pa)					mxIsNumeric_d(pa, __FILE__, __LINE__)
#define mxIsSingle(pa)					mxIsSingle_d(pa, __FILE__, __LINE__)
#define mxIsSparse(pa)					mxIsSparse_d(pa, __FILE__, __LINE__)
#define mxIsStruct(pa)					mxIsStruct_d(pa, __FILE__, __LINE__)
#define mxIsUint8(pa)					mxIsUint8_d(pa, __FILE__, __LINE__)
#define mxIsUint16(pa)					mxIsUint16_d(pa, __FILE__, __LINE__)
#define mxIsUint32(pa)					mxIsUint32_d(pa, __FILE__, __LINE__)
#define mxRealloc(pm, nelems)				mxRealloc_d(pm, nelems, __FILE__, __LINE__)
#if !defined(MATLAB_MEX_FILE)
#define mxSetAllocFcns(callocptr, freeptr, reallocptr, mallocptr) mxSetAllocFcns_d(callocptr, freeptr, reallocptr, freeptr, __FILE__, __LINE__)
#endif /* MATLAB_MEX_FILE */
#define mxSetCell(pa, index, value)		mxSetCell_d(pa, index, value, __FILE__, __LINE__)
#define mxSetClassName(pa, name)		mxSetClassName_d(pa, name, __FILE__, __LINE__)
#define mxSetData(pa, pd)				mxSetData_d(pa, pd, __FILE__, __LINE__)
#define mxSetDimensions(pa, size, ndims) mxSetDimensions_d(pa, size, ndims, __FILE__, __LINE__)
#define mxSetField(pa, index, fieldname, value) mxSetField_d(pa, index, fieldname, value, __FILE__, __LINE__)
#define mxSetFieldByNumber(pa, index, fieldnum, value) mxSetFieldByNumber_d(pa, index, fieldnum, value, __FILE__, __LINE__)
#define mxSetImagData(pa, pid)			   mxSetImagData_d(pa, pid, __FILE__, __LINE__)
#define mxSetIr(pa, ir)					mxSetIr_d(pa, ir, __FILE__, __LINE__)
#define mxSetJc(pa, jc)					mxSetJc_d(pa, jc, __FILE__, __LINE__)
#define mxSetLogical(pa)				mxSetLogical_d(pa, __FILE__, __LINE__)
#define mxSetM(pa, m)				mxSetM_d(pa, m, __FILE__, __LINE__)
#define mxSetN(pa, n)				mxSetN_d(pa, n, __FILE__, __LINE__)
#define mxSetName(pa, name) 		mxSetName_d(pa, name, __FILE__, __LINE__)
#define mxSetNzmax(pa, nzmax)			mxSetNzmax_d(pa, nzmax, __FILE__, __LINE__)
#define mxSetPi(pa, pi)					mxSetPi_d(pa, pi, __FILE__, __LINE__)
#define mxSetPr(pa, pr)					mxSetPr_d(pa, pr, __FILE__, __LINE__)
#endif

#ifdef __cplusplus
    }	/* extern "C" */
#endif

#endif /* matrix_h */
