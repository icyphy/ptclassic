defstar {
  name { TWTA }
  domain { SDF }
  desc { Model of a TWTA amplifier }
  version { $Id$ }
  location { SDF user contribution library }
  author { N. Becker }
  explanation {
A TWTA amplifier is modeled by its transfer characteristic.
Data points for output backoff (dB) vs. input backoff (dB) and
phase (degree) vs. input backoff are supplied in three arrays.
Then for each complex input sample the input backoff is computed,
and linear interpolation/extrapolation is used to find the phase and 
output amplitude, which is converted back to a complex output sample.
}
  copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  state {
    name { InputBackoff }
    type { floatArray }
    desc { Input Backoff sample values in dB (for phase and amplitude values) }
  }
  state {
    name { Phase }
    type { floatArray }
    desc { Phase (degree) vs. Input Backoff (dB) }
  }
  state {
    name { Amplitude }
    type { floatArray }
    desc { Amplitude (dB) vs. Input Backoff (dB) }
  }
  state {
    name { Backoff }
    type { float }
    default { 0 }
    desc { Input Backoff (dB) }
  }
  input {
    name { in }
    type { complex }
  }
  output {
    name { out }
    type { complex }
  }
  private {
    Interp *PhaseInterp;
    Interp *AmplInterp;
  }
  constructor {
    PhaseInterp = 0;
    AmplInterp = 0;
  }
  destructor {
    delete PhaseInterp;
    delete AmplInterp;
  }
  hinclude { "Interp.h" }
  ccinclude { <math.h> }
  setup {
    if( Phase.size() != InputBackoff.size() ||
        Amplitude.size() != InputBackoff.size() ) {
      Error::abortRun( *this, "Phase, InputBackoff, and Amplitude arrays must be the same size" );
    }
    if( InputBackoff.size() < 2 ) {
      Error::abortRun( *this, "Need at least 2 data points" );
    }
    delete AmplInterp;
    delete PhaseInterp;
    AmplInterp = new Interp( (double *)InputBackoff,
			     (double *)Amplitude, InputBackoff.size() );
    PhaseInterp = new Interp( (double *)InputBackoff,
			      (double *)Phase, InputBackoff.size() );
  }
  header {
#define DEGREES_TO_RADIANS 0.0174532925199432957692
  }
  go {
    Complex z = Complex(in%0);
    double bo = norm(z);
    double oldphase = arg(z);
    double bodB = 10. * log10(bo) - double(Backoff);
    double ampdB = (*AmplInterp)[ bodB ];
    // Note the square root to convert power to volts
    double voltage = power( 10., 0.05 * ampdB );
    double Iphase = (*PhaseInterp)[ bodB ] * DEGREES_TO_RADIANS;
    double phase = oldphase + Iphase;
    Complex v = Complex( cos( phase ) * voltage, sin( phase ) * voltage );
    out%0 << v;
  }
}
