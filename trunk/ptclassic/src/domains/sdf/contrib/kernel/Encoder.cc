static const char file_id[] = "Encoder.cc";

/*
Viterbi Encoder

Version identification:
$Id$

Programmer:     Neal Becker
 */

#include "Encoder.h"

void Encoder::Reset() {
  for( int i = 0; i < 6; i++ )
    State[ i ] = 0;
}

void Encoder::Shift( int Input ) {
  State[ 5 ] = State[ 4 ];
  State[ 4 ] = State[ 3 ];
  State[ 3 ] = State[ 2 ];
  State[ 2 ] = State[ 1 ];
  State[ 1 ] = State[ 0 ];
  State[ 0 ] = Input;
}

int Encoder::G1( int Input ) {
  return Input ^ State[ 1 ] ^ State[ 2 ] ^ State[ 4 ] ^ State[ 5 ];
}

int Encoder::G2( int Input ) {
  return Input ^ State[ 0 ] ^ State[ 1 ] ^ State[ 2 ] ^ State[ 5 ];
}

void Encoder::operator() ( int Input, int Output[ 2 ] ) {
  Output[ 0 ] = G1( Input );
  Output[ 1 ] = G2( Input );
  Shift( Input );
}

