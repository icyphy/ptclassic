static const char file_id[] = "Interp.cc";

/*
Version identification:
$Id$

Programmer:     Neal Becker
 */

#include "Interp.h"

int Interp::Threshold( double x ) {
  for( int i = 0; i < Size; i++ ) {
    if( x < X[ i ] )
      return i;
  }
// should't happen
  return Size - 1;
}

double Interp::Lin( double x, int below, int above ) {
  double delta = ( x - X[ below ] )/( X[ above ] - X[ below ] );
  return Y[ above ] * delta + Y[ below ] * ( 1 - delta );
}

double Interp::operator [] ( double x ) {
  int below, above;
  if( x <= X[ 0 ] ) {
    below = 0;
    above = 1;
  }
  else if ( x >= X[ Size - 1 ] ) {
    below = Size - 2;
    above = Size - 1;
  }
  else {
    above = Threshold( x );
    below = above - 1;
  }
  return Lin( x, below, above );
}
