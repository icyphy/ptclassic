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

				// AIX-4.1 under gcc-2.7.2 can't
  				// handle having the definition of getW
  				// in this file.
  Complex getW( int m, int n );

};

#endif
