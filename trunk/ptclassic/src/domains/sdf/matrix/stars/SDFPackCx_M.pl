defstar {
  name      { PackCx_M }
  domain    { SDF }
  desc      { Takes complex inputs and produces ComplexMatrix messages. }
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
    ComplexMatrix *matrix = new ComplexMatrix(int(numRows),int(numCols),input);
    output%0 << *matrix;
  }
}

