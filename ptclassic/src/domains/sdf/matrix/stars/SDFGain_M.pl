defstar {
  name { Gain_M }
  domain { SDF }
  desc {
    Takes an input FloatMatrix and multiplies it by a scalar gain value.
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
    type { FLOAT_MATRIX_ENV }
  }
  defstate {
    name { gain }
    type { float }
    default { "1.0" }
    desc { Gain to be multiplied with the input matrix }
  }
  ccinclude { "Matrix.h" }
  go {
    // get input
    Envelope inpkt;
    (input%0).getMessage(inpkt);
    const FloatMatrix *matrix = (const FloatMatrix *)inpkt.myData();

    // do scalar * matrix
    FloatMatrix *result = new FloatMatrix(matrix->numRows(),matrix->numCols());
    *result = double(gain) * *matrix;
    output%0 << *result;
  }
}

