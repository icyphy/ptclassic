defstar {
        name { SubFix }
        domain { SDF }
	derivedFrom { SDFFix }
        desc {
Output as a fixed-point number the "pos" input minus all "neg" inputs.
	}
        author { A. Khazeni }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
        location { SDF main library }
        input {
                name { pos }
                type { fix }
        }
        inmulti {
                name { neg }
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
Flag indicating whether or not to use the precision of the arriving particles
(the "pos" input and the "neg" inputs) as they are:  YES processes them
unaltered, and NO casts them to the precision specified by the
parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the "neg" input in bits.
The input particles are only cast to this precision if the
parameter "ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the output in bits.
This is the precision that will hold the result of the difference
of the inputs.
When the value of the accumulation extends outside of the precision,
the OverflowHandler will be called.
		}
        }
        protected {
		Fix fixIn, diff;
        }
        setup {
		SDFFix::setup();

                if ( ! int(ArrivingPrecision) ) {
                  fixIn = Fix( ((const char *) InputPrecision) );
		  if ( fixIn.invalid() )
		    Error::abortRun( *this, "Invalid InputPrecision" );
		}
		fixIn.set_rounding( ((int) RoundFix) );

                diff = Fix( ((const char *) OutputPrecision) );
		if ( diff.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );
                diff.set_ovflow( ((const char *) OverflowHandler) );
		if ( diff.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		diff.set_rounding( ((int) RoundFix) );
        }
        go {
                MPHIter nexti(neg);
                PortHole *p;

                if ( int(ArrivingPrecision) ) {
		  diff = Fix(pos%0);
                  while ((p = nexti++) != 0) {
                    diff -= (Fix)((*p)%0);
		    checkOverflow(diff);
		  }
		}
		else {
		  fixIn = Fix(pos%0);
		  diff = fixIn;
                  while ((p = nexti++) != 0) {
		    fixIn = (Fix)((*p)%0);
                    diff -= fixIn;
		    checkOverflow(diff);
		  }
		}
                output%0 << diff;
        }
        // a wrap-up method is inherited from SDFFix
        // if you defined your own, you should call SDFFix::wrapup()
}
