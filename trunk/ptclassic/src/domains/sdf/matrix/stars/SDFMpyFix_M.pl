defstar {
	name { MpyFix_M }
	domain { SDF }
	desc {
Multiply two fixed-point matrices A and B to produce matrix C.
Matrix A has dimensions (numRows,X). Matrix B has dimensions (X,numCols).
Matrix C has dimensions (numRows,numCols). The user need only specify
numRows and numCols. An error will be generated automatically if the
number of columns in A does not match the number of rows in B.
	}
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
                name { KeepArrivingPrecision }
                type { int }
                default { "YES" }
                desc {
Flag that indicates whether or not to keep the preciion of the
arriving matrix: YES keeps the same precision, and NO casts
them to the precision specified by the paramter "InputPrecision". }
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "4.14" }
                desc {
Precision of the input in bits.  The input matrix is only cast to
this precision if the parameter "ArrivingPrecision" is set to NO. }
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "4.14" }
                desc { Precision of the output matrix in bits. }
        }
        defstate {
                name { OverflowHandler }
                type { string }
                default { "saturate" }
                desc {
The type of overflow that happens if the result of any addition
overflows.  The options are: "saturate"(default), "zero-saturate",
"wrapped", "warning". }
        }
	defstate {
		name { numRows }
		type { int }
		default { 2 }
		desc { The number of rows in Matrix A and Matrix C.}
	}
	defstate {
		name { numCols }
		type { int }
		default { 2 }
		desc { The number of columns in Matrix B and Matrix C}
	}
        ccinclude { "Matrix.h" }	
        protected {
          const char* IP;
          const char* OP;
          const char* OV;
          int in_IntBits;
          int in_len;
          int out_IntBits;
          int out_len;
        }
        setup {
          IP = InputPrecision;
          OP = OutputPrecision;
          OV = OverflowHandler;
          in_IntBits = Fix::get_intBits(IP);
          in_len = Fix::get_length(IP);
          out_IntBits = Fix::get_intBits(OP);
          out_len = Fix::get_length(OP);
        }
        go {
          // get inputs
          Envelope Apkt;
          (Ainput%0).getMessage(Apkt);
          const FixMatrix& Amatrix = *(const FixMatrix *)Apkt.myData();

          Envelope Bpkt;
          (Binput%0).getMessage(Bpkt);
          const FixMatrix& Bmatrix = *(const FixMatrix *)Bpkt.myData();

          // check for "null" matrix inputs, caused by delays
          if(Apkt.empty() || Bpkt.empty()) {
            // if either empty, return a zero matrix with the given dimensions
            FixMatrix& result = *(new FixMatrix(int(numRows),int(numCols),
                                                out_len,out_IntBits));
            result = 0.0;
            output%0 << result;
          }
          else {
            // Amatrix and Bmatrix both valid

            if((Amatrix.numRows() != int(numRows)) ||
               (Bmatrix.numCols() != int(numCols))) {
              Error::abortRun(*this,"Dimension size of FixMatrix inputs do ",
                                    "not match the given state parameters.");
              return;
            }

            // do matrix multiplication, much more complicated than the normal
            // case because of all the precision and overflow handling
            FixMatrix& result = *(new FixMatrix(int(numRows),int(numCols),
                                                out_len,out_IntBits));

            for(int i = 0; i < int(numRows) * int(numCols); i++)
              (result.entry(i)).set_ovflow(OV); // set overflow function

            if(int(KeepArrivingPrecision)) 
              multiply(Amatrix,Bmatrix,result);
            else {
              FixMatrix newA = FixMatrix(Amatrix.numRows(),Amatrix.numCols(),
                                         in_len,in_IntBits);
              FixMatrix newB = FixMatrix(Bmatrix.numRows(),Bmatrix.numCols(),
                                         in_len,in_IntBits);
              newA = Amatrix;
              newB = Bmatrix;
              multiply(Amatrix,Bmatrix,result);
            }
            output%0 << result;
          }
	}
}

