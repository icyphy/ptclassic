defstar {
  name { FixToCx_M }
  domain { SDF }
  desc {
Take an input FixMatrix and convert it to an ComplexMatrix.  This
is done by using the cast conversion method of the FixMatrix class.
The conversion results in a ComplexMatrix that has entries which have
real values which are the double-precision representation of each
corresponding entry of the FixMatrix, with 0 imaginary values.
I.e. ComplexMatrix.entry(i) = double(FixMatrix.entry(i)).
  }
  version { $Id$ }
  author { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF matrix library }
  input {
    name { input }
    type { FIX_MATRIX_ENV }
  }
  output {
    name { output }
    type { COMPLEX_MATRIX_ENV }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const FixMatrix& matrix = *(const FixMatrix *)inpkt.myData();

    // do conversion using copy constructor
    ComplexMatrix& result = *(new ComplexMatrix(ComplexMatrix(matrix)));
    output%0 << result;
  }
}

