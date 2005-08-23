defcore {
    name { Magnitude }
    domain { ACS }
    coreCategory { FixSim }
    corona { Magnitude } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSMagnitudeFixSim.pl	1.0 04/18/00}
    author { James Lundblad, Ken Smith}
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
Flag that indicates whether or not to keep the precision of the arriving
particles as is:  YES keeps the same precision, and NO casts them to the
precision specified by the parameter "InputPrecision". } 
        }
      defstate {
                name { InputPrecision }
                type { precision }
                default { 2.14 }
                desc { 
Precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
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
        protected {
		Fix fixIn, sum;
        }
        setup {
		ACSFixSimCore::setup();

		  // FIX: method is from Adder
		  
                if ( ! int(ArrivingPrecision) ) {
		  fixIn = Fix( ((const char *) InputPrecision) );
		  if ( fixIn.invalid() )
		    Error::abortRun( *this, "Invalid InputPrecision" );
		}
		fixIn.set_rounding( int(RoundFix) );

		sum = Fix( ((const char *) OutputPrecision) );
		if ( sum.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
		sum.set_ovflow( ((const char *) OverflowHandler) );
		if ( sum.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		sum.set_rounding( int(RoundFix) );        }

	go {
		  // FIX: method is from Adder
	}
}
