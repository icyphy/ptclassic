defstar {
  name      { UnPkInt_M }
  domain    { SDF }
  desc      { Read an integer matrix and output its elements, row by row. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
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
      for(int i = 0; i < size; i++)
        output%(size - i - 1) << matrix.entry(i);
    }
  }
}

