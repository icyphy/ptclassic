defstar {
	name { SmithForm }
	domain { SDF }
	desc {
 Decomposes an integer matrix S into a Smith form S = U D V, where
U, D, and V are integer matrices.
 The Smith form decomposition for integer matrices is analogous to
Singular Value Decomposition for floating-point matrices.
	}
	version { $Id$ }
	author { Brian Evans }
	copyright { 1993 The Regents of the University of California }
        location  { SDF matrix library }
	explanation {
.pp
 Smith forms are useful in making non-separable multidimensional 
operations that rely on integer matrices separable.
 Examples include multidimensional discrete Fourier transforms 
on an arbitrary sampling grid (hexagonal, etc.) [1],
multidimensional non-separable up/downsampling [2], and 
multidimensional non-uniform filter bank design [3-4].
.pp
 This function returns the Smith form of an integer matrix S [5].
 The matrix $S$ is factored into three simpler integer matrices such that
$S = U D V$.
 Here, $D$ is diagonal and
$U$ and $V$ have determinant of $+1$ or $-1$
(and are called regular unimodular).
 Therefore, $|det S| = |det D|$.
 Note that $S$ is m x n, so $U$ is m x m, $D$ is m x n, and $V$ is n x n.
 Hence, $U$ and $V$ are always square.
.pp
 Smith form decompositions are not unique [4].
 However, the Smith normal form imposes a canonical structure on $D$
so that $D$ is unique [5].
 The canonical form of $D$ is that each diagonal element is a factor of
the next diagonal element and the greatest common divisor of two adjacent
diagonal elements is one.
 Even in the canonical form, however, the $U$ and $V$ matrices are not unique.
 Note that this routine \fIwas not\fR coded to return the Smith normal form
(although sometimes this will happen).
 Converting a Smith form to a Smith normal form requires an extra n steps
for an n x n matrix [5].
.pp
 Because we have chosen to implement the decomposition using
integers represented by a computer, the intermediate integer
computations for either large integer matrices or for matrices
with large integer entries may exceed the precision of integers
on a given machine.
 As a consequence, we check the decomposition at the end and flag an
error if it does not give the original matrix.
.Ir "Guessoum, A."
.Ir "Vaidyanathan, P."
.Ir "Gardos, T."
.Ir "Nayebi, K."
.Ir "Mersereau, R."
.Ir "Evans, B."
.Ir "McClellan, J."
.Ir "Kaufmann, A."
.Ir "Henry-Labordiere, A."
.UH REFERENCES
.ip [1]
Guessoum, A.,
\fIFast Algorithms for the Multidimensional Discrete Fourier Transform\fR,
Ph. D. Thesis, Georgia Institute of Technology, Atlanta, GA, June, 1984.
.ip [2]
Vaidyanathan, P.
``The Role of Smith-Form Decomposition of Integer Matrices
in Multidimensional Multirate Systems,''
\fIInt. Conf. on Acoustics, Speech, and Signal Processing\fR,
Toronto, Canada, May, 1991, pp. 1777--1780,
.ip [3]
Gardos, T., Nayebi, K., and Mersereau, R.,
``Analysis and Design of Multi-Dimensional Non-Uniform Band Filter Banks,''
\fISPIE Proc. Visual Communications and Image Processing\fR,
Nov., 1992, pp. 49-60.
.ip [4]
Evans, B., Gardos, T., and McClellan, J.,
``Imposing Structure on Smith Form Decompositions of
Rational Resampling Matrices,''
\fITransactions on Signal Processing\fR,
vol. 42, April, 1994.
.ip [5]
Kaufmann, A., and Henry-Labordiere, A.,
\fIInteger and Mixed Programming: Theory and Applications\fR,
Academic Press, New York, 1977.
	}
	input {
		name { S }
		type { INT_MATRIX_ENV }
		desc { Rectangular or square integer input matrix. }
	}
	output {
		name { U }
		type { INT_MATRIX_ENV }
		desc { Left regular unimodular integer matrix. }
	}
	output {
		name { D }
		type { INT_MATRIX_ENV }
		desc { Diagonal integer matrix. }
	}
	output {
		name { V }
		type { INT_MATRIX_ENV }
		desc { Right regular unimodular integer matrix. }
	}
	defstate {
		name { numRows }
		type { int }
		default { 3 }
		desc { The number of rows in the input matrix. }
	}
	defstate {
		name { numCols }
		type { int }
		default { 3 }
		desc { The number of columns in the input matrix. }
	}
        ccinclude { "Matrix.h" }
	header {
#define intmin(m,n)             ( ( (m) < (n) ) ? (m) : (n) )
#define intabs(m)               ( ( (m) > 0 ) ? (m) : (-(m)) )

#define INT_IS_ZERO(x)          ( ! (x) )
#define INT_IS_NOT_ZERO(x)      (x)
	}
	code {

void intSwapRows( IntMatrix mat, int row1, int row2 )
{
	int col, numcols, temp;
	int *ptr1;
	int *ptr2;

	numcols = mat.numCols();
	if ( row1 != row2 ) {
	  ptr1 = &mat[row1][0];
	  ptr2 = &mat[row2][0];
	  for ( col = 0; col < numcols; col++ ) {
	    temp = *ptr1;
	    *ptr1++ = *ptr2;
	    *ptr2++ = temp;
	  }
	}

	return;
}

void intSwapCols( IntMatrix mat, int col1, int col2 )
{
	int col, numcols, temp;
	int *ptr1;
	int *ptr2;

	numcols = mat.numCols();
	if ( col1 != col2 ) {
	  ptr1 = &mat[0][col1];
	  ptr2 = &mat[0][col2];
	  for ( col = 0; col < numcols; col++ ) {
	    temp = *ptr1;
	    *ptr1 = *ptr2;
	    *ptr2 = temp;
	    ptr1 += numcols;
	    ptr2 += numcols;
	  }
	}

	return;
}

	}
	go {
          // get the input
          Envelope Apkt;
          (S%0).getMessage(Apkt);
          const IntMatrix *mat = (const IntMatrix *)Apkt.myData();

	  // initialize the local variables
	  int col, dim, entry, m, mincol, minrow, minabsvalue, minvalue,
	      n, quotient, r, row, sum;
	  int *mVector;
	  int *nVector;
	  int endflag;

	  m = numRows;
	  n = numCols;

          mVector = (int *) malloc( m * sizeof(int) );
	  nVector = (int *) malloc( n * sizeof(int) );
	  if ( ( mVector == (int *) NULL ) || ( nVector == (int *) NULL ) ) {
	    Error::abortRun( *this,
			     "The SDFSmithForm star cannot allocate ",
			     "dynamic memory." );
	    return;
	  }

          // initialize the matrices u, d, and v
          IntMatrix& d = *(new IntMatrix(*mat));
	  IntMatrix& u = *(new IntMatrix(m, m));
	  u.identity();
	  IntMatrix& v = *(new IntMatrix(n, n));
	  v.identity();

	  // perform the decomposition
	  r = intmin(m, n);
	  for ( dim = 0; dim < r - 1; dim++ ) {
	    endflag = FALSE;
	    while ( ! endflag ) {

	      // find the indices of the pivot: non-zero entry with min value
	      mincol = minrow = dim;
	      minvalue = d[mincol][minrow];
	      minabsvalue = intabs(minvalue);
	      for ( row = dim; row < m; row++ ) {
	        for ( col = dim; col < n; col++ ) {
		  entry = d[row][col];
		  if ( entry < 0 ) entry = -entry;
		  if ( INT_IS_NOT_ZERO(entry) && ( entry < minabsvalue ) ) {
		    minabsvalue = entry;
		    minrow = row;
		    mincol = col;
		  }
	        }
	      }

	      // move the pivot to location (dim, dim) via elementary matrices
	      // U (E E^-1) D (F^-1 F) V = (U E) (E^-1 D F^-1) (F V)
	      intSwapRows( d, dim, minrow );		/* E^-1 D */
	      intSwapCols( d, dim, mincol );		/* E^-1 D F^-1 */

	      intSwapCols( u, dim, minrow );		/* U E */
	      intSwapRows( v, dim, mincol );		/* F V */

	      // eliminate elements in column dim except for element dim by
	      // U (E E^-1) D (F^-1 F) V = (U E) (E^-1 D F^-1) (F V)
	      // E^-1 is identity except for column dim which equals mVector
	      // F^-1 is identity except for row dim which is equal to nVector
	      // Therefore, full matrix multiplication is not required
	      // We must be careful because we are performing updates in place
	      minvalue = d[dim][dim];
	      for ( row = dim + 1; row < m; row++ )
	        mVector[row] = -d[row][dim] / minvalue;
	      for ( col = dim + 1; col < n; col++ )
	        nVector[col] = -d[dim][col] / minvalue;

	      for ( row = dim + 1; row < m; row++ ) {
	        quotient = mVector[row];
	        for ( col = dim + 1; col < n; col++ ) {
		  d[row][col] += quotient * d[dim][col];
	        }
	        d[row][dim] += quotient * minvalue;
	      }						/* E^-1 D */
	      for ( col = dim + 1; col < n; col++ ) {
	        quotient = nVector[col];
	        for ( row = dim + 1; row < m; row++ ) {
		  d[row][col] += d[row][dim] * quotient;
	        }
	        d[dim][col] += quotient * minvalue;
	      }						/* (E^-1 D) F^-1 */

	      for ( row = 0; row < m; row++ ) {
	        sum = u[row][dim];
	        for ( col = dim + 1; col < m; col++ )
		  sum -= u[row][col] * mVector[col];
	        u[row][dim] = sum;
	      }						/* U E */
	      for ( col = 0; col < n; col++ ) {
	        sum = v[dim][col];
	        for ( row = dim + 1; row < n; row++ )
		  sum -= nVector[row] * v[row][col];
	        v[dim][col] = sum;
	      }						/* F V */

	      // check for the ending condition for this iteration
	      endflag = TRUE;
	      for ( col = dim + 1; col < n; col++ )
	        if ( INT_IS_NOT_ZERO(d[dim][col]) ) {
		  endflag = FALSE;
		  break;
	        }
	      if ( endflag ) {
	        for ( row = dim + 1; row < m; row++ )
		  if ( INT_IS_NOT_ZERO(d[row][dim]) ) {
		    endflag = FALSE;
		    break;
	        }
	      }
	    }
	  }

	  // the decomposition is valid if mat = u d v
	  int validflag = ( *mat == u * d * v );
	  if ( ! validflag ) {
	    Error::abortRun( *this,
			     "Error in Smith form decomposition: ",
			     "intermediate integral calculations ",
			     "may have exceed machine precision." );
	    return;
	  }

	  // free memory 
	  free( mVector );
	  free( nVector );

	  // output the Smith form matrices
          U%0 << u;
          D%0 << d;
          V%0 << v;
	}
}
