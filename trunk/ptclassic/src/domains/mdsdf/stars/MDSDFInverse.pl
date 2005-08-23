defstar {
	name { Inverse }
	domain { MDSDF }
	desc { Invert a square floating-point matrix.}
	version { @(#)MDSDFInverse.pl	1.4 12/1/95 }
	author { Mike J. Chen }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location  { MDSDF library }
	input {
		name { input }
		type { FLOAT_MATRIX }
	}
	output {
		name { output }
		type { FLOAT_MATRIX }
	}
	defstate {
		name { rowsCols }
		type { int }
		default { 2 }
		desc { The number of rows/columns of the input square matrix. }
	}
        ccinclude { "SubMatrix.h" }
        setup {
                input.setMDSDFParams(int(rowsCols),int(rowsCols));
		output.setMDSDFParams(int(rowsCols),int(rowsCols));
	}

	go {
          FloatSubMatrix& matrix = *(FloatSubMatrix*)(input.getInput());
	  FloatSubMatrix& result = *(FloatSubMatrix*)(output.getOutput());
	  result = !matrix;              // invert the matrix
          delete &matrix;
	  delete &result;
        }
}
