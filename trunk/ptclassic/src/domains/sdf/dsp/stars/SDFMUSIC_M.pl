defstar {
  name      { MUSIC_M }
  domain    { SDF }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright { 1993 The Regents of the University of California }
  location  { SDF dsp library }
  descriptor {
This star is used to estimate the frequencies of some specified number
of sinusoids in a signal. 
The output is the eigenspectrum of a signal such that the locations of the 
peaks of the eigenspectrum correspond to the frequencies of sinusoids
in the signal.
The input is the right singular vectors in the form generated by the
SVD_M star.
The MUSIC algorithm is used, where MUSIC stands for
"multiple signal classification."
  }
  explanation {
The MUSIC algorithm is a general-purpose algorithm for estimating the
presence of sinusoids buried in white noise [1-2].
MUSIC provides high-resolution spectral estimates [3-4] which
can be used in source localization in spatial array processing [4-5].
Several derivatives of MUSIC exist such as root-MUSIC [6].
.Id "Dudgeon, D. E."
.Id "Friedlander, B."
.Id "Haykin, S."
.Id "Johnson, D. H."
.Id "Nehorai, A."
.Id "Stoica, P."
.Id "Soderstrom, B."
.Id "Weiss, A. J."
.UH REFERENCES
.ip [1]
S. Haykin, ed., \fIAdvances in Spectrum Analysis and Array Processing\fR,
vol. 2, Prentice-Hall: Englewood Cliffs, NJ, 1991.
.ip [2]
S. Haykin, \fIAdaptive Filter Theory\fR,
Prentice-Hall: Englewood Cliffs, NJ.  1991.  2nd ed.
.ip [3]
P. Stoica and A. Nehorai, ``MUSIC, Maximum Likelihood, and Cramer-Rao Bound:
Further Results and Comparisons,''
\fIIEEE Trans. on Acoustics, Speech, and Signal Processing\fR,
vol. 38, pp. 2140-2150, Dec. 1990.
.ip [4]
D. H. Johnson and D. E. Dudgeon, \fIArray Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ.  1993.
.ip [5]
P. Stoica and B. Soderstrom, ``Statistical Analysis of MUSIC and
Subspace Rotation Estimates of Sinusoidal Frequencies,''
\fIIEEE Trans. on Acoustics, Speech, and Signal Processing\fR,
vol. 39, pp. 1122-1135, Aug. 1991.
.ip [6]
B. Friedlander and A. J. Weiss,
``Direction Finding Using Spatial Smoothing With Interpolated Arrays,''
\fIIEEE Trans. on Aerospace and Electronic Systems\fR,
vol. 28, no. 2, pp. 574-587, April 1992.
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

    // check for empty input
    if(inputPkt.empty()) {
      FloatMatrix& V = *(new FloatMatrix(int(numRows),int(numCols)));
      V = 0.0;
      for(int row = 0; row < nrows; row++)
        for(int col = 0; col < numNoise; col++)
          (*Vn)[row][col] = V[row][col+(2*int(numSignals))];
      delete &V;
    }
    else {
      const FloatMatrix& V = *(const FloatMatrix *)inputPkt.myData();
      for(int row = 0; row < nrows; row++)
        for(int col = 0; col < numNoise; col++)
          (*Vn)[row][col] = V[row][col+(2*int(numSignals))];
    }
      
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
