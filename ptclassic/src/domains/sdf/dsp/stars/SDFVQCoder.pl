defstar {
  name { VQCoder }
  domain { SDF }
  desc {  
Full search vector quantization encoder, ie, find the index of the nearest 
neighbor in the given codebook corresponding to the input matrix.
Note that each input matrix will first be viewed as a row vector in row by
row, in order to find the nearest neighbor codeword in the codebook.
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
    name { floatCodebook }
    type { floatarray }
    default { "1.0 1.0   1.0 -1.0   -1.0 1.0   -1.0 -1.0" }
    desc { 
The float codewords for the codebook. Each codeword with length numRows*numCols.
    }
  }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
    desc{ Input matrix. }
  }
  output {
    name { output }
    type { int }
    desc{
The index of the nearest neighbor in the codebook corresponding to the
input matrix.
    }
  }
  ccinclude { "Matrix.h" }
  protected {
    int dimension;
    int numCodewords;
    double *halfCodewordPower;
  }
  constructor {
    halfCodewordPower = 0;
  }
  destructor {
    LOG_DEL; delete [] halfCodewordPower;
  }
  setup {
    int size = floatCodebook.size();
    dimension = int(numRows)*int(numCols);
//  check if codebook doesn't have enough elements.
    if ( size != size/dimension*dimension ) {
	Error::abortRun(*this,"The number of elements in state parameter ",
			      "'floatCodebook' does't match the specified ",
			      "dimension.");
	return;
    }
    numCodewords = size/dimension;
    LOG_DEL; delete [] halfCodewordPower;
    LOG_NEW; halfCodewordPower = new double[numCodewords];
/* 
 * Each input matrix will first be viewed as the row vector in row major
 * ordering for manipulation.
 * Let X=input vector, and Yi=i_th codeword.
 * Find the nearest neighbor is to find the Yi to minimize ||X-Yi||^2
 * ( ||.|| means two norm ), and is equivalent to find the Yi to maximize 
 * X'*Yi-Ai ( ' means transpose ) ,where Ai=||Yi||^2/2.
 * We can precompute the values of Ai=||Yi||^2/2 and store them in the 
 * array halfCodewordPower[numCodewords].
 */

    double sum = 0;
    for ( int n=0; n<numCodewords; n++ ) {
	for ( int i=0; i<dimension; i++ )  
	    sum += floatCodebook[n*dimension+i]*floatCodebook[n*dimension+i];
	halfCodewordPower[n]=sum/2;
	sum = 0;
    }
  }
  go {
    Envelope pkt;
    (input%0).getMessage(pkt);
    const FloatMatrix& matrix = *(const FloatMatrix *)pkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(pkt.empty()) {
      // input empty, send out a zero index
      output%0 << int(0);
    }
    else {
      // valid input matrix
      if((matrix.numRows() != int(numRows)) ||
         (matrix.numCols() != int(numCols))) {
        Error::abortRun(*this,"Input matrix does't match the specified ",
			      "dimension");
        return;
      }
/*
 * Find the nearest neighbor codeword Yi to maximize X'*Yi-Ai 
 * ( ' means transpose ), where Ai=||Yi||^2/2 and have already 
 * been stored in the array halfCodewordPower[numCodewords].
 */
      int NNIndex = 0;
      double distance = 0;
      for ( int i=0; i<dimension; i++ )
	  distance += matrix.entry(i)*floatCodebook[i];
      distance -= halfCodewordPower[0];

      double sum = 0;
      for ( int n = 1; n<numCodewords; n++ ) {
	  for ( i=0; i<dimension; i++ )
		sum += matrix.entry(i)*floatCodebook[n*dimension+i];
	  sum -= halfCodewordPower[n];
	  if ( sum > distance ) {
		distance = sum;
		NNIndex = n;
	  }
	  sum = 0;
      }
      output%0 << NNIndex;
    }	// end of else
  }	// end of go method
}
