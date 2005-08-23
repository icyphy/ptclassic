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
static const char file_id[] = "Decoder.cc";

/*
Viterbi Decoder

Version identification:
@(#)Decoder.cc	1.5	3/7/96

Programmer:	Neal Becker
 */

#include "Decoder.h"

int Decoder::G1( int Input, int State ) {
  return Input ^ Bit( State, 1 ) ^ Bit( State, 2 ) ^ Bit( State, 4 ) ^ Bit( State, 5 );
}

int Decoder::G2( int Input, int State ) {
  return Input ^ Bit( State, 0 ) ^ Bit( State, 1 ) ^ Bit( State, 2 ) ^ Bit( State, 5 );
}

void Decoder::ComputeXsymbol() {
  int state;
  int input;
  int NextState;
  int HighBit;			/* MSB of predecessor state */
				/* used to distinguish paths */

  for( state = 0; state < 64; state++ ) {
    for( input = 0; input < 2; input++ ) {
      NextState = ( (state << 1) & 0x3f ) | input;
      HighBit = ( state & 0x20 ) ? 1 : 0 ;
      Xsymbol[ HighBit ][ NextState ] = G1( input, state ) + ( G2( input, state ) << 1 );
      PrevState[ HighBit ][ NextState ] = state;
    }
  }
}

void Decoder::InitStateMetrics() {
  for( int flip = 0; flip < 2; flip++ )
    for( int s = 0; s < 64; s++ )
      StateMetric[ flip ][ s ] = 0;
}

void Decoder::InitPaths() {
  for( int i = 0; i < 64; i++ )
    for( int j = 0; j < 64; j++ )
      Path[ i ][ j ] = 0;
}

void Decoder::Reset() {
  ComputeXsymbol();
  InitStateMetrics();
  InitPaths();
  PathIndex = 0;
  StateMetricIndex = 0;
}

double Decoder::BM( int xsym, int I, int Q ) {
  double NormI = double( I ) / double( Gain );
  double NormQ = double( Q ) / double( Gain );
  double XmitI = XmitMap (xsym & 1);
  double XmitQ = XmitMap ((xsym >> 1) & 1);
  double Dist = sqdist( XmitI, NormI ) + sqdist( XmitQ, NormQ );
  return Dist;
}

/* Do ACS */
void Decoder::DoACS( int I, int Q ) {
  int state;
  double* NewState;
  double* OldState;
  int* PathPt;

  PathPt = Path[ PathIndex ];

  if( StateMetricIndex == 0 ) {
    NewState = StateMetric[ 0 ];
    OldState = StateMetric[ 1 ];
  }
  else {
    NewState = StateMetric[ 1 ];
    OldState = StateMetric[ 0 ];
  }


  for( state = 0; state < 64; state++ ) {
    double d0 = BM (Xsymbol [0][state], I, Q) + 
      OldState [PrevState [0][state]];
    double d1 = BM (Xsymbol [1][state], I, Q) +
      OldState [PrevState [1][state]];
    if( d0 < d1 ) {
      NewState[ state ] = d0;
      PathPt[ state ] = PrevState[ 0 ][ state ];
    }
    else {
      NewState[ state ] = d1;
      PathPt[ state ] = PrevState[ 1 ][ state ];
    }
  }
}

/* Do traceback.  Also update PathIndex */
int Decoder::TraceBack() {
  int pi = PathIndex;
  int step;
  int state = 0;

  PathIndex = ( PathIndex + 1 ) % 64;

  for( step = 0; step < 64; step++ ) {
    pi &= 0x3f;			/* ring buffer size 64 */
    state = Path[ pi-- ][ state ]; 
  }
  return state & 1;		/* LSB of state is info bit */
}

/* Find the minimum state metric.  Subtract this from all */
/* the metrics.  This is *not* the most efficient way to */
/* normalize, it's the most basic. */
/* StateMetricIndex is flipped here also. */
void Decoder::Normalize() {
  double* NewState;
  int state;

  if( StateMetricIndex == 0 ) {
    NewState = StateMetric[ 0 ];
    StateMetricIndex = 1;
  }
  else {
    NewState = StateMetric[ 1 ];
    StateMetricIndex = 0;
  }

  double MinMetric = NewState[0];
  for( state = 1; state < 64; state++ ) {
    MinMetric = ( NewState[ state ] < MinMetric ) ?
		NewState[ state ] : MinMetric;
  }

  for( state = 0; state < 64; state++ )
    NewState[ state ] -= MinMetric;
}

int Decoder::operator() ( int I, int Q ) {
  DoACS( I, Q );
  int out = TraceBack();
  Normalize();
  return out;
}
