defstar {
  name { FloatToCx }
  domain { MDSDF }
  desc {
Takes an input FloatMatrix and converts it to an ComplexMatrix.  This
is done by using the cast conversion method of the FloatMatrix class.
The conversion results in a ComplexMatrix that has entries which have
real values that are the values of each corresponding entry of the 
FloatMatrix and 0 imaginary value.
I.e. ComplexMatrix.entry(i) = FloatMatrix.entry(i).
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { MDSDF library }
  input {
    name { input }
    type { FLOAT_MATRIX }
  }
  output {
    name { output }
    type { COMPLEX_MATRIX }
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
    FloatSubMatrix* data = (FloatSubMatrix*)(input.getInput());
    ComplexSubMatrix* result = (ComplexSubMatrix*)(output.getOutput());

    // do conversion using copy constructor
    for(int row = 0; row < int(numRows); row++)
      for(int col = 0; col < int(numCols); col++)
        (*result)[row][col] = (Complex)((*data)[row][col]);
 
    // delete i/o access structures
    delete data;
    delete result;
  }
}

