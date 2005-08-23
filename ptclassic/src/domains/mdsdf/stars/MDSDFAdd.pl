defstar {
	name { Add }
	domain { MDSDF }
	desc {
Does a matrix addition of two input matrices A and B to produce matrix C.
All matrices must have the same dimensions.
	}
	version { @(#)MDSDFAdd.pl	1.4 12/1/95 }
	author { Mike J. Chen }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location  { MDSDF library }
	input {
		name { Ainput }
		type { FLOAT_MATRIX }
	}
	input {
		name { Binput }
		type { FLOAT_MATRIX }
	}
	output {
		name { output }
		type { FLOAT_MATRIX }
	}
	defstate {
		name { numRows }
		type { int }
		default { 2 }
		desc { The number of rows in the input/output matricies. }
	}
	defstate {
		name { numCols }
		type { int }
		default { 2 }
		desc { The number of columns in the input/output matricies. }
	}
        ccinclude { "SubMatrix.h" }
	setup {
	    Ainput.setMDSDFParams(int(numRows), int(numCols));
	    Binput.setMDSDFParams(int(numRows), int(numCols));
	    output.setMDSDFParams(int(numRows), int(numCols));
	}
	go {
            // get a SubMatrix from the buffer
            FloatSubMatrix& input1 = *(FloatSubMatrix*)(Ainput.getInput());
            FloatSubMatrix& input2 = *(FloatSubMatrix*)(Binput.getInput());
            FloatSubMatrix& result = *(FloatSubMatrix*)(output.getOutput());

            // compute product, putting result into output

            result = input1 + input2;
	    
	    delete &input1;
	    delete &input2;
	    delete &result;
	}
}
