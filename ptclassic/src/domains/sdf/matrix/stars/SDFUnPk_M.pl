defstar {
  name      { UnPk_M }
  domain    { SDF }
  desc      { Read a floating-point matrix and output its elements, row by row. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
	name { input }
	type { FLOAT_MATRIX_ENV }
  }
  output {
	name { output }
	type { float }
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
    const FloatMatrix& matrix = *(const FloatMatrix *)pkt.myData();
    if((matrix.numRows() != int(numRows)) ||
       (matrix.numCols() != int(numCols))) {
      Error::abortRun(*this,"Dimension size of FloatMatrix received does ",
                            "not match the given state parameters.");
      return;
    }
    for(int i = 0; i < size; i++)
      output%(size - i - 1) << matrix.entry(i);
  }
}

