defstar {
	name { Add }
	domain { MDSDF }
	desc {
Does a matrix addition of two input matrices A and B to produce matrix C.
All matrices must have the same dimensions.
	}
	version { $Id$ }
	author { Mike J. Chen }
	copyright { 1994 The Regents of the University of California }
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

