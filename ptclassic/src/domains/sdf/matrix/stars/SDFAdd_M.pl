defstar {
	name { Add_M }
	domain { SDF }
	desc {
Does a matrix addition of two input Float Matricies A and B to 
produce matrix C.
Matrix A has dimensions (numRows,numCols).
Matrix B has dimensions (numRows,numCols).
Matrix C has dimensions (numRows,numCols).
	}
	version { $Id$ }
	author { Mike J. Chen }
	copyright { 1993 The Regents of the University of California }
        location  { SDF matrix library }
	input {
		name { Ainput }
		type { FLOAT_MATRIX_ENV }
	}
	input {
		name { Binput }
		type { FLOAT_MATRIX_ENV }
	}
	output {
		name { output }
		type { FLOAT_MATRIX_ENV }
	}
	defstate {
		name { numRows }
		type { int }
		default { 8 }
		desc { The number of rows in Matrix A and B}
	}
	defstate {
		name { numCols }
		type { int }
		default { 8 }
		desc { The number of columns in Matrix A and B }
	}
        ccinclude { "Matrix.h" }
	go {
          // get inputs
          Envelope Apkt;
          (Ainput%0).getMessage(Apkt);
          const FloatMatrix& Amatrix = *(const FloatMatrix *)Apkt.myData();

          Envelope Bpkt;
          (Binput%0).getMessage(Bpkt);
          const FloatMatrix& Bmatrix = *(const FloatMatrix *)Bpkt.myData();

          // just check that A's dimensions match the state info.
          // the operator + on matricies will check that A matches B
          if((Amatrix.numRows() != int(numRows)) ||
             (Amatrix.numCols() != int(numCols))) {
            Error::abortRun(*this,"Dimension size of FloatMatrix inputs do ",
                                  "not match the given state parameters.");
            return;
          }

          // do matrix addition
          FloatMatrix *result = new FloatMatrix(int(numRows),int(numCols));
          *result = Amatrix + Bmatrix;
          output%0 << *result;
	}
}

