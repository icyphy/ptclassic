defstar {
	name { Add_M }
	domain { SDF }
	desc { Add two floating-point matrices. }
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
		desc { The number of rows in the input matrices }
	}
	defstate {
		name { numCols }
		type { int }
		default { 8 }
		desc { The number of columns in the input matrices }
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

          // check for "null" matrix inputs, caused by delays
          if(Apkt.empty() && Bpkt.empty()) {
            // both empty, return a zero matrix with the given dimensions
            FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));
            result = 0.0;
            output%0 << result;
          }
          else if(Apkt.empty()) {
            // Amatrix is empty but B is not, return just B
            output%0 << Bmatrix;
          }
          else if(Bpkt.empty()) {
            // Bmatrix is empty but A is not, return just A
            output%0 << Amatrix;
          }
          else {
            // Amatrix and Bmatrix both valid

            // just check that A's dimensions match the state info.
            // the operator + on matricies will check that A matches B
          
            if((Amatrix.numRows() != int(numRows)) ||
               (Amatrix.numCols() != int(numCols))) {
              Error::abortRun(*this,"Dimension size of FloatMatrix inputs do ",
                                    "not match the given state parameters.");
              return;
            }

            // do matrix addition
            FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));
            result = Amatrix + Bmatrix;
            output%0 << result;
          }
	}
}
