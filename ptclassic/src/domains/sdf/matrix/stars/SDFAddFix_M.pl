defstar {
	name { AddFix_M }
	domain { SDF }
	desc { Add two fixed-point matrices. }
	version { $Id$ }
	author { Mike J. Chen }
	copyright { 1993 The Regents of the University of California }
        location  { SDF matrix library }
	input {
		name { Ainput }
		type { FIX_MATRIX_ENV }
	}
	input {
		name { Binput }
		type { FIX_MATRIX_ENV }
	}
	output {
		name { output }
		type { FIX_MATRIX_ENV }
	}
	defstate {
		name { numRows }
		type { int }
		default { 8 }
		desc { The number of rows in the input matrices. }
	}
	defstate {
		name { numCols }
		type { int }
		default { 8 }
		desc { The number of columns in the input matrices. }
	}
        ccinclude { "Matrix.h" }
	go {
          // get inputs
          Envelope Apkt;
          (Ainput%0).getMessage(Apkt);
          const FixMatrix& Amatrix = *(const FixMatrix *)Apkt.myData();

          Envelope Bpkt;
          (Binput%0).getMessage(Bpkt);
          const FixMatrix& Bmatrix = *(const FixMatrix *)Bpkt.myData();

          // just check that A's dimensions match the state info.
          // the operator + on matricies will check that A matches B
          if((Amatrix.numRows() != int(numRows)) ||
             (Amatrix.numCols() != int(numCols))) {
            Error::abortRun(*this,"Dimension size of FixMatrix inputs do ",
                                  "not match the given state parameters.");
            return;
          }

          // do matrix addition
          FixMatrix *result = new FixMatrix(int(numRows),int(numCols));
          *result = Amatrix + Bmatrix;
          output%0 << *result;
	}
}

