defstar {
	name { SmithForm }
	domain { SDF }
	desc {
Decompose an integer matrix S into one of its Smith forms S = U D V,
where U, D, and V are simpler integer matrices. The Smith form
decomposition for integer matrices is analogous to singular
value decomposition for floating-point matrices.
	}
	version { @(#)SDFSmithForm.pl	1.23	12/08/97 }
	author { Brian Evans }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location  { SDF matrix library }
	htmldoc {
<p>
Smith forms are useful in making non-separable multidimensional 
operations that rely on integer matrix parameters separable.
Examples include multidimensional discrete Fourier transforms 
on arbitrary sampling grids (hexagonal, etc.) [1],
multidimensional non-separable up/downsampling [2], and 
multidimensional non-uniform filter bank design [3-5].
<p>
This function returns one of the Smith forms of an integer matrix <i>S</i> 
by factoring <i>S</i> into three simpler integer matrices such that
<i>S   </i>=<i>   U   D   V</i> [6].
Here, <i>D</i> is diagonal, and <i>U</i> and <i>V</i> have determinant of <i>+</i>1 or <i>-</i>1
(and are called <i>regular unimodular</i>).
Therefore, <i>|det   S| </i>=<i> |det   D|</i>.
Note that <i>S</i> is <i>m</i> &#215; <i>n</i>, so <i>U</i> is <i>m</i> &#215; <i>m</i>, <i>D</i> is <i>m</i> &#215; <i>n</i>, and
<i>V</i> is <i>n</i> &#215; <i>n</i>.
Hence, <i>U</i> and <i>V</i> are always square.
<p>
Smith form decompositions are not unique [5-6].
However, the canonical structure imposes structure on the form of <i>D</i>
to make <i>D</i> unique [6].
The canonical form of <i>D</i> is that each diagonal element is a factor of
the next diagonal element.
Even in the canonical form, however, the <i>U</i> and <i>V</i> matrices are not unique.
This block will return the canonical form if the state
<i>SmithCanonicalForm</i> is <i>TRUE</i>.
<p>
The Smith form decomposition algorithm requires on the order of
(<i>n + m</i>)<sup>4</sup> operations.
Converting the Smith form into canonical form requires on the order of
min(<i>m, n</i>)<sup>2</sup> comparisons and min(<i>m, n</i>) arithmetic operations [6].
Therefore, it takes the same order of operations to compute a Smith
form or the Smith canonical form.
<p>
The intermediate integral computations of the decomposition for integer
matrices of large dimension and for matrices with large integer entries
may exceed the integer precision of a given machine.
Because we have chosen to implement the decomposition in the native integer
format, this routine checks the decomposition at the end and flags an
error if the quantity <i>U   D   V</i> does not give the original matrix.
The Smith form decomposition of integer matrices common in image and
video processing applications will not exceed integer precision.
<a name="Guessoum, A."></a>
<a name="Vaidyanathan, P."></a>
<a name="Allebach, J."></a>
<a name="Viscito, E."></a>
<a name="Gardos, T."></a>
<a name="Nayebi, K."></a>
<a name="Mersereau, R."></a>
<a name="Evans, B."></a>
<a name="McClellan, J."></a>
<a name="Kaufmann, A."></a>
<a name="Henry-Labordiere, A."></a>
<h3>References</h3>
<p>[1]  
A. Guessoum,
<i>Fast Algorithms for the Multidimensional Discrete Fourier Transform</i>,
Ph. D. Thesis, Georgia Institute of Technology, Atlanta, GA, June, 1984.
<p>[2]  
P. P. Vaidyanathan,
``The Role of Smith-Form Decomposition of Integer Matrices
in Multidimensional Multirate Systems,''
<i>Int. Conf. on Acoustics, Speech, and Signal Processing</i>,
Toronto, Canada, May, 1991, pp. 1777--1780.
<p>[3]  
E. Viscito and J. Allebach,
``The Analysis and Design of Multidimensional {FIR} Perfect
Reconstruction Filter Banks For Arbitrary Sampling Lattices,''
<i>Transactions on Circuits and Systems</i>,
vol. 38, no. 1, pp. 29-41, Jan., 1991.
<p>[4]  
T. Gardos, K. Nayebi, and R. Mersereau,
``Analysis and Design of Multi-Dimensional Non-Uniform Band Filter Banks,''
<i>Proc. SPIE Visual Communications and Image Processing</i>, vol. 1818,
pp. 49-60, Nov., 1992, Boston, MA.
<p>[5]  
B. Evans, T. Gardos, and J. McClellan,
``Imposing Structure on Smith Form Decompositions of
Rational Resampling Matrices,''
<i>Transactions on Signal Processing</i>,
vol. 42, no. 4, pp. 970-973, April, 1994.
<p>[6]  
A. Kaufmann and A. Henry-Labordiere,
<i>Integer and Mixed Programming: Theory and Applications</i>,
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
		
	  int i;     
	  for ( i = 0; i < m*n; i++)
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
