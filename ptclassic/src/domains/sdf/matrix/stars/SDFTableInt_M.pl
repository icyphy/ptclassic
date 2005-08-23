defstar {
  name { TableInt_M }
  domain { SDF }
  desc {
This star implements a lookup table indexed by an integer-valued input,
and the output will be a matrix.
The input must lie between 0 and N-1, inclusive, where N is the number
of matrices in the table.
The "intTable" parameter specifies the entries of matrices in the table.
Note that the entries of each matrix in the table should be given in row 
major ordering.
The first matrix in the table is indexed by a zero-valued input.
An error occurs if the input value is out of bounds.
  }
  version { @(#)SDFTableInt_M.pl	1.4 01 Oct 1996 }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF matrix library }
	htmldoc {
<a name="table lookup"></a>
  }
  input {
    name { input }
    type { int }
    desc { The index for table lookup }
  }
  output {
    name { output }
    type { INT_MATRIX_ENV }
    desc { 
The matrix in the table corresponding to the index.
    }
  }
  defstate {
    name { numRows }
    type { int }
    default { 1 }
    desc { The number of rows for each matrix in the table. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { The number of columns for each matrix in the table. }
  }
  defstate {
    name { intTable }
    type { intarray }
    default { "1 1   1 -1   -1 1   -1 -1" }
    desc { 
The table containing matrices. Each matrix with dimension numRows*numCols is given in row major ordering.
    }
  }
  ccinclude { "Matrix.h" }
  protected {
    int dimension;
    int numMatrices;
  }
  setup {
    int size = intTable.size();
    dimension = int(numRows)*int(numCols);
//  check if table doesn't have enough elements.
    if ( size != size/dimension*dimension ) {
      Error::abortRun(*this,"The number of elements in state parameter ",
                            "'intTable' doesn't match the specified ",
                            "dimension.");
      return;
    }
    numMatrices = size/dimension;
  }
  go {
    int index = int(input%0);
    if ( index < 0 || index >= numMatrices )
      Error::abortRun(*this, ": out of bounds input value");
    else {
      IntMatrix& result = *(new IntMatrix(int(numRows),int(numCols)));
      for ( int i=0; i<dimension; i++)
        result.entry(i) = intTable[index*dimension+i];
      output%0 << result;
    }
  }
}
