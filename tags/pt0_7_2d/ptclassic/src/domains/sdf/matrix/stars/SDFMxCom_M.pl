defstar {
  name { MxCom_M }
  domain { SDF }
  version { @(#)SDFMxCom_M.pl	1.4 10/23/95 }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF matrix library }
  desc {
Accept input matrices and create a matrix output. Each input 
matrix represents a decomposed submatrix of output matrix in 
row by row. Note that for one output image, we will need a total
(numRows/numRowsSubMx)*(numCols/numColsSubMx) input matrices. 
  }
  ccinclude { "Matrix.h" }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
    desc { Input matrix should have dimension numRowsSubMx*numColsSubMx. }
  }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
  }
  defstate {
    name { numRows }
    type { int }
    default { 100 }
    desc { number of rows for output matrix }
  }
  defstate {
    name { numCols }
    type { int }
    default { 100 }
    desc { number of columns for output matrix }
  }
  defstate {
    name { numRowsSubMx }
    type { int }
    default { 4 }
    desc { number of rows for decomposed submatrix (ie, input matrix) }
  }
  defstate {
    name { numColsSubMx }
    type { int }
    default { 4 }
    desc { number of columns for decomposed submatrix (ie, input matrix) }
  }
  protected {
    int numHeight;  // number of submatrices in the height of output matrix
    int numWidth;   // number of submatrices in the width of output matrix
    int numBlocks;  // total number of submatrices in the output matrix
  }
  setup {
    numHeight = int(numRows)/int(numRowsSubMx);
    numWidth  = int(numCols)/int(numColsSubMx);
    numBlocks = numHeight * numWidth; 
    // check if output matrix can't be properly decomposed into the desired
    // submatrix size.
    if ( int(numRows) != numHeight*int(numRowsSubMx) || 
         int(numCols) != numWidth*int(numColsSubMx) ) {
      Error::abortRun(*this,
		      "The size of desired output matrix cannot be ",
                      "properly decomposed into the specified ",
                      "submatrix size.");
      return;
    }
    input.setSDFParams(numBlocks, numBlocks-1);
  }
  go {
    Envelope pkt;
    FloatMatrix& result = *(new FloatMatrix(int(numRows), int(numCols)));
    int numsubrows = int(numRowsSubMx);
    int numsubcols = int(numColsSubMx);

    for (int h = 0; h < numHeight; h++) {
      for (int w = 0; w < numWidth; w++) {

	// treat "null" matrix inputs (caused by delays) as zero matrices
	(input%(numBlocks-1-(h*numWidth+w))).getMessage(pkt);
	if ( pkt.empty() ) {
	  for (int i = 0; i < numsubrows; i++)
	    for (int j = 0; j < numsubcols; j++)
	      result[h*numsubrows + i][w*numsubcols + j] = 0.0;
	}
	else {
	  const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();
	  if ( matrix.numRows() * matrix.numCols() !=
	       numsubrows * numsubcols ) {
	    Error::message(*this,
			   "Input matrix does not have the same number of ",
			   "elements as the specified submatrix.");
	    return;
	  } 
	  // FIXME: matrix[i][j] is not the right way to index because
	  // the above "if" tests the total number of matrix elements
	  else {
	    for (int i = 0; i < numsubrows; i++)
	      for (int j = 0; j < numsubcols; j++)
	        result[h*numsubrows + i][w*numsubcols + j] = matrix[i][j];
	  }
	}
      }
    }
    output%0 << result;
  } // end of go()
}
