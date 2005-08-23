defcore {
    name { Gain }
    domain { ACS }
    coreCategory { FixSim }
    corona { Gain } 
    desc { This is an amplifier; the output is the input multiplied by the "gain" (default 1.0). }
    version { @(#)ACSGainFixSim.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

    defstate {
      name { OutputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the output in bits.
This is the precision that will hold the result of the arithmetic operation
on the inputs.
When the value of the product extends outside of the precision,
the OverflowHandler will be called.
		}
    }
    defstate {
      name { ArrivingPrecision }
      type {int}
      default {"YES"}
      desc {
Flag indicated whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". }
    }
    defstate {
      name { InputPrecision }
      type { precision }
      default { 2.14 }
      desc {
Precision of the input in bits.  The input particles are only cast
to this precision if the parameter "ArrivingPrecision" is set to NO.}
    }
    protected {
      Fix fixIn, out;
    }
    setup {
      ACSFixSimCore::setup();
      
      if ( ! int(ArrivingPrecision) ) {
	fixIn = Fix( ((const char *) InputPrecision) );
	if ( fixIn.invalid() )
	  Error::abortRun( *this, "Invalid InputPrecision" );
      }
      fixIn.set_rounding( int(RoundFix) );
      
      
      out = Fix( ((const char *) OutputPrecision) );
      if ( out.invalid() )
	Error::abortRun( *this, "Invalid OutputPrecision" );
      out.set_ovflow( ((const char *) "saturate") );
      if ( out.invalid() )
	Error::abortRun( *this, "Invalid OverflowHandler" );
      out.set_rounding( int(TRUE) );
    }
    go {
      // We use a temporary variable to avoid gcc2.7.2/2.8 problems
      Fix tmpgain = corona.input%0;
      // all computations should be performed with out since that
      // is the Fix variable with the desired overflow handler
      out = Fix(corona.gain);
      if ( int(ArrivingPrecision) ) {
	out *= tmpgain;
      }
      else {
	fixIn = tmpgain;
	out *= fixIn;
      }
      checkOverflow(out);
      corona.output%0 << out;
    }
}
