defstar {
  name { Transpose }
  domain { MDSDF }
  desc { Transpose a floating-point matrix read as a single particle. }
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
    desc { The number of rows in the input matrix. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { The number of columns in the input matrix.  }
  }
  ccinclude { "SubMatrix.h" }
  setup {
    input.setMDSDFParams(int(numRows),int(numCols));
    output.setMDSDFParams(int(numCols),int(numRows));
  }
  go {
    FloatSubMatrix& matrix = *(FloatSubMatrix*)(input.getInput());
    FloatSubMatrix& result = *(FloatSubMatrix*)(output.getOutput());
    result = ~matrix;              // equivalent to matrix.transpose()
    delete &matrix;
    delete &result;
  }
}
