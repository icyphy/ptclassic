defstar {
	name { AddFix }	
	domain {SDF}
	derivedFrom{ SDFFix }
	desc {
Output the sum of the fixed-point inputs as a fixed-point value.
	}
	version { $Id$ }
        author { A. Khazeni }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        location { SDF main library }
	inmulti {
		name { input }
		type { fix }
	}
	output {
		name { output }
		type { fix }
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
                type { string }
                default { "2.14" }
                desc { 
Precision of the input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the output in bits and precision of the accumulation.
When the value of the accumulation extends outside of the precision,
the OverflowHandler will be called.
                }
        }
        protected {
		Fix fixIn, sum;
        }
        setup {
		SDFFix::setup();

                if ( ! int(ArrivingPrecision) ) {
		  fixIn = Fix( ((const char *) InputPrecision) );
		  if ( fixIn.invalid() )
		    Error::abortRun( *this, "Invalid InputPrecision" );
		}
		fixIn.set_rounding( ((int) RoundFix) );

		sum = Fix( ((const char *) OutputPrecision) );
		if ( sum.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
		sum.set_ovflow( ((const char *) OverflowHandler) );
		if ( sum.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		sum.set_rounding( ((int) RoundFix) );
        }
        go {
                MPHIter nexti(input);
                PortHole *p;

		// Fixed-point class in Ptolemy 0.5 can always represent 0.0
		sum.setToZero();	// set to zero and clear error bits
                while ((p = nexti++) != 0) {
                  if ( int(ArrivingPrecision) )
                    sum += (Fix)((*p)%0);
		  else {
                    fixIn = (Fix)((*p)%0);
                    sum += fixIn;
		  }
		  checkOverflow(sum);
		}
                output%0 << sum;
        }
	// a wrap-up method is inherited from SDFFix
	// if you defined your own, you should call SDFFix::wrapup()
}
