defcore {
	name { Sub }
	domain { ACS }
	coreCategory { FixSim }
	corona { Sub }
	desc { Output as a fixed-point number the "pos" input minus all "neg" inputs. }
	version { @(#)ACSSubFixSim.pl	1.2 09/08/99 }
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
Flag indicating whether or not to use the precision of the arriving particles
(the "pos" input and the "neg" inputs) as they are:  YES processes them
unaltered, and NO casts them to the precision specified by the
parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the "neg" input in bits.
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
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the output in bits.
This is the precision that will hold the result of the difference
of the inputs.
When the value of the accumulation extends outside of the precision,
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
        protected {
		Fix fixIn, diff;
        }
       	setup {
		ACSFixSimCore::setup();

                if ( ! int(ArrivingPrecision) ) {
                  fixIn = Fix( ((const char *) InputPrecision) );
		  if ( fixIn.invalid() )
		    Error::abortRun( *this, "Invalid InputPrecision" );
		}
		fixIn.set_rounding( int(RoundFix) );

                diff = Fix( ((const char *) OutputPrecision) );
		if ( diff.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
                diff.set_ovflow( ((const char *) OverflowHandler) );
		if ( diff.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		diff.set_rounding( int(RoundFix) );
        }
    	go {
	  if ( int(ArrivingPrecision) ) {
	    diff = (const Fix&)(corona.pos%0);
	    // We use temporary variables to avoid
	    // gcc2.7.2/2.8 problems
	    Fix tmp = corona.neg%0;	
	    diff -= tmp;
	    checkOverflow(diff);
	  }
	  else {
	    Fix tmp = corona.pos%0;
	    fixIn = tmp;
	    diff = tmp;
	    // We use temporary variables to avoid
	    // gcc2.7.2/2.8 problems
	    Fix tmp1 = corona.neg%0;
	    fixIn = tmp1;
	    diff -= fixIn;
	    checkOverflow(diff);
	  }
	  corona.output%0 << diff;
        }
}
