defstar {
  name { Transpose }
  domain { MDSDF }
  desc { Transpose a floating-point matrix read as a single particle. }
  version { @(#)MDSDFTranspose.pl	1.3 12/1/95 }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
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
