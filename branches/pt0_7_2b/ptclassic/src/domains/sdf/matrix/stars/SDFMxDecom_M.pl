defstar {
  name { MxDecom_M }
  domain { SDF }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF matrix library }
  desc {
Decompose a portion of input matrix into a sequence of submatrices.
The desired portion of input matrix is specified by state parameters 
"startRow","startCol","numRows", and "numCols".
Then output each submatrix with dimension "numRowsSubMx"*"numColsSubMx"
in row by row.
Note that for one input matrix, there will be totally (numRows/numRowsSubMx)
*(numCols/numColsSubMx) output matrices.
  }
  ccinclude { "Matrix.h" }
  defstate {
    name { startRow }
    type { int }
    default { 0 }
    desc { The starting row in the input matrix }
  }
  defstate {
    name { startCol }
    type { int }
    default { 0 }
    desc { The starting column in the input matrix }
  }
  defstate {
    name { numRows }
    type { int }
    default { 100 }
    desc { number of rows from input matrix }
  }
  defstate {
    name { numCols }
    type { int }
    default { 100 }
    desc { number of columns from input matrix }
  }
  defstate {
    name { numRowsSubMx }
    type { int }
    default { 4 }
    desc { number of rows for decomposed submatrix (ie, output matrix) }
  }
  defstate {
    name { numColsSubMx }
    type { int }
    default { 4 }
    desc { number of columns for decomposed submatrix (ie, output matrix) }
  }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV  }
  }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
    desc {
Output matrix with numRowsSubMx*numColsSubMx dimension.
    }
  }
  protected {
    int numHeight;  // number of submatrices in the height of input matrix
    int numWidth;   // number of submatrices in the width of input matrix
    int numBlocks;  // total number of submatrices in the input matrix
  }
  setup {
    numHeight = int(numRows)/int(numRowsSubMx);
    numWidth  = int(numCols)/int(numColsSubMx);
    numBlocks = numHeight*numWidth; 
/*
 * check if input matrix can't be properly decomposed into the desired 
 * submatrix size.
 */
    if ( int(numRows) != numHeight*int(numRowsSubMx) || 
         int(numCols)  != numWidth*int(numColsSubMx) ) {
      Error::abortRun(*this,"The size of the desired portion of input "
			   ,"matrix can't be properly decomposed into the "
			   ,"specified submatrix size.");
      return;
    }
    output.setSDFParams(numBlocks, numBlocks-1);
  }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();
    if ( int(startRow)+int(numRows) > matrix.numRows() ) {
      Error::abortRun(*this,"The desired portion of input matrix extend"
                           ,"beyond the rows of the input matrix.");
      return;
    }
    if ( int(startCol)+int(numCols) > matrix.numCols() ) {
      Error::abortRun(*this,"The desired portion of input matrix extend"
                           ,"beyond the columns of the input matrix.");
      return;
    }
    for (int h=0; h<numHeight; h++) 
      for (int w=0; w<numWidth; w++) {
        FloatMatrix& result = *(new FloatMatrix(int(numRowsSubMx),int(numColsSubMx)));
        for (int i=0; i<int(numRowsSubMx); i++)
          for (int j=0; j<int(numColsSubMx); j++)
	    result[i][j] = matrix[int(startRow)+h*int(numRowsSubMx)+i][int(startCol)+w*int(numColsSubMx)+j];
	    output%(numBlocks-1-(h*numWidth+w)) << result;
      }
  } // end of go()
}



