defstar {
  name { MxCom_M }
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
Accept input matrices and creates a matrix output. Each input 
matrix represents a decomposed submatrix of output matrix in 
row by row. Note that for one output image, we will need totally 
(numRows/numRowsSubMx)*(numCols/numColsSubMx) input matrices. 
  }
  ccinclude { "Matrix.h" }
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
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
    desc {
Input matrix should have dimension numRowsSubMx*numColsSubMx.
    }
  }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
  }
  protected {
    int numHeight;  // number of submatrices in the height of output matrix
    int numWidth;   // number of submatrices in the width of output matrix
    int numBlocks;  // total number of submatrices in the output matrix
  }
  setup {
    numHeight = int(numRows)/int(numRowsSubMx);
    numWidth  = int(numCols)/int(numColsSubMx);
    numBlocks = numHeight*numWidth; 
/*
 * check if output matrix can't be properly decomposed into the desired
 * submatrix size.
 */
    if ( int(numRows) != numHeight*int(numRowsSubMx) || 
         int(numCols)  != numWidth*int(numColsSubMx) ) {
      Error::abortRun(*this,"The size of desired output matrix can't be"
                           ,"properly decomposed into the specified"
                           ,"submatrix size.");
      return;
    }
    input.setSDFParams(numBlocks, numBlocks-1);
  }
  go {
    Envelope pkt;
    FloatMatrix& matrix = *(new FloatMatrix);
    FloatMatrix& result = *(new FloatMatrix(int(numRows),int(numCols)));

    for (int h=0; h<numHeight; h++)
      for (int w=0; w<numWidth; w++) {
	(input%(numBlocks-1-(h*numWidth+w))).getMessage(pkt);
	matrix = *(const FloatMatrix*)pkt.myData();
	// check for "null" matrix inputs, caused by delays
	if (pkt.empty()) {
	  // input empty, just think it as a zero matrix
	  matrix = *(new FloatMatrix(int(numRowsSubMx),int(numColsSubMx)));
	  matrix = 0;
	}
	if ( matrix.numRows()*matrix.numCols() != 
             int(numRowsSubMx)*int(numColsSubMx) ) {
	  Error::message(*this,"Input matrix don't have the same number of "
			      ,"elements as the specified submatrix.");
	  return;
	} 

	for (int i=0; i<int(numRowsSubMx); i++)
	  for (int j=0; j<int(numColsSubMx); j++)
	    result[h*int(numRowsSubMx)+i][w*int(numColsSubMx)+j]=matrix[i][j];
      }
    output%0 << result;
  } // end of go()
}

