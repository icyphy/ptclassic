defstar {
  name { IntToFloat_M }
  domain { SDF }
  desc {
Take an input IntMatrix and convert it to an FloatMatrix.  This
is done by using the cast conversion method of the IntMatrix class.
The conversion results in an FloatMatrix that has entries which are the
double of each corresponding entry of the IntMatrix being converted.  
I.e. FloatMatrix.entry(i) = double(IntMatrix.entry(i))
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { INT_MATRIX_ENV }
  }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const IntMatrix& matrix = *(const IntMatrix *)inpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(inpkt.empty()) {
      // input empty, just send it back out
      output%0 << inpkt;
    }
    else {
      // valid input matrix

      // do conversion using copy constructor
      FloatMatrix& result = *(new FloatMatrix(FloatMatrix(matrix)));
      output%0 << result;
    }
  }
}

