defstar {
  name { SGVQCoder }
  domain { SDF }
  desc {  
Shape-gain vector quantization encoder.
Note that each input matrix will be viewed as a row vector in row 
by row. 
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF dsp library }
  defstate {
    name { numRows }
    type { int }
    default { 1 }
    desc { The number of rows for input matrix. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 2 }
    desc { The number of columns for input matrix. }
  }
  defstate {
    name { shapeCodebook }
    type { floatarray }
    default { "0.6 0.8   0.6 -0.8   -0.6 0.8   -0.6 -0.8" }
    desc {
The float codewords for the shapeCodebook. Each codeword with length numRows*numCols.
    }
  }
  defstate {
    name { gainCodebook }
    type { floatarray }
    default { "0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0" }
    desc {
The float codewords for the gainCodebook. Each codeword with length 1.
    }
  }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
    desc{ 
Input matrix will be viewed as a row vector in row major ordering to
represent a training vector.
    }
  }
  output {
    name { outShape }
    type { int }
    desc{ Output index in the shapeCodebook }
  }
  output {
    name { outGain }
    type { int }
    desc{ Output index in the gainCodebook }
  }
  ccinclude { "Matrix.h" }
  protected {
    int dimension;
    int sizeShapeCodebook;
    int sizeGainCodebook;
  }
  setup {
    int size = shapeCodebook.size();
    dimension = int(numRows)*int(numCols);
//  check if codebook doesn't have enough elements.
    if ( size != size/dimension*dimension ) {
        Error::abortRun(*this,"The number of elements in state parameter ",
                              "'shapeCodebook' does't match the specified ",
                              "dimension.");
        return;
    }
    sizeShapeCodebook = size/dimension;
    sizeGainCodebook = gainCodebook.size();
  }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const FloatMatrix& matrix = *(const FloatMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // input empty, send out zero indexes
      outShape%0 << int(0);
      outGain%0  << int(0);
    } else {
      // valid input matrix
      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Input matrix does't match the specified ",
                              "dimension");
	return;
      }

// Each input matrix will be viewed as a row vector in row major ordering.
// If X=input vector, and Si=i_th shape codeword.
// First find the shape codeword Si to maximize X'*Si ( ' means transpose ),
      int indexShape = 0;
      double shapeDistance = 0;
      for ( int j=0; j<dimension; j++ )
	shapeDistance += matrix.entry(j)*shapeCodebook[j];

      double sum = 0;
      for ( int i = 1; i<sizeShapeCodebook; i++ ) {
	for ( j=0; j<dimension; j++ )
	  sum += matrix.entry(j)*shapeCodebook[i*dimension+j];
	if ( sum > shapeDistance ) {
	  shapeDistance = sum;
	  indexShape = i;
	}
	sum = 0;
      }
// now shapeDistance store the maximun X'*Si

// Then find the j_th gain codeword gj to minimize (gj-X'*Si)^2
      int indexGain = 0;
      double distance = 0;
      distance = gainCodebook[0]-shapeDistance;
      distance *= distance;

      for (i=1; i<sizeGainCodebook; i++) {
	sum = gainCodebook[i]-shapeDistance;
	sum *= sum;
        if ( sum < distance ) {
          distance = sum;
          indexGain = i;
        }
      }
// now distance store the minimun (gj-X'*Si)^2

      outShape%0 << indexShape;
      outGain%0  << indexGain;
    }    // end else
  } 	 // end of go method
}
