defstar {
    name 	{ Toeplitz_M }
    domain 	{ SDF }
    version	{ @(#)SDFToeplitz_M.pl	1.16 06 Oct 1996 }
    author	{ Mike J. Chen }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location    { SDF matrix library }
    desc        {
Put a sequence of real-valued samples into a rectangular Toeplitz matrix.
    }
	htmldoc {
<a name="Toeplitz matrix"></a>
Generate a complex data matrix <i>X</i>, with dimensions
<i>numRows</i> &#215; <i>numCols</i>, from a stream of
<i>numRows</i> + <i>numCols</i> - 1 input particles.
The data matrix is a Toeplitz matrix such that the first row is
[ x(M-1) x(M-2) ... x(0) ], the second row is [ x(M) x(M-1) x(M-2) ... x(1) ],
and so forth until the last row, which is [ x(N-1) x(N-2) ... x(N-M) ], where
<i>numRows</i> = <i>N-M+</i>1 and <i>numCols</i> = <i>M</i>.
<a name="singular-value decomposition"></a>
This is the form of the matrix that is required by the singular-value
decomposition star
<tt>SVD_M</tt>,
among others.
    } 
    defstate {
	name 	{ numRows }
	type 	{ int }
	default	{ 2 }	
	desc	{ Number of rows in X. }
    }
    defstate {
	name 	{ numCols }
	type	{ int }
	default { 2 }
	desc	{ Number of columns in X. }
    }
    input {
	name	{ input }
	type	{ float	}
	desc	{ Input stream.}
    }
    output {
	name	{ output }
	type	{ FLOAT_MATRIX_ENV }
	desc	{ The data matrix X. }
    }
    setup {
	input.setSDFParams(int(numRows)+int(numCols)-1);
    }
    hinclude 	{ "Matrix.h" }
    go {
	int numrows = int(numRows);
	int numcols = int(numCols);

        // collect inputs and put into the matrix
        FloatMatrix& X = *(new FloatMatrix(numrows, numcols));

	for (int i = 0; i < numrows; i++) {
            int k = numrows - i - 1;
	    for (int j = 0; j < numcols; j++, k++)
		X[i][j] = input%k;
        }

	// Output X
        output%0 << X;
    }		
}
