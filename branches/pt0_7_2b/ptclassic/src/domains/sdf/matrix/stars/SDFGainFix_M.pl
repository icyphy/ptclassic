defstar {
  name { GainFix_M }
  domain { SDF }
  desc {
    Takes an input FixMatrix and multiplies it by a scalar gain value.
    The value of the "gain" parameter and its precision in bits can currently 
    be specified using two different notations.
    Specifying only a value by itself in the dialog box would create a
    fixed-point number with the default precision which has a total length
    of 24 bits with the number of range bits as required by the value.
    For example, the default value 1.0 creates a fixed-point object with
    precision 2.22, and a value like 0.5 would create one with precision
    1.23.  An alternative way of specifying the value and the
    precision of this parameter is to use the parenthesis notation
    of (value, precision).  For example, filling the dialog
    box for the gain parameter with (2.546, 3.5) would create a fixed-point
    object formed by casting the double-precision floating-point number
    2.546 to a fixed-point number with a precision of 3.5.
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { FIX_MATRIX_ENV }
  }
  output {
    name { output }
    type { FIX_MATRIX_ENV }
  }
  defstate {
    name { gain }
    type { fix }
    default { "1.0" }
    desc { Gain to be multiplied with the input matrix }
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
    const FixMatrix *matrix = (const FixMatrix *)inpkt.myData();

    FixMatrix *result = new FixMatrix(matrix->numRows(),matrix->numCols(),
                                      out_len, out_IntBits);

    Fix fixIn;

    // do scalar * matrix
    for(int i = 0; i < matrix->numRows() * matrix->numCols(); i++) {
      if(int(UseArrivingPrecision))
        fixIn = matrix->entry(i);
      else
        fixIn = Fix(in_len, in_IntBits, matrix->entry(i));
      result->entry(i).set_ovflow(OV);
      result->entry(i) = fixIn * Fix(gain);
    }

    output%0 << *result;
  }
}


