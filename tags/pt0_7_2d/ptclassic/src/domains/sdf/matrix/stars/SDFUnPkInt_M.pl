defstar {
  name      { UnPkInt_M }
  domain    { SDF }
  desc      { Read an integer matrix and output its elements, row by row. }
  version   { @(#)SDFUnPkInt_M.pl	1.8 10/23/98 }
  author    { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
	name { input }
	type { INT_MATRIX_ENV }
  }
  output {
	name { output }
	type { int }
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
	name { addStopSymbol }
	type { int }
	default { NO }
	desc { Whether to add a stop symbol at end of output tokens or not. }
  }
  defstate {
	name { stopSymbol }
	type { int }
	default { "-1" }
	desc { The stop symbol to be added at end of output tokens. }
  }
  ccinclude { "Matrix.h" } 
  protected {
    int size;
  }
  setup {
    size = int(numRows)*int(numCols);
    if (!int(addStopSymbol)) {
      output.setSDFParams(size);
    } else {
      output.setSDFParams(size+1);
    }
  }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const IntMatrix& matrix = *(const IntMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // input empty, just send a stream of (numRows*numCols) zeros
      for(int i = 0; i < int(numRows)*int(numCols); i++)
        output%i << 0;
    }
    else {
      // valid input matrix

      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
          Error::abortRun(*this,"Dimension size of IntMatrix received does ",
                                "not match the given state parameters.");
          return;
      }
      if (!int(addStopSymbol)) {
	for(int i = 0; i < size; i++)
	  output%(size - i - 1) << matrix.entry(i);
      } else {
	for(int i = 0; i < size; i++)
	  output%(size - i) << matrix.entry(i);
	output%0 << int(stopSymbol);
      }
    }
  }
}
