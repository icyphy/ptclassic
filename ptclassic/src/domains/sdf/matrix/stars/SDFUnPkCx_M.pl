defstar {
  name      { UnPkCx_M }
  domain    { SDF }
  desc      { Read a complex matrix and output its elements, row by row. }
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
	type { COMPLEX_MATRIX_ENV }
  }
  output {
	name { output }
	type { complex }
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
  protected {
    int size;
  }
  setup {
    size = int(numRows)*int(numCols);
    output.setSDFParams(size);
  }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const ComplexMatrix& matrix = *(const ComplexMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // input empty, just send a stream of (numRows*numCols) zeros
      for(int i = 0; i < int(numRows)*int(numCols); i++)
        output%i << Complex(0,0);
    }
    else {
      // valid input matrix

      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
          Error::abortRun(*this,"Dimension of ComplexMatrix received does ",
                                "not match the given state parameters.");
          return;
      }
      for(int i = 0; i < size; i++)
        output%(size - i - 1) << matrix.entry(i);
    }
  }
}

