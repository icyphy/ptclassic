defstar {
	name {Const}
	domain {MDSDF}
	desc { Output a constant matrix.  }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	output {
		name { output }
		type { FLOAT_MATRIX }
	}
        defstate {
                name { numRows }
                type { int }
                default { 2 }
                desc {The number of rows in the output matrix.}
        }
        defstate {
                name { numCols }
                type { int }
                default { 2 }
                desc {The number of columns in the output matrix.}
        }
	defstate {
		name {level}
		type{float}
		default {"0.0"}
		desc {The constant value.}
	}
        setup {
            // set dimensions of output matrix
            output.setMDSDFParams(int(numRows),int(numCols));
        }
	go {
                FloatSubMatrix& out = *(FloatSubMatrix*)(output.getOutput());
		out = double(level);
		delete &out;
	}
}

