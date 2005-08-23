defstar {
  name { MpyScalarFix_M }
  domain { SDF }
  desc { Multiply a fixed-point matrix by a scalar input gain value. }
  version { @(#)SDFMpyScalarFix_M.pl	1.6 12/09/97 }
  author { Bilung Lee, modified from SDFGainFix.pl by Mike J. Chen }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
    name { input }
    type { FIX_MATRIX_ENV }
  }
  input {
    name { gain }
    type { fix }
    desc { Input gain to be multiplied with the input matrix }
  }
  output {
    name { output }
    type { FIX_MATRIX_ENV }
  }
  defstate {
    name { UseArrivingPrecision }
    type {int}
    default {"YES"}
    desc {
Flag indicated whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the
precision specified by the parameter "NewInputPrecision". }
  }
  defstate {
    name { NewInputPrecision }
    type { string }
    default { "2.14" }
    desc {
New precision of the input in bits.  The input particles are only cast
to this precision if the parameter "UseArrivingPrecision" is set to NO.}
  }
  defstate {
    name { OutputPrecision }
    type { string }
    default { "2.14" }
    desc {
Precision of the output in bits.  This is the precision that will hold
the result of the arithmetic operation on the inputs.}
  }
  defstate {
    name { OverflowHandler }
    type { string }
    default { "saturate" }
    desc {
Overflow characteristic for the output.  If the result
of the product cannot be fit into the precision of the output, overflow
occurs and the overflow is taken care of by the method specified by this
parameter.  The keywords for overflow handling methods are :
"saturate"(default), "zero_saturate", "wrapped", "warning". }
  }
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
    IP = NewInputPrecision;
    OP = OutputPrecision;
    OV = OverflowHandler;
    in_IntBits = Fix::get_intBits (IP);
    in_len = Fix::get_length (IP);
    out_IntBits = Fix::get_intBits (OP);
    out_len = Fix::get_length (OP);
  }
  ccinclude { "Matrix.h" }
  go {    
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const FixMatrix& matrix = *(const FixMatrix *)inpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(inpkt.empty()) {
      // input empty, just send it back out
      output%0 << inpkt;
    }
    else {
      // valid input matrix

      FixMatrix& result = *(new FixMatrix(matrix.numRows(),matrix.numCols(),
                                          out_len, out_IntBits));

      Fix fixIn;

      // do scalar * matrix
      for(int i = 0; i < (matrix.numRows() * matrix.numCols()); i++) {
        if(int(UseArrivingPrecision))
          fixIn = matrix.entry(i);
        else {
	  // Use a temporary variable to avoid gcc2.7.2/2.8 problems
	  Fix tmp = Fix(in_len, in_IntBits, matrix.entry(i));
          fixIn = tmp;
	}
        result.entry(i).set_ovflow(OV);
        result.entry(i) = fixIn * (const Fix&)(gain%0);
      }

      output%0 << result;
    }
  }
}


