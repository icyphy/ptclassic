defstar {
  name { GainCx_M }
  domain { SDF }
  desc {
    Takes an input ComplexMatrix and multiplies it by a scalar gain value.
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
    type { COMPLEX_MATRIX_ENV }
  }
  defstate {
    name { gain }
    type { complex }
    default { "(1,0)" }
    desc { Gain to be multiplied with the input matrix }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const ComplexMatrix *matrix = (const ComplexMatrix *)inpkt.myData();

    // do scalar * matrix
    ComplexMatrix *result = new ComplexMatrix(matrix->numRows(),matrix->numCols());
    *result = Complex(gain) * *matrix;
    output%0 << *result;
  }
}

