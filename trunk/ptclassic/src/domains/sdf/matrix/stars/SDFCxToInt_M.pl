defstar {
  name { CxToInt_M }
  domain { SDF }
  desc {
Take an input ComplexMatrix and convert it to an IntMatrix.  This
is done by using the cast conversion method of the ComplexMatrix class.
The conversion results in an IntMatrix that has entries which are the
int of the absolute value of each corresponding entry of the 
ComplexMatrix being converted.  
I.e. IntMatrix.entry(i) = (int)abs(ComplexMatrix.entry(i))
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF matrix library }
  input {
    name { input }
    type { COMPLEX_MATRIX_ENV }
  }
  output {
    name { output }
    type { INT_MATRIX_ENV }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const ComplexMatrix& matrix = *(const ComplexMatrix *)inpkt.myData();

    // check for "null" matrix inputs, caused by delays
    if(inpkt.empty()) {
      // input empty, just send it back out
      output%0 << inpkt;
    }
    else {
      // valid input matrix

      // do conversion using copy constructor
      IntMatrix& result = *(new IntMatrix(IntMatrix(matrix)));
      output%0 << result;
    }
  }
}

