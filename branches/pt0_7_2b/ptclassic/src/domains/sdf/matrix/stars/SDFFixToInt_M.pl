defstar {
  name { FixToInt_M }
  domain { SDF }
  desc {
    Takes an input FixMatrix and converts it to an IntMatrix.  This
    is done by using the cast conversion method of the FixMatrix class.
    The conversion results in a IntMatrix that has entries which are
    the int of each corresponding entry of the FixMatrix.
    I.e. IntMatrix.entry(i) = int(FixMatrix.entry(i))
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
    type { INT_MATRIX_ENV }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const FixMatrix& matrix = *(const FixMatrix *)inpkt.myData();

    // do conversion using copy constructor
    IntMatrix& result = *(new IntMatrix(IntMatrix(matrix)));
    output%0 << result;
  }
}
