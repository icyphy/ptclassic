// This may look like C code, but it is really -*- C++ -*-

#ifndef nb_fft_h
#define nb_fft_h

/* #include <Complex.h> */
#include "Constants.h"

class nb_fft {

protected:

  int N;

  int Inverse;

private:

  void fftstp( Complex* zin, int after, int now, int before, Complex* zout );

public:

  nb_fft( int inverse, int size ) : N( size ), Inverse( inverse ) {}

  Complex* Compute( Complex* z1, Complex* z2 );

  Complex getW( int m, int n ) {
    double angle = ( Pi2 * m ) / n;
    return Complex( cos( angle ), sin( angle ) );
  }

};

#endif
