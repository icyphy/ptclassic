defcore {
	name { NL }
	domain { ACS }
	coreCategory { FixSim }
	corona { NL }
	desc { nonlinear function providing lower threshold, variable gain linear region, and upper threshold. }
	version { @(#)ACSNLFixSim.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }

	defstate {
		name { fthreshold1 }
		type { Fix }
		default { 0.0 }
		desc { lower threshold below which output is zero. }
	}
	defstate {
		name { fgain }
		type { Fix }
		default { 0.5 }
		desc { gain of linear region. }
	}
	defstate {
		name { fthreshold2 }
		type { Fix }
		default { 15.0 }
		desc { upper threshold above which output saturates. }
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
		Fix tmp = corona.input%0;
		if ( tmp < Fix(fthreshold1) )
			tmp = Fix(0.0);
		else if ( int(ArrivingPrecision) ) {
			out = tmp * Fix(fgain);
		} else {
			fixIn = tmp;
			out = fixIn * Fix(fgain);
		}
		if ( out > Fix(fthreshold2) )
			out = Fix(fthreshold2);
		checkOverflow(out);
		corona.input%0 << out;
	}
}
