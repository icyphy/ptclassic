defstar {
  name      { MatrixInt }
  domain    { SDF }
  desc      { 
Produces a matrix with fixed-point entries. The entries are
read from the array parameter "FixMatrixContents" in rasterized order:
i.e. for a MxN matrix, the first row is filled from left to right
using the first N values from the array.
  }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  output {
	name { output }
	type { INT_MATRIX_ENV }
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
	name { IntMatrixContents }
	type { intarray }
	default { "1 -2 2 -2" }
	desc { The integer contents of the matrix. }
  }
  ccinclude { "Matrix.h" } 
  go {
    // collect inputs and put into the matrix
    IntMatrix *matrix = new IntMatrix(int(numRows),int(numCols),
                                      IntMatrixContents);
    output%0 << *matrix;
  }
}

