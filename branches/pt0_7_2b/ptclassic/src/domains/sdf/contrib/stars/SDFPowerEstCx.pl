defstar {
  name { PowerEstCx }
  domain { SDF }
  desc { Power Estimator }
  location { SDF user contribution library }
  author { N. Becker }
	htmldoc {
Power is estimated by computing magnitude squared and then filtering with a
simple 1-pole filter.
  }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  state {
    name { TimeConstant }
    type { float }
    desc { Time constant in samples }
    default { 100 }
  }
  state {
    name { dB }
    type { int }
    desc { convert to dB if True }
    default { "YES" }
  }
  state {
    name { BlockSize }
    type { int }
    desc { Read a block of this size at a time }
    default { 1 }
  }
  input {
    name { in }
    type { complex }
  }
  output {
    name { out }
    type { float }
  }
  private {
    double Sum;
    double FeedbackGain;
  }
  setup {
    Sum = 0;
    FeedbackGain = 1. - 1./double( TimeConstant );
    in.setSDFParams( int( BlockSize ), int( BlockSize ) - 1 );
  }
  ccinclude { <math.h> }
  code {
    inline double TodB( double x ) {
      return 10. * log10( x );
    }
  }
  go {
    for( int i = int(BlockSize) - 1; i >= 0; i-- ) {
      // We use a temporary variable to avoid gcc2.7.2/2.8 problems
      Complex tmp = in%i;
      Sum = Sum * FeedbackGain + norm(tmp);
    }
    double output = Sum / double( TimeConstant );
    out%0 << ( int( dB ) ? TodB( output ) : output );
  }
}
