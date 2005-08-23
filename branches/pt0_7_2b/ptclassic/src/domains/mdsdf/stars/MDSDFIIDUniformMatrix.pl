defstar {
  name      { IIDUniformMatrix }
  domain    { MDSDF }
  desc      { 
Produces a FloatMatrix with IIDUniform entries.
This star uses the GNU library <Uniform.h>.
}
  version   { @(#)MDSDFIIDUniformMatrix.pl	1.4 12/1/95 }
  author    { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
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
	name { lower }
	type { float }
	default { "0.0" }
	desc { Lower limit. }
  }
  defstate {
	name { upper }
	type { float }
	default { "1.0" }
	desc { Upper limit. }
  }
  hinclude { <Uniform.h> }
  ccinclude { <ACG.h> }
  ccinclude { "SubMatrix.h" } 
  protected {
	Uniform *random;
  }
// declare the static random-number generator in the .cc file
  code {
	extern ACG* gen;
  }
  constructor {
	random = NULL;
  }
  destructor {
	LOG_DEL; delete random;
  }
  setup {
    // set the dimensions of the porthole
    output.setMDSDFParams(int(numRows), int(numCols));

    LOG_DEL; delete random;
    LOG_NEW; random = new Uniform(double(lower),double(upper),gen);
  }
  go {
    // get a SubMatrix from the buffer
    FloatSubMatrix *matrix;

    matrix = (FloatSubMatrix*)output.getOutput();

    // initialize the subMatrix with values from the FloatStateArray
    for(int i = 0; i < int(numRows)*int(numCols); i++)
      matrix->entry(i) = (*random)();
    delete matrix;
  }
}

