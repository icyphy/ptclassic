defstar {
  name      { Pack_M }
  domain    { SDF }
  desc      { 
Produce a matrix with floating-point entries constructed from
floating-point input values.  The inputs are put in the matrix
in rasterized order, e.g. for a MxN matrix, the first row is 
filled from left to right using the first N input values.
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
  input {
	name { input }
	type { float }
  }
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
	desc { The number of colums in the matrix. }
  }
  ccinclude { "Matrix.h" } 
  setup {
    input.setSDFParams(int(numRows)*int(numCols));
  }
  go {
    // collect inputs and put into the matrix
    FloatMatrix& matrix = *(new FloatMatrix(int(numRows),int(numCols),input));
    output%0 << matrix;
  }
}

