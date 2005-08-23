/*******************************************************************
Version identification:
@(#)ptdspSmithForm.c	1.6 04 Oct 1996

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

       Functions for decomposition of integer matrices into their
       Smith forms. 

********************************************************************/

#include "ptdspSmithForm.h"
#include "ptdspExtendedGCD.h"
#include <malloc.h>

#define INT_MIN_VALUE(m,n)      ( ( (m) < (n) ) ? (m) : (n) )
#define INT_ABS_VALUE(m)        ( ( (m) > 0 ) ? (m) : (-(m)) )
#define INT_IS_ZERO(x)          ( ! (x) )
#define INT_IS_NOT_ZERO(x)      (x)
#define INT_SWAP3(a,b,t)        { t = a; a = b; b = t; }

/* Swap two rows in matrix mat the slow but safe way. */
static void 
intSwapRows(int *mat, int row1, int row2, int numRows, int numCols ) {
  if (row1 != row2) {
    int col, temp;
    for ( col = 0; col < numCols; col++ )
      INT_SWAP3(mat[row1*numCols + col], mat[row2*numCols + col], temp);
  }
  return;
}

/* Swap two columns in matrix mat the slow but safe way. */
static void 
intSwapCols(int *mat, int col1, int col2, int numRows, int numCols) {
  if ( col1 != col2 ) {
    int temp, row;
    for ( row = 0; row < numRows; row++ )
      INT_SWAP3(mat[row*numCols + col1], mat[row*numCols + col2], temp);
  }
  return;
}


/* FIXME
   The two functions below, identity and mul, should be in a file of
   their own, a file that provides functions for manipulating matrices
   in the form of double arrays, when more functions are added. 
*/

/* Sets matrix mat to be an identity matrix. */
static void 
identity(int* mat, int numRows, int numCols) {
  int row;
  for ( row = 0; row < numRows; row++) {
    int col;
    for ( col = 0; col < numCols; col++) {
      *mat++ = (row == col) ? 1 : 0;
    }
  }
}

/* Matrix multiplication.
        outMatrix = src1 * src2
   Multiply two matrices, true matrix multiply, this is a fairly fast
   algorithm, especially when optimized by the compiler.
         outMatrix = src1 * src2
   Returns 1 if error, ie nCols1 != nRows2 
   This function writes the contents to a separate vector before
   copying to the outMatrix if the outMatrix is equal to the
   source. Thus this allows
             X = X * Y

   The return result is stored in integer array outMatrix .
*/
static int 
mul(int* outMatrix, const int* src1, const int nRows1, const int nCols1, 
    const int * src2, const int nRows2, const int nCols2) {
  int i = 0;
  int duplicateMatrix = 0;
  int* tempMat = 0;
  int* destMat = 0;
  int maxi = nRows1 * nCols2;

  if (nCols1 != nRows2) return 0;

  duplicateMatrix = ((outMatrix == src1 || outMatrix == src2));
  if (duplicateMatrix) tempMat = (int *) malloc(maxi * sizeof(int));
  destMat = (duplicateMatrix) ? tempMat : outMatrix;

  for( i = 0; i < nRows1; i++) {
    int j;
    for( j = 0; j < nCols2; j++) {
      int k;
      int temp = src1[i*nCols1] * src2[j];
      for (k = 1; k < nCols1; k++) {
        temp += src1[i*nCols1 + k] * src2[k*nCols2 + j];
      }
      destMat[i*nCols2 + j] = temp;
    }
  }
  if (duplicateMatrix) {
    for ( i = 0; i < maxi; i++) 
      outMatrix[i] = tempMat[i];
  }
  free(tempMat);
  return 1;
}

