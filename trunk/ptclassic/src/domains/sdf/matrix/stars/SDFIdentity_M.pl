defstar {
  name { Identity_M }
  domain { SDF }
  desc { Output a floating-point identity matrix. }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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
