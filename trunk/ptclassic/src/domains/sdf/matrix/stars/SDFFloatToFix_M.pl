defstar {
  name { FloatToFix_M }
  domain { SDF }
  desc {
Take an input FloatMatrix and convert it to an FixMatrix.  This
is done by constructing a new FixMatrix with the given precision.
Each entry of the FloatMatrix is cast using the given masking function
and then copied to the new matrix.
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
  }
  output {
    name { output }
    type { FIX_MATRIX_ENV }
  }
  defstate {
    name { FixPrecision }
    type { string }
    default { "4.14" }
    desc {
Precision of the entries in the FixMatrix.  The floating-point values
of the FloatMatrix are converted to this precision.   }
  }
  defstate {
    name { masking }
    type { string }
    default { "truncate" }
    desc {
Masking method.  This parameter is used to specify the way the floating-point
number is masked for casting to the fixed-point notation.  The keywords are:
"truncate" (default), "round". }
  }
  ccinclude { "Matrix.h" }
  protected {
    const char* Precision;
    const char* Masking;
    int intBits;
    int length;
  }
  setup {
    Precision = FixPrecision;
    Masking = masking;
    intBits = Fix::get_intBits(FixPrecision);
    length = Fix::get_length(FixPrecision);
  }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const FloatMatrix& matrix = *(const FloatMatrix *)inpkt.myData();

    // do conversion using copy constructor
    if(strcmp(Masking, "truncate") == 0) {
      FixMatrix& result = *(new FixMatrix(matrix,intBits,length,Fix::mask_truncate));
      output%0 << result;
    }
    else if(strcmp(Masking, "round") == 0) {
      FixMatrix& result = *(new FixMatrix(matrix,intBits,length,Fix::mask_truncate_round));
      output%0 << result;
    }
    Error::abortRun(*this, ": not a valid function for masking");
  }
}

