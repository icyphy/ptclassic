defstar {
  name      { PackInt_M }
  domain    { SDF }
  desc      { Takes integer inputs and produces IntMatrix messages. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
	name { input }
	type { int }
  }
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
  ccinclude { "Matrix.h" } 
  setup {
    input.setSDFParams(int(numRows)*int(numCols));
  }
  go {
    // collect inputs and put into the matrix
    IntMatrix *matrix = new IntMatrix(int(numRows),int(numCols),input);
    Envelope pkt(*matrix);
    output%0 << pkt;
  }
}

