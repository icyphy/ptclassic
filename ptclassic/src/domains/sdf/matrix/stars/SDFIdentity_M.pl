defstar {
  name { Identity_M }
  domain { SDF }
  desc {
Make and identity matrix of the given size.  An indentity matrix is square
with all diagonal entries equal to 1 and the rest equal to 0.
The output matrix has dimensions (rowsCols,rowsCols).
  }
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
    default { 8 }
    desc { Number of rows/columns of the output square matrix. }
  }

  ccinclude { "Matrix.h" }
  go {
    FloatMatrix *result = new FloatMatrix(int(rowsCols),int(rowsCols));
    result->identity();
    output%0 << *result;
  }
}
