defstar {
  name      { PackFix_M }
  domain    { SDF }
  desc      { Takes Fix inputs and produces FixMatrix messages. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
	name { input }
	type { Fix }
  }
  output {
	name { output }
	type { FIX_MATRIX_ENV }
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
    FixMatrix *matrix = new FixMatrix(int(numRows),int(numCols),input);
    Envelope pkt(*matrix);
    output%0 << pkt;
  }
}

