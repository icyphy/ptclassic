// $Id$
// Programmer: Neal Becker
// This may look like C code, but it is really -*- C++ -*-

#ifndef Decoder_h
#define Decoder_h

inline float sqr( float x ) {
  return x * x;
}

inline int Bit( int x, int b ) {
  return ( x >> b ) & 1 ;
}

class Decoder {

private:

  float Gain;

  // Xsymbol gives the xmit symbols for each branch
  // into each state.  The first index is the branch
  // and the second is the state.
  int Xsymbol[ 2 ][ 64 ];

  // Predecessor state table
  int PrevState[ 2 ][ 64 ];

  float BM (int xsym, int I, int Q);

  // This is the path memory
  int Path[ 64 ][ 64 ];

  // This index is the current write position in the path memory
  int PathIndex;

  // This is the state metric memory.  We ping-pong between 2
  float StateMetric[ 2 ][ 64 ];

  // This index is the current write position for the state metrics
  int StateMetricIndex;

  // That's right folks.  *Squared* distance.  RTFM.
  float sqdist( float xmit, float rcv ) {
    return sqr( xmit - rcv );
  }

  float XmitMap( int val ) {
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

  Decoder (float g) : Gain (g) { Reset(); }

  int operator() ( int I, int Q );

  void SetGain (float g) { Gain = g; }

  void Reset();
  
};

#endif
