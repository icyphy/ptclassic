defstar {
  name { CxToFloat_M }
  domain { SDF }
  desc {
Take an input ComplexMatrix and convert it to an FloatMatrix.  This
is done by using the cast conversion method of the ComplexMatrix class.
The conversion results in an FloatMatrix that has entries which are the
absolute value of each corresponding entry of the ComplexMatrix being converted,
i.e. FloatMatrix.entry(i) = abs(ComplexMatrix.entry(i)).
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { COMPLEX_MATRIX_ENV }
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
    const ComplexMatrix& matrix = *(const ComplexMatrix *)inpkt.myData();

    // do conversion using copy constructor
    FloatMatrix& result = *(new FloatMatrix(FloatMatrix(matrix)));
    output%0 << result;
  }
}