/* Decompose integer matrix inputMat into one of its Smith forms S = U D V,
   where U, D, and V are simpler integer matrices. Here, D is
   diagonal, and U and V have determinant of +1 or -1 (and are called
   regular unimodular). The Smith form decomposition for integer
   matrices is analogous to singular value decomposition for
   floating-point matrices. 

   U, D and V are stored in the integer arrays d, u and v,
   respectively. 
*/
void 
Ptdsp_SmithForm(const int* inputMat, int* d, int* u, int* v,
		const int m, const int n) {

  /* perform the basic Smith form decomposition */
  int* mVector = (int *) malloc(m * sizeof(int));
  int* nVector = (int *) malloc(n * sizeof(int));
  int r = INT_MIN_VALUE(m, n);
  int dim, i;

  /* copy contents of inputMat into d */
  for (i = 0; i < m * n; i++)
    d[i] = inputMat[i];

  /* set u and v to be identity matrixes */
  identity(u, m, m);
  identity(v, n, n);

  for (dim = 0; dim < r - 1; dim++) {
    int endflag = 0;
    while (! endflag) {
      int col, row;

      /* find indices of the pivot: non-zero entry with min value in d */
      int minrow = dim;
      int mincol = dim;
      int minvalue = d[minrow * n + mincol]; /* ie inputMat[minrow][mincol] */
      int minabsvalue = INT_ABS_VALUE(minvalue);
      for (row = dim; row < m; row++) {
	int col;
	for (col = dim; col < n; col++) {
	  int entry = d[row*n + col];
	  if ( entry < 0 ) entry = -entry;
	  if ( INT_IS_NOT_ZERO(entry) && ( entry < minabsvalue ) ) {
	    minabsvalue = entry;
	    minrow = row;
	    mincol = col;
	  }
	}
      }
      
      /* move the pivot to location (dim, dim) in d
	 in terms of elementary matrices E and F working on U D V,
	 U (E E^-1) D (F^-1 F) V = (U E) (E^-1 D F^-1) (F V) */
      intSwapRows( d, dim, minrow, m, n );		/* E^-1 D */
      intSwapCols( d, dim, mincol, m, n );		/* E^-1 D F^-1 */
      
      intSwapCols( u, dim, minrow, m, m );		/* U E */
      intSwapRows( v, dim, mincol, n, n );		/* F V */
      
      /* eliminate elements in column dim except for element dim by
	 U (E E^-1) D (F^-1 F) V = (U E) (E^-1 D F^-1) (F V)
	 E^-1 is identity except for column dim which equals mVector
	 F^-1 is identity except for row dim which is equal to nVector
	 Therefore, full matrix multiplication is not required
	 We must be careful because we are performing updates in place */
      minvalue = d[dim*n + dim];
      for (row = dim + 1; row < m; row++ )
	mVector[row] = -d[row*n + dim] / minvalue;
      for (col = dim + 1; col < n; col++ )
	nVector[col] = -d[dim*n + col] / minvalue;
      
      for (row = dim + 1; row < m; row++) {
	int quotient = mVector[row];
	for ( col = dim + 1; col < n; col++ ) {
	  d[row*n + col] += quotient * d[dim*n + col];
	}
	d[row*n + dim] += quotient * minvalue;
      }						/* E^-1 D */
      for (col = dim + 1; col < n; col++) {
	int quotient = nVector[col];
	for ( row = dim + 1; row < m; row++ ) {
	  d[row*n + col] += d[row*n + dim] * quotient;
	}
	d[dim*n + col] += quotient * minvalue;
      }						/* (E^-1 D) F^-1 */

      for (row = 0; row < m; row++) {
	int sum = u[row*m + dim];
	for ( col = dim + 1; col < m; col++ )
	  sum -= u[row*m + col] * mVector[col];
	u[row*m + dim] = sum;
      }						/* U E */
      for ( col = 0; col < n; col++ ) {
	int sum = v[dim*n + col];
	for ( row = dim + 1; row < n; row++ )
	  sum -= nVector[row] * v[row*n + col];
	v[dim*n + col] = sum;
      }						/* F V */
      
      /* check for the ending condition for this iteration */
      endflag = 1;
      for (col = dim + 1; col < n; col++)
	if ( INT_IS_NOT_ZERO(d[dim*n + col]) ) {
	  endflag = 0;
	  break;
	}
      if (endflag) {
	for (row = dim + 1; row < m; row++)
	  if ( INT_IS_NOT_ZERO(d[row*n + dim]) ) {
	    endflag = 0;
	    break;
	  }
      }
    }
  }
  free(mVector);
  free(nVector);
}
  
