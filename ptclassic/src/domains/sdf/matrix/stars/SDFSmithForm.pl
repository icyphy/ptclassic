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
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
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
However, the canonical structure imposes structure on the form of $D$
to make $D$ unique [6].
The canonical form of $D$ is that each diagonal element is a factor of
the next diagonal element.
Even in the canonical form, however, the $U$ and $V$ matrices are not unique.
This block will return the canonical form if the state
\fISmithCanonicalForm\fR is \fITRUE\fR.
.pp
The Smith form decomposition algorithm requires on the order of
$[n + m]^4$ operations.
Converting the Smith form into canonical form requires on the order of
$min(m, n)^2$ comparisons and $min(m, n)$ arithmetic operations [6].
Therefore, it takes the same order of operations to compute a Smith
form or the Smith canonical form.
.pp
The intermediate integral computations of the decomposition for integer
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
Toronto, Canada, May, 1991, pp. 1777--1780.
.ip [3]
E. Viscito and J. Allebach,
``The Analysis and Design of Multidimensional {FIR} Perfect
Reconstruction Filter Banks For Arbitrary Sampling Lattices,''
\fITransactions on Circuits and Systems\fR,
vol. 38, no. 1, pp. 29-41, Jan., 1991.
.ip [4]
T. Gardos, K. Nayebi, and R. Mersereau,
``Analysis and Design of Multi-Dimensional Non-Uniform Band Filter Banks,''
\fIProc. SPIE Visual Communications and Image Processing\fR, vol. 1818,
pp. 49-60, Nov., 1992, Boston, MA.
.ip [5]
B. Evans, T. Gardos, and J. McClellan,
``Imposing Structure on Smith Form Decompositions of
Rational Resampling Matrices,''
\fITransactions on Signal Processing\fR,
vol. 42, no. 4, pp. 970-973, April, 1994.
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
		name { SmithCanonicalForm }
		type { int }
		default { FALSE }
		desc {
Whether or not to put the matrix into Smith canonical form. }
	}
	ccinclude { "Matrix.h", "ptdspSmithForm.h" }

	go {

	  // get the input
	  Envelope Apkt;
	  (S%0).getMessage(Apkt);
	  const IntMatrix *mat = (const IntMatrix *)Apkt.myData();

	  // check for empty inputs
	  if ( Apkt.empty() ) {
	    Error::abortRun( *this,
			     "Empty packet received in Smith form ",
			     "decomposition star." );
	  }

	  // initialize the local variables
	  int m = mat->numRows();	// number of rows in input matrix
	  int n = mat->numCols();	// number of columns in input matrix

	  int * dOutput = new int[m * n];
	  int * uOutput = new int[m * m];
	  int * vOutput = new int[n * n];

	  // FIXME
	  // Sets inputMat to the vector representing the IntMatrix *mat.
	  // This only works because in the underlying implementation of 
	  // IntMatrix, (*mat)[0] for example, which returns the 1st row of the 
	  // matrix, also returns the entire vector representing the matrix. 
	  // A method should be added to the IntMatrix class to do this instead
	  // of relying on this current operation
	  const int * inputMat = (*mat)[0];

	  Ptdsp_SmithForm (inputMat, dOutput, uOutput, vOutput, m, n);

	  if ( int(SmithCanonicalForm) )
	    Ptdsp_SmithCanonForm(dOutput, uOutput, vOutput, m, n);

	  // initialize the matrices u, d, and v
	  // do not delete them because they will be sent through the
	  // output ports and they will deleted automatically later
	  IntMatrix& d = *(new IntMatrix(m, n));
	  IntMatrix& u = *(new IntMatrix(m, m));
	  IntMatrix& v = *(new IntMatrix(n, n));

	  for (int i = 0; i < m*n; i++)
	    d.entry(i) = dOutput[i];
	  for ( i = 0; i < m*m; i++)
	    u.entry(i) = uOutput[i];
	  for ( i = 0; i < n*n; i++)
	    v.entry(i) = vOutput[i];

	  // the decomposition is valid if mat = u d v
	  int validflag = ( *mat == u * d * v );
	  if ( ! validflag ) {
	    Error::abortRun( *this,
			     "Error in Smith form decomposition: ",
			     "intermediate integral calculations ",
			     "may have exceed machine precision." );
	    return;
	  }

	  // output the Smith form matrices
	  U%0 << u;
	  D%0 << d;
	  V%0 << v;

	}
}
