defcore {
	name { FIR }
	domain { ACS }
	coreCategory { FixSim }
	corona { FIR }
	desc {
A finite impulse response (FIR) filter.  Coefficients are specified by
the "taps" parameter.  The default coefficients give an 8th-order, linear
phase lowpass filter. To read coefficients from a file, replace the default
coefficients with "&lt; fileName", preferably specifying a complete path.
Rational sampling rate changes, implemented by polyphase multirate filters,
is also supported.
	}
	version { @(#)ACSFIRFixSim.pl	1.2 09/08/99 }
	author { James Lundblad }
    	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Flag indicating whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the precision specified
by the parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 4.14 }
                desc {
Precision of the input in bits.
The input particles are only cast to this precision if the parameter
"ArrivingPrecision" is set to NO.
		}
        }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
        defstate {
                name { TapPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the taps in bits. }
        }
        defstate {
                name { AccumulationPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the accumulation in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the output in bits. } 
        }
	defstate {
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
	protected {
		Fix Accum, fixIn, out, tap;
                int phaseLength;
	}
	setup {
		ACSFixSimCore::setup();

                int d = corona.decimation;
                int i = corona.interpolation;
                int dP = corona.decimationPhase;
                corona.input.setSDFParams(d, d+1+(corona.taps.size()/i));
                corona.output.setSDFParams(i, i-1);
                if (dP >= d) {
                  Error::abortRun (*this, ": decimationPhase too large");
                  return;
                }
                // The phaseLength is ceiling(corona.taps.size/interpolation)
                // It is a protected instance variable.
                phaseLength = corona.taps.size() / i;
                if ((corona.taps.size() % i) != 0) phaseLength++;

		// Set the precision on the fixed-point variables
		fixIn = Fix( ((const char *) InputPrecision) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid InputPrecision" );

		tap = Fix( ((const char *) TapPrecision) );
		if ( tap.invalid() )
		  Error::abortRun( *this, "Invalid TapPrecision" );

		Accum = Fix( ((const char *) AccumulationPrecision) );
		if ( Accum.invalid() )
		  Error::abortRun( *this, "Invalid AccumulationPrecision" );

		out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );

		// Set the overflow handlers for assignments/computations
		fixIn.set_ovflow( ((const char *) OverflowHandler) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		tap.set_ovflow( ((const char *) OverflowHandler) );
		Accum.set_ovflow( ((const char *) OverflowHandler) );
		out.set_ovflow( ((const char *) OverflowHandler) );

		// Set all fixed-point assignments/computations to use rounding
		fixIn.set_rounding( int(RoundFix) );
		tap.set_rounding( int(RoundFix) );
		Accum.set_rounding( int(RoundFix) );
		out.set_rounding( int(RoundFix) );
	}
        go {
            int phase, tapsIndex;
            int Interp = corona.interpolation;
            int Decim = corona.decimation;
            int outCount = Interp - 1;

            // phase keeps track of which phase of filter coefficients are used
            // Starting phase depends on the decimationPhase state.
            phase = Decim - int(corona.decimationPhase) - 1;

            // Interpolate once for each input consumed
            for (int inC = 1; inC <= Decim; inC++) {
                // Produce however many outputs are required
		// for each input consumed
                while (phase < Interp) {
                   Accum = 0.0;
                   // Compute the inner product.
                   for ( int i = 0; i < phaseLength; i++ ) {
			// We use a temporary variable to avoid gcc2.7.2/2.8 problems
			Fix tmpA = (corona.input%(Decim - inC + i)); 
                        tapsIndex = i * Interp + phase;
                        if (tapsIndex >= corona.taps.size())
                            tap = 0.0;
                        else 
                            tap = corona.taps[tapsIndex];
        
                        if ( int(ArrivingPrecision) )
                            Accum += tap * tmpA;
                        else {
                            fixIn = tmpA;
                            Accum += tap * fixIn;
			}
                   }
                   out = Accum;
                   // note: output%0 is the last output chronologically
                   corona.output%(outCount--) << out;
                   phase += Decim;
                }
                phase -= Interp;
            }
        }
}
