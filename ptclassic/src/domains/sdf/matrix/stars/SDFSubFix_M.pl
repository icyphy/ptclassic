defstar {
	name { SubFix_M }
	domain { SDF }
	desc { Subtract fixed-point matrix B from A. }
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
		desc { The number of rows in Matrix A and B}
	}
	defstate {
		name { numCols }
		type { int }
		default { 2 }
		desc { The number of columns in Matrix A and B }
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
          if(Apkt.empty() && Bpkt.empty()) {
            // both empty, return a zero matrix with the given dimensions
            FixMatrix& result = *(new FixMatrix(int(numRows),int(numCols),
                                                out_len,out_IntBits));
            result = 0.0;
            output%0 << result;
          }
          else if(Apkt.empty()) {
            // Amatrix is empty but Bmatrix is not, return -Bmatrix
            FixMatrix& result = *(new FixMatrix(int(numRows),int(numCols),
                                                out_len,out_IntBits));
            result = -Bmatrix;
            output%0 << result;
          }
          else if(Bpkt.empty()) {
            // Bmatrix is empty but Amatrix is not, return just Amatrix
            // with a new output precision
            FixMatrix& result = *(new FixMatrix(int(numRows),int(numCols),
                                                out_len,out_IntBits));
            result = Amatrix;
            output%0 << result;
          }
          else {
            // Amatrix and Bmatrix both valid

            // just check that A's dimensions match the state info.
            // the operator - on matricies will check that A matches B
            if((Amatrix.numRows() != int(numRows)) ||
               (Amatrix.numCols() != int(numCols))) {
              Error::abortRun(*this,"Dimension size of FixMatrix inputs do ",
                                    "not match the given state parameters.");
              return;
            }
  
            // do matrix subtraction, much more complicated than the normal
            // case because of all the precision and overflow handling
            FixMatrix& result = *(new FixMatrix(int(numRows),int(numCols),
                                                out_len,out_IntBits));
            for(int i = 0; i < int(numRows) * int(numCols); i++) {
              (result.entry(i)).set_ovflow(OV); // set overflow function
              if(int(KeepArrivingPrecision))
                result.entry(i) = Amatrix.entry(i) - Bmatrix.entry(i);
              else {
                Fix newA = Fix(in_len,in_IntBits,Amatrix.entry(i));
                Fix newB = Fix(in_len,in_IntBits,Bmatrix.entry(i));
                result.entry(i) = newA - newB;
              }
            }
            output%0 << result;
          }
	}
}

