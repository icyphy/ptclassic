defcore {
    name { PolarToRect }
    domain { ACS }
    coreCategory { FixSim }
    corona { PolarToRect } 
    desc { Convert magnitude and phase to rectangular form. }
    version { @(#)ACSPolarToRectFixSim.pl	1.4 09/08/99}
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
	        type {int }
	        default { "YES" }
	        desc {
Indicates whether or not to keep the precision of the arriving particles
as they are:  YES keeps the same precision, and NO casts the inputs
to the precision specified by the parameter "InputPrecision".
		}
	}
	defstate {
	        name { InputPrecision }
	        type { precision }
	        default { 2.14 }
	        desc {
Sets the precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
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
                out.set_ovflow( ((const char *) OverflowHandler) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		out.set_rounding( int(RoundFix) );
        }
	ccinclude { <math.h> }
	go {
		Fix m = corona.magnitude%0;
		Fix p = corona.phase%0;
		if ( int(ArrivingPrecision) ) {
			out =  Fix(double(m) * cos(double(p)));
			checkOverflow(out);
			corona.x%0 << double(out);
			out = Fix(double(m) * sin(double(p)));
			checkOverflow(out);
			corona.y%0 << double(out);
		} else {
			fixIn = p;
			double tmp = cos(double(fixIn));
			fixIn = m;
			out = Fix(double(m) * tmp);
			checkOverflow(out);
			corona.x%0 << double(out);
			fixIn = p;
			tmp = sin(double(fixIn));
			fixIn = m;
			out = Fix(double(m) * tmp);
			checkOverflow(out);
			corona.y%0 << double(out);
		}
	}
}
