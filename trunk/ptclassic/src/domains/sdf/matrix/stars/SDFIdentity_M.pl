defstar {
  name { Identity_M }
  domain { SDF }
  desc { Output a floating-point identity matrix. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
  }
  defstate {
    name { rowsCols }
    type { int }
    default { 2 }
    desc { Number of rows and columns of the output square matrix. }
  }
  ccinclude { "Matrix.h" }
  go {
    FloatMatrix& result = *(new FloatMatrix(int(rowsCols),int(rowsCols)));
    result.identity();
    output%0 << result;
  }
}
