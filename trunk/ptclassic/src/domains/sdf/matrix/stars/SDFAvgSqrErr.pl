defstar {
  name { AvgSqrErr }
  domain { SDF }
  desc { 
Find the average squared error between two input sequences of matrix. 
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input1 }
    type { FLOAT_MATRIX_ENV }
  }
  input {
    name { input2 }
    type { FLOAT_MATRIX_ENV }
  }
  output {
    name { output }
    type { float }
  }
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
    name { numInputs }
    type { int }
    default { 8 }
    desc { The number of input matrices to average. }
  }
  ccinclude { "Matrix.h" }
  setup {
    input1.setSDFParams(int(numInputs),int(numInputs)-1);
    input2.setSDFParams(int(numInputs),int(numInputs)-1);
  }
  go {
    Envelope inpkt1;
    FloatMatrix& matrix1 = *(new FloatMatrix);
    Envelope inpkt2;
    FloatMatrix& matrix2 = *(new FloatMatrix);

    FloatMatrix& result  = *(new FloatMatrix(int(numRows),int(numCols)));
    double sqrErr = 0;
    for (int i=0; i<int(numInputs); i++) {
      // get input
      (input1%(int(numInputs)-1-i)).getMessage(inpkt1);
      matrix1 = *(const FloatMatrix *)inpkt1.myData();
      (input2%(int(numInputs)-1-i)).getMessage(inpkt2);
      matrix2 = *(const FloatMatrix *)inpkt2.myData();

      // check for "null" matrix inputs, caused by delays
      if(inpkt1.empty()) {
        // input1 empty, just think it as a zero matrix
	result  = 0;
        matrix1 = result;
      } 
      if (inpkt2.empty()) {
        // input2 empty, just think it as a zero matrix
	result  = 0;
        matrix2 = result;
      }  

      // valid input matrix
      if((matrix1.numRows() != int(numRows)) ||
         (matrix1.numCols() != int(numCols))) {
        Error::abortRun(*this,"Input1 matrix does't match the specified ",
                              "dimension");
        return;
      } else if ((matrix2.numRows() != int(numRows)) ||
                 (matrix2.numCols() != int(numCols))) {
        Error::abortRun(*this,"Input2 matrix does't match the specified ",
                              "dimension");
        return;
      }

      result = matrix1 - matrix2;
      for (int i=0; i<int(numRows)*int(numCols); i++) 
	sqrErr += result.entry(i)*result.entry(i);
    }  // end of for
    sqrErr /= int(numInputs);
    output%0 << sqrErr;
  }    // end of go method
}

