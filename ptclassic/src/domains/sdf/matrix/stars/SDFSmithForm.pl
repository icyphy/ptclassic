defstar {
	name { SmithForm }
	domain { SDF }
	desc {
Decompose an integer matrix S into one of its Smith forms S = U D V,
where U, D, and V are simpler integer matrices. The Smith form
decomposition for integer matrices is analogous to singular
value decomposition for floating-point matrices.
	}
	version { $Id$ }
	author { Brian Evans }
	copyright { 1993 The Regents of the University of California }
        location  { SDF matrix library }
	explanation {
.pp
Smith forms are useful in making non-separable multidimensional 
operations that rely on integer matrix parameters separable.
Examples include multidimensional discrete Fourier transforms 
on arbitrary sampling grids (hexagonal, etc.) [1],
multidimensional non-separable up/downsampling [2], and 
multidimensional non-uniform filter bank design [3-5].
.pp
This function returns one of the Smith forms of an integer matrix $S$ 
by factoring $S$ into three simpler integer matrices such that
$S ~ = ~ U ~ D ~ V$ [6].
Here, $D$ is diagonal, and $U$ and $V$ have determinant of $+1$ or $-1$
(and are called \fIregular unimodular\fR).
Therefore, $|det ~ S| = |det ~ D|$.
Note that $S$ is $m$ x $n$, so $U$ is $m$ x $m$, $D$ is $m$ x $n$, and
$V$ is $n$ x $n$.
Hence, $U$ and $V$ are always square.
.pp
Smith form decompositions are not unique [5-6].
However, the Smith normal form imposes a canonical structure on $D$
that makes $D$ unique [6].
The canonical form of $D$ is that each diagonal element is a factor of
the next diagonal element.
Even in the canonical form, however, the $U$ and $V$ matrices are not unique.
Note that this routine \fIwas not\fR coded to return the Smith normal form
(although sometimes this will happen).
Converting a Smith form into a Smith normal form requires an extra
min($m$, $n$) steps for an $m$ x $n$ matrix [6].
.pp
The intermediate integer computations of the decomposition for integer
matrices of large dimension and for matrices with large integer entries
may exceed the integer precision of a given machine.
Because we have chosen to implement the decomposition in the native integer
format, this routine checks the decomposition at the end and flags an
error if the quantity $U ~ D ~ V$ does not give the original matrix.
The Smith form decomposition of integer matrices common in image and
video processing applications will not exceed integer precision.
.Ir "Guessoum, A."
.Ir "Vaidyanathan, P."
.Ir "Allebach, J."
.Ir "Viscito, E."
.Ir "Gardos, T."
.Ir "Nayebi, K."
.Ir "Mersereau, R."
.Ir "Evans, B."
.Ir "McClellan, J."
.Ir "Kaufmann, A."
.Ir "Henry-Labordiere, A."
.UH References
.ip [1]
A. Guessoum,
\fIFast Algorithms for the Multidimensional Discrete Fourier Transform\fR,
Ph. D. Thesis, Georgia Institute of Technology, Atlanta, GA, June, 1984.
.ip [2]
P. P. Vaidyanathan,
``The Role of Smith-Form Decomposition of Integer Matrices
in Multidimensional Multirate Systems,''
\fIInt. Conf. on Acoustics, Speech, and Signal Processing\fR,
Toronto, Canada, May, 1991, pp. 1777--1780,
.ip [3]
E. Viscito and J. Allebach,
``The Analysis and Design of Multidimensional {FIR} Perfect
Reconstruction Filter Banks For Arbitrary Sampling Lattices,''
\fITransactions on Circuits and Systems\fR,
vol. 38, no. 1, pp. 29-41, Jan., 1991.
.ip [4]
T. Gardos, K. Nayebi, and R. Mersereau,
``Analysis and Design of Multi-Dimensional Non-Uniform Band Filter Banks,''
\fISPIE Proc. Visual Communications and Image Processing\fR,
Nov., 1992, pp. 49-60.
.ip [5]
B. Evans, T. Gardos, and J. McClellan,
``Imposing Structure on Smith Form Decompositions of
Rational Resampling Matrices,''
\fITransactions on Signal Processing\fR,
vol. 42, April, 1994.
.ip [6]
A. Kaufmann and A. Henry-Labordiere,
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
#define INT_SWAP3(a,b,t)	{ t = a; a = b; b = t; }
	}
	code {

// Swap two rows in a matrix the slow but safe way
void intSwapRows( IntMatrix *mat, int row1, int row2 )
{
	int col, numcols, temp;

	numcols = mat->numCols();
	if ( row1 != row2 ) {
	  for ( col = 0; col < numcols; col++ )
	    INT_SWAP3((*mat)[row1][col], (*mat)[row2][col], temp)
	}

	return;
}

// Swap two columns in a matrix the slow but safe way
void intSwapCols( IntMatrix *mat, int col1, int col2 )
{
	int numrows, row, temp;

	numrows = mat->numRows();
	if ( col1 != col2 ) {
	  for ( row = 0; row < numrows; row++ )
	    INT_SWAP3((*mat)[row][col1], (*mat)[row][col2], temp)
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

	  m = (int) numRows;	// cast extracts the value of the state numRows
	  n = (int) numCols;	// cast extracts the value of the state numCols

          mVector = (int *) malloc( m * sizeof(int) );
	  nVector = (int *) malloc( n * sizeof(int) );
	  if ( ( mVector == (int *) NULL ) || ( nVector == (int *) NULL ) ) {
	    Error::abortRun( *this,
			     "The SDFSmithForm star cannot allocate ",
			     "dynamic memory." );
	    return;
	  }

          // initialize the matrices u, d, and v
          
          if(Apkt.empty()) { // check for empty inputs, possibly caused by delays
            IntMatrix& d = *(new IntMatrix(m,n));
            d = 0;
            mat = &d;
          }
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

	      // find indices of the pivot: non-zero entry with min value in d
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

	      // move the pivot to location (dim, dim) in d
	      // in terms of elementary matrices E and F working on U D V,
	      // U (E E^-1) D (F^-1 F) V = (U E) (E^-1 D F^-1) (F V)
	      intSwapRows( &d, dim, minrow );		/* E^-1 D */
	      intSwapCols( &d, dim, mincol );		/* E^-1 D F^-1 */

	      intSwapCols( &u, dim, minrow );		/* U E */
	      intSwapRows( &v, dim, mincol );		/* F V */

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

	  // free memory --  must put the (char *) cast for cfront
	  free( (char *) mVector );
	  free( (char *) nVector );

	  // output the Smith form matrices
          U%0 << u;
          D%0 << d;
          V%0 << v;
	}
}
