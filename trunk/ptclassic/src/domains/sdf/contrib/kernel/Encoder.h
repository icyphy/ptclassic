// $Id$
// Programmer: Neal Becker
// This may look like C code, but it is really -*- C++ -*-

#ifndef Encoder_h
#define Encoder_h

class Encoder {

private:

  int State[ 6 ];

  void Shift( int Input );

  int G1( int Input );

  int G2( int Input );

public:

  Encoder() { Reset(); }
  
  void Reset();

  void operator() ( int Input, int Output[ 2 ] );

};

#endif
