// This may look like C code, but it is really -*- C++ -*-

#ifndef fft_h
#define fft_h

/* #include <Complex.h> */
#include "Constants.h"

class fft {

protected:

  int N;

  int Inverse;

private:

  void fftstp( Complex* zin, int after, int now, int before, Complex* zout );

public:

  fft( int _Inverse, int size ) : Inverse( _Inverse ), N( size ) {}

  Complex* Compute( Complex* z1, Complex* z2 );

  Complex getW( int m, int N ) {
    double angle = ( Pi2 * m ) / N;
    return Complex( cos( angle ), sin( angle ) );
  }

};

#endif
