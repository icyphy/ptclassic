defstar {
  name      { PackCx_M }
  domain    { SDF }
  desc      { 
Produces a matrix with complex value entries. The entries are
read in from values in a sequence of complex valued particles
in rasterized order, e.g. for a MxN matrix, the first row is 
filled from left to right using the first N values from the 
first N particles.
  }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
	name { input }
	type { complex }
  }
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
  ccinclude { "Matrix.h" } 
  setup {
    input.setSDFParams(int(numRows)*int(numCols));
  }
  go {
    // collect inputs and put into the matrix
    ComplexMatrix& matrix = *(new ComplexMatrix(int(numRows),int(numCols),input));
    output%0 << matrix;
  }
}

