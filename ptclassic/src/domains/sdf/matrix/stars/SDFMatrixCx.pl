defstar {
  name      { MatrixCx }
  domain    { SDF }
  desc      {
Produces a matrix with complex entries. The entries are
read from the array state ComplexMatrixContents in rasterized order:
i.e. for a MxN matrix, the first row is filled from left to right
using the first N values from the array. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  output {
	name { output }
	type { COMPLEX_MATRIX_ENV }
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
	name { ComplexMatrixContents }
	type { complexarray }
	default { "(1,0) (0,1) (-1,0) (0,-1)" }
	desc { The complex contents of the matrix. }
  }
  ccinclude { "Matrix.h" } 
  go {
    // collect inputs and put into the matrix
    ComplexMatrix *matrix = new ComplexMatrix(int(numRows),int(numCols),
                                              ComplexMatrixContents);
    output%0 << *matrix;
  }
}






