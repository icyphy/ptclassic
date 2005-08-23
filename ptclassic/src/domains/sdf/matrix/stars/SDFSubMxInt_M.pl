defstar {
	name { SubMxInt_M }
	domain { SDF }
	version { @(#)SDFSubMxInt_M.pl	1.2 3/2/95 }
	author { Bilung Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF matrix library }
	desc { Find a submatrix of the input matrix. }
	ccinclude { "Matrix.h" }
        defstate {
                name { startRow }
                type { int }
                default { 0 }
                desc { Starting row of the input matrix }
        }
        defstate {
                name { startCol }
                type { int }
                default { 0 }
                desc { Starting column of the input matrix }
        }
        defstate {
                name { numRows }
                type { int }
                default { 1 }
                desc { The number of rows for the desired submatrix } 
        }
        defstate {
                name { numCols }
                type { int }
                default { 1 }
                desc { The number of columns for the desired submatrix } 
        }
	input {
		name { input }
		type { INT_MATRIX_ENV }
	}
	output {
		name { output }
		type { INT_MATRIX_ENV }
	}
	go {
                Envelope pkt;
		(input%0).getMessage(pkt);
		const IntMatrix& matrix = *(const IntMatrix*)pkt.myData();

                if ( int(startRow)+int(numRows) > matrix.numRows() ) {
                  Error::abortRun(*this,"Submatrix rows extend beyond "
					,"the rows of input matrix.");
                  return;
                }
                if ( int(startCol)+int(numCols) > matrix.numCols() ) {
                  Error::abortRun(*this,"Submatrix columns extend beyond "
                                        ,"the columns of input matrix.");
                  return;
                }

		// check for "null" matrix inputs, caused by delays
		if (pkt.empty()) {
		  // input empty, send out a zero matrix 
		  IntMatrix& result = *(new IntMatrix(int(numRows),int(numCols)));
		  result = 0;
		  output%0 << result;
    		}
		else {
		  IntMatrix& result = *(new IntMatrix(int(numRows),int(numCols)));
		  for (int i=0; i<int(numRows);i++)
		    for (int j=0; j<int(numCols);j++)
                      result[i][j] = matrix[int(startRow)+i][int(startCol)+j];
		  output%0 << result;
		}
	} // end of go()
}
