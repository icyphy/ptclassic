defstar {
    name 	{ DataInt_M }
    domain 	{ SDF }
    version	{ $Id$ }
    author	{ Mike J. Chen }
    location    { SDF matrix library }
    descriptor	{
Generate a integer data matrix X from a stream of input particles.
The data matrix is a Toeplitz matrix such that the first row is
[ x(M-1) x(M-2) ... x(0) ], the second row is [ x(M) x(M-1) x(M-2) ... x(2) ],
and so forth until the last row, which is [ x(N-1) x(N-2) ... x(N-M) ].
This is the form of the matrix that is required by the SVD_M star,
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
	type	{ int	}
	desc	{ Input stream.}
    }
    output {
	name	{ output }
	type	{ INT_MATRIX_ENV }
	desc	{ The data matrix X. }
    }
    protected	{
        IntMatrix *X;
    }
    setup {
	input.setSDFParams(int(numRows)+int(numCols)-1);
    }
    hinclude 	{ "Matrix.h" }
    go {
        int i,j,k;
        // collect inputs and put into the matrix
        LOG_NEW; X = new IntMatrix(int(numRows),int(numCols));

	for(i = 0; i < int(numRows); i++) {
            k = int(numCols) - i - 1;
	    for(j = 0; j < int(numCols); j++,k++)
		(*X)[i][j] = input%k;
        }

	// Output X
        output%0 << *X;
    }		
}


