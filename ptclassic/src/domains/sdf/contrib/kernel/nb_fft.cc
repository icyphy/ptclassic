static const char file_id[] = "nb_fft.cc";

#include <ComplexSubset.h>
#include "nb_fft.h"

const NEXTMX = 12;

const int prime[ NEXTMX ] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };

				// AIX-4.1 under gcc-2.7.2 can't
  				// handle having the definition of
  				// getW in the .h file, so we move it here.
Complex nb_fft::getW( int m, int n ) {
    double angle = ( Pi2 * m ) / n;
    return Complex( cos( angle ), sin( angle ) );
}

Complex* nb_fft::Compute( Complex* z1, Complex* z2 ) {
  int before = N;
  int after = 1;
  int next = 0;
  int inzee = 1;
  int now;

  do {
    int np = prime[ next ];
    if( ( before/np ) * np < before ) {
      if( ++next < NEXTMX ) continue;
      now = before;
      before = 1;
    }
    else {
      now = np;
      before /= np;
    }
    if( inzee == 1 )
      fftstp( z1, after, now, before, z2 );
    else
      fftstp( z2, after, now, before, z1 );
    inzee = 3 - inzee;
    after *= now;
  } while( 1 < before );

  return ( inzee == 1 ) ? z1 : z2 ;
}

void nb_fft::fftstp( Complex* zin, int after, int now, int before, Complex* zout ) {
  double angle = ( Inverse ? -Pi2 : Pi2 ) / ( now * after );
  Complex omega = Complex( cos( angle ), -sin( angle ) );
  Complex arg = 1;
  for( int j = 0; j < now; j++ ) {
    for( int ia = 0; ia < after; ia++ ) {
      for( int ib = 0; ib < before; ib++ ) {
	Complex value = zin[ ia + ib * after + ( now - 1 ) * before * after ];

	for( int in = now - 2; 0 <= in; in-- ) {
	  value *= arg;
	  value += zin[ ia + ib * after + in * before * after ];
	}

	zout[ ia + j * after + ib * now * after ] = value;
      }
      arg *= omega;
    }
  }
}
