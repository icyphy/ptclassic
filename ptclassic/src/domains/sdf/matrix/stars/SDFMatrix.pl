defstar {
  name      { Matrix }
  domain    { SDF }
  desc      { 
Produces a matrix with floating-point entries. The entries are
read from the array state FloatMatrixContents in rasterized order:
i.e. for a MxN matrix, the first row is filled from left to right
using the first N values from the array. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  output {
	name { output }
	type { FLOAT_MATRIX_ENV }
  }
  defstate {
	name { numRows }
	type { int }
	default { 2 }
	desc { The number of rows in the matrix. }
  }
  defstate {
	name { numCols }
	type { int }
	default { 2 }
	desc { The number of columns in the matrix. }
  }
  defstate {
	name { FloatMatrixContents }
	type { floatarray }
	default { "1.0 -2.0 2.0 -2.0" }
	desc { The float contents of the matrix. }
  }
  ccinclude { "Matrix.h" } 
  go {
    // collect inputs and put into the matrix
    FloatMatrix *matrix = new FloatMatrix(int(numRows),int(numCols),
                                          FloatMatrixContents);
    output%0 << *matrix;
  }
}

