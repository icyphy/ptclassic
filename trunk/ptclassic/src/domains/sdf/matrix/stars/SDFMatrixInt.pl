defstar {
  name      { MatrixInt }
  domain    { SDF }
  desc      { 
Produces a matrix with integer entries. The entries are
read from the array parameter "IntMatrixContents" in rasterized order:
i.e. for a MxN matrix, the first row is filled from left to right
using the first N values from the array.
  }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
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
    IntMatrix& matrix = *(new IntMatrix(int(numRows),int(numCols),
                                        IntMatrixContents));
    output%0 << matrix;
  }
}

