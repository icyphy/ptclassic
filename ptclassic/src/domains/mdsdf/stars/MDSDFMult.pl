defstar {
	name { Mult }
	domain { MDSDF }
	desc {
Does a matrix multiplication of two input matrices A and B to produce matrix C.
Matrix A has dimensions M x N.
Matrix B has dimensions N x O.
Matrix C has dimensions M x O.
	}
	version { $Id$ }
	author { Mike J. Chen }
	copyright { 1993 The Regents of the University of California }
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
		name { AinputRows }
		type { int }
		default { 2 }
		desc { The number of rows in Matrix A }
	}
	defstate {
		name { AinputColsBinputRows }
		type { int }
		default { 2 }
		desc { The number of columns in Matrix A = rows in Matrix B }
	}
	defstate {
		name { BinputCols }
		type { int }
		default { 2 }
		desc { The number of columns in Matrix B }
	}
        ccinclude { "SubMatrix.h" }
	setup {
	    Ainput.setMDSDFParams(int(AinputRows), int(AinputColsBinputRows));
	    Binput.setMDSDFParams(int(AinputColsBinputRows), int(BinputCols));
	    output.setMDSDFParams(int(AinputRows), int(BinputCols));
	}
	go {
            // get a SubMatrix from the buffer
            FloatSubMatrix& input1 = *(FloatSubMatrix*)(Ainput.getInput());
            FloatSubMatrix& input2 = *(FloatSubMatrix*)(Binput.getInput());
            FloatSubMatrix& result = *(FloatSubMatrix*)(output.getOutput());

            // compute product, putting result into output
            result = input1 * input2;

            delete &input1;
            delete &input2;
            delete &result;
	}
}

