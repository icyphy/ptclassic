static const char file_id[] = "Decoder.cc";

/*
Viterbi Decoder

Version identification:
$Id$

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

float Decoder::BM( int xsym, int I, int Q ) {
  float NormI = float( I ) / float( Gain );
  float NormQ = float( Q ) / float( Gain );
  float XmitI = XmitMap (xsym & 1);
  float XmitQ = XmitMap ((xsym >> 1) & 1);
  float Dist = sqdist( XmitI, NormI ) + sqdist( XmitQ, NormQ );
  return Dist;
}

/* Do ACS */
void Decoder::DoACS( int I, int Q ) {
  int state;
  float* NewState;
  float* OldState;
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
    float d0 = BM (Xsymbol [0][state], I, Q) + 
      OldState [PrevState [0][state]];
    float d1 = BM (Xsymbol [1][state], I, Q) +
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
  float* NewState;
  int state;

  if( StateMetricIndex == 0 ) {
    NewState = StateMetric[ 0 ];
    StateMetricIndex = 1;
  }
  else {
    NewState = StateMetric[ 1 ];
    StateMetricIndex = 0;
  }

  float MinMetric = NewState[0];
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
