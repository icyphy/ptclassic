defstar {
  name      { FloatMatrix }
  domain    { MDSDF }
  desc      { Produces FloatMatrix data from state information. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { MDSDF library }
  output {
	name { output }
	type { FLOAT_MATRIX }
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
  ccinclude { "SubMatrix.h" } 
  setup {
    // set the dimensions of the porthole
    output.setMDSDFParams(int(numRows), int(numCols));
  }
  go {
    // get a SubMatrix from the buffer
    FloatSubMatrix *matrix = (FloatSubMatrix*)output.getOutput();

    // initialize the subMatrix with values from the FloatStateArray
    *matrix << FloatMatrixContents;     

    delete matrix;
  }
}