/* Put the Smith form into canonical form.
   This function takes the result of D, U, and V of a SmithForm
   decomposition, and puts them into the canonical form. The result is
   that D is now unique. Note, however, that U and V are still not
   unique. 
   The processes carried out are :
   (1) pulling out negative signs 
   (2) sorting diagonal entries 
   (3) shuffling factors along diagonal by iteratively finding regular
       unimodular G and H such that
   U D V = U (G^-1 G) D (H H^-1) V = (U G^-1) (G D H) (H^-1 V)
       so that G and H move D closer to canonical form.

   Integer arrays d, u and v are modified into canonical form.
*/
void 
Ptdsp_SmithCanonForm (int * d, int * u, int * v, const int m, const int n) {
  
  int* ginv = (int *) malloc(m * m * sizeof(int));
  int* hinv = (int *) malloc(n * n * sizeof(int));
  int i, j, di, jend, temp, lastd, lasti;
  int lcmvalue, lambda, mu, gcdvalue;
  int r = INT_MIN_VALUE(m, n);

  /* (1) pull out negative signs */
  identity(ginv, m, m);
  for ( i = 0; i < r; i++ ) {
    di = d[i*n + i];
    if ( di < 0 ) {
      d[i*n + i] = -di;
      ginv[i*m + i] = -1;
    }
  }
  mul(u, u, m, m, ginv, m, m);
  
  /* (2) sort diagonal elements */
  for ( i = 0; i < r; i++ ) {
    jend = r - i - 1;
    for ( j = 0; j < jend; j++ ) {
      if ( d[j*n + j] > d[(j+1)*n + (j+1)] ) {
	INT_SWAP3(d[j*n + j], d[(j+1)*n + (j+1)], temp);
	intSwapCols( u, j, j+1, m, m );
	intSwapRows( v, j, j+1, n, n );
      }
    }
  }
  
  /* (3) shuffle factors along the diagonal */
  lastd = d[0];
  lasti = 0; 

  /*  process second to last diagonal elements, i = 1 ... r-1 */
  for ( i = 1; i < r; i++ ) {
    di = d[i*n + i];
    gcdvalue = Ptdsp_ExtendedGCD(lastd, di, &lambda, &mu);
    if ( gcdvalue == lastd ) {
      lastd = di;
      lasti = i;
      continue;
    }
    lcmvalue = Ptdsp_LCM(lastd, di);
    
    /* define the G^-1 matrix */
    identity(ginv, m, m);
    ginv[lasti*m + lasti] = lambda * lastd / gcdvalue;
    ginv[lasti*m + i] = -1;
    ginv[i*m + lasti] = mu * di / gcdvalue;
    ginv[i*m + i] = 1;
    
    /* define the H^-1 matrix */
    identity(hinv, n, n);
    hinv[lasti*n + lasti] = lastd / gcdvalue;
    hinv[lasti*n + i] = di / gcdvalue;
    hinv[i*n + lasti] = -mu;
    hinv[i*n + i] = lambda;
    
    /* shuffle factors on the diagonal matrix D */
    d[lasti*n + lasti] = gcdvalue;
    d[i*n + i] = lcmvalue;
    
    /* update U and V matrices */
    mul(u, u, m, m, ginv, m, m);
    mul(v, hinv, n, n, v, n, n);

    lastd = lcmvalue;
    lasti = i;
  }

  free(hinv);
  free(ginv);
}  
