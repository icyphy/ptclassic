defstar {
  name { FloatToInt_M }
  domain { SDF }
  desc {
    Takes an input FloatMatrix and converts it to an IntMatrix.  This
    is done by using the cast conversion method of the FloatMatrix class.
    The conversion results in a IntMatrix that has entries which are
    the int of each corresponding entry of the FloatMatrix.
    I.e. IntMatrix.entry(i) = int(FloatMatrix.entry(i))
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
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
    const FloatMatrix& matrix = *(const FloatMatrix *)inpkt.myData();

    // do conversion using copy constructor
    IntMatrix& result = *(new IntMatrix(IntMatrix(matrix)));
    output%0 << result;
  }
}

