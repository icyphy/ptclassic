defstar {
	name { MpyFix_M }
	domain { SDF }
	desc {
Does a matrix multiplication of two input Fix Matricies A and B to
produce matrix C.
Matrix A has dimensions (numRows,X).
Matrix B has dimensions (X,numCols).
Matrix C has dimensions (numRows,numCols).

The user need only specify numRows and numCols.  An error will be generated
automatically if the number of columns in A does not match the number of
columns in B.
	}
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
		desc { The number of rows in Matrix A and Matrix C.}
	}
	defstate {
		name { numCols }
		type { int }
		default { 8 }
		desc { The number of columns in Matrix B and Matrix C}
	}
        ccinclude { "Matrix.h" }	
        go {
          // get inputs
          Envelope Apkt;
          (Ainput%0).getMessage(Apkt);
          const FixMatrix *Amatrix = (const FixMatrix *)Apkt.myData();

          Envelope Bpkt;
          (Binput%0).getMessage(Bpkt);
          const FixMatrix *Bmatrix = (const FixMatrix *)Bpkt.myData();

          if((Amatrix->numRows() != int(numRows)) ||
             (Bmatrix->numCols() != int(numCols))) {
            Error::abortRun(*this,"Dimension size of FixMatrix inputs do ",
                                  "not match the given state parameters.");
            return;
          }

          // do matrix multiplication
          FixMatrix *result = new FixMatrix(int(numRows),int(numCols));
          // we could write 
          //   *result = *Amatrix * *Bmatrix;
          // but the following is faster
          multiply(*Amatrix,*Bmatrix,*result);

          output%0 << *result;
	}
}

