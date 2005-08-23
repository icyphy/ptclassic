defcore {
    name { Cos }
    domain { ACS }
    coreCategory { FixSim }
    corona { Cos } 
    desc { This star computes the cosine of its input, assumed to be an angle in radians. }
    version { @(#)ACSCosFixSim.pl	1.4 09/08/99}
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
	    name { LockOutput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified output precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
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
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
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
                out.set_ovflow( ((const char *) OverflowHandler) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		out.set_rounding( int(RoundFix) );
        }
     ccinclude { <math.h> }
	go {
		Fix tmp = corona.input%0;
		if ( int(ArrivingPrecision) ) {
			out = Fix(cos (double(tmp)));
		} else {
			fixIn = tmp;
			out = Fix(cos (double(fixIn)));
		}
		checkOverflow(out);
		corona.output%0 << out;
	}
}
