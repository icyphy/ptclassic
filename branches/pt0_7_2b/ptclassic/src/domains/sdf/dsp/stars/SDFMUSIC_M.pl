defstar {
  name      { MUSIC_M }
  domain    { SDF }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF dsp library }
  descriptor {
MUSIC - Multiple Signal Characterization algorithm, using the Matrix 
        message class for input

Applies the MUSIC algorithm on a given set of singular value, left
and right singular vectors of a data matrix X, where X is a data matrix

        +--                                   --+
        | x(M-1)      x(M)     ...     x(0)     |
        | x(M)        x(M-1)   ...     x(1)     |
    X = |   .                                   |
        |   .                                   |
        |   .                                   |
        | x(N-1)      x(N-2)   ...     x(N-M)   |
        +--                                   --+

  }
  input { 
    name { rsvec }
    type { FLOAT_MATRIX_ENV }
    desc { Right singular vectors. }
  }
  output {
    name { output }
    type { float }
    desc { S(w) eigenspectrum points. }
  }
  defstate {
    name { numRows }
    type { int }
    default { 4 }
    desc { The number of rows in the right singular matrix V. }
  }
  defstate {
    name { numCols }
    type { int }
    default { 4 }
    desc { The number of columns in the right singular matrix V. }
  }
  defstate {
    name { numSignals }
    type { int }
    default { 1 }
    desc { The number of unique signals we are trying to locate. }
  }
  defstate {
    name { resolution }
    type { int }
    default { 256 }
    desc { The number of points in the frequency domain. }
  }
  hinclude { <math.h>, "Matrix.h" }
  protected {
    ComplexMatrix *e;  // e(w) is the comple frequency search vector
    FloatMatrix   *Vn; // matrix of the right singular vectors of the noise
    FloatMatrix   *VnVnt;  // Matrix that is the product of Vn * transpose(Vn)
    int numNoise;    // Number of right singular vectors of the noise subspace
    int nrows;       // Number of rows in X
    int ncols;       // Number of columns in X
    ComplexMatrix *F;  // F(w) = eh(w)*VnVnt*e(w)
    double result;   // The final calculated value of the amplitude of S(w)
  }
  destructor {
    delete e;
    delete Vn;
    delete VnVnt;
    delete F;
  }
  setup {
    nrows = int(numRows);
    ncols = int(numCols);
    numNoise = ncols - 2*int(numSignals);
    e = new ComplexMatrix(nrows,1);
    Vn = new FloatMatrix(nrows,numNoise);
    VnVnt = new FloatMatrix(nrows,nrows);
    F = new ComplexMatrix(1,1);
    output.setSDFParams(int(resolution)+1);
  }
  go {
    // assumes that the singular values and the associated right singular
    // vectors have been sorted in descending order

    // load matrix of right singular vectors of the noise subspace
    Envelope inputPkt;
    (rsvec%0).getMessage(inputPkt);
    const FloatMatrix *V = (const FloatMatrix *)inputPkt.myData();
    for(int row = 0; row < nrows; row++)
      for(int col = 0; col < numNoise; col++)
        (*Vn)[row][col] = (*V)[row][col+(2*int(numSignals))];
      
    // compute VnVnt = Vn * transpose(Vn)
    *VnVnt = (*Vn) * (~(*Vn));

    double w = -(M_PI);         // set omega to scan from -PI to PI
    for(int count = 0; count <= int(resolution); count++) {
      // compute e(w), the frequency-searching vector, given an w
      //  and eh(w), the hermitian transpose of e(w)
      for(int row = 0; row < nrows; row++)
        (*e)[row][0] = Complex(cos(row*w),sin(row*w));

      // calculate the real part of F(w) = eh(w)*VnVnt*e(w)
      *F = (e->hermitian()) * ComplexMatrix(*VnVnt) * (*e);

      // calculate the amplitude of S(w)
      result = 1/abs((*F)[0][0]);
      output%(count) << result;
      w += 2*(M_PI)/int(resolution);
    }
  }
}



