defstar {
    name 	{ DataFix_M }
    domain 	{ SDF }
    version	{ $Id$ }
    author	{ Mike J. Chen }
    location    { SDF matrix library }
    descriptor	{
Generate a fixed-point data matrix X from a stream of input particles
organized as shown below:

        X = [   [ x(M-1)      x(M-1)  ...     x(0)    ]
                [ x(M)        x(M)    ...     x(1)    ]
                .
                .
                .
                [ x(N-1)      x(N-2)  ...     x(N-M)] ]

This Toeplitz matrix is the form of the matrix that is required by the SVD_M star,
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
	type	{ fix	}
	desc	{ Input stream.}
    }
    output {
	name	{ output }
	type	{ FIX_MATRIX_ENV }
	desc	{ The data matrix X. }
    }
    protected	{
        FixMatrix *X;
    }
    setup {
	input.setSDFParams(int(numRows)+int(numCols)-1);
    }
    hinclude 	{ "Matrix.h" }
    go {
        int i,j,k;
        // collect inputs and put into the matrix
        LOG_NEW; X = new FixMatrix(int(numRows),int(numCols));

	for(i = 0; i < int(numRows); i++) {
            k = int(numCols) - i - 1;
	    for(j = 0; j < int(numCols); j++,k++)
		(*X)[i][j] = input%k;
        }

	// Output X
        output%0 << *X;
    }		
}


