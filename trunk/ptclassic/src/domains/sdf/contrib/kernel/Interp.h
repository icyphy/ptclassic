// $Id$
// Programmer: Neal Becker
// This may look like C code, but it is really -*- C++ -*-

#ifndef Interp_h
#define Interp_h

class Interp {

protected:

  const double *X;
  const double *Y;
  int Size;

  int Threshold( double x );
  double Lin( double x, int below, int above );

public:

  Interp( double *_X, double *_Y, int _Size ) : X( _X ), Y( _Y ), 
  Size( _Size ) {}

  double operator [] ( double x );

};

#endif
