defstar {
	name {Repeat}
	domain {MDSDF}
	desc { Output a constant matrix with the value given at the input.  }
	version { $Id$ }
	author { Mike J. Chen and E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	input {
		name { input }
		type { FLOAT_MATRIX }
	}
	output {
		name { output }
		type { FLOAT_MATRIX }
	}
        defstate {
                name { numInputRows }
                type { int }
                default { 1 }
                desc {The number of rows in the input matrix.}
        }
        defstate {
                name { numInputCols }
                type { int }
                default { 1 }
                desc {The number of columns in the input matrix.}
        }
	defstate {
		name { rowRepetitionFactor }
		type{ int }
		default { 2 }
		desc { Duplication factor in the vertical direction }
	}
	defstate {
		name { colRepetitionFactor }
		type{ int }
		default { 2 }
		desc { Duplication factor in the horizontal direction }
	}
        setup {
            // set dimensions of output matrix
            input.setMDSDFParams(int(numInputRows),int(numInputCols));
            output.setMDSDFParams(int(numInputRows)*int(rowRepetitionFactor),
				  int(numInputCols)*int(colRepetitionFactor));
        }
	go {
                FloatSubMatrix& in = *(FloatSubMatrix*)(input.getInput());
                FloatSubMatrix& out = *(FloatSubMatrix*)(output.getOutput());
		for (int i = 0; i < int(rowRepetitionFactor); i++) {
		    for (int j = 0; j < int(colRepetitionFactor); j++) {
			// Create a submatrix of the output matrix
			FloatSubMatrix temp(out,
				i*int(numInputRows),	// starting row number
				j*int(numInputCols),	// starting col number
				int(numInputRows),	// number of rows
				int(numInputCols));	// number of cols
			temp = in;
		    }
		}
		delete &out;
		delete &in;
	}
}

