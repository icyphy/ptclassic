/*
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
// @(#)Decoder.h	1.4	3/7/96
// Programmer: Neal Becker
// This may look like C code, but it is really -*- C++ -*-

#ifndef Decoder_h
#define Decoder_h

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
    return ( (xmit - rcv) * (xmit - rcv) );
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
