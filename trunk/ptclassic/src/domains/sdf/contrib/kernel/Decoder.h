// $Id$
// Programmer: Neal Becker
// This may look like C code, but it is really -*- C++ -*-

#ifndef Decoder_h
#define Decoder_h

inline double sqr( double x ) {
  return x * x;
}

inline int Bit( int x, int b ) {
  return ( x >> b ) & 1 ;
}

class Decoder {

private:

  double Gain;

  // Xsymbol gives the xmit symbols for each branch
  // into each state.  The first index is the branch
  // and the second is the state.
  int Xsymbol[ 2 ][ 64 ];

  // Predecessor state table
  int PrevState[ 2 ][ 64 ];

  double BM (int xsym, int I, int Q);

  // This is the path memory
  int Path[ 64 ][ 64 ];

  // This index is the current write position in the path memory
  int PathIndex;

  // This is the state metric memory.  We ping-pong between 2
  double StateMetric[ 2 ][ 64 ];

  // This index is the current write position for the state metrics
  int StateMetricIndex;

  // That's right folks.  *Squared* distance.  RTFM.
  double sqdist( double xmit, double rcv ) {
    return sqr( xmit - rcv );
  }

  double XmitMap( int val ) {
    return ( val == 0 ) ? 1 : -1;
  }

  int G1( int Input, int State );

  int G2( int Input, int State );

  void ComputeXsymbol();

  void InitStateMetrics();
  
  void InitPaths();
  
  void DoACS( int g1, int g2 );

  int TraceBack();

  void Normalize();

public:

  Decoder (double g) : Gain (g) { Reset(); }

  int operator() ( int I, int Q );

  void SetGain (double g) { Gain = g; }

  void Reset();
  
};

#endif
