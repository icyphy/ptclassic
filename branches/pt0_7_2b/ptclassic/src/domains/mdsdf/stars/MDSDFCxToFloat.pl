defstar {
  name { CxToFloat }
  domain { MDSDF }
  desc {
Takes an input ComplexSubMatrix and converts it to a FloatSubMatrix.  This
is done by using the cast conversion method of the FloatMatrix class.
The conversion results in a ComplexMatrix that has entries which have
real values that are the values of each corresponding entry of the 
FloatMatrix and 0 imaginary value.
I.e. ComplexMatrix.entry(i) = FloatMatrix.entry(i).
  }
  version { @(#)MDSDFCxToFloat.pl	1.4 12/1/95 }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { MDSDF library }
  input {
    name { input }
    type { COMPLEX_MATRIX }
  }
  output {
    name { output }
    type { FLOAT_MATRIX }
  }
  defstate {
    name { numRows }
    type { int }
    default { 2 }
    desc { The number of rows in the matrix being converted. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { The number of columns in the matrix being converted. }
  }
  ccinclude { "SubMatrix.h" }
  setup {
    input.setMDSDFParams(int(numRows),int(numCols));
    output.setMDSDFParams(int(numRows),int(numCols));
  }
  go {
    // get input & output structures
    ComplexSubMatrix* data = (ComplexSubMatrix*)(input.getInput());
    FloatSubMatrix* result = (FloatSubMatrix*)(output.getOutput());

    // do conversion using copy constructor
    for(int row = 0; row < int(numRows); row++)
      for(int col = 0; col < int(numCols); col++)
        (*result)[row][col] = abs((*data)[row][col]);
 
    // delete i/o access structures
    delete data;
    delete result;
  }
}

