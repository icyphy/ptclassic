defstar {
  name { Gain }
  domain { MDSDF }
  desc { Multiply a floating-point matrix by a scalar gain value. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1994 The Regents of the University of California }
  location  { MDSDF library }
  input {
    name { input }
    type { FLOAT_MATRIX }
  }
  output {
    name { output }
    type { FLOAT_MATRIX }
  }
  defstate {
    name { numRows }
    type { int }
    default { 2 }
    desc { Number of rows in the input block. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { Number of columns in the input block. }
  }   
  defstate {
    name { gain }
    type { float }
    default { "1.0" }
    desc { Gain to be multiplied with the input matrix }
  }
  ccinclude { "SubMatrix.h" }
  setup {
    input.setMDSDFParams(int(numRows),int(numCols));
    output.setMDSDFParams(int(numRows),int(numCols));
  }
  go {
    // get a FloatSubMatrix access structure from the buffer
    FloatSubMatrix& in = *(FloatSubMatrix*)(input.getInput());
    FloatSubMatrix& out = *(FloatSubMatrix*)(output.getOutput());

    out = double(gain) * in;

    delete &in;
    delete &out;
  }
}

